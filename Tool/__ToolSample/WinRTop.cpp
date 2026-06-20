// WinRTop.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "About.h"
#include "WinRTop.h"


WinRTop*		g_pWinRTop = NULL;

// WinRTop

IMPLEMENT_DYNCREATE(WinRTop, CFormView)

WinRTop::WinRTop()
	: CFormView(WinRTop::IDD)
{

}

WinRTop::~WinRTop()
{
}

void WinRTop::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(WinRTop, CFormView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// WinRTop 진단입니다.

#ifdef _DEBUG
void WinRTop::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void WinRTop::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// WinRTop 메시지 처리기입니다.

void WinRTop::OnSize(UINT nType, int cx, int cy)
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
