//---------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include "Flow.h"

class cSecondPassword;
class CFade;

//---------------------------------------------------------------------------
namespace Flow
{
	class CSecondPasswordFlow	: public CFlow
	{
	public:
		CSecondPasswordFlow(int p_iID);
		virtual ~CSecondPasswordFlow();

	public:
		// ************************
		// Lost, Reset Device
		// ************************
		virtual bool LostDevice(void* p_pvData);
		virtual bool ResetDevice(bool p_bBeforeReset, void* p_pvData);
		// ************************
		virtual void ReservedChangeFlow(int p_iNextFlowID);
		virtual void InputFrame();						// Input  
		virtual BOOL OnMsgHandler(const MSG& p_kMsg);	//2016-01-13-nova 윈도우Msg로 처리
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
		virtual void UpdateFrame();
		virtual void RenderUIFrame();
		virtual void RenderSceneFrame();
		virtual void RenderBackScreenFrame(); // 2D Screen Draw
		// ************************

	private:
		bool				m_bBgmPlay;

		CFade*				m_pFadeUI;
		cSecondPassword*	m_pSecondPassword;
	};
}
//---------------------------------------------------------------------------