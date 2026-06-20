//---------------------------------------------------------------------------
//
// 파일명 : FlowMgr.cpp
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "FlowMgr.h"
#include "FlowFactory.h"
#include "FlowCmdQueue.h"
#include "FlowStack.h"
#include "Flow.h"

//---------------------------------------------------------------------------
namespace Flow
{
	CFlowMgr::CFlowMgr()
		: m_pkFlowCmdQueue(NULL)
		, m_pkFlowStack(NULL)
		, m_pkFlowInstMgr(NULL)
		, m_bFlowLock(FALSE)
		, m_bProcessingLock(FALSE)
	{
	}
	//---------------------------------------------------------------------------
	CFlowMgr::~CFlowMgr()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CFlowMgr::Create()
	{
		if (!m_pkFlowCmdQueue)
			m_pkFlowCmdQueue = NiNew CFlowCmdQueue;

		if (!m_pkFlowStack)
			m_pkFlowStack = NiNew CFlowStack;

		if (!m_pkFlowInstMgr)
			m_pkFlowInstMgr = NiNew CInstMgr<int, CFlow*>;

		if (!m_pkFlowCmdQueue || !m_pkFlowStack || !m_pkFlowInstMgr)
		{
			SAFE_NIDELETE(m_pkFlowCmdQueue);
			SAFE_NIDELETE(m_pkFlowStack);
			SAFE_NIDELETE(m_pkFlowInstMgr);
			return FALSE;
		}

		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::Destroy()
	{
		CurFlowImmediatelyDestroy();

		if (m_pkFlowCmdQueue)
			m_pkFlowCmdQueue->Clear();

		if (m_pkFlowStack)
			m_pkFlowStack->Clear();

		if (m_pkFlowInstMgr)
			m_pkFlowInstMgr->AllDelInst();

		SAFE_NIDELETE(m_pkFlowCmdQueue);
		SAFE_NIDELETE(m_pkFlowStack);
		SAFE_NIDELETE(m_pkFlowInstMgr);
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::CurFlowImmediatelyDestroy()
	{
		int iID = GetCurTopFlowID();

		while (iID != CFlow::FLW_NONE)
		{
			if (IsCurTopFlow())
				GetCurTopFlow()->OnExit(-1);

			EraseFlow(iID);

			iID = GetCurTopFlowID();
		}
	}
	//---------------------------------------------------------------------------
	BOOL CFlowMgr::StartFlow(int p_iID)
	{
		if (!m_pkFlowStack || !m_pkFlowCmdQueue)
			return FALSE;

		assert(m_pkFlowStack->IsEmpty() && "StartFlow Failed!");

		if (!m_pkFlowStack->IsEmpty())
			return FALSE;

		PushFlow(p_iID);

		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::PushFlow(int p_iID)
	{
		if (!m_pkFlowCmdQueue)
			return;

		if (!m_pkFlowCmdQueue->IsEmpty())
			return;

		int iSize = GetCurFlowSize();

		for (int i = 0; i < iSize; ++i)
		{
			if (IsCurPosFlow(i))
				GetCurPosFlow(i)->ReservedPushFlow(p_iID);
		}

		m_pkFlowCmdQueue->PushFlow(p_iID);
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::ChangePopAllFlow(int p_iID)
	{
		if (!m_pkFlowCmdQueue)
			return;

		if (!m_pkFlowCmdQueue->IsEmpty())
			return;

		int iSize = GetCurFlowSize();

		for (int i = 0; i < iSize; ++i)
		{
			if (IsCurPosFlow(i))
				GetCurPosFlow(i)->ReservedChangeFlow(p_iID);
		}

		m_pkFlowCmdQueue->ChangePopAllFlow(p_iID);
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::ChangeFlow(int p_iID)
	{
#ifdef DEBUG_NETWORK
		DBG("Changing flow\n");
#endif

		if (!m_pkFlowCmdQueue)
			return;

		if (!m_pkFlowCmdQueue->IsEmpty())
			return;

		if (m_pkFlowCmdQueue->IsFrontChange(p_iID))
			return;

		int iSize = GetCurFlowSize();

#ifdef DEBUG_NETWORK
		DBG("Flow size -> %d\n", iSize);
#endif

		for (int i = 0; i < iSize; ++i)
		{
			if (IsCurPosFlow(i))
				GetCurPosFlow(i)->ReservedChangeFlow(p_iID);
		}

		m_pkFlowCmdQueue->ChangeFlow(p_iID);
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::PopFlow(int p_iID)
	{
		if (!m_pkFlowCmdQueue)
			return;

		if (!m_pkFlowCmdQueue->IsEmpty())
			return;

		int iSize = GetCurFlowSize();

		for (int i = 0; i < iSize; ++i)
		{
			if (IsCurPosFlow(i))
				GetCurPosFlow(i)->ReservedPopFlow();
		}

		m_pkFlowCmdQueue->PopFlow(p_iID);
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::PopAllFlow()
	{
		if (!m_pkFlowCmdQueue)
			return;

		if (m_pkFlowCmdQueue->IsFrontPopAll())
			return;

		m_pkFlowCmdQueue->PopAllFlow();
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::CMDUpdate()
	{
		if (!m_pkFlowCmdQueue)
			return;

		while (!m_pkFlowCmdQueue->IsEmpty())
		{
			if (m_bFlowLock)
				return;

			m_bProcessingLock = TRUE;

			CFlowCmdQueue::FLOWCMD kCmd;
			ZeroMemory(&kCmd, sizeof(kCmd));
			kCmd = m_pkFlowCmdQueue->GetAt();

#ifdef DEBUG_NETWORK
			DBG("Flow changing CMD to -> %d\n", kCmd.eCmdType);
#endif

			switch (kCmd.eCmdType)
			{
			case CFlowCmdQueue::CMD_PUSH:
			{
				int iID = GetCurTopFlowID();

				if (iID != kCmd.iID)
				{
					if (IsCurTopFlow())
						GetCurTopFlow()->OnOverride(kCmd.iID);

					if (RegisterFlow(kCmd.iID))
					{
						if (IsCurTopFlow())
							GetCurTopFlow()->OnEnter();
					}
				}
			}
			break;

			case CFlowCmdQueue::CMD_CHANGE:
			{
				int iID = GetCurTopFlowID();

				if (iID != kCmd.iID)
				{
					if (IsCurTopFlow())
						GetCurTopFlow()->OnExit(kCmd.iID);

					EraseFlow(iID);

					if (RegisterFlow(kCmd.iID))
					{
						if (IsCurTopFlow())
							GetCurTopFlow()->OnEnter();
					}
				}
			}
			break;

			case CFlowCmdQueue::CMD_CHANGE_POP_ALL:
			{
				int iID = GetCurTopFlowID();

				while (iID != CFlow::FLW_NONE)
				{
					if (IsCurTopFlow())
						GetCurTopFlow()->OnExit(-1);

					EraseFlow(iID);

					iID = GetCurTopFlowID();
				}

				if (RegisterFlow(kCmd.iID))
				{
					if (IsCurTopFlow())
						GetCurTopFlow()->OnEnter();
				}
			}
			break;

			case CFlowCmdQueue::CMD_POP:
			{
				if (!m_pkFlowStack)
					break;

				if (!m_pkFlowStack->IsExistID(kCmd.iID))
					break;

				int iID = GetCurTopFlowID();

				while (iID != CFlow::FLW_NONE)
				{
					if (IsCurTopFlow())
						GetCurTopFlow()->OnExit(-1);

					EraseFlow(iID);

					if (IsCurTopFlow())
						GetCurTopFlow()->OnResume(kCmd.iID);

					if (iID == kCmd.iID)
						break;

					iID = GetCurTopFlowID();
				}
			}
			break;

			case CFlowCmdQueue::CMD_POP_ALL:
			{
				int iID = GetCurTopFlowID();

				while (iID != CFlow::FLW_NONE)
				{
					if (IsCurTopFlow())
						GetCurTopFlow()->OnExit(-1);

					EraseFlow(iID);

					iID = GetCurTopFlowID();
				}
			}
			break;
			}

			m_bProcessingLock = FALSE;
		}
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::OnIdle()
	{
		CMDUpdate();

		int iSize = GetCurFlowSize();

		for (int i = 0; i < iSize; ++i)
		{
			if (IsCurPosFlow(i))
			{
				if (!GetCurPosFlow(i)->IsPaused())
					GetCurPosFlow(i)->OnIdle();
			}
		}
	}
	//---------------------------------------------------------------------------
	BOOL CFlowMgr::OnMsgHandler(const MSG& p_kMsg)
	{
		BOOL bResult = FALSE;

		if (m_bProcessingLock)
			return bResult;

		if (m_bFlowLock)
			return bResult;

		if (IsCurTopFlow())
			bResult = GetCurTopFlow()->OnMsgHandler(p_kMsg);

		return bResult;
	}
	//---------------------------------------------------------------------------
	bool CFlowMgr::LostDevice(void* p_pvData)
	{
		int iSize = GetCurFlowSize();

		for (int i = 0; i < iSize; ++i)
		{
			if (GetCurPosFlow(i))
				GetCurPosFlow(i)->LostDevice(p_pvData);
		}

		return true;
	}
	//---------------------------------------------------------------------------
	bool CFlowMgr::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		int iSize = GetCurFlowSize();

		for (int i = 0; i < iSize; ++i)
		{
			if (GetCurPosFlow(i))
				GetCurPosFlow(i)->ResetDevice(p_bBeforeReset, p_pvData);
		}

		return true;
	}
	//---------------------------------------------------------------------------
	BOOL CFlowMgr::RegisterFlow(int p_iID)
	{
		if (!m_pkFlowStack || !m_pkFlowInstMgr)
			return FALSE;

		if (p_iID <= CFlow::FLW_NONE)
		{
#ifdef DEBUG_FLOW
			DBG("RegisterFlow failed: invalid flow id %d\n", p_iID);
#endif
			return FALSE;
		}

		if (m_pkFlowInstMgr->GetInst(p_iID))
		{
#ifdef DEBUG_FLOW
			DBG("RegisterFlow failed: flow already exists %d\n", p_iID);
#endif
			return FALSE;
		}

		CFlow* pkFlow = CFlowFactory::CreateFlow(p_iID);

		if (!pkFlow)
		{
#ifdef DEBUG_FLOW
			DBG("RegisterFlow failed: factory returned NULL. flow id %d\n", p_iID);
#endif
			return FALSE;
		}

		if (!m_pkFlowStack->Push(p_iID))
		{
#ifdef DEBUG_FLOW
			DBG("RegisterFlow failed: stack push failed. flow id %d\n", p_iID);
#endif
			SAFE_NIDELETE(pkFlow);
			return FALSE;
		}

		m_pkFlowInstMgr->AddInst(p_iID, pkFlow);

#ifdef DEBUG_FLOW
		DBG("RegisterFlow success. flow id %d\n", p_iID);
#endif

		return TRUE;
	}
	//---------------------------------------------------------------------------
	BOOL CFlowMgr::EraseFlow(int p_iID)
	{
		if (!m_pkFlowStack || !m_pkFlowInstMgr)
			return FALSE;

		if (p_iID == CFlow::FLW_NONE)
			return FALSE;

		int iID = m_pkFlowStack->Top();

		if (iID != p_iID)
		{
#ifdef DEBUG_FLOW
			DBG("EraseFlow failed: top flow mismatch. top %d, erase %d\n", iID, p_iID);
#endif
			return FALSE;
		}

		m_pkFlowStack->Pop();

#ifdef DEBUG_FLOW
		DBG("EraseFlow success. flow id %d\n", p_iID);
#endif

		m_pkFlowInstMgr->DelInst(p_iID);

		return TRUE;
	}
	//---------------------------------------------------------------------------
	int CFlowMgr::GetCurFlowSize() const
	{
		if (!m_pkFlowStack)
			return 0;

		return m_pkFlowStack->GetSize();
	}
	//---------------------------------------------------------------------------
	BOOL CFlowMgr::IsCurTopFlow() const
	{
		return GetCurTopFlow() != NULL ? TRUE : FALSE;
	}
	//---------------------------------------------------------------------------
	CFlow* CFlowMgr::GetCurTopFlow() const
	{
#ifdef DEBUG_FLOW
		DBG("GetCurTopFlow\n");
#endif

		if (!m_pkFlowStack || !m_pkFlowInstMgr)
			return NULL;

		int iID = m_pkFlowStack->GetTopAt();

		if (iID <= CFlow::FLW_NONE)
			return NULL;

		return m_pkFlowInstMgr->GetInst(iID);
	}
	//---------------------------------------------------------------------------
	int CFlowMgr::GetCurTopFlowID() const
	{
		if (!m_pkFlowStack)
			return CFlow::FLW_NONE;

		return m_pkFlowStack->GetTopAt();
	}
	//---------------------------------------------------------------------------
	BOOL CFlowMgr::IsCurPosFlow(int p_iPos) const
	{
#ifdef DEBUG_FLOW
		DBG("IsCurPosFlow\n");
#endif

		return GetCurPosFlow(p_iPos) != NULL ? TRUE : FALSE;
	}
	//---------------------------------------------------------------------------
	CFlow* CFlowMgr::GetCurPosFlow(int p_iPos) const
	{
		if (!m_pkFlowStack || !m_pkFlowInstMgr)
			return NULL;

		int iID = m_pkFlowStack->GetPosAt(p_iPos);

		if (iID <= CFlow::FLW_NONE)
			return NULL;

		return m_pkFlowInstMgr->GetInst(iID);
	}
	//---------------------------------------------------------------------------
	int CFlowMgr::GetCurPosFlowID(int p_iPos) const
	{
		if (!m_pkFlowStack)
			return CFlow::FLW_NONE;

		return m_pkFlowStack->GetPosAt(p_iPos);
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::LockFlow()
	{
		m_bFlowLock = TRUE;

		int iSize = GetCurFlowSize();

		for (int i = 0; i < iSize; ++i)
		{
			if (IsCurPosFlow(i))
				GetCurPosFlow(i)->SetInputLock();
		}
	}
	//---------------------------------------------------------------------------
	void CFlowMgr::UnLockFlow()
	{
		m_bFlowLock = FALSE;

		int iSize = GetCurFlowSize();

		for (int i = 0; i < iSize; ++i)
		{
			if (IsCurPosFlow(i))
				GetCurPosFlow(i)->SetInputUnLock();
		}
	}
	//---------------------------------------------------------------------------
	BOOL CFlowMgr::IsCurrentFlow(int p_iID) const
	{
		int iSize = GetCurFlowSize();

		for (int i = 0; i < iSize; ++i)
		{
			if (GetCurPosFlowID(i) == p_iID)
				return TRUE;
		}

		return FALSE;
	}
}
//---------------------------------------------------------------------------