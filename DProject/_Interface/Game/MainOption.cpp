
#include "stdafx.h"
#include "MainOption.h"

cMainOption::cMainOption():m_pCancelButton(NULL),m_pInterface(NULL),m_pGraphicBtn(NULL),m_pSoundBtn(NULL),
m_pMacroKeyBtn(NULL),m_pLogOutBtn(NULL),m_pGameExitBtn(NULL)
#ifdef VERSION_KOR
,m_pUserGradeBtn(NULL)
#endif
{

}

void cMainOption::Destroy()
{ 
	cBaseWindow::Delete();	
}

void cMainOption::DeleteResource()
{
	DeleteScript();
}

void cMainOption::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "MainOption\\Option_Win.tga", CsPoint( g_nScreenWidth/2 - 267/2, g_nScreenHeight/2 - 329/2 ), CsPoint( 267, 329 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	AddTitle( UISTRING_TEXT( "MAINOPTION_TITLE_OPTION" ).c_str() );

	m_pCancelButton = AddButton( CsPoint( 247, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	

	m_pInterface = AddButton( CsPoint( 43, 45 ), CsPoint( 181, 27 ), cButton::IMAGE_NOR_9, UISTRING_TEXT( "MAINOPTION_BTN_INTERFACE" ).c_str() );	
	m_pGraphicBtn = AddButton( CsPoint( 43, 83 ), CsPoint( 181, 27 ), cButton::IMAGE_NOR_9, UISTRING_TEXT( "MAINOPTION_BTN_GRAPHIC" ).c_str() );	
	m_pSoundBtn = AddButton( CsPoint( 43, 121 ), CsPoint( 181, 27 ), cButton::IMAGE_NOR_9, UISTRING_TEXT( "MAINOPTION_BTN_SOUND" ).c_str() );	
	m_pMacroKeyBtn = AddButton( CsPoint( 43, 159 ), CsPoint( 181, 27 ), cButton::IMAGE_NOR_9, UISTRING_TEXT( "MAINOPTION_BTN_SHORTCUT" ).c_str() );

#ifdef VERSION_KOR
	m_pUserGradeBtn = AddButton( CsPoint( 43, 197 ), CsPoint( 181, 27 ), cButton::IMAGE_NOR_9, UISTRING_TEXT( "MAINOPTION_BTN_USER_RATING" ).c_str() );
	m_pLogOutBtn = AddButton( CsPoint( 43, 235 ), CsPoint( 181, 27 ), cButton::IMAGE_NOR_9, UISTRING_TEXT( "MAINOPTION_BTN_LOGOUT" ).c_str() );
#else
	m_pLogOutBtn = AddButton( CsPoint( 43, 197 ), CsPoint( 181, 27 ), cButton::IMAGE_NOR_9, UISTRING_TEXT( "MAINOPTION_BTN_LOGOUT" ).c_str() );
#endif

	m_pGameExitBtn = AddButton( CsPoint( 43, 284 ), CsPoint( 181, 27 ), cButton::IMAGE_NOR_5, UISTRING_TEXT( "MAINOPTION_BTN_EXIT" ).c_str() );
}

void cMainOption::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cMainOption::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
	{
		m_pCancelButton->Update_ForMouse();
		m_pInterface->Update_ForMouse();
		m_pSoundBtn->Update_ForMouse();
		m_pMacroKeyBtn->Update_ForMouse();
#ifdef VERSION_KOR
		m_pUserGradeBtn->Update_ForMouse();
#endif
		m_pLogOutBtn->Update_ForMouse();
		m_pGameExitBtn->Update_ForMouse();
		return muReturn;
	}

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		Close();
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	switch( m_pInterface->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_OPTION_INTERFACE );
		Close( false );
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	switch( m_pGraphicBtn->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_OPTION_GRAPHIC );
		Close( false );
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	switch( m_pSoundBtn->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_OPTION_SOUND );
		Close( false );
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	switch( m_pMacroKeyBtn->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_OPTION_MACRO );
		Close( false );
	case cButton::ACTION_DOWN:
		return muReturn;
	}

#ifdef VERSION_KOR
	switch( m_pUserGradeBtn->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		g_pGameIF->GetDynamicIF(cBaseWindow::WT_OPTION_USERGRADE );
		//cUserGrade::BeginGrade( false );
		Close( false );
	case cButton::ACTION_DOWN:
		return muReturn;
	}
#endif

	switch( m_pLogOutBtn->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		if( g_pCharMng->GetTamerUser()->IsBattle() == true )
		{
			cPrintMsg::PrintMsg( 16002 );
		}
		else
		{
			g_pCharMng->GetTamerUser()->DeletePath();
			for( int i=0; i<MAX_DIGIMONUSER_COUNT; ++i )
			{
				if( g_pCharMng->GetDigimonUser( i ) )
					g_pCharMng->GetDigimonUser( i )->DeletePath();
			}

			cMessageBox::InsertList( UISTRING_TEXT( "MAINOPTION_MSGBOX_ALERT" ).c_str(), _T( "" ), cPrintMsg::LOG_OUT, CsTalk_Message::MT_CANCEL );

			unsigned int nNextExpRate = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_EXP_EVENT_NEXT_EXPRATE, &nNextExpRate );			
			if( nNextExpRate > 0 )
			{
				cMessageBox* pExitWindow = cMessageBox::GetMessageBox( cPrintMsg::LOG_OUT );
				if( pExitWindow )
					pExitWindow->SetValue2( nNextExpRate );
			}

			Close( false );
		}
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	switch( m_pGameExitBtn->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		if( g_pCharMng->GetTamerUser()->IsBattle() == true )
		{
			cPrintMsg::PrintMsg( 16002 );
		}
		else
		{
			g_pCharMng->GetTamerUser()->DeletePath();
			for( int i=0; i<MAX_DIGIMONUSER_COUNT; ++i )
			{
				if( g_pCharMng->GetDigimonUser( i ) )
					g_pCharMng->GetDigimonUser( i )->DeletePath();
			}

			cMessageBox::InsertList( UISTRING_TEXT( "MAINOPTION_MSGBOX_ALERT" ).c_str(), _T( "" ), cPrintMsg::GAME_EXIT, CsTalk_Message::MT_CANCEL );

			unsigned int nNextExpRate = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_EXP_EVENT_NEXT_EXPRATE, &nNextExpRate );
			if( nNextExpRate > 0 )
			{
				cMessageBox* pExitWindow = cMessageBox::GetMessageBox( cPrintMsg::GAME_EXIT );
				if( pExitWindow )
					pExitWindow->SetValue2( nNextExpRate );
			}

			Close( false );
		}
	case cButton::ACTION_DOWN:
		return muReturn;
	}
	
	_UpdateMoveWindow_ForMouse();
	return muReturn;
}

void cMainOption::Render()
{
	RenderScript();
}

void cMainOption::ResetDevice()
{	
	ResetDeviceScript();
} 