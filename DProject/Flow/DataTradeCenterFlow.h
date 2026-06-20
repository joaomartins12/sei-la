//---------------------------------------------------------------------------
//
// 파일명 : LoginFlow.h
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include "Flow.h"

class CFade;
//---------------------------------------------------------------------------
namespace Flow
{
	class CDataTradeCenterFlow : public CFlow
	{
	public:
		CDataTradeCenterFlow(int p_iID);
		virtual ~CDataTradeCenterFlow();

	public:
		// ************************
		// Lost, Reset Device
		// ************************
		virtual bool LostDevice(void* p_pvData);
		virtual bool ResetDevice(bool p_bBeforeReset, void* p_pvData);
		// ************************

		virtual void ReservedChangeFlow(int p_iNextFlowID);
		virtual void ReservedPopFlow();
		virtual void OnEnter(void);
		virtual	void OnExit(int p_iNextFlowID);

	protected:
		// ************************
		// Init, Term
		// ************************
		virtual BOOL Initialize();
		virtual void Terminate();
		// ************************

		// ************************
		// Update, Cull, Render
		// ************************
		virtual bool BeginRenderTarget();
		virtual void InputFrame();
		virtual BOOL OnMsgHandler(const MSG& p_kMsg);	//2016-01-14-nova 윈도우Msg로 처리
		virtual void UpdateFrame();
		virtual void RenderSceneFrame();
		virtual void RenderUIFrame();
		// ************************

	private:
		bool		m_bBgmPlay;
		CFade*		m_pFadeUI;
		CDataTradeCenter*		m_pDataTradeCenter;
	};
}
//---------------------------------------------------------------------------