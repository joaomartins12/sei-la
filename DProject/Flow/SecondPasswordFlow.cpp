//---------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "SecondPasswordFlow.h"
#include "../_Interface/04.SecondPassword/SecondPassword.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace
{
	void SPFlowLog(const char* pszText)
	{
		if (pszText)
		{
			OutputDebugStringA("[FLOW] ");
			OutputDebugStringA(pszText);
			OutputDebugStringA("\n");
		}
	}
}

//---------------------------------------------------------------------------

namespace Flow
{
	CSecondPasswordFlow::CSecondPasswordFlow(int p_iID)
		: CFlow(p_iID)
		, m_bBgmPlay(false)
		, m_pSecondPassword(NULL)
		, m_pFadeUI(NULL)
	{
		SPFlowLog("SecondPasswordFlow::Constructor");
	}
	//---------------------------------------------------------------------------
	CSecondPasswordFlow::~CSecondPasswordFlow()
	{
		SPFlowLog("SecondPasswordFlow::Destructor");
	}
	//---------------------------------------------------------------------------
	BOOL CSecondPasswordFlow::Initialize()
	{
		SPFlowLog("SecondPasswordFlow::Initialize begin");

		if (g_pSoundMgr)
			g_pSoundMgr->PlayMusic("Game_opening.mp3", 0.0f);

		m_pSecondPassword = NiNew cSecondPassword;

		if (!m_pSecondPassword)
		{
			SPFlowLog("SecondPasswordFlow::Initialize failed - m_pSecondPassword allocation failed");
			return FALSE;
		}

		if (!m_pSecondPassword->Construct())
		{
			SPFlowLog("SecondPasswordFlow::Initialize failed - cSecondPassword::Construct failed");
			SAFE_NIDELETE(m_pSecondPassword);
			return FALSE;
		}

		m_pSecondPassword->Init();

		m_pFadeUI = NiNew CFade;

		if (m_pFadeUI)
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);

		if (g_pEngine)
			g_pEngine->SetGaussianBlurVal(0.9f, 0.25f, 0.3f);

		SPFlowLog("SecondPasswordFlow::Initialize ok");

		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::OnEnter(void)
	{
		SPFlowLog("SecondPasswordFlow::OnEnter begin");

		CFlow::OnEnter();

		// IMPORTANTE:
		// Não chamar EnterContents aqui por agora.
		// Nesta source, o SecondPassword é uma tela de transição e Enter/ExitContents
		// pode impedir a passagem para ServerSelectFlow.
		if (CONTENTSSYSTEM_PTR)
			CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_SECOND_PASSWORD);

		SPFlowLog("SecondPasswordFlow::OnEnter end");
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::OnExit(int p_iNextFlowID)
	{
		SPFlowLog("SecondPasswordFlow::OnExit begin");

		// IMPORTANTE:
		// Não chamar ExitContents aqui por agora.
		// O comportamento original desta tela era apenas chamar CFlow::OnExit.
		CFlow::OnExit(p_iNextFlowID);

		SPFlowLog("SecondPasswordFlow::OnExit end");
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		SPFlowLog("SecondPasswordFlow::ReservedChangeFlow");

		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::Terminate()
	{
		SPFlowLog("SecondPasswordFlow::Terminate begin");

		SAFE_NIDELETE(m_pFadeUI);
		SAFE_NIDELETE(m_pSecondPassword);

		RESOURCEMGR_ST.CleanUpResource();

		SPFlowLog("SecondPasswordFlow::Terminate end");
	}
	//---------------------------------------------------------------------------
	BOOL CSecondPasswordFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if (cMessageBox::UpdateList())
			return FALSE;

		if (m_pSecondPassword)
			m_pSecondPassword->UpdateKeyboard(p_kMsg);

		return FALSE;
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::InputFrame()
	{
		if (cMessageBox::UpdateList())
			return;

		if (m_pSecondPassword)
			m_pSecondPassword->UpdateMouse();
	}
	//---------------------------------------------------------------------------
	bool CSecondPasswordFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CSecondPasswordFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::RenderBackScreenFrame()
	{
		if (m_pSecondPassword)
			m_pSecondPassword->RenderScreenUI();
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::RenderSceneFrame()
	{
		if (g_pEngine)
			g_pEngine->ResetRendererCamera();

		if (m_pSecondPassword)
			m_pSecondPassword->Render3DModel();

		if (g_pEngine)
			g_pEngine->Render();
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::UpdateFrame()
	{
		static int s_nLogCounter = 0;
		if (++s_nLogCounter == 120)
		{
			s_nLogCounter = 0;
			SPFlowLog("SecondPasswordFlow::UpdateFrame alive");
		}

		if (CONTENTSSYSTEM_PTR)
			CONTENTSSYSTEM_PTR->Update((float)g_fDeltaTime);

		if (g_pSystemMsg)
			g_pSystemMsg->Update();

		if (m_pSecondPassword)
		{
			m_pSecondPassword->Update();
			m_pSecondPassword->Update3DAccumtime(g_fAccumTime);
		}

		if (m_pFadeUI)
			m_pFadeUI->Update(g_fDeltaTime);

		if (TOOLTIPMNG_STPTR)
			TOOLTIPMNG_STPTR->Update(g_fDeltaTime);

		CURSOR_ST.LoopReset();
	}
	//---------------------------------------------------------------------------
	void CSecondPasswordFlow::RenderUIFrame()
	{
		if (g_pEngine)
			g_pEngine->ScreenSpace();

		if (m_pSecondPassword)
			m_pSecondPassword->Render();

		if (TOOLTIPMNG_STPTR)
			TOOLTIPMNG_STPTR->Render();

		if (g_pSystemMsg)
			g_pSystemMsg->Render();

		cMessageBox::RenderList();

		if (m_pFadeUI)
			m_pFadeUI->Render();
	}
}
//---------------------------------------------------------------------------