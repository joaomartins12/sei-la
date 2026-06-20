
#include "stdafx.h"
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



CMainFrame*			g_pMainWnd = NULL;
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
	// 그렇지 않으면 기본 처리합니다.
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if( !m_wndSplitter.CreateStatic( this, 2, 2 ) )
	{		
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS(WinTop), CSize(VIEWER_WIDTH, TOTAL_HEIGHT-VIEWER_HEIGHT), pContext ) )
	{
		TRACE0("첫번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS(WinTopMenu), CSize(TOTAL_WIDTH-VIEWER_WIDTH, TOTAL_HEIGHT-VIEWER_HEIGHT), pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 1, 0, RUNTIME_CLASS(WinViewer), CSize(VIEWER_WIDTH, VIEWER_HEIGHT), pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 1, 1, RUNTIME_CLASS(WinDiscript), CSize(TOTAL_WIDTH-VIEWER_WIDTH, VIEWER_HEIGHT), pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}

	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapResurrection = false;
	nsCsFileTable::g_bReadInactiveQuest = true;
	
	Csini x( ".\\__ToolLocal.ini" );	
	nsCsFileTable::eLANGUAGE lan;
	char* cLan = x.GetStr( "LANGUAGE", "tool" );

	//* 각 국가 버전 별로 define이 다르므로 국가별로 버전을 나눕니다. *//

#ifdef VERSION_USA	//GSP 버전은 고정으로 영문버전
	lan = nsCsFileTable::ENGLISH;	CsMessageBox( MB_OK, L"이것은 영문 버젼이에요.~!" );

#elif VERSION_TW	// 대만
	lan = nsCsFileTable::TAIWAN;	CsMessageBox( MB_OK, L"이것은 대만 버전이에요~!" );	

#elif VERSION_HK	// 홍콩
	lan = nsCsFileTable::HONGKONG;	CsMessageBox( MB_OK, L"이것은 홍콩 버전이에요~!" );		

#elif VERSION_TH
	lan = nsCsFileTable::THAILAND;	CsMessageBox( MB_OK, L"สวัสดีครับ ยินดีที่ได้รู้จักครับ~!" );		

#else

	lan = nsCsFileTable::KOREA_TRANSLATION;	CsMessageBox( MB_OK, L"이것은 한국 버전이에요~!" );		
#endif

	nsCsFileTable::g_bUseQuest = true;

	nsCsFileTable::g_bUseTotalExcel_None = false;
	nsCsFileTable::g_bUseBase = false;
	nsCsFileTable::g_bUseSkill = false;
	nsCsFileTable::g_bUseNature = false;
	nsCsFileTable::g_bUseCuid = false;
	nsCsFileTable::g_bUseEvent = false;
	nsCsFileTable::g_bAddExp = false;
	nsCsFileTable::g_bUseCashShop = false;
	nsCsFileTable::g_bSceneDataMng = false;
	nsCsFileTable::g_bBuffMng = false;
	nsCsFileTable::g_bUseGotcha = false;
	nsCsFileTable::g_bUseHelp = false;
	nsCsFileTable::g_bUseMoveObject = false;
	nsCsFileTable::g_bUseReward = false;
	nsCsFileTable::g_bUseMaster_CardMng = false;

	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, lan );	

	CsMFC::Init();

	g_pMainWnd = this;

	g_pViewer = (WinViewer*)m_wndSplitter.GetPane( 1, 0 );
	g_pViewer->_Init();

	g_pTop = (WinTop*)m_wndSplitter.GetPane( 0, 0 );
	g_pTop->_Init();

	g_pDiscript = (WinDiscript*)m_wndSplitter.GetPane( 1, 1 );
	g_pDiscript->_Init();

	g_pTopMenu = (WinTopMenu*)m_wndSplitter.GetPane( 0, 1 );
	g_pTopMenu->_Init();

	g_pViewer->SetFocus();

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnDestroy()
{
	nsCsFileTable::g_FileTableMng.Delete();

	g_pViewer->_Delete();

	CFrameWnd::OnDestroy();
}

void CMainFrame::_Update()
{
	static float fOldTimeInSec = NiGetCurrentTimeInSec();
	float fCurTimeInSec = NiGetCurrentTimeInSec();
	g_fDeltaTime = fCurTimeInSec - fOldTimeInSec;
	fOldTimeInSec = fCurTimeInSec;

	g_pViewer->_Update();
}

void CMainFrame::_Render()
{
	g_pViewer->_Render();
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if( IsCTRLPressed() == true )
		{
			switch( pMsg->wParam )
			{
			case 'F':
				{
					g_pTop->OnBnClickedButtonFindwin();
				}				
				return TRUE;
			}
		}
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}
