#include "stdafx.h"
#include "BmpRender.h"

void BMP_RENDER::Render_Bmp( CDC * pDC, int nX, int nY, CBitmap* pBitmap, bool bAlpa )
{
	CDC dcImage; 	
	dcImage.CreateCompatibleDC(pDC);

	BITMAP	bmpInfo;
	pBitmap->GetBitmap( &bmpInfo );	
	CBitmap* pOldBitmapImage = dcImage.SelectObject(pBitmap);		

	if( bAlpa == false )
		pDC->BitBlt( nX, nY, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcImage, 0, 0, SRCCOPY );	
	else
	{
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 50;
		bf.AlphaFormat = 0;
		pDC->AlphaBlend( nX, nY, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcImage, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf );
	}

	dcImage.SelectObject(pOldBitmapImage);	
}

void BMP_RENDER::Render_ColorKey( CDC* pDC, CDC* pTempDC, int nX, int nY, CBitmap* pBitmap )
{
	CDC bufferDC;
	CBitmap* pOldBitmap;
	bufferDC.CreateCompatibleDC( pDC );

	BITMAP	bmpInfo;
	pBitmap->GetBitmap( &bmpInfo );
	pOldBitmap = bufferDC.SelectObject( pBitmap );

	_Render_ColorKey( pTempDC, nX, nY, bmpInfo.bmWidth, bmpInfo.bmHeight, &bufferDC, 0, 0, RGB( 255, 0, 255 ) );	

	bufferDC.SelectObject( pOldBitmap );
}

void BMP_RENDER::_Render_ColorKey( CDC * pDC, int x, int y, int width, int height, CDC* srcDC, int sx, int sy, COLORREF crColour )
{
	CDC dcImage, dcTrans;

	// Create two memory dcs for the image and the mask
	dcImage.CreateCompatibleDC(pDC);
	dcTrans.CreateCompatibleDC(pDC);	

	// Create the mask bitmap
	CBitmap bitmapTrans;	
	int nWidth = width;
	int nHeight = height;
	bitmapTrans.CreateBitmap(nWidth, nHeight, 1, 1, NULL);

	// Select the mask bitmap into the appropriate dc
	CBitmap* pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);

	// Build mask based on transparent colour
	dcImage.SetBkColor(crColour);
	srcDC->SetBkColor(crColour);

	dcTrans.BitBlt(0, 0, nWidth, nHeight, srcDC, sx, sy, SRCCOPY);
	pDC->BitBlt(x, y, nWidth, nHeight, srcDC, sx, sy, SRCINVERT);
	pDC->BitBlt(x, y, nWidth, nHeight, &dcTrans, 0, 0, SRCAND);	
	pDC->BitBlt(x, y, nWidth, nHeight, srcDC, sx, sy, SRCINVERT);

	// Restore settings	
	dcTrans.SelectObject(pOldBitmapTrans);		
}