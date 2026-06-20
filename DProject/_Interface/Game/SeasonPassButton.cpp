
#include "stdafx.h"
#include "SeasonPassButton.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

// ------------------------------------------------------------------------------------------

cSeasonPassButton::cSeasonPassButton() : m_pOpenButton(NULL)
{
}

cSeasonPassButton::~cSeasonPassButton()
{
}

// ------------------------------------------------------------------------------------------

void cSeasonPassButton::Destroy()
{
	cBaseWindow::Delete();
}

void cSeasonPassButton::DeleteResource()
{
	DeleteScript();

	if (GetSystem())
		GetSystem()->UnRegisterAll(this);
}

// ------------------------------------------------------------------------------------------

void cSeasonPassButton::Create(int nValue /* = 0  */)
{
	if (!cBaseWindow::Init())
		return;

	InitScript(NULL, CsPoint(770, -15), CsPoint(50, 50), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM, true);

	m_pOpenButton = AddButton(CsPoint::ZERO, CsPoint(50, 50), CsPoint(0, 50), "seasonpass\\pass_btn_effect.png");

	if (m_pOpenButton)
	{
		m_pOpenButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSeasonPassButton::OpenWindow);
		//m_pOpenButton->SetButtonEffect(cSpriteAni::LOOP, CsPoint::ZERO, CsPoint(50, 50), "seasonpass\\pass_btn_effect.png", NULL, 3, false, CsPoint(50, 0), cSpriteAni::SPRITE_PAGE);
		//m_pOpenButton->SetEffectAniTime(0.2f);
		//m_pOpenButton->SetEffectOn(cButton::eSTATE::NORMAL, true);
	}
}

// ------------------------------------------------------------------------------------------

cBaseWindow::eMU_TYPE cSeasonPassButton::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if (muReturn == MUT_OUT_WINDOW)
	{
		if (m_pOpenButton)
			m_pOpenButton->Update_ForMouse();

		return muReturn;
	}

	if (m_pOpenButton && m_pOpenButton->Update_ForMouse() == cButton::ACTION_ON)
		ShowToolTip(NULL);

	return muReturn;
}

// ------------------------------------------------------------------------------------------

void cSeasonPassButton::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

void cSeasonPassButton::Render()
{
	RenderScript();
}

void cSeasonPassButton::ResetDevice()
{
	ResetDeviceScript();
}

// ------------------------------------------------------------------------------------------

void cSeasonPassButton::OpenWindow(void* pSender, void* pData)
{
	//if(GetSystem())
	GetSystem()->Req_Open(NULL);
}

// ------------------------------------------------------------------------------------------

void cSeasonPassButton::ShowToolTip(void* pData)
{
	std::wstring wsTooltip;
	DmCS::StringFn::Format(wsTooltip, L"%s", UISTRING_TEXT("SeasonPass").c_str());

	CsPoint ptPos;
	ptPos.x = m_ptRootClient.x + m_ptRootSize.x;
	ptPos.y = m_ptRootClient.y - m_ptRootSize.y + 10;	// arrumar altura

	TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text(ptPos, CsPoint::ZERO, wsTooltip.c_str());
}

// ------------------------------------------------------------------------------------------

bool cSeasonPassButton::Construct(void)
{
	if (!CONTENTSSYSTEM_PTR)
		return false;

	SetSystem(CONTENTSSYSTEM_PTR->GetContents< SystemType >(SystemType::IsContentsIdentity()));

	if (!GetSystem())
		return false;

	return true;
}

// ------------------------------------------------------------------------------------------

void cSeasonPassButton::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

// ------------------------------------------------------------------------------------------
