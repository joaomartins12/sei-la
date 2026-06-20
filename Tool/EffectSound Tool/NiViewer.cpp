
#include "stdafx.h"
#include "NiViewer.h"


IMPLEMENT_DYNCREATE(NiViewer, CView)

NiViewer::NiViewer()
{

}
void NiViewer::OnDraw(CDC* /*pDC*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


void NiViewer::Update()
{
	POINT pt;
	GetCursorPos( &pt );
	ScreenToClient( &pt );

	CRect rc;
	GetClientRect( &rc );

	if( PtInRect( &rc, pt ) == TRUE )
		SetFocus();
}

BEGIN_MESSAGE_MAP(NiViewer, CView)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
END_MESSAGE_MAP()

void NiViewer::OnRButtonDown(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnRButtonDown( point );
	SetCapture();

	CView::OnRButtonDown(nFlags, point);
}

void NiViewer::OnRButtonUp(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnRButtonUp( point );
	ReleaseCapture();

	CView::OnRButtonUp(nFlags, point);
}

void NiViewer::OnMouseMove(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnMouseMove( point );

	CView::OnMouseMove(nFlags, point);
}

BOOL NiViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CAMERA_ST.OnMouseWheel( zDelta );

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void NiViewer::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if( g_pRenderer )
	{
		g_pRenderer->Recreate( cx, cy );

		CAMERA_ST.ChangeStandardFov( (float)cx, (float)cy );
	}
}

void NiViewer::OnMButtonDown(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnMButtonDown( point );

	SetCapture();

	CView::OnMButtonDown(nFlags, point);
}

void NiViewer::OnMButtonUp(UINT nFlags, CPoint point)
{
	CAMERA_ST.OnMButtonUp( point );

	ReleaseCapture();

	CView::OnMButtonUp(nFlags, point);
}
