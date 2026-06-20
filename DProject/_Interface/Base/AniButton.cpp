#include "StdAfx.h"
#include "AniButton.h"

cAniButton::cAniButton(void) : m_nAniIndex(0),	m_nAniStartIndex(0), m_nAniEndIndex(0),
m_fCurAniTime(0.0f), m_fSetAniTime(1.0f), m_AniState(NORMAL), m_bLoop(false)
{
}

cAniButton::~cAniButton(void)
{
}

void cAniButton::Init( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize )
{
	cSprite::Init( pParent, pos, size, cImagePath, bApplyWindowSize );

	m_eOldTexTokenState = MAX_STATE_COUNT;
	m_eTexTokenState = NORMAL;
	m_bClick = false;
}

void cAniButton::Init( cWindow* pParent, CsPoint pos, CsPoint size, TCHAR const* bs, bool bApplyWindowSize, CsPoint ptDeltaPos /*= CsPoint::ZERO*/ )
{
	Init( pParent, pos, size, cButton::DefTextInfo( bs ), bApplyWindowSize, ptDeltaPos );
}

void cAniButton::Init( cWindow* pParent, CsPoint pos, CsPoint size, cText::sTEXTINFO* ti, bool bApplyWindowSize, CsPoint ptDeltaPos /*= CsPoint::ZERO*/ )
{
	cSprite::Init( pParent, pos, size, "System\\Btn_Nor1.tga", bApplyWindowSize );

	if( size == CsPoint::ZERO )
		size = cButton::DefSize();

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
	}

	m_eOldTexTokenState = MAX_STATE_COUNT;
	m_eTexTokenState = NORMAL;
	m_bClick = false;
}

void cAniButton::Delete()
{
	cSprite::Delete();
}
void cAniButton::SetPos( CsPoint ptPos )
{
	cSprite::SetPos( ptPos );
}
int	 cAniButton::Update_ForMouse()
{
	if( m_eTexTokenState == DISABLE || !m_bVisible )
	{
		m_nAniIndex = 0;
		m_bClick = false;
		return ACTION_NONE;
	}

	CURSOR::eTYPE nType = Click();

	switch( nType )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
		{
			m_bClick = false;
			m_eTexTokenState = NORMAL;
			m_nAniIndex = 0;
		}
		else if( m_bClick == true )
		{
			m_eTexTokenState = MOUSE_CLICK;
			m_nAniIndex = m_nTotalIndex-1;
		}

		return ACTION_NONE;

	case CURSOR::LBUTTON_DOWN:
		{ 
			if( m_eSoundType != cWindow::SD_NONE )
				PlaySound( m_eSoundType );

			m_eTexTokenState = MOUSE_CLICK;
			m_nAniIndex = m_nTotalIndex-1;
			m_bClick = true;
			OnEvent( BUTTON_LBDOWN_EVENT, NULL);
		}
		return ACTION_DOWN;
	case CURSOR::LBUTTON_PRESS:
		if( m_bClick == true )
		{
			m_eTexTokenState = MOUSE_CLICK;
			m_nAniIndex = m_nTotalIndex-1;
			return ACTION_PRESS;
		}
		break;
	case CURSOR::LBUTTON_UP:
		if( m_bClick == true )
		{				
			OnEvent( BUTTON_LBUP_EVENT, NULL);
			// 함수 호출
			return ACTION_CLICK;
		}
		break;
	default:
		m_eTexTokenState = NORMAL;
		break;
	}

	m_eTexTokenState = MOUSE_ON;

	return ACTION_ON;
}
void cAniButton::Update(float fTime)
{
	if( m_AniState == NORMAL )				//	NORMAL 이면 애니 없음.
		return;

	else if( m_AniState != m_eTexTokenState )	//	그렇지 않으면 애니 있음.(보통 MOUSE_ON)일 경우.
		return;


	if( m_nAniEndIndex <= 0 )
		return;

	m_fCurAniTime -= fTime;
	while( m_fCurAniTime < 0.0f )
	{
		m_fCurAniTime += m_fSetAniTime;

		if( m_nAniIndex >= m_nAniEndIndex )
		{
			if(m_bLoop==true)
				m_nAniIndex = m_nAniStartIndex;
		}
		else
			m_nAniIndex++;
	}
}
void cAniButton::Render()
{
	 if( !m_bVisible )
		 return;

	if( m_eTexTokenState == NORMAL )
		m_bClick = false;

	//if( m_eOldTexTokenState != m_eTexTokenState || m_eTexTokenState == MOUSE_ON)
	{
		float	sx = m_TexToken.s_fLeft+m_TexToken.s_fRight*m_nAniIndex;
		float	sy = m_TexToken.s_fTop+m_TexToken.s_fBottom*m_nAniIndex;
		float	ex = m_TexToken.s_fRight+m_TexToken.s_fRight*m_nAniIndex;
		float	ey = m_TexToken.s_fBottom+m_TexToken.s_fBottom*m_nAniIndex;

		//if( sx >= 1.0f) sx =1.0f;
		//if( sy >= 1.0f) sy =1.0f;
		//if( ex >= 1.0f) ex =1.0f;
		//if( ey >= 1.0f) ey =1.0f;

		m_eOldTexTokenState = m_eTexTokenState;
		m_spScreen->SetTextures( 0, 0, sx, sy, ex, ey);
	}

	cSprite::Render();
}
void cAniButton::ResetDevice()
{
	cSprite::ResetDevice();
}

cWindow::eTYPE cAniButton::GetType()
{
	return cWindow::T_Button;
	//T_AniButton; 
}

void cAniButton::OnMouseOver()
{
	if( Click() == CURSOR::BUTTON_OUTWINDOW )
	{
		ResetTokenState();
		return;
	}

	if( m_eTexTokenState != DISABLE )
		m_eTexTokenState = MOUSE_ON;
}
void cAniButton::OnMouseLeave()
{
	if( Click() != CURSOR::BUTTON_OUTWINDOW )
		return;

	if( m_eTexTokenState != DISABLE )
	{
		m_eTexTokenState = NORMAL;
		m_nAniIndex = 0;
	}
}
void cAniButton::OnMouseDown()
{
	if( Click() == CURSOR::BUTTON_OUTWINDOW )
		return;

	if( m_eTexTokenState == DISABLE )
		return;

	m_eTexTokenState = MOUSE_CLICK;
	m_nAniIndex = m_nTotalIndex-1;
	m_bClick = true;
}
void cAniButton::OnMouseUp()
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
void cAniButton::OnMousePressed()
{
	if( Click() == CURSOR::BUTTON_OUTWINDOW )
		return;

	if( m_eTexTokenState == DISABLE )
		return;

	if( !m_bClick )
		return;

	m_eTexTokenState = MOUSE_CLICK;
	m_nAniIndex = m_nTotalIndex-1;
}

void cAniButton::SetAniLoop(bool	Loop)
{
	m_bLoop = Loop;
}
void cAniButton::SetAniState(eSTATE	state)
{
	m_AniState = state;
}

void cAniButton::ResetTokenState()
{
	m_bClick = false;
	m_eTexTokenState = NORMAL;
	m_nAniIndex = 0;
}

void cAniButton::SetAniTime( float fTime )
{
	m_fCurAniTime = m_fSetAniTime = fTime; 
}

void cAniButton::SetAniStartEnd(int StartId, int EndID)
{
	m_nAniStartIndex	= StartId;
	m_nAniEndIndex		= EndID;
}

void cAniButton::SetTexToken( CsPoint tex )
{
	float x = tex.x*m_fInvTexWidth;
	float y = tex.y*m_fInvTexHeight;

	if(x==0)
	{
		m_nTotalIndex = m_fInvTexHeight/tex.y;
	}
	else if(y==0)
	{
		m_nTotalIndex = m_fInvTexWidth/tex.x;
	}

	m_TexToken.s_fLeft = 0.0f;
	m_TexToken.s_fTop = 0.0f;
	m_TexToken.s_fRight = ( tex.x == 0 ) ? 1.0f : x;
	m_TexToken.s_fBottom = ( tex.y == 0 ) ? 1.0f : y;
}