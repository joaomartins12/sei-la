//---------------------------------------------------------------------------
//
// 파일명 : LoginFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "LoginFlow.h"

#include "../_Interface/03.Login/Login.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CLoginFlow::CLoginFlow(int p_iID) : CFlow(p_iID), m_bBgmPlay(false), m_pLoginUI(NULL), m_pFadeUI(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CLoginFlow::~CLoginFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CLoginFlow::Initialize()
	{
		m_pLoginUI = NiNew CLogin;
		m_pFadeUI = NiNew CFade;

		if(m_pLoginUI && m_pLoginUI->Construct() )
			m_pLoginUI->LoginInit();

		if( m_pFadeUI )
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

		if( g_pSoundMgr )
			g_pSoundMgr->PlayMusic( "Game_opening.mp3", 0.0f );

		GLOBALDATA_ST.Clear2ndPass();		

		g_pEngine->SetGaussianBlurVal( 0.9f, 0.25f, 0.3f );
		return TRUE;
	}

	void CLoginFlow::OnEnter(void)
	{		
		CFlow::OnEnter();
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_LOGIN);
	}

	void CLoginFlow::OnExit(int p_iNextFlowID)
	{
		CFlow::OnExit(p_iNextFlowID);
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_LOGIN);
	}

	//---------------------------------------------------------------------------
	void CLoginFlow::Terminate()
	{
		SAFE_NIDELETE(m_pLoginUI);
		SAFE_NIDELETE(m_pFadeUI);

		RESOURCEMGR_ST.CleanUpResource();
	}
	
	void CLoginFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	//---------------------------------------------------------------------------
	bool CLoginFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CLoginFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		return true;
	}

	BOOL CLoginFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if( cMessageBox::UpdateList() )
			return FALSE;

		if( m_pLoginUI )
			m_pLoginUI->UpdateKeyboard(p_kMsg);

		return FALSE;
	}
	
	void CLoginFlow::InputFrame()            // Input     
	{
		if( cMessageBox::UpdateList() )
			return;

		if( m_pLoginUI )
			m_pLoginUI->UpdateMouse();
	}

	//---------------------------------------------------------------------------
	void CLoginFlow::UpdateFrame()
	{
		CONTENTSSYSTEM_PTR->Update( (float)g_fDeltaTime );

		if( g_pSystemMsg )
			g_pSystemMsg->Update();

		if( m_pFadeUI )
			m_pFadeUI->Update(g_fDeltaTime);

		if( m_pLoginUI )
		{
			m_pLoginUI->Update( g_fDeltaTime );
			m_pLoginUI->Update3DAccumtime( g_fAccumTime );
		}

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Update( g_fDeltaTime );

		CURSOR_ST.LoopReset();	//2016-02-23-nova
	}

	//---------------------------------------------------------------------------
	void CLoginFlow::RenderBackScreenFrame()
	{
		// 2D Screen
		if( m_pLoginUI )
			m_pLoginUI->RenderScreenUI();
	}
	//---------------------------------------------------------------------------
	void CLoginFlow::RenderSceneFrame()
	{// 3D Draw
		// 3D 렌더, 월드, 캐릭터, 이팩트 등등 ...
		if( g_pEngine )
			g_pEngine->ResetRendererCamera();

		if( m_pLoginUI )
			m_pLoginUI->Render3DModel();

		if( g_pEngine )
			g_pEngine->Render();
	}
	//---------------------------------------------------------------------------
	void CLoginFlow::RenderUIFrame()
	{
		if( g_pEngine )
			g_pEngine->ScreenSpace();

		// UI Render
		if( m_pLoginUI )
			m_pLoginUI->Render();

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Render();

		if( g_pSystemMsg )
			g_pSystemMsg->Render();

		cMessageBox::RenderList();

		if( m_pFadeUI )
			m_pFadeUI->Render();
	}
}
//---------------------------------------------------------------------------