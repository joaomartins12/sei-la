
#include "stdafx.h"
#include "Slider.h"

cSlider::cSlider()
{
	m_pLeftBtn = NULL;
	m_pRightBtn = NULL;
	m_pBar = NULL;
}

void cSlider::Delete()
{
	SAFE_NIDELETE( m_pLeftBtn );
	SAFE_NIDELETE( m_pRightBtn );
	SAFE_NIDELETE( m_pBar );
}

void cSlider::Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize )
{
	assert_cs( size.x >= 50 );

	cWindow::Init( pParent, pos, size, bApplyWindowSize );


	assert_cs( m_pLeftBtn == NULL );
	assert_cs( m_pRightBtn == NULL );
	assert_cs( m_pBar == NULL );

	int nBtnDeltaY = ( size.y - 17 ) / 2;
	m_ptBarPos.x = 0;
	m_ptBarPos.y = ( size.y - 19 ) / 2;

	m_pLeftBtn = NiNew cButton;
	m_pLeftBtn->Init( this, CsPoint( 0, nBtnDeltaY ), CsPoint( 9, 17 ) , "Control_G\\Slider\\Lbtn.tga", bApplyWindowSize );
	m_pLeftBtn->SetTexToken( CsPoint( 9, 0 ) );

	m_pRightBtn = NiNew cButton;
	m_pRightBtn->Init( this, CsPoint( size.x - 9, nBtnDeltaY ), CsPoint( 9, 17 ) , "Control_G\\Slider\\Rbtn.tga", bApplyWindowSize );
	m_pRightBtn->SetTexToken( CsPoint( 9, 0 ) );

	m_pBar = NiNew cSprite;
	m_pBar->Init( this, CsPoint::ZERO, CsPoint( 19, 19 ), "Control_G\\Slider\\Bar.tga", bApplyWindowSize );

	m_ptBarRegion = CsPoint( 16, size.x - 35 );

	m_eBarClickType = CB_NONE;

	m_nCurPosIndex = 0;

	m_nBtnMoveRange = 1;	

	m_nLastState = ACTION_NONE;
	m_bEnable = true;
}

void cSlider::OnKillFocus()
{
	_CalBarPos();
	m_eBarClickType = CB_NONE;
}

int cSlider::Update_ForMouse()
{
	if( !m_bEnable || !m_bVisible )
		return m_nLastState = ACTION_NONE;

	// 로컬 마우스 좌표로
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	switch( m_eBarClickType )
	{
	case CB_NONE:
		{
			// 바클릭			
			CsRect rc( m_ptBarPos, CsSIZE( 19, 19 ) );
			if( rc.PtInRect( ptMouseLocalPos ) )
			{
				if(CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN)
				{
					m_ptBackupLocalMousePos = ptMouseLocalPos;
					m_eBarClickType = CB_BAR;
					return m_nLastState = ACTION_CLICK;
				}
			}
			else
			{
				m_eBarClickType = CB_NONE;
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
				int nDelta = ptMouseLocalPos.x - m_ptBackupLocalMousePos.x;
				m_ptBackupLocalMousePos = ptMouseLocalPos;

				if( m_ptBarPos.x + nDelta < m_ptBarRegion.x )
					nDelta = m_ptBarRegion.x - m_ptBarPos.x;
				if( m_ptBarPos.x + nDelta > m_ptBarRegion.y )
					nDelta = m_ptBarRegion.y - m_ptBarPos.x;

				m_ptBarPos.x += nDelta;
				m_nCurPosIndex = (int)( ( m_ptBarPos.x - m_ptBarRegion.x + 0.5f*m_fPos_Width )/m_fPos_Width );
				return m_nLastState = ACTION_CLICK;
			}
			else if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
			{
				_CalBarPos();
			}
			m_eBarClickType = CB_NONE;
		}
		break;	
	}

	switch( m_pLeftBtn->Update_ForMouse() )	
	{
	case cButton::ACTION_CLICK:
		if( ( m_nCurPosIndex -= m_nBtnMoveRange ) < 0 )
			m_nCurPosIndex = 0;		
		_CalBarPos();
		return m_nLastState = ACTION_CLICK;	
	}

	switch( m_pRightBtn->Update_ForMouse() )	
	{
	case cButton::ACTION_CLICK:
		if( ( m_nCurPosIndex += m_nBtnMoveRange ) > m_nMaxPosIndex )
			m_nCurPosIndex = m_nMaxPosIndex;
		_CalBarPos();
		return m_nLastState = ACTION_CLICK;	
	}	

	CsRect rc( m_ptBarPos, CsSIZE( 19, 19 ) );
	if( rc.PtInRect( ptMouseLocalPos ) )
	{
		return m_nLastState = ACTION_ON;
	}

	return m_nLastState = ACTION_NONE;
}

void cSlider::Render()
{
	if( !m_bVisible )
		return;

	if( m_pLeftBtn )
		m_pLeftBtn->Render();
	if( m_pRightBtn )
		m_pRightBtn->Render();
	if( m_pBar )
		m_pBar->Render( m_ptBarPos );
}

void cSlider::Render( CsPoint pos )
{
	SetPos( pos );
	if( m_pLeftBtn )
		m_pLeftBtn->ResetDevice();
	if( m_pRightBtn )
		m_pRightBtn->ResetDevice();
	if( m_pBar )
		m_pBar->ResetDevice();

	Render();
}

void cSlider::ResetDevice()
{
	cWindow::ResetDevice();

	m_pLeftBtn->ResetDevice();
	m_pRightBtn->ResetDevice();
	m_pBar->ResetDevice();
}

void cSlider::SetAlpha( float fAlpha )
{
	m_pLeftBtn->SetAlpha( fAlpha );
	m_pRightBtn->SetAlpha( fAlpha );
	m_pBar->SetAlpha( fAlpha );
}

void cSlider::SetRange( CsPoint ptRange )
{
	m_ptRange = ptRange;
	_ChangeBarConstant();
}

void cSlider::SetCurPosIndex( int nIndex )
{
	m_nCurPosIndex = nIndex-m_ptRange.x;
	if( m_nCurPosIndex < 0 )
		m_nCurPosIndex = 0;
	if( m_nCurPosIndex > m_nMaxPosIndex )
		m_nCurPosIndex = m_nMaxPosIndex;
	_CalBarPos();
}

void cSlider::_ChangeBarConstant()
{
	m_nMaxPosIndex = m_ptRange.y - m_ptRange.x;
	if( m_nCurPosIndex > m_nMaxPosIndex )
	{
		m_nCurPosIndex = m_nMaxPosIndex;
	}
	m_fPos_Width = ( m_ptBarRegion.y - m_ptBarRegion.x )/(float)m_nMaxPosIndex;
	_CalBarPos();
}

void cSlider::_CalBarPos()
{
	m_ptBarPos.x = (int)( m_fPos_Width*m_nCurPosIndex ) + m_ptBarRegion.x;
}

void cSlider::Enable( bool bEnable )
{
	m_bEnable = bEnable;
	if( m_bEnable )
	{
		SetAlpha( 1.0f );			
	}
	else
	{
		SetAlpha( 0.3f );			
	}	
	_ChangeBarConstant();
}

