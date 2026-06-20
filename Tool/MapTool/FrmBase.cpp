// ChildFrm.cpp : CFrameBase 클래스의 구현
//
#include "stdafx.h"
#include "FrmBase.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

// CFrameBase 생성/소멸

CFrameBase::CFrameBase()
{
	m_pView		=	NULL;
}

CFrameBase::~CFrameBase()
{
}

BOOL CFrameBase::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서 Window 클래스 또는 스타일을 수정합니다.
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	//cs.style &= ~ WS_THICKFRAME;
		cs.style &= ~ WS_SYSMENU;
		//cs.style &= ~ WS_MAXIMIZE;
		//cs.style &= ~ WS_MAXIMIZEBOX;
		//cs.style &= ~ WS_MINIMIZE;
		//cs.style &= ~ WS_MINIMIZEBOX;
		//cs.style &= ~ WS_SIZEBOX;
		cs.dwExStyle &= ~ WS_EX_CLIENTEDGE;	
		cs.dwExStyle |= WS_EX_TOOLWINDOW;
		cs.lpszClass = AfxRegisterWndClass(0);
	
		return TRUE;
}

// CFrameBase 진단

#ifdef _DEBUG
void CFrameBase::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CFrameBase::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


void CFrameBase::_Delete()
{
	m_pView->_Delete();
	SAFE_DELETE( m_pView );
}

void CFrameBase::_Init( DWORD ID, CRect rcView )
{
	// ID 연결
	m_ID	=	ID;

	// Child 생성
	m_pView	=	CViewBase::_GetInstance( ID );
	// 프레임의 클라이언트 영역을 차지하는 뷰를 만듭니다.
	m_pView->Create( NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect( 0, 0, rcView.Width(), rcView.Height() ), this, AFX_IDW_PANE_FIRST, NULL );

	m_pView->_Init();
	theApp._SortWindow( m_ID );	
}

IMPLEMENT_DYNCREATE(CFrameBase, CMDIChildWnd)
//CObject로 부터 상속받은 클래스에 동적생성 기능을 적용시켜준다.
//이 매크로를 추가하면 CRuntimeClass의 CreateObject() 함수를 통해 클래스를 동적으로 생성할 수 있게 된다.



BEGIN_MESSAGE_MAP(CFrameBase, CMDIChildWnd)	
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void CFrameBase::OnDestroy()
{
	_Delete();
	CMDIChildWnd::OnDestroy();
}



void CFrameBase::OnSetFocus(CWnd* pOldWnd)
{
	CMDIChildWnd::OnSetFocus(pOldWnd);

	theApp._SortWindow( m_ID );
}
