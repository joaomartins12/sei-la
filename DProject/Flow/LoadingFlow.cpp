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
		OutputDebugStringA("[FLOW][Loading] Constructor\n");
	}

	//---------------------------------------------------------------------------
	CLoadingFlow::~CLoadingFlow()
	{
		OutputDebugStringA("[FLOW][Loading] Destructor\n");
	}

	//---------------------------------------------------------------------------
	BOOL CLoadingFlow::Initialize()
	{
		OutputDebugStringA("[FLOW][Loading] Initialize begin\n");

		try
		{
			OutputDebugStringA("[FLOW][Loading] Initialize - cursor/input begin\n");

			CURSOR_ST.InitCursor();
			GLOBALINPUT_ST.ReleaseCameraRotation();

			OutputDebugStringA("[FLOW][Loading] Initialize - cursor/input ok\n");

			/*
				IMPORTANTE:
				Este bloco é perigoso durante a transição CharacterSelect -> Loading.
				Se g_pGameIF existir mas estiver parcialmente inicializado ou já destruído,
				ResetMap/PreResetMap/DeleteWindowUpdate podem rebentar ou tentar alocar memória absurda.
			*/
			if (g_pGameIF)
			{
				OutputDebugStringA("[FLOW][Loading] Initialize - g_pGameIF cleanup begin\n");

				try
				{
					OutputDebugStringA("[FLOW][Loading] Initialize - before g_pGameIF->ResetMap\n");
					g_pGameIF->ResetMap();
					OutputDebugStringA("[FLOW][Loading] Initialize - after g_pGameIF->ResetMap\n");

					OutputDebugStringA("[FLOW][Loading] Initialize - before g_pGameIF->PreResetMap\n");
					g_pGameIF->PreResetMap();
					OutputDebugStringA("[FLOW][Loading] Initialize - after g_pGameIF->PreResetMap\n");

					OutputDebugStringA("[FLOW][Loading] Initialize - before g_pGameIF->DeleteWindowUpdate\n");
					g_pGameIF->DeleteWindowUpdate();
					OutputDebugStringA("[FLOW][Loading] Initialize - after g_pGameIF->DeleteWindowUpdate\n");
				}
				catch (const std::bad_alloc&)
				{
					OutputDebugStringA("[FLOW][Loading] Initialize - bad_alloc inside g_pGameIF cleanup\n");
					return FALSE;
				}
				catch (...)
				{
					OutputDebugStringA("[FLOW][Loading] Initialize - exception inside g_pGameIF cleanup\n");
					return FALSE;
				}

				OutputDebugStringA("[FLOW][Loading] Initialize - g_pGameIF cleanup ok\n");
			}
			else
			{
				OutputDebugStringA("[FLOW][Loading] Initialize - g_pGameIF NULL, cleanup skipped\n");
			}

			OutputDebugStringA("[FLOW][Loading] Initialize - fade allocation begin\n");

			m_pFadeUI = NiNew CFade;

			if (m_pFadeUI)
			{
				m_pFadeUI->Init(CFade::FADE_IN, 0.5f);
				OutputDebugStringA("[FLOW][Loading] Initialize - fade init ok\n");
			}
			else
			{
				OutputDebugStringA("[FLOW][Loading] Initialize - fade allocation failed/null\n");
			}

			if (g_pSoundMgr)
			{
				OutputDebugStringA("[FLOW][Loading] Initialize - DeleteMusic begin\n");
				g_pSoundMgr->DeleteMusic(false);
				OutputDebugStringA("[FLOW][Loading] Initialize - DeleteMusic ok\n");
			}

			if (m_pLoadingUI == NULL)
			{
				OutputDebugStringA("[FLOW][Loading] Initialize - CLoading allocation begin\n");
				m_pLoadingUI = NiNew CLoading;
				OutputDebugStringA("[FLOW][Loading] Initialize - CLoading allocation end\n");
			}

			if (!m_pLoadingUI)
			{
				OutputDebugStringA("[FLOW][Loading] Initialize failed: m_pLoadingUI NULL\n");
				return FALSE;
			}

			OutputDebugStringA("[FLOW][Loading] Initialize - CLoading::Construct begin\n");

			if (!m_pLoadingUI->Construct())
			{
				OutputDebugStringA("[FLOW][Loading] Initialize failed: CLoading::Construct failed\n");
				SAFE_NIDELETE(m_pLoadingUI);
				return FALSE;
			}

			OutputDebugStringA("[FLOW][Loading] Initialize - CLoading::Construct ok\n");

			OutputDebugStringA("[FLOW][Loading] Initialize - CLoading::Create begin\n");

			try
			{
				m_pLoadingUI->Create();
			}
			catch (const std::bad_alloc&)
			{
				OutputDebugStringA("[FLOW][Loading] Initialize - bad_alloc inside CLoading::Create\n");
				SAFE_NIDELETE(m_pLoadingUI);
				return FALSE;
			}
			catch (...)
			{
				OutputDebugStringA("[FLOW][Loading] Initialize - exception inside CLoading::Create\n");
				SAFE_NIDELETE(m_pLoadingUI);
				return FALSE;
			}

			OutputDebugStringA("[FLOW][Loading] Initialize - CLoading::Create ok\n");

			if (g_pEngine)
			{
				g_pEngine->SetClearColor(NiColorA::BLACK);
				OutputDebugStringA("[FLOW][Loading] Initialize - SetClearColor ok\n");
			}

			OutputDebugStringA("[FLOW][Loading] Initialize ok\n");
			return TRUE;
		}
		catch (const std::bad_alloc&)
		{
			OutputDebugStringA("[FLOW][Loading] Initialize failed: std::bad_alloc outer catch\n");
			return FALSE;
		}
		catch (...)
		{
			OutputDebugStringA("[FLOW][Loading] Initialize failed: unknown outer exception\n");
			return FALSE;
		}
	}

	//---------------------------------------------------------------------------
	void CLoadingFlow::OnEnter(void)
	{
		OutputDebugStringA("[FLOW][Loading] OnEnter begin\n");

		/*
			Mesma correção que fizemos no ServerSelect:
			primeiro inicializa o ContentsSystem, depois deixa o CFlow chamar Initialize().
		*/
		if (CONTENTSSYSTEM_PTR)
		{
			OutputDebugStringA("[FLOW][Loading] OnEnter - InitializeContents begin\n");
			CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_LOADING);
			OutputDebugStringA("[FLOW][Loading] OnEnter - InitializeContents end\n");
		}
		else
		{
			OutputDebugStringA("[FLOW][Loading] OnEnter - CONTENTSSYSTEM_PTR NULL\n");
		}

		CFlow::OnEnter();

		if (CONTENTSSYSTEM_PTR)
		{
			OutputDebugStringA("[FLOW][Loading] OnEnter - EnterContents begin\n");
			CONTENTSSYSTEM_PTR->EnterContents(eContentsType::E_CT_LOADING);
			OutputDebugStringA("[FLOW][Loading] OnEnter - EnterContents end\n");
		}

		OutputDebugStringA("[FLOW][Loading] OnEnter end\n");
	}

	//---------------------------------------------------------------------------
	void CLoadingFlow::OnExit(int p_iNextFlowID)
	{
		OutputDebugStringA("[FLOW][Loading] OnExit begin\n");

		if (CONTENTSSYSTEM_PTR)
		{
			OutputDebugStringA("[FLOW][Loading] OnExit - ExitContents begin\n");
			CONTENTSSYSTEM_PTR->ExitContents(eContentsType::E_CT_LOADING);
			OutputDebugStringA("[FLOW][Loading] OnExit - ExitContents end\n");
		}

		CFlow::OnExit(p_iNextFlowID);

		OutputDebugStringA("[FLOW][Loading] OnExit end\n");
	}

	//---------------------------------------------------------------------------
	void CLoadingFlow::Terminate()
	{
		OutputDebugStringA("[FLOW][Loading] Terminate begin\n");

		SAFE_NIDELETE(m_pLoadingUI);
		OutputDebugStringA("[FLOW][Loading] Terminate - m_pLoadingUI deleted\n");

		SAFE_NIDELETE(m_pFadeUI);
		OutputDebugStringA("[FLOW][Loading] Terminate - m_pFadeUI deleted\n");

		RESOURCEMGR_ST.CleanUpResource();
		OutputDebugStringA("[FLOW][Loading] Terminate - CleanUpResource done\n");

		OutputDebugStringA("[FLOW][Loading] Terminate end\n");
	}

	//---------------------------------------------------------------------------
	void CLoadingFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		OutputDebugStringA("[FLOW][Loading] ReservedChangeFlow\n");

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
		OutputDebugStringA("[FLOW][Loading] LoadResource\n");
		return TRUE;
	}

	//---------------------------------------------------------------------------
	void CLoadingFlow::ReleaseResource()
	{
		OutputDebugStringA("[FLOW][Loading] ReleaseResource\n");
	}
}
//---------------------------------------------------------------------------