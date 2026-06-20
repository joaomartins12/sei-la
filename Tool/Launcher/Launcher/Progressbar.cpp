#include "StdAfx.h"
#include "Progressbar.h"

cProgressbar::cProgressbar(void)
{
}

cProgressbar::~cProgressbar(void)
{
}

void cProgressbar::Delete()
{	
}

void cProgressbar::Init( UINT nIDresource, CPoint ptPos, CPoint ptSize )
{
	m_bmpDefault.LoadBitmap( nIDresource );
	m_ptPos = ptPos;
	m_ptMaxSize = ptSize;
	m_ptSize = CPoint( 0, 0 );
}

void cProgressbar::Update()
{

}

void cProgressbar::Render( CDC* pMemDc, CDC* pTempDC )
{
	CDC bufferDC;
	CBitmap* pOldBitmap;
	bufferDC.CreateCompatibleDC( pMemDc );

	BITMAP	bmpInfo;
	m_bmpDefault.GetBitmap( &bmpInfo );
	pOldBitmap = bufferDC.SelectObject( &m_bmpDefault );

	int nHLoop;
	int nVLoop;
	int nHTiles;
	int nVTiles;

	// Calc number of horizontal tiles
	nHTiles = (m_ptSize.x / bmpInfo.bmWidth);
	if (m_ptSize.x % bmpInfo.bmWidth != 0) nHTiles++;
	// Calc number of vertical tiles
	nVTiles = (m_ptSize.y / bmpInfo.bmHeight);
	if (m_ptSize.y % bmpInfo.bmHeight != 0) nVTiles++;

	// Tile bitmap horizontally
	for (nHLoop = 0; nHLoop < nHTiles; nHLoop++)
	{
		// Tile bitmap vertically
		for (nVLoop = 0; nVLoop < nVTiles; nVLoop++)
		{
			pTempDC->BitBlt( m_ptPos.x + (nHLoop*bmpInfo.bmWidth), m_ptPos.y + (nVLoop*bmpInfo.bmHeight), 
				bmpInfo.bmWidth, bmpInfo.bmHeight, &bufferDC, 0, 0, SRCCOPY );
		}
	}	
	
	bufferDC.SelectObject( pOldBitmap );
	bufferDC.DeleteDC();	
}

void cProgressbar::SetPersent( float fPer )
{
	if( fPer >= 1.0f )
		fPer = 1.0f;

	m_ptSize.x = (int)( m_ptMaxSize.x * fPer );
	m_ptSize.y = m_ptMaxSize.y;
}

