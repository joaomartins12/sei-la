// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CsGBDX9RendererPtr	g_pRenderer = NULL;
CMainFrame*			g_pMainWnd = NULL;
float				g_fDeltaTime = 0.0f;
int					g_nScreenHeight = 0;
int					g_nScreenWidth = 0;

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
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

	cs.cx = 1680;
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
	if( !m_wndSplitter.CreateStatic( this, 1, 2 ) )
	{		
		return FALSE;
	}
	if( !m_wndSplitter.CreateView( 0, 0, RUNTIME_CLASS(NiViewer), CSize(856, 1024), pContext ) )
	{
		TRACE0("첫번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}	
	if( !m_wndSplitter.CreateView( 0, 1, RUNTIME_CLASS(_DlgController), CSize(1680-856, 1024), pContext ) )
	{
		TRACE0("두번째 스플래터 윈도우를 생성하지 못했습니다.\n");
		return FALSE;
	}

	g_pMainWnd = this;
	m_pNiViewer = (NiViewer*)m_wndSplitter.GetPane( 0, 0 );
	m_pDlgController = (_DlgController*)m_wndSplitter.GetPane( 0, 1 );	

	__NiInit();

	CsMFC::Init();
	m_pNiViewer->_Init();	
	m_pDlgController->_Init();

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnDestroy()
{	
	__NiDelete();

	CFrameWnd::OnDestroy();
}

void CMainFrame::__NiDelete()
{	
	SAFE_NIDELETE( g_pSoundMgr );

	CsGBPick::Delete();	
	g_Camera.Delete();				
	g_Sorting.Delete();		
	g_EventScene.Delete();
	_GetDlgController()->_Delete();				

	nsCsFileTable::g_FileTableMng.Delete();				

	THREAD_MEMPOOL_DELETE( CNpc );
	THREAD_MEMPOOL_DELETE( CsC_EventObject );	
	THREAD_MEMPOOL_DELETE( CsC_EventTamerObject );

	CsC_Thread::GlobalShotDown();	

	g_Terrain.Delete();	
	CsGBTerrainMng::DeleteMempool();

	g_pRenderer = 0;	
	NiShutdown();
}

bool CMainFrame::CALLBACK_ThreadDelete( sTCUnit* pUnit )
{
	CsC_Object* pObject = pUnit->s_pLoadedObject;
	pObject->Delete();

	switch( pObject->GetLeafRTTI() )
	{	
	case RTTI_NPC:
		CNpc::DeleteInstance( (CNpc*)pObject );
		return true;			
	//case RTTI_EVENT:
	//	CsC_EventObject::DeleteInstance( (CsC_EventObject*)pObject );
	//	return true;			
	default:
		assert_cs( false );
	}
	return false;
}

void CMainFrame::__NiInit()
{
#ifdef _DEBUG
	NiInit();
#else
	NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions( NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(), false) );
	NiInit(pkInitOptions);
#endif	

	g_pRenderer = CsGBDX9Renderer::CsGBCreate( 500, 600,
		NiDX9Renderer::USE_MULTITHREADED | NiDX9Renderer::USE_STENCIL, m_pNiViewer->GetSafeHwnd(), m_pNiViewer->GetSafeHwnd(), 0,
		NiDX9Renderer::DEVDESC_PURE, NiDX9Renderer::FBFMT_UNKNOWN, NiDX9Renderer::DSFMT_D24S8, NiDX9Renderer::PRESENT_INTERVAL_ONE,
		NiDX9Renderer::SWAPEFFECT_COPY,	NiDX9Renderer::FBMODE_DEFAULT, 1, NiDX9Renderer::REFRESHRATE_DEFAULT);	

	g_pRenderer->SetBackgroundColor( NiColor( 0.5f, 0.5f, 0.5f) );	

	// Pick	
	CsGBPick::Create( NiPick::FIND_ALL, NiPick::WORLD_COORDINATES, true, false );

	//쓰레드 등록
	CsC_Thread::GlobalInit();
	g_pThread->ResistCallBack_DeleteUnit( CALLBACK_ThreadDelete );	
	g_pThread->UseThread( false );

	//파일테이블
	nsCsMapTable::g_bUseMapStart = false;	
	nsCsMapTable::g_bUseMapList = true;
		
#ifdef _DEBUG
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_BIN, nsCsFileTable::KOREA_TRANSLATION );
#else
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::KOREA_TRANSLATION );	
#endif

	// 사운드 매니져 메모리 할당
	g_pSoundMgr = new CSoundMgr;
	g_pSoundMgr->Init();

	//지형 설정
	g_Terrain.Init();

	//씬 설정
	g_EventScene.Init();		
}
 
void CMainFrame::_Update()
{
	Sleep( 10 );

	static float fOldTimeInSec = NiGetCurrentTimeInSec();
	float fCurTimeInSec = NiGetCurrentTimeInSec();	

	g_fDeltaTime = fCurTimeInSec - fOldTimeInSec;
	fOldTimeInSec = fCurTimeInSec;

	g_fAccumTime += g_fDeltaTime;
	
	m_pNiViewer->_Update();		
	m_pDlgController->_Update();
	g_EventScene.Update();
	g_Terrain.Update();
	
}

void CMainFrame::_Render()
{	
	g_pRenderer->BeginFrame();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
	g_pRenderer->SetCameraData( g_Camera.GetCameraObj() );	

	g_Terrain.Render();

	g_Sorting.Render( g_Camera.GetCameraObj() );
	m_pDlgController->_Render();
	g_EventScene.Render();
	g_Sorting.Render( g_Camera.GetCameraObj() );

	g_pRenderer->SetScreenSpaceCameraData();

	//말풍선
	g_pTalkBallone->Render();			

	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->EndFrame();
	g_pRenderer->DisplayFrame();	
}


void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.		

	CFrameWnd::OnClose();
}
