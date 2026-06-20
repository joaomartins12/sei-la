// ---------------------------------------------------------------------------
// 
// 파일명 : LoginFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
// 
// ---------------------------------------------------------------------------
#include "StdAfx.h"
#include "DigimonCreateFlow.h"

#include "../_Interface/07.CharacterCreate/DigimonCreate.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CDigimonCreateFlow::CDigimonCreateFlow(int p_iID) : CFlow(p_iID), m_bBgmPlay(false), m_pFadeUI(NULL),m_pDigimonCreate(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CDigimonCreateFlow::~CDigimonCreateFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CDigimonCreateFlow::Initialize()
	{
		m_pDigimonCreate = NiNew CDigimonCreate;

		if( m_pDigimonCreate  && m_pDigimonCreate->Construct() )
			m_pDigimonCreate->Init();

		m_pFadeUI = NiNew CFade;
		if( m_pFadeUI )
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

		if( g_pSoundMgr )
			g_pSoundMgr->PlayMusic( "Game_opening.mp3", 0.0f );

		g_pEngine->SetGaussianBlurVal( 0.9f, 0.25f, 0.3f );

		return TRUE;
	}

	void CDigimonCreateFlow::OnEnter(void)
	{		
		CFlow::OnEnter();
	}

	void CDigimonCreateFlow::OnExit(int p_iNextFlowID)
	{
		CFlow::OnExit(p_iNextFlowID);
	}


	void CDigimonCreateFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	void CDigimonCreateFlow::ReservedPopFlow()
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	//---------------------------------------------------------------------------
	void CDigimonCreateFlow::Terminate()
	{
		SAFE_NIDELETE(m_pDigimonCreate);
		SAFE_NIDELETE(m_pFadeUI);
		RESOURCEMGR_ST.CleanUpResource();
	}
	//---------------------------------------------------------------------------
	bool CDigimonCreateFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CDigimonCreateFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------

	BOOL CDigimonCreateFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if( cMessageBox::UpdateList() )
			return FALSE;	

		if( m_pDigimonCreate )
			m_pDigimonCreate->UpdateKeyboard(p_kMsg);

		return FALSE;
	}

	void CDigimonCreateFlow::InputFrame()
	{
		if( cMessageBox::UpdateList() )
			return;

		if( m_pDigimonCreate )
			m_pDigimonCreate->UpdateMouse();
	}
	//---------------------------------------------------------------------------
	void CDigimonCreateFlow::UpdateFrame()
	{
		CONTENTSSYSTEM_PTR->Update( (float)g_fDeltaTime );

		g_pSystemMsg->Update();

		if(m_pDigimonCreate)
			m_pDigimonCreate->Update(g_fDeltaTime);

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Update( g_fDeltaTime );

		if( m_pFadeUI )
			m_pFadeUI->Update(g_fDeltaTime);

		CURSOR_ST.LoopReset();	//2016-02-23-nova
	}
	//---------------------------------------------------------------------------
	void CDigimonCreateFlow::CullFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CDigimonCreateFlow::RenderSceneFrame()
	{
	}

	void CDigimonCreateFlow::RenderBackScreenFrame()
	{
		if(m_pDigimonCreate)
			m_pDigimonCreate->RenderScreenUI();
	}

	//---------------------------------------------------------------------------
	void CDigimonCreateFlow::RenderScreenFrame()
	{
		g_pEngine->ResetRendererCamera();

		if( m_pDigimonCreate )
			m_pDigimonCreate->Render3DModel();

		g_pEngine->Render();
	}
	//---------------------------------------------------------------------------
	void CDigimonCreateFlow::RenderUIFrame()
	{
		g_pEngine->ScreenSpace();

		if(m_pDigimonCreate)
			m_pDigimonCreate->Render();

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Render();
		
		if( g_pSystemMsg )
			g_pSystemMsg->Render();

		cMessageBox::RenderList();

		if( m_pFadeUI )
			m_pFadeUI->Render();
	}

	BOOL CDigimonCreateFlow::LoadResource()
	{
		return TRUE;
	}

	void CDigimonCreateFlow::ReleaseResource()
	{
	}
}
//---------------------------------------------------------------------------
