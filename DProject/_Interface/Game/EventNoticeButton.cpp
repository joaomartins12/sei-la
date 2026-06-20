
#include "stdafx.h"
#include "EventNoticeButton.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

// ------------------------------------------------------------------------------------------

CEventNoticeButton::CEventNoticeButton() : m_pOpenButton(NULL)
{
}

CEventNoticeButton::~CEventNoticeButton()
{
}

// ------------------------------------------------------------------------------------------

void CEventNoticeButton::Destroy()
{
	cBaseWindow::Delete();
}

void CEventNoticeButton::DeleteResource()
{
	DeleteScript();

	if (GetSystem())
		GetSystem()->UnRegisterAll(this);
}

// ------------------------------------------------------------------------------------------

void CEventNoticeButton::Create(int nValue /* = 0  */)
{
	if (!cBaseWindow::Init())
		return;

	InitScript(NULL, CsPoint(700, -20), CsPoint(64, 51), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM, true);

	m_pOpenButton = AddButton(CsPoint::ZERO, CsPoint(64, 51), CsPoint(0, 51), "eventpopup\\event_popup_openbtn.png");
	
	if (m_pOpenButton)
	{
		m_pOpenButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CEventNoticeButton::OpenWindow);
		m_pOpenButton->SetButtonEffect(cSpriteAni::LOOP, CsPoint::ZERO, CsPoint(64, 51), "eventpopup\\event_popup_openbtn.png", NULL, 3, false, CsPoint(0, 51), cSpriteAni::SPRITE_POS);
		m_pOpenButton->SetEffectAniTime(0.2f);
		m_pOpenButton->SetEffectOn(cButton::eSTATE::NORMAL, true);
	}
}

// ------------------------------------------------------------------------------------------

cBaseWindow::eMU_TYPE CEventNoticeButton::Update_ForMouse()
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

void CEventNoticeButton::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

void CEventNoticeButton::Render()
{
	RenderScript();
}

void CEventNoticeButton::ResetDevice()
{
	ResetDeviceScript();
}

// ------------------------------------------------------------------------------------------

void CEventNoticeButton::OpenWindow(void* pSender, void* pData)
{
	//if(GetSystem())
		GetSystem()->Req_OpenEventNotice(NULL);
}

// ------------------------------------------------------------------------------------------

void CEventNoticeButton::ShowToolTip(void* pData)
{
	std::wstring wsTooltip;
	DmCS::StringFn::Format(wsTooltip, L"%s", UISTRING_TEXT("Event Notice").c_str());

	CsPoint ptPos;
	ptPos.x = m_ptRootClient.x + m_ptRootSize.x;
	ptPos.y = m_ptRootClient.y - m_ptRootSize.y + 10;	// arrumar altura

	TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text(ptPos, CsPoint::ZERO, wsTooltip.c_str());
}

// ------------------------------------------------------------------------------------------

bool CEventNoticeButton::Construct(void)
{
	if (!CONTENTSSYSTEM_PTR)
		return false;
	
	SetSystem(CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ));

	if (!GetSystem())
		return false;
	
	return true;
}

// ------------------------------------------------------------------------------------------

void CEventNoticeButton::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

// ------------------------------------------------------------------------------------------
