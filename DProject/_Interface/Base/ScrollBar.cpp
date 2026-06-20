
#include "stdafx.h"
#include "ScrollBar.h"

#define IMAGE_DEFAULT_HEIGHT		2


cScrollBar::cScrollBar() : m_rcRegion(CsRect(CsPoint(0, 0), CsPoint(0, 0))), 
m_rcBarView(CsRect(CsPoint(0, 0), CsPoint(0, 0))), m_rcBarTotal(CsRect(CsPoint(0, 0), CsPoint(0, 0))),
m_ptRange(CsPoint(0, 0)), m_nRenderCount(-1), m_nMaxPosIndex(0), m_nCurPosIndex(0), m_fPos_Height(0.0f),
m_ptButtonSize(CsPoint(0, 0)), m_bEnableScroll(false), m_nBarTotalHeight(0), m_nBarViewHeight(0), m_nRegionClickMoveIndex(5),
m_ptBackupLocalMousePos(CsPoint(0, 0)), m_bEnableRenderFromEnableScroll(false), m_nBarTopHeight(IMAGE_DEFAULT_HEIGHT), m_nBarBotHeight(IMAGE_DEFAULT_HEIGHT)
{
	m_pUpButton = NULL;
	m_pDownButton = NULL;
	m_pSpriteStart = NULL;
	m_pSpriteCenter = NULL;
	m_pSpriteEnd = NULL;
	m_pSpriteBG = NULL;
	m_ScrollTimeSeq.SetDeltaTime( 100 );
}

void cScrollBar::Delete()
{
	SAFE_NIDELETE( m_pUpButton );
	SAFE_NIDELETE( m_pDownButton );
	SAFE_NIDELETE( m_pSpriteStart );
	SAFE_NIDELETE( m_pSpriteCenter );
	SAFE_NIDELETE( m_pSpriteEnd );
	SAFE_NIDELETE( m_pSpriteBG );

	m_nRenderCount = -1;
}

void cScrollBar::ChangeGBImg( char* cImagePath, bool bApplyWindowSize )
{
	SAFE_NIDELETE( m_pSpriteBG );
	CsPoint csSize = GetSize();
	m_pSpriteBG = NiNew cSprite;
	m_pSpriteBG->Init( this, CsPoint( 0, m_ptButtonSize.y ), CsPoint( m_ptButtonSize.x, csSize.y - m_ptButtonSize.y*2 ), cImagePath, bApplyWindowSize );
}

void cScrollBar::Init( eSCROLL_TYPE type, cWindow* pParent, CsPoint pos, CsPoint CtrlSize, CsPoint btnSize, CsRect rcRegion, int nRenderCount, bool bApplyWindowSize )
{
	assert_cs( CtrlSize.x == btnSize.x );
	cWindow::Init( pParent, pos, CtrlSize, bApplyWindowSize );

	assert_cs( m_pUpButton == NULL );
	assert_cs( m_pDownButton == NULL );
	assert_cs( m_pSpriteStart == NULL );
	assert_cs( m_pSpriteCenter == NULL );
	assert_cs( m_pSpriteEnd == NULL );

	m_ScrollTimeSeq.SetDeltaTime( 100 );

	m_nRegionClickMoveIndex = 5;

	// 영역을 로칼 좌표로
	m_rcRegion = rcRegion - pos;

	m_nRenderCount = nRenderCount;
	m_bEnableScroll = false;
	m_eBarClickType = CB_NONE;

	m_ptButtonSize = btnSize;
	m_nBarTotalHeight = CtrlSize.y - m_ptButtonSize.y*2;

	switch( type )
	{
	case TYPE_1:
		{
			m_pUpButton = NiNew cButton;
			m_pUpButton->Init( this, CsPoint::ZERO, btnSize, "Control_G\\Scroll01\\ScrollButton1.tga", bApplyWindowSize );
			m_pUpButton->SetTexToken( CsPoint( 0, 15 ) );
			m_pUpButton->SetEnable( false );

			m_pDownButton = NiNew cButton;
			m_pDownButton->Init( this, CsPoint( 0, CtrlSize.y - m_ptButtonSize.y ), btnSize, "Control_G\\Scroll01\\ScrollButton2.tga", bApplyWindowSize );
			m_pDownButton->SetTexToken( CsPoint( 0, 15 ) );
			m_pDownButton->SetEnable( false );

			m_pSpriteStart = NiNew cSprite;
			m_pSpriteStart->Init( this, CsPoint::ZERO, CsPoint( m_ptButtonSize.x, 2 ), "Control_G\\Scroll01\\ScrollBarTop.tga", bApplyWindowSize );

			m_pSpriteCenter = NiNew cSprite;
			m_pSpriteCenter->Init( this, CsPoint::ZERO, CsPoint::ZERO, "Control_G\\Scroll01\\ScrollBar.tga", bApplyWindowSize );

			m_pSpriteEnd = NiNew cSprite;
			m_pSpriteEnd->Init( this, CsPoint::ZERO, CsPoint( m_ptButtonSize.x, 2 ), "Control_G\\Scroll01\\ScrollBarBottom.tga", bApplyWindowSize );

			m_pSpriteBG = NiNew cSprite;
			m_pSpriteBG->Init( this, CsPoint( 0, m_ptButtonSize.y ), CsPoint( m_ptButtonSize.x, CtrlSize.y - m_ptButtonSize.y*2 ), "Control_G\\Scroll01\\ScrollBG.tga", bApplyWindowSize );
		}
		break;
	case TYPE_2:
		{
			m_pUpButton = NiNew cButton;
			m_pUpButton->Init( this, CsPoint::ZERO, btnSize, "Control_G\\Scroll02\\ScrollButton1.tga", bApplyWindowSize );
			m_pUpButton->SetTexToken( CsPoint( 0, 15 ) );
			m_pUpButton->SetEnable( false );

			m_pDownButton = NiNew cButton;
			m_pDownButton->Init( this, CsPoint( 0, CtrlSize.y - m_ptButtonSize.y ), btnSize, "Control_G\\Scroll02\\ScrollButton2.tga", bApplyWindowSize );
			m_pDownButton->SetTexToken( CsPoint( 0, 15 ) );
			m_pDownButton->SetEnable( false );

			m_pSpriteStart = NiNew cSprite;
			m_pSpriteStart->Init( this, CsPoint::ZERO, CsPoint( m_ptButtonSize.x, 2 ), "Control_G\\Scroll02\\ScrollBarTop.tga", bApplyWindowSize );

			m_pSpriteCenter = NiNew cSprite;
			m_pSpriteCenter->Init( this, CsPoint::ZERO, CsPoint::ZERO, "Control_G\\Scroll02\\ScrollBar.tga", bApplyWindowSize );

			m_pSpriteEnd = NiNew cSprite;
			m_pSpriteEnd->Init( this, CsPoint::ZERO, CsPoint( m_ptButtonSize.x, 2 ), "Control_G\\Scroll02\\ScrollBarBottom.tga", bApplyWindowSize );

			m_pSpriteBG = NiNew cSprite;
			m_pSpriteBG->Init( this, CsPoint( 0, m_ptButtonSize.y ), CsPoint( m_ptButtonSize.x, CtrlSize.y - m_ptButtonSize.y*2 ), "Control_G\\Scroll02\\ScrollBG.tga", bApplyWindowSize );
		}
		break;
	case TYPE_GOTCHA:
		{
			m_pUpButton = NiNew cButton;
			m_pUpButton->Init( this, CsPoint::ZERO, btnSize, "GotchaMachine\\gotcha_scrollbutton1.tga", bApplyWindowSize );
			m_pUpButton->SetTexToken( CsPoint( 0, 27 ) );
			m_pUpButton->SetEnable( false );

			m_pDownButton = NiNew cButton;
			m_pDownButton->Init( this, CsPoint( 0, CtrlSize.y - m_ptButtonSize.y ), btnSize, "GotchaMachine\\gotcha_ScrollButton2.tga", bApplyWindowSize );
			m_pDownButton->SetTexToken( CsPoint( 0, 27 ) );
			m_pDownButton->SetEnable( false );

			m_pSpriteStart = NiNew cSprite;
			m_pSpriteStart->Init( this, CsPoint::ZERO, CsPoint( m_ptButtonSize.x, 2 ), "GotchaMachine\\gotcha_ScrollBarTop.tga", bApplyWindowSize );

			m_pSpriteCenter = NiNew cSprite;
			m_pSpriteCenter->Init( this, CsPoint::ZERO, CsPoint::ZERO, "GotchaMachine\\gotcha_ScrollBar.tga", bApplyWindowSize );

			m_pSpriteEnd = NiNew cSprite;
			m_pSpriteEnd->Init( this, CsPoint::ZERO, CsPoint( m_ptButtonSize.x, 2 ), "GotchaMachine\\gotcha_ScrollBarBot.tga", bApplyWindowSize );

			m_pSpriteBG = NiNew cSprite;
			m_pSpriteBG->Init( this, CsPoint( 0, m_ptButtonSize.y ), CsPoint( m_ptButtonSize.x, CtrlSize.y - m_ptButtonSize.y*2 ), "Control_G\\Scroll02\\ScrollBG.tga", bApplyWindowSize );
		}
		break;
	case TYPE_ENCY:
		{
			m_pUpButton = NiNew cButton;
			m_pUpButton->Init( this, CsPoint::ZERO, btnSize, "Encyclopedia\\Scroll\\ScrollUpBtn.tga", bApplyWindowSize );
			m_pUpButton->SetTexToken( CsPoint( 0, 14 ) );
			m_pUpButton->SetEnable( false );

			m_pDownButton = NiNew cButton;
			m_pDownButton->Init( this, CsPoint( 0, CtrlSize.y - m_ptButtonSize.y ), btnSize, "Encyclopedia\\Scroll\\ScrollDownBtn.tga", bApplyWindowSize );
			m_pDownButton->SetTexToken( CsPoint( 0, 14 ) );
			m_pDownButton->SetEnable( false );

			m_pSpriteStart = NiNew cSprite;
			m_pSpriteStart->Init( this, CsPoint::ZERO, CsPoint( m_ptButtonSize.x, 2 ), "Encyclopedia\\Scroll\\ScrollBar_Top.tga", bApplyWindowSize );

			m_pSpriteCenter = NiNew cSprite;
			m_pSpriteCenter->Init( this, CsPoint::ZERO, CsPoint::ZERO, "Encyclopedia\\Scroll\\ScrollBar_Mid.tga", bApplyWindowSize );

			m_pSpriteEnd = NiNew cSprite;
			m_pSpriteEnd->Init( this, CsPoint::ZERO, CsPoint( m_ptButtonSize.x, 2 ), "Encyclopedia\\Scroll\\ScrollBar_Bot.tga", bApplyWindowSize );

			m_pSpriteBG = NiNew cSprite;
			m_pSpriteBG->Init( this, CsPoint( 0, m_ptButtonSize.y ), CsPoint( m_ptButtonSize.x, CtrlSize.y - m_ptButtonSize.y*2 ), "Encyclopedia\\Scroll\\ScrollBG.tga", bApplyWindowSize );
		}
		break;
	case TYPE_SEALMASTER:
		{
			m_pUpButton = NiNew cButton;
			m_pUpButton->Init( this, CsPoint::ZERO, btnSize, "SealMaster\\common_scroll_btn_01.tga", bApplyWindowSize );
			m_pUpButton->SetTexToken( CsPoint( 0, 16 ) );
			m_pUpButton->SetEnable( false );

			m_pDownButton = NiNew cButton;
			m_pDownButton->Init( this, CsPoint( 0, CtrlSize.y - m_ptButtonSize.y ), btnSize, "SealMaster\\common_scroll_btn_02.tga", bApplyWindowSize );
			m_pDownButton->SetTexToken( CsPoint( 0, 16 ) );
			m_pDownButton->SetEnable( false );

			m_pSpriteStart = NiNew cSprite;
			m_pSpriteStart->Init( this, CsPoint::ZERO, CsPoint( m_ptButtonSize.x, 8 ), "SealMaster\\common_scroll_bar_01.tga", bApplyWindowSize );
			//m_nBarTopHeight의 기본은 2 8로 증가 시키기 때문에 토탈 사이즈도 변경한다.
			m_nBarTopHeight = 8;

			m_pSpriteCenter = NiNew cSprite;
			m_pSpriteCenter->Init( this, CsPoint::ZERO, CsPoint::ZERO, "SealMaster\\common_scroll_bar_02.tga", bApplyWindowSize );

			m_pSpriteEnd = NiNew cSprite;
			m_pSpriteEnd->Init( this, CsPoint::ZERO, CsPoint( m_ptButtonSize.x, 8 ), "SealMaster\\common_scroll_bar_03.tga", bApplyWindowSize );
			m_nBarBotHeight = 8;

			m_pSpriteBG = NiNew cSprite;
			m_pSpriteBG->Init( this, CsPoint( 0, m_ptButtonSize.y ), CsPoint( m_ptButtonSize.x, CtrlSize.y - m_ptButtonSize.y*2 ), "SealMaster\\common_scroll_bg_02.tga", bApplyWindowSize );
		}
		break;
	default:
		assert_cs( false );
	}

	m_bEnableRenderFromEnableScroll = false;
	m_nCurPosIndex = m_nMaxPosIndex = 0;
}

void cScrollBar::OnKillFocus()
{
	if( m_bEnableScroll == true )
		_CalBarViewRect();
	m_eBarClickType = CB_NONE;
}

int cScrollBar::Update_ForMouse()
{
	if( !m_bVisible )
		return ACTION_NONE;

	// 로컬 마우스 좌표로
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	if( m_bEnableScroll == false )
	{
		m_eBarClickType = CB_NONE;
		return ACTION_NONE;
	}
	else if( m_rcRegion.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		if( CURSOR_ST.GetWheel() != INVALIDE_WHEEL )
		{			
			int nWheel = (int)( CURSOR_ST.GetWheel()*0.01f );
			SetMouseWheelMove( nWheel );
			CURSOR_ST.ResetWheel();
			return ACTION_CLICK;
		}
	}	

	switch( m_eBarClickType )
	{
	case CB_NONE:
		{
			// 바만 클릭
			if( m_rcBarView.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				if(CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN)
				{
					m_ptBackupLocalMousePos = ptMouseLocalPos;
					m_eBarClickType = CB_BAR;
					return ACTION_CLICK;
				}
			}
			// 뷰영역 클릭
			else if( m_rcBarTotal.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				if(CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN)
				{
					m_eBarClickType = CB_REGION;
					m_ScrollTimeSeq.SetDeltaTime( 300 );
					m_ScrollTimeSeq.OffReset();

					// 한번 움직여 주자
					_BarRegionClick( ptMouseLocalPos );
					return ACTION_CLICK;
				}
			}
		}
		break;
	case CB_BAR:
		{
			CURSOR::eTYPE m_nButtonPressState = CURSOR::LBUTTON_PRESS;

// 			if( !g_pResist->m_Global.s_bRightHandMouse )
// 			{
// 				m_nButtonPressState = CURSOR::RBUTTON_PRESS;
// 			}

			if( CURSOR_ST.GetButtonState() == m_nButtonPressState )
			{				
				int nDelta = ptMouseLocalPos.y - m_ptBackupLocalMousePos.y;
				m_ptBackupLocalMousePos = ptMouseLocalPos;

				if( m_rcBarView.top + nDelta < m_rcBarTotal.top )
					nDelta = m_rcBarTotal.top -m_rcBarView.top;
				if( m_rcBarView.bottom + nDelta > m_rcBarTotal.bottom )
					nDelta = m_rcBarTotal.bottom - m_rcBarView.bottom;

				m_rcBarView.top += nDelta;
				m_rcBarView.bottom += nDelta;

				m_nCurPosIndex = (int)( ( m_rcBarView.top - m_ptButtonSize.y + 0.5f*m_fPos_Height )/m_fPos_Height );
				return ACTION_CLICK;
			}
			else if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
			{
				_CalBarViewRect();				
			}
			m_eBarClickType = CB_NONE;
		}
		break;
	case CB_REGION:
		if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_PRESS )
		{
			if( m_ScrollTimeSeq.IsEnable() == true )
			{
				m_ScrollTimeSeq.SetDeltaTime( 100 );
				_BarRegionClick( ptMouseLocalPos );
			}
			return ACTION_CLICK;
		}
		m_eBarClickType = CB_NONE;
		break;
	}

	switch( m_pUpButton->Update_ForMouse() )	
	{
	case cButton::ACTION_DOWN:
		m_ScrollTimeSeq.SetDeltaTime( 300 );
		m_ScrollTimeSeq.OffReset();
		if( --m_nCurPosIndex < 0 )
			m_nCurPosIndex = 0;

		_CalBarViewRect();
		return ACTION_CLICK;
	case cButton::ACTION_PRESS:
		if( m_ScrollTimeSeq.IsEnable() == true )
		{
			m_ScrollTimeSeq.SetDeltaTime( 100 );
			if( --m_nCurPosIndex < 0 )
				m_nCurPosIndex = 0;
			_CalBarViewRect();
		}
		return ACTION_CLICK;
	}

	switch( m_pDownButton->Update_ForMouse() )	
	{
	case cButton::ACTION_DOWN:
		m_ScrollTimeSeq.SetDeltaTime( 300 );
		m_ScrollTimeSeq.OffReset();
		if( ++m_nCurPosIndex > m_nMaxPosIndex )
			m_nCurPosIndex = m_nMaxPosIndex;
		_CalBarViewRect();
		return ACTION_CLICK;
	case cButton::ACTION_PRESS:
		if( m_ScrollTimeSeq.IsEnable() == true )
		{
			m_ScrollTimeSeq.SetDeltaTime( 100 );
			if( ++m_nCurPosIndex > m_nMaxPosIndex )
				m_nCurPosIndex = m_nMaxPosIndex;
			_CalBarViewRect();
		}
		return ACTION_CLICK;
	}

	return ACTION_NONE;
}

void cScrollBar::Render()
{
	if( !m_bVisible )
		return;

	if( ( m_bEnableRenderFromEnableScroll == true )&&( m_bEnableScroll == false ) )
		return;

	assert_cs( m_nRenderCount > 0 );

	if( m_pUpButton )
		m_pUpButton->Render();
	if( m_pDownButton )
		m_pDownButton->Render();
	if( m_pSpriteBG )
		m_pSpriteBG->Render();

	if( m_bEnableScroll == true )
	{
		assert_cs( m_rcBarView.Height() == m_nBarViewHeight );
		CsPoint ptStart = CsPoint( 0, m_rcBarView.top );
		if( m_pSpriteStart )
			m_pSpriteStart->Render( ptStart );

		ptStart.y += m_nBarTopHeight;
		int nCenterHeight = m_nBarViewHeight-(m_nBarTopHeight+m_nBarBotHeight);
		assert_cs( nCenterHeight > 0 );
		if( m_pSpriteCenter )
			m_pSpriteCenter->Render( ptStart, CsPoint( m_ptButtonSize.x, nCenterHeight ) );

		ptStart.y += nCenterHeight;
		if( m_pSpriteEnd )
			m_pSpriteEnd->Render( ptStart );
	}	
}

void cScrollBar::ResetDevice()
{
	cWindow::ResetDevice();

	m_pUpButton->ResetDevice();
	m_pDownButton->ResetDevice();

	m_pSpriteStart->ResetDevice();
	m_pSpriteCenter->ResetDevice();
	m_pSpriteEnd->ResetDevice();
	m_pSpriteBG->ResetDevice();
}

void cScrollBar::SetAlpha( float fAlpha )
{
	m_pUpButton->SetAlpha( fAlpha );
	m_pDownButton->SetAlpha( fAlpha );

	m_pSpriteStart->SetAlpha( fAlpha );
	m_pSpriteCenter->SetAlpha( fAlpha );
	m_pSpriteEnd->SetAlpha( fAlpha );
	m_pSpriteBG->SetAlpha( fAlpha );
}

void cScrollBar::SetRenderCount( int nRenderCount )
{
	if( m_nRenderCount == nRenderCount )
		return;

	m_nRenderCount = nRenderCount;
	_ChangeBarConstant();
}

void cScrollBar::ReSize( CsPoint ptSize, CsRect rcRegion )
{
	if( m_pDownButton )
		m_pDownButton->SetPos( CsPoint( 0, ptSize.y - m_ptButtonSize.y ) );

	if( m_pSpriteBG )
		m_pSpriteBG->SetSize( CsPoint( ptSize.x, ptSize.y - m_ptButtonSize.y*2 ) );

	m_rcRegion = rcRegion - GetDefaultPos();
	m_nBarTotalHeight = ptSize.y - m_ptButtonSize.y*2;


	_ChangeBarConstant();
	_CalBarViewRect();
}

bool cScrollBar::SetRange( CsPoint ptRange )
{
	if( m_ptRange != ptRange )
	{
		m_ptRange = ptRange;
		_ChangeBarConstant();
	}

	return m_bEnableScroll;
}

void cScrollBar::IncreaseMaxCount( int nLimitMax /*=INT_MAX*/ )
{
	if( m_ptRange.y + 1 <= nLimitMax )
	{
		++m_ptRange.y;
		_ChangeBarConstant();
	}
}

void cScrollBar::_ChangeBarConstant()
{
	int nDeltaRange = m_ptRange.y - m_ptRange.x;
	if( nDeltaRange > m_nRenderCount )
	{
		if( m_bEnableScroll == false )
			m_nCurPosIndex = 0;

		m_bEnableScroll = true;
		// 바의 최하길이 - 8

		int extendBarHeight = (m_nBarTopHeight - IMAGE_DEFAULT_HEIGHT) + (m_nBarBotHeight - IMAGE_DEFAULT_HEIGHT);
		m_nBarViewHeight = CsMax( 8 + extendBarHeight, (int)( m_nBarTotalHeight*(float)m_nRenderCount/(float)nDeltaRange ) );

		m_nMaxPosIndex = nDeltaRange - m_nRenderCount;
		if( m_nCurPosIndex > m_nMaxPosIndex )
		{
			m_nCurPosIndex = m_nMaxPosIndex;
		}

		// 바의 최하 길이(8) 보다 더 작은 경우를 위해 절대값으로 구한다.
		m_fPos_Height = abs( m_nBarTotalHeight - m_nBarViewHeight )/(float)m_nMaxPosIndex;
		
		m_rcBarTotal = CsRect( 0, m_ptButtonSize.y, m_ptButtonSize.x, m_ptButtonSize.y + m_nBarTotalHeight );

		_CalBarViewRect();
	}
	else
	{
		m_bEnableScroll = false;
		m_nCurPosIndex = 0;
		m_nMaxPosIndex = 0;
	}
	if(m_pUpButton)
		m_pUpButton->SetEnable( m_bEnableScroll );
	if(m_pDownButton)
		m_pDownButton->SetEnable( m_bEnableScroll );
}

bool cScrollBar::_BarRegionClick( CsPoint ptMouseLocalPos )
{
	bool bUp = m_rcBarView.top > ptMouseLocalPos.y ? true : false;

	for( int i=0; i<m_nRegionClickMoveIndex; ++i )
	{
		if( m_rcBarView.PtInRect( ptMouseLocalPos ) == TRUE )
			return true;

		if( bUp == true )
		{
			if( --m_nCurPosIndex < 0 )
			{
				m_nCurPosIndex = 0;
				return true;
			}
		}
		else
		{
			if( ++m_nCurPosIndex > m_nMaxPosIndex )
			{
				m_nCurPosIndex = m_nMaxPosIndex;
				return true;
			}
		}
		_CalBarViewRect();
	}
	return false;
}

void cScrollBar::_CalBarViewRect()
{
	int nStartBarView = (int)( m_fPos_Height*m_nCurPosIndex ) + m_ptButtonSize.y;
	m_rcBarView = CsRect( 0, nStartBarView, m_ptButtonSize.x, nStartBarView + m_nBarViewHeight );
}

void cScrollBar::SetCurPosIndex( int nIndex )
{
	if( IsEnableScroll() == false )
	{
		assert_cs( m_nCurPosIndex == 0 );
		return;
	}
	m_nCurPosIndex = CsMin( nIndex, m_nMaxPosIndex );
	_CalBarViewRect();
}

void cScrollBar::SetMouseWheelMove( int nWheel )
{
	if( IsEnableScroll() == false )
	{
		assert_cs( m_nCurPosIndex == 0 );
		return;
	}
	m_nCurPosIndex = CsMax( 0, CsMin( m_nMaxPosIndex, m_nCurPosIndex - nWheel ) );
	_CalBarViewRect();
}