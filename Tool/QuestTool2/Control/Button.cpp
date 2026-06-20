
#include "stdafx.h"
#include "Button.h"

void cButton::Init( cWindow* pParent, CsPoint pos, CsPoint size, char* cImagePath, float fScaleUI, bool bApplyWindowSize )
{
	cSprite::Init( pParent, pos, size, cImagePath, fScaleUI, bApplyWindowSize );

	m_eOldTexTokenState = MAX_STATE_COUNT;
	m_eTexTokenState = NORMAL;
	m_bClick = false;
}

void cButton::SetTexToken( CsPoint tex )
{
	float x = tex.x*m_fInvTexWidth;
	float y = tex.y*m_fInvTexHeight;

	m_TexToken[ NORMAL ].s_fLeft = 0.0f;
	m_TexToken[ NORMAL ].s_fTop = 0.0f;
	m_TexToken[ NORMAL ].s_fRight = ( tex.x == 0 ) ? 1.0f : x;
	m_TexToken[ NORMAL ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y;

	m_TexToken[ MOUSE_ON ].s_fLeft = x;
	m_TexToken[ MOUSE_ON ].s_fTop = y;
	m_TexToken[ MOUSE_ON ].s_fRight = ( tex.x == 0 ) ? 1.0f : x*2.0f;
	m_TexToken[ MOUSE_ON ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y*2.0f;

	m_TexToken[ MOUSE_CLICK ].s_fLeft = x*2.0f;
	m_TexToken[ MOUSE_CLICK ].s_fTop = y*2.0f;
	m_TexToken[ MOUSE_CLICK ].s_fRight = ( tex.x == 0 ) ? 1.0f : x*3.0f;
	m_TexToken[ MOUSE_CLICK ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y*3.0f;

	m_TexToken[ DISABLE ].s_fLeft = x*3.0f;
	m_TexToken[ DISABLE ].s_fTop = y*3.0f;
	m_TexToken[ DISABLE ].s_fRight = 1.0f;
	m_TexToken[ DISABLE ].s_fBottom = 1.0f;
}

int cButton::Update_ForMouse()
{
	//if( m_eTexTokenState == DISABLE )
	//{
	//	m_bClick = false;
	//	return ACTION_NONE;
	//}
	//
	//CURSOR::eTYPE nType = Click();

	//switch( nType )
	//{
	//case CURSOR::BUTTON_OUTWINDOW:
	//	if( g_Cursor.GetButtonState() != CURSOR::LBUTTON_PRESS )
	//	{
	//		m_bClick = false;
	//	}
	//	else if( m_bClick == true )
	//	{
	//		m_eTexTokenState = MOUSE_CLICK;
	//	}
	//	return ACTION_NONE;

	//case CURSOR::LBUTTON_DOWN:
	//	{ 
	//		m_eTexTokenState = MOUSE_CLICK;
	//		if( m_eSoundType != cWindow::SD_NONE )
	//			PlaySound( m_eSoundType );
	//		m_bClick = true;
	//	}
	//	return ACTION_DOWN;
	//case CURSOR::LBUTTON_PRESS:
	//	if( m_bClick == true )
	//	{
	//		m_eTexTokenState = MOUSE_CLICK;
	//		return ACTION_PRESS;
	//	}
	//	break;
	//case CURSOR::LBUTTON_UP:
	//	if( m_bClick == true )
	//	{
	//		// ÇÔ¼ö È£Ãâ
	//		return ACTION_CLICK;
	//	}
	//	break;
	//default:
	//	m_bClick = false;
	//}
	//
	//m_eTexTokenState = MOUSE_ON;
	return ACTION_ON;
}

void cButton::Render()
{
	if( m_eTexTokenState == NORMAL )
		m_bClick = false;

	if( m_eOldTexTokenState != m_eTexTokenState )
	{
		m_eOldTexTokenState = m_eTexTokenState;
		m_spScreen->SetTextures( 0, 0,
								m_TexToken[ m_eTexTokenState ].s_fLeft,
								m_TexToken[ m_eTexTokenState ].s_fTop,
								m_TexToken[ m_eTexTokenState ].s_fRight,
								m_TexToken[ m_eTexTokenState ].s_fBottom );									
	}
	cSprite::Render();

	if( m_eTexTokenState != DISABLE )
		m_eTexTokenState = NORMAL;
}