// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "MainFrm.h"

CsGBDX9RendererPtr	g_pRenderer = NULL;
CMainFrame*			g_pMainWnd = NULL;

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// 툴 해상도
#define SCREENWIDTH		1280

#define LEFT		500
#define RIGHT		SCREENWIDTH - LEFT

#define SCREENHEIGHT	1024

#define TOP			600
#define BOTTOM		SCREENWIDTH - TOP

#define SECTION "SEQUENCETOOL"

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
#ifdef FIX_TOOL_RESIZE
	_SaveWindowRect();
#endif
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	// 디폴트
	cs.x = 0;
	cs.y = 0;
	cs.cx = 1280;
	cs.cy = 1024;

	
#ifdef FIX_TOOL_RESIZE
	Csini x( ".\\__ToolLocal.ini" );
	int nLoadUse = x.GetInt(SECTION, "LoadUse");

	if( nLoadUse == 1)
	{
		int nLeft = x.GetInt(SECTION, "LEFT");
		int nRight = x.GetInt(SECTION, "RIGHT");	
		int nTop = x.GetInt(SECTION, "TOP");
		int nBottom = x.GetInt(SECTION, "BOTTOM");

		cs.x = x.GetInt(SECTION, "PosX");
		cs.y = x.GetInt(SECTION, "PosY");

		cs.cx = nLeft + nRight;
		cs.cy = nTop + nBottom;		

		
	}
#endif
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


	Csini x( ".\\__ToolLocal.ini" );
	int nLoadUse = x.GetInt(SECTION, "LoadUse");

	// 디폴트 셋팅
	CSize SizeLT(LEFT,TOP);
	CSize SizeRT(RIGHT,TOP);
	CSize SizeLB(LEFT,BOTTOM);
	CSize SizeRB(RIGHT,BOTTOM);

#ifdef FIX_TOOL_RESIZE
	if( nLoadUse < 0 )	// 해상도 설정 초기 실행,
	{
		CsMessageBoxA_Title(MB_OK, "초기 실행 설정", "기본 해상도 환경으로 설정되었습니다. 종료 시 해상도 환경이 저장됩니다.");
		x.WriteInt(SECTION, "SaveUse", 1);
		x.WriteInt(SECTION, "LoadUse", 1);
	}

	else if( nLoadUse == 1 )
	{
		int nLeft = x.GetInt(SECTION, "LEFT");
		int nRight = x.GetInt(SECTION, "RIGHT");
		int nTop = x.GetInt(SECTION, "TOP");
		int nBottom = x.GetInt(SECTION, "BOTTOM");

		if(nLeft < 0 || nRight < 0 || nTop < 0 || nBottom < 0)
		{
			CsMessageBoxA_Title(MB_OK, "__ToolLocal.ini Error", "ini 설정이 잘못되었습니다. 기본 해상도가 설정되었습니다.");
		}
		else
		{
			SizeLT.SetSize(nLeft, nTop);
			SizeRT.SetSize(nRight, nTop);
			SizeLB.SetSize(nLeft, nBottom);
			SizeRB.SetSize(nRight, nBottom);
		}
	}
	
#endif

	if( !m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS(NiViewer), SizeLT, pContext ) )
	{
		TRACE0("첫번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS(_DlgAnimation), SizeRT, pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 1, 0, RUNTIME_CLASS(_DlgEtc), SizeLB, pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 1, 1, RUNTIME_CLASS(_DlgController), SizeRB, pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}

	g_pMainWnd = this;
	m_pNiViewer = (NiViewer*)m_wndSplitter.GetPane( 0, 0 );
	m_pDlgAnimation = (_DlgAnimation*)m_wndSplitter.GetPane( 0, 1 );
	m_pDlgEtc = (_DlgEtc*)m_wndSplitter.GetPane( 1, 0 );
	m_pDlgController = (_DlgController*)m_wndSplitter.GetPane( 1, 1 );

	__NiInit();

	// 사운드 매니져 메모리 할당
	g_pSoundMgr = new CSoundMgr;
	g_pSoundMgr->Init();

	CsMFC::Init();
	m_pNiViewer->_Init();
	m_pDlgController->_Init();
	m_pDlgAnimation->_Init();
	m_pDlgEtc->_Init();

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnDestroy()
{
	m_pDlgAnimation->_Delete();
	m_pDlgEtc->_Delete();
	SAFE_NIDELETE( g_pSoundMgr );
	__NiDelete();

#ifdef FIX_TOOL_RESIZE
	Csini x( ".\\__ToolLocal.ini" );
	int nSaveUse = x.GetInt(SECTION, "SaveUse");
	
	// 저장 옵션 사용
	if( !IsIconic() && nSaveUse == 1 )		// 최소화 상태이면 저장하지 않음.
	{
		_SaveWindowRect();
	}
#endif
	CFrameWnd::OnDestroy();
}



void CMainFrame::__NiDelete()
{
	g_Sorting.Delete();
	g_ViewModel.Delete();
	
	g_pRenderer = 0;
	NiShutdown();

	const NiInitOptions* pkInitOptions = NiStaticDataManager::GetInitOptions();
	NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
	NiExternalDelete pkInitOptions;
	NiExternalDelete pkAllocator;
}

void CMainFrame::__NiInit()
{
	NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(
#ifdef NI_MEMORY_DEBUGGER
		NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(),	false)
#else
		NiExternalNew NiStandardAllocator()
#endif
		);
	NiInit(pkInitOptions);

	char* pcWorkingDir = "Data\\Shaders\\Generated";
	NiMaterial::SetDefaultWorkingDirectory(pcWorkingDir);
	NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);
	NiTexture::SetMipmapByDefault(true);
	NiSourceTexture::SetUseMipmapping(true);

	Csini x( ".\\__ToolLocal.ini" );
	int nLoadUse = x.GetInt(SECTION, "LoadUse");

	int nWidth = 500;
	int nHeight = 600;
	
#ifdef FIX_TOOL_RESIZE
	if( x.GetInt(SECTION, "LoadUse") == 1 )		// 환경 로드 옵션 사용 시
	{
		if( x.GetInt(SECTION, "LEFT") > 0 || x.GetInt(SECTION, "TOP") > 0)
		{
			nWidth = x.GetInt(SECTION, "LEFT");
			nHeight = x.GetInt(SECTION, "TOP");
		}
	}
#endif

	g_pRenderer = CsGBDX9Renderer::CsGBCreate( nWidth, nHeight,
		NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL, m_pNiViewer->GetSafeHwnd(), m_pNiViewer->GetSafeHwnd(), 0,
		NiDX9Renderer::DEVDESC_PURE, NiDX9Renderer::FBFMT_UNKNOWN, NiDX9Renderer::DSFMT_D24S8, NiDX9Renderer::PRESENT_INTERVAL_ONE,
		NiDX9Renderer::SWAPEFFECT_COPY,	NiDX9Renderer::FBMODE_MULTISAMPLES_4, 1, NiDX9Renderer::REFRESHRATE_DEFAULT);

	nsCsGBTerrain::g_pRenderer = g_pRenderer;
	g_pRenderer->SetBackgroundColor( NiColor( 0.5f, 0.5f, 0.5f) );
	g_pRenderer->SetSorter( NiNew NiAlphaAccumulator );
	g_ViewModel.Init();	
}

void CMainFrame::_Update()
{
	Sleep( 10 );

	g_ViewModel.Update();

	m_pDlgEtc->_Update();
	m_pDlgController->_Update();
	m_pNiViewer->_Update();
	m_pDlgAnimation->_Update();	
}

void CMainFrame::_Render()
{
	g_ViewModel.Render();
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYUP )
	{
		if( pMsg->wParam == VK_SPACE )
		{
			m_pDlgAnimation->OnBnClickedButtonNewEvent();
			return TRUE;
		}
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}

#ifdef FIX_TOOL_RESIZE
void CMainFrame::_SaveWindowRect()
{
// 현재 윈도우 영역 가져옴
	Csini x( ".\\__ToolLocal.ini" );

	RECT rcWin;
	this->GetWindowRect(&rcWin);
	if( IsZoomed() )	// 최대화 시 프레임 보정
	{
 		OffsetRect(&rcWin, +8, +8);
	}

	// 좌상단 뷰어 Width 저장
	int nCurrentCol, nMinCol;
	m_wndSplitter.GetColumnInfo(0, nCurrentCol, nMinCol);		
	// rcWin.right - rcWin.left 는 가로 총 사이즈
	x.WriteInt(SECTION, "LEFT", nCurrentCol );

	// 좌상단 뷰어 Height 저장
	int nCurrentRow, nMinRow;
	m_wndSplitter.GetRowInfo(0, nCurrentRow, nMinRow);			
	x.WriteInt(SECTION, "TOP", nCurrentRow);

	// 툴 윈도우 생성 시 0행,1열 / 1행,1열의 윈도우는 0행,0열/0행,1열의 윈도우 크기를 사용한 나머지 영역을 사용하게됨
	x.WriteInt(SECTION, "RIGHT", rcWin.right - rcWin.left - nCurrentCol);
	x.WriteInt(SECTION, "BOTTOM", rcWin.bottom - rcWin.top - nCurrentRow);

	x.WriteInt(SECTION, "PosX", rcWin.left);
	x.WriteInt(SECTION, "PosY", rcWin.top);

	
}
#endif