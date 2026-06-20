//---------------------------------------------------------------------------
//
// 파일명 : LoadingFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "LoadingFlow.h"

#include "../_Interface/08.Loading/Loading.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CLoadingFlow::CLoadingFlow(int p_iID)
		: CFlow(p_iID)
		, m_bBgmPlay(false)
		, m_pFadeUI(NULL)
		, m_pLoadingUI(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CLoadingFlow::~CLoadingFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CLoadingFlow::Initialize()
	{
		CURSOR_ST.InitCursor();
		GLOBALINPUT_ST.ReleaseCameraRotation();

		if (g_pGameIF)
		{
			g_pGameIF->ResetMap();
			g_pGameIF->PreResetMap();
			g_pGameIF->DeleteWindowUpdate();
		}

		// 		if( g_pGameIF )
		// 		{
		// 			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_MATCH_START_COUNT, true );
		// 			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_MATCH_PLAY_COUNT, true );
		// 			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_MATCH_NEXT_COUNT, true );
		// 		}

		m_pFadeUI = NiNew CFade;

		if (m_pFadeUI)
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

		if (g_pSoundMgr)
			g_pSoundMgr->DeleteMusic(false);

		if (m_pLoadingUI == NULL)
			m_pLoadingUI = NiNew CLoading;

		if (!m_pLoadingUI)
			return FALSE;

		if (!m_pLoadingUI->Construct())
		{
			SAFE_NIDELETE(m_pLoadingUI);
			return FALSE;
		}

		m_pLoadingUI->Create();

		if (g_pEngine)
			g_pEngine->SetClearColor(NiColorA::BLACK);

		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::OnEnter(void)
	{
		CFlow::OnEnter();

		if (CONTENTSSYSTEM_PTR)
		{
			CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_LOADING);
			CONTENTSSYSTEM_PTR->EnterContents(eContentsType::E_CT_LOADING);
		}
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::OnExit(int p_iNextFlowID)
	{
		if (CONTENTSSYSTEM_PTR)
			CONTENTSSYSTEM_PTR->ExitContents(eContentsType::E_CT_LOADING);

		CFlow::OnExit(p_iNextFlowID);
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::Terminate()
	{
		SAFE_NIDELETE(m_pLoadingUI);
		SAFE_NIDELETE(m_pFadeUI);

		RESOURCEMGR_ST.CleanUpResource();
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	bool CLoadingFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CLoadingFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::InputFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::UpdateFrame()
	{
		if (CONTENTSSYSTEM_PTR)
			CONTENTSSYSTEM_PTR->Update(g_fDeltaTime);

		if (m_pLoadingUI)
			m_pLoadingUI->Update(g_fDeltaTime);

		if (m_pFadeUI)
			m_pFadeUI->Update(g_fDeltaTime);
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::CullFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::RenderSceneFrame()
	{
		if (g_pEngine)
			g_pEngine->Render();
	}
	//---------------------------------------------------------------------------
	BOOL CLoadingFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if (cMessageBox::UpdateList())
			return FALSE;

		return FALSE;
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::RenderBackScreenFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::RenderScreenFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::RenderUIFrame()
	{
		if (g_pEngine)
			g_pEngine->ScreenSpace();

		if (m_pLoadingUI)
			m_pLoadingUI->Render();

		cMessageBox::RenderList();

		if (m_pFadeUI)
			m_pFadeUI->Render();
	}
	//---------------------------------------------------------------------------
	BOOL CLoadingFlow::LoadResource()
	{
		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CLoadingFlow::ReleaseResource()
	{
	}
}
//---------------------------------------------------------------------------