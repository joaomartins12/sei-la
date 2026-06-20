//---------------------------------------------------------------------------
//
// 파일명 : FlowCmdQueue.cpp
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "FlowCmdQueue.h"
#include "Flow.h"
//---------------------------------------------------------------------------
namespace Flow
{
	CFlowCmdQueue::CFlowCmdQueue()
	:m_que_FlowCMD()
	{
		Clear();
	}
//---------------------------------------------------------------------------
	CFlowCmdQueue::~CFlowCmdQueue()
	{
	}
//---------------------------------------------------------------------------
	void CFlowCmdQueue::PushFlow(int p_iID)
	{
		// Psuh
		FLOWCMD kCmd;
		kCmd.eCmdType = CMD_PUSH;
		kCmd.iID      = p_iID;
		kCmd.fDelay   = 0.0f;
		m_que_FlowCMD.push(kCmd);
	}
//---------------------------------------------------------------------------
	void CFlowCmdQueue::ChangeFlow(int p_iID)
	{
		// Change
		FLOWCMD kCmd;
		kCmd.eCmdType = CMD_CHANGE;
		kCmd.iID      = p_iID;
		kCmd.fDelay   = 0.0f;
		m_que_FlowCMD.push(kCmd);
#ifdef DEBUG_NETWORK
		DBG("Inner change flow END\n");
#endif
	}
	//---------------------------------------------------------------------------
	void CFlowCmdQueue::ChangePopAllFlow(int p_iID)
	{
		// Change
		FLOWCMD kCmd;
		kCmd.eCmdType = CMD_CHANGE_POP_ALL;
		kCmd.iID      = p_iID;
		kCmd.fDelay   = 0.0f;
		m_que_FlowCMD.push(kCmd);
	}
//---------------------------------------------------------------------------
	void CFlowCmdQueue::PopFlow(int p_iID)
	{
		// Pop
		FLOWCMD kCmd;
		kCmd.eCmdType = CMD_POP;
		kCmd.iID      = p_iID;
		kCmd.fDelay   = 0.0f;
		m_que_FlowCMD.push(kCmd);
	}
//---------------------------------------------------------------------------
	void CFlowCmdQueue::PopAllFlow()
	{
		// Pop All
		FLOWCMD kCmd;
		kCmd.eCmdType = CMD_POP_ALL;
		kCmd.iID      = CFlow::FLW_NONE;
		kCmd.fDelay   = 0.0f;
		m_que_FlowCMD.push(kCmd);
	}
//---------------------------------------------------------------------------
	CFlowCmdQueue::FLOWCMD CFlowCmdQueue::GetAt()
	{
		FLOWCMD kCmd;
		kCmd = m_que_FlowCMD.front();
		m_que_FlowCMD.pop();
		return kCmd;
	}
//---------------------------------------------------------------------------
	CFlowCmdQueue::FLOWCMD CFlowCmdQueue::Front() const
	{
		return m_que_FlowCMD.front();
	}
//---------------------------------------------------------------------------
	void CFlowCmdQueue::Pop()
	{
		m_que_FlowCMD.pop();
	}
//---------------------------------------------------------------------------
	int CFlowCmdQueue::GetSize() const
	{
		return static_cast<int>(m_que_FlowCMD.size());
	}
//---------------------------------------------------------------------------
	BOOL CFlowCmdQueue::IsEmpty() const
	{
		return m_que_FlowCMD.empty();
	}
//---------------------------------------------------------------------------
	void CFlowCmdQueue::Clear()
	{
		while(!IsEmpty())
		{
			Pop();
		}
	}
//---------------------------------------------------------------------------
	bool CFlowCmdQueue::IsFrontPushFlow(int p_iID) const
	{
		if( IsEmpty() )
			return false;

		FLOWCMD fCmd = Front();
		if( fCmd.iID == p_iID && fCmd.eCmdType == CMD_PUSH )
			return true;

		return false;
	}

	//---------------------------------------------------------------------------
	bool CFlowCmdQueue::IsFrontChange(int p_iID) const
	{
		if( IsEmpty() )
			return false;

		FLOWCMD fCmd = Front();
		if( fCmd.iID == p_iID && fCmd.eCmdType == CMD_CHANGE )
			return true;

		return false;
	}

	//---------------------------------------------------------------------------
	bool CFlowCmdQueue::IsFrontChangePopAll(int p_iID) const
	{
		if( IsEmpty() )
			return false;

		FLOWCMD fCmd = Front();
		if( fCmd.iID == p_iID && fCmd.eCmdType == CMD_CHANGE_POP_ALL )
			return true;

		return false;
	}

	//---------------------------------------------------------------------------
	bool CFlowCmdQueue::IsFrontPop(int p_iID) const
	{
		if( IsEmpty() )
			return false;

		FLOWCMD fCmd = Front();
		if( fCmd.iID == p_iID && fCmd.eCmdType == CMD_POP )
			return true;

		return false;
	}

	//---------------------------------------------------------------------------
	bool CFlowCmdQueue::IsFrontPopAll() const
	{
		if( IsEmpty() )
			return false;

		FLOWCMD fCmd = Front();
		if( fCmd.eCmdType == CMD_POP_ALL )
			return true;

		return false;
	}
}
//---------------------------------------------------------------------------