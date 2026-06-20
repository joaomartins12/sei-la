
#include "stdafx.h"
#include "SeasonPassWindow.h"

SeasonPassWindow::SeasonPassWindow() :m_pCloseBtn(NULL)
{
}

SeasonPassWindow::~SeasonPassWindow()
{
}

// ---------------------------------------------------------------------

bool SeasonPassWindow::Close(bool bSound)
{
	return cBaseWindow::Close(bSound);
}

void SeasonPassWindow::Destroy()
{
	cBaseWindow::Delete();
}

void SeasonPassWindow::DeleteResource()
{
	if (GetSystem())
		GetSystem()->UnRegisterAll(this);

	DeleteScript();
}

void SeasonPassWindow::ResetDevice()
{
	ResetDeviceScript();
}

// ---------------------------------------------------------------------

void SeasonPassWindow::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

cBaseWindow::eMU_TYPE SeasonPassWindow::Update_ForMouse(void)
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if (muReturn == MUT_OUT_WINDOW)
		return MUT_NONE;

	if (m_pCloseBtn && m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE)
		return muReturn;

	return MUT_NONE;
}

// ---------------------------------------------------------------------

void SeasonPassWindow::Render(void)
{
	RenderScript();
	EndRenderScript();
}

// ---------------------------------------------------------------------

void SeasonPassWindow::Create(int nValue)
{
	if (!cBaseWindow::Init())
		return;

	int windowWidth = 892;		// Largura da janela
	int windowHeight = 684;		// Altura da janela

	int posX = (g_nScreenWidth / 2) - (windowWidth / 2);		// Posiçao X da janela
	int posY = (g_nScreenHeight / 2) - (windowHeight / 2);		// Posiçao Y da janela

	InitScript("SeasonPass\\pass_main_bg.png", CsPoint(posX, posY), CsPoint(windowWidth, windowHeight), true);
	AddTitle(UISTRING_TEXT("SEASON PASS").c_str(), CsPoint(0, 15));

	m_pCloseBtn = AddButton(CsPoint(850, 15), CsPoint(30, 28), CsPoint(0, 28), "CommonUI\\CommonUI_close.tga");

	if (m_pCloseBtn)
		m_pCloseBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &SeasonPassWindow::CloseWindow);
}

// ---------------------------------------------------------------------

void SeasonPassWindow::CloseWindow(void* pSender, void* pData)
{
	GetSystem()->Req_Close(NULL);
}

// ---------------------------------------------------------------------

bool SeasonPassWindow::Construct(void)
{
	if (!CONTENTSSYSTEM_PTR)
		return false;

	SetSystem(CONTENTSSYSTEM_PTR->GetContents< SystemType >(SystemType::IsContentsIdentity()));

	if (!GetSystem())
		return false;

	return true;
}

// ---------------------------------------------------------------------

void SeasonPassWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}