// BmpButton.cpp : implementation file
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "BmpButton.h"
#include "AutoUpgradeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpButton

CBmpButton::CBmpButton()
{
	m_bEnable = true;
	m_iButtonKind = -1 ;
	m_pkParent = 0 ;
}

CBmpButton::~CBmpButton()
{
	if ( m_hWnd != NULL )
		KillTimer(0);
}


BEGIN_MESSAGE_MAP(CBmpButton, CStatic)
	//{{AFX_MSG_MAP(CBmpButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpButton message handlers

//--------------------------------------------------
//-- LoadButtonBitmaps
// Use this function to load the bitmaps you will 
// use for different states of the button
//---------------------------------------------------
BOOL CBmpButton::LoadButtonBitmaps(int idDefault, int idUp, int idDown, int idDisable)
{
	BOOL retVal = TRUE;

	retVal &= m_bmpDefault.LoadBitmap(idDefault);

	if (idUp < 0) m_bmpUp.LoadBitmap(idDefault);
	else retVal &= m_bmpUp.LoadBitmap(idUp);

	if (idDown < 0) m_bmpDown.LoadBitmap(idDefault);
	else retVal &= m_bmpDown.LoadBitmap(idDown);

	if (idDisable < 0) m_bmpDisable.LoadBitmap(idDefault);
	else retVal &= m_bmpDisable.LoadBitmap(idDisable);

	return retVal;
}

//--------------------------------------------------
//-- ResetButton
// Use this function to set the button default state
//---------------------------------------------------
void CBmpButton::ResetButton()
{
	if ( m_bEnable == true )
		SetBitmap(HBITMAP(m_bmpDefault));
	else
		SetBitmap(HBITMAP(m_bmpDisable));
}

//--------------------------------------------------
//-- ON_WM_LBUTTONDOWN
// Change the bitmap to "down" state
//---------------------------------------------------
void CBmpButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_bEnable == true )
	{
		// TODO: Add your message handler code here and/or call default
		SetBitmap(HBITMAP(m_bmpDown));
	}	

	CStatic::OnLButtonDown(nFlags, point);
}

//---------------------------------------------------
//-- ON_WM_LBUTTONDOWN
// Change the bitmap to "up" state
//---------------------------------------------------
void CBmpButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bEnable == true )
	{
		if( GetBitmap() == HBITMAP(m_bmpDown) )
		{
			if( m_pkParent && m_iButtonKind>-1 )
			{
				m_pkParent->ProcessButtonUp( m_iButtonKind ) ;
				SetBitmap(HBITMAP(m_bmpDefault));
			}
		}
		else
			return;
	}	
	
	CStatic::OnLButtonUp(nFlags, point);
}

//--------------------------------------------------
//-- ON_WM_MOUSEMOVE
// Change the bitmap to "selected" state
//---------------------------------------------------
void CBmpButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bEnable == true )
	{
		if( GetBitmap() != HBITMAP(m_bmpDown) )
			SetBitmap(HBITMAP(m_bmpUp));
	}	

	CStatic::OnMouseMove(nFlags, point);
}

void CBmpButton::SetButtonEable(bool bEnable)
{
	m_bEnable = bEnable;
	Invalidate();
}

void CBmpButton::OnTimer(UINT nIDEvent) 
{
	CStatic::OnTimer(nIDEvent);
}


