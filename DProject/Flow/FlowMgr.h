//---------------------------------------------------------------------------
//
// 파일명 : FlowMgr.h
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------

#include "InstMgr.h"

namespace Flow
{
	class CFlow;
	class CFlowCmdQueue;
	class CFlowStack;
//---------------------------------------------------------------------------
	class CFlowMgr : public CSingleton< CFlowMgr >
	{
	public:
		CFlowMgr();
		~CFlowMgr();

	public:
		// Create, Destroy
		BOOL Create();
		void Destroy();

		// Flow CMD
		BOOL StartFlow(int p_iID);
		void PushFlow(int p_iID);
		void ChangeFlow(int p_iID);
		void PopFlow(int p_iID);
		void ChangePopAllFlow( int p_iID );
		void PopAllFlow();

		// 각 게임 Flowd 의 
		// OnIdle, OnMsgHandler
		void OnIdle();
		BOOL OnMsgHandler(const MSG& p_kMsg);

		// Lost, Reset Device
		bool LostDevice(void* p_pvData);
		bool ResetDevice(bool p_bBeforeReset, void* p_pvData);

		void LockFlow();
		void UnLockFlow();

	private:
		// CMD Update
		void CMDUpdate();

		// Resigster, Erase
		BOOL RegisterFlow(int p_iID);
		BOOL EraseFlow(int p_iID);
		void CurFlowImmediatelyDestroy();

	public:
		// 현재 Flow 관련 *****************

		// Flow  Size
		int GetCurFlowSize() const;

		// Cur Top Flow
		BOOL IsCurTopFlow() const;
		CFlow* GetCurTopFlow() const;
		int GetCurTopFlowID() const;

		// Cur Pos Flow
		BOOL IsCurPosFlow(int p_iPos) const;
		CFlow* GetCurPosFlow(int p_iPos) const;
		int GetCurPosFlowID(int p_iPos) const;

		BOOL IsCurrentFlow(int p_iID) const;
		// *********************************

	private:
		CFlowCmdQueue* m_pkFlowCmdQueue;
		CFlowStack* m_pkFlowStack;
		CInstMgr<int, CFlow*>* m_pkFlowInstMgr;

		BOOL		m_bFlowLock;
		BOOL		m_bProcessingLock;
	};
}
// Singleton
#define FLOWMGR_ST			Flow::CFlowMgr::GetSingleton()
#define FLOWMGR_STPTR		Flow::CFlowMgr::GetSingletonPtr()
//---------------------------------------------------------------------------