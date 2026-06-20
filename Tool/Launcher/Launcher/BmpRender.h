#pragma once

namespace BMP_RENDER
{
	// 외부함수
	void Render_Bmp( CDC * pDC, int nX, int nY, CBitmap* pBitmap, bool bAlpa );
	void Render_ColorKey( CDC* pDC, CDC* pTempDC, int nX, int nY, CBitmap* pBitmap );

	// 내부함수
	void _Render_ColorKey( CDC * pDC, int x, int y, int width, int height, CDC* srcDC, int sx, int sy, COLORREF crColour );
};


