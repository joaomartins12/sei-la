//---------------------------------------------------------------------------
//
// 파일명 : LoginFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "DatsCenterFlow.h"

#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CDatsCenterFlow::CDatsCenterFlow(int p_iID) : CFlow(p_iID), m_bBgmPlay(false), m_pFadeUI(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CDatsCenterFlow::~CDatsCenterFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CDatsCenterFlow::Initialize()
	{
		CURSOR_ST.InitCursor();

		if( g_pMngCollector )
			g_pMngCollector->LoadDatsCenter();

		m_pFadeUI = NiNew CFade;
		if( m_pFadeUI )
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);
		
		return TRUE;
	}

	void CDatsCenterFlow::OnEnter(void)
	{		
		CFlow::OnEnter();
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_DATS_CENTER);
	}

	void CDatsCenterFlow::OnExit(int p_iNextFlowID)
	{
		CFlow::OnExit(p_iNextFlowID);
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_DATS_CENTER);
	}
	//---------------------------------------------------------------------------
	void CDatsCenterFlow::Terminate()
	{
		SAFE_NIDELETE(m_pFadeUI);

		if( g_pMngCollector )
			g_pMngCollector->EndDatsCenter();
		RESOURCEMGR_ST.CleanUpResource();
	}

	void CDatsCenterFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	void CDatsCenterFlow::ReservedPopFlow()
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CDatsCenterFlow::RenderSceneFrame()
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

	bool CDatsCenterFlow::BeginRenderTarget()
	{
		// BeginTarget
		return g_pEngine->BeginUsingCurrentRenderTargetGroup( NiRenderer::CLEAR_ALL );
	}

	//---------------------------------------------------------------------------
	bool CDatsCenterFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CDatsCenterFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------

	BOOL CDatsCenterFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);
		GLOBALINPUT_ST._Keyboard_Input( p_kMsg );	

		return FALSE;
	}

	void CDatsCenterFlow::InputFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CDatsCenterFlow::UpdateFrame()
	{
		CONTENTSSYSTEM_PTR->Update( (float)g_fDeltaTime );

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

		cGlobalInput::eMOUSE_INPUT mi = g_pGameIF->Update(g_fDeltaTime);
		GLOBALINPUT_ST.KeyboardUpdate();
		GLOBALINPUT_ST._Mouse_Input(mi);
		g_pMngCollector->Update( mi );		

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Update( g_fDeltaTime );

		CAMERA_ST.Update( g_fDeltaTime );

		if( g_pWeather )
		{
			g_pWeather->SetPos( g_pCharMng->GetTamerUser()->GetPos() );
			g_pWeather->Update( g_fDeltaTime, g_pEngine->GetD3DView() );	
		}

		CURSOR_ST.LoopReset();	//2016-02-23-nova
	}
	//---------------------------------------------------------------------------
	void CDatsCenterFlow::RenderUIFrame()
	{
		g_pEngine->ScreenSpace();

		if( g_pGameIF )
			g_pGameIF->Render();

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Render();

		if( g_pMngCollector )
			g_pMngCollector->Render_PostIF();

#ifndef _GIVE
		if( g_pMngCollector )
			g_pMngCollector->RenderText();
#endif

		if( g_pSystemMsg )
			g_pSystemMsg->Render();

		cBaseWindow* pWin = g_pGameIF->_GetPointer( cBaseWindow::WT_PUBLICITY, 0 );
		if( pWin && pWin->IsShowWindow() )
			pWin->Render();

		cMessageBox::RenderList();

		CURSOR_ST.Render();

		if( m_pFadeUI )
			m_pFadeUI->Render();
	}
}
//---------------------------------------------------------------------------