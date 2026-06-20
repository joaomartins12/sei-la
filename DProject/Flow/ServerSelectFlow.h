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

class CSelectServer;
class CFade;

//---------------------------------------------------------------------------
namespace Flow
{
	class CServerSelectFlow	: public CFlow
	{
	public:
		CServerSelectFlow(int p_iID);
		virtual ~CServerSelectFlow();

	public:
		// ************************
		// Lost, Reset Device
		// ************************
		virtual bool LostDevice(void* p_pvData);
		virtual bool ResetDevice(bool p_bBeforeReset, void* p_pvData);
		// ************************

		virtual void ReservedChangeFlow(int p_iNextFlowID);
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
		virtual void InputFrame();
		virtual BOOL OnMsgHandler(const MSG& p_kMsg);	//2016-01-13-nova 윈도우Msg로 처리
		virtual void UpdateFrame();
		virtual void CullFrame();
		virtual void RenderBackScreenFrame();
		virtual void RenderSceneFrame();
		virtual void RenderScreenFrame();
		virtual void RenderUIFrame();
		// ************************

	private:

		// ************************
		// Resource
		// ************************
		virtual BOOL LoadResource();
		virtual void ReleaseResource();
		// ************************

		bool  m_bBgmPlay;

		CSelectServer*	m_pServerSelect;
		CFade*			m_pFadeUI;
	};
}
//---------------------------------------------------------------------------