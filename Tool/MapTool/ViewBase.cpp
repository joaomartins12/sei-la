// ChildView.cpp : CViewBase 클래스의 구현
//

#include "stdafx.h"
#include "ViewBase.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// CViewBase

CViewBase::CViewBase()
{
	m_ViewID	=	0;
}

CViewBase::~CViewBase()
{
}


BEGIN_MESSAGE_MAP(CViewBase, CWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_NCMOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CViewBase 메시지 처리기

BOOL CViewBase::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

CViewBase* CViewBase::_GetInstance( DWORD ID )
{
	CViewBase* pView = NULL;
	switch( ID )
	{
	case nsMAPTOOL::MV_VIEWER:			pView	=	csnew CViewViewer;			break;
	case nsMAPTOOL::MV_MAINTAB:			pView	=	csnew CViewMainTab;			break;
	case nsMAPTOOL::MV_NODE:			pView	=	csnew CViewNode;			break;
	case nsMAPTOOL::MV_SETUP:			pView	=	csnew CViewSetUp;			break;
	case nsMAPTOOL::MV_OBJECT_CONTROL:	pView	=	csnew CViewObjectControl;	break;
	case nsMAPTOOL::MV_OBJECT_LIST:		pView	=	csnew CViewObjectList;		break;
	default:							assert_cs( false );
	}

	pView->m_ViewID	= ID;

	return pView;
}

//==============================================================
//
// Mouse
//
//==============================================================
void CViewBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	_OnLButtonDown( point );
	CWnd::OnLButtonDown(nFlags, point);
}

void CViewBase::OnLButtonUp(UINT nFlags, CPoint point)
{
	_OnLButtonUp( point );
	ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
}

void CViewBase::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	_OnRButtonDown( point );
	CWnd::OnRButtonDown(nFlags, point);
}

void CViewBase::OnRButtonUp(UINT nFlags, CPoint point)
{
	_OnRButtonUp( point );
	ReleaseCapture();
	CWnd::OnRButtonUp(nFlags, point);
}

void CViewBase::OnMouseMove(UINT nFlags, CPoint point)
{
	_OnMouseMove( point );
	CWnd::OnMouseMove(nFlags, point);
}


BOOL CViewBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	_OnMouseWheel( zDelta );
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CViewBase::PostNcDestroy()
{

	CWnd::PostNcDestroy();
}

void CViewBase::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	_OnMButtonDown( point );
	CWnd::OnMButtonDown(nFlags, point);
}

void CViewBase::OnMButtonUp(UINT nFlags, CPoint point)
{
	_OnMButtonUp( point );
	ReleaseCapture();
	CWnd::OnMButtonUp(nFlags, point);
}

void CViewBase::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//SetCapture();
	_OnLButtonDBCLick( point );
	CWnd::OnLButtonDblClk(nFlags, point);
}
