#include "stdafx.h"
#include "TutorialSkipUI.h"


cTutorialSkipUI::cTutorialSkipUI() : m_pSkipButton(NULL)
{

}

cTutorialSkipUI::~cTutorialSkipUI()
{

}

bool cTutorialSkipUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void cTutorialSkipUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}

cBaseWindow::eWINDOW_TYPE cTutorialSkipUI::GetWindowType()
{
	return WT_TUTORIAL_SKIP_UI;
}

void cTutorialSkipUI::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	CsPoint ptSize( 128, 48 );
	InitScript(NULL, CsPoint(DEFAULT_SCREEN_WIDTH - ptSize.x - 32, ptSize.y + 220), ptSize, false, IFREGION_X::RIGHT);

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_14, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( UISTRING_TEXT( "TUTORIAL_EVENT_SKIP" ).c_str() );
	m_pSkipButton = AddButton( CsPoint::ZERO, ptSize, cButton::IMAGE_NOR_13_R, &ti );
	if( m_pSkipButton )
		m_pSkipButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cTutorialSkipUI::_OnClickSkipButton );
}

void cTutorialSkipUI::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );
}

cBaseWindow::eMU_TYPE cTutorialSkipUI::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( MUT_OUT_WINDOW == muReturn )
		return muReturn;

	if( m_pSkipButton && m_pSkipButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	return muReturn;
}

void cTutorialSkipUI::Destroy()
{
	cBaseWindow::Delete();
}

void cTutorialSkipUI::DeleteResource()
{
	DeleteScript();
}

void cTutorialSkipUI::Render()
{
	RenderScript();
}

void cTutorialSkipUI::ResetDevice()
{
	ResetDeviceScript();
}

void cTutorialSkipUI::_OnClickSkipButton(void* pSender, void* pData)
{
	GetSystem()->SkipTutorialEvent();
}
