

#include "stdafx.h"
#include "AutoUpgradeDlg.h"

void CAutoUpgradeDlg::_ResetButton()
{
	m_bmpStart.ResetButton();
	m_bmpCancle.ResetButton();
	m_bmpOption.ResetButton();
}

void CAutoUpgradeDlg::RenderText(CDC *pDC)
{
	const int _iPosY = 290;

	CFont font;
	font.CreatePointFont(90, _T("Tahoma"));
	CFont* pFont = pDC->SelectObject(&font);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(212,203,200));	

	pDC->TextOut( _MainState_Pos_x, _MainState_Pos_y, m_szMainState );
	pDC->TextOut( _SubState_Pos_x, _SubState_Pos_y, m_szSubState );

	pDC->SelectObject(font);

	//Launcher Version
	CFont font2;
	font2.CreatePointFont(80, _T("Tahoma"));
	pFont = pDC->SelectObject(&font2);
	pDC->SetTextColor(RGB(212,203,200));		

	CString szVersion;
	szVersion.Format( L"Launcher V.0.%d", m_CurrentLauncherVersion);

	pDC->TextOut(569, 405 + 64, szVersion);
	pDC->SelectObject(font2);

	//coptright
	CFont font3;
	font3.CreatePointFont(95, _T("Tahoma"));
	pFont = pDC->SelectObject(&font3);
	pDC->SetTextColor(RGB(212,203,200));

	pDC->TextOut(190, 500, COPYLIGHT);
	pDC->SelectObject(pFont);
}

void CAutoUpgradeDlg::_SetRange_MainProgress( int nMaxPos )
{
	m_ctrlProgressMain.SetRange( 0, nMaxPos );
}

void CAutoUpgradeDlg::_SetPos_MainProgress( int nPos )
{
	m_ctrlProgressMain.SetPos( nPos );

	CRect rect;
	rect.left = _ProgressMainX;
	rect.top = _ProgressMainY;
	rect.right = _ProgressMainX + _ProgressWidth;
	rect.bottom = _ProgressMainY + _ProgressHeight;
	InvalidateRect( &rect, TRUE );
}

void CAutoUpgradeDlg::_SetRange_SubDownProgress( int nMaxPos )
{
	m_ctrlProgressSubDown.SetRange( 0, nMaxPos );
}

void CAutoUpgradeDlg::_SetPos_SubDownProgress( int nPos )
{	
	BOOL bErase = ( nPos == 0 ) ? TRUE : FALSE;
	int nTempX = _ProgressSubDownX;
	if( bErase == FALSE )
	{
		int l, h;
		m_ctrlProgressSubDown.GetRange( l, h );
		assert( l == 0 );

		int w = max( 0, (int)( _ProgressWidth*( m_ctrlProgressSubDown.GetPos()/(float)h ) ) - 1 );
		nTempX += w;
	}

	m_ctrlProgressSubDown.SetPos( nPos );	

	CRect rect;
	rect.left = nTempX;
	rect.top = _ProgressSubDownY;
	rect.right = _ProgressSubDownX + _ProgressWidth;
	rect.bottom = _ProgressSubDownY + _ProgressHeight;
	InvalidateRect( &rect, bErase );
}

void CAutoUpgradeDlg::_SetRange_SubUnzipProgress( int nMaxPos )
{
	m_ctrlProgressSubUnzip.SetRange( 0, nMaxPos );
}

void CAutoUpgradeDlg::_SetPos_SubUnzipProgress( int nPos )
{
	BOOL bErase = ( nPos == 0 ) ? TRUE : FALSE;
	int nTempX = _ProgressSubUnzipX;
	if( bErase == FALSE )
	{
		int l, h;
		m_ctrlProgressSubUnzip.GetRange( l, h );
		assert( l == 0 );

		int w = max( 0, (int)( _ProgressWidth*( m_ctrlProgressSubUnzip.GetPos()/(float)h ) ) - 1 );
		nTempX += w;
	}

	m_ctrlProgressSubUnzip.SetPos( nPos );

	CRect rect;
	rect.left = nTempX;
	rect.top = _ProgressSubUnzipY;
	rect.right = _ProgressSubUnzipX + _ProgressWidth;
	rect.bottom = _ProgressSubUnzipY + _ProgressHeight;
	InvalidateRect( &rect, bErase );
}