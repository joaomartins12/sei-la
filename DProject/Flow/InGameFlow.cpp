//---------------------------------------------------------------------------
//
// 파일명 : InGameFlow.cpp
// 작성일 :
// 작성자 :
// 설  명 :
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "InGameFlow.h"

#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"

//---------------------------------------------------------------------------

namespace
{
	static int g_nInGameFrameIndex = 0;

	/*
		Diagnóstico:
		O crash atual acontece dentro de g_pGameIF->Update().
		Vamos bloquear temporariamente os updates da UI principal nos primeiros frames
		para deixar o gameworld entrar e confirmar que o problema está numa janela/event handler.
	*/
	static const int GAMEIF_UPDATE_DELAY_FRAMES = 120;

	void IGLog(const char* pszText)
	{
		if (pszText)
		{
			OutputDebugStringA("[FLOW][InGame] ");
			OutputDebugStringA(pszText);
			OutputDebugStringA("\n");
		}
	}

	void IGLogInt(const char* pszText, int nValue)
	{
		char szBuffer[256] = { 0, };
		sprintf_s(szBuffer, 256, "[FLOW][InGame] %s%d\n", pszText ? pszText : "", nValue);
		OutputDebugStringA(szBuffer);
	}

	void IGLogFrameStage(const char* pszStage)
	{
		if (g_nInGameFrameIndex <= 180 && pszStage)
		{
			char szBuffer[256] = { 0, };
			sprintf_s(szBuffer, 256, "[FLOW][InGame][Frame %d] %s\n", g_nInGameFrameIndex, pszStage);
			OutputDebugStringA(szBuffer);
		}
	}

	bool SafePostLoad(cData_PostLoad* pPostLoad, bool bTutorial)
	{
		if (pPostLoad == NULL)
		{
			IGLog("SafePostLoad failed - pPostLoad NULL");
			return false;
		}

		__try
		{
			if (bTutorial)
			{
				IGLog("SafePostLoad - PostLoad_Tutorial begin");
				pPostLoad->PostLoad_Tutorial();
				IGLog("SafePostLoad - PostLoad_Tutorial end");
			}
			else
			{
				IGLog("SafePostLoad - PostLoad begin");
				pPostLoad->PostLoad();
				IGLog("SafePostLoad - PostLoad end");
			}

			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] SafePostLoad - PostLoad/PostLoad_Tutorial crashed and was skipped");
			return false;
		}
	}
}

//---------------------------------------------------------------------------

namespace Flow
{
	CInGameFlow::CInGameFlow(int p_iID)
		: CFlow(p_iID)
		, m_bBgmPlay(false)
		, m_pFadeUI(NULL)
	{
		g_nInGameFrameIndex = 0;

		IGLog("Constructor");
		IGLogInt("FlowID=", p_iID);
	}

	//---------------------------------------------------------------------------

	CInGameFlow::~CInGameFlow()
	{
		IGLog("Destructor");
	}

	//---------------------------------------------------------------------------

	BOOL CInGameFlow::Initialize()
	{
		IGLog("Initialize begin");

		if (g_pEngine == NULL)
		{
			IGLog("Initialize failed - g_pEngine NULL");
			return FALSE;
		}

		CURSOR_ST.InitCursor();
		IGLog("Initialize - cursor ok");

		if (g_pDataMng)
		{
			IGLog("Initialize - PostLoad begin");

			cData_PostLoad* pPostLoad = g_pDataMng->GetPostLoad();

			if (pPostLoad)
			{
				bool bTutorialPostLoad = false;

				if (g_pMngCollector && g_pMngCollector->IsSceneState(CMngCollector::eSCENE_TUTORIAL_EVENT))
					bTutorialPostLoad = true;

				IGLog("Initialize - SafePostLoad begin");

				if (SafePostLoad(pPostLoad, bTutorialPostLoad))
				{
					IGLog("Initialize - SafePostLoad ok");
				}
				else
				{
					IGLog("Initialize warning - SafePostLoad failed/skipped");
				}
			}
			else
			{
				IGLog("Initialize warning - pPostLoad NULL");
			}

			IGLog("Initialize - PostLoad block end");
		}
		else
		{
			IGLog("Initialize warning - g_pDataMng NULL");
		}

		m_pFadeUI = NiNew CFade;

		if (m_pFadeUI)
		{
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);
			IGLog("Initialize - Fade init ok");
		}
		else
		{
			IGLog("Initialize warning - Fade allocation failed");
		}

		IGLog("Initialize ok");
		return TRUE;
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::OnEnter(void)
	{
		IGLog("OnEnter begin");

		CFlow::OnEnter();

		IGLog("OnEnter after CFlow::OnEnter");

		if (g_pGameIF)
		{
			IGLog("OnEnter - g_pGameIF exists");

			if (g_pGameIF->GetMiniMap())
			{
				g_pGameIF->GetMiniMap()->SetShowWindow(true);
				IGLog("OnEnter - MiniMap show ok");
			}
			else
			{
				IGLog("OnEnter warning - MiniMap NULL");
			}

			if (g_pGameIF->GetQuestHelper())
			{
				g_pGameIF->GetQuestHelper()->SetShowWindow(true);
				IGLog("OnEnter - QuestHelper show ok");
			}
			else
			{
				IGLog("OnEnter warning - QuestHelper NULL");
			}

#ifdef SIMPLEFICATION
			g_pGameIF->GetDynamicIF(cBaseWindow::WT_SIMPLE_BTN, 0, 0, false);

			if (g_pGameIF->GetSimple())
			{
				g_pGameIF->GetSimple()->SetShowWindow(true);
				IGLog("OnEnter - Simple show ok");
			}
			else
			{
				IGLog("OnEnter warning - Simple NULL");
			}
#endif
		}
		else
		{
			IGLog("OnEnter warning - g_pGameIF NULL before world creation");
		}

		if (CONTENTSSYSTEM_PTR)
		{
			__try
			{
				IGLog("OnEnter - MakeWorldData begin");
				CONTENTSSYSTEM_PTR->MakeWorldData();
				IGLog("OnEnter - MakeWorldData end");
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				IGLog("[EXCEPTION] OnEnter - MakeWorldData");
			}

			__try
			{
				IGLog("OnEnter - MakeMainActorData begin");
				CONTENTSSYSTEM_PTR->MakeMainActorData();
				IGLog("OnEnter - MakeMainActorData end");
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				IGLog("[EXCEPTION] OnEnter - MakeMainActorData");
			}
		}
		else
		{
			IGLog("OnEnter warning - CONTENTSSYSTEM_PTR NULL, world/main actor skipped");
		}

		IGLog("OnEnter end");
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::OnExit(int p_iNextFlowID)
	{
		IGLogInt("OnExit begin. nextFlowID=", p_iNextFlowID);

		CFlow::OnExit(p_iNextFlowID);

		if (CONTENTSSYSTEM_PTR)
		{
			__try
			{
				IGLog("OnExit - ClearWorldData begin");
				CONTENTSSYSTEM_PTR->ClearWorldData();
				IGLog("OnExit - ClearWorldData end");
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				IGLog("[EXCEPTION] OnExit - ClearWorldData");
			}

			__try
			{
				IGLog("OnExit - ClearMainActorData begin");
				CONTENTSSYSTEM_PTR->ClearMainActorData();
				IGLog("OnExit - ClearMainActorData end");
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				IGLog("[EXCEPTION] OnExit - ClearMainActorData");
			}
		}
		else
		{
			IGLog("OnExit warning - CONTENTSSYSTEM_PTR NULL");
		}

		__try
		{
			CAMERA_ST.RestOcclusionGeometry();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] OnExit - CAMERA_ST.RestOcclusionGeometry");
		}

		IGLog("OnExit end");
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::OnOverride(int p_iNextFlowID)
	{
		IGLogInt("OnOverride. nextFlowID=", p_iNextFlowID);

		SetPaused(true);

		if (CONTENTSSYSTEM_PTR)
		{
			__try
			{
				CONTENTSSYSTEM_PTR->ClearMainActorData();
				CONTENTSSYSTEM_PTR->OnOverride(p_iNextFlowID);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				IGLog("[EXCEPTION] OnOverride - CONTENTSSYSTEM_PTR");
			}
		}
		else
		{
			IGLog("OnOverride warning - CONTENTSSYSTEM_PTR NULL");
		}
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::OnResume(int p_iNextFlowID)
	{
		IGLogInt("OnResume begin. nextFlowID=", p_iNextFlowID);

		CURSOR_ST.InitCursor();

		if (p_iNextFlowID == CFlow::FLW_LOADING)
		{
			if (g_pDataMng)
			{
				cData_PostLoad* pPostLoad = g_pDataMng->GetPostLoad();

				if (pPostLoad)
				{
					if (g_pMngCollector && g_pMngCollector->IsSceneState(CMngCollector::eSCENE_TUTORIAL_EVENT))
						pPostLoad->PostLoad_Tutorial();
					else
						pPostLoad->PostLoad();
				}
			}
		}

		SetPaused(false);

		if (p_iNextFlowID != CFlow::FLW_BATTLE_RESULT)
		{
			if (g_pGameIF)
			{
				if (g_pGameIF->GetMiniMap())
					g_pGameIF->GetMiniMap()->SetShowWindow(true);

				if (g_pGameIF->GetQuestHelper())
					g_pGameIF->GetQuestHelper()->SetShowWindow(true);

#ifdef SIMPLEFICATION
				g_pGameIF->GetDynamicIF(cBaseWindow::WT_SIMPLE_BTN, 0, 0, false);

				if (g_pGameIF->GetSimple())
					g_pGameIF->GetSimple()->SetShowWindow(true);
#endif
			}
			else
			{
				IGLog("OnResume warning - g_pGameIF NULL");
			}
		}

		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_IN, 0.5f);

		if (CONTENTSSYSTEM_PTR)
		{
			__try
			{
				CONTENTSSYSTEM_PTR->MakeMainActorData();
				CONTENTSSYSTEM_PTR->OnResume(p_iNextFlowID);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				IGLog("[EXCEPTION] OnResume - CONTENTSSYSTEM_PTR");
			}
		}
		else
		{
			IGLog("OnResume warning - CONTENTSSYSTEM_PTR NULL");
		}

		IGLog("OnResume end");
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		IGLogInt("ReservedChangeFlow. nextFlowID=", p_iNextFlowID);

		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::ReservedPushFlow(int p_iNextFlowID)
	{
		IGLogInt("ReservedPushFlow. nextFlowID=", p_iNextFlowID);

		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::Terminate()
	{
		IGLog("Terminate begin");

		nsCsGBTerrain::g_nSvrLibType = nLIB::SVR_GAME;

		CURSOR_ST.InitCursor();

		SAFE_NIDELETE(m_pFadeUI);
		IGLog("Terminate - Fade deleted");

		if (g_pDataMng)
		{
			IGLog("Terminate - g_pDataMng LogOut begin");
			g_pDataMng->LogOut();
			IGLog("Terminate - g_pDataMng LogOut end");
		}

		if (g_pMngCollector)
		{
			IGLog("Terminate - collector cleanup begin");
			g_pMngCollector->ReleaseScene();
			g_pMngCollector->EndDatsCenter();
			g_pMngCollector->ResetMap();
			IGLog("Terminate - collector cleanup end");
		}

		SAFE_NIDELETE(g_pGameIF);
		IGLog("Terminate - g_pGameIF deleted");

		if (g_pHelpArrow)
			g_pHelpArrow->ReleaseArrowPoint();

		if (g_pThread)
			g_pThread->GetResMng()->ReleaseConnetTerrain();

		if (nsCsGBTerrain::g_pTRMng)
			nsCsGBTerrain::g_pTRMng->DeleteRoot(&nsCsGBTerrain::g_pCurRoot);

		if (g_pNpcMng)
			g_pNpcMng->ResetOldMap();

		net::stop();
		net::prev_map_no = 0;

		if (g_pResist)
			g_pResist->SaveChar();

		if (OPTIONMNG_PTR)
			OPTIONMNG_PTR->SaveCharOption(OPTIONFILE_NAME);

#ifdef USE_BARCODE_REDER
		nsBARCODE::LogOut();
#endif

		g_bFirstLoding = true;

		RESOURCEMGR_ST.CleanUpResource();

		IGLog("Terminate end");
	}

	//---------------------------------------------------------------------------

	bool CInGameFlow::LostDevice(void* p_pvData)
	{
		IGLog("LostDevice");
		return true;
	}

	//---------------------------------------------------------------------------

	bool CInGameFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		IGLogInt("ResetDevice. beforeReset=", p_bBeforeReset ? 1 : 0);
		return true;
	}

	//---------------------------------------------------------------------------

	BOOL CInGameFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		__try
		{
			CURSOR_ST.Input(p_kMsg);
			GLOBALINPUT_ST._Keyboard_Input(p_kMsg);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] OnMsgHandler");
		}

		return FALSE;
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::InputFrame()
	{
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::UpdateFrame()
	{
		++g_nInGameFrameIndex;

		IGLogFrameStage("UpdateFrame begin");

		__try
		{
			if (CONTENTSSYSTEM_PTR)
			{
				IGLogFrameStage("CONTENTSSYSTEM_PTR->Update begin");
				CONTENTSSYSTEM_PTR->Update(g_fDeltaTime);
				IGLogFrameStage("CONTENTSSYSTEM_PTR->Update end");
			}
			else
			{
				IGLogFrameStage("CONTENTSSYSTEM_PTR NULL");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] CONTENTSSYSTEM_PTR->Update");
		}

		__try
		{
			if (g_pSystemMsg)
			{
				IGLogFrameStage("g_pSystemMsg->Update begin");
				g_pSystemMsg->Update();
				IGLogFrameStage("g_pSystemMsg->Update end");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pSystemMsg->Update");
		}

		__try
		{
			if (g_pSkillMsg)
			{
				IGLogFrameStage("g_pSkillMsg->Update begin");
				g_pSkillMsg->Update();
				IGLogFrameStage("g_pSkillMsg->Update end");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pSkillMsg->Update");
		}

		__try
		{
			if (TOOLTIPMNG_STPTR)
			{
				IGLogFrameStage("TOOLTIPMNG_STPTR->Update begin");
				TOOLTIPMNG_STPTR->Update(g_fDeltaTime);
				IGLogFrameStage("TOOLTIPMNG_STPTR->Update end");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] TOOLTIPMNG_STPTR->Update");
		}

		__try
		{
			if (m_pFadeUI)
			{
				IGLogFrameStage("m_pFadeUI->Update begin");
				m_pFadeUI->Update(g_fDeltaTime);
				IGLogFrameStage("m_pFadeUI->Update end");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] m_pFadeUI->Update");
		}

#ifdef USE_BARCODE_REDER
		__try
		{
			IGLogFrameStage("nsBARCODE::Update begin");
			nsBARCODE::Update();
			IGLogFrameStage("nsBARCODE::Update end");
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] nsBARCODE::Update");
		}
#endif

		__try
		{
			if (net::game)
				nsCsGBTerrain::g_ServerTime.s_nServerTime = net::game->GetTimeTS();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] net::game->GetTimeTS");
		}

		cGlobalInput::eMOUSE_INPUT mi = (cGlobalInput::eMOUSE_INPUT)0;

		/*
			CRÍTICO:
			O teu crash atual acontece aqui:
				[FLOW][InGame][Frame 1] g_pGameIF->Update begin
				Critical error detected c0000374

			Por isso, durante os primeiros frames, saltamos a atualização da UI.
			Se o gameworld aparecer, a causa está dentro de alguma janela/event handler da UI.
		*/
		if (g_nInGameFrameIndex > GAMEIF_UPDATE_DELAY_FRAMES)
		{
			__try
			{
				if (g_pGameIF)
				{
					IGLogFrameStage("g_pGameIF->Update begin");
					mi = g_pGameIF->Update(g_fDeltaTime);
					IGLogFrameStage("g_pGameIF->Update end");

					IGLogFrameStage("GLOBALINPUT keyboard/mouse begin");
					GLOBALINPUT_ST.KeyboardUpdate();
					GLOBALINPUT_ST._Mouse_Input(mi);
					IGLogFrameStage("GLOBALINPUT keyboard/mouse end");
				}
				else
				{
					IGLogFrameStage("g_pGameIF NULL");
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				IGLog("[EXCEPTION] g_pGameIF->Update / GLOBALINPUT");
			}
		}
		else
		{
			IGLogFrameStage("g_pGameIF->Update skipped for startup stability");
		}

		__try
		{
			if (g_pMngCollector)
			{
				IGLogFrameStage("g_pMngCollector->Update begin");
				g_pMngCollector->Update(mi);
				IGLogFrameStage("g_pMngCollector->Update end");
			}
			else
			{
				IGLogFrameStage("g_pMngCollector NULL");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pMngCollector->Update");
		}

		__try
		{
			IGLogFrameStage("cIconMng::GlobalUpdate begin");
			cIconMng::GlobalUpdate();
			IGLogFrameStage("cIconMng::GlobalUpdate end");
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] cIconMng::GlobalUpdate");
		}

		__try
		{
			IGLogFrameStage("CAMERA_ST.Update begin");
			CAMERA_ST.Update(g_fDeltaTime);
			IGLogFrameStage("CAMERA_ST.Update end");
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] CAMERA_ST.Update");
		}

		__try
		{
			if (g_pWeather)
			{
				if (g_pCharMng && g_pCharMng->GetTamerUser() && g_pEngine)
				{
					IGLogFrameStage("g_pWeather update begin");
					g_pWeather->SetPos(g_pCharMng->GetTamerUser()->GetPos());
					g_pWeather->Update(g_fDeltaTime, g_pEngine->GetD3DView());
					IGLogFrameStage("g_pWeather update end");
				}
				else
				{
					IGLogFrameStage("weather skipped, missing TamerUser/Engine");
				}
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pWeather update");
		}

		__try
		{
			CURSOR_ST.LoopReset();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] CURSOR_ST.LoopReset");
		}

		IGLogFrameStage("UpdateFrame end");
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::CullFrame()
	{
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::RenderSceneFrame()
	{
		IGLogFrameStage("RenderSceneFrame begin");

		if (g_pEngine == NULL)
		{
			IGLogFrameStage("RenderSceneFrame skipped - g_pEngine NULL");
			return;
		}

		__try
		{
			if (g_pMngCollector)
			{
				IGLogFrameStage("g_pMngCollector render begin");

				switch (g_pEngine->GetPostEffect())
				{
				case CEngine::PE_NONE:
				case CEngine::PE_GAUSSIAN_BLUR:
				{
					if (nsCsGBTerrain::g_bShadowRender == true)
						g_pMngCollector->Render_Shadow(true);
					else
						g_pMngCollector->Render(true);
				}
				break;

				default:
					break;
				}

				IGLogFrameStage("g_pMngCollector render end");
			}
			else
			{
				IGLogFrameStage("g_pMngCollector NULL, scene render skipped");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pMngCollector->Render / Render_Shadow");
		}

		__try
		{
			IGLogFrameStage("g_pEngine->Render begin");
			g_pEngine->Render();
			IGLogFrameStage("g_pEngine->Render end");
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pEngine->Render");
		}

		IGLogFrameStage("RenderSceneFrame end");
	}

	//---------------------------------------------------------------------------

	bool CInGameFlow::BeginRenderTarget()
	{
		IGLogFrameStage("BeginRenderTarget begin");

		if (g_pEngine == NULL)
		{
			IGLog("[EXCEPTION] BeginRenderTarget - g_pEngine NULL");
			return false;
		}

		bool bResult = false;

		__try
		{
			bResult = g_pEngine->BeginUsingCurrentRenderTargetGroup(NiRenderer::CLEAR_ALL);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] BeginUsingCurrentRenderTargetGroup");
			return false;
		}

		IGLogFrameStage("BeginRenderTarget end");
		return bResult;
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::RenderBackScreenFrame()
	{
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::RenderScreenFrame()
	{
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::RenderUIFrame()
	{
		IGLogFrameStage("RenderUIFrame begin");

		if (g_pEngine == NULL)
		{
			IGLogFrameStage("RenderUIFrame skipped - g_pEngine NULL");
			return;
		}

		__try
		{
			IGLogFrameStage("g_pEngine->ScreenSpace begin");
			g_pEngine->ScreenSpace();
			IGLogFrameStage("g_pEngine->ScreenSpace end");
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pEngine->ScreenSpace");
			return;
		}

		__try
		{
			if (g_pMngCollector)
			{
				IGLogFrameStage("g_pMngCollector->Render_PostIF begin");
				g_pMngCollector->Render_PostIF();
				IGLogFrameStage("g_pMngCollector->Render_PostIF end");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pMngCollector->Render_PostIF");
		}

		/*
			Render da UI fica ativo.
			Apenas o Update da UI foi atrasado, porque é onde o crash foi confirmado.
		*/
		__try
		{
			if (g_pGameIF)
			{
				IGLogFrameStage("g_pGameIF->Render begin");
				g_pGameIF->Render();
				IGLogFrameStage("g_pGameIF->Render end");
			}
			else
			{
				IGLogFrameStage("g_pGameIF NULL");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pGameIF->Render");
		}

		__try
		{
			if (g_pSystemMsg)
			{
				IGLogFrameStage("g_pSystemMsg->Render begin");
				g_pSystemMsg->Render();
				IGLogFrameStage("g_pSystemMsg->Render end");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pSystemMsg->Render");
		}

		__try
		{
			if (g_pSkillMsg)
			{
				IGLogFrameStage("g_pSkillMsg->Render begin");
				g_pSkillMsg->Render();
				IGLogFrameStage("g_pSkillMsg->Render end");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] g_pSkillMsg->Render");
		}

		__try
		{
			if (g_pGameIF)
			{
				cBaseWindow* pWin = g_pGameIF->_GetPointer(cBaseWindow::WT_PUBLICITY, 0);

				if (pWin && pWin->IsShowWindow())
					pWin->Render();
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] WT_PUBLICITY render");
		}

		__try
		{
			if (TOOLTIPMNG_STPTR)
			{
				IGLogFrameStage("TOOLTIPMNG_STPTR->Render begin");
				TOOLTIPMNG_STPTR->Render();
				IGLogFrameStage("TOOLTIPMNG_STPTR->Render end");
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] TOOLTIPMNG_STPTR->Render");
		}

		__try
		{
			cMessageBox::RenderList();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] cMessageBox::RenderList");
		}

#ifndef _GIVE
		/*
			Desativado temporariamente por causa dos crashes anteriores no Text.cpp.
		*/
		// __try
		// {
		// 	if (g_pMngCollector)
		// 		g_pMngCollector->RenderText();
		// }
		// __except (EXCEPTION_EXECUTE_HANDLER)
		// {
		// 	IGLog("[EXCEPTION] g_pMngCollector->RenderText");
		// }
#endif

		__try
		{
			CURSOR_ST.Render();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] CURSOR_ST.Render");
		}

		__try
		{
			if (m_pFadeUI)
				m_pFadeUI->Render();
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			IGLog("[EXCEPTION] m_pFadeUI->Render");
		}

		IGLogFrameStage("RenderUIFrame end");
	}

	//---------------------------------------------------------------------------

	BOOL CInGameFlow::LoadResource()
	{
		IGLog("LoadResource");
		return TRUE;
	}

	//---------------------------------------------------------------------------

	void CInGameFlow::ReleaseResource()
	{
		IGLog("ReleaseResource");
	}
}

//---------------------------------------------------------------------------