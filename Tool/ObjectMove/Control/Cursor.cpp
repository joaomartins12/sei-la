#include "stdafx.h"
#include "Cursor.h"

CCursor g_Cursor;

void CCursor::Init()
{
	m_nModifier = NiInputMouse::MMOD_NONE;
	m_DbClickTime.SetDeltaTime( 200 );
	m_bDoubleClick = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CCursor::Update( HWND hWnd )
{
	if( ( g_pEngine->m_pkMouse == NULL )||( g_pEngine->m_pkKeyboard == NULL ) )
		return;

	m_bDoubleClick = false;
	GetCursorPos( &m_ptCursorPos );
	ScreenToClient( hWnd, &m_ptCursorPos );

	NiInputMouse* pkMouse = g_pEngine->m_pkMouse;
	pkMouse->GetStatus();
	m_nModifier = pkMouse->GetModifiers();
	if( ( pkMouse->GetPositionDelta( NiInputMouse::NIM_AXIS_Z, m_nWheel ) == false )|| m_nWheel == 0 )
	{		
		m_nWheel = INVALIDE_WHEEL;
	}

	// 프레스 우선순위를 낮게 두겠다.
	if( pkMouse->ButtonWasPressed( NiInputMouse::NIM_LEFT ) )
	{
		bool bEnableCheckTime = true;
		if( m_DbClickTime.IsSameValue( NiInputMouse::NIM_LEFT ) )
		{			
			if( m_DbClickTime.IsEnable() )
			{
				m_DbClickTime.SetDisable();
				m_bDoubleClick = true;
				bEnableCheckTime = false;
			}
		}
		if( bEnableCheckTime == true )
		{
			m_DbClickTime.SetValue( NiInputMouse::NIM_LEFT );
			m_DbClickTime.SetEnable();
		}
		SetButtonState( CURSOR::LBUTTON_DOWN );
	}
	else if( pkMouse->ButtonWasReleased( NiInputMouse::NIM_LEFT ) )
	{
		SetButtonState( CURSOR::LBUTTON_UP );
	}	
	else if( pkMouse->ButtonWasPressed( NiInputMouse::NIM_RIGHT ) )
	{
		bool bEnableCheckTime = true;
		if( m_DbClickTime.IsSameValue( NiInputMouse::NIM_RIGHT ) )
		{			
			if( m_DbClickTime.IsEnable() )
			{
				m_DbClickTime.SetDisable();
				m_bDoubleClick = true;
				bEnableCheckTime = false;
			}
		}
		if( bEnableCheckTime == true )
		{
			m_DbClickTime.SetValue( NiInputMouse::NIM_RIGHT );
			m_DbClickTime.SetEnable();
		}
		SetButtonState( CURSOR::RBUTTON_DOWN );
	}
	else if( pkMouse->ButtonWasReleased( NiInputMouse::NIM_RIGHT ) )
	{
		SetButtonState( CURSOR::RBUTTON_UP );
	}	
	else if( pkMouse->ButtonWasPressed( NiInputMouse::NIM_MIDDLE ) )
	{
		SetButtonState( CURSOR::MBUTTON_DOWN );
	}
	else if( pkMouse->ButtonWasReleased( NiInputMouse::NIM_MIDDLE ) )
	{
		SetButtonState( CURSOR::MBUTTON_UP );
	}
	else if( pkMouse->ButtonIsDown( NiInputMouse::NIM_RIGHT ) )
	{
		SetButtonState( CURSOR::RBUTTON_PRESS );
	}
	else if( pkMouse->ButtonIsDown( NiInputMouse::NIM_LEFT ) )
	{
		SetButtonState( CURSOR::LBUTTON_PRESS );
	}	
	else if( pkMouse->ButtonIsDown( NiInputMouse::NIM_MIDDLE ) )
	{
		SetButtonState( CURSOR::MBUTTON_PRESS );
	}
	else
	{
		SetButtonState( CURSOR::BUTTON_NONE );
	}
}

void CCursor::SetButtonState( CURSOR::eTYPE type )
{
	m_nButtonState = type;
}

void CCursor::ResetDevice()
{
}

