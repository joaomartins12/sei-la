// WinLTop.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "About.h"
#include "WinLTop.h"


WinLTop*		g_pWinLTop = NULL;

// WinLTop

IMPLEMENT_DYNCREATE(WinLTop, CFormView)

WinLTop::WinLTop()
	: CFormView(WinLTop::IDD)
{

}

WinLTop::~WinLTop()
{
}

void WinLTop::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(WinLTop, CFormView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// WinLTop 진단입니다.

#ifdef _DEBUG
void WinLTop::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void WinLTop::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// WinLTop 메시지 처리기입니다.

void WinLTop::OnSize(UINT nType, int cx, int cy)
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
