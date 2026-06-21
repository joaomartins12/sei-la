//---------------------------------------------------------------------------
//
// 파일명 : GameApp.cpp
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------

#include "StdAfx.h"
#include "GameApp.h"

#include "../Flow/FlowMgr.h"
#include "../Flow/Flow.h"

#include "../ContentsSystem/ContentsSystemDef.h"
#include "../ContentsSystem/ContentsSystem.h"

#include "../MngCollector.h"

#ifdef DEF_CORE_NPROTECT
#include "../nProtect/Client_nProtect.h"
#define CHECK_NPROTECT_TIME 5.0f
#endif

CsCriticalSection	__g_GlobalCS;

int					__g_nGlobalState = 0;

void SetGlobalState(int nState)
{
	__g_GlobalCS.Lock();
	__g_nGlobalState = nState;
	__g_GlobalCS.Unlock();
}

int GetGlobalState()
{
	__g_GlobalCS.Lock();
	int nReturn = __g_nGlobalState;
	__g_GlobalCS.Unlock();
	return nReturn;
}

void Thread_LoadFileTable()
{
	// ==== 파일 테이블
	// 로드 안할 목록	
	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapResurrection = false;
	nsCsMapTable::g_eModeMapMonster = nsCsMapTable::eMode_Client;

	// 특별 로드 목록
	nsCsFileTable::g_bAddExp = true;
	nsCsFileTable::g_bUseMoveObject = true;
	nsCsFileTable::g_bUseHelp = true;
	nsCsFileTable::g_bUseAchieve = true;
	nsCsFileTable::g_bAddExp = true;
	nsCsFileTable::g_bBuffMng = true;
	nsCsFileTable::g_bSceneDataMng = true;

#ifndef BATTLE_MATCH
	nsCsFileTable::g_bUseEvoExtra = false;
	nsCsFileTable::g_bUseEvoBattle = false;
#endif

	nsCsFileTable::g_bUseCashShop = true;
	nsCsFileTable::g_bUseGotcha = true;

	if (GLOBALDATA_ST.IsCountry(GData::eCountry_Aeria))
		g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::ENGLISH_A;
	else if (GLOBALDATA_ST.IsCountry(GData::eCountry_GSP))
		g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::ENGLISH;
	else if (GLOBALDATA_ST.IsCountry(GData::eCountry_Steam))
		g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::ENGLISH;
	else if (GLOBALDATA_ST.IsCountry(GData::eCountry_Kor))
		g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::KOREA_TRANSLATION;
	else if (GLOBALDATA_ST.IsCountry(GData::eCountry_Hongkong))
		g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::HONGKONG;
	else if (GLOBALDATA_ST.IsCountry(GData::eCountry_Taiwan))
		g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::TAIWAN;
	else if (GLOBALDATA_ST.IsCountry(GData::eCountry_Thailand))
		g_pResist->m_Global.s_eFTLanguage = nsCsFileTable::THAILAND;

#ifdef _DEBUG
	if (g_bUseFilePack)
		nsCsFileTable::g_eFileType = nsCsFileTable::FT_FILEPACK;
	else
		nsCsFileTable::g_eFileType = nsCsFileTable::FT_BIN;
#elif defined _GIVE
	if (g_bUseFilePack)
		nsCsFileTable::g_eFileType = nsCsFileTable::FT_FILEPACK;
	else
		nsCsFileTable::g_eFileType = nsCsFileTable::FT_BIN;
#else
	// Release_English (non-DEBUG, non-GIVE): honor dmo.ini [DEBUG] FilePack=1 like the other configs.
	// Without this, FT_BIN is forced and managers try fopen on Data\Bin\<lang>\*.bin which only exist
	// inside Pack03, producing the "<path> 파일이 존재 하지 않습니다." popup.
	if (g_bUseFilePack)
		nsCsFileTable::g_eFileType = nsCsFileTable::FT_FILEPACK;
	else
		nsCsFileTable::g_eFileType = nsCsFileTable::FT_BIN;
#endif

#ifdef PC_BANG_SERVICE_TEST//PC방 테스트 관련 알파서버 접속 클라 생성
	nsCsFileTable::g_eFileType = nsCsFileTable::FT_FILEPACK;
#endif

	if (nsCsFileTable::g_FileTableMng.Init(nsCsFileTable::g_eFileType, g_pResist->m_Global.s_eFTLanguage) == false)
	{
		SetGlobalState(2);
		return;
	}

#ifndef _DEBUG
#ifndef _GIVE
#ifndef PC_BANG_SERVICE_TEST//PC방 테스트 관련 알파서버 접속 클라 생성
	// 위치찾기에 필요한 데이터 bin파일로 Reload
	// Skip when running from packs — Reload() fopen's bins from disk and they
	// only exist inside Pack03 in this build; the file-not-found assert would
	// otherwise cascade into the GameApp.cpp:117 bResult assert.
	if (nsCsFileTable::g_eFileType != nsCsFileTable::FT_FILEPACK)
	{
		char cPath[MAX_PATH];
		nsCsFileTable::g_FileTableMng.GetLanguagePath(g_pResist->m_Global.s_eFTLanguage, cPath);
		bool bResult = nsCsFileTable::g_pQuestMng->Reload(cPath);
		assert_cs(bResult);
		nsCsMapTable::g_pMapMonsterMng->Reload(cPath);
	}
#endif
#endif
#endif

	SetGlobalState(1);
}

bool IsUiTexture(char const* pName)
{
	assert_cs(pName[0] != '\\');

	bool bUiTex = false;

	if (pName[0] == '.')
		bUiTex = (strnicmp(&pName[2], "data\\Interface", 14) == 0);
	else
		bUiTex = (strnicmp(pName, "data\\Interface", 14) == 0);

	if (bUiTex)
		return (_access_s(pName, 0) == 0) ? true : false;

	return false;
}

NiFile* CsFilePackFileCreateFunc(const char* pcName, NiFile::OpenMode eMode, unsigned int uiBufferSize)
{
	//BHPRT( "Load File : %s", pcName );
#ifdef SDM_USER_UI_SKIN_CHANGE_20160331
	if (IsUiTexture(pcName))
		return NiNew NiFile(pcName, eMode, uiBufferSize);
#endif

	CsFPS::CsFileHash::sINFO* pHashInfo = CsFPS::CsFPSystem::GetHashData(0, pcName);
	if (pHashInfo == NULL)
		return NiNew NiFile(pcName, eMode, uiBufferSize);

	char* pBuffer = NiAlloc(char, pHashInfo->s_nDataSize);
	if (NULL == pBuffer)
	{
		//DUMPLOGA( "NiAlloc False : %s", pcName );
		//CsMessageBoxA( MB_OK, "NiAlloc Fail : %s, %d", pcName, pHashInfo->s_nDataSize );
		return NULL;
	}

	CsFPS::CsFPSystem::GetFileData(0, &pBuffer, pHashInfo->s_nOffset, pHashInfo->s_nDataSize);

	return NiNew NiMemFile(pBuffer, NiFile::READ_ONLY, pHashInfo->s_nDataSize);
}

bool CsFilePackFileAccessFunc(const char* pcName, NiFile::OpenMode eMode)
{
	assert_cs(pcName[0] != '\\');

	if (pcName[0] == '.')
		return (strnicmp(&pcName[2], "data\\", 5) == 0);

	return (strnicmp(pcName, "data\\", 5) == 0);
}

namespace
{
	static const DWORD STARTUP_CHARACTER_CREATE_LOBBY_MAP_ID = 105;
	static bool g_bStartupCharacterCreateLobbyMapLoaded = false;

	void StartupLobbyLog(const char* pszText)
	{
		if (pszText)
		{
			OutputDebugStringA("[STARTUP][LobbyMap] ");
			OutputDebugStringA(pszText);
			OutputDebugStringA("\n");
		}
	}

	void StartupLobbyLogInt(const char* pszText, int nValue)
	{
		char szBuffer[256] = { 0, };
		sprintf_s(szBuffer, 256, "[STARTUP][LobbyMap] %s%d\n", pszText ? pszText : "", nValue);
		OutputDebugStringA(szBuffer);
	}

	void StartupPreloadCharacterCreateLobbyMap()
	{
		// Desativado de propósito.
		//
		// O preload no GameApp::OnInitialize() acontece cedo demais:
		// - cGameInterface::GlobalInit() aqui rebenta em HotKey.cpp;
		// - sem GameInterface/flow carregado, o terrain 105 fica só com skybox/fundo branco.
		//
		// A solução segura é fazer o preload completo alguns frames depois de entrar no
		// CharacterSelectFlow, antes do jogador clicar em Create.
		OutputDebugStringA("[STARTUP][LobbyMap] skipped - preload moved to CharacterSelectFlow\n");
		g_bStartupCharacterCreateLobbyMapLoaded = false;
	}
}

namespace App
{
	//---------------------------------------------------------------------------
	CGameApp::CGameApp() : m_bInitialStarting(false), m_fCheckGameGuardTimer(0.0f)
	{
	}
	//---------------------------------------------------------------------------
	CGameApp::~CGameApp()
	{
	}
	//---------------------------------------------------------------------------
	BOOL CGameApp::OnInitialize()
	{
		CREATE_SINGLETON(GameEventMng);

		CREATE_SINGLETON(Flow::CFlowMgr);
		if (!FLOWMGR_ST.Create())
		{
			assert(!"Created FlowMgr Failed!");
			return FALSE;
		}

		CREATE_SINGLETON(COptionMng);	// 파일 매니저

		if (OPTIONMNG_PTR)
			OPTIONMNG_PTR->LoadMachineOption();

		CREATE_SINGLETON(ContentsSystem);
		SAFE_POINTER_RETVAL(CONTENTSSYSTEM_PTR, FALSE);

		if (CONTENTSSYSTEM_PTR)
		{
			CONTENTSSYSTEM_PTR->BuildContents();
			CONTENTSSYSTEM_PTR->IntraConnection();
		}

		CREATE_SINGLETON(ResourceMng);
		if (RESOURCEMGR_STPTR)
			RESOURCEMGR_ST.Init();

		CREATE_SINGLETON(cGlobalInput);

		CREATE_SINGLETON(CCursor);
		if (CURSOR_STPTR)
			CURSOR_ST.Init(m_hInstance);

		// 엔진의 렌더러 생성
		if (CEngine::Init() == false)
			return FALSE;

		DxResolutionInfo::GlobalInit();

		if (g_pEngine->Create() == false)
			return FALSE;

		g_pResist->m_Global.CheckResolution();
		CalculateSize(g_pResist->m_Global.s_nResolutionWidth, g_pResist->m_Global.s_nResolutionHeight, g_pResist->m_Global.s_bFullScreen);

		ReSize(GetWidth(), GetHeight(), GetFullMode(), true);

		g_nScreenWidth = GetWidth();
		g_nScreenHeight = GetHeight();

		g_pEngine->ChangeResolutionMode(GetFullMode(), GetWidth(), GetHeight());

		//CREATE_SINGLETON( CClock )
		//if(!CLOCK_ST.Create( NiNew CTimeSourceNi ) )
		//{
		//	assert(!"Created Clock Failed!");
		//	return FALSE;
		//}

		////------------------------------------------------
		//// Init Setup
		//CLOCK_ST.SetMaxFrameRate(0.0f);
		//CLOCK_ST.SetCheckFps(TRUE);

		if (g_bUseFilePack)
		{
			// 파일패킹 콜백, 엔진 생성 이후에
			//CsMessageBox(MB_OK, _T("Loading File packs"));
			NiFile::SetFileCreateFunc(CsFilePackFileCreateFunc);
			NiFile::SetFileAccessFunc(CsFilePackFileAccessFunc);

			/*AIL_set_file_callbacks( Sound_file_open_callback,
			Sound_file_close_callback,
			Sound_file_seek_callback,
			Sound_file_read_callback );*/
		}

		CMngCollector::ShotInit();

		// 쓰레드로 파일테이블 로드
		sTCUnit* pUnit = sTCUnit::NewInstance(sTCUnit::LoadFileTable);
		pUnit->s_pLoadedObject = NULL;
		g_pThread->LoadChar(pUnit);

		bool bLoad = true;
		while (bLoad)
		{
			switch (GetGlobalState())
			{
			case 0:
				Sleep(100);
				break;

			case 1:
				bLoad = false;
				break;

			case 2:
				bLoad = false;
				CsMessageBox(MB_OK, _LAN("파일테이블이 잘못 되었습니다"));
				return false;
			}
		}

		if (GAME_EVENT_STPTR)
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::TABLE_LOAD_SUCCESS);

		cIconMng::GlobalInit();
		cDataMng::GlobalInit();

		// 레지스트 설정된거 적용	
		nsCsGBTerrain::g_eTexFilter = (NiTexturingProperty::FilterMode)g_pResist->m_Global.s_eTexFilter;
		nsCsGBTerrain::g_bShadowRender = (g_pResist->m_Global.s_nShadowType == cResist::sGLOBAL::SHADOW_ON);
		nsCsGBTerrain::g_bCharOutLine = g_pResist->m_Global.s_bCharOutLine;
		nsCsGBTerrain::g_bSpeedCellRender = g_pResist->m_Global.s_bCell;
		g_pWeather->SetPerformance(g_pResist->m_Global.s_nWeather);
		CsC_AvObject::g_bEnableVoice = g_pResist->m_Global.s_bEnableVoice;

		// O preload real do mapa 105 foi movido para CharacterSelectFlow.
		// Aqui fica apenas um log/no-op para garantir que o GameApp já não tenta
		// carregar mundo cedo demais no ciclo do client.
		StartupPreloadCharacterCreateLobbyMap();

		CREATE_SINGLETON(CToolTipMng);
		if (TOOLTIPMNG_STPTR)
			TOOLTIPMNG_ST.Init();

		net::start();

#ifndef SKIP_LOGO
		FLOWMGR_ST.StartFlow(Flow::CFlow::FLW_LOGO);
#else
		FLOWMGR_ST.StartFlow(Flow::CFlow::FLW_LOGIN);
#endif

#ifdef DEF_CORE_NPROTECT
		m_fCheckGameGuardTimer = CHECK_NPROTECT_TIME;
#endif

		// 		if( GAME_EVENT_STPTR )
		// 			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::START_RESOURCECHECKER);

		return TRUE;
	}
	//---------------------------------------------------------------------------
	void CGameApp::OnIdleExtern()
	{
#ifdef VERSION_STEAM
		STEAM_ST.SteamCallbackUpdate();
#endif

		// 네트웍 업데이트
		net::execute();

#ifdef DEF_CORE_NPROTECT
		m_fCheckGameGuardTimer -= g_fDeltaTime;
		if (m_fCheckGameGuardTimer < 0.0f)
		{
			m_fCheckGameGuardTimer = CHECK_NPROTECT_TIME;
			nProtect_Check();
		}
#endif
	}

	//---------------------------------------------------------------------------
	void CGameApp::OnIdle()
	{
		// Flow Idle
		if (FLOWMGR_STPTR)
			FLOWMGR_ST.OnIdle();
	}
	//---------------------------------------------------------------------------
	void CGameApp::OnTerminate()
	{
		//	SAFE_NIDELETE( g_pLoading );

		if (TOOLTIPMNG_STPTR)
			DESTROY_SINGLETON(TOOLTIPMNG_STPTR);

		BHPRT("TOOLTIPMNG_STPTR");

		if (CONTENTSSYSTEM_PTR)
		{
			CONTENTSSYSTEM_PTR->ClearWorldData();
			CONTENTSSYSTEM_PTR->ClearMainActorData();
			BHPRT("CONTENTSSYSTEM_PTR");
		}

		if (FLOWMGR_STPTR)
		{
			FLOWMGR_STPTR->LockFlow();
			FLOWMGR_STPTR->Destroy();
			DESTROY_SINGLETON(FLOWMGR_STPTR);
			BHPRT("FLOWMGR_STPTR");
		}

		g_Sorting.Delete();
		BHPRT("g_Sorting.Delete()");

		SAFE_NIDELETE(g_pGameIF);

		BHPRT("g_pGameIF");

		if (CONTENTSSYSTEM_PTR)
		{
			CONTENTSSYSTEM_PTR->PrepareDestroy();
			CONTENTSSYSTEM_PTR->RemoveAll();
			DESTROY_SINGLETON(CONTENTSSYSTEM_PTR);
			BHPRT("CONTENTSSYSTEM_PTR");
		}

		if (OPTIONMNG_PTR)
			DESTROY_SINGLETON(OPTIONMNG_PTR);

		BHPRT("OPTIONMNG_PTR");

		if (CURSOR_STPTR)
			DESTROY_SINGLETON(CURSOR_STPTR);

		BHPRT("CURSOR_STPTR");

		cIconMng::GlobalShotdown();
		BHPRT("cIconMng::GlobalShotdown()");

		CMngCollector::ShotDown();
		BHPRT("CMngCollector::ShotDown()");

		DxResolutionInfo::GlobalShotDown();
		BHPRT("DxResolutionInfo::GlobalShotDown()");

		nsDIRECTSHOW::GlobalShotDown();
		BHPRT("nsDIRECTSHOW::GlobalShotDown()");

		cGameInterface::GlobalShotDown();
		BHPRT("cGameInterface::GlobalShotDown()");

		nsCsFileTable::g_FileTableMng.Delete();
		BHPRT("nsCsFileTable::g_FileTableMng.Delete()");

		if (CURSOR_STPTR)
			DESTROY_SINGLETON(CURSOR_STPTR);

		BHPRT("CURSOR_STPTR");

		if (GLOBALINPUT_STPTR)
			DESTROY_SINGLETON(GLOBALINPUT_STPTR);

		BHPRT("GLOBALINPUT_STPTR");

		// 		// Clock Destroy
		// 		if(CLOCK_STPTR)
		// 			CLOCK_ST.Destroy();
		// 		DESTROY_SINGLETON(CLOCK_STPTR);

		if (GAME_EVENT_STPTR)
			DESTROY_SINGLETON(GAME_EVENT_STPTR);

		BHPRT("GAME_EVENT_STPTR");

		if (RESOURCEMGR_STPTR)
		{
			RESOURCEMGR_STPTR->End();
			DESTROY_SINGLETON(RESOURCEMGR_STPTR);
		}
	}
	//---------------------------------------------------------------------------
	BOOL CGameApp::OnMsgHandler(const MSG& p_kMsg)
	{
		switch (p_kMsg.message)
		{
		case WM_CTLCOLOREDIT:
		{
			if (g_pGameIF)
			{
				if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_CHAT_MAIN_WINDOW))
				{
					ChatContents* pChatCon = (ChatContents*)CONTENTSSYSTEM_PTR->GetContents(E_CT_CHATTING_STANDARDIZATION);
					if (pChatCon && (HWND)p_kMsg.lParam == pChatCon->_GetEditHwnd())
					{
						return pChatCon->_ApplyEditColor((HDC)p_kMsg.wParam);
					}
				}

				if (g_pMoneySeparate)
				{
					LRESULT hr = g_pMoneySeparate->ApplyEditColor((HWND)p_kMsg.lParam, (HDC)p_kMsg.wParam);
					if (hr != NULL)
					{
						return hr;
					}
				}

				if (g_pItemSeparate)
				{
					LRESULT hr = g_pItemSeparate->ApplyEditColor((HWND)p_kMsg.lParam, (HDC)p_kMsg.wParam);
					if (hr != NULL)
					{
						return hr;
					}
				}

				if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_MASTERS_MATCHING))
				{
					LRESULT hr = g_pGameIF->GetMastersMatching()->ApplyEditColor((HWND)p_kMsg.lParam, (HDC)p_kMsg.wParam);
					if (hr != NULL)
					{
						return hr;
					}
				}
			}
		}
		break;

		case WM_DISPLAYCHANGE:
		{
			if (!m_bFullMode)
				::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_uiWindowModeWorkSpaceRect, 0);
		}
		break;
		}

		if (FLOWMGR_STPTR && FLOWMGR_ST.OnMsgHandler(p_kMsg))
			return TRUE;

		return FALSE;
	}
	//---------------------------------------------------------------------------
	bool CGameApp::LostDevice(void* p_pvData)
	{
		// Flow LostDevice
		if (FLOWMGR_STPTR && !FLOWMGR_ST.LostDevice(p_pvData))
			return false;

		return true;
	}
	//---------------------------------------------------------------------------
	bool CGameApp::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		// Flow ResetDevice
		if (FLOWMGR_STPTR && !FLOWMGR_ST.ResetDevice(p_bBeforeReset, p_pvData))
			return false;

		return true;
	}
	//---------------------------------------------------------------------------
	bool CGameApp::RecreateDevice(WORD wWidth, WORD wHeight, BYTE byBit, BYTE RefRate, bool bFullMode)
	{
		bool bScreenModeChanged = (static_cast<bool>(m_bFullMode) != bFullMode);

		CalculateSize(wWidth, wHeight, bFullMode);

		//if( GBRENDERER_STPTR )
		//	GBRENDERER_STPTR->Recreate( GetWidth(), GetHeight(), GetFullMode() );

		ReSize(GetWidth(), GetHeight(), GetFullMode(), bScreenModeChanged);

		// 		if( GBCAMERAMGR_STPTR )
		// 		{
		// 			GbCamera* pkGbCamera = NiNew GbCamera;
		// 			if(!pkGbCamera->Create(GetWidth(), GetHeight()))
		// 			{
		// 				assert(!"Created GbCamera Failed!");
		// 				return FALSE;
		// 			}
		// 
		// 			GbCamera* pkOldCamera = GBCAMERAMGR_STPTR->GetGbCamera(GAME_CAMERA);
		// 			pkGbCamera->SetPitch( pkOldCamera->GetPitch() );
		// 			pkGbCamera->SetYaw( pkOldCamera->GetYaw() );
		// 			pkGbCamera->SetRoll( pkOldCamera->GetRoll() );
		// 			pkGbCamera->SetHeight( pkOldCamera->GetHeight() );
		// 			pkGbCamera->SetDist( pkOldCamera->GetDist() );
		// 			pkGbCamera->SetOriginDist( pkOldCamera->GetOriginDist() );
		// 			pkGbCamera->SetDestDist( pkOldCamera->GetDestDist() );
		// 			pkGbCamera->SetSpinPitch( pkOldCamera->GetSpinPitch() );
		// 			pkGbCamera->SetSpinYaw( pkOldCamera->GetSpinYaw() );
		// 			pkGbCamera->SetSpinRoll( pkOldCamera->GetSpinRoll() );
		// 			pkGbCamera->Update();
		// 
		// 			GBCAMERAMGR_STPTR->DelGbCamera(GAME_CAMERA);
		// 			GBCAMERAMGR_STPTR->AddGbCamera(GAME_CAMERA, pkGbCamera);
		// 
		// 			//[11/5/2010 passion] 
		// 			// 캐릭터 셀렉트와 크레이트에서 사용하기위한 카메라
		// 			GbCamera* pkGbViewCamera = NiNew GbCamera;
		// 			if(!pkGbViewCamera->Create(GetWidth(), GetHeight()))
		// 			{
		// 				assert(!"Created GbCamera Failed!");
		// 				return FALSE;
		// 			}
		// 
		// 			pkGbCamera = GBCAMERAMGR_STPTR->GetGbCamera(VIEW_CAMERA);
		// 			pkGbViewCamera->SetPitch( pkGbCamera->GetPitch() );
		// 			pkGbViewCamera->SetYaw( pkGbCamera->GetYaw() );
		// 			pkGbViewCamera->SetRoll( pkGbCamera->GetRoll() );
		// 			pkGbViewCamera->SetHeight( pkGbCamera->GetHeight() );
		// 			pkGbViewCamera->SetDist( pkGbCamera->GetDist() );
		// 			pkGbViewCamera->SetOriginDist( pkGbCamera->GetOriginDist() );
		// 			pkGbViewCamera->SetDestDist( pkGbCamera->GetDestDist() );
		// 			pkGbViewCamera->SetSpinPitch( pkGbCamera->GetSpinPitch() );
		// 			pkGbViewCamera->SetSpinYaw( pkGbCamera->GetSpinYaw() );
		// 			pkGbViewCamera->SetSpinRoll( pkGbCamera->GetSpinRoll() );
		// 			pkGbViewCamera->Update();
		// 
		// 			GBCAMERAMGR_STPTR->DelGbCamera(VIEW_CAMERA);
		// 			GBCAMERAMGR_STPTR->AddGbCamera(VIEW_CAMERA, pkGbViewCamera);
		// 
		// 			//[8/24/2011 passion] 
		// 			// UI 카메라
		// 
		// 			if( pkGbCamera )
		// 			{
		// 				GbCamera* pkGbUICamera = NiNew GbCamera;
		// 				const NiRenderTargetGroup* pkRTGroup = 
		// 					NiRenderer::GetRenderer()->GetDefaultRenderTargetGroup();
		// 
		// 				UINT uiWidth = pkRTGroup->GetWidth(0);
		// 				UINT uiHeight = pkRTGroup->GetHeight(0);
		// 
		// 				if(!pkGbUICamera->Create(uiWidth, uiHeight ) )
		// 				{
		// 					assert(!"Created GbCamera Failed!");
		// 					return FALSE;
		// 				}
		// 
		// 				pkGbUICamera->SetOrtho(true);
		// 
		// 				NiFrustum m_kCachedFrustum;
		// 				float fAspectRatio = 0.75f;
		// 				float fXScale = static_cast<float>(uiWidth)/1024.0f ;
		// 				float fYScale = static_cast<float>(uiHeight)/768.0f ;
		// 
		// 				const float fDoubleWidth = 2.0f * (static_cast<float>(pkRTGroup->GetWidth(0)));
		// 				const float fDoubleHeight = 2.0f * (static_cast<float>(pkRTGroup->GetHeight(0)));
		// 				m_kCachedFrustum.m_fLeft = (-0.5f + 1.0f / fDoubleWidth)*fXScale;
		// 				m_kCachedFrustum.m_fRight = (0.5f + 1.0f / fDoubleWidth)*fXScale;
		// 				m_kCachedFrustum.m_fTop = (0.5f - 1.0f / fDoubleHeight)*fAspectRatio*fYScale;
		// 				m_kCachedFrustum.m_fBottom = (-0.5f - 1.0f / fDoubleHeight)*fAspectRatio*fYScale;
		// 				m_kCachedFrustum.m_fNear = 1.0f;
		// 				m_kCachedFrustum.m_fFar = 10000.0f;
		// 				m_kCachedFrustum.m_bOrtho = true;
		// 
		// 				pkGbUICamera->GetCamera()->SetViewFrustum(m_kCachedFrustum);
		// 				pkGbUICamera->Update();
		// 
		// 				GBCAMERAMGR_STPTR->DelGbCamera(UI_CAMERA);
		// 				GBCAMERAMGR_STPTR->AddGbCamera(UI_CAMERA, pkGbUICamera);
		// 
		// 				GUIMGR_STPTR->SetUiCamera(pkGbUICamera->GetCamera());
		// 			}
		// 		}
		return true;
	}
}

//---------------------------------------------------------------------------