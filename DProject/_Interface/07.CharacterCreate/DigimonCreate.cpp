#include "stdafx.h"
#include "DigimonCreate.h"

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"


// Cache global carregado no GameApp.cpp.
// O DigimonCreate apenas usa Render/Update; não faz load do mapa aqui.
namespace CharacterCreateLobbyMapCache
{
	bool IsLoaded();
	bool Load();
	void Update();
	void Render();
}

CDigimonCreate::CDigimonCreate() :m_pIntroSound(NULL), m_pButtonOK(NULL), m_pButtonCancel(NULL)
, bIMECheck(false), m_pEditBox(NULL), m_pDigimonName(NULL), m_fBackupMusic(1.0f), m_pDigimonDesc(NULL),
m_pDigimonSkills(NULL), m_pDigimonAttributeType(NULL), m_pDigimonAttributeName(NULL)
{
}

CDigimonCreate::~CDigimonCreate()
{
	if (GetSystem())
		GetSystem()->UnRegisterAll(this);

	Destory();
}


void CDigimonCreate::Destory()
{

	g_IME.SetLockConidateList(false);

	DeleteScript();
	m_MainButtonUI.DeleteScript();
	m_DigimonListWindowUI.DeleteScript();

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

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

bool CDigimonCreate::Init()
{
	SetCameraInfo();

	GetSystem()->MakeCreatedDigimonList();

	// Fundo transparente: o cenário 3D vem do cache carregado no GameApp.cpp.
	InitScript(NULL, CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight), false);

	// O normal é já estar carregado pelo GameApp.cpp.
	// Fallback para evitar tela vazia caso algum fluxo entre aqui antes do startup cache.
	if (!CharacterCreateLobbyMapCache::IsLoaded())
	{
		OutputDebugStringA("[UI][DigimonCreate] lobby map cache not loaded, fallback Load()\n");
		CharacterCreateLobbyMapCache::Load();
	}

	m_fBackupMusic = g_pResist->m_Global.s_fMusic;
	g_IME.SetLockConidateList(true);

	makeMainButtonUI();
	makeDigimonListWindowUI();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// 메인 버튼 UI 생성
//////////////////////////////////////////////////////////////////////////////////////////////
void CDigimonCreate::makeMainButtonUI()
{
	m_MainButtonUI.InitScript(NULL, CsPoint::ZERO, CsPoint(g_nScreenWidth, g_nScreenHeight), false);

	m_MainButtonUI.AddSprite(CsPoint((g_nScreenWidth - 446) / 2, 0), CsPoint(446, 118), "Lobby\\TopText_BG.tga");

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_16, FONT_WHITE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("CHARACTER_CREATE_DIGIMON_TITLE").c_str());
		m_MainButtonUI.AddText(&ti, CsPoint(g_nScreenWidth / 2, 49), true);
	}

	m_pButtonCancel = m_MainButtonUI.AddButton(CsPoint((g_nScreenWidth - 176) / 2 - 400, g_nScreenHeight - 80), CsPoint(176, 50), CsPoint(0, 50), "CommonUI\\Simple_btn_L.tga", cWindow::SD_Bu3);
	if (m_pButtonCancel)
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_14, NiColor::WHITE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("CHARACTER_CREATE_BACK").c_str()); // 뒤로
		m_pButtonCancel->SetText(&ti);
		m_pButtonCancel->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CDigimonCreate::PressCancelButton);
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
			m_pEditBox->SetEmptyMsgText(UISTRING_TEXT("CHARACTER_CREATE_DIGIMON_NAME_INPUT_EMPTY_MSG").c_str(), NiColor(0.5f, 0.5f, 0.5f));
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
		m_pButtonOK->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CDigimonCreate::PressOkButton);
	}
}

// DIGIMON WINDOW UI
void CDigimonCreate::makeDigimonListWindowUI()
{
	m_DigimonListWindowUI.InitScript("Lobby\\CharacterCreate\\ListBack.tga", CsPoint(g_nScreenWidth - 530, (g_nScreenHeight - 592) / 2), CsPoint(512, 592), false);

	m_pDigimonListBox = NiNew cGridListBox;
	SAFE_POINTER_RET(m_pDigimonListBox);

	m_pDigimonListBox->Init(GetRoot(), CsPoint(30, 30), CsPoint(460, 220)
		, CsPoint::ZERO, CsPoint(72, 72), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0);
	m_pDigimonListBox->AddEvent(cGridListBox::GRID_SELECT_CHANGE_ITEM, this, &CDigimonCreate::_SelectedDigimon);
	m_DigimonListWindowUI.AddChildControl(m_pDigimonListBox);
	m_pDigimonListBox->SetMouseOverImg("Lobby\\CharacterCreate\\digimon_glow.tga");
	m_pDigimonListBox->SetSelectedImg("Lobby\\CharacterCreate\\digimon_glow.tga");
	m_pDigimonListBox->SetBackOverAndSelectedImgRender(false);

	cScrollBar* pScrollBar = NiNew cScrollBar;
	if (pScrollBar)
	{
		pScrollBar->Init(cScrollBar::TYPE_SEALMASTER, NULL, CsPoint(0, 0), CsPoint(16, 260), cScrollBar::GetDefaultBtnSize(), CsRect(CsPoint(0, 0), CsPoint(450, 220)), 2, false);
		//pScrollBar->SetEnableRenderFromEnableScroll(true);
		m_pDigimonListBox->SetScrollBar(pScrollBar);
	}

	m_DigimonListWindowUI.AddSprite(CsPoint(6, 266), CsPoint(500, 38), "Lobby\\CharacterCreate\\char_gline.tga");

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_16, FONT_GREEN);
		m_pDigimonName = m_DigimonListWindowUI.AddText(&ti, CsPoint(21, 276), true);

		m_pDigimonAttributeType = NiNew cImage;
		m_pDigimonAttributeType->Init(m_DigimonListWindowUI.GetRoot(), CsPoint(450, 267), CsPoint(36, 36), "CommonUI\\Digimon_Attributetype.tga", false);
		m_pDigimonAttributeType->SetTexToken(CsPoint(36, 0));
		m_DigimonListWindowUI.AddChildControl(m_pDigimonAttributeType);

		cText::sTEXTINFO ti2;
		ti2.Init(CFont::FS_13, FONT_GOLD);
		ti2.s_eTextAlign = DT_RIGHT | DT_VCENTER;
		m_pDigimonAttributeName = m_DigimonListWindowUI.AddText(&ti2, CsPoint(440, 287), true);
	}


	m_DigimonListWindowUI.AddSprite(CsPoint(6, 304), CsPoint(494, 105), "Lobby\\CharacterCreate\\TextBack_line.tga");
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_11, FONT_WHITE);
		m_pDigimonDesc = m_DigimonListWindowUI.AddText(&ti, CsPoint(21, 309), true);
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, FONT_NEWGOLD);
		ti.SetText(UISTRING_TEXT("CHARACTER_CREATE_DIGIMON_EVOLUTION_INFO").c_str());
		m_DigimonListWindowUI.AddText(&ti, CsPoint(24, 413), true);

		m_pDigimonEvolutions = NiNew cGridListBox;
		if (m_pDigimonEvolutions)
		{
			m_pDigimonEvolutions->Init(GetRoot(), CsPoint(21, 438), CsPoint(460, 45), CsPoint(10, 0), CsPoint(44, 44),
				cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 8);
			m_DigimonListWindowUI.AddChildControl(m_pDigimonEvolutions);

			m_pDigimonEvolutions->AddEvent(cGridListBox::GRID_SELECT_CHANGE_ITEM, this, &CDigimonCreate::_SelectedDigimonEvolution);
			m_pDigimonEvolutions->SetMouseOverImg("Icon\\Mask_Over.tga");
			m_pDigimonEvolutions->SetSelectedImg("Icon\\Mask_Over.tga");
			//m_pDigimonEvolutions->SetBackGorundImg( " " );
			m_pDigimonEvolutions->SetBackOverAndSelectedImgRender(false);

			cScrollBar* pScrollBar = NiNew cScrollBar;
			if (pScrollBar)
			{
				pScrollBar->Init(cScrollBar::TYPE_SEALMASTER, NULL, CsPoint(0, 0), CsPoint(16, 85), cScrollBar::GetDefaultBtnSize(), CsRect(CsPoint(0, 0), CsPoint(450, 220)), 2, false);
				//pScrollBar->SetEnableRenderFromEnableScroll(true);
				m_pDigimonEvolutions->SetScrollBar(pScrollBar);
			}
		}
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, FONT_NEWGOLD);
		ti.SetText(UISTRING_TEXT("CHARACTER_CREATE_DIGIMON_HAVE_SKILL").c_str());
		m_DigimonListWindowUI.AddText(&ti, CsPoint(24, 511), true);
	}

	m_pDigimonSkills = NiNew cGridListBox;
	if (m_pDigimonSkills)
	{
		m_pDigimonSkills->Init(GetRoot(), CsPoint(21, 536), CsPoint(460, 35), CsPoint(10, 0), CsPoint(32, 32), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0);
		m_pDigimonSkills->SetMouseOverImg("Icon\\Mask_Over.tga");
		m_pDigimonSkills->SetBackOverAndSelectedImgRender(false);
		m_DigimonListWindowUI.AddChildControl(m_pDigimonSkills);
	}

	setDigimonList();
}

void CDigimonCreate::setDigimonList()
{
	SAFE_POINTER_RET(m_pDigimonListBox);

	m_pDigimonListBox->RemoveAllItem();
	CharacterCreateContents::LIST_DIGIMON_INFO const& pList = GetSystem()->GetMakeDigimonList();
	CharacterCreateContents::LIST_DIGIMON_INFO_CIT it = pList.begin();
	for (int n = 0; it != pList.end(); ++it, ++n)
	{
		cString* pItem = NiNew cString;
		SAFE_POINTER_BEK(pItem);

		// 테이머 이지미
		cSprite* pImage = NiNew cSprite;	// 1
		if (pImage)
		{
			NiColor color = NiColor::WHITE;
			if (!(*it).m_nEnableCreated)
				color = NiColor(0.3f, 0.3f, 0.3f);

			pImage->Init(NULL, CsPoint::ZERO, CsPoint(64, 64), (*it).m_sDigimonIconName.c_str(), false, color, false);
			cString::sSPRITE* sSprite = pItem->AddSprite(pImage, CsPoint(4, 4), CsPoint(64, 64));
			if (sSprite)
				sSprite->SetAutoPointerDelete(true);
		}

		cGridListBoxItem* addItem = NiNew cGridListBoxItem(n, CsPoint(72, 72));
		addItem->SetItem(pItem);
		addItem->SetUserData(new sDigimonInfo((*it).m_nFileTableID, (*it).m_nEnableCreated));
		m_pDigimonListBox->AddItem(addItem);
	}

	m_pDigimonListBox->SetSelectedItemFromIdx(0, true);
}

void CDigimonCreate::SetCameraInfo()
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
//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

void CDigimonCreate::UpdateSound()
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

void CDigimonCreate::ChangeCharacterSound(std::string const& soundFileName)
{
	SAFE_POINTER_RET(g_pSoundMgr);
	if (m_pIntroSound)
	{
		g_pSoundMgr->StopSound(m_pIntroSound);
		m_pIntroSound = NULL;
	}

	if (soundFileName.empty())
		return;

	std::string fileName = soundFileName;
	m_pIntroSound = g_pSoundMgr->PlaySystemSound(const_cast<char*>(fileName.c_str()));
	// 사운드 플레이가 된다면
	if (m_pIntroSound)
		m_pIntroSound->SetVolume(1.0f);
}

BOOL CDigimonCreate::UpdateMouse()
{
	if (m_pButtonOK && m_pButtonOK->Update_ForMouse() == cButton::ACTION_CLICK)
		return TRUE;

	if (m_pButtonCancel && m_pButtonCancel->Update_ForMouse() == cButton::ACTION_CLICK)
		return TRUE;

	if (m_pDigimonListBox && m_pDigimonListBox->Update_ForMouse(CURSOR_ST.GetPos()))
		return TRUE;

	if (m_pDigimonEvolutions && m_pDigimonEvolutions->Update_ForMouse(CURSOR_ST.GetPos()))
	{
		cGridListBoxItem const* pOverGrid = m_pDigimonEvolutions->GetMouseOverItem();
		if (pOverGrid)
		{
			sEvolutionInfo* pEvolInfo = dynamic_cast<sEvolutionInfo*>(pOverGrid->GetUserData());
			if (pEvolInfo)
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip(pOverGrid->GetWorldPos(), CsPoint(44, 44), 360,
					cTooltip::QUICK_EVOL_SIMPLE, pEvolInfo->m_nBaseDigimonIDX, pEvolInfo->m_nTableIDX);
		}
		return TRUE;
	}

	if (m_pDigimonSkills && m_pDigimonSkills->Update_ForMouse(CURSOR_ST.GetPos()))
	{
		cGridListBoxItem const* pOverGrid = m_pDigimonSkills->GetMouseOverItem();
		if (pOverGrid)
		{
			sSkillInfo* pSkillInfo = dynamic_cast<sSkillInfo*>(pOverGrid->GetUserData());
			if (pSkillInfo)
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip(pOverGrid->GetWorldPos(), CsPoint(32, 32), 360, cTooltip::SKILL_SIMPLE, pSkillInfo->m_nTableIDX, 1);
		}
		return TRUE;
	}

	return FALSE;
}

BOOL CDigimonCreate::UpdateKeyboard(const MSG& p_kMsg)
{
	const int isBitSet = (DWORD)p_kMsg.lParam & 0x40000000;

	switch (p_kMsg.message)
	{
	case WM_IME_NOTIFY:
	{
		if (!g_IME.IsLockChangeLanguage())
		{
			if (g_IME.OnNotify(GAMEAPP_ST.GetHWnd(), p_kMsg.wParam, p_kMsg.lParam) == true)
			{
				bIMECheck = true;
			}
		}
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

#ifndef NOT_ENTER_CREATENAME	// 대만에서는 글자 조합 선택 시 Enter로 하므로 조합 중 Enter로 생성되지 않도록 함
			if (m_pButtonOK && m_pButtonOK->IsEnable())
			{
				m_pButtonOK->KeyboardBtnUp();
				PressOkButton(m_pButtonOK, NULL);
				return TRUE;
			}
#endif
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

void CDigimonCreate::Update(float fDeltaTime)
{
	UpdateScript(fDeltaTime);
	m_MainButtonUI.UpdateScript(fDeltaTime);
	m_DigimonListWindowUI.UpdateScript(fDeltaTime);

	CharacterCreateLobbyMapCache::Update();

	GetSystem()->Update_DigimonModel(fDeltaTime);
	UpdateSound();
}


//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

void CDigimonCreate::RenderScreenUI()
{
	RenderScript();
}

void CDigimonCreate::Render3DModel()
{
	// O Flow faz ResetRendererCamera antes deste render.
	// Aplicamos a câmera antes do mapa e antes do Digimon para manter o mesmo enquadramento.
	SetCameraInfo();

	CharacterCreateLobbyMapCache::Render();

	SetCameraInfo();

	GetSystem()->Render_DigimonModel();
}

void CDigimonCreate::Render()
{
	m_MainButtonUI.RenderScript();
	m_DigimonListWindowUI.RenderScript();
}


void CDigimonCreate::ResetDevice()
{
	ResetDeviceScript();
	m_MainButtonUI.ResetDeviceScript();
	m_DigimonListWindowUI.ResetDeviceScript();
}

void CDigimonCreate::_SelectedDigimon(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);

	sDigimonInfo* pUserData = dynamic_cast<sDigimonInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET(pUserData);

	SAFE_POINTER_RET(GetSystem());

	// 디지몬 선택을 했을 때 진화 정보 리스트 갱신 및 선택하게 함.
	CharacterCreateContents::sDigimonCreatedInfo const* pInfo = GetSystem()->GetDigimonInfo(pUserData->m_nTableIDX);
	SAFE_POINTER_RET(pInfo);

	// 디지몬 진화 트리 셋팅
	_ResetEvolutionTree(pInfo);

	ChangeCharacterSound(pInfo->m_soundFileName);

	if (m_pDigimonDesc)
		m_pDigimonDesc->SetText(pInfo->m_szComment.c_str(), 450);

	if (m_pButtonOK)
		m_pButtonOK->SetEnable(pInfo->m_nEnableCreated);
}

// 디지몬 진화 트리 셋팅
void CDigimonCreate::_ResetEvolutionTree(CharacterCreateContents::sDigimonCreatedInfo const* pInfo)
{
	SAFE_POINTER_RET(m_pDigimonEvolutions);
	m_pDigimonEvolutions->RemoveAllItem();
	SAFE_POINTER_RET(pInfo);

	CharacterCreateContents::LIST_DIGIMON_EVO_INFO_CIT it = pInfo->m_EvolutionInfo.begin();
	for (int n = 0; it != pInfo->m_EvolutionInfo.end(); ++it, ++n)
	{
		cString* pItem = NiNew cString;
		SAFE_POINTER_BEK(pItem);

		cSprite* IconBg = NiNew cSprite;
		if (IconBg)
		{
			IconBg->Init(NULL, CsPoint::ZERO, CsPoint(44, 44), "Lobby\\CharacterCreate\\Evol_IconBG.tga", false);
			cString::sSPRITE* sSprite = pItem->AddSprite(IconBg, CsPoint::ZERO, CsPoint(44, 44));
			if (sSprite)
				sSprite->SetAutoPointerDelete(true);
		}

		cImage* pEvoImage = NiNew cImage;	// 1
		if (pEvoImage)
		{
			pEvoImage->Init(NULL, CsPoint(6, 6), CsPoint(85, 91), (*it).m_szDigimonIcon.c_str(), false, false);
			pEvoImage->SetTexToken(CsPoint(32, 0));
			cString::sIMAGE* sImg = pItem->AddImage(pEvoImage, 0, CsPoint::ZERO, CsPoint(32, 32));
			if (sImg)
				sImg->SetAutoPointerDelete(true);
		}

		cGridListBoxItem* addItem = NiNew cGridListBoxItem(n, CsPoint(44, 44));
		addItem->SetItem(pItem);
		addItem->SetUserData(new sEvolutionInfo(pInfo->m_nFileTableID, (*it).m_nFileTableID));

		m_pDigimonEvolutions->AddItem(addItem);
	}

	// 디지몬 진화 정보가 셋팅이 끝났을 경우 앞에 있는 것 선택하게 함.
	m_pDigimonEvolutions->SetSelectedItemFromIdx(0);
}

// 디지몬 진화 목록에서 디지몬을 선택했을 때
// 선택한 디지몬의 스킬 트리 및 디지몬 이름/속성 셋팅
// 디지몬 3D 변경
void CDigimonCreate::_SelectedDigimonEvolution(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);

	sEvolutionInfo* pUserData = dynamic_cast<sEvolutionInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET(pUserData);

	SAFE_POINTER_RET(GetSystem());
	GetSystem()->SetSelectDigimonIdx(pUserData->m_nBaseDigimonIDX, pUserData->m_nTableIDX);
}

void CDigimonCreate::_ResetDigimonSkill(CharacterCreateContents::LIST_SKILL_INFO const& skillList)
{
	SAFE_POINTER_RET(m_pDigimonSkills);
	m_pDigimonSkills->RemoveAllItem();

	CharacterCreateContents::LIST_SKILL_INFO_CIT it = skillList.begin();
	for (int n = 0; it != skillList.end(); ++it, ++n)
	{
		cString* pItem = NiNew cString;
		SAFE_POINTER_BEK(pItem);

		ICONITEM::eTYPE type = ICONITEM::SKILL1;
		switch ((*it).m_dwIconIndex)
		{
		case CsSkill::IT_CHANGE3:
		case CsSkill::IT_CHANGE1:
		case CsSkill::IT_CHANGE2:	type = ICONITEM::SKILL_MASK;	break;
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
		m_pDigimonSkills->AddItem(addItem);
	}
}

void CDigimonCreate::_DigimonSelectedChange(unsigned int const& nChangeSelectIdx, unsigned int const& nEvolSelectIdx)
{
	CharacterCreateContents::sDigimonEvolutionInfo const* pEvolInfo = GetSystem()->GetDigimonEvolutionInfo(nChangeSelectIdx, nEvolSelectIdx);
	SAFE_POINTER_RET(pEvolInfo);

	if (m_pDigimonName)
	{
		std::wstring text = UISTRING_TEXT("CHARACTER_CREATE_DIGIMON_SELECTED_NAME");
		DmCS::StringFn::ReplaceAll(text, L"#TargetName#", pEvolInfo->m_szName);
		DmCS::StringFn::ReplaceAll(text, L"#EvolType#", pEvolInfo->m_strEvolType);
		m_pDigimonName->SetText(text.c_str());
	}

	if (m_pDigimonAttributeType)
		m_pDigimonAttributeType->SetState((cImage::eSTATE)(pEvolInfo->m_nDigimonAttributeType - 1));

	if (m_pDigimonAttributeName)
		m_pDigimonAttributeName->SetText(pEvolInfo->m_strAttributeType.c_str());


	_ResetDigimonSkill(pEvolInfo->m_skilList);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CDigimonCreate::Construct(void)
{
	if (!CONTENTSSYSTEM_PTR)
		return false;

	SetSystem(CONTENTSSYSTEM_PTR->GetContents< SystemType >(SystemType::IsContentsIdentity()));
	if (!GetSystem())
		return false;

	GetSystem()->Register(SystemType::eDigimon_Selected, this);

	return true;
}

void CDigimonCreate::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch (iNotifiedEvt)
	{
	case SystemType::eDigimon_Selected:
	{
		unsigned int nSelectedIdx = 0;
		unsigned int nEvolIdx = 0;
		kStream >> nSelectedIdx >> nEvolIdx;
		_DigimonSelectedChange(nSelectedIdx, nEvolIdx);
	}
	break;
	}
}

void CDigimonCreate::PressOkButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET(m_pEditBox);
	std::wstring name = m_pEditBox->GetTextAll();
	GetSystem()->SendCreate(name);
}

void CDigimonCreate::PressCancelButton(void* pSender, void* pData)
{
	GetSystem()->GotoBack();
}