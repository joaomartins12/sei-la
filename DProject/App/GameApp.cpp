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
#include "../../../LibProj/CsFilePack/CsFilePackSystem.h"

#include <vector>
#include <list>
#include <string>
#include <algorithm>

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

namespace CharacterCreateLobbyMapCache
{
#define CHARACTER_CREATE_STARTUP_MAIN_MAP_NIF "Data\\map\\realworld_r\\04_datsunderground\\04\\map_data\\04.nif"
#define CHARACTER_CREATE_STARTUP_MAIN_MAP_NIF_FALLBACK "Data\\map\\realworld_r\\04_datsunderground\\04\\04.nif"
#define CHARACTER_CREATE_STARTUP_OBJECT_DATA_ROOT "Data\\map\\realworld_r\\04_datsunderground\\04\\object_data"
#define CHARACTER_CREATE_STARTUP_DECORATION_ROOT "Data\\map\\realworld_r\\04_datsunderground\\04\\object_data\\decoration"

	static CsNodeObj g_StartupScene;
	static bool g_bStartupSceneLoaded = false;
	static bool g_bStartupTried = false;
	static std::vector<CsNodeObj*> g_vecStartupObjects;

	void Log(const char* pszText)
	{
		if (pszText)
		{
			OutputDebugStringA("[STARTUP][CharacterCreateMap] ");
			OutputDebugStringA(pszText);
			OutputDebugStringA("\n");
		}
	}

	void NormalizePackPath(std::string& kPath)
	{
		std::replace(kPath.begin(), kPath.end(), '/', '\\');

		for (size_t i = 0; i < kPath.size(); ++i)
		{
			if (kPath[i] >= 'A' && kPath[i] <= 'Z')
				kPath[i] = (char)(kPath[i] - 'A' + 'a');
		}
	}

	bool StartsWith(std::string const& kPath, std::string const& kPrefix)
	{
		if (kPath.size() < kPrefix.size())
			return false;

		return kPath.compare(0, kPrefix.size(), kPrefix) == 0;
	}

	bool EndsWithNif(std::string const& kPath)
	{
		if (kPath.size() < 4)
			return false;

		std::string kTail = kPath.substr(kPath.size() - 4);
		NormalizePackPath(kTail);

		return kTail == ".nif";
	}

	bool IsSelectedDecorationNif(std::string const& kNormalizedPath)
	{
		static char const* s_szAllowed[] =
		{
			"tutorial_decoration_1.nif",
			"tutorial_decoration_ani_1.nif",
			"tutorial_floor_1.nif",
			"tutorial_floor_1_1.nif",
			"tutorial_floor_1_g.nif",
			"tutorial_floor_2.nif",
			"tutorial_floor_2_g.nif",
			"tutorial_pipe_1.nif",
			"tutorial_wall_1.nif",
			"tutorial_wall_1_1.nif"
		};

		std::string kDecorationRoot = CHARACTER_CREATE_STARTUP_DECORATION_ROOT;
		NormalizePackPath(kDecorationRoot);

		if (!kDecorationRoot.empty() && kDecorationRoot[kDecorationRoot.size() - 1] != '\\')
			kDecorationRoot += "\\";

		if (!StartsWith(kNormalizedPath, kDecorationRoot))
			return false;

		for (int i = 0; i < (int)(sizeof(s_szAllowed) / sizeof(s_szAllowed[0])); ++i)
		{
			std::string kExpected = kDecorationRoot + s_szAllowed[i];
			NormalizePackPath(kExpected);

			if (kNormalizedPath == kExpected)
				return true;
		}

		return false;
	}

	bool LoadNifToSceneObject(char const* pszPath, CsNodeObj& kOutObj, NiPoint3 const& kTranslate)
	{
		if (!pszPath || pszPath[0] == 0)
			return false;

		NiStream kStream;

		if (!kStream.Load(pszPath))
			return false;

		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		if (!pNode)
			return false;

		nsCSGBFUNC::InitAnimation(pNode, NiTimeController::APP_INIT, NiTimeController::LOOP);
		nsCSGBFUNC::Set_Emittance(pNode, NiColor(1.0f, 1.0f, 1.0f));

		kOutObj.SetNiObject(pNode, CGeometry::Normal);

		if (kOutObj.m_pNiNode)
		{
			kOutObj.m_pNiNode->SetTranslate(kTranslate);
			NiTimeController::StartAnimations(kOutObj.m_pNiNode, (float)g_fAccumTime);
			kOutObj.m_pNiNode->UpdateEffects();
			kOutObj.m_pNiNode->Update(0.0f);
		}

		return true;
	}

	bool LoadNifToNewSceneObject(char const* pszPath)
	{
		if (!pszPath || pszPath[0] == 0)
			return false;

		CsNodeObj* pObj = NiNew CsNodeObj;
		if (!pObj)
			return false;

		if (!LoadNifToSceneObject(pszPath, *pObj, NiPoint3::ZERO))
		{
			pObj->Delete();
			NiDelete pObj;
			return false;
		}

		g_vecStartupObjects.push_back(pObj);
		return true;
	}

	void Release()
	{
		g_StartupScene.Delete();

		for (size_t i = 0; i < g_vecStartupObjects.size(); ++i)
		{
			CsNodeObj* pObj = g_vecStartupObjects[i];
			if (pObj)
			{
				pObj->Delete();
				NiDelete pObj;
			}
		}

		g_vecStartupObjects.clear();
		g_bStartupSceneLoaded = false;
	}

	int LoadSelectedDecorationFromPackIndex(int nPackIndex)
	{
		std::list<std::string> kFiles;
		CsFPS::CsFPSystem::GetFileList(nPackIndex, kFiles);

		char szLog[512] = { 0, };
		sprintf_s(szLog, 512, "PackIndex=%d FileList count=%d", nPackIndex, (int)kFiles.size());
		Log(szLog);

		if (kFiles.empty())
			return 0;

		std::string kObjectRoot = CHARACTER_CREATE_STARTUP_OBJECT_DATA_ROOT;
		NormalizePackPath(kObjectRoot);

		if (!kObjectRoot.empty() && kObjectRoot[kObjectRoot.size() - 1] != '\\')
			kObjectRoot += "\\";

		int nFound = 0;
		int nLoaded = 0;
		int nFailed = 0;
		int nSampleLogged = 0;

		for (std::list<std::string>::iterator it = kFiles.begin(); it != kFiles.end(); ++it)
		{
			std::string kOriginal = *it;
			std::string kNormalized = kOriginal;
			NormalizePackPath(kNormalized);

			if (!StartsWith(kNormalized, kObjectRoot))
				continue;

			if (!EndsWithNif(kNormalized))
				continue;

			if (!IsSelectedDecorationNif(kNormalized))
				continue;

			++nFound;

			if (LoadNifToNewSceneObject(kOriginal.c_str()))
			{
				++nLoaded;

				if (nSampleLogged < 20)
				{
					char szSample[MAX_PATH + 256] = { 0, };
					sprintf_s(szSample, sizeof(szSample), "loaded pack nif=%s", kOriginal.c_str());
					Log(szSample);
					++nSampleLogged;
				}
			}
			else
			{
				++nFailed;

				if (nFailed <= 20)
				{
					char szFail[MAX_PATH + 256] = { 0, };
					sprintf_s(szFail, sizeof(szFail), "failed pack nif=%s", kOriginal.c_str());
					Log(szFail);
				}
			}
		}

		sprintf_s(
			szLog,
			512,
			"PackIndex=%d selected decoration found=%d loaded=%d failed=%d",
			nPackIndex,
			nFound,
			nLoaded,
			nFailed
		);
		Log(szLog);

		return nLoaded;
	}

	bool Load()
	{
		if (g_bStartupSceneLoaded && !g_vecStartupObjects.empty())
		{
			Log("Load skipped - already loaded");
			return true;
		}

		g_bStartupTried = true;

		Release();

		Log("Load begin - DatsUnderground 04 main map NIF + selected decoration NIFs");
		Log("Load main map NIF begin: " CHARACTER_CREATE_STARTUP_MAIN_MAP_NIF);

		if (!LoadNifToSceneObject(CHARACTER_CREATE_STARTUP_MAIN_MAP_NIF, g_StartupScene, NiPoint3::ZERO))
		{
			Log("map_data\\04.nif failed, trying fallback root\\04.nif");
			Log("Fallback main map NIF begin: " CHARACTER_CREATE_STARTUP_MAIN_MAP_NIF_FALLBACK);

			if (!LoadNifToSceneObject(CHARACTER_CREATE_STARTUP_MAIN_MAP_NIF_FALLBACK, g_StartupScene, NiPoint3::ZERO))
			{
				Log("Load failed - could not load main map04 NIF from both paths");
				Release();
				return false;
			}
		}

		g_bStartupSceneLoaded = true;
		Log("main map NIF loaded");

		int nObjectLoaded = LoadSelectedDecorationFromPackIndex(0);

		char szLog[256] = { 0, };
		sprintf_s(szLog, 256, "selected decoration pack01 total loaded=%d", nObjectLoaded);
		Log(szLog);

		Log("Load ok - startup cache ready");
		return true;
	}

	bool IsLoaded()
	{
		return (g_bStartupSceneLoaded && !g_vecStartupObjects.empty());
	}

	void Update()
	{
		if (g_bStartupSceneLoaded && g_StartupScene.m_pNiNode)
			g_StartupScene.m_pNiNode->Update((float)g_fAccumTime);

		for (size_t i = 0; i < g_vecStartupObjects.size(); ++i)
		{
			CsNodeObj* pObj = g_vecStartupObjects[i];
			if (pObj && pObj->m_pNiNode)
				pObj->m_pNiNode->Update((float)g_fAccumTime);
		}
	}

	void Render()
	{
		if (g_bStartupSceneLoaded && g_StartupScene.m_pNiNode)
			g_StartupScene.Render();

		for (size_t i = 0; i < g_vecStartupObjects.size(); ++i)
		{
			CsNodeObj* pObj = g_vecStartupObjects[i];
			if (pObj && pObj->m_pNiNode)
				pObj->Render();
		}
	}
}


namespace
{
	static const DWORD STARTUP_CHARACTER_CREATE_LOBBY_MAP_ID = 4;
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
		StartupLobbyLog("preload begin - GameApp startup cache");

		g_bStartupCharacterCreateLobbyMapLoaded = CharacterCreateLobbyMapCache::Load();

		if (g_bStartupCharacterCreateLobbyMapLoaded)
			StartupLobbyLog("preload ok - DatsUnderground 04 cache ready");
		else
			StartupLobbyLog("preload failed - CharacterCreate will fallback to Background.dds");
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

		// Pré-carrega o cenário do CharacterCreate no startup, depois do Pack/FileTable
		// já estarem prontos. O CharacterCreate apenas usa este cache, sem carregar mapa.
		StartupPreloadCharacterCreateLobbyMap();

		CREATE_SINGLETON(CToolTipMng);
		if (TOOLTIPMNG_STPTR)
			TOOLTIPMNG_ST.Init();

		net::start();

		FLOWMGR_ST.StartFlow(Flow::CFlow::FLW_LOGIN);

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
		CharacterCreateLobbyMapCache::Release();
		BHPRT("CharacterCreateLobbyMapCache::Release()");

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