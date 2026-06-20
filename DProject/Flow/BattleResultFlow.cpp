//---------------------------------------------------------------------------
//
// 파일명 : DataTradeCenterFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "BattleResultFlow.h"

#include "../_Interface/09.BattleSystem/BattleResultPage.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CBattleResultFlow::CBattleResultFlow(int p_iID) : CFlow(p_iID), m_bBgmPlay(false), m_pFadeUI(NULL),m_pBattleResult(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CBattleResultFlow::~CBattleResultFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CBattleResultFlow::Initialize()
	{
		CURSOR_ST.InitCursor();
		CAMERA_ST.SaveCameraState();	// 기존 카메라 상태 저장

		if( g_pSoundMgr )
			g_pSoundMgr->PlayMusic( "Game_opening.mp3", 0.0f );


		//g_pGameIF->HideWindow();		// 기존 UI 상태 저장 및 숨김.

		m_pBattleResult = NiNew CBattleResult;

		if(m_pBattleResult && m_pBattleResult->Construct() )
			m_pBattleResult->Create();

		m_pFadeUI = NiNew CFade;
		if( m_pFadeUI )
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

		if( g_pEngine )
			g_pEngine->SetClearColor( NiColorA::BLACK );

		return TRUE;
	}

	void CBattleResultFlow::OnEnter(void)
	{		
		CFlow::OnEnter();
	}

	void CBattleResultFlow::OnExit(int p_iNextFlowID)
	{
		CFlow::OnExit(p_iNextFlowID);
	}
	//---------------------------------------------------------------------------
	void CBattleResultFlow::Terminate()
	{
		SAFE_NIDELETE(m_pBattleResult);
		SAFE_NIDELETE(m_pFadeUI);

		//g_pGameIF->HideWindowRestore();	// 기존 UI 상태 복구.

		CAMERA_ST.LoadCameraState();	// 카메라 상태 복구
		RESOURCEMGR_ST.CleanUpResource();
	}

	void CBattleResultFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	void CBattleResultFlow::ReservedPopFlow()
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CBattleResultFlow::RenderSceneFrame()
	{// 3D Draw

		// 3D 렌더, 월드, 캐릭터, 이팩트 등등 ...
		if( g_pEngine )
			g_pEngine->ResetRendererCamera();

		if( m_pBattleResult )
			m_pBattleResult->Render3DModel();

		if( g_pEngine )
			g_pEngine->Render();
	}

	bool CBattleResultFlow::BeginRenderTarget()
	{
		// BeginTarget
		return g_pEngine->BeginUsingCurrentRenderTargetGroup( NiRenderer::CLEAR_ALL );
	}

	//---------------------------------------------------------------------------
	bool CBattleResultFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CBattleResultFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{

		return true;
	}

	BOOL CBattleResultFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if( cMessageBox::UpdateList() )
			return FALSE;	

// 		if( m_pBattleResult )
// 			m_pBattleResult->UpdateKeyboard(p_kMsg);

		return FALSE;
	}

	//---------------------------------------------------------------------------
	void CBattleResultFlow::InputFrame()
	{
		if( cMessageBox::UpdateList() )
			return;
		if( m_pBattleResult )
			m_pBattleResult->UpdateMouse();
	}
	//---------------------------------------------------------------------------
	void CBattleResultFlow::UpdateFrame()
	{
		CONTENTSSYSTEM_PTR->Update( g_fDeltaTime );

		g_pSystemMsg->Update();

		if( m_pFadeUI )
			m_pFadeUI->Update(g_fDeltaTime);

		if( m_pBattleResult )
		{
			m_pBattleResult->Update( g_fDeltaTime );
			m_pBattleResult->Update3DAccumtime( g_fAccumTime );
		}

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Update( g_fDeltaTime );

		if( net::game )
			nsCsGBTerrain::g_ServerTime.s_nServerTime = net::game->GetTimeTS();

		CAMERA_ST.Update( g_fDeltaTime );

		CURSOR_ST.LoopReset();	//2016-02-23-nova 
	}

	//---------------------------------------------------------------------------
	void CBattleResultFlow::RenderUIFrame()
	{
		g_pEngine->ScreenSpace();

		if( m_pBattleResult )
			m_pBattleResult->Render();

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