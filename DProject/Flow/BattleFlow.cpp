//---------------------------------------------------------------------------
//
// 파일명 : DataTradeCenterFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "BattleFlow.h"

#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------
namespace Flow
{
	CBattleFlow::CBattleFlow(int p_iID) : CFlow(p_iID), m_bBgmPlay(false), m_pFadeUI(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CBattleFlow::~CBattleFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CBattleFlow::Initialize()
	{
		if( g_pDataMng )
			g_pDataMng->GetPostLoad()->PostLoad();

		m_pFadeUI = NiNew CFade;
		if( m_pFadeUI )
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

		return TRUE;
	}

	void CBattleFlow::OnEnter(void)
	{		
		CFlow::OnEnter();

		if( g_pGameIF )
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PROTECT );

			g_pGameIF->GetMiniMap()->SetShowWindow( false );
			g_pGameIF->GetQuestHelper()->SetShowWindow( false );
// 			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PUBLICITY ) )
// 				g_pGameIF->GetPublicity()->ClearList();

		}

		if( g_pHelpArrow )
			g_pHelpArrow->ReleaseArrowPoint();
	}

	void CBattleFlow::OnExit(int p_iNextFlowID)
	{
		CFlow::OnExit(p_iNextFlowID);
	}


	void CBattleFlow::OnOverride(int p_iNextFlowID)
	{
		SetPaused( true );
	}

	void CBattleFlow::OnResume(int p_iNextFlowID)
	{
		SetPaused( false );	
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_IN, 0.5f);
	}

	//---------------------------------------------------------------------------
	void CBattleFlow::Terminate()
	{
		SAFE_NIDELETE(m_pFadeUI);
		RESOURCEMGR_ST.CleanUpResource();
	}

	void CBattleFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	void CBattleFlow::ReservedPopFlow()
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CBattleFlow::RenderSceneFrame()
	{// 3D Draw
		switch( g_pEngine->GetPostEffect() )
		{
		case CEngine::PE_NONE:
		case CEngine::PE_GAUSSIAN_BLUR:
			{
				if( nsCsGBTerrain::g_bShadowRender == true )
					g_pMngCollector->Render_Shadow( true );
				else
					g_pMngCollector->Render( true );
				g_pEngine->Render();
			}		
			break;
		}
	}

	bool CBattleFlow::BeginRenderTarget()
	{
		// BeginTarget
		return g_pEngine->BeginUsingCurrentRenderTargetGroup( NiRenderer::CLEAR_ALL );
	}

	//---------------------------------------------------------------------------
	bool CBattleFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CBattleFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		return true;
	}

	BOOL CBattleFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);
		GLOBALINPUT_ST._Keyboard_Input( p_kMsg );	
		return FALSE;
	}

	//---------------------------------------------------------------------------
	void CBattleFlow::InputFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CBattleFlow::UpdateFrame()
	{
		CONTENTSSYSTEM_PTR->Update( g_fDeltaTime );

		if( g_pSystemMsg )
			g_pSystemMsg->Update();

		if( g_pSkillMsg )
			g_pSkillMsg->Update();

		if( m_pFadeUI )
			m_pFadeUI->Update(g_fDeltaTime);

#ifdef USE_BARCODE_REDER
		nsBARCODE::Update();
#endif
		if( net::game )
			nsCsGBTerrain::g_ServerTime.s_nServerTime = net::game->GetTimeTS();

		if( g_pGameIF )
		{			
			cGlobalInput::eMOUSE_INPUT mi = g_pGameIF->Update(g_fDeltaTime);
			GLOBALINPUT_ST.KeyboardUpdate();
			GLOBALINPUT_ST._Mouse_Input(mi);
			g_pMngCollector->Update( mi );					
		}

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Update( g_fDeltaTime );

		cIconMng::GlobalUpdate();
		CAMERA_ST.Update( g_fDeltaTime );

		if( g_pWeather )
		{
			g_pWeather->SetPos( g_pCharMng->GetTamerUser()->GetPos() );
			g_pWeather->Update( g_fDeltaTime, g_pEngine->GetD3DView() );	
		}	

		CURSOR_ST.LoopReset();	//2016-02-23-nova
	}
	//---------------------------------------------------------------------------
	void CBattleFlow::RenderUIFrame()
	{
		g_pEngine->ScreenSpace();

		if( g_pGameIF )
			g_pGameIF->Render();

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Render();

		if( g_pSystemMsg )
			g_pSystemMsg->Render();

		cMessageBox::RenderList();

		CURSOR_ST.Render();

		if( m_pFadeUI )
			m_pFadeUI->Render();
	}
}
//---------------------------------------------------------------------------