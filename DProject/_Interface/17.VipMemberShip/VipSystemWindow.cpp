#include "stdafx.h"

#include "VipSystemWindow.h"

CVipSystemWindow::CVipSystemWindow() : m_pOpenButton(NULL), m_pkRemainTime(NULL), m_pTimerBack(NULL)
{
}

CVipSystemWindow::~CVipSystemWindow()
{
}

cBaseWindow::eWINDOW_TYPE CVipSystemWindow::GetWindowType()
{
	return WT_VIP_BUTTON;
}

void CVipSystemWindow::Destroy()
{
	cBaseWindow::Delete();
}

void CVipSystemWindow::DeleteResource()
{
	if (GetSystem())
		GetSystem()->UnRegisterAll(this);

	DeleteScript();
}

void CVipSystemWindow::Create(int nValue /* = 0  */)
{
	if (cBaseWindow::Init() == false)
		return;

	InitScript(NULL, CsPoint(765, 40), CsPoint(64, 51), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM, true);

	m_pOpenButton = AddButton(CsPoint::ZERO, CsPoint(64, 51), CsPoint(0, 51), "MemberShip\\membership_Button_Icon.tga");
	if (m_pOpenButton)
	{
		m_pOpenButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CVipSystemWindow::OpenWindow);
		m_pOpenButton->SetButtonEffect(cSpriteAni::LOOP, CsPoint::ZERO, CsPoint(64, 51), "MemberShip\\membership_Button_Icon_ani.tga", NULL, 8, false, CsPoint(64, 0), cSpriteAni::SPRITE_POS);
		m_pOpenButton->SetEffectAniTime(0.2f);
		m_pOpenButton->SetEffectOn(cButton::eSTATE::NORMAL, true);
	}

	m_pTimerBack = NiNew cSprite;
	if (m_pTimerBack)
	{
		m_pTimerBack->Init(GetRoot(), CsPoint(0, 31), CsPoint(64, 20), NiColorA(0.3f, 0.3f, 0.3f, 0.5f), false);
		AddChildControl(m_pTimerBack);
	}

	m_pkRemainTime = NiNew cUI_TimerProgressBar;
	if (m_pkRemainTime)
	{
		m_pkRemainTime->Init(GetRoot(), CsPoint(0, 31), CsPoint(64, 20), false);

		cText::sTEXTINFO ti;
		ti.Init(&g_pEngine->m_FontSystem, CFont::FS_12);
		ti.s_Color = NiColor(0.86f, 0.45f, 0.0f);
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		ti.SetText(L" ");

		m_pkRemainTime->SetStringControl(&ti);
		m_pkRemainTime->SetShowValueType(cUI_TimerProgressBar::eRemainingTime);

		std::wstring wsDay = UISTRING_TEXT("VIPMEMBERSHIP_WINDOW_REMAINTIME_DAY");
		std::wstring wsHour = UISTRING_TEXT("VIPMEMBERSHIP_WINDOW_REMAINTIME_HOUR");
		std::wstring wsMin = UISTRING_TEXT("VIPMEMBERSHIP_WINDOW_REMAINTIME_MIN");
		std::wstring wsSec = UISTRING_TEXT("VIPMEMBERSHIP_WINDOW_REMAINTIME_SEC");

		m_pkRemainTime->SetTimerText(wsDay, wsHour, wsMin, wsSec);
		m_pkRemainTime->SetTimerType(cUI_TimerProgressBar::eFirstTime);
		m_pkRemainTime->SetRenderText(true);

		//m_pkRemainTime->AddEvent( cUI_ProgressBar::eProgressbar_Done, this, &CVipSystemWindow::_SendRequestRankingDataUpdate );
		AddChildControl(m_pkRemainTime);
	}
}

cBaseWindow::eMU_TYPE CVipSystemWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if (muReturn == MUT_OUT_WINDOW)
		return muReturn;

	if (m_pOpenButton && m_pOpenButton->Update_ForMouse() != cButton::ACTION_NONE)
		return muReturn;

	return muReturn;
}

void CVipSystemWindow::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

void CVipSystemWindow::Render()
{
	RenderScript();
}

void CVipSystemWindow::ResetDevice()
{
	ResetDeviceScript();
}

void CVipSystemWindow::OpenWindow(void* pSender, void* pData)
{
	// if( GetSystem() )
	//     GetSystem()->OnClick_OpenBanner( "http://www.digimonmasters.com/event/2018/membership/index.aspx" );

	if (GetSystem())
		GetSystem()->ShowInGameCashshopItemSelect();
}

void CVipSystemWindow::_SetMemberShipEndTime(bool bVisible, float fRemainTime)
{
	if (m_pTimerBack)
		m_pTimerBack->SetVisible(bVisible);

	SAFE_POINTER_RET(m_pkRemainTime);

	m_pkRemainTime->SetVisible(bVisible);
	m_pkRemainTime->Reset();

	if (bVisible)
	{
		m_pkRemainTime->SetTimer(0.0f, fRemainTime);
		m_pkRemainTime->SetProgressPos(0.0f);
		m_pkRemainTime->Start();
	}
}

bool CVipSystemWindow::Construct(void)
{
	if (!CONTENTSSYSTEM_PTR)
		return false;

	SetSystem(CONTENTSSYSTEM_PTR->GetContents< SystemType >(SystemType::IsContentsIdentity()));
	if (!GetSystem())
		return false;

	GetSystem()->Register(SystemType::eOnMemberShip, this);
	GetSystem()->Register(SystemType::eOffMemberShip, this);

	return true;
}

void CVipSystemWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch (iNotifiedEvt)
	{
	case SystemType::eOnMemberShip:
	{
		float fEndTiem = 0.0f;
		kStream >> fEndTiem;
		_SetMemberShipEndTime(true, fEndTiem);
	}
	break;

	case SystemType::eOffMemberShip:
		_SetMemberShipEndTime(false, 0);
		break;
	}
}