
// WinLBottom.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "About.h"
#include "WinLBottom.h"


WinLBottom*		g_pWinLBottom = NULL;

// WinLBottom

IMPLEMENT_DYNCREATE(WinLBottom, CFormView)

WinLBottom::WinLBottom()
	: CFormView(WinLBottom::IDD)
{

}

WinLBottom::~WinLBottom()
{
}

void WinLBottom::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(WinLBottom, CFormView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// WinLBottom 진단입니다.

#ifdef _DEBUG
void WinLBottom::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void WinLBottom::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// WinLBottom 메시지 처리기입니다.

void WinLBottom::OnSize(UINT nType, int cx, int cy)
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
