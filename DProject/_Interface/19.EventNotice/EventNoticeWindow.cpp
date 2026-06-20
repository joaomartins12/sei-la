
#include "stdafx.h"
#include "EventNoticeWindow.h"

EventNoticeWindow::EventNoticeWindow() :m_pCloseBtn(NULL)
{
}

EventNoticeWindow::~EventNoticeWindow()
{
}

// ---------------------------------------------------------------------

bool EventNoticeWindow::Close(bool bSound)
{
	return cBaseWindow::Close(bSound);
}

void EventNoticeWindow::Destroy()
{
	cBaseWindow::Delete();
}

void EventNoticeWindow::DeleteResource()
{
	if (GetSystem())
		GetSystem()->UnRegisterAll(this);

	DeleteScript();
}

void EventNoticeWindow::ResetDevice()
{
	ResetDeviceScript();
}

// ---------------------------------------------------------------------

void EventNoticeWindow::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

cBaseWindow::eMU_TYPE EventNoticeWindow::Update_ForMouse(void)
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if (muReturn == MUT_OUT_WINDOW)
		return MUT_NONE;

	if (m_pCloseBtn && m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE)
		return muReturn;

	return MUT_NONE;
}

// ---------------------------------------------------------------------

void EventNoticeWindow::Render(void)
{
	RenderScript();
	EndRenderScript();
}

// ---------------------------------------------------------------------

void EventNoticeWindow::Create(int nValue)
{
	if (!cBaseWindow::Init())
		return;

	int windowWidth = 562;		// Largura da janela
	int windowHeight = 415;		// Altura da janela

	int posX = (g_nScreenWidth / 2) - (windowWidth / 2);		// Posiçao X da janela
	int posY = (g_nScreenHeight / 2) - (windowHeight / 2);		// Posiçao Y da janela

	InitScript("eventpopup\\event_popup_background.png", CsPoint(posX, posY), CsPoint(windowWidth, windowHeight), true);
	AddTitle(UISTRING_TEXT("EVENT NOTICE").c_str(), CsPoint(0, 0));

	m_pCloseBtn = AddButton(CsPoint(530, 2), CsPoint(30, 28), CsPoint(0, 28), "CommonUI\\CommonUI_close.tga");

	if (m_pCloseBtn)
		m_pCloseBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &EventNoticeWindow::CloseWindow);
}

// ---------------------------------------------------------------------

void EventNoticeWindow::CloseWindow(void* pSender, void* pData)
{
	DBG("Event Notice Screen - Close Button Clicked !!");

	GetSystem()->Req_CloseEventNotice(NULL);
}

// ---------------------------------------------------------------------

bool EventNoticeWindow::Construct(void)
{
	if (!CONTENTSSYSTEM_PTR)
		return false;

	SetSystem(CONTENTSSYSTEM_PTR->GetContents< SystemType >(SystemType::IsContentsIdentity()));

	if (!GetSystem())
		return false;

	return true;
}

// ---------------------------------------------------------------------

void EventNoticeWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}