#include "stdafx.h"
#include "CharacterCreate.h"

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"
#include "../../MngCollector.h"
#include "../../../LibProj/CsFilePack/CsFilePackSystem.h"

#include <vector>
#include <list>
#include <string>
#include <algorithm>

namespace
{
	static const DWORD CHARACTER_CREATE_LOBBY_MAP_ID = 4;

	static bool g_bCharacterCreateLobbyMapPreloaded = false;
	static bool g_bCharacterCreateLobbyMapPreloadTried = false;

	// Carregamento direto do mapa 04 / DatsUnderground:
	// 1) carrega o NIF principal:
	//    Data\map\realworld_r\04_datsunderground\04\map_data\04.nif
	// 2) carrega também todos os .nif dentro de:
	//    Data\map\realworld_r\04_datsunderground\04\object_data\...
	//
	// Esta abordagem segue o mesmo estilo do DatsCenter: NiStream.Load(path exato),
	// usando o FilePack para resolver os ficheiros.
#define CHARACTER_CREATE_MAP_UNDERGROUND_ROOT "Data\\map\\realworld_r\\04_datsunderground\\04"
#define CHARACTER_CREATE_MAIN_MAP_NIF "Data\\map\\realworld_r\\04_datsunderground\\04\\map_data\\04.nif"
#define CHARACTER_CREATE_OBJECT_DATA_ROOT "Data\\map\\realworld_r\\04_datsunderground\\04\\object_data"

#define CHARACTER_CREATE_RENDER_MAIN_MAP_NIF 1
#define CHARACTER_CREATE_RENDER_OBJECT_DATA_ONLY 0
#define CHARACTER_CREATE_SKIP_BACKGROUND_OBJECTS 0

	static CsNodeObj g_CharacterCreateScene;
	static bool g_bCharacterCreateSceneLoaded = false;

	// Object_data do mapa 04 / DatsUnderground.
	static std::vector<CsNodeObj*> g_vecCharacterCreateMapObjects;

	// Posição ideal testada para o mapa 04 / DatsUnderground.
	// Coordenadas do jogo: X=11378, Y=19805, Z=0.00
	// Em NiPoint3/Gamebryo usamos: X=11378, Y(altura)=0, Z=19805.
	static const float CHARACTER_CREATE_MAP_START_X = 0.0f;
	static const float CHARACTER_CREATE_MAP_START_Z = 0.0f;

	// Ajuste temporário da câmera do CharacterCreate.
	// Usa F1/F2/F3/F4/F5/F6 dentro da tela de criação para encontrar a posição bonita.
	static float g_fCreateCamDist = 1800.0f;
	static float g_fCreateCamPitch = -8.0f;
	static float g_fCreateCamYaw = 0.0f;
	static float g_fCreateCamHeight = 120.0f;

	// Velocidade dos ajustes em modo debug.
	static float g_fCreateCamMoveStep = 1000.0f;
	static float g_fCreateCamHeightStep = 250.0f;
	static float g_fCreateCamDistStep = 500.0f;
	static float g_fCreateCamAngleStep = 5.0f;

	// Alvo real da câmera dentro do mapa 04 / DatsUnderground.
	// Este valor tem de bater com a posição usada em CharacterCreateContents.cpp.
	static float g_fCreateCamTargetX = CHARACTER_CREATE_MAP_START_X;
	static float g_fCreateCamTargetY = 120.0f;
	static float g_fCreateCamTargetZ = CHARACTER_CREATE_MAP_START_Z;




	void CCDeleteMapObjectData()
	{
		for (size_t i = 0; i < g_vecCharacterCreateMapObjects.size(); ++i)
		{
			CsNodeObj* pObj = g_vecCharacterCreateMapObjects[i];
			if (pObj)
			{
				pObj->Delete();
				NiDelete pObj;
			}
		}

		g_vecCharacterCreateMapObjects.clear();
	}

	void CCNormalizePackPath(std::string& kPath)
	{
		std::replace(kPath.begin(), kPath.end(), '/', '\\');

		for (size_t i = 0; i < kPath.size(); ++i)
		{
			if (kPath[i] >= 'A' && kPath[i] <= 'Z')
				kPath[i] = (char)(kPath[i] - 'A' + 'a');
		}
	}


	bool CCShouldSkipObjectDataPath(std::string const& kNormalizedPath)
	{
#if CHARACTER_CREATE_SKIP_BACKGROUND_OBJECTS
		// O path já vem normalizado para minúsculas e com '\\'.
		if (kNormalizedPath.find("\\object_data\\background\\") != std::string::npos)
			return true;
#endif
		return false;
	}

	bool CCEndsWithNif(std::string const& kPath)
	{
		if (kPath.size() < 4)
			return false;

		std::string kTail = kPath.substr(kPath.size() - 4);
		CCNormalizePackPath(kTail);

		return kTail == ".nif";
	}

	bool CCStartsWith(std::string const& kPath, std::string const& kPrefix)
	{
		if (kPath.size() < kPrefix.size())
			return false;

		return kPath.compare(0, kPrefix.size(), kPrefix) == 0;
	}

	bool CCLoadNifToSceneObject(char const* pszPath, CsNodeObj& kOutObj, NiPoint3 const& kTranslate)
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

		// IMPORTANTE:
		// O mapa carregado diretamente não passa pelo shader/lighting normal do terrain.
		// Sem emittance, muitos NIFs aparecem totalmente pretos nesta tela.
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

	bool CCLoadNifToNewSceneObject(char const* pszPath)
	{
		if (!pszPath || pszPath[0] == 0)
			return false;

		CsNodeObj* pObj = NiNew CsNodeObj;
		if (!pObj)
			return false;

		if (!CCLoadNifToSceneObject(pszPath, *pObj, NiPoint3::ZERO))
		{
			pObj->Delete();
			NiDelete pObj;
			return false;
		}

		g_vecCharacterCreateMapObjects.push_back(pObj);
		return true;
	}

	int CCLoadMap04ObjectDataFromPackIndex(int nPackIndex)
	{
		std::list<std::string> kFiles;
		CsFPS::CsFPSystem::GetFileList(nPackIndex, kFiles);

		char szLog[512] = { 0, };
		sprintf_s(szLog, 512, "[UI][CharacterCreate][MAP04] PackIndex=%d FileList count=%d\n", nPackIndex, (int)kFiles.size());
		OutputDebugStringA(szLog);

		if (kFiles.empty())
			return 0;

		std::string kObjectRoot = CHARACTER_CREATE_OBJECT_DATA_ROOT;
		CCNormalizePackPath(kObjectRoot);

		// Garantir barra final para não apanhar caminhos parecidos.
		if (!kObjectRoot.empty() && kObjectRoot[kObjectRoot.size() - 1] != '\\')
			kObjectRoot += "\\";

		int nLoaded = 0;
		int nFound = 0;
		int nFailed = 0;
		int nSampleLogged = 0;

		for (std::list<std::string>::iterator it = kFiles.begin(); it != kFiles.end(); ++it)
		{
			std::string kOriginal = *it;
			std::string kNormalized = kOriginal;
			CCNormalizePackPath(kNormalized);

			if (!CCStartsWith(kNormalized, kObjectRoot))
				continue;

			if (!CCEndsWithNif(kNormalized))
				continue;

			if (CCShouldSkipObjectDataPath(kNormalized))
			{
				if (nSampleLogged < 20)
				{
					char szSkip[MAX_PATH + 256] = { 0, };
					sprintf_s(szSkip, sizeof(szSkip), "[UI][CharacterCreate][MAP04] skipped background nif=%s\n", kOriginal.c_str());
					OutputDebugStringA(szSkip);
					++nSampleLogged;
				}
				continue;
			}

			++nFound;

			// Usar o path original devolvido pelo pack, porque pode preservar maiúsculas/minúsculas.
			if (CCLoadNifToNewSceneObject(kOriginal.c_str()))
			{
				++nLoaded;

				if (nSampleLogged < 20)
				{
					char szSample[MAX_PATH + 256] = { 0, };
					sprintf_s(szSample, sizeof(szSample), "[UI][CharacterCreate][MAP04] loaded pack nif=%s\n", kOriginal.c_str());
					OutputDebugStringA(szSample);
					++nSampleLogged;
				}
			}
			else
			{
				++nFailed;

				if (nFailed <= 20)
				{
					char szFail[MAX_PATH + 256] = { 0, };
					sprintf_s(szFail, sizeof(szFail), "[UI][CharacterCreate][MAP04] failed pack nif=%s\n", kOriginal.c_str());
					OutputDebugStringA(szFail);
				}
			}
		}

		sprintf_s(
			szLog,
			512,
			"[UI][CharacterCreate][MAP04] PackIndex=%d object_data found=%d loaded=%d failed=%d\n",
			nPackIndex,
			nFound,
			nLoaded,
			nFailed
		);
		OutputDebugStringA(szLog);

		return nLoaded;
	}

	int CCLoadMap04ObjectDataFromPacks()
	{
		int nTotalLoaded = 0;

		// IMPORTANTE:
		// O log mostrou que PackIndex=0 carregou corretamente:
		//   FileList count=55211
		//   object_data found=... loaded=... failed=...
		//
		// O crash acontece logo depois, ao tentar enumerar PackIndex=1/Pack03.
		// Como o mapa 04 / DatsUnderground e os object_data já estão todos no Pack01, não precisamos
		// tocar no Pack03 aqui.
		nTotalLoaded += CCLoadMap04ObjectDataFromPackIndex(0);

		char szLog[256] = { 0, };
		sprintf_s(szLog, 256, "[UI][CharacterCreate][MAP04] DatsUnderground object_data pack01-only total loaded=%d\n", nTotalLoaded);
		OutputDebugStringA(szLog);

		return nTotalLoaded;
	}


	void CCLogObjectDataBoundsSample()
	{
		int nLogged = 0;

		for (size_t i = 0; i < g_vecCharacterCreateMapObjects.size(); ++i)
		{
			CsNodeObj* pObj = g_vecCharacterCreateMapObjects[i];
			if (!pObj || !pObj->m_pNiNode)
				continue;

			NiBound kBound = pObj->m_pNiNode->GetWorldBound();

			char szLog[512] = { 0, };
			sprintf_s(
				szLog,
				512,
				"[UI][CharacterCreate][MAP04] object sample idx=%d center=(%.2f, %.2f, %.2f) radius=%.2f\n",
				(int)i,
				kBound.GetCenter().x,
				kBound.GetCenter().y,
				kBound.GetCenter().z,
				kBound.GetRadius()
			);
			OutputDebugStringA(szLog);

			++nLogged;
			if (nLogged >= 10)
				break;
		}
	}

	void CCLogCamera()
	{
		char szBuffer[768] = { 0, };
		sprintf_s(
			szBuffer,
			768,
			"[UI][CharacterCreate][CAMERA] target=(%.2f, %.2f, %.2f) dist=%.2f pitch=%.2f yaw=%.2f height=%.2f stepMove=%.2f stepY=%.2f\n",
			g_fCreateCamTargetX,
			g_fCreateCamTargetY,
			g_fCreateCamTargetZ,
			g_fCreateCamDist,
			g_fCreateCamPitch,
			g_fCreateCamYaw,
			g_fCreateCamHeight,
			g_fCreateCamMoveStep,
			g_fCreateCamHeightStep
		);
		OutputDebugStringA(szBuffer);

		sprintf_s(
			szBuffer,
			768,
			"[UI][CharacterCreate][COPY] CharacterCreate.cpp targetX=%.2ff targetY=%.2ff targetZ=%.2ff dist=%.2ff pitch=%.2ff yaw=%.2ff height=%.2ff\n",
			g_fCreateCamTargetX,
			g_fCreateCamTargetY,
			g_fCreateCamTargetZ,
			g_fCreateCamDist,
			g_fCreateCamPitch,
			g_fCreateCamYaw,
			g_fCreateCamHeight
		);
		OutputDebugStringA(szBuffer);
	}

	void CCLogMapCenter()
	{
		char szBuffer[256] = { 0, };
		sprintf_s(
			szBuffer,
			256,
			"[UI][CharacterCreate][MAP] mapID=%lu startX=%.2f startZ=%.2f\n",
			(unsigned long)CHARACTER_CREATE_LOBBY_MAP_ID,
			CHARACTER_CREATE_MAP_START_X,
			CHARACTER_CREATE_MAP_START_Z
		);
		OutputDebugStringA(szBuffer);
	}

	void CCLog(const char* pszText)
	{
		if (pszText)
		{
			OutputDebugStringA("[UI][CharacterCreate] ");
			OutputDebugStringA(pszText);
			OutputDebugStringA("\n");
		}
	}

	void CCLogInt(const char* pszText, int nValue)
	{
		char szBuffer[256] = { 0, };
		sprintf_s(szBuffer, 256, "[UI][CharacterCreate] %s%d\n", pszText ? pszText : "", nValue);
		OutputDebugStringA(szBuffer);
	}
}

CCharacterCreate::CCharacterCreate() :
	m_pIntroSound(NULL),
	m_pButtonOK(NULL),
	m_pButtonCancel(NULL),
	m_pTamerName(NULL),
	m_pEditBox(NULL),
	bIMECheck(false),
	m_pTamerListBox(NULL),
	m_pLefBtn(NULL),
	m_pRightBtn(NULL),
	m_pTamerSkills(NULL),
	m_pSkillList(NULL),
	m_pTamerDesc(NULL),
	m_pBaseStateAT(NULL),
	m_pBaseStateDP(NULL),
	m_pBaseStateHP(NULL),
	m_pBaseStateDS(NULL),
	m_pCostumList(NULL),
	m_bLobbyMapLoaded(false),
	m_dwLobbyMapID(CHARACTER_CREATE_LOBBY_MAP_ID)
{
}

CCharacterCreate::~CCharacterCreate()
{
	if (GetSystem())
		GetSystem()->UnRegisterAll(this);

	Destory();
}

void CCharacterCreate::Destory()
{
	ReleaseLobbyMap();

	g_IME.SetLockConidateList(false);
	DeleteScript();
	m_MainButtonUI.DeleteScript();
	m_CharacterListUI.DeleteScript();
	m_pCostumSelectWindow.DeleteScript();

	if (m_pIntroSound)
	{
		if (g_pSoundMgr)
		{
			g_pSoundMgr->StopSound(m_pIntroSound);
			g_pSoundMgr->Set_BGM_FadeVolume(m_fBackupMusic);
		}
		m_pIntroSound = NULL;
	}
}

bool CCharacterCreate::PreloadLobbyMapStatic()
{
	CCLog("PreloadLobbyMapStatic skipped - DatsUnderground 04 loads directly in CharacterCreate");

	g_bCharacterCreateLobbyMapPreloaded = false;
	g_bCharacterCreateLobbyMapPreloadTried = true;

	return true;
}

bool CCharacterCreate::IsLobbyMapPreloadedStatic()
{
	return g_bCharacterCreateLobbyMapPreloaded;
}

void CCharacterCreate::ResetLobbyMapPreloadStatic()
{
	CCLog("ResetLobbyMapPreloadStatic");

	g_bCharacterCreateLobbyMapPreloaded = false;
	g_bCharacterCreateLobbyMapPreloadTried = false;
}

bool CCharacterCreate::LoadLobbyMap()
{
	CCLog("LoadLobbyMap begin - DatsUnderground 04 main map NIF + object_data");

	if (m_bLobbyMapLoaded && g_bCharacterCreateSceneLoaded && !g_vecCharacterCreateMapObjects.empty())
	{
		CCLog("LoadLobbyMap skipped - map already loaded by this instance");
		return true;
	}

	g_CharacterCreateScene.Delete();
	g_bCharacterCreateSceneLoaded = false;
	CCDeleteMapObjectData();

	// 1) Carrega o NIF principal do mapa:
	// Data\map\realworld_r\04_datsunderground\04\map_data\04.nif
	CCLog("LoadLobbyMap - load main map NIF begin: " CHARACTER_CREATE_MAIN_MAP_NIF);

	if (!CCLoadNifToSceneObject(CHARACTER_CREATE_MAIN_MAP_NIF, g_CharacterCreateScene, NiPoint3::ZERO))
	{
		CCLog("LoadLobbyMap failed - could not load main map04 NIF");
		CsMessageBoxA(MB_OK, "%s 경로\n에서 오브젝트를 찾지 못했습니다.", CHARACTER_CREATE_MAIN_MAP_NIF);
		return false;
	}

	g_bCharacterCreateSceneLoaded = true;
	CCLog("LoadLobbyMap - main map NIF loaded");

	// 2) Carrega os objetos do mapa pelo FilePack.
	// CsFPSystem::GetFileList(0) lista o Pack01; depois filtramos:
	// Data\map\realworld_r\04_datsunderground\04\object_data\*.nif
	int nObjectDataLoaded = CCLoadMap04ObjectDataFromPacks();

	char szLog[256] = { 0, };
	sprintf_s(szLog, 256, "LoadLobbyMap - object_data pack loaded count=%d", nObjectDataLoaded);
	CCLog(szLog);
	CCLogObjectDataBoundsSample();

	if (nObjectDataLoaded <= 0)
	{
		CCLog("LoadLobbyMap warning - object_data loaded count is 0");
		// Não falhar aqui: o NIF principal já carregou.
	}

	m_bLobbyMapLoaded = true;
	g_bCharacterCreateLobbyMapPreloaded = true;
	g_bCharacterCreateLobbyMapPreloadTried = true;

	CCLog("LoadLobbyMap ok - DatsUnderground 04 main map NIF + object_data loaded");
	CCLogCamera();

	return true;
}

void CCharacterCreate::ReleaseLobbyMap()
{
	CCLog("ReleaseLobbyMap begin");

	if (!m_bLobbyMapLoaded && g_vecCharacterCreateMapObjects.empty())
	{
		CCLog("ReleaseLobbyMap skipped - not loaded");
		return;
	}

	g_CharacterCreateScene.Delete();
	CCDeleteMapObjectData();

	g_bCharacterCreateSceneLoaded = false;
	m_bLobbyMapLoaded = false;

	CCLog("ReleaseLobbyMap end - DatsUnderground 04 main map NIF/object_data deleted");
}

void CCharacterCreate::RenderLobbyMap()
{
	static int s_nRenderLobbyMapLogCounter = 0;

	if (!m_bLobbyMapLoaded)
	{
		if (++s_nRenderLobbyMapLogCounter >= 120)
		{
			s_nRenderLobbyMapLogCounter = 0;
			CCLog("RenderLobbyMap skipped - map not loaded");
		}
		return;
	}

	if (++s_nRenderLobbyMapLogCounter >= 120)
	{
		s_nRenderLobbyMapLogCounter = 0;

		char szLog[256] = { 0, };
		sprintf_s(
			szLog,
			256,
			"RenderLobbyMap - main=%d object_data count=%d",
			CHARACTER_CREATE_RENDER_MAIN_MAP_NIF,
			(int)g_vecCharacterCreateMapObjects.size()
		);
		CCLog(szLog);
	}

#if CHARACTER_CREATE_RENDER_MAIN_MAP_NIF
	if (g_bCharacterCreateSceneLoaded && g_CharacterCreateScene.m_pNiNode)
		g_CharacterCreateScene.Render();
#endif

	for (size_t i = 0; i < g_vecCharacterCreateMapObjects.size(); ++i)
	{
		CsNodeObj* pObj = g_vecCharacterCreateMapObjects[i];
		if (pObj && pObj->m_pNiNode)
			pObj->Render();
	}
}

bool CCharacterCreate::Init()
{
	CCLog("Init begin");
	CCLogMapCenter();

	CCLog("Init - SetCameraInfo begin");
	SetCameraInfo();
	CCLog("Init - SetCameraInfo end");

	if (!GetSystem())
	{
		CCLog("Init failed - GetSystem is NULL");
		return false;
	}

	CCLog("Init - MakeCreatedTamerList begin");
	GetSystem()->MakeCreatedTamerList();
	CCLog("Init - MakeCreatedTamerList end");

	m_fBackupMusic = g_pResist->m_Global.s_fMusic;
	g_IME.SetLockConidateList(true);

	CCLog("Init - InitScript NULL begin");
	InitScript(NULL, CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight), false);
	CCLog("Init - InitScript NULL end");

	CCLog("Init - LoadLobbyMap begin");
	if (!LoadLobbyMap())
	{
		CCLog("Init - LoadLobbyMap failed, fallback to Background.dds");

		DeleteScript();
		InitScript("Lobby\\CharacterCreate\\Background.dds", CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight), false);
	}
	else
	{
		CCLog("Init - LoadLobbyMap success");
	}

	CCLog("Init - makeMainButtonUI begin");
	makeMainButtonUI();
	CCLog("Init - makeMainButtonUI end");

	CCLog("Init - makeCostumWindowUI begin");
	makeCostumWindowUI();
	CCLog("Init - makeCostumWindowUI end");

	CCLog("Init - makeTamerWindowUI begin");
	makeTamerWindowUI();
	CCLog("Init - makeTamerWindowUI end");

	CCLog("Init - setTamerList begin");
	setTamerList();
	CCLog("Init - setTamerList end");

	CCLog("Init ok");
	return true;
}

void CCharacterCreate::makeMainButtonUI()
{
	m_MainButtonUI.InitScript(NULL/*"CharSelect\\BottomBack.tga"*/, CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight), false);
	m_MainButtonUI.AddSprite(CsPoint((g_nScreenWidth - 446) / 2, 0), CsPoint(446, 118), "Lobby\\TopText_BG.tga");

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_16, FONT_WHITE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("CHARACTER_CREATE_TAMER_TITLE").c_str());
		m_MainButtonUI.AddText(&ti, CsPoint(g_nScreenWidth / 2, 49), true);
	}

	m_pButtonCancel = m_MainButtonUI.AddButton(CsPoint((g_nScreenWidth - 176) / 2 - 400, g_nScreenHeight - 80), CsPoint(176, 50), CsPoint(0, 50), "CommonUI\\Simple_btn_L.tga", cWindow::SD_Bu3);
	if (m_pButtonCancel)
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_14, NiColor::WHITE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("CHARACTER_CREATE_BACK").c_str());
		m_pButtonCancel->SetText(&ti);
		m_pButtonCancel->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CCharacterCreate::PressCancelButton);
	}

	m_MainButtonUI.AddSprite(CsPoint((g_nScreenWidth - 256) / 2, g_nScreenHeight - 80), CsPoint(256, 68), "Lobby\\CharacterCreate\\input_field.tga", true);

	{
		m_pEditBox = NiNew cEditBox;
		if (m_pEditBox)
		{
			cText::sTEXTINFO ti;
			ti.Init(&g_pEngine->m_FontSystem, CFont::FS_12);
			ti.s_Color = NiColor::WHITE;
			ti.s_eTextAlign = DT_LEFT;
			m_pEditBox->Init(m_MainButtonUI.GetRoot(), CsPoint((g_nScreenWidth - 150) / 2, g_nScreenHeight - 55), CsPoint(150, 28), &ti, false);
			m_pEditBox->EnableUnderline();
			m_pEditBox->SetEmptyMsgText(UISTRING_TEXT("CHARACTER_CREATE_TAMER_NAME_INPUT_EMPTY_MSG").c_str(), NiColor(0.5f, 0.5f, 0.5f));
			m_pEditBox->SetFontLength(NAME_MAX_LEN);
			m_pEditBox->SetFocus();
			m_MainButtonUI.AddChildControl(m_pEditBox);
		}
	}

	m_pButtonOK = m_MainButtonUI.AddButton(CsPoint((g_nScreenWidth - 176) / 2 + 400, g_nScreenHeight - 80), CsPoint(176, 50), CsPoint(0, 50), "CommonUI\\Simple_btn_L.tga", cWindow::SD_Bu3);
	if (m_pButtonOK)
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_14, NiColor::WHITE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("CHARACTER_CREATE_CREATE").c_str());
		m_pButtonOK->SetText(&ti);
		m_pButtonOK->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CCharacterCreate::PressOkButton);
	}
}

void CCharacterCreate::makeTamerWindowUI()
{
	m_CharacterListUI.InitScript("Lobby\\CharacterCreate\\ListBack.tga", CsPoint(g_nScreenWidth - 530, (g_nScreenHeight - 592) / 2), CsPoint(512, 592), false);

	m_pTamerListBox = NiNew cGridListBox;
	SAFE_POINTER_RET(m_pTamerListBox);

	m_pTamerListBox->Init(GetRoot(), CsPoint(60, 25), CsPoint(420, 300), CsPoint::ZERO, CsPoint(94, 99), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0);
	m_pTamerListBox->AddEvent(cGridListBox::GRID_SELECT_CHANGE_ITEM, this, &CCharacterCreate::_SelectedTamer);
	m_CharacterListUI.AddChildControl(m_pTamerListBox);
	m_pTamerListBox->SetMouseOverImg("Lobby\\CharacterCreate\\TamerIcon_glow.tga");
	m_pTamerListBox->SetSelectedImg("Lobby\\CharacterCreate\\TamerIcon_glow.tga");
	m_pTamerListBox->SetBackOverAndSelectedImgRender(false);

	cScrollBar* pScrollBar = NiNew cScrollBar;
	if (pScrollBar)
	{
		pScrollBar->Init(cScrollBar::TYPE_SEALMASTER, NULL, CsPoint(0, 0), CsPoint(16, 330), cScrollBar::GetDefaultBtnSize(), CsRect(CsPoint(0, 0), CsPoint(450, 220)), 2, false);
		m_pTamerListBox->SetScrollBar(pScrollBar);
	}

	m_CharacterListUI.AddSprite(CsPoint(6, 337), CsPoint(500, 38), "Lobby\\CharacterCreate\\char_gline.tga");
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_16, FONT_WHITE);
		m_pTamerName = m_CharacterListUI.AddText(&ti, CsPoint(21, 347), true);
	}

	m_CharacterListUI.AddSprite(CsPoint(6, 375), CsPoint(494, 105), "Lobby\\CharacterCreate\\TextBack_line.tga");
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_11, FONT_WHITE);
		m_pTamerDesc = m_CharacterListUI.AddText(&ti, CsPoint(21, 380), true);
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, FONT_NEWGOLD);
		ti.SetText(UISTRING_TEXT("CHARACTER_CREATE_TAMER_HAVE_SKILL").c_str());
		m_CharacterListUI.AddText(&ti, CsPoint(25, 510), true);
	}

	m_pTamerSkills = NiNew cGridListBox;
	if (m_pTamerSkills)
	{
		m_pTamerSkills->Init(NULL, CsPoint(18, 536), CsPoint(460, 35), CsPoint(10, 0), CsPoint(32, 32), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0);
		m_pTamerSkills->SetMouseOverImg("Icon\\Mask_Over.tga");
		m_pTamerSkills->SetBackOverAndSelectedImgRender(false);
		m_CharacterListUI.AddChildControl(m_pTamerSkills);
	}

	{
		m_CharacterListUI.AddSprite(CsPoint(204, 518), CsPoint(282, 50), "Lobby\\CharacterCreate\\Char_status.tga");

		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, FONT_GREEN);
		ti.s_eTextAlign = DT_CENTER;
		m_pBaseStateAT = m_CharacterListUI.AddText(&ti, CsPoint(239, 546));
		m_pBaseStateDP = m_CharacterListUI.AddText(&ti, CsPoint(309, 546));
		m_pBaseStateHP = m_CharacterListUI.AddText(&ti, CsPoint(379, 546));
		m_pBaseStateDS = m_CharacterListUI.AddText(&ti, CsPoint(449, 546));
	}
}

void CCharacterCreate::makeCostumWindowUI()
{
	m_pCostumSelectWindow.InitScript(NULL, CsPoint(10, (g_nScreenHeight - 350) / 2), CsPoint(156, 350), false);

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_14, FONT_NEWGOLD);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("CHARACTER_CREATE_TAMER_COSTUME_LIST_TITLE").c_str());
		m_pCostumSelectWindow.AddText(&ti, CsPoint(78, 0));
	}

	m_pCostumList = NiNew cGridListBox;
	if (m_pCostumList)
	{
		m_pCostumList->Init(NULL, CsPoint(52, 30), CsPoint(52, 310), CsPoint::ZERO, CsPoint(52, 51), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0);
		m_pCostumList->AddEvent(cGridListBox::GRID_BUTTON_UP, this, &CCharacterCreate::_ChangeCostume);
		m_pCostumSelectWindow.AddChildControl(m_pCostumList);
	}
}

void CCharacterCreate::_UpdateCostumData(DWORD const& dwTamerIDX, std::list<DWORD> const& costum)
{
	SAFE_POINTER_RET(m_pCostumList);
	m_pCostumList->RemoveAllItem();

	if (costum.empty())
	{
		m_pCostumSelectWindow.SetVisible(false);
		return;
	}

	m_pCostumSelectWindow.SetVisible(true);

	int n = 0;
	std::list<DWORD>::const_iterator it = costum.begin();
	for (; it != costum.end(); ++it, ++n)
	{
		cString* pItem = NiNew cString;
		SAFE_POINTER_BEK(pItem);

		std::string file;
		DmCS::StringFn::FormatA(file, "Lobby\\CharacterCreate\\lobby_costume_%02d.tga", n + 1);

		cButton* pBtn = NiNew cButton;
		if (pBtn)
		{
			pBtn->Init(NULL, CsPoint::ZERO, CsPoint(52, 51), file.c_str(), false);
			pBtn->SetUserData(new sCostumeInfo(dwTamerIDX, (*it)));
			pBtn->SetTexToken(CsPoint(0, 51));
			pBtn->SetSound(cWindow::SD_Bu3);
		}

		cString::sBUTTON* paddImg = pItem->AddButton(pBtn, 0, CsPoint::ZERO, CsPoint(52, 51));
		if (paddImg)
			paddImg->SetAutoPointerDelete(true);

		cGridListBoxItem* addItem = NiNew cGridListBoxItem(n, CsPoint(52, 51));
		addItem->SetItem(pItem);
		std::wstring name = GetSystem()->GetItemName((*it));
		addItem->SetUserData(new sCostumeName(name));
		m_pCostumList->AddItem(addItem);
	}

	{
		cString* pItem = NiNew cString;
		cButton* pBtn = NiNew cButton;
		pBtn->Init(NULL, CsPoint::ZERO, CsPoint(52, 51), "Lobby\\SelectServer\\lobby_server_refresh.tga", false);
		pBtn->SetUserData(new sCostumeInfo(dwTamerIDX, 0));
		pBtn->SetTexToken(CsPoint(0, 51));
		cString::sBUTTON* paddImg = pItem->AddButton(pBtn, 0, CsPoint::ZERO, CsPoint(52, 51));
		if (paddImg)
			paddImg->SetAutoPointerDelete(true);
		cGridListBoxItem* addItem = NiNew cGridListBoxItem(n++, CsPoint(52, 51));
		addItem->SetItem(pItem);
		addItem->SetUserData(new sCostumeName(UISTRING_TEXT("CHARACTER_CREATE_TAMER_COSTUME_UNEQUIP").c_str()));
		m_pCostumList->AddItem(addItem);
	}
}

void CCharacterCreate::_ChangeCostume(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cString::sBUTTON* pButton = static_cast<cString::sBUTTON*>(pData);
	SAFE_POINTER_RET(pButton->s_pButton);

	sCostumeInfo* pUserData = dynamic_cast<sCostumeInfo*>(pButton->s_pButton->GetUserData());
	SAFE_POINTER_RET(pUserData);

	SAFE_POINTER_RET(GetSystem());

	GetSystem()->ChangeTamerCostume(pUserData->m_dwCostumeID);
}

void CCharacterCreate::setTamerList()
{
	SAFE_POINTER_RET(m_pTamerListBox);

	m_pTamerListBox->RemoveAllItem();
	CharacterCreateContents::LIST_TAMER_INFO const& pList = GetSystem()->GetMakeTamerList();
	CharacterCreateContents::LIST_TAMER_INFO_CIT it = pList.begin();
	for (int n = 0; it != pList.end(); ++it, ++n)
	{
		cString* pItem = NiNew cString;
		SAFE_POINTER_BEK(pItem);

		cImage* pTamerImage = NiNew cImage;
		if (pTamerImage)
		{
			pTamerImage->Init(NULL, CsPoint::ZERO, CsPoint(85, 91), "Lobby\\CharacterCreate\\TamerIcon_L.tga", false);
			pTamerImage->SetTexToken(CsPoint(85, 0));
			cString::sIMAGE* sImg = pItem->AddImage(pTamerImage, (*it).m_nIconIdx, CsPoint(4, 4), CsPoint(85, 91));
			if (sImg)
				sImg->SetAutoPointerDelete(true);

			if (!(*it).m_nEnableCreated)
				sImg->SetColor(NiColor(0.3f, 0.3f, 0.3f));
		}

		cGridListBoxItem* addItem = NiNew cGridListBoxItem(n, CsPoint(94, 99));
		addItem->SetItem(pItem);
		addItem->SetUserData(new sTamerInfo((*it).m_nFileTableID, (*it).m_nEnableCreated));
		m_pTamerListBox->AddItem(addItem);
	}

	m_pTamerListBox->SetSelectedItemFromIdx(0, true);
}

void CCharacterCreate::Update3DModel(float fDeltaTime)
{
	if (GetSystem())
		GetSystem()->Update_TamerModel(fDeltaTime);
}

void CCharacterCreate::UpdateSound()
{
	SAFE_POINTER_RET(g_pSoundMgr);
	SAFE_POINTER_RET(m_pIntroSound);

	if (g_pSoundMgr->IsSound(m_pIntroSound) == false)
	{
		g_pSoundMgr->Set_BGM_FadeVolume(m_fBackupMusic);
		m_pIntroSound = NULL;
	}
	else
		g_pSoundMgr->Set_BGM_Volume(0.07f);
}

void CCharacterCreate::ChangeCharacterSound(std::string const& soundFile)
{
	SAFE_POINTER_RET(g_pSoundMgr);

	if (m_pIntroSound)
	{
		g_pSoundMgr->StopSound(m_pIntroSound);
		m_pIntroSound = NULL;
	}

	if (soundFile.empty())
		return;

	std::string file = soundFile;
	m_pIntroSound = g_pSoundMgr->PlaySystemSound(const_cast<char*>(file.c_str()));

	if (m_pIntroSound)
		m_pIntroSound->SetVolume(1.0f);
}

BOOL CCharacterCreate::UpdateMouse()
{
	if (m_pButtonOK && m_pButtonOK->Update_ForMouse() == cButton::ACTION_CLICK)
		return TRUE;

	if (m_pButtonCancel && m_pButtonCancel->Update_ForMouse() == cButton::ACTION_CLICK)
		return TRUE;

	if (m_pLefBtn && m_pLefBtn->Update_ForMouse() == cButton::ACTION_CLICK)
		return TRUE;

	if (m_pRightBtn && m_pRightBtn->Update_ForMouse() == cButton::ACTION_CLICK)
		return TRUE;

	if (m_pTamerListBox && m_pTamerListBox->Update_ForMouse(CURSOR_ST.GetPos()))
		return TRUE;

	if (m_pCostumSelectWindow.IsVisible())
	{
		if (m_pCostumList && m_pCostumList->Update_ForMouse(CURSOR_ST.GetPos()))
		{
			cGridListBoxItem const* pOverGrid = m_pCostumList->GetMouseOverItem();
			if (pOverGrid)
			{
				sCostumeName* pCostumeInfo = dynamic_cast<sCostumeName*>(pOverGrid->GetUserData());
				if (pCostumeInfo)
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text(pOverGrid->GetWorldPos(), CsPoint(52, 51), pCostumeInfo->m_sItemName.c_str(), CFont::FS_13);
			}
			return TRUE;
		}
	}

	if (m_pTamerSkills && m_pTamerSkills->Update_ForMouse(CURSOR_ST.GetPos()))
	{
		cGridListBoxItem const* pOverGrid = m_pTamerSkills->GetMouseOverItem();
		if (pOverGrid)
		{
			sSkillInfo* pSkillInfo = dynamic_cast<sSkillInfo*>(pOverGrid->GetUserData());
			if (pSkillInfo)
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip(pOverGrid->GetWorldPos(), CsPoint(32, 32), 360, cTooltip::SKILL_SIMPLE, pSkillInfo->m_nTableIDX, 0);
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CCharacterCreate::UpdateKeyboard(const MSG& p_kMsg)
{
	const int isBitSet = (DWORD)p_kMsg.lParam & 0x40000000;

	switch (p_kMsg.message)
	{
	case WM_IME_NOTIFY:
	{
		if (!g_IME.IsLockChangeLanguage())
		{
			if (g_IME.OnNotify(GAMEAPP_ST.GetHWnd(), p_kMsg.wParam, p_kMsg.lParam) == true)
				bIMECheck = true;
		}
		return TRUE;
	}
	break;

	case WM_KEYDOWN:
	{
		switch (p_kMsg.wParam)
		{
		case VK_RETURN:
		{
			if (isBitSet == 0 && m_pButtonOK && m_pButtonOK->IsEnable())
			{
				m_pButtonOK->KeyboardBtnDn();
			}
			return TRUE;
		}
		break;

		case VK_ESCAPE:
		{
			if (isBitSet == 0)
			{
				if (m_pButtonCancel)
					m_pButtonCancel->KeyboardBtnDn();
			}
			return TRUE;
		}
		break;

		// ==============================================================
		// MODO DEBUG DA CÂMERA DO CHARACTER CREATE
		//
		// Movimento do alvo:
		//   W / S        = target Z + / -
		//   A / D        = target X - / +
		//   Q / E        = target Y - / +
		//
		// Câmera:
		//   F1 / F2      = distância - / +
		//   F3 / F4      = pitch - / +
		//   F5 / F6      = delta height - / +
		//   F7 / F8      = yaw - / +
		//
		// Velocidade:
		//   F9 / F10     = reduzir/aumentar passo de movimento
		//   PageUp/Down  = target Y + / - com passo maior
		//
		// Output:
		//   F11          = imprimir coordenadas atuais para copiar
		// ==============================================================

		case 'W':
		{
			g_fCreateCamTargetZ += g_fCreateCamMoveStep;
			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case 'S':
		{
			g_fCreateCamTargetZ -= g_fCreateCamMoveStep;
			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case 'A':
		{
			g_fCreateCamTargetX -= g_fCreateCamMoveStep;
			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case 'D':
		{
			g_fCreateCamTargetX += g_fCreateCamMoveStep;
			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case 'Q':
		{
			g_fCreateCamTargetY -= g_fCreateCamHeightStep;
			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case 'E':
		{
			g_fCreateCamTargetY += g_fCreateCamHeightStep;
			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_PRIOR: // PageUp
		{
			g_fCreateCamTargetY += g_fCreateCamHeightStep * 4.0f;
			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_NEXT: // PageDown
		{
			g_fCreateCamTargetY -= g_fCreateCamHeightStep * 4.0f;
			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F1:
		{
			g_fCreateCamDist -= g_fCreateCamDistStep;
			if (g_fCreateCamDist < 100.0f)
				g_fCreateCamDist = 100.0f;

			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F2:
		{
			g_fCreateCamDist += g_fCreateCamDistStep;
			if (g_fCreateCamDist > 50000.0f)
				g_fCreateCamDist = 50000.0f;

			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F3:
		{
			g_fCreateCamPitch -= g_fCreateCamAngleStep;
			if (g_fCreateCamPitch < -89.0f)
				g_fCreateCamPitch = -89.0f;

			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F4:
		{
			g_fCreateCamPitch += g_fCreateCamAngleStep;
			if (g_fCreateCamPitch > 89.0f)
				g_fCreateCamPitch = 89.0f;

			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F5:
		{
			g_fCreateCamHeight -= g_fCreateCamHeightStep;
			if (g_fCreateCamHeight < -3000.0f)
				g_fCreateCamHeight = -3000.0f;

			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F6:
		{
			g_fCreateCamHeight += g_fCreateCamHeightStep;
			if (g_fCreateCamHeight > 3000.0f)
				g_fCreateCamHeight = 3000.0f;

			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F7:
		{
			g_fCreateCamYaw -= g_fCreateCamAngleStep;
			if (g_fCreateCamYaw < -360.0f)
				g_fCreateCamYaw += 360.0f;

			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F8:
		{
			g_fCreateCamYaw += g_fCreateCamAngleStep;
			if (g_fCreateCamYaw > 360.0f)
				g_fCreateCamYaw -= 360.0f;

			SetCameraInfo();
			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F9:
		{
			g_fCreateCamMoveStep *= 0.5f;
			g_fCreateCamHeightStep *= 0.5f;

			if (g_fCreateCamMoveStep < 5.0f)
				g_fCreateCamMoveStep = 5.0f;

			if (g_fCreateCamHeightStep < 2.0f)
				g_fCreateCamHeightStep = 2.0f;

			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F10:
		{
			g_fCreateCamMoveStep *= 2.0f;
			g_fCreateCamHeightStep *= 2.0f;

			if (g_fCreateCamMoveStep > 20000.0f)
				g_fCreateCamMoveStep = 20000.0f;

			if (g_fCreateCamHeightStep > 10000.0f)
				g_fCreateCamHeightStep = 10000.0f;

			CCLogCamera();
			return TRUE;
		}
		break;

		case VK_F11:
		{
			CCLogCamera();
			return TRUE;
		}
		break;
		}
		break;
	}
	break;

	case WM_KEYUP:
	{
		switch (p_kMsg.wParam)
		{
		case VK_RETURN:
		{
			if (bIMECheck)
			{
				bIMECheck = false;
				return FALSE;
			}

#ifndef NOT_ENTER_CREATENAME
			if (m_pButtonOK && m_pButtonOK->IsEnable())
			{
				PressOkButton(m_pButtonOK, NULL);
				m_pButtonOK->KeyboardBtnUp();
				return TRUE;
			}
#endif					
			return TRUE;
		}
		break;

		case VK_ESCAPE:
		{
			if (bIMECheck)
			{
				bIMECheck = false;
				return FALSE;
			}

			if (m_pButtonCancel && GetSystem())
			{
				m_pButtonCancel->KeyboardBtnUp();
				GetSystem()->GotoBack();
			}

			return TRUE;
		}
		break;
		}
	}
	break;
	}

	return FALSE;
}

void CCharacterCreate::Update(float fDeltaTime)
{
	if (g_bCharacterCreateSceneLoaded && g_CharacterCreateScene.m_pNiNode)
		g_CharacterCreateScene.m_pNiNode->Update((float)g_fAccumTime);

	for (size_t i = 0; i < g_vecCharacterCreateMapObjects.size(); ++i)
	{
		CsNodeObj* pObj = g_vecCharacterCreateMapObjects[i];
		if (pObj && pObj->m_pNiNode)
			pObj->m_pNiNode->Update((float)g_fAccumTime);
	}

	Update3DModel(fDeltaTime);
	UpdateSound();
}

void CCharacterCreate::RenderScreenUI()
{
	RenderScript();
}

void CCharacterCreate::Render3DModel()
{
	static int s_nRender3DLogCounter = 0;

	if (++s_nRender3DLogCounter >= 120)
	{
		s_nRender3DLogCounter = 0;
		CCLog("Render3DModel alive");
	}

	// O Flow chama g_pEngine->ResetRendererCamera() antes deste Render3DModel().
	// Por isso a câmera tem de ser aplicada antes dos object_data e antes do Tamer.
	SetCameraInfo();

	RenderLobbyMap();

	SetCameraInfo();

	if (GetSystem())
	{
		if (s_nRender3DLogCounter == 0)
			CCLog("Render3DModel - Render_TamerModel");

		GetSystem()->Render_TamerModel();
	}
	else
	{
		if (s_nRender3DLogCounter == 0)
			CCLog("Render3DModel warning - GetSystem is NULL");
	}
}

void CCharacterCreate::Render()
{
	m_MainButtonUI.RenderScript();
	m_CharacterListUI.RenderScript();
	m_pCostumSelectWindow.RenderScript();
}

void CCharacterCreate::ResetDevice()
{
	ResetDeviceScript();
	m_MainButtonUI.ResetDeviceScript();
	m_CharacterListUI.ResetDeviceScript();
	m_pCostumSelectWindow.ResetDeviceScript();
}

void CCharacterCreate::CharSelectedChange(unsigned int const& nChangeSelectIdx)
{
	SAFE_POINTER_RET(GetSystem());

	CharacterCreateContents::sTamerCreatedInfo const* pInfo = GetSystem()->GetTamerInfo(nChangeSelectIdx);
	SAFE_POINTER_RET(pInfo);

	if (m_pTamerName)
	{
		std::wstring text = UISTRING_TEXT("CHARACTER_CREATE_TAMER_SELECTED_NAME");
		DmCS::StringFn::ReplaceAll(text, L"#TargetName#", pInfo->m_szName);
		DmCS::StringFn::ReplaceAll(text, L"#Season#", pInfo->m_szSeasonName);
		m_pTamerName->SetText(text.c_str());
	}

	if (m_pTamerDesc)
		m_pTamerDesc->SetText(pInfo->m_szComment.c_str(), 450);

	if (m_pButtonOK)
		m_pButtonOK->SetEnable(pInfo->m_nEnableCreated);

	ChangeCharacterSound(pInfo->m_soundFileName);
	_ResetTamerSkill(pInfo);

	_UpdateTamerBaseState(pInfo->m_mapTamerState);
	_UpdateCostumData(pInfo->m_nFileTableID, pInfo->m_listCostume);
}

void CCharacterCreate::_ResetTamerSkill(CharacterCreateContents::sTamerCreatedInfo const* pInfo)
{
	SAFE_POINTER_RET(m_pTamerSkills);
	m_pTamerSkills->RemoveAllItem();

	SAFE_POINTER_RET(pInfo);
	CharacterCreateContents::LIST_SKILL_INFO_CIT it = pInfo->m_skilList.begin();
	for (int n = 0; it != pInfo->m_skilList.end(); ++it, ++n)
	{
		cString* pItem = NiNew cString;
		SAFE_POINTER_BEK(pItem);

		ICONITEM::eTYPE type = ICONITEM::SKILL1;
		switch ((*it).m_dwIconIndex)
		{
		case CsSkill::IT_CHANGE3:
		case CsSkill::IT_CHANGE1:
		case CsSkill::IT_CHANGE2:
			type = ICONITEM::SKILL_MASK;
			break;
		default:
		{
			if ((*it).m_dwIconIndex >= 4000)
				type = ICONITEM::SKILL4;
			else if ((*it).m_dwIconIndex >= 3000)
				type = ICONITEM::SKILL3;
			else if ((*it).m_dwIconIndex >= 2000)
				type = ICONITEM::SKILL2;
		}
		break;
		}

		cString::sICON* pSkillIcon = NULL;

		if (type != ICONITEM::SKILL_MASK)
			pSkillIcon = pItem->AddIcon(CsPoint(32, 32), type, (*it).m_dwIconIndex % 1000, 1);
		else
			pSkillIcon = pItem->AddIcon(CsPoint(32, 32), type, (*it).m_dwIconIndex, 1);

		if (pSkillIcon)
			pSkillIcon->SetAutoPointerDelete(true);

		cGridListBoxItem* addItem = NiNew cGridListBoxItem(n, CsPoint(32, 32));
		addItem->SetItem(pItem);
		addItem->SetUserData(new sSkillInfo((*it).m_dwSkillCode));
		m_pTamerSkills->AddItem(addItem);
	}
}

void CCharacterCreate::_UpdateTamerBaseState(std::map<int, int> const& baseState)
{
	if (m_pBaseStateAT)	m_pBaseStateAT->SetText(L"");
	if (m_pBaseStateDP)	m_pBaseStateDP->SetText(L"");
	if (m_pBaseStateHP)	m_pBaseStateHP->SetText(L"");
	if (m_pBaseStateDS)	m_pBaseStateDS->SetText(L"");

	std::map<int, int>::const_iterator it = baseState.begin();
	for (; it != baseState.end(); ++it)
	{
		switch (it->first)
		{
		case eBaseStats::AP:
			if (m_pBaseStateAT)
				m_pBaseStateAT->SetText(it->second);
			break;
		case eBaseStats::DP:
			if (m_pBaseStateDP)
				m_pBaseStateDP->SetText(it->second);
			break;
		case eBaseStats::HP:
			if (m_pBaseStateHP)
				m_pBaseStateHP->SetText(it->second);
			break;
		case eBaseStats::DS:
			if (m_pBaseStateDS)
				m_pBaseStateDS->SetText(it->second);
			break;
		}
	}
}

void CCharacterCreate::PressOkButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET(m_pEditBox);
	std::wstring name = m_pEditBox->GetTextAll();

	if (GetSystem())
		GetSystem()->SendCheckTamerName(name);
}

void CCharacterCreate::PressCancelButton(void* pSender, void* pData)
{
	if (GetSystem())
		GetSystem()->GotoBack();
}

void CCharacterCreate::_SelectedTamer(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);

	sTamerInfo* pUserData = dynamic_cast<sTamerInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET(pUserData);

	SAFE_POINTER_RET(GetSystem());
	GetSystem()->SetSelectTamerIdx(pUserData->m_nTableIDX);
}

void CCharacterCreate::SetCameraInfo()
{
	static int s_nCameraLogCounter = 0;

	if (++s_nCameraLogCounter >= 120)
	{
		s_nCameraLogCounter = 0;
		CCLogCamera();
	}

	// A câmera do CsGBCamera segue um target node interno.
	// Aqui movemos esse target livremente em X/Y/Z com WASD/QE.
	sCAMERAINFO ci;
	ci.s_fDist = g_fCreateCamDist;
	ci.s_fFarPlane = 100000.0f;
	ci.s_fInitPitch = CsD2R(g_fCreateCamPitch);
	ci.s_fInitRoll = 0.0f;
	ci.s_ptInitPos = NiPoint3(g_fCreateCamTargetX, g_fCreateCamTargetY, g_fCreateCamTargetZ);

	NiPoint3 kTarget(g_fCreateCamTargetX, g_fCreateCamTargetY, g_fCreateCamTargetZ);

	CAMERA_ST.Reset(&ci);
	CAMERA_ST.ReleaseDistRange();
	CAMERA_ST.ReleaseRotationLimit();
	CAMERA_ST.SetUsingTerrainCollition(false);

	CAMERA_ST.SetTranslate(kTarget);

	if (CAMERA_ST.GetTargetObj())
		CAMERA_ST.GetTargetObj()->SetTranslate(kTarget);

	CAMERA_ST.SetDeltaHeight(g_fCreateCamHeight);
	CAMERA_ST.SetDist(g_fCreateCamDist, true);
	CAMERA_ST.SetRotation(CsD2R(g_fCreateCamYaw), CsD2R(g_fCreateCamPitch));
	CAMERA_ST._UpdateCamera();
}

void CCharacterCreate::_ChangeSelectedLeft(void* pSender, void* pData)
{
	if (m_pTamerListBox)
		m_pTamerListBox->ChangeSelectFront();
}

void CCharacterCreate::_ChangeSelectedRight(void* pSender, void* pData)
{
	if (m_pTamerListBox)
		m_pTamerListBox->ChangeSelectNext();
}

bool CCharacterCreate::Construct(void)
{
	if (!CONTENTSSYSTEM_PTR)
		return false;

	SetSystem(CONTENTSSYSTEM_PTR->GetContents< SystemType >(SystemType::IsContentsIdentity()));
	if (!GetSystem())
		return false;

	GetSystem()->Register(SystemType::eTamer_Selected, this);

	return true;
}

void CCharacterCreate::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch (iNotifiedEvt)
	{
	case SystemType::eTamer_Selected:
	{
		unsigned int nSelectedIdx = 0;
		kStream >> nSelectedIdx;
		CharSelectedChange(nSelectedIdx);
	}
	break;
	}
}