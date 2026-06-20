//---------------------------------------------------------------------------
//
// 파일명 : DataTradeCenterFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "BattleRegisterFlow.h"

#include "../_Interface/09.BattleSystem/BattleRegisterPage.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CBattleRegisterFlow::CBattleRegisterFlow(int p_iID) : CFlow(p_iID), m_bBgmPlay(false), m_pFadeUI(NULL),m_pBattleRegister(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CBattleRegisterFlow::~CBattleRegisterFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CBattleRegisterFlow::Initialize()
	{
		CURSOR_ST.InitCursor();
		CAMERA_ST.SaveCameraState();	// 기존 카메라 상태 저장

		if( g_pSoundMgr )
			g_pSoundMgr->PlayMusic( "Game_opening.mp3", 0.0f );


		//g_pGameIF->HideWindow();		// 기존 UI 상태 저장 및 숨김.
		
		m_pBattleRegister = NiNew CBattleRegister;

		if(m_pBattleRegister && m_pBattleRegister->Construct() )
			m_pBattleRegister->Create();

		m_pFadeUI = NiNew CFade;
		if( m_pFadeUI )
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

		if( g_pEngine )
			g_pEngine->SetClearColor( NiColorA::BLACK );

		return TRUE;
	}

	void CBattleRegisterFlow::OnEnter(void)
	{		
		CFlow::OnEnter();
	}

	void CBattleRegisterFlow::OnExit(int p_iNextFlowID)
	{
		CFlow::OnExit(p_iNextFlowID);
	}
	//---------------------------------------------------------------------------
	void CBattleRegisterFlow::Terminate()
	{
		SAFE_NIDELETE(m_pBattleRegister);
		SAFE_NIDELETE(m_pFadeUI);

		//g_pGameIF->HideWindowRestore();	// 기존 UI 상태 복구.

		CAMERA_ST.LoadCameraState();	// 카메라 상태 복구
		RESOURCEMGR_ST.CleanUpResource();
	}

	void CBattleRegisterFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	void CBattleRegisterFlow::ReservedPopFlow()
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CBattleRegisterFlow::RenderSceneFrame()
	{// 3D Draw

		// 3D 렌더, 월드, 캐릭터, 이팩트 등등 ...
		if( g_pEngine )
			g_pEngine->ResetRendererCamera();

		if( m_pBattleRegister )
			m_pBattleRegister->Render3DModel();

		if( g_pEngine )
			g_pEngine->Render();
	}

	bool CBattleRegisterFlow::BeginRenderTarget()
	{
		// BeginTarget
		return g_pEngine->BeginUsingCurrentRenderTargetGroup( NiRenderer::CLEAR_ALL );
	}

	//---------------------------------------------------------------------------
	bool CBattleRegisterFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CBattleRegisterFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{

		return true;
	}

	BOOL CBattleRegisterFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if( cMessageBox::UpdateList() )
			return FALSE;	

		if( m_pBattleRegister )
			m_pBattleRegister->UpdateKeyboard(p_kMsg);

		return FALSE;
	}

	//---------------------------------------------------------------------------
	void CBattleRegisterFlow::InputFrame()
	{
		if( cMessageBox::UpdateList() )
			return;
		if( m_pBattleRegister )
			m_pBattleRegister->UpdateMouse();
	}
	//---------------------------------------------------------------------------
	void CBattleRegisterFlow::UpdateFrame()
	{
		CONTENTSSYSTEM_PTR->Update( g_fDeltaTime );

		g_pSystemMsg->Update();

		if( m_pFadeUI )
			m_pFadeUI->Update(g_fDeltaTime);

		if( m_pBattleRegister )
		{
			m_pBattleRegister->Update( g_fDeltaTime );
			m_pBattleRegister->Update3DAccumtime( g_fAccumTime );
		}

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Update( g_fDeltaTime );

		if( net::game )
			nsCsGBTerrain::g_ServerTime.s_nServerTime = net::game->GetTimeTS();

		CAMERA_ST.Update( g_fDeltaTime );

		CURSOR_ST.LoopReset();	//2016-02-23-nova 
	}

	//---------------------------------------------------------------------------
	void CBattleRegisterFlow::RenderUIFrame()
	{
		g_pEngine->ScreenSpace();

		if( m_pBattleRegister )
			m_pBattleRegister->Render();

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