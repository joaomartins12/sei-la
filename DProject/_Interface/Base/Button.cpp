
#include "stdafx.h"
#include "Button.h"

cText::sTEXTINFO		cButton::__g_TextInfo;

CsPoint cButton::DefSize()
{
	return CsPoint( 76, 26 ); 
}

cText::sTEXTINFO* cButton::DefTextInfo( TCHAR const* sz )
{
	__g_TextInfo.SetText( sz );
	return &__g_TextInfo; 
}

cButton::cButton():m_pBtnText(NULL), m_pButtonEffectAni(NULL)
//, m_bClick(false)
, m_bMouseOn(false)
{
	m_dwEffectOn = (1 << NORMAL) | ( 1 << MOUSE_ON ) | (1 << MOUSE_CLICK) | (1 << DISABLE);
}

cButton::~cButton()
{ 
	Delete(); 
}

void cButton::GlobalInit()
{
	__g_TextInfo.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	__g_TextInfo.s_eTextAlign = DT_CENTER;
}

void cButton::Delete()
{
	cSprite::Delete();

	SAFE_NIDELETE( m_pBtnText );
	SAFE_NIDELETE( m_pButtonEffectAni );
}

void cButton::Init( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize )
{
	cSprite::Init( pParent, pos, size, cImagePath, bApplyWindowSize );

	m_eOldTexTokenState = MAX_STATE_COUNT;
	m_eTexTokenState = NORMAL;
	m_bClick = false;

	bKeyboardInput = false;
	bCursorIn = false;
}

void cButton::Init( cWindow* pParent, CsPoint pos, CsPoint size, eIMAGE_TYPE it, TCHAR const* bs, bool bApplyWindowSize, CsPoint ptDeltaPos /*= CsPoint::ZERO*/ )
{
	Init( pParent, pos, size, it, cButton::DefTextInfo( bs ), bApplyWindowSize, ptDeltaPos );
}

void cButton::Init( cWindow* pParent, CsPoint pos, CsPoint size, eIMAGE_TYPE it, cText::sTEXTINFO* ti, bool bApplyWindowSize, CsPoint ptDeltaPos /*= CsPoint::ZERO*/ )
{
	if( size == CsPoint::ZERO )
		size = cButton::DefSize();

	switch( it )
	{
	case IMAGE_NOR_1:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor1.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 26 ) );
		break;
	case IMAGE_NOR_2:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor2.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 55 ) );
		break;
	case IMAGE_NOR_3:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor3.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 29 ) );
		break;
	case IMAGE_NOR_4:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor4.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 27 ) );
		break;
	case IMAGE_NOR_5:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor5.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 27 ) );
		break;
	case IMAGE_NOR_6:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor6.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 30 ) );
		break;
	case IMAGE_NOR_7:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor7.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 30 ) );
		break;
	case IMAGE_NOR_8:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor8.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 23 ) );
		break;
	case IMAGE_NOR_9:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor9.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 27 ) );
		break;
	case IMAGE_NOR_10:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor10.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 18 ) );
		break;
	case IMAGE_NOR_11:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor11.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 19 ) );
		break;
	case IMAGE_NOR_12:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor12.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 19 ) );
		break;
	case IMAGE_NOR_13:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor13.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 37 ) );
		break;
	case IMAGE_NOR_13_R:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor13_R.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 37 ) );
		break;
	case IMAGE_NOR_14:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor14.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 47 ) );
		break;
	case IMAGE_NOR_15:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor15.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 26 ) );
		break;
	case IMAGE_NOR_16:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor16.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 31 ) );
		break;
	case IMAGE_NOR_17:
		cSprite::Init( pParent, pos, size, "System\\Btn_Nor17.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 42 ) );
		break;
	case IMAGE_BACK:		
		cSprite::Init( pParent, pos, size, "CharSelect\\BackBT.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 54 ) );
		break;
	case IMAGE_FORWARD:		
		cSprite::Init( pParent, pos, size, "CharSelect\\CharStartBT.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 54 ) );
		break;
	case IMAGE_CANCEL:		
		cSprite::Init( pParent, pos, size, "CharSelect\\CharDelBT.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 54 ) );
		break;
	case IMAGE_OK:		
		cSprite::Init( pParent, pos, size, "CharSelect\\CharCreateBT.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 54 ) );
		break;
	case IMAGE_ALPHA_1:
		cSprite::Init( pParent, pos, size, "System\\Btn_Alpha1.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 32 ) );
		break;		
	case IMAGE_ALPHA_2:
		cSprite::Init( pParent, pos, size, "System\\Btn_Alpha2.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 32 ) );
		break;		
#ifdef MINIGAME
	case IMAGE_MINI_CANCEL://미니게임 나가기 버튼
		cSprite::Init( pParent, pos, size, "MiniGame\\Mini_Out.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 36 ) );
		break;
	case IMAGE_MINI_STOP://미니게임 멈추기 버튼
		cSprite::Init( pParent, pos, size, "MiniGame\\Mini_Stop.tga", bApplyWindowSize );//Mini_Stop.tga
		SetTexToken( CsPoint( 0, 80 ) );
		break;
#endif
#ifdef SIMPLEFICATION
	case IMAGE_SIMPLE_OK:
		cSprite::Init( pParent, pos, size, "Simple\\simple_is_best_booster_on.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 24 ) );
		break;
	case IMAGE_SIMPLE_CANCEL:
		cSprite::Init( pParent, pos, size, "Simple\\simple_is_best_off.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 24 ) );
		break;
#endif
#ifdef CROSSWARS_SYSTEM
	case IMAGE_CROSSBUTTON:
		cSprite::Init( pParent, pos, size, "CrossWars\\130729\\CrossRoder_Btn.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 51, 0 ) );	
		break;
#endif
	case IMAGE_ENCYCLOPEDIA_GET:
		cSprite::Init( pParent, pos, size, "Encyclopedia\\Encyclopedia_GetItemBtn.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 21 ) );	
		break;
	case IMAGE_PINOKIMON_EVENT_CHOICE:
		cSprite::Init( pParent, pos, size, "PinochimonCard\\Card_Vote_Button.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 50 ) );
		break;
	case IMAGE_PINOKIMON_EVENT_VOTE:
		cSprite::Init( pParent, pos, size, "PinochimonCard\\Card_Vote_Number_Button.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 36 ) );
		break;
	default:
		assert_cs( false );
	}
	m_eImeageType = it;

	if( ti != NULL )
	{
		int x = 0;		
		switch( ti->s_eTextAlign )
		{
		case DT_LEFT:							break;
		case DT_CENTER:		x = size.x/2;		break;
		case DT_RIGHT:		x = size.x;			break;
		default:			assert_cs( false );	break;
		}

#ifdef VERSION_USA
		int nBackupSize = ti->s_eFontSize;
		switch( it )
		{
		case IMAGE_ALPHA_1:
		case IMAGE_ALPHA_2:
			break;
		default:
			ti->s_eFontSize = CFont::FS_9;
		}		
#endif
		x += ptDeltaPos.x;

		if( ptDeltaPos.y == 0 )
		{			
			ptDeltaPos.y =  (int)( ( size.y - ti->s_eFontSize*1.4f )/2.0f );
		}		

		m_pBtnText = NiNew cText;
		m_pBtnText->Init( this, CsPoint( x, ptDeltaPos.y ), ti, false );

#ifdef VERSION_USA
		ti->s_eFontSize = (CFont::eFACE_SIZE)nBackupSize;
#endif
	}

	m_eOldTexTokenState = MAX_STATE_COUNT;
	m_eTexTokenState = NORMAL;
	m_bClick = false;

	bKeyboardInput = false;
}

void cButton::SetPos( CsPoint ptPos )
{
	cSprite::SetPos( ptPos );	
}

void cButton::SetPosText( CsPoint ptPos )
{
	if( m_pBtnText == NULL )
		return;

	m_pBtnText->SetPos( ptPos - GetClient() );
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
#ifdef MINIGAME
	m_TexToken[ READY ].s_fLeft = 0.0f;
	m_TexToken[ READY ].s_fTop = 0.0f;
	m_TexToken[ READY ].s_fRight = ( tex.x == 0 ) ? 1.0f : x;
	m_TexToken[ READY ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y;

	m_TexToken[ SUCCESS ].s_fLeft = x;
	m_TexToken[ SUCCESS ].s_fTop = y;
	m_TexToken[ SUCCESS ].s_fRight = ( tex.x == 0 ) ? 1.0f : x*2.0f;
	m_TexToken[ SUCCESS ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y*2.0f;

	m_TexToken[ FAIL ].s_fLeft = x*2.0f;
	m_TexToken[ FAIL ].s_fTop = y*2.0f;
	m_TexToken[ FAIL ].s_fRight = ( tex.x == 0 ) ? 1.0f : x*3.0f;
	m_TexToken[ FAIL ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y*3.0f;

	m_TexToken[ THREE ].s_fLeft = x*3.0f;
	m_TexToken[ THREE ].s_fTop = y*3.0f;
	m_TexToken[ THREE ].s_fRight = ( tex.x == 0 ) ? 1.0f : x*4.0f;
	m_TexToken[ THREE ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y*4.0f;

	m_TexToken[ FOUR ].s_fLeft = x*4.0f;
	m_TexToken[ FOUR ].s_fTop = y*4.0f;
	m_TexToken[ FOUR ].s_fRight = ( tex.x == 0 ) ? 1.0f : x*5.0f;
	m_TexToken[ FOUR ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y*5.0f;

	m_TexToken[ FIVE ].s_fLeft = x*5.0f;
	m_TexToken[ FIVE ].s_fTop = y*5.0f;
	m_TexToken[ FIVE ].s_fRight = ( tex.x == 0 ) ? 1.0f : x*6.0f;
	m_TexToken[ FIVE ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y*6.0f;

	m_TexToken[ SIX ].s_fLeft = x*6.0f;
	m_TexToken[ SIX ].s_fTop = y*6.0f;
	m_TexToken[ SIX ].s_fRight = ( tex.x == 0 ) ? 1.0f : x*7.0f;
	m_TexToken[ SIX ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y*7.0f;

	m_TexToken[ SEVEN ].s_fLeft = x*7.0f;
	m_TexToken[ SEVEN ].s_fTop = y*7.0f;
	m_TexToken[ SEVEN ].s_fRight = ( tex.x == 0 ) ? 1.0f : x*8.0f;
	m_TexToken[ SEVEN ].s_fBottom = ( tex.y == 0 ) ? 1.0f : y*8.0f;
#endif
}

int cButton::Update_ForMouse()
{
	if( !m_bVisible )
		return ACTION_NONE;

	if( m_eTexTokenState == DISABLE || bKeyboardInput)
	{
		m_bClick = false;
		return ACTION_NONE;
	}

	CURSOR::eTYPE nType = Click();	

	switch( nType )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		{
			if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_PRESS )
			{
				if(m_bClick)
				{
					m_eTexTokenState = MOUSE_CLICK;
					return ACTION_DOWN;
				}
			}

			bCursorIn = false;
			m_bClick = false;
			m_eTexTokenState = NORMAL;	

			return ACTION_NONE;
		}
		break;		

	case CURSOR::LBUTTON_DOWN:
		{ 
			if(m_bClick)
			{
				return ACTION_NONE;
			}

			if(!bCursorIn)
			{
				bCursorIn = true;
				CURSOR_ST.SetButtonState(CURSOR::LBUTTON_PRESS);
				return ACTION_NONE;
			}

			m_eTexTokenState = MOUSE_CLICK;


			if( m_eSoundType != cWindow::SD_NONE )
				PlaySound( m_eSoundType );

			m_bClick = true;
			OnEvent( BUTTON_LBDOWN_EVENT, NULL);

			CURSOR_ST.SetButtonState(CURSOR::LBUTTON_PRESS);

			return ACTION_DOWN;
		}
		break;		

	case CURSOR::LBUTTON_UP:
		{
			m_eTexTokenState = MOUSE_ON;

			if( m_bClick == true )
			{		
				m_bClick = false;			
				OnEvent( BUTTON_LBUP_EVENT, NULL );
				return ACTION_CLICK;
			}		
		}		
		break;

	case CURSOR::LBUTTON_PRESS:
		{
			if( m_bClick == true )
			{
				m_eTexTokenState = MOUSE_CLICK;
				return ACTION_PRESS;
			}
		}		
		break;
	case CURSOR::RBUTTON_DOWN:
		{
			if (m_bClick)
			{
				return ACTION_NONE;
			}

			if (!bCursorIn)
			{
				bCursorIn = true;
				CURSOR_ST.SetButtonState(CURSOR::RBUTTON_PRESS);
				return ACTION_NONE;
			}

			m_eTexTokenState = MOUSE_CLICK;


			if (m_eSoundType != cWindow::SD_NONE)
				PlaySound(m_eSoundType);

			m_bClick = true;
			OnEvent(BUTTON_RBDOWN_EVENT, NULL);

			CURSOR_ST.SetButtonState(CURSOR::RBUTTON_PRESS);

			return ACTION_DOWN;
		}
		break;

	case CURSOR::RBUTTON_UP:
		{
			m_eTexTokenState = MOUSE_ON;

			if (m_bClick == true)
			{
				m_bClick = false;
				OnEvent(BUTTON_RBUP_EVENT, NULL);
				return ACTION_CLICK;
			}
		}
		break;

	case CURSOR::RBUTTON_PRESS:
		{
			if (m_bClick == true)
			{
				m_eTexTokenState = MOUSE_CLICK;
				return ACTION_PRESS;
			}
		}
		break;
	}


	if(m_eTexTokenState != MOUSE_CLICK)
	{
		bCursorIn = true;
		m_eTexTokenState = MOUSE_ON;
	}
	return ACTION_ON;
}

void cButton::OnMouseOver()
{
	if( Click() == CURSOR::BUTTON_OUTWINDOW )
		return;

	if( m_eTexTokenState != DISABLE )
		m_eTexTokenState = MOUSE_ON;
}

void cButton::OnMouseLeave()
{
	if( Click() != CURSOR::BUTTON_OUTWINDOW )
		return;

	if( m_eTexTokenState != DISABLE )
		m_eTexTokenState = NORMAL;
}

void cButton::OnMouseDown()
{
	if( Click() == CURSOR::BUTTON_OUTWINDOW )
		return;

	if( m_eTexTokenState == DISABLE )
		return;

	m_eTexTokenState = MOUSE_CLICK;

	m_bClick = true;
}

void cButton::OnMouseUp()
{
	CURSOR::eTYPE eClickType = Click();
	if( eClickType == CURSOR::BUTTON_OUTWINDOW )
		return;

	if( m_eTexTokenState == DISABLE )
		return;

	if( !m_bClick )
		return;

	m_eTexTokenState = NORMAL;

	if( eClickType == CURSOR::LBUTTON_UP )
		OnEvent( BUTTON_LBUP_EVENT, NULL );
}

void cButton::OnMousePressed()
{
	if( Click() == CURSOR::BUTTON_OUTWINDOW )
		return;

	if( m_eTexTokenState == DISABLE )
		return;

	if( !m_bClick )
		return;

	m_eTexTokenState = MOUSE_CLICK;
}

void cButton::StopEffect(bool bVisible)
{
	SAFE_POINTER_RET( m_pButtonEffectAni );
	m_pButtonEffectAni->PlayOff();
	m_pButtonEffectAni->SetVisible( bVisible );
}

void cButton::StartEffect( bool bResetAni )
{
	SAFE_POINTER_RET( m_pButtonEffectAni );
	m_pButtonEffectAni->PlayOn(bResetAni);
	m_pButtonEffectAni->SetVisible( true );
}

void cButton::Update(float const& fDeltaTime)
{
	if(m_pButtonEffectAni)
		m_pButtonEffectAni->Update(fDeltaTime);
}

void cButton::Render()
{
	if(! m_bVisible )
		return;

	if( m_eOldTexTokenState != m_eTexTokenState )
	{
		m_eOldTexTokenState = m_eTexTokenState;
		if( m_bMouseOn )
		{
			m_spScreen->SetTextures( 0, 0,
				m_TexToken[ MOUSE_ON ].s_fLeft,
				m_TexToken[ MOUSE_ON ].s_fTop,
				m_TexToken[ MOUSE_ON ].s_fRight,
				m_TexToken[ MOUSE_ON ].s_fBottom );	
		}
		else
		{
			m_spScreen->SetTextures( 0, 0,
				m_TexToken[ m_eTexTokenState ].s_fLeft,
				m_TexToken[ m_eTexTokenState ].s_fTop,
				m_TexToken[ m_eTexTokenState ].s_fRight,
				m_TexToken[ m_eTexTokenState ].s_fBottom );
		}
	}

	cSprite::Render();

	if( m_pBtnText )
		m_pBtnText->Render();

	if(m_pButtonEffectAni)
	{
		if( m_dwEffectOn & (1 << m_eTexTokenState) )
			m_pButtonEffectAni->Render();
	}

	if( m_eTexTokenState != DISABLE )
		m_eTexTokenState = NORMAL;
}

void cButton::ResetDevice()
{
	cSprite::ResetDevice();
	if( m_pBtnText )
		m_pBtnText->ResetDevice();

	if( m_pButtonEffectAni )
		m_pButtonEffectAni->ResetDevice();
}

void cButton::SetTextColor( NiColor Color )
{
	if( m_pBtnText )
		m_pBtnText->SetColor( Color ); 
}

void cButton::SetText( TCHAR const* szText )
{ 
	if( m_pBtnText )
		m_pBtnText->SetText( szText ); 
}

void cButton::SetText( cText::sTEXTINFO* ti )
{ 
	SAFE_POINTER_RET( ti );

	if( NULL == m_pBtnText )
	{
		CsPoint csSize = GetSize();
		CsPoint csPos = CsPoint::ZERO;

		switch( ti->s_eTextAlign )
		{
		case DT_LEFT:									break;
		case DT_CENTER:		csPos.x = csSize.x >> 1;	break;
		case DT_RIGHT:		csPos.x = csSize.x;			break;
		default:			assert_cs( false );			return;
		}

		csPos.y = (int)( ( csSize.y - ti->s_eFontSize*1.4f )/2.0f );

		m_pBtnText = NiNew cText;
		m_pBtnText->Init( this, csPos, ti, false );
	}

	if( m_pBtnText )
		m_pBtnText->SetText( ti->GetText() ); 
}

void cButton::SetEffectOn( eSTATE eStateData, bool bFirstEffectSetting)
{
	if( bFirstEffectSetting )
		m_dwEffectOn &= (1 << eStateData);
	else
		m_dwEffectOn |= (1 << eStateData);
} 

void cButton::SetButtonEffect( cSpriteAni::ePLAY_TYPE eType,  CsPoint pos, CsPoint size, char* cImagePath, char* cExt, int nAniCount, bool bApplyWindowSize, CsPoint textoken , cSpriteAni::eSPRITE_PLAY eSprite ,int nEnterCount/*스프라이트가 아래로 내려가게하는 조건 수 */)
{
	SAFE_NIDELETE(m_pButtonEffectAni);

	m_pButtonEffectAni = NiNew cSpriteAni;
	m_pButtonEffectAni->Init( eType, this, pos, size,cImagePath, cExt, nAniCount, bApplyWindowSize, textoken , eSprite, nEnterCount );		
	m_pButtonEffectAni->AddEvent( cSpriteAni::SPRITEANI_END, this, &cButton::_ButtonEffectPlayEnd );
}

void cButton::SetEffectAniTime( float fTime )
{
	if(m_pButtonEffectAni)
		m_pButtonEffectAni->SetAniTime(fTime);
}

void cButton::ActiveLBtnDown()
{ 
	m_eTexTokenState = MOUSE_CLICK; 
	if( m_eSoundType != cWindow::SD_NONE )
		PlaySound( m_eSoundType );

	m_bClick = true; 
}
void cButton::ACtiveLBtnDownPress()
{ 
	m_eTexTokenState = MOUSE_CLICK;
}
bool cButton::ActiveLBtnUp()
{ 
	if( m_bClick == true )
		m_eTexTokenState = MOUSE_CLICK;

	return m_bClick;
}
bool cButton::IsEnable(){ return ( m_eTexTokenState != DISABLE ); }

void cButton::setTexTokenState(eSTATE type)
{ 
	m_eTexTokenState = type;
}
void cButton::SetMouseOnMode(bool	bMode)
{
	m_bMouseOn = bMode;

	m_eTexTokenState	= NORMAL;
	m_eOldTexTokenState	= MOUSE_ON;
}
bool cButton::IsClicked() 
{
	return m_bClick;
}

void cButton::KeyboardBtnDn()
{
	m_eTexTokenState = MOUSE_CLICK; 

	if( m_eSoundType != cWindow::SD_NONE )
	{ 
		PlaySound( m_eSoundType ); 
	}

	bKeyboardInput = true;
}

void cButton::KeyboardBtnUp()
{
	m_eTexTokenState = NORMAL; 

	bKeyboardInput = false;
}

void cButton::SetEnable( bool bEnable )
{ 
	if(bEnable)
	{
		if(!bKeyboardInput)
		{
			m_eTexTokenState = NORMAL;
		}
	}
	else
	{
		m_eTexTokenState = DISABLE;
	}	
}

bool cButton::IsEffectPlay() const
{
	SAFE_POINTER_RETVAL( m_pButtonEffectAni, false );
	return m_pButtonEffectAni->IsPlay();
}

void cButton::_ButtonEffectPlayEnd(void* pSender, void* pData)
{
	OnEvent( BUTTON_EFFECT_PLAY_END );
}