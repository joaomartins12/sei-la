//---------------------------------------------------------------------------
//
// 파일명 : CharacterCreateFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "CharacterCreateFlow.h"

#include "../_Interface/07.CharacterCreate/CharacterCreate.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace
{
	void CCFlowLog(const char* pszText)
	{
		if (pszText)
		{
			OutputDebugStringA("[FLOW][CharacterCreate] ");
			OutputDebugStringA(pszText);
			OutputDebugStringA("\n");
		}
	}

	void CCFlowLogInt(const char* pszText, int nValue)
	{
		char szBuffer[256] = { 0, };
		sprintf_s(szBuffer, 256, "[FLOW][CharacterCreate] %s%d\n", pszText ? pszText : "", nValue);
		OutputDebugStringA(szBuffer);
	}
}

//---------------------------------------------------------------------------

namespace Flow
{
	CCharacterCreateFlow::CCharacterCreateFlow(int p_iID)
		: CFlow(p_iID)
		, m_bBgmPlay(false)
		, m_pFadeUI(NULL)
		, m_pCharCreate(NULL)
	{
		CCFlowLog("Constructor");
	}
	//---------------------------------------------------------------------------
	CCharacterCreateFlow::~CCharacterCreateFlow()
	{
		CCFlowLog("Destructor");
	}
	//---------------------------------------------------------------------------
	BOOL CCharacterCreateFlow::Initialize()
	{
		CCFlowLog("Initialize begin");

		m_pCharCreate = NiNew CCharacterCreate;

		if (!m_pCharCreate)
		{
			CCFlowLog("Initialize failed - CCharacterCreate allocation failed");
			return FALSE;
		}

		CCFlowLog("Initialize - CCharacterCreate allocated");

		if (!m_pCharCreate->Construct())
		{
			CCFlowLog("Initialize failed - CCharacterCreate::Construct failed");
			SAFE_NIDELETE(m_pCharCreate);
			return FALSE;
		}

		CCFlowLog("Initialize - CCharacterCreate::Construct ok");

		if (!m_pCharCreate->Init())
		{
			CCFlowLog("Initialize failed - CCharacterCreate::Init failed");
			SAFE_NIDELETE(m_pCharCreate);
			return FALSE;
		}

		CCFlowLog("Initialize - CCharacterCreate::Init ok");

		m_pFadeUI = NiNew CFade;

		if (m_pFadeUI)
		{
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);
			CCFlowLog("Initialize - Fade init ok");
		}
		else
		{
			CCFlowLog("Initialize warning - Fade allocation failed");
		}

		if (g_pSoundMgr)
		{
			g_pSoundMgr->PlayMusic("Game_opening.mp3", 0.0f);
			CCFlowLog("Initialize - PlayMusic ok");
		}
		else
		{
			CCFlowLog("Initialize warning - g_pSoundMgr is NULL");
		}

		if (g_pEngine)
		{
			g_pEngine->SetGaussianBlurVal(0.9f, 0.25f, 0.3f);
			CCFlowLog("Initialize - GaussianBlur ok");
		}
		else
		{
			CCFlowLog("Initialize warning - g_pEngine is NULL");
		}

		CCFlowLog("Initialize ok");

		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::OnEnter(void)
	{
		CCFlowLog("OnEnter begin");

		CFlow::OnEnter();

		CCFlowLog("OnEnter after CFlow::OnEnter");

		if (CONTENTSSYSTEM_PTR)
		{
			CCFlowLog("OnEnter - InitializeContents begin");
			CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_CHARACTER_CREATE);
			CCFlowLog("OnEnter - InitializeContents end");

			CCFlowLog("OnEnter - EnterContents begin");
			CONTENTSSYSTEM_PTR->EnterContents(eContentsType::E_CT_CHARACTER_CREATE);
			CCFlowLog("OnEnter - EnterContents end");
		}
		else
		{
			CCFlowLog("OnEnter warning - CONTENTSSYSTEM_PTR is NULL");
		}

		CCFlowLog("OnEnter end");
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::OnExit(int p_iNextFlowID)
	{
		CCFlowLogInt("OnExit begin. nextFlow=", p_iNextFlowID);

		if (CONTENTSSYSTEM_PTR)
		{
			CCFlowLog("OnExit - ExitContents begin");
			CONTENTSSYSTEM_PTR->ExitContents(eContentsType::E_CT_CHARACTER_CREATE);
			CCFlowLog("OnExit - ExitContents end");
		}
		else
		{
			CCFlowLog("OnExit warning - CONTENTSSYSTEM_PTR is NULL");
		}

		CFlow::OnExit(p_iNextFlowID);

		CCFlowLog("OnExit end");
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		CCFlowLogInt("ReservedChangeFlow. nextFlow=", p_iNextFlowID);

		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::ReservedPopFlow()
	{
		CCFlowLog("ReservedPopFlow");

		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::Terminate()
	{
		CCFlowLog("Terminate begin");

		SAFE_NIDELETE(m_pCharCreate);
		CCFlowLog("Terminate - m_pCharCreate deleted");

		SAFE_NIDELETE(m_pFadeUI);
		CCFlowLog("Terminate - m_pFadeUI deleted");

		RESOURCEMGR_ST.CleanUpResource();
		CCFlowLog("Terminate - CleanUpResource done");

		CCFlowLog("Terminate end");
	}
	//---------------------------------------------------------------------------
	bool CCharacterCreateFlow::LostDevice(void* p_pvData)
	{
		CCFlowLog("LostDevice");
		return true;
	}
	//---------------------------------------------------------------------------
	bool CCharacterCreateFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		CCFlowLog("ResetDevice");
		return true;
	}
	//---------------------------------------------------------------------------
	BOOL CCharacterCreateFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if (cMessageBox::UpdateList())
			return FALSE;

		if (m_pCharCreate)
			m_pCharCreate->UpdateKeyboard(p_kMsg);

		return FALSE;
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::InputFrame()
	{
		if (cMessageBox::UpdateList())
			return;

		if (m_pCharCreate)
			m_pCharCreate->UpdateMouse();
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::UpdateFrame()
	{
		static int s_nUpdateLogCounter = 0;
		if (++s_nUpdateLogCounter >= 120)
		{
			s_nUpdateLogCounter = 0;
			CCFlowLog("UpdateFrame alive");
		}

		if (CONTENTSSYSTEM_PTR)
			CONTENTSSYSTEM_PTR->Update((float)g_fDeltaTime);
		else
			CCFlowLog("UpdateFrame warning - CONTENTSSYSTEM_PTR is NULL");

		if (g_pSystemMsg)
			g_pSystemMsg->Update();

		if (m_pCharCreate)
			m_pCharCreate->Update(g_fDeltaTime);
		else
			CCFlowLog("UpdateFrame warning - m_pCharCreate is NULL");

		if (m_pFadeUI)
			m_pFadeUI->Update(g_fDeltaTime);

		if (TOOLTIPMNG_STPTR)
			TOOLTIPMNG_STPTR->Update(g_fDeltaTime);

		CURSOR_ST.LoopReset();
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::CullFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::RenderSceneFrame()
	{
		static int s_nRenderSceneLogCounter = 0;
		if (++s_nRenderSceneLogCounter >= 120)
		{
			s_nRenderSceneLogCounter = 0;
			CCFlowLog("RenderSceneFrame alive");
		}

		if (g_pEngine)
			g_pEngine->ResetRendererCamera();
		else
			CCFlowLog("RenderSceneFrame warning - g_pEngine is NULL");

		if (m_pCharCreate)
			m_pCharCreate->Render3DModel();
		else
			CCFlowLog("RenderSceneFrame warning - m_pCharCreate is NULL");

		if (g_pEngine)
			g_pEngine->Render();
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::RenderBackScreenFrame()
	{
		static int s_nRenderBackLogCounter = 0;
		if (++s_nRenderBackLogCounter >= 120)
		{
			s_nRenderBackLogCounter = 0;
			CCFlowLog("RenderBackScreenFrame alive");
		}

		if (m_pCharCreate)
			m_pCharCreate->RenderScreenUI();
		else
			CCFlowLog("RenderBackScreenFrame warning - m_pCharCreate is NULL");
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::RenderScreenFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::RenderUIFrame()
	{
		static int s_nRenderUILogCounter = 0;
		if (++s_nRenderUILogCounter >= 120)
		{
			s_nRenderUILogCounter = 0;
			CCFlowLog("RenderUIFrame alive");
		}

		if (g_pEngine)
			g_pEngine->ScreenSpace();
		else
			CCFlowLog("RenderUIFrame warning - g_pEngine is NULL");

		if (m_pCharCreate)
			m_pCharCreate->Render();
		else
			CCFlowLog("RenderUIFrame warning - m_pCharCreate is NULL");

		if (TOOLTIPMNG_STPTR)
			TOOLTIPMNG_STPTR->Render();

		if (g_pSystemMsg)
			g_pSystemMsg->Render();

		cMessageBox::RenderList();

		if (m_pFadeUI)
			m_pFadeUI->Render();
	}
	//---------------------------------------------------------------------------
	BOOL CCharacterCreateFlow::LoadResource()
	{
		CCFlowLog("LoadResource");
		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CCharacterCreateFlow::ReleaseResource()
	{
		CCFlowLog("ReleaseResource");
	}
}
//---------------------------------------------------------------------------