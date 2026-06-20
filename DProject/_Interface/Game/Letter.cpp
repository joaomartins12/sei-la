#include "stdafx.h"
#include "Letter.h"

void cLetter::Destroy()
{	
	cBaseWindow::Delete();	
}

void cLetter::DeleteResource()
{
	if( m_pEditBox )
		m_pEditBox->ReleaseFocus();

	m_strMessage.Delete();
	DeleteScript();

	SAFE_NIDELETE( m_pEditBox );
}

void cLetter::Create(int nValue /* = 0  */)
{	
	SetRootClient( CsPoint( ( g_nScreenWidth - 310 )/ 2, ( g_nScreenHeight - 372 ) / 2 ) );

	cBaseWindow::Init();
	InitScript( "Mail\\Letter2.tga", m_ptRootClient, CsPoint( 310, 372 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );		
	m_pOkButton = AddButton( CsPoint( 116, 340 ), CsPoint( 78, 22 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str(), cWindow::SD_Ba1 ); 	

	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 285, 20 ), CsPoint( 16, 337 ), CsPoint( 16, 15 ), CsRect( CsPoint( 0, 0 ), CsSIZE( 310, 372 ) ), 12 );
	m_strMessage.SetBuddy( m_pScrollBar );
	m_strMessage.SetUnderLineCursor( FONT_BLACK );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_BLACK );
	ti.s_bOutLine = false;
	m_pEditBox = NiNew cEditBox;	
	m_pEditBox->Init( m_pRoot, CsPoint::ZERO, CsPoint( 310, 372 ), &ti, false );	
	m_pEditBox->SetFontLength( 256 );	
	m_pEditBox->EnablePaste();
	m_pEditBox->SetWantReturn( true );
	if( g_pFocusEdit != NULL )
		g_pFocusEdit->ReleaseFocus();
	m_pEditBox->SetFocus();

	memset( m_szMsgTemp, 0, sizeof( m_szMsgTemp ) );

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value= 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void cLetter::Update(float const& fDeltaTime)
{
	if( _tcscmp( m_szMsgTemp, m_pEditBox->GetTextAll() ) != 0 )
	{
		m_strMessage.Delete( false );

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_BLACK );
		ti.s_bOutLine = false;
		_tcscpy_s( m_szMsgTemp, 512, m_pEditBox->GetTextAll() );
		g_pStringAnalysis->Cut( &m_strMessage, 255, m_szMsgTemp, &ti );

		m_pScrollBar->SetRange( CsPoint( 0, m_strMessage.GetSize() ) );
		m_pScrollBar->SetCurPosIndex( m_strMessage.GetSize() );
	}
	

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cLetter::Update_ForMouse()
{	
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	

	if( m_pOkButton->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		g_pCharMng->GetTamerUser()->SetAnimation( 901906 );
		g_pCharMng->GetTamerUser()->GetProp_Effect()->AddEffect_FT( "System\\Event.nif" );
		Close();
		return muReturn;
	}

	if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 310, 27 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cLetter::Render()
{
	RenderScript();	

	m_strMessage.Render( GetRootClient() + CsPoint( 20, 107 ) , 5 );	
}

void cLetter::ResetDevice()
{	
	ResetDeviceScript();

	m_strMessage.ResetDevice();

	if( m_pScrollBar )
		m_pScrollBar->ResetDevice();

	if( m_pEditBox )
		m_pEditBox->ResetDevice();
}

void cLetter::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	m_strMessage.ResetDevice();

	if( m_pScrollBar )
		m_pScrollBar->ResetDevice();

	if( m_pEditBox )
		m_pEditBox->ResetDevice();
}



