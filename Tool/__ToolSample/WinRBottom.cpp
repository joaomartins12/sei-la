// WinRBottom.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "About.h"
#include "WinRBottom.h"



WinRBottom*		g_pWinRBottom = NULL;


// WinRBottom

IMPLEMENT_DYNCREATE(WinRBottom, CFormView)

WinRBottom::WinRBottom()
	: CFormView(WinRBottom::IDD)
{

}

WinRBottom::~WinRBottom()
{
}

void WinRBottom::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(WinRBottom, CFormView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// WinRBottom 진단입니다.

#ifdef _DEBUG
void WinRBottom::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void WinRBottom::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// WinRBottom 메시지 처리기입니다.

void WinRBottom::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if( this == g_pProc3DWin )
	{
		g_ptScreen = CsPoint( cx, cy );
		if( g_pRenderer )
		{		
			g_pRenderer->Recreate( cx, cy );		
		}
	}
}
