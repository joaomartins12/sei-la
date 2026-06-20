#include "StdAfx.h"
#include "BmpButton.h"

cBmpButton::cBmpButton(void)
{
}

cBmpButton::~cBmpButton(void)
{
}

void cBmpButton::Init( CDialog* pkParent, CPoint ptPos, CPoint ptSize, int idDefault, int idUp, int idDown, int idDisable )
{		
	m_pkParent = pkParent ; 
	m_bmpDefault.LoadBitmap(idDefault);
	m_bmpUp.LoadBitmap(idUp);
	m_bmpDown.LoadBitmap(idDown);
	m_bmpDisable.LoadBitmap(idDisable);

	m_ptPos = ptPos;
	m_ptSize = ptSize;

	m_eState = eMOUSE_DEFAULT;
	m_pCurBitmap = &m_bmpDefault;	
}

void cBmpButton::Update( eSTATE eMouseState )
{
	if( m_eState == eMOUSE_DISABLE )
		return;

	CPoint ptCurPos;
	GetCursorPos( &ptCurPos );	

	CRect rcWin;
	m_pkParent->GetWindowRect( rcWin );		

	CRect rc;
	rc.SetRect( CPoint( rcWin.left, rcWin.top ) + m_ptPos, CPoint( rcWin.left, rcWin.top ) + m_ptPos + m_ptSize );
	if( rc.PtInRect( ptCurPos ) == TRUE )
	{
		if( eMouseState == eMOUSE_DOWN )
		{
			SetButton( eMOUSE_DOWN );			
		}
		else
		{			
			SetButton( eMOUSE_ON );			
		}		
	}
	else
	{
		SetButton( eMOUSE_DEFAULT );			
	}	
	
}

void cBmpButton::Render( CDC* pDc, CDC* pTempDC )
{
	//pTempDC->StretchBlt( m_ptPos.x, m_ptPos.y, m_ptSize.x, m_ptSize.y, &bufferDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY );	

	BMP_RENDER::Render_ColorKey( pDc, pTempDC, m_ptPos.x, m_ptPos.y, m_pCurBitmap );

	//CLauncherDlg::Render_A( pTempDC, m_ptPos.x, m_ptPos.y, bmpInfo.bmWidth, bmpInfo.bmHeight, &bufferDC, 0, 0, RGB( 0, 0, 0 ) );	

	//BLENDFUNCTION bf;
	//bf.BlendOp = AC_SRC_OVER;
	//bf.BlendFlags = 0;
	//bf.SourceConstantAlpha = 150;
	//bf.AlphaFormat = 0;
	//pTempDC->AlphaBlend( m_ptPos.x, m_ptPos.y, bmpInfo.bmWidth, bmpInfo.bmHeight, &bufferDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf );
}

void cBmpButton::SetButton( eSTATE eState )
{
	m_eState = eState;

	switch( m_eState )
	{
	case eMOUSE_DEFAULT:
		m_pCurBitmap = &m_bmpDefault;
		break;
	case eMOUSE_ON:
		m_pCurBitmap = &m_bmpUp;
		break;
	case eMOUSE_DOWN:
		m_pCurBitmap = &m_bmpDown;
		break;
	case eMOUSE_DISABLE:
		m_pCurBitmap = &m_bmpDisable;
		break;	
	}		
}

