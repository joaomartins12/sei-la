//---------------------------------------------------------------------------
//
// 파일명 : CharacterSelectFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "CharacterSelectFlow.h"

#include "../_Interface/06.CharacterSelect/CharacterSelect.h"
#include "../_Interface/Game/Fade.h"
#include "../ContentsSystem/ContentsSystem.h"
#include "../MngCollector.h"

//---------------------------------------------------------------------------

namespace
{
	void CSFlowLog(const char* pszText)
	{
		if (pszText)
		{
			OutputDebugStringA("[FLOW][CharacterSelect] ");
			OutputDebugStringA(pszText);
			OutputDebugStringA("\n");
		}
	}

	void CSFlowLogInt(const char* pszText, int nValue)
	{
		char szBuffer[256] = { 0, };
		sprintf_s(szBuffer, 256, "[FLOW][CharacterSelect] %s%d\n", pszText ? pszText : "", nValue);
		OutputDebugStringA(szBuffer);
	}
	static const DWORD CHARACTER_CREATE_PRELOAD_MAP_ID = 105;

	bool CSFlowPreloadCharacterCreateMap105()
	{
		CSFlowLog("CharacterCreate map105 preload begin");

		if (!g_pMngCollector)
		{
			CSFlowLog("CharacterCreate map105 preload failed - g_pMngCollector is NULL");
			return false;
		}

		net::prev_map_no = 0;
		net::next_map_no = CHARACTER_CREATE_PRELOAD_MAP_ID;

		CSFlowLogInt("CharacterCreate map105 DeleteChar mapID=", (int)CHARACTER_CREATE_PRELOAD_MAP_ID);
		g_pMngCollector->DeleteChar(CHARACTER_CREATE_PRELOAD_MAP_ID);

		if (g_pThread && g_pThread->GetResMng())
		{
			CSFlowLog("CharacterCreate map105 ReleaseImmediatelyResource begin");
			g_pThread->GetResMng()->ReleaseImmediatelyResource();
			CSFlowLog("CharacterCreate map105 ReleaseImmediatelyResource end");

			CSFlowLog("CharacterCreate map105 ReleaseConnetTerrain begin");
			g_pThread->GetResMng()->ReleaseConnetTerrain();
			CSFlowLog("CharacterCreate map105 ReleaseConnetTerrain end");
		}
		else
		{
			CSFlowLog("CharacterCreate map105 warning - g_pThread/GetResMng is NULL before ReleaseConnetTerrain");
		}

		CSFlowLog("CharacterCreate map105 ResetMap begin");
		g_pMngCollector->ResetMap();
		CSFlowLog("CharacterCreate map105 ResetMap end");

		if (g_pThread && g_pThread->GetResMng())
		{
			CSFlowLog("CharacterCreate map105 ReleaseImmediatelyResource after ResetMap begin");
			g_pThread->GetResMng()->ReleaseImmediatelyResource();
			CSFlowLog("CharacterCreate map105 ReleaseImmediatelyResource after ResetMap end");
		}

		CSFlowLogInt("CharacterCreate map105 LoadTerrain begin mapID=", (int)CHARACTER_CREATE_PRELOAD_MAP_ID);
		g_pMngCollector->LoadTerrain(CHARACTER_CREATE_PRELOAD_MAP_ID);
		CSFlowLog("CharacterCreate map105 LoadTerrain end");

		if (g_pThread && g_pThread->GetResMng())
		{
			CSFlowLog("CharacterCreate map105 ApplyConnetTerrain begin");
			g_pThread->GetResMng()->ApplyConnetTerrain();
			CSFlowLog("CharacterCreate map105 ApplyConnetTerrain end");
		}
		else
		{
			CSFlowLog("CharacterCreate map105 warning - g_pThread/GetResMng is NULL before ApplyConnetTerrain");
		}

		CSFlowLogInt("CharacterCreate map105 LoadChar begin mapID=", (int)CHARACTER_CREATE_PRELOAD_MAP_ID);
		g_pMngCollector->LoadChar(CHARACTER_CREATE_PRELOAD_MAP_ID);
		CSFlowLog("CharacterCreate map105 LoadChar end");

		if (g_pThread && g_pThread->GetResMng())
		{
			CSFlowLog("CharacterCreate map105 ReleaseImmediatelyResource after LoadChar begin");
			g_pThread->GetResMng()->ReleaseImmediatelyResource();
			CSFlowLog("CharacterCreate map105 ReleaseImmediatelyResource after LoadChar end");
		}

		CSFlowLogInt("CharacterCreate map105 LoadCompleate begin mapID=", (int)CHARACTER_CREATE_PRELOAD_MAP_ID);
		g_pMngCollector->LoadCompleate(CHARACTER_CREATE_PRELOAD_MAP_ID);
		CSFlowLog("CharacterCreate map105 LoadCompleate end");

		CSFlowLog("CharacterCreate map105 preload ok");
		return true;
	}

}

//---------------------------------------------------------------------------

namespace Flow
{
	CCharacterSelectFlow::CCharacterSelectFlow(int p_iID)
		: CFlow(p_iID)
		, m_bBgmPlay(false)
		, m_pFadeUI(NULL)
		, m_pCharSelect(NULL)
	{
		CSFlowLog("Constructor");
		CSFlowLogInt("FlowID=", p_iID);
	}
	//---------------------------------------------------------------------------
	CCharacterSelectFlow::~CCharacterSelectFlow()
	{
		CSFlowLog("Destructor");
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::OnOverride(int p_iNextFlowID)
	{
		CSFlowLogInt("OnOverride. nextFlowID=", p_iNextFlowID);
		SetPaused(true);
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::OnResume(int p_iNextFlowID)
	{
		CSFlowLogInt("OnResume. nextFlowID=", p_iNextFlowID);

		SetPaused(false);

		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_IN, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
		CSFlowLogInt("ReservedChangeFlow. nextFlowID=", p_iNextFlowID);

		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::ReservedPushFlow(int p_iNextFlowID)
	{
		CSFlowLogInt("ReservedPushFlow. nextFlowID=", p_iNextFlowID);

		if (m_pFadeUI)
			m_pFadeUI->Reset(CFade::FADE_OUT, 0.5f);
	}
	//---------------------------------------------------------------------------
	BOOL CCharacterSelectFlow::Initialize()
	{
		CSFlowLog("Initialize begin");

		m_pCharSelect = NiNew CCharacterSelect;

		if (!m_pCharSelect)
		{
			CSFlowLog("Initialize failed - CCharacterSelect allocation failed");
			return FALSE;
		}

		CSFlowLog("Initialize - CCharacterSelect allocated");

		if (!m_pCharSelect->Construct())
		{
			CSFlowLog("Initialize failed - CCharacterSelect::Construct failed");
			SAFE_NIDELETE(m_pCharSelect);
			return FALSE;
		}

		CSFlowLog("Initialize - CCharacterSelect::Construct ok");

		m_pCharSelect->Init();

		CSFlowLog("Initialize - CCharacterSelect::Init ok");

		m_pFadeUI = NiNew CFade;

		if (m_pFadeUI)
		{
			m_pFadeUI->Init(CFade::FADE_IN, 0.5f);
			CSFlowLog("Initialize - Fade init ok");
		}
		else
		{
			CSFlowLog("Initialize warning - Fade allocation failed");
		}

		if (g_pSoundMgr)
		{
			g_pSoundMgr->PlayMusic("Game_opening.mp3", 0.0f);
			CSFlowLog("Initialize - PlayMusic ok");
		}
		else
		{
			CSFlowLog("Initialize warning - g_pSoundMgr is NULL");
		}

		if (g_pEngine)
		{
			g_pEngine->SetGaussianBlurVal(0.9f, 0.25f, 0.3f);
			CSFlowLog("Initialize - GaussianBlur ok");
		}
		else
		{
			CSFlowLog("Initialize warning - g_pEngine is NULL");
		}

		CSFlowLog("Initialize ok");

		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::OnEnter(void)
	{
		CSFlowLog("OnEnter begin");

		CFlow::OnEnter();

		CSFlowLog("OnEnter after CFlow::OnEnter");

		if (CONTENTSSYSTEM_PTR)
		{
			CSFlowLog("OnEnter - InitializeContents begin");
			CONTENTSSYSTEM_PTR->InitializeContents(eContentsType::E_CT_CHARACTER_SELECT);
			CSFlowLog("OnEnter - InitializeContents end");

			CSFlowLog("OnEnter - EnterContents begin");
			CONTENTSSYSTEM_PTR->EnterContents(eContentsType::E_CT_CHARACTER_SELECT);
			CSFlowLog("OnEnter - EnterContents end");
		}
		else
		{
			CSFlowLog("OnEnter warning - CONTENTSSYSTEM_PTR is NULL");
		}

		CSFlowLog("OnEnter end");
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::OnExit(int p_iNextFlowID)
	{
		CSFlowLogInt("OnExit begin. nextFlowID=", p_iNextFlowID);

		if (CONTENTSSYSTEM_PTR)
		{
			CSFlowLog("OnExit - ExitContents begin");
			CONTENTSSYSTEM_PTR->ExitContents(eContentsType::E_CT_CHARACTER_SELECT);
			CSFlowLog("OnExit - ExitContents end");
		}
		else
		{
			CSFlowLog("OnExit warning - CONTENTSSYSTEM_PTR is NULL");
		}

		CFlow::OnExit(p_iNextFlowID);

		CSFlowLog("OnExit end");
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::Terminate()
	{
		CSFlowLog("Terminate begin");

		SAFE_NIDELETE(m_pFadeUI);
		CSFlowLog("Terminate - m_pFadeUI deleted");

		SAFE_NIDELETE(m_pCharSelect);
		CSFlowLog("Terminate - m_pCharSelect deleted");

		RESOURCEMGR_ST.CleanUpResource();
		CSFlowLog("Terminate - CleanUpResource done");

		CSFlowLog("Terminate end");
	}
	//---------------------------------------------------------------------------
	bool CCharacterSelectFlow::LostDevice(void* p_pvData)
	{
		CSFlowLog("LostDevice");
		return true;
	}
	//---------------------------------------------------------------------------
	bool CCharacterSelectFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		CSFlowLogInt("ResetDevice. beforeReset=", p_bBeforeReset ? 1 : 0);
		return true;
	}
	//---------------------------------------------------------------------------
	BOOL CCharacterSelectFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		CURSOR_ST.Input(p_kMsg);

		if (cMessageBox::UpdateList())
			return FALSE;

		if (m_pCharSelect)
			m_pCharSelect->UpdateKeyboard(p_kMsg);

		return FALSE;
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::InputFrame()
	{
		if (cMessageBox::UpdateList())
			return;

		if (m_pCharSelect)
			m_pCharSelect->UpdateMouse();
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::UpdateFrame()
	{
		static int s_nUpdateLogCounter = 0;
		if (++s_nUpdateLogCounter >= 120)
		{
			s_nUpdateLogCounter = 0;
			CSFlowLog("UpdateFrame alive");
		}

		if (CONTENTSSYSTEM_PTR)
			CONTENTSSYSTEM_PTR->Update((float)g_fDeltaTime);
		else
			CSFlowLog("UpdateFrame warning - CONTENTSSYSTEM_PTR is NULL");

		if (g_pSystemMsg)
			g_pSystemMsg->Update();

		if (m_pCharSelect)
			m_pCharSelect->Update(g_fAccumTime, g_fDeltaTime);
		else
			CSFlowLog("UpdateFrame warning - m_pCharSelect is NULL");

		if (m_pFadeUI)
			m_pFadeUI->Update(g_fDeltaTime);

		if (TOOLTIPMNG_STPTR)
			TOOLTIPMNG_STPTR->Update(g_fDeltaTime);

		// Preload real do mapa 105 usado no CharacterCreate.
		// Foi movido do GameApp para aqui porque no GameApp era cedo demais.
		// Aqui o CharacterSelect já entrou, ContentSystem já inicializou/entrou,
		// engine/filetables/resources já estão prontos, e ainda estamos antes do clique no Create.
		static bool s_bMap105PreloadDone = false;
		static bool s_bMap105PreloadTried = false;
		static int s_nMap105PreloadDelayFrame = 0;

		if (!s_bMap105PreloadDone && !s_bMap105PreloadTried)
		{
			++s_nMap105PreloadDelayFrame;

			if (s_nMap105PreloadDelayFrame == 60)
				CSFlowLog("CharacterCreate map105 preload scheduled");

			if (s_nMap105PreloadDelayFrame > 120)
			{
				s_bMap105PreloadTried = true;

				if (CSFlowPreloadCharacterCreateMap105())
					s_bMap105PreloadDone = true;
				else
					CSFlowLog("CharacterCreate map105 preload failed");
			}
		}

		CURSOR_ST.LoopReset();
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::CullFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::RenderSceneFrame()
	{
		static int s_nRenderSceneLogCounter = 0;
		if (++s_nRenderSceneLogCounter >= 120)
		{
			s_nRenderSceneLogCounter = 0;
			CSFlowLog("RenderSceneFrame alive");
		}

		if (g_pEngine)
			g_pEngine->ResetRendererCamera();
		else
			CSFlowLog("RenderSceneFrame warning - g_pEngine is NULL");

		if (m_pCharSelect)
			m_pCharSelect->Render3DModel();
		else
			CSFlowLog("RenderSceneFrame warning - m_pCharSelect is NULL");

		if (g_pEngine)
			g_pEngine->Render();
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::RenderBackScreenFrame()
	{
		static int s_nRenderBackLogCounter = 0;
		if (++s_nRenderBackLogCounter >= 120)
		{
			s_nRenderBackLogCounter = 0;
			CSFlowLog("RenderBackScreenFrame alive");
		}

		if (m_pCharSelect)
			m_pCharSelect->RenderScreenUI();
		else
			CSFlowLog("RenderBackScreenFrame warning - m_pCharSelect is NULL");
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::RenderScreenFrame()
	{
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::RenderUIFrame()
	{
		static int s_nRenderUILogCounter = 0;
		if (++s_nRenderUILogCounter >= 120)
		{
			s_nRenderUILogCounter = 0;
			CSFlowLog("RenderUIFrame alive");
		}

		if (g_pEngine)
			g_pEngine->ScreenSpace();
		else
			CSFlowLog("RenderUIFrame warning - g_pEngine is NULL");

		if (m_pCharSelect)
			m_pCharSelect->Render();
		else
			CSFlowLog("RenderUIFrame warning - m_pCharSelect is NULL");

		if (TOOLTIPMNG_STPTR)
			TOOLTIPMNG_STPTR->Render();

		if (g_pSystemMsg)
			g_pSystemMsg->Render();

		cMessageBox::RenderList();

		if (m_pFadeUI)
			m_pFadeUI->Render();
	}
	//---------------------------------------------------------------------------
	BOOL CCharacterSelectFlow::LoadResource()
	{
		CSFlowLog("LoadResource");
		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CCharacterSelectFlow::ReleaseResource()
	{
		CSFlowLog("ReleaseResource");
	}
}
//---------------------------------------------------------------------------