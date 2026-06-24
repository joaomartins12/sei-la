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
		OutputDebugStringA("[FLOWMGR] Create begin\n");

		if (!m_pkFlowCmdQueue)
			m_pkFlowCmdQueue = NiNew CFlowCmdQueue;

		if (!m_pkFlowStack)
			m_pkFlowStack = NiNew CFlowStack;

		if (!m_pkFlowInstMgr)
			m_pkFlowInstMgr = NiNew CInstMgr<int, CFlow*>;

		if (!m_pkFlowCmdQueue || !m_pkFlowStack || !m_pkFlowInstMgr)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] Create failed: allocation failed\n");

			SAFE_NIDELETE(m_pkFlowCmdQueue);
			SAFE_NIDELETE(m_pkFlowStack);
			SAFE_NIDELETE(m_pkFlowInstMgr);
			return FALSE;
		}

		OutputDebugStringA("[FLOWMGR] Create ok\n");
		return TRUE;
	}

	//---------------------------------------------------------------------------

	void CFlowMgr::Destroy()
	{
		OutputDebugStringA("[FLOWMGR] Destroy begin\n");

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

		m_bFlowLock = FALSE;
		m_bProcessingLock = FALSE;

		OutputDebugStringA("[FLOWMGR] Destroy end\n");
	}

	//---------------------------------------------------------------------------

	void CFlowMgr::CurFlowImmediatelyDestroy()
	{
		OutputDebugStringA("[FLOWMGR] CurFlowImmediatelyDestroy begin\n");

		int iID = GetCurTopFlowID();

		while (iID != CFlow::FLW_NONE)
		{
			if (IsCurTopFlow())
			{
				char log[256] = { 0 };
				sprintf_s(log, "[FLOWMGR] CurFlowImmediatelyDestroy OnExit flow=%d\n", iID);
				OutputDebugStringA(log);

				GetCurTopFlow()->OnExit(-1);
			}

			EraseFlow(iID);

			iID = GetCurTopFlowID();
		}

		OutputDebugStringA("[FLOWMGR] CurFlowImmediatelyDestroy end\n");
	}

	//---------------------------------------------------------------------------

	BOOL CFlowMgr::StartFlow(int p_iID)
	{
		char log[256] = { 0 };
		sprintf_s(log, "[FLOWMGR] StartFlow id=%d\n", p_iID);
		OutputDebugStringA(log);

		if (!m_pkFlowStack || !m_pkFlowCmdQueue)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] StartFlow failed: queue/stack NULL\n");
			return FALSE;
		}

		assert(m_pkFlowStack->IsEmpty() && "StartFlow Failed!");

		if (!m_pkFlowStack->IsEmpty())
		{
			OutputDebugStringA("[FLOWMGR][ERROR] StartFlow failed: stack not empty\n");
			return FALSE;
		}

		PushFlow(p_iID);

		return TRUE;
	}

	//---------------------------------------------------------------------------

	void CFlowMgr::PushFlow(int p_iID)
	{
		char log[256] = { 0 };
		sprintf_s(log, "[FLOWMGR] PushFlow request id=%d\n", p_iID);
		OutputDebugStringA(log);

		if (!m_pkFlowCmdQueue)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] PushFlow failed: queue NULL\n");
			return;
		}

		if (!m_pkFlowCmdQueue->IsEmpty())
		{
			OutputDebugStringA("[FLOWMGR][WARN] PushFlow ignored: queue not empty\n");
			return;
		}

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
		char log[256] = { 0 };
		sprintf_s(log, "[FLOWMGR] ChangePopAllFlow request id=%d\n", p_iID);
		OutputDebugStringA(log);

		if (!m_pkFlowCmdQueue)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] ChangePopAllFlow failed: queue NULL\n");
			return;
		}

		if (!m_pkFlowCmdQueue->IsEmpty())
		{
			OutputDebugStringA("[FLOWMGR][WARN] ChangePopAllFlow ignored: queue not empty\n");
			return;
		}

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
		char log[256] = { 0 };
		sprintf_s(log, "[FLOWMGR] ChangeFlow request id=%d current=%d lock=%d processing=%d\n",
			p_iID,
			GetCurTopFlowID(),
			m_bFlowLock,
			m_bProcessingLock
		);
		OutputDebugStringA(log);

		if (!m_pkFlowCmdQueue)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] ChangeFlow failed: queue NULL\n");
			return;
		}

		if (!m_pkFlowCmdQueue->IsEmpty())
		{
			OutputDebugStringA("[FLOWMGR][WARN] ChangeFlow ignored: queue not empty\n");
			return;
		}

		if (m_pkFlowCmdQueue->IsFrontChange(p_iID))
		{
			OutputDebugStringA("[FLOWMGR][WARN] ChangeFlow ignored: same front change\n");
			return;
		}

		int iSize = GetCurFlowSize();

		for (int i = 0; i < iSize; ++i)
		{
			if (IsCurPosFlow(i))
			{
				char log2[256] = { 0 };
				sprintf_s(log2, "[FLOWMGR] ChangeFlow ReservedChangeFlow pos=%d flowID=%d next=%d\n",
					i,
					GetCurPosFlowID(i),
					p_iID
				);
				OutputDebugStringA(log2);

				GetCurPosFlow(i)->ReservedChangeFlow(p_iID);
			}
		}

		m_pkFlowCmdQueue->ChangeFlow(p_iID);

		OutputDebugStringA("[FLOWMGR] ChangeFlow queued\n");
	}

	//---------------------------------------------------------------------------

	void CFlowMgr::PopFlow(int p_iID)
	{
		char log[256] = { 0 };
		sprintf_s(log, "[FLOWMGR] PopFlow request id=%d\n", p_iID);
		OutputDebugStringA(log);

		if (!m_pkFlowCmdQueue)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] PopFlow failed: queue NULL\n");
			return;
		}

		if (!m_pkFlowCmdQueue->IsEmpty())
		{
			OutputDebugStringA("[FLOWMGR][WARN] PopFlow ignored: queue not empty\n");
			return;
		}

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
		OutputDebugStringA("[FLOWMGR] PopAllFlow request\n");

		if (!m_pkFlowCmdQueue)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] PopAllFlow failed: queue NULL\n");
			return;
		}

		if (m_pkFlowCmdQueue->IsFrontPopAll())
		{
			OutputDebugStringA("[FLOWMGR][WARN] PopAllFlow ignored: already front pop all\n");
			return;
		}

		m_pkFlowCmdQueue->PopAllFlow();
	}

	//---------------------------------------------------------------------------

	void CFlowMgr::CMDUpdate()
	{
		if (!m_pkFlowCmdQueue)
			return;

		/*
			Fallback global:
			Quando um flow chama ReservedChangeFlow(), normalmente o fade faz LockFlow()
			e depois chama UnLockFlow() quando termina.

			No teu caso, ao sair do Loading para InGame:
				[FLOW][Loading] FadeOut begin
				[FLOWMGR] LockFlow
				[FLOWMGR] ChangeFlow queued

			Mas depois o unlock não vem, então o CMD fica preso.
			Este contador deixa o fade aparecer, mas não deixa ficar preso para sempre.
		*/
		static int s_nFlowLockWaitFrames = 0;

		int nSafeCounter = 0;

		while (!m_pkFlowCmdQueue->IsEmpty())
		{
			if (++nSafeCounter > 16)
			{
				OutputDebugStringA("[FLOWMGR][ERROR] CMDUpdate safe counter exceeded. Breaking.\n");
				break;
			}

			if (m_bFlowLock)
			{
				++s_nFlowLockWaitFrames;

				if ((s_nFlowLockWaitFrames % 30) == 0)
				{
					char log[256] = { 0 };
					sprintf_s(log, "[FLOWMGR] CMDUpdate waiting: FlowLock=TRUE frames=%d\n", s_nFlowLockWaitFrames);
					OutputDebugStringA(log);
				}

				/*
					60 frames dá tempo ao fade para escurecer.
					Se mesmo assim ninguém chamar UnLockFlow(), desbloqueamos.
				*/
				if (s_nFlowLockWaitFrames >= 60)
				{
					OutputDebugStringA("[FLOWMGR][WARN] CMDUpdate FlowLock timeout. Force UnLockFlow.\n");
					UnLockFlow();
					s_nFlowLockWaitFrames = 0;
				}
				else
				{
					m_bProcessingLock = FALSE;
					return;
				}
			}
			else
			{
				s_nFlowLockWaitFrames = 0;
			}

			m_bProcessingLock = TRUE;

			CFlowCmdQueue::FLOWCMD kCmd;
			ZeroMemory(&kCmd, sizeof(kCmd));
			kCmd = m_pkFlowCmdQueue->GetAt();

			{
				char log[256] = { 0 };
				sprintf_s(log, "[FLOWMGR] CMDUpdate cmd=%d target=%d current=%d\n",
					kCmd.eCmdType,
					kCmd.iID,
					GetCurTopFlowID()
				);
				OutputDebugStringA(log);
			}

			switch (kCmd.eCmdType)
			{
			case CFlowCmdQueue::CMD_PUSH:
			{
				int iID = GetCurTopFlowID();

				if (iID != kCmd.iID)
				{
					if (IsCurTopFlow())
					{
						OutputDebugStringA("[FLOWMGR] CMD_PUSH OnOverride begin\n");
						GetCurTopFlow()->OnOverride(kCmd.iID);
						OutputDebugStringA("[FLOWMGR] CMD_PUSH OnOverride end\n");
					}

					if (RegisterFlow(kCmd.iID))
					{
						if (IsCurTopFlow())
						{
							OutputDebugStringA("[FLOWMGR] CMD_PUSH OnEnter begin\n");
							GetCurTopFlow()->OnEnter();
							OutputDebugStringA("[FLOWMGR] CMD_PUSH OnEnter end\n");
						}
					}
					else
					{
						OutputDebugStringA("[FLOWMGR][ERROR] CMD_PUSH RegisterFlow failed\n");
					}
				}
				else
				{
					OutputDebugStringA("[FLOWMGR][WARN] CMD_PUSH ignored: same current flow\n");
				}
			}
			break;

			case CFlowCmdQueue::CMD_CHANGE:
			{
				int iID = GetCurTopFlowID();

				{
					char log[256] = { 0 };
					sprintf_s(log, "[FLOWMGR] CMD_CHANGE begin current=%d target=%d\n", iID, kCmd.iID);
					OutputDebugStringA(log);
				}

				if (iID != kCmd.iID)
				{
					if (IsCurTopFlow())
					{
						OutputDebugStringA("[FLOWMGR] CMD_CHANGE OnExit begin\n");
						GetCurTopFlow()->OnExit(kCmd.iID);
						OutputDebugStringA("[FLOWMGR] CMD_CHANGE OnExit end\n");
					}
					else
					{
						OutputDebugStringA("[FLOWMGR][WARN] CMD_CHANGE current top flow NULL before OnExit\n");
					}

					if (EraseFlow(iID))
					{
						OutputDebugStringA("[FLOWMGR] CMD_CHANGE EraseFlow ok\n");
					}
					else
					{
						OutputDebugStringA("[FLOWMGR][WARN] CMD_CHANGE EraseFlow failed\n");
					}

					if (RegisterFlow(kCmd.iID))
					{
						OutputDebugStringA("[FLOWMGR] CMD_CHANGE RegisterFlow ok\n");

						if (IsCurTopFlow())
						{
							OutputDebugStringA("[FLOWMGR] CMD_CHANGE OnEnter begin\n");
							GetCurTopFlow()->OnEnter();
							OutputDebugStringA("[FLOWMGR] CMD_CHANGE OnEnter end\n");
						}
						else
						{
							OutputDebugStringA("[FLOWMGR][ERROR] CMD_CHANGE top flow NULL after RegisterFlow\n");
						}
					}
					else
					{
						OutputDebugStringA("[FLOWMGR][ERROR] CMD_CHANGE RegisterFlow failed\n");
					}
				}
				else
				{
					OutputDebugStringA("[FLOWMGR][WARN] CMD_CHANGE ignored: already current flow\n");
				}

				OutputDebugStringA("[FLOWMGR] CMD_CHANGE end\n");
			}
			break;

			case CFlowCmdQueue::CMD_CHANGE_POP_ALL:
			{
				OutputDebugStringA("[FLOWMGR] CMD_CHANGE_POP_ALL begin\n");

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
				else
				{
					OutputDebugStringA("[FLOWMGR][ERROR] CMD_CHANGE_POP_ALL RegisterFlow failed\n");
				}

				OutputDebugStringA("[FLOWMGR] CMD_CHANGE_POP_ALL end\n");
			}
			break;

			case CFlowCmdQueue::CMD_POP:
			{
				OutputDebugStringA("[FLOWMGR] CMD_POP begin\n");

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

				OutputDebugStringA("[FLOWMGR] CMD_POP end\n");
			}
			break;

			case CFlowCmdQueue::CMD_POP_ALL:
			{
				OutputDebugStringA("[FLOWMGR] CMD_POP_ALL begin\n");

				int iID = GetCurTopFlowID();

				while (iID != CFlow::FLW_NONE)
				{
					if (IsCurTopFlow())
						GetCurTopFlow()->OnExit(-1);

					EraseFlow(iID);

					iID = GetCurTopFlowID();
				}

				OutputDebugStringA("[FLOWMGR] CMD_POP_ALL end\n");
			}
			break;

			default:
			{
				char log[256] = { 0 };
				sprintf_s(log, "[FLOWMGR][ERROR] CMDUpdate unknown cmd=%d target=%d\n", kCmd.eCmdType, kCmd.iID);
				OutputDebugStringA(log);
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
		char log[256] = { 0 };
		sprintf_s(log, "[FLOWMGR] RegisterFlow begin id=%d\n", p_iID);
		OutputDebugStringA(log);

		if (!m_pkFlowStack || !m_pkFlowInstMgr)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] RegisterFlow failed: stack/instmgr NULL\n");
			return FALSE;
		}

		if (p_iID <= CFlow::FLW_NONE)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] RegisterFlow failed: invalid flow id\n");
			return FALSE;
		}

		if (m_pkFlowInstMgr->GetInst(p_iID))
		{
			OutputDebugStringA("[FLOWMGR][ERROR] RegisterFlow failed: flow already exists\n");
			return FALSE;
		}

		CFlow* pkFlow = CFlowFactory::CreateFlow(p_iID);

		if (!pkFlow)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] RegisterFlow failed: factory returned NULL\n");
			return FALSE;
		}

		if (!m_pkFlowStack->Push(p_iID))
		{
			OutputDebugStringA("[FLOWMGR][ERROR] RegisterFlow failed: stack push failed\n");
			SAFE_NIDELETE(pkFlow);
			return FALSE;
		}

		m_pkFlowInstMgr->AddInst(p_iID, pkFlow);

		OutputDebugStringA("[FLOWMGR] RegisterFlow ok\n");
		return TRUE;
	}

	//---------------------------------------------------------------------------

	BOOL CFlowMgr::EraseFlow(int p_iID)
	{
		char log[256] = { 0 };
		sprintf_s(log, "[FLOWMGR] EraseFlow begin id=%d top=%d\n", p_iID, GetCurTopFlowID());
		OutputDebugStringA(log);

		if (!m_pkFlowStack || !m_pkFlowInstMgr)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] EraseFlow failed: stack/instmgr NULL\n");
			return FALSE;
		}

		if (p_iID == CFlow::FLW_NONE)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] EraseFlow failed: FLW_NONE\n");
			return FALSE;
		}

		int iID = m_pkFlowStack->Top();

		if (iID != p_iID)
		{
			OutputDebugStringA("[FLOWMGR][ERROR] EraseFlow failed: top flow mismatch\n");
			return FALSE;
		}

		m_pkFlowStack->Pop();

		m_pkFlowInstMgr->DelInst(p_iID);

		OutputDebugStringA("[FLOWMGR] EraseFlow ok\n");
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
		OutputDebugStringA("[FLOWMGR] LockFlow\n");

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
		OutputDebugStringA("[FLOWMGR] UnLockFlow\n");

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