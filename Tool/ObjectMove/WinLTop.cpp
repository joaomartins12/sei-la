// WinLTop.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ObjectMove.h"
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
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
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

void WinLTop::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	g_pProc3D->GetMap()->GetMoveMng()->OnLButtonDown( CsPoint( point.x, point.y ) );

	CAMERA_ST.OnLButtonDown( point );
	CFormView::OnLButtonDown(nFlags, point);
}

void WinLTop::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	CAMERA_ST.OnLButtonUp( point );

	CFormView::OnLButtonUp(nFlags, point);
}

void WinLTop::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	CAMERA_ST.OnMButtonDown( point );

	CFormView::OnMButtonDown(nFlags, point);
}

void WinLTop::OnMButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	CAMERA_ST.OnMButtonUp( point );


	CFormView::OnMButtonUp(nFlags, point);
}

void WinLTop::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	CAMERA_ST.OnRButtonDown( point );

	CFormView::OnRButtonDown(nFlags, point);
}

void WinLTop::OnRButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	CAMERA_ST.OnRButtonUp( point );

	CFormView::OnRButtonUp(nFlags, point);
}

void WinLTop::OnMouseMove(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnMouseMove( point );

	CFormView::OnMouseMove(nFlags, point);
}

BOOL WinLTop::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	g_pProc3D->GetMap()->SetCameraAccel();
	CAMERA_ST.OnMouseWheel( zDelta );

	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}
