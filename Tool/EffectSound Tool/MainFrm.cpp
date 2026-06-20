// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CsGBDX9RendererPtr	g_pRenderer = NULL;
CMainFrame*			g_pMainWnd = NULL;

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
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
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.cx = 1280;
	cs.cy = 1024;
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
	if( !m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS(NiViewer), CSize(800, 650), pContext ) )
	{
		TRACE0("첫번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}	
	if( !m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS(_DlgEtc), CSize(1280-856, 650), pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}	
	if( !m_wndSplitter.CreateView( 1, 0, RUNTIME_CLASS(_DlgController), CSize(800, 1024-650), pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 1, 1, RUNTIME_CLASS(_DlgChar), CSize(1280-856, 1024-650), pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}
	
	g_pMainWnd = this;
	m_pNiViewer = (NiViewer*)m_wndSplitter.GetPane( 0, 0 );
	m_pDlgEtc = (_DlgEtc*)m_wndSplitter.GetPane( 0, 1 );	
	m_pDlgController = (_DlgController*)m_wndSplitter.GetPane( 1, 0 );
	m_pDlgChar = (_DlgChar*)m_wndSplitter.GetPane( 1, 1 );	

	__NiInit();

	// 사운드 매니져 메모리 할당
	g_pSoundMgr = new CSoundMgr;
	g_pSoundMgr->Init();

	CsMFC::Init();
	m_pNiViewer->Init();
	m_pDlgController->Init();
	m_pDlgChar->Init();
	m_pDlgEtc->Init();	

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnDestroy()
{	
	//m_pDlgEtc->_Delete();
	SAFE_NIDELETE( g_pSoundMgr );
	__NiDelete();

	CFrameWnd::OnDestroy();
}



void CMainFrame::__NiDelete()
{
	g_ViewEffect.Delete();

	g_pRenderer = 0;
	NiShutdown();
}

void CMainFrame::__NiInit()
{
#ifdef _DEBUG
	NiInit();
#else
	NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions( NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(), false) );
	NiInit(pkInitOptions);
#endif

	NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);	
	NiTexture::SetMipmapByDefault(false);
	NiSourceTexture::SetUseMipmapping(false);
	NiMaterial::SetDefaultWorkingDirectory(".\\Data\\Shaders\\Generated");	
	NiSourceTexture::SetDestroyAppDataFlag( false );

	g_pRenderer = CsGBDX9Renderer::CsGBCreate( 500, 600,
		NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL, m_pNiViewer->GetSafeHwnd(), m_pNiViewer->GetSafeHwnd(), 0,
		NiDX9Renderer::DEVDESC_PURE, NiDX9Renderer::FBFMT_UNKNOWN, NiDX9Renderer::DSFMT_D24S8, NiDX9Renderer::PRESENT_INTERVAL_ONE,
		NiDX9Renderer::SWAPEFFECT_COPY,	NiDX9Renderer::FBMODE_MULTISAMPLES_4, 1, NiDX9Renderer::REFRESHRATE_DEFAULT);

	nsCsGBTerrain::g_pRenderer = g_pRenderer;

	NiAlphaAccumulator* pkAccum = NiNew NiAlphaAccumulator;
	g_pRenderer->SetSorter( pkAccum );
	g_pRenderer->SetBackgroundColor( NiColor( 0.5f, 0.5f, 0.5f) );	
	g_ViewEffect.Init();
}

void CMainFrame::_Update()
{
	//Sleep( 10 );

	if( CAMERA_STPTR )
		CAMERA_ST.Update_ByTool( 0.01f, false );

	g_ViewEffect.Update();	
	m_pNiViewer->Update();	
}

void CMainFrame::_Render()
{
	g_ViewEffect.Render();
}

