
#include "stdafx.h"
#include "HelpText.h"

cHelpText::cHelpText()
{
}

void cHelpText::Destroy()
{
	cBaseWindow::Delete();	
}

void cHelpText::DeleteResource()
{
	DeleteScript();

	m_Title.Delete();
	m_Body.Delete();

	//SAFE_NIDELETE( m_pLogo );
}

void cHelpText::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint vInitPos = CsPoint( 2, 160 );		
	InitScript( "Help\\Win.tga", vInitPos, CsPoint( (int)IF_HELPTEXT_WIDTH, (int)IF_HELPTEXT_HEIGHT ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, true );
	AddTitle( UISTRING_TEXT( "COMMON_TXT_HELP" ).c_str() );
	m_pCancelButton = AddButton( CsPoint( IF_HELPTEXT_WIDTH - 22, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );

// 	m_pLogo = NiNew cSprite;
// 	m_pLogo->Init( m_pRoot, CsPoint::ZERO, CsPoint( 211, 234 ), "Help\\Help_Logo.tga", true );

	m_Title.Init();
	m_Body.Init();	
}

void cHelpText::Update(float const& fDeltaTime)
{
	nsDIRECTSHOW::UpdateDShow();
	m_Title.Update(fDeltaTime);
	m_Body.Update(fDeltaTime);

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cHelpText::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		Close();
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
	if( m_Title.Update_ForMouse( ptLocal - CsPoint( 7, 0 ) ) == true )
		return muReturn;

	if( m_Body.Update_ForMouse( ptLocal ) == true )
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 25 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
	
	return muReturn;
}

void cHelpText::Render()
{
	RenderScript();

	m_Title.Render( GetRootClient() + GetTitleStartPos() + CsPoint( 7, 0 ) );
	m_Body.Render( GetRootClient() + GetBodyStartPos() );

// 	if( m_Title.GetSelectTitle() == NULL )
// 	{
// 		m_pLogo->Render( CsPoint( 125, 132 ) );
// 	}
}

void cHelpText::ResetDevice()
{	
	ResetDeviceScript();

	m_Title.ResetDevice();
	m_Body.ResetDevice();

	//m_pLogo->ResetDevice();
}

void cHelpText::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	m_Title.ResetDevice();
	m_Body.ResetDevice();

	//m_pLogo->ResetDevice();
}

