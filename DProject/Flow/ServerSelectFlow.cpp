//---------------------------------------------------------------------------
//
// 파일명 : ServerSelectFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "ServerSelectFlow.h"
#include "../_Interface/05.ServerSelect/SelectServer.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace Flow
{
	CServerSelectFlow::CServerSelectFlow(int p_iID)
		: CFlow(p_iID)
		, m_bBgmPlay(false)
		, m_pServerSelect(NULL)
		, m_pFadeUI(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CServerSelectFlow::~CServerSelectFlow()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CServerSelectFlow::Initialize()
	{
		if (g_pSoundMgr)
			g_pSoundMgr->PlayMusic("Game_opening.mp3", 0.0f);

		m_pServerSelect = NiNew CSelectServer;

		if (!m_pServerSelect)
			return FALSE;

		if (!m_pServerSelect->Construct())
		{
			SAFE_NIDELETE(m_pServerSelect);
			return FALSE;
		}

		m_pServerSelect->Init();

		m_pFadeUI = NiNew CFade;

		if (m_pFadeUI)
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

		if (g_pEngine)
			g_pEngine->SetGaussianBlurVal(0.9f, 0.25f, 0.3f);

		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::OnEnter(void)
	{
		CFlow::OnEnter();

		if (CONTENTSSYSTEM_PTR)
		{
			CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_SERVER_SELECT);
			CONTENTSSYSTEM_PTR->EnterContents(eContentsType::E_CT_SERVER_SELECT);
		}
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::OnExit(int p_iNextFlowID)
	{
		if (CONTENTSSYSTEM_PTR)
			CONTENTSSYSTEM_PTR->ExitContents(eContentsType::E_CT_SERVER_SELECT);

		CFlow::OnExit(p_iNextFlowID);
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::Terminate()
	{
		SAFE_NIDELETE(m_pServerSelect);
		SAFE_NIDELETE(m_pFadeUI);

		RESOURCEMGR_ST.CleanUpResource();
	}
	//---------------------------------------------------------------------------
	bool CServerSelectFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CServerSelectFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	BOOL CServerSelectFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if (cMessageBox::UpdateList())
			return FALSE;

		if (m_pServerSelect)
			m_pServerSelect->UpdateKeyboard(p_kMsg);

		return FALSE;
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::InputFrame()
	{
		if (cMessageBox::UpdateList())
			return;

		if (m_pServerSelect)
			m_pServerSelect->UpdateMouse();
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::UpdateFrame()
	{
		if (CONTENTSSYSTEM_PTR)
			CONTENTSSYSTEM_PTR->Update((float)g_fDeltaTime);

		if (g_pSystemMsg)
			g_pSystemMsg->Update();

		if (m_pServerSelect)
			m_pServerSelect->Update3DAccumtime(g_fAccumTime);

		if (m_pFadeUI)
			m_pFadeUI->Update(g_fDeltaTime);

		if (TOOLTIPMNG_STPTR)
			TOOLTIPMNG_STPTR->Update(g_fDeltaTime);

		CURSOR_ST.LoopReset();
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::CullFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::RenderSceneFrame()
	{
		if (g_pEngine)
			g_pEngine->ResetRendererCamera();

		if (m_pServerSelect)
			m_pServerSelect->Render3DModel();

		if (g_pEngine)
			g_pEngine->Render();
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::RenderScreenFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::RenderBackScreenFrame()
	{
		if (m_pServerSelect)
			m_pServerSelect->RenderScreenUI();
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::RenderUIFrame()
	{
		if (g_pEngine)
			g_pEngine->ScreenSpace();

		if (m_pServerSelect)
			m_pServerSelect->Render();

		if (TOOLTIPMNG_STPTR)
			TOOLTIPMNG_STPTR->Render();

		if (g_pSystemMsg)
			g_pSystemMsg->Render();

		cMessageBox::RenderList();

		if (m_pFadeUI)
			m_pFadeUI->Render();
	}
	//---------------------------------------------------------------------------
	BOOL CServerSelectFlow::LoadResource()
	{
		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CServerSelectFlow::ReleaseResource()
	{
	}
}
//---------------------------------------------------------------------------