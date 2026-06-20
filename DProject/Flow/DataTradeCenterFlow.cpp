//---------------------------------------------------------------------------
//
// 파일명 : DataTradeCenterFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "DataTradeCenterFlow.h"

#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"


#include "../_Interface/Game/DataTradeContents.h"
#include "../_Interface/Game/DataTradeCenter.h"
#include "../_Interface/Game/DataTradeSelect.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CDataTradeCenterFlow::CDataTradeCenterFlow(int p_iID) : CFlow(p_iID), m_bBgmPlay(false), m_pFadeUI(NULL), m_pDataTradeCenter(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CDataTradeCenterFlow::~CDataTradeCenterFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CDataTradeCenterFlow::Initialize()
	{
		CURSOR_ST.InitCursor();

		CAMERA_ST.SaveCameraState();	// 기존 카메라 상태 저장

		if( g_pGameIF )
		{
			g_pGameIF->ResetMap();
			g_pGameIF->DeleteWindowUpdate();
			g_pGameIF->HideWindow();		// 기존 UI 상태 저장 및 숨김.
		}

		m_pDataTradeCenter = NiNew CDataTradeCenter;

		if( m_pDataTradeCenter && m_pDataTradeCenter->Construct() )
			m_pDataTradeCenter->Init();
		
		m_pFadeUI = NiNew CFade;

		if( m_pFadeUI )
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);
		
		bool	bOpen = true;
		GAME_EVENT_ST.OnEvent(EVENT_CODE::OPEN_TRADE_SELECT, &bOpen );

		return TRUE;
	}

	void CDataTradeCenterFlow::OnEnter(void)
	{		
		CFlow::OnEnter();
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_DATA_TRADE_SYSTEM);
	}

	void CDataTradeCenterFlow::OnExit(int p_iNextFlowID)
	{
		CFlow::OnExit(p_iNextFlowID);
		CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_DATA_TRADE_SYSTEM);
	}
	//---------------------------------------------------------------------------
	void CDataTradeCenterFlow::Terminate()
	{
		SAFE_NIDELETE(m_pFadeUI);
		SAFE_NIDELETE(m_pDataTradeCenter);

		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DATA_TRADE_SELECT ); 
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DATA_TRADE_DIGIMON_SCALE ); 
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DATA_TRADE_DIGIMON_UPGRADE ); 
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DATA_TRADE_DIGIMON_EVOLUTION );

		g_pGameIF->HideWindowRestore();	// 기존 UI 상태 복구.

		CAMERA_ST.LoadCameraState();	// 카메라 상태 복구
		RESOURCEMGR_ST.CleanUpResource();
	}

	void CDataTradeCenterFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	void CDataTradeCenterFlow::ReservedPopFlow()
	{
		if( m_pFadeUI )
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CDataTradeCenterFlow::RenderSceneFrame()
	{// 3D Draw
		switch( g_pEngine->GetPostEffect() )
		{
		case CEngine::PE_NONE:
		case CEngine::PE_GAUSSIAN_BLUR:
			{
				if(m_pDataTradeCenter)
					m_pDataTradeCenter->Render();
				g_pEngine->Render();
			}
			break;
		}
	}

	bool CDataTradeCenterFlow::BeginRenderTarget()
	{
		// BeginTarget
		return g_pEngine->BeginUsingCurrentRenderTargetGroup( NiRenderer::CLEAR_ALL );
	}

	//---------------------------------------------------------------------------
	bool CDataTradeCenterFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CDataTradeCenterFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		if(m_pDataTradeCenter)
			m_pDataTradeCenter->ResetDevice();
		return true;
	}

	BOOL CDataTradeCenterFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);
		GLOBALINPUT_ST._Keyboard_Input( p_kMsg );	

		return FALSE;
	}

	//---------------------------------------------------------------------------
	void CDataTradeCenterFlow::InputFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CDataTradeCenterFlow::UpdateFrame()
	{
		CONTENTSSYSTEM_PTR->Update( g_fDeltaTime );

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
			cGlobalInput::eMOUSE_INPUT mi = g_pGameIF->Update(g_fDeltaTime);		// 새로운 함수 만들어서 처리하면 좋겠다..
			if(m_pDataTradeCenter)
				m_pDataTradeCenter->Update(g_fDeltaTime, mi );
		}

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
	void CDataTradeCenterFlow::RenderUIFrame()
	{
		g_pEngine->ScreenSpace();

		if( g_pGameIF )
			g_pGameIF->Render();

		if( TOOLTIPMNG_STPTR )
			TOOLTIPMNG_STPTR->Render();

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