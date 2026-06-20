// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "MapTool.h"

#include "MainFrm.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_KILLFOCUS()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowPos( NULL, 0, 0, DEFAULT_TOOLWINDOW_WIDTH, DEFAULT_TOOLWINDOW_HEIGHT, 0 );
		
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	/*m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);*/

	theApp._Init();
	
	return 0;
}

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
		// TODO: CREATESTRUCT cs를 수정하여 여기에서
			//  Window 클래스 또는 스타일을 수정합니다.
		
		//cs.style &= ~ WS_MAXIMIZE;
		//cs.style &= ~ WS_MAXIMIZEBOX;
		cs.style &= ~ WS_SIZEBOX;
		cs.lpszClass = AfxRegisterWndClass(0);
	
		return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::OnMove(int x, int y)
{
	CMDIFrameWnd::OnMove(x, y);
}

void CMainFrame::OnKillFocus(CWnd* pNewWnd)
{
	CMDIFrameWnd::OnKillFocus(pNewWnd);
}

