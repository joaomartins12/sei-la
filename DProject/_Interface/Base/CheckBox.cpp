
#include "stdafx.h"
#include "CheckBox.h"

void cCheckBox::Delete()
{
	cSprite::Delete();
	SAFE_NIDELETE( m_pBtnText );
}

void cCheckBox::Init( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize )
{
	cSprite::Init( pParent, pos, size, cImagePath, bApplyWindowSize );

	m_eOldTexTokenState = MAX_STATE_COUNT;
	m_eTexTokenState = NORMAL;
	m_bClick = false;
	bCursorIn = false;
}

void cCheckBox::Init( cWindow* pParent, CsPoint pos, CsPoint size, eIMAGE_TYPE it, TCHAR const* cs, bool bApplyWindowSize )
{
	Init( pParent, pos, size, it, cButton::DefTextInfo( cs ), false );
}

void cCheckBox::Init( cWindow* pParent, CsPoint pos, CsPoint size, eIMAGE_TYPE it, cText::sTEXTINFO* ti, bool bApplyWindowSize )
{
	if( size == CsPoint::ZERO )
		size = cButton::DefSize();

	switch( it )
	{
	case IMAGE_RADIO_CHAT1:
		ti->SetText( UISTRING_TEXT( "COMMON_TXT_NORMAL" ).c_str() );
		cSprite::Init( pParent, pos, size, "Chat\\ChatType\\Ch_Nomal.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 15 ) );		
		break;
	case IMAGE_RADIO_CHAT2:
		ti->SetText( UISTRING_TEXT( "COMMON_TXT_PARTY" ).c_str() );
		cSprite::Init( pParent, pos, size, "Chat\\ChatType\\Ch_Party.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 15 ) );		
		break;
	case IMAGE_RADIO_CHAT3:
		ti->SetText( UISTRING_TEXT( "COMMON_TXT_GUILD" ).c_str() );
		cSprite::Init( pParent, pos, size, "Chat\\ChatType\\Ch_Guild.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 15 ) );		
		break;	
	case IMAGE_RADIO_CHAT4:
		ti->SetText( UISTRING_TEXT( "COMMON_TXT_WHISPER" ).c_str() );
		cSprite::Init( pParent, pos, size, "Chat\\ChatType\\Ch_Whisper.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 15 ) );		
		break;	
	case IMAGE_RADIO_CHAT5:
		ti->SetText( UISTRING_TEXT( "COMMON_TXT_SHOUT" ).c_str() );
		cSprite::Init( pParent, pos, size, "Chat\\ChatType\\Ch_Shout.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 15 ) );		
		break;
	case IMAGE_RADIO_CHAT6:
		ti->SetText( UISTRING_TEXT( "COMMON_TXT_TRADE" ).c_str() );
		cSprite::Init( pParent, pos, size, "Chat\\ChatType\\Ch_Trade.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 15 ) );		
		break;	
	case IMAGE_RADIO_1:
		cSprite::Init( pParent, pos, size, "System\\Btn_Radio1.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 23 ) );		
		break;
	case IMAGE_RADIO_2:
		cSprite::Init( pParent, pos, size, "System\\Btn_Radio2.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 26 ) );		
		break;
	case IMAGE_RADIO_3:
		cSprite::Init( pParent, pos, size, "System\\Btn_Radio3.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 28 ) );
		break;
	case IMAGE_RADIO_4:
		cSprite::Init( pParent, pos, size, "System\\Btn_Radio4.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 24 ) );
		break;
	case IMAGE_RADIO_5:
		cSprite::Init( pParent, pos, size, "System\\Btn_Radio5.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 29 ) );
		break;
	case IMAGE_RADIO_6:
		cSprite::Init( pParent, pos, size, "System\\Btn_Radio6.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 23 ) );
		break;
	case IMAGE_RADIO_ENCYCLOPEDIA:
		cSprite::Init( pParent, pos, size, "Encyclopedia\\Encyclopedia_Tab.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 31 ) );
		break;
	case IMAGE_RADIO_ENCY_DECK:
		cSprite::Init( pParent, pos, size, "Encyclopedia\\Encyclopedia_DeckBtn.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 29 ) );
		break;
	default:
		assert_cs( false );
	}

	SetCheckBoxText( ti );
// 	if( ti != NULL )
// 	{
// 		int x = 0;
// 		switch( ti->s_eTextAlign )
// 		{
// 		case DT_LEFT:							break;
// 		case DT_CENTER:		x = size.x/2;		break;
// 		case DT_RIGHT:		x = size.x;			break;
// 		default:			assert_cs( false );	break;
// 		}
// 
// 		m_pBtnText = NiNew cText;
// 		m_pBtnText->Init( this, CsPoint( x, (int)( ( size.y - ti->s_eFontSize*1.4f )/2.0f ) ), ti, bApplyWindowSize );		
// 	}

	m_eOldTexTokenState = MAX_STATE_COUNT;
	m_eTexTokenState = NORMAL;
	m_bClick = false;
}

void cCheckBox::SetCheckBoxText( cText::sTEXTINFO* ti )
{
	NISAFE_DELETE( m_pBtnText );
	SAFE_POINTER_RET( ti );
//	m_pBtnText = NiNew cText;
//	m_pBtnText->Init( this, CsPoint( m_ptSize.x + 5.0f, (int)( ( m_ptSize.y - ti->s_eFontSize*1.4f )/2.0f ) ), ti, false );		

	int x = 0;
	switch( ti->s_eTextAlign )
	{
	case DT_LEFT:							break;
	case DT_CENTER:		x = m_ptSize.x/2;		break;
	case DT_RIGHT:		x = m_ptSize.x;			break;
	default:			assert_cs( false );	break;
	}

	m_pBtnText = NiNew cText;
	m_pBtnText->Init( this, CsPoint( x, (int)( ( m_ptSize.y - ti->s_eFontSize*1.4f )/2.0f ) ), ti, false );		
}

void cCheckBox::SetTexToken( CsPoint tex )
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

	m_TexToken[ CHECK_OK ].s_fLeft = x*3.0f;
	m_TexToken[ CHECK_OK ].s_fTop = y*3.0f;
	m_TexToken[ CHECK_OK ].s_fRight = ( tex.x == 0 ) ? 1.0f : x*4.0f;
	m_TexToken[ CHECK_OK ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y*4.0f;

	m_TexToken[ DISABLE ].s_fLeft = x*4.0f;
	m_TexToken[ DISABLE ].s_fTop = y*4.0f;
	m_TexToken[ DISABLE ].s_fRight = 1.0f;
	m_TexToken[ DISABLE ].s_fBottom = 1.0f;
}

bool cCheckBox::IsCheck() const
{ 
	return ( m_eTexTokenState == CHECK_OK ); 
}

void cCheckBox::SetCheck( bool bCheck )
{ 
	bCheck == true ? m_eTexTokenState = CHECK_OK : m_eTexTokenState = NORMAL; 
}

void cCheckBox::SetTexTokenState( eSTATE state )
{ 
	m_eTexTokenState = state; 
}

void cCheckBox::SetTexNState(CsPoint ptTex, float fStartPos, eSTATE eTokenState)
{
	float x = ptTex.x*m_fInvTexWidth;
	float y = ptTex.y*m_fInvTexHeight;

	m_TexToken[ eTokenState ].s_fLeft = x*fStartPos;
	m_TexToken[ eTokenState ].s_fTop = y*fStartPos;
	fStartPos += 1.0f;
	m_TexToken[ eTokenState ].s_fRight = ( ptTex.x == 0 ) ? 1.0f : x*fStartPos;
	m_TexToken[ eTokenState ].s_fBottom = ( ptTex.y == 0 ) ? 1.0f : y*fStartPos;
}

void cCheckBox::SetTextColor( NiColor Color ) 
{ 
	if( m_pBtnText ) 
		m_pBtnText->SetColor( Color ); 
}

int cCheckBox::Update_ForMouse()
{
	if( m_eTexTokenState == DISABLE || !m_bVisible )
		return ACTION_NONE;

	CURSOR::eTYPE nType = Click();
	switch( nType )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		{
			if( m_eTexTokenState != CHECK_OK )
				m_eTexTokenState = NORMAL;
			
			bCursorIn = false;

			return ACTION_NONE;
		}
		break;		

	case CURSOR::LBUTTON_DOWN:
		{
			bCursorIn = true;

			if(m_bClick)
			{
				return ACTION_NONE;
			}			

			if( m_eTexTokenState != CHECK_OK )
			{
				m_eTexTokenState = MOUSE_CLICK;
			}

			if( m_eSoundType != cWindow::SD_NONE )
			{
				PlaySound( m_eSoundType );
			}

			m_bClick = !m_bClick;

			OnEvent( BUTTON_LBDOWN_EVENT, NULL);
			return ACTION_DOWN;
		}
		break;

	case CURSOR::LBUTTON_UP:
		{
			if(!bCursorIn)
			{
				return ACTION_NONE;
			}

			if( m_bClick )
			{
				if( m_eTexTokenState == CHECK_OK )
				{
					m_eTexTokenState = MOUSE_ON;
				}
				else
				{
					m_eTexTokenState = CHECK_OK;
				}

				OnEvent( BUTTON_LBUP_EVENT, NULL);
				m_bClick = !m_bClick;
				return ACTION_CLICK;
			}
		}		
		break;
	}

 	if( m_eTexTokenState != CHECK_OK )
	{
 		m_eTexTokenState = MOUSE_ON;
	}

	return ACTION_ON;
}

void cCheckBox::Render()
{
	if( !m_bVisible )
		return;

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

	if( m_pBtnText )
		m_pBtnText->Render();
}

void cCheckBox::ResetDevice()
{
	cSprite::ResetDevice();
	if( m_pBtnText )
		m_pBtnText->ResetDevice();
}

void cCheckBox::OnMouseOver()
{
	if( m_eTexTokenState == DISABLE )
		return;

	if( m_eTexTokenState == CHECK_OK )
		return;

	m_eTexTokenState = MOUSE_ON;
}

void cCheckBox::OnMouseLeave()
{
	if( m_eTexTokenState == DISABLE )
		return;

	if( m_eTexTokenState == CHECK_OK )
		return;

	m_eTexTokenState = NORMAL;
}

void cCheckBox::OnMouseDown()
{
	if( m_eTexTokenState == DISABLE )
		return;

	m_bClick = true;

	if( m_eTexTokenState == CHECK_OK )
		return;
	
	m_eTexTokenState = MOUSE_CLICK;
}

void cCheckBox::OnMouseUp()
{
	if( m_eTexTokenState == DISABLE )
		return;

	if( !m_bClick )
		return;

	if( m_eTexTokenState == CHECK_OK )
		m_eTexTokenState = MOUSE_ON;
	else
		m_eTexTokenState = CHECK_OK;
}

void cCheckBox::OnMousePressed()
{
	if( m_eTexTokenState == DISABLE )
		return;

	if( !m_bClick )
		return;

	if( m_eTexTokenState != CHECK_OK )
		m_eTexTokenState = MOUSE_CLICK;
}
