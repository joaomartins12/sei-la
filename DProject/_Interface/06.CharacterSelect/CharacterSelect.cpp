#include "stdafx.h"
#include "CharacterSelect.h"
#include "common_vs2019/pPass2.h"

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"
#include "../../MngCollector.h"


// Cache global carregado no GameApp.cpp.
// O CharacterSelect apenas usa Render/Update; não faz load do mapa aqui.
namespace CharacterCreateLobbyMapCache
{
	bool IsLoaded();
	bool Load();
	void Update();
	void Render();
}

namespace
{
	static const DWORD CHARACTER_SELECT_LOBBY_MAP_ID = 4;

	static bool g_bCharacterDeleteWaitingServer = false;

	// Limite final da tela Character Select.
	static const int CHARACTER_SELECT_MAX_SLOT = 3;

	// Botões do lado direito.
	static const int RIGHT_PANEL_W = 220;
	static const int RIGHT_PANEL_X_MARGIN = 32;

	static const int START_BTN_W = 176;
	static const int START_BTN_H = 54;

	static const int SIDE_BTN_W = 137;
	static const int SIDE_BTN_H = 46;

	static const int MAIN_BTN_GAP = 14;

	// Slots em baixo, lado a lado.
	static const int SLOT_BTN_W = 296;
	static const int SLOT_BTN_H = 84;
	static const int SLOT_GAP = 16;
	static const int SLOT_PANEL_SIDE_PADDING = 12;
	static const int SLOT_PANEL_TOP_PADDING = 0;
	static const int SLOT_BOTTOM_MARGIN = 28;

	static int ClampInt(int value, int minValue)
	{
		return value < minValue ? minValue : value;
	}

	static int GetRightPanelX()
	{
		return ClampInt(g_nScreenWidth - RIGHT_PANEL_W - RIGHT_PANEL_X_MARGIN, 10);
	}

	static int GetButtonPanelY()
	{
		const int panelH =
			START_BTN_H +
			MAIN_BTN_GAP +
			SIDE_BTN_H +
			MAIN_BTN_GAP +
			SIDE_BTN_H +
			MAIN_BTN_GAP +
			SIDE_BTN_H;

		return ClampInt((g_nScreenHeight - panelH) / 2, 10);
	}

	static int GetSlotPanelWidth()
	{
		return SLOT_PANEL_SIDE_PADDING * 2 +
			(SLOT_BTN_W * CHARACTER_SELECT_MAX_SLOT) +
			(SLOT_GAP * (CHARACTER_SELECT_MAX_SLOT - 1));
	}

	static int GetSlotPanelHeight()
	{
		return SLOT_PANEL_TOP_PADDING * 2 + SLOT_BTN_H;
	}

	static int GetSlotPanelX()
	{
		return ClampInt((g_nScreenWidth - GetSlotPanelWidth()) / 2, 10);
	}

	static int GetSlotPanelY()
	{
		return ClampInt(g_nScreenHeight - GetSlotPanelHeight() - SLOT_BOTTOM_MARGIN, 10);
	}

	static int GetSlotOffsetX(int nSlotIndex)
	{
		return SLOT_PANEL_SIDE_PADDING + nSlotIndex * (SLOT_BTN_W + SLOT_GAP);
	}

	static int GetSafeSelectedIndex(int nCurrent)
	{
		if (nCurrent < 0)
			return 0;

		if (nCurrent >= CHARACTER_SELECT_MAX_SLOT)
			return CHARACTER_SELECT_MAX_SLOT - 1;

		return nCurrent;
	}
}

CCharacterSelect::sCharUIControls::sCharUIControls()
	: m_pSlotBtn(NULL)
	, m_TamerName(NULL)
	, m_DigimonName(NULL)
	, m_EmptyNLockMsg(NULL)
	, m_pTamerImage(NULL)
	, m_pLockSlotImg(NULL)
	, m_pEmptySlotImg(NULL)
{
}

CCharacterSelect::sCharUIControls::~sCharUIControls()
{
}

void CCharacterSelect::sCharUIControls::SetEmptySlot(bool const& bSlotLock)
{
	if (m_DigimonName)		m_DigimonName->SetVisible(false);
	if (m_TamerName)		m_TamerName->SetVisible(false);

	if (m_EmptyNLockMsg)
	{
		std::wstring emptyMsg;
		if (bSlotLock)
			emptyMsg = UISTRING_TEXT("Que pena acabou os slots gratis");
		else
			emptyMsg = UISTRING_TEXT("CHARACTER_SELECT_EMPTY_SLOT_MESSAGE");

		m_EmptyNLockMsg->SetText(emptyMsg.c_str(), 170);
		m_EmptyNLockMsg->SetVisible(true);
	}

	if (m_pLockSlotImg)
	{
		if (bSlotLock)		m_pLockSlotImg->SetVisible(true);
		else				m_pLockSlotImg->SetVisible(false);
	}

	if (m_pEmptySlotImg)
	{
		if (bSlotLock)		m_pEmptySlotImg->SetVisible(false);
		else				m_pEmptySlotImg->SetVisible(true);
	}

	if (m_pTamerImage)
		m_pTamerImage->SetVisible(false);
}

void CCharacterSelect::sCharUIControls::SetTamerInfo(int const& nLv, std::wstring const& wsName, int const& nTamerImgIdx, bool const& bGray)
{
	if (m_TamerName)
	{
		std::wstring wsMsg;
		DmCS::StringFn::Format(wsMsg, L"Lv.%d %s", nLv, wsName.c_str());
		m_TamerName->SetText(wsMsg.c_str());
		if (bGray)		m_TamerName->SetColor(FONT_GLAY);
		else			m_TamerName->SetColor(FONT_WHITE);
		m_TamerName->SetVisible(true);
	}

	if (m_EmptyNLockMsg)
		m_EmptyNLockMsg->SetVisible(false);

	if (m_pLockSlotImg)
		m_pLockSlotImg->SetVisible(false);

	if (m_pEmptySlotImg)
		m_pEmptySlotImg->SetVisible(false);

	if (m_pTamerImage)
	{
		if (bGray)		m_pTamerImage->SetColor(FONT_GLAY);
		else			m_pTamerImage->SetColor(FONT_WHITE);
		m_pTamerImage->SetImangeIndex(nTamerImgIdx);
		m_pTamerImage->SetVisible(true);
	}
}

void CCharacterSelect::sCharUIControls::SetDigimonInfo(int const& nLv, std::wstring const& wsName, bool const& bGray)
{
	SAFE_POINTER_RET(m_DigimonName);

	std::wstring wsMsg;
	DmCS::StringFn::Format(wsMsg, L"Lv.%d %s", nLv, wsName.c_str());
	m_DigimonName->SetText(wsMsg.c_str());
	if (bGray)	m_DigimonName->SetColor(FONT_GLAY);
	else		m_DigimonName->SetColor(FONT_WHITE);
	m_DigimonName->SetVisible(true);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CCharacterSelect::CCharacterSelect()
	: m_pMapName(NULL)
	, m_pMapNameBack(NULL)
	, m_pOk(NULL)
	, m_pCancel(NULL)
	, m_pCreate(NULL)
	, m_pDelete(NULL)
	, m_pListUpBtn(NULL)
	, m_pListDownBtn(NULL)
	, m_pCharacterList(NULL)
	, m_pEditResistNumber(NULL)
	, m_pDeleteCharCloseBtn(NULL)
	, m_pDeleteCharOkBtn(NULL)
	, m_bLobbyMapLoaded(false)
	, m_dwLobbyMapID(CHARACTER_SELECT_LOBBY_MAP_ID)
{
}

CCharacterSelect::~CCharacterSelect()
{
	if (GetSystem())
	{
		GetSystem()->RestData();
		GetSystem()->UnRegisterAll(this);
	}

	Destory();
}

void CCharacterSelect::Destory()
{
	// O mapa/cache é global e pertence ao GameApp.cpp.
	// Não fazer Release aqui, senão o CharacterCreate também perde o mapa.

	m_mapUIControls.clear();
	DeleteScript();
	m_MainButtonUI.DeleteScript();
	m_CharListUI.DeleteScript();
	m_DeleteWindowUI.DeleteScript();
	g_IME.SetNumberOnly(false);
}

void CCharacterSelect::Init()
{
	MakeBackGroundUI();

	LoadLobbyMap();

	MakeMainButtonUI();

	MakeCharListUI();

	MakeDelectWindow();

	SetCameraInfo();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

bool CCharacterSelect::LoadLobbyMap()
{
	if (m_bLobbyMapLoaded)
		return true;

	m_dwLobbyMapID = CHARACTER_SELECT_LOBBY_MAP_ID;

	// O normal é já estar carregado pelo GameApp.cpp no startup.
	// Se por algum motivo ainda não estiver, tentamos carregar aqui como fallback.
	if (!CharacterCreateLobbyMapCache::IsLoaded())
	{
		OutputDebugStringA("[UI][CharacterSelect] lobby map cache not loaded, fallback Load()\n");

		if (!CharacterCreateLobbyMapCache::Load())
		{
			OutputDebugStringA("[UI][CharacterSelect] lobby map cache Load() failed\n");
			return false;
		}
	}

	m_bLobbyMapLoaded = true;
	OutputDebugStringA("[UI][CharacterSelect] attached to startup lobby map cache\n");

	return true;
}

void CCharacterSelect::ReleaseLobbyMap()
{
	// O cache é global e fica vivo para CharacterSelect + CharacterCreate.
	// O release real acontece em GameApp::OnTerminate().
	m_bLobbyMapLoaded = false;
}

void CCharacterSelect::RenderLobbyMap()
{
	if (!m_bLobbyMapLoaded)
		return;

	CharacterCreateLobbyMapCache::Render();
}

void CCharacterSelect::MakeBackGroundUI()
{
	// Fundo transparente: o cenário 3D vem do cache carregado no GameApp.cpp.
	InitScript(NULL, CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight), false);
}

void CCharacterSelect::MakeMainButtonUI()
{
	m_MainButtonUI.InitScript(NULL, CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight), false);

	if (GetSystem() && GetSystem()->IsPcbangConnect())
		m_MainButtonUI.AddSprite(CsPoint(10, 10), CsPoint(256, 108), "Lobby\\CharacterSelect\\PC_smallbanner.tga", true);

	m_pMapNameBack = m_MainButtonUI.AddSprite(CsPoint((g_nScreenWidth - 446) / 2, 0), CsPoint(446, 118), "Lobby\\TopText_BG.tga");

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_16, FONT_WHITE);
		ti.s_eTextAlign = DT_CENTER;
		m_pMapName = m_MainButtonUI.AddText(&ti, CsPoint(g_nScreenWidth / 2, 49), true);
	}

	const int nPanelX = GetRightPanelX();
	const int nPanelY = GetButtonPanelY();

	const int nStartX = nPanelX + (RIGHT_PANEL_W - START_BTN_W) / 2;
	const int nSideX = nPanelX + (RIGHT_PANEL_W - SIDE_BTN_W) / 2;

	int nCurY = nPanelY;

	// 1) Start em primeiro e maior.
	m_pOk = m_MainButtonUI.AddButton(CsPoint(nStartX, nCurY), CsPoint(START_BTN_W, START_BTN_H), CsPoint(0, 50), "CommonUI\\Simple_btn_L.tga", cWindow::SD_Bu3);
	if (m_pOk)
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_14, FONT_WHITE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("CHARACTER_SELECT_START").c_str());
		m_pOk->SetText(&ti);
		m_pOk->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CCharacterSelect::PressOkButton);
	}

	nCurY += START_BTN_H + MAIN_BTN_GAP;

	// 2) Create.
	m_pCreate = m_MainButtonUI.AddButton(CsPoint(nSideX, nCurY), CsPoint(SIDE_BTN_W, SIDE_BTN_H), CsPoint(0, SIDE_BTN_H), "CommonUI\\Simple_btn_s.tga", cWindow::SD_Bu3);
	if (m_pCreate)
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_14, FONT_WHITE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("CHARACTER_SELECT_CREATE").c_str());
		m_pCreate->SetText(&ti);
		m_pCreate->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CCharacterSelect::PressCreateButton);
	}

	nCurY += SIDE_BTN_H + MAIN_BTN_GAP;

	// 3) Delete.
	m_pDelete = m_MainButtonUI.AddButton(CsPoint(nSideX, nCurY), CsPoint(SIDE_BTN_W, SIDE_BTN_H), CsPoint(0, SIDE_BTN_H), "CommonUI\\Simple_btn_s.tga", cWindow::SD_Bu3);
	if (m_pDelete)
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_14, FONT_YELLOW);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("CHARACTER_SELECT_DELETE").c_str());
		m_pDelete->SetText(&ti);
		m_pDelete->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CCharacterSelect::PressDeleteButton);
	}

	nCurY += SIDE_BTN_H + MAIN_BTN_GAP;

	// 4) Back.
	m_pCancel = m_MainButtonUI.AddButton(CsPoint(nSideX, nCurY), CsPoint(SIDE_BTN_W, SIDE_BTN_H), CsPoint(0, SIDE_BTN_H), "CommonUI\\Simple_btn_s.tga", cWindow::SD_Bu3);
	if (m_pCancel)
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_14, FONT_WHITE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("CHARACTER_SELECT_BACK").c_str());
		m_pCancel->SetText(&ti);
		m_pCancel->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CCharacterSelect::PressCancelButton);
	}
}

void CCharacterSelect::MakeCharListUI()
{
	m_mapUIControls.clear();

	m_CharListUI.InitScript(NULL, CsPoint(GetSlotPanelX(), GetSlotPanelY()), CsPoint(GetSlotPanelWidth(), GetSlotPanelHeight()), false);

	m_pListUpBtn = NULL;
	m_pListDownBtn = NULL;

	m_pCharacterList = NiNew cListBox;
	if (m_pCharacterList)
	{
		m_pCharacterList->Init(NULL, CsPoint(0, 0), CsPoint(GetSlotPanelWidth(), GetSlotPanelHeight()), NULL, false);

		// Não usamos seleção vertical do cListBox, porque agora os 3 slots estão na mesma linha.
		m_pCharacterList->SetAutoItemSelectStateChange(false);

		// Cria 1 item horizontal que contém os 3 cards lado a lado.
		_makeCharacterSlot(0, m_pCharacterList);

		CharacterSelectContents::LIST_CHARLIST const& pInfo = GetSystem()->GetCharacterList();
		CharacterSelectContents::LIST_CHARLIST_CIT it = pInfo.begin();
		int nSlot = 0;

		for (; it != pInfo.end() && nSlot < CHARACTER_SELECT_MAX_SLOT; ++it, ++nSlot)
		{
			_UpdateCharacterSlotUI(nSlot, (*it));
		}

		m_CharListUI.AddChildControl(m_pCharacterList);
	}

	if (GetSystem())
	{
		int nSelectIdx = GetSafeSelectedIndex(GetSystem()->GetSelectCharIdx());
		GetSystem()->SetChangeCharacter(nSelectIdx);
	}
}

void CCharacterSelect::_makeCharacterSlot(int const& nSlotNum, cListBox* pTree)
{
	SAFE_POINTER_RET(pTree);

	cListBoxItem* pAddItem = NiNew cListBoxItem;
	SAFE_POINTER_RET(pAddItem);

	cString* pItem = NiNew cString;
	SAFE_POINTER_RET(pItem);

	for (int i = 0; i < CHARACTER_SELECT_MAX_SLOT; ++i)
	{
		sCharUIControls controls;

		const int nSlotX = GetSlotOffsetX(i);
		const int nSlotY = SLOT_PANEL_TOP_PADDING;

		controls.m_pSlotBtn = NiNew cButton;
		if (controls.m_pSlotBtn)
		{
			controls.m_pSlotBtn->Init(NULL, CsPoint::ZERO, CsPoint(SLOT_BTN_W, SLOT_BTN_H), "Lobby\\CharacterSelect\\CharSelectBT.tga", false);
			controls.m_pSlotBtn->SetTexToken(CsPoint(0, 84));
			controls.m_pSlotBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CCharacterSelect::ChangeCharacter);

			cString::sBUTTON* paddImg = pItem->AddButton(controls.m_pSlotBtn, 0, CsPoint(nSlotX, nSlotY), CsPoint(SLOT_BTN_W, SLOT_BTN_H));
			if (paddImg)
				paddImg->SetAutoPointerDelete(true);
		}

		{
			cText::sTEXTINFO ti2;
			ti2.Init(&g_pEngine->m_FontText, CFont::FS_11);
			controls.m_TamerName = pItem->AddText(&ti2, CsPoint(nSlotX + 83, nSlotY + 22));
		}

		{
			cText::sTEXTINFO ti2;
			ti2.Init(&g_pEngine->m_FontText, CFont::FS_11);
			controls.m_DigimonName = pItem->AddText(&ti2, CsPoint(nSlotX + 83, nSlotY + 46));
		}

		{
			cText::sTEXTINFO ti2;
			ti2.Init(&g_pEngine->m_FontSystem, CFont::FS_11);
			controls.m_EmptyNLockMsg = pItem->AddText(&ti2, CsPoint(nSlotX + 83, nSlotY + 22));
		}

		cImage* pTamerImage = NiNew cImage;
		if (pTamerImage)
		{
			pTamerImage->Init(NULL, CsPoint::ZERO, CsPoint(74, 82), "Lobby\\CharacterSelect\\TamerIcon.tga", false);
			pTamerImage->SetTexToken(CsPoint(74, 0));
			controls.m_pTamerImage = pItem->AddImage(pTamerImage, 0, CsPoint(nSlotX, nSlotY), CsPoint(74, 82));
			if (controls.m_pTamerImage)
				controls.m_pTamerImage->SetAutoPointerDelete(true);
		}

		controls.m_pLockSlotImg = NiNew cSprite;
		if (controls.m_pLockSlotImg)
		{
			controls.m_pLockSlotImg->Init(NULL, CsPoint::ZERO, CsPoint(74, 82), "Lobby\\CharacterSelect\\SlotLock.tga", false);
			cString::sSPRITE* paddImg = pItem->AddSprite(controls.m_pLockSlotImg, CsPoint(nSlotX, nSlotY), CsPoint(74, 82));
			if (paddImg)
				paddImg->SetAutoPointerDelete(true);
		}

		controls.m_pEmptySlotImg = NiNew cSprite;
		if (controls.m_pEmptySlotImg)
		{
			controls.m_pEmptySlotImg->Init(NULL, CsPoint::ZERO, CsPoint(74, 82), "Lobby\\CharacterSelect\\SlotPlus.tga", false);
			cString::sSPRITE* paddImg = pItem->AddSprite(controls.m_pEmptySlotImg, CsPoint(nSlotX, nSlotY), CsPoint(74, 82));
			if (paddImg)
				paddImg->SetAutoPointerDelete(true);
		}

		m_mapUIControls.insert(std::make_pair(i, controls));
	}

	pAddItem->SetItem(pItem);
	pTree->AddItem(pAddItem);
}

void CCharacterSelect::_UpdateCharacterSlotUI(int const& nSlotNum, CharacterSelectContents::sCharacterInfo const& pInfo)
{
	std::map<int, sCharUIControls>::iterator it = m_mapUIControls.find(nSlotNum);
	if (it == m_mapUIControls.end())
		return;

	bool bIsEmptySlot = !pInfo.IsHaveCharInfo();
	if (bIsEmptySlot)
	{
		it->second.SetEmptySlot(pInfo.IsLockSlot());
	}
	else
	{
		it->second.SetTamerInfo(pInfo.m_nTamerLevel,
			pInfo.m_szTamerName,
			pInfo.m_nCharImgIdx,
			pInfo.IsServerChangeWaitCharactor());
		it->second.SetDigimonInfo(pInfo.m_nDigimonLevel,
			pInfo.m_szDigimonName,
			pInfo.IsServerChangeWaitCharactor());
	}
}

void CCharacterSelect::MakeDelectWindow()
{
	m_DeleteWindowUI.InitScript(NULL, "Lobby\\Popup_Notification.tga", CsPoint((g_nScreenWidth - 482) / 2, 150), CsPoint(482, 288), false);

	std::wstring title;
	std::wstring msg;
	switch (GLOBALDATA_ST.Get2ndPassType())
	{
	case GData::eEmail:
	{
		title = UISTRING_TEXT("CHARACTER_SELECT_EMAIL_INPUT_TITLE");
		msg = UISTRING_TEXT("CHARACTER_SELECT_EMAIL_MSG");
	}
	break;
	case GData::eAccountPass:
	{
		title = UISTRING_TEXT("CHARACTER_SELECT_PASSWORD_INPUT_TITLE");
		msg = UISTRING_TEXT("CHARACTER_SELECT_PASSWORD_MSG");
	}
	break;
	case GData::eStringPass:
	{
		title = UISTRING_TEXT("CHARACTER_SELECT_WORD_INPUT_TITLE");
		msg = UISTRING_TEXT("CHARACTER_SELECT_WORD_MSG");

		std::wstring deleteKey;
		DmCS::StringFn::From(deleteKey, DeleteTamerSteamSecondPass);
		DmCS::StringFn::ReplaceAll(msg, L"#Value#", deleteKey);
	}
	break;
	case GData::e2ndNumberPass:
	{
		title = UISTRING_TEXT("CHARACTER_SELECT_2nd_PASSWORD_INPUT_TITLE");
		msg = UISTRING_TEXT("CHARACTER_SELECT_2nd_PASSWORD_MSG");
	}
	break;
	}

	m_DeleteWindowUI.AddTitle(title.c_str(), CsPoint(0, 20), CFont::FS_16, NiColor(1.0f, 0.17f, 0.17f));

	cText::sTEXTINFO ti;
	ti.Init(&g_pEngine->m_FontText, CFont::FS_12);
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText(msg.c_str());
	m_DeleteWindowUI.AddText(&ti, CsPoint(482 / 2, 90));

	m_DeleteWindowUI.AddSprite(CsPoint((482 - 358) / 2, 160), CsPoint(358, 48), "Lobby\\CharacterSelect\\lobby_Password_glow.tga");

	m_pEditResistNumber = NiNew cEditBox;
	if (m_pEditResistNumber)
	{
		cText::sTEXTINFO ti;
		ti.Init(&g_pEngine->m_FontText, CFont::FS_14);
		ti.SetText(_T(""));

		ti.s_eTextAlign = DT_LEFT;
		m_pEditResistNumber->Init(m_DeleteWindowUI.GetRoot(), CsPoint(80, 175), CsPoint(355, 40), &ti, false);
		m_pEditResistNumber->SetFontLength(64);

		m_pEditResistNumber->EnableUnderline();
		m_pEditResistNumber->SetEnableSound(true);
		m_pEditResistNumber->AddEvent(cEditBox::eEditbox_ChangeText, this, &CCharacterSelect::CheckEditBoxText);
		m_DeleteWindowUI.AddChildControl(m_pEditResistNumber);

		switch (GLOBALDATA_ST.Get2ndPassType())
		{
		case GData::eEmail:
			break;
		case GData::eStringPass:
			break;
		case GData::eAccountPass:
			m_pEditResistNumber->GetText()->SetMark(L'*');
			break;
		case GData::e2ndNumberPass:
			m_pEditResistNumber->GetText()->SetMark(L'*');
			break;
		}
	}

	m_pDeleteCharCloseBtn = m_DeleteWindowUI.AddButton(CsPoint(482 / 2 - 150, 230), CsPoint(137, 46), CsPoint(0, 46), "CommonUI\\Simple_btn_Cancel.tga", cWindow::SD_Bu3);
	if (m_pDeleteCharCloseBtn)
	{
		cText::sTEXTINFO ti;
		ti.Init(&g_pEngine->m_FontText, CFont::FS_14);
		ti.SetText(UISTRING_TEXT("CHARACTER_SELECT_DELETE_CANCEL").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pDeleteCharCloseBtn->SetText(&ti);
		m_pDeleteCharCloseBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CCharacterSelect::PressCharDelCloseButton);
	}

	m_pDeleteCharOkBtn = m_DeleteWindowUI.AddButton(CsPoint(482 / 2 + 13, 230), CsPoint(137, 46), CsPoint(0, 46), "CommonUI\\Simple_btn_Ok.tga", cWindow::SD_Bu3);
	if (m_pDeleteCharOkBtn)
	{
		cText::sTEXTINFO ti;
		ti.Init(&g_pEngine->m_FontText, CFont::FS_14);
		ti.SetText(UISTRING_TEXT("CHARACTER_SELECT_DELETE_CONFIRM").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pDeleteCharOkBtn->SetText(&ti);
		m_pDeleteCharOkBtn->SetEnable(false);
		m_pDeleteCharOkBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CCharacterSelect::PressCharDelOkButton);
	}

	m_DeleteWindowUI.SetVisible(false);
}

void CCharacterSelect::showAndhideDeleteWindow(bool bVisible)
{
	/*
	 * Enquanto aguardamos resposta do server para delete,
	 * não deixa reabrir/fechar a popup nem editar novamente.
	 */
	if (g_bCharacterDeleteWaitingServer && bVisible == false)
		return;

	m_DeleteWindowUI.SetVisible(bVisible);

	if (bVisible)
	{
		if (m_pDeleteCharOkBtn)
			m_pDeleteCharOkBtn->SetEnable(false);

		if (m_pEditResistNumber)
		{
			m_pEditResistNumber->SetText(L"");
			m_pEditResistNumber->SetFocus();
		}

		switch (GLOBALDATA_ST.Get2ndPassType())
		{
		case GData::eAccountPass:
			g_IME.SetNumberOnly(false);
			break;

		case GData::eStringPass:
			g_IME.SetNumberOnly(false);
			break;

		case GData::eEmail:
			g_IME.SetNumberOnly(false);
			break;

#ifdef SDM_SECONDPASSWORD_REINFORCE_20180330
		case GData::e2ndNumberPass:
			g_IME.SetNumberOnly(false);
			break;
#else
		case GData::e2ndNumberPass:
			g_IME.SetNumberOnly(true);
			break;
#endif
		}
	}
	else
	{
		if (m_pEditResistNumber)
			m_pEditResistNumber->ReleaseFocus();

		g_IME.SetNumberOnly(false);
	}
}

void CCharacterSelect::_ResetCharacterSlot(int const& nResetSlotNum)
{
	SAFE_POINTER_RET(m_pCharacterList);
	CharacterSelectContents::sCharacterInfo const* pCharInfo = GetSystem()->GetCharacterInfo(nResetSlotNum);

	SAFE_POINTER_RET(pCharInfo);
	_UpdateCharacterSlotUI(nResetSlotNum, *pCharInfo);
}

BOOL CCharacterSelect::UpdateMouse()
{
	/*
	 * Enquanto o delete está pendente no server, bloqueia todos os cliques.
	 * Isto evita:
	 * - clicar Start
	 * - clicar Create
	 * - clicar Delete outra vez
	 * - mudar slot
	 * - voltar ao server select
	 */
	if (g_bCharacterDeleteWaitingServer)
		return TRUE;

	if (m_DeleteWindowUI.IsVisible())
	{
		if (m_pDeleteCharCloseBtn && m_pDeleteCharCloseBtn->Update_ForMouse() == cButton::ACTION_CLICK)
			return TRUE;

		if (m_pDeleteCharOkBtn && m_pDeleteCharOkBtn->Update_ForMouse() == cButton::ACTION_CLICK)
			return TRUE;

		return TRUE;
	}

	if (m_pOk && m_pOk->Update_ForMouse() == cButton::ACTION_CLICK)
		return TRUE;

	if (m_pCreate && m_pCreate->Update_ForMouse() == cButton::ACTION_CLICK)
		return TRUE;

	if (m_pDelete && m_pDelete->Update_ForMouse() == cButton::ACTION_CLICK)
		return TRUE;

	if (m_pCancel && m_pCancel->Update_ForMouse() == cButton::ACTION_CLICK)
		return TRUE;

	if (m_pCharacterList && m_pCharacterList->Update_ForMouse(CURSOR_ST.GetPos()))
		return TRUE;

	return FALSE;
}

BOOL CCharacterSelect::UpdateKeyboard(const MSG& p_kMsg)
{
	/*
	 * Enquanto está a apagar, bloqueia teclado também.
	 */
	if (g_bCharacterDeleteWaitingServer)
		return TRUE;

	if (m_DeleteWindowUI.IsVisible())
		return FALSE;

	const int isBitSet = (DWORD)p_kMsg.lParam & 0x40000000;

	switch (p_kMsg.message)
	{
	case WM_KEYDOWN:
	{
		switch (p_kMsg.wParam)
		{
		case VK_UP:
		{
			if (isBitSet == 0)
			{
				if (m_pCharacterList)
					m_pCharacterList->ChangeSelectFront();
			}

			return TRUE;
		}
		break;

		case VK_DOWN:
		{
			if (isBitSet == 0)
			{
				if (m_pCharacterList)
					m_pCharacterList->ChangeSelectNext();
			}

			return TRUE;
		}
		break;

		case VK_RETURN:
		{
			if (isBitSet == 0)
			{
				if (m_pOk && m_pOk->IsEnable() && GetSystem())
				{
					m_pOk->KeyboardBtnDn();
					GetSystem()->SendGameStart();
				}
			}

			return TRUE;
		}
		break;

		case VK_ESCAPE:
		{
			if (isBitSet == 0)
			{
				if (m_DeleteWindowUI.IsVisible())
				{
					showAndhideDeleteWindow(false);
				}
				else
				{
					if (m_pCancel && GetSystem())
					{
						m_pCancel->KeyboardBtnDn();
						GetSystem()->gotoBack();
					}
				}
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

void CCharacterSelect::Update(float const& fAccumTime, float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
	m_MainButtonUI.UpdateScript(fDeltaTime);
	m_CharListUI.UpdateScript(fDeltaTime);

	if (m_bLobbyMapLoaded)
		CharacterCreateLobbyMapCache::Update();

	if (GetSystem())
		GetSystem()->UpdateModel(fAccumTime, fDeltaTime);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

void CCharacterSelect::RenderScreenUI()
{
	RenderScript();
}

void CCharacterSelect::Render3DModel()
{
	// O Flow faz ResetRendererCamera antes deste render.
	// Aplicamos a câmera antes do mapa e antes do Tamer para manter o mesmo enquadramento.
	SetCameraInfo();

	RenderLobbyMap();

	SetCameraInfo();

	if (GetSystem())
		GetSystem()->RenderModel();
}

void CCharacterSelect::Render()
{
	m_MainButtonUI.RenderScript();
	m_CharListUI.RenderScript();
	m_DeleteWindowUI.RenderScript();
}

void CCharacterSelect::ResetDevice()
{
	ResetDeviceScript();
	m_MainButtonUI.ResetDeviceScript();
	m_CharListUI.ResetDeviceScript();
	m_DeleteWindowUI.ResetDeviceScript();
}

void CCharacterSelect::ChangeCharacterInfoWindow(const int& nSelIdx)
{
	CharacterSelectContents::sCharacterInfo const* pInfo = GetSystem()->GetCharacterInfo(nSelIdx);
	SAFE_POINTER_RET(pInfo);

	bool bHaveCharSlot = pInfo->IsHaveCharInfo();
	bool bIsWaitRelocate = pInfo->IsServerChangeWaitCharactor();
	bool bShowInfo = false;

	if (bHaveCharSlot && !bIsWaitRelocate)
		bShowInfo = true;

	if (m_pDelete)
		m_pDelete->SetEnable(bShowInfo);

	if (m_pOk)
		m_pOk->SetEnable(bShowInfo);

	if (m_pMapNameBack)
	{
		if (bShowInfo || bIsWaitRelocate)
			m_pMapNameBack->SetVisible(true);
		else
			m_pMapNameBack->SetVisible(false);
	}

	if (m_pMapName)
	{
		std::wstring wsMsg;
		NiColor fontColor = FONT_WHITE;
		if (bShowInfo)
			wsMsg = pInfo->m_szMapName;
		else if (bIsWaitRelocate)
		{
			wsMsg = UISTRING_TEXT("CHARACTER_SELECT_SERVERRELOCATE_WAIT_MSG");
			fontColor = FONT_RED;
		}
		else
		{
			wsMsg = L"";
		}

		m_pMapName->SetColor(fontColor);
		m_pMapName->SetText(wsMsg.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CCharacterSelect::Construct(void)
{
	if (!CONTENTSSYSTEM_PTR)
		return false;

	SetSystem(CONTENTSSYSTEM_PTR->GetContents< SystemType >(SystemType::IsContentsIdentity()));
	if (!GetSystem())
		return false;

	GetSystem()->Register(SystemType::eOPEN_DELTE_WINDOW, this);
	GetSystem()->Register(SystemType::eCHAR_DELETE_SUCCESS, this);
	GetSystem()->Register(SystemType::eCHANGE_SELECT_CHAR, this);
	GetSystem()->Register(SystemType::eADD_NEW_CHARACTER, this);
	return true;
}

void CCharacterSelect::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch (iNotifiedEvt)
	{
	case SystemType::eOPEN_DELTE_WINDOW:
		showAndhideDeleteWindow(true);
		break;

	case SystemType::eCHAR_DELETE_SUCCESS:
	{
		/*
		 * O server já respondeu ao delete.
		 * Neste ponto o CharacterSelectContents já mostrou a mensagem de sucesso
		 * e limpou os dados do personagem.
		 *
		 * Agora podemos desbloquear a UI.
		 */
		g_bCharacterDeleteWaitingServer = false;

		int nDelSlot = 0;

		kStream >> nDelSlot;

		_ResetCharacterSlot(nDelSlot);

		if (m_pCharacterList)
		{
			if (m_pCharacterList->GetSelectedItemIdx() == nDelSlot)
			{
				if (m_pDelete)
					m_pDelete->SetEnable(false);

				if (m_pOk)
					m_pOk->SetEnable(false);

				if (m_pMapNameBack)
					m_pMapNameBack->SetVisible(false);

				if (m_pMapName)
				{
					std::wstring wsMsg;
					NiColor fontColor = FONT_WHITE;

					wsMsg = L"";

					m_pMapName->SetColor(fontColor);
					m_pMapName->SetText(wsMsg.c_str());
				}
			}

			m_pCharacterList->UnSelectionItem(nDelSlot);
		}

		if (m_pDeleteCharCloseBtn)
			m_pDeleteCharCloseBtn->SetEnable(true);

		if (m_pDeleteCharOkBtn)
			m_pDeleteCharOkBtn->SetEnable(false);
	}
	break;

	case SystemType::eADD_NEW_CHARACTER:
	{
		int nNewIdx = 0;
		kStream >> nNewIdx;
		_ResetCharacterSlot(nNewIdx);

		if (m_pCharacterList)
		{
			m_pCharacterList->UnSelectionItem(nNewIdx);
			m_pCharacterList->SetSelectedItemFromIdx(nNewIdx);
		}
	}
	break;

	case SystemType::eCHANGE_SELECT_CHAR:
	{
		int nSelIdx = 0;
		kStream >> nSelIdx;
		ChangeCharacterInfoWindow(nSelIdx);
	}
	break;
	}
}

void CCharacterSelect::PressOkButton(void* pSender, void* pData)
{
	if (GetSystem())
		GetSystem()->SendGameStart();
}

void CCharacterSelect::PressCancelButton(void* pSender, void* pData)
{
	if (GetSystem())
		GetSystem()->gotoBack();
}

void CCharacterSelect::PressCreateButton(void* pSender, void* pData)
{
	if (!GetSystem())
		return;

	bool bHasFreeSlot = false;

	for (int i = 0; i < CHARACTER_SELECT_MAX_SLOT; ++i)
	{
		CharacterSelectContents::sCharacterInfo const* pInfo = GetSystem()->GetCharacterInfo(i);

		if (!pInfo)
			continue;

		if (!pInfo->IsHaveCharInfo() && !pInfo->IsLockSlot())
		{
			bHasFreeSlot = true;
			break;
		}
	}

	if (!bHasFreeSlot)
	{
		cPrintMsg::PrintMsg(cPrintMsg::CHARSELECT_MAX_CHAR_COUNT);
		return;
	}

	GetSystem()->gotoCharCreate();
}

void CCharacterSelect::PressDeleteButton(void* pSender, void* pData)
{
	cPrintMsg::PrintMsg(cPrintMsg::CHARSELECT_DELETE_CHAR);
}

void CCharacterSelect::PressCharDelCloseButton(void* pSender, void* pData)
{
	if (g_bCharacterDeleteWaitingServer)
		return;

	showAndhideDeleteWindow(false);
}

void CCharacterSelect::PressCharDelOkButton(void* pSender, void* pData)
{
	if (g_bCharacterDeleteWaitingServer)
		return;

	SAFE_POINTER_RET(m_pEditResistNumber);

	cText* pText = m_pEditResistNumber->GetText();

	SAFE_POINTER_RET(pText);

	cText::sTEXTINFO* pTextInfo = pText->GetTextInfo();

	SAFE_POINTER_RET(pTextInfo);

	if (!GetSystem())
		return;

	/*
	 * Envia o pedido.
	 * Se o pedido foi aceite localmente, bloqueia a UI até vir resposta do server.
	 */
	if (GetSystem()->SendDeleteCharacter(pTextInfo->GetText()))
	{
		g_bCharacterDeleteWaitingServer = true;

		if (m_pDeleteCharOkBtn)
			m_pDeleteCharOkBtn->SetEnable(false);

		if (m_pDeleteCharCloseBtn)
			m_pDeleteCharCloseBtn->SetEnable(false);

		if (m_pEditResistNumber)
			m_pEditResistNumber->ReleaseFocus();

		g_IME.SetNumberOnly(false);

		/*
		 * Fecha a janela de password, mas a UI fica bloqueada
		 * até receber RECV_CHAR_DELETE_RESULT.
		 */
		m_DeleteWindowUI.SetVisible(false);
	}
}

void CCharacterSelect::CheckEditBoxText(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	SAFE_POINTER_RET(m_pDeleteCharOkBtn);

	TCHAR* szResistNumber = static_cast<TCHAR*>(pData);
	m_pDeleteCharOkBtn->SetEnable((_tcslen(szResistNumber) >= 4));
}

void CCharacterSelect::SetCameraInfo()
{
	sCAMERAINFO ci;
	ci.s_fDist = 300.0f;
	ci.s_fFarPlane = 100000.0f;
	ci.s_fInitPitch = CsD2R(-3.0f);
	ci.s_fInitRoll = 0.0f;
	ci.s_ptInitPos = NiPoint3(3000.0f, 17948.0f, 425.0f);

	NiPoint3 kTarget(3000.0f, 17948.0f, 425.0f);

	CAMERA_ST.Reset(&ci);
	CAMERA_ST.ReleaseDistRange();
	CAMERA_ST.ReleaseRotationLimit();
	CAMERA_ST.SetUsingTerrainCollition(false);

	CAMERA_ST.SetTranslate(kTarget);

	if (CAMERA_ST.GetTargetObj())
		CAMERA_ST.GetTargetObj()->SetTranslate(kTarget);

	CAMERA_ST.SetDeltaHeight(120.0f);
	CAMERA_ST.SetDist(300.0f, true);
	CAMERA_ST.SetRotation(CsD2R(0.0f), CsD2R(-3.0f));
	CAMERA_ST._UpdateCamera();
}

void CCharacterSelect::ChangeCharacter(void* pSender, void* pData)
{
	for (std::map<int, sCharUIControls>::iterator it = m_mapUIControls.begin(); it != m_mapUIControls.end(); ++it)
	{
		if (it->second.m_pSlotBtn == pSender)
		{
			if (GetSystem())
				GetSystem()->SetChangeCharacter(it->first);

			return;
		}
	}

	if (m_pCharacterList && GetSystem())
	{
		int nSelIdx = GetSafeSelectedIndex(m_pCharacterList->GetSelectedItemIdx());
		GetSystem()->SetChangeCharacter(nSelIdx);
	}
}

void CCharacterSelect::SeletedCharacter(void* pSender, void* pData)
{
	if (GetSystem())
		GetSystem()->SendGameStart();
}

void CCharacterSelect::_ListShowUp(void* pSender, void* pData)
{
	if (GetSystem())
	{
		int nSelIdx = GetSafeSelectedIndex(GetSystem()->GetSelectCharIdx());

		if (nSelIdx > 0)
			GetSystem()->SetChangeCharacter(nSelIdx - 1);
	}
}

void CCharacterSelect::_ListShowDown(void* pSender, void* pData)
{
	if (GetSystem())
	{
		int nSelIdx = GetSafeSelectedIndex(GetSystem()->GetSelectCharIdx());

		if (nSelIdx < CHARACTER_SELECT_MAX_SLOT - 1)
			GetSystem()->SetChangeCharacter(nSelIdx + 1);
	}
}