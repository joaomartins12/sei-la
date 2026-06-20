// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "About.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

NiDX9RendererPtr	g_pRenderer = NULL;
CMainFrame*			g_pMainWnd = NULL;
CFormView*			g_pProc3DWin = NULL;
float				g_fDeltaTime = 0.0f;


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
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//// 프레임의 클라이언트 영역을 차지하는 뷰를 만듭니다.
	//if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
	//	CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	//{
	//	TRACE0("뷰 창을 만들지 못했습니다.\n");
	//	return -1;
	//}

	//if (!m_wndStatusBar.Create(this) ||
	//	!m_wndStatusBar.SetIndicators(indicators,
	//	  sizeof(indicators)/sizeof(UINT)))
	//{
	//	TRACE0("상태 표시줄을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.cx = TOTAL_WIDTH;
	cs.cy = TOTAL_HEIGHT;

	/*cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_SYSMENU
	| WS_MINIMIZEBOX | WS_MAXIMIZEBOX;*/

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 뷰 창으로 포커스를 이동합니다.
	//m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 뷰에서 첫째 크랙이 해당 명령에 나타나도록 합니다.
	/*if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;*/

	// 그렇지 않으면 기본 처리합니다.
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	cEngine::GlobalShutDown();
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if( !m_wndSplitter.CreateStatic( this, 2, 2 ) )
	{		
		return FALSE;
	}

	CSize size00;
	CSize size01;
	CSize size10;
	CSize size11;

	if( PROC_3D_PANE_X == 0 )
	{
		size00.cx = PROC_3D_WIN_WIDTH;
		size01.cx = TOTAL_WIDTH - PROC_3D_WIN_WIDTH;
		size10.cx = PROC_3D_WIN_WIDTH;
		size11.cx = TOTAL_WIDTH - PROC_3D_WIN_WIDTH;
	}
	else
	{
		assert_cs( PROC_3D_PANE_X == 1 );
		size00.cx = TOTAL_WIDTH - PROC_3D_WIN_WIDTH;
		size01.cx = PROC_3D_WIN_WIDTH;
		size10.cx = TOTAL_WIDTH - PROC_3D_WIN_WIDTH;
		size11.cx = PROC_3D_WIN_WIDTH;
	}

	if( PROC_3D_PANE_Y == 0 )
	{
		size00.cy = PROC_3D_WIN_HEIGHT;
		size01.cy = PROC_3D_WIN_HEIGHT;
		size10.cy = TOTAL_HEIGHT - PROC_3D_WIN_HEIGHT;
		size11.cy = TOTAL_HEIGHT - PROC_3D_WIN_HEIGHT;
	}
	else
	{
		assert_cs( PROC_3D_PANE_Y == 1 );
		size00.cy = TOTAL_HEIGHT - PROC_3D_WIN_HEIGHT;
		size01.cy = TOTAL_HEIGHT - PROC_3D_WIN_HEIGHT;
		size10.cy = PROC_3D_WIN_HEIGHT;
		size11.cy = PROC_3D_WIN_HEIGHT;
	}

	if( !m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS(WinLTop), size00, pContext ) )
	{
		TRACE0("첫번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS(WinRTop), size01, pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 1, 0, RUNTIME_CLASS(WinLBottom), size10, pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 1, 1, RUNTIME_CLASS(WinRBottom), size11, pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}

	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapResurrection = false;
#ifdef _DEBUG
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_BIN, nsCsFileTable::KOREA_TRANSLATION );
#else
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::KOREA_TRANSLATION );	
#endif

	CsMFC::Init();

	g_pMainWnd = this;

	g_pProc3DWin = (CFormView*)m_wndSplitter.GetPane( PROC_3D_PANE_Y, PROC_3D_PANE_X );
	cEngine::GlobalInit( g_pProc3DWin->GetSafeHwnd() );

	g_pWinLTop = (WinLTop*)m_wndSplitter.GetPane( 0, 0 );
	g_pWinLTop->_Init();

	g_pWinRTop = (WinRTop*)m_wndSplitter.GetPane( 0, 1 );
	g_pWinRTop->_Init();

	g_pWinLBottom = (WinLBottom*)m_wndSplitter.GetPane( 1, 0 );
	g_pWinLBottom->_Init();

	g_pWinRBottom = (WinRBottom*)m_wndSplitter.GetPane( 1, 1 );
	g_pWinRBottom->_Init();

	g_pProc3DWin->SetFocus();
	
	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::_Update()
{
	static float fOldTimeInSec = NiGetCurrentTimeInSec();
	float fCurTimeInSec = NiGetCurrentTimeInSec();
	g_fDeltaTime = fCurTimeInSec - fOldTimeInSec;
	fOldTimeInSec = fCurTimeInSec;

	g_pWinLTop->_Update();
	g_pWinRTop->_Update();
	g_pWinLBottom->_Update();
	g_pWinRBottom->_Update();
	g_pEngine->Update();
}

void CMainFrame::_Render()
{
	g_pWinLTop->_Render();
	g_pWinRTop->_Render();
	g_pWinLBottom->_Render();
	g_pWinRBottom->_Render();
	g_pEngine->Render();
}
