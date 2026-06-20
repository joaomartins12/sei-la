#include "stdafx.h"
#include "TutorialHelpUI.h"

cTutorialHelpUI::cTutorialHelpUI() : m_pCloseBtn(NULL)
{

}

cTutorialHelpUI::~cTutorialHelpUI()
{

}

bool cTutorialHelpUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void cTutorialHelpUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}

cBaseWindow::eWINDOW_TYPE cTutorialHelpUI::GetWindowType()
{
	return WT_TUTORIAL_HELP_UI;
}

void cTutorialHelpUI::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	CsPoint ptSprtSize( 990, 541 );
	CsPoint ptStartPos(DEFAULT_SCREEN_WIDTH - 48, (DEFAULT_SCREEN_HEIGHT - ptSprtSize.y + 5) >> 1);
	CsPoint ptSprtPos( -ptSprtSize.x + 32, -5 );
	InitScript(NULL, ptStartPos, CsPoint(28, 28), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER);
	AddSprite( ptSprtPos, ptSprtSize, "Tutorial\\TutorialHelp.tga" );
	m_pCloseBtn = AddButton( CsPoint::ZERO, CsPoint( 28, 28 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );
	if( m_pCloseBtn )
		m_pCloseBtn->SetControlName( "KeyHelpCloseBtn" );
}

void cTutorialHelpUI::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );
}

cBaseWindow::eMU_TYPE cTutorialHelpUI::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( MUT_OUT_WINDOW == muReturn )
		return muReturn;

	if( m_pCloseBtn && m_pCloseBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		Close( false );
		return muReturn;
	}

// 	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 990, 32 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
// 		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cTutorialHelpUI::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cTutorialHelpUI::DeleteResource()
{
	DeleteScript();
}

void cTutorialHelpUI::Render()
{
	RenderScript();
}

void cTutorialHelpUI::ResetDevice()
{
	ResetDeviceScript();
}

bool cTutorialHelpUI::OnMacroKey(const MSG& p_kMsg)
{
	return false;
}

bool cTutorialHelpUI::OnEscapeKey()
{
	Close( false );
	return true;
}
