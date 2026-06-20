
#include "stdafx.h"
#include "CsSpinCtrl.h"


BEGIN_MESSAGE_MAP(CsSpinCtrl, CSpinButtonCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


#define CSSPIN_DELTA_PIXEL		1


int CsSpinCtrl::m_nDispHeight = 0;

// ==============================================================================================
//
//		Base
//
// ==============================================================================================

CsSpinCtrl::CsSpinCtrl()
:m_CallBackMove(NULL),
m_CallbackMouseDown(NULL),
m_CallbackMouseUp(NULL)
{
	m_bRange = false;
	m_bDown = false;

	m_nDeltaPixel = CSSPIN_DELTA_PIXEL;

	m_pCsEdit = NULL;
	m_CallBackMove = NULL;
	m_fAccel = 1.0f;

	// 함수 포인터 연결 - 기본 플로우트형으로
	fPushButton			=	&CsSpinCtrl::_PushButtonFloat;
	fPushMove			=	&CsSpinCtrl::_PushMoveFloat;
}


void CsSpinCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_nDispPage =	0;
	m_bDown		=	true;
	m_ptMouse	=	point;

	(this->*fPushButton)( point );

	if( m_CallbackMouseDown != NULL )
	{
		float ClickedPart = 1.0;
		if( point.y > 12)
			ClickedPart = -1.0f;
		m_CallbackMouseDown( ClickedPart );
	}

	CSpinButtonCtrl::OnLButtonDown(nFlags, point);
}

void CsSpinCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDown		=	false;
	m_ptMouse	=	point;	
	
	if( m_CallbackMouseUp != NULL )
	{
		float ClickedPart = 1.0;
		if( point.y > 12)
			ClickedPart = -1.0f;
		m_CallbackMouseUp( ClickedPart );
	}

	CSpinButtonCtrl::OnLButtonUp(nFlags, point);
}

void CsSpinCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_nDispPage =	0;
	m_bDown		=	true;
	m_ptMouse	=	point;
	(this->*fPushButton)( point );

	CSpinButtonCtrl::OnLButtonDblClk(nFlags, point);
}

void CsSpinCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDown == false )	return;

	CPoint ptCursor;
	GetCursorPos( &ptCursor );
	if( ptCursor.y == 0 )
	{
		//++m_nDispPage;
		int nOldCursorY = ptCursor.y;
		ptCursor.y = m_nDispHeight - 2;
		SetCursorPos( ptCursor.x, ptCursor.y );
		point.y += ( ptCursor.y - nOldCursorY );
		m_ptMouse = point;
		return;
	}
	else if( ptCursor.y == m_nDispHeight - 1 )
	{
		//--m_nDispPage;
		int nOldCursorY = ptCursor.y;
		ptCursor.y = 1;
		SetCursorPos( ptCursor.x, ptCursor.y );
		point.y += ( ptCursor.y - nOldCursorY );
		m_ptMouse = point;
		m_ptMouse = point;
		return;
	}

	//point.y -= m_nDispPage*m_nDispHeight;
	(this->*fPushMove)(point);

	CSpinButtonCtrl::OnMouseMove(nFlags, point);
}


// ==============================================================================================
//
//		Int
//
// ==============================================================================================

void CsSpinCtrl::SetCsBuddy( CsEdit* pBuddyEdit, int nAccel )
{
	m_pCsEdit = pBuddyEdit;
	m_nAccel = nAccel;

	// 함수 포인터 연결
	fPushButton			=	&CsSpinCtrl::_PushButtonInt;
	fPushMove			=	&CsSpinCtrl::_PushMoveInt;

	if( m_pCsEdit != NULL )
		SetBuddy( m_pCsEdit );
}

void CsSpinCtrl::SetCsRange( int nMin, int nMax )
{
	assert_cs( nMin <= nMax );

	m_nMin = nMin;
	m_nMax = nMax;
	m_bRange = true;
}

void CsSpinCtrl::_PushButtonInt(POINT pt)
{
	if( m_pCsEdit == NULL )
		return;

	m_nCurValue = nsCsMfcFunc::Control2Int( m_pCsEdit );
}

void CsSpinCtrl::_PushMoveInt( POINT pt )
{
	if( abs( m_ptMouse.y - pt.y ) < m_nDeltaPixel )
		return;

	int nDelta = ( (m_ptMouse.y - pt.y)/m_nDeltaPixel ) * m_nAccel;
	m_ptMouse.y -= (int)( m_nDeltaPixel*nDelta/m_nAccel );

	if( m_CallBackMove != NULL )
	{
		m_CallBackMove( (float)nDelta );
	}

	if( m_pCsEdit != NULL )
	{
		m_nCurValue += nDelta;
		// 스핀의 최대 값 지정
		if( m_bRange == true )
		{
			if( m_nCurValue > m_nMax )
			{
				nDelta = nDelta - m_nCurValue - m_nMax;
				m_nCurValue = m_nMax;	
			}
			else if( m_nCurValue < m_nMin )
			{
				nDelta = nDelta - m_nCurValue - m_nMin;
				m_nCurValue = m_nMin;
			}
		}

		nsCsMfcFunc::Int2Control( m_pCsEdit, m_nCurValue );
	}	
}


// ==============================================================================================
//
//		float
//
// ==============================================================================================

void CsSpinCtrl::SetCsBuddy( CsEdit* pBuddyEdit, float fAccel, int nDecimalPlace )
{
	m_pCsEdit = pBuddyEdit;
	m_bDown = false;
	m_fAccel = fAccel;
	m_nDecimalPlace = nDecimalPlace;

	// 함수 포인터 연결
	fPushButton			=	&CsSpinCtrl::_PushButtonFloat;
	fPushMove			=	&CsSpinCtrl::_PushMoveFloat;

	if( m_pCsEdit != NULL )
		SetBuddy( m_pCsEdit );
}

void CsSpinCtrl::_PushButtonFloat(POINT pt)
{
	if( m_pCsEdit == NULL )
		return;
	
	m_fCurValue = nsCsMfcFunc::Control2Float( m_pCsEdit );
}

void CsSpinCtrl::_PushMoveFloat( POINT pt )
{
	if( abs( m_ptMouse.y - pt.y ) < m_nDeltaPixel )
		return;

	float fDelta = ( (m_ptMouse.y - pt.y)/m_nDeltaPixel ) * m_fAccel;
	m_ptMouse.y -= (int)( m_nDeltaPixel*fDelta/m_fAccel );

	if( m_CallBackMove != NULL )
	{
		m_CallBackMove( fDelta );
	}

	if( m_pCsEdit != NULL )
	{
		m_fCurValue += fDelta;
		/*if( m_SpinFloat.fCurValue > m_SpinFloat.fMax )
			m_SpinFloat.fCurValue = m_SpinFloat.fMax;
		else if( m_SpinFloat.fCurValue < m_SpinFloat.fMin )
			m_SpinFloat.fCurValue = m_SpinFloat.fMin;*/

		nsCsMfcFunc::Float2Control( m_pCsEdit, m_fCurValue, m_nDecimalPlace );
	}
}