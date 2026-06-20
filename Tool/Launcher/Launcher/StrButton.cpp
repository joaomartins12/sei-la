#include "StdAfx.h"
#include "StrButton.h"

cStrButton::cStrButton(void)
{
}

cStrButton::~cStrButton(void)
{
	m_font.DeleteObject();
}

void cStrButton::Init( CDialog* pkParent, CPoint ptPos, TCHAR* szStr, bool bUndetline, COLORREF nColor, COLORREF nOverColor, int nFontSize /* = 14 */ )
{
	m_pkParent = pkParent;
	_tcscpy_s( m_szStr, 64, szStr );
	m_nColor = nColor;
	m_nOverColor = nOverColor;
	m_ptPos = ptPos;

	m_font.CreateFont( nFontSize, 0, 0, 0, FW_NORMAL, 0, bUndetline, 0, ANSI_CHARSET, 0, 0, 0, 0, _T("°íµñÃ¼") );

	CDC* pDc = m_pkParent->GetDC();
	CFont* pOldFont = pDc->SelectObject( &m_font );	
	CSize ptSize = pDc->GetTextExtent( szStr, (int)_tcslen( szStr ) );
	m_ptSize.x = ptSize.cx;
	m_ptSize.y = ptSize.cy;

	m_rc.SetRect( m_ptPos, m_ptPos + m_ptSize );

	pDc->SelectObject( pOldFont );
}

void cStrButton::Update( eSTATE eMouseState )
{
	CPoint ptCurPos;
	GetCursorPos( &ptCurPos );	

	CRect rcWin;
	m_pkParent->GetWindowRect( rcWin );		

	CRect rc;
	rc.SetRect( CPoint( rcWin.left, rcWin.top ) + CPoint( m_rc.left, m_rc.top ), 
		CPoint( rcWin.left, rcWin.top ) + CPoint( m_rc.right, m_rc.bottom ) );
	if( rc.PtInRect( ptCurPos ) == TRUE )
	{		
		m_eState = eMOUSE_ON;
		if( eMouseState == eMOUSE_DOWN )
		{
			m_eState = eMOUSE_DOWN;
		}		
	}
	else
	{
		m_eState = eMOUSE_DEFAULT;			
	}		
}

void cStrButton::Render( CDC* pDC )
{
	CFont* pFont = pDC->SelectObject( &m_font );
	pDC->SetBkMode( TRANSPARENT );					

	if( m_eState == eMOUSE_ON || m_eState == eMOUSE_DOWN )
	{		
		pDC->SetTextColor( m_nOverColor );				
	}
	else
	{
		pDC->SetTextColor( m_nColor );
	}

	pDC->TextOut( m_ptPos.x, m_ptPos.y, m_szStr );				
	pDC->SelectObject( pFont );		
}