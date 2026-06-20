// MapTool.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "afxwin.h"
#include "cDumper.hpp"




//#ifdef _DEBUG
//#define new12 DEBUG_NEW
//#endif


// CMapToolApp
BEGIN_MESSAGE_MAP(CMapToolApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMapToolApp::OnAppAbout)
	ON_COMMAND(ID_CAMERA_RESET, &CMapToolApp::OnCameraReset)
	ON_COMMAND(ID_VIEW_GRIDLINETOGGLE, &CMapToolApp::OnViewGridlinetoggle)
	ON_COMMAND(ID_VIEW_PATHOBJTOGGLE, &CMapToolApp::ShowPathObjectToggle)
	ON_COMMAND(ID_CONFIG, &CMapToolApp::OnConfig)
	ON_COMMAND(ID_FILE_LOADAS, &CMapToolApp::OnFileLoadas)
	ON_COMMAND(ID_FILE_SAVEAS, &CMapToolApp::OnFileSaveas)
END_MESSAGE_MAP()


// CMapToolApp 생성

CMapToolApp::CMapToolApp()
:ShowPathObj(false)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CMapToolApp 개체입니다.

CMapToolApp theApp;

bool		g_bOnServerComputer = false;


// CMapToolApp 초기화

BOOL CMapToolApp::InitInstance()
{
#if 0
	// 현재 경로가 서버라면 패스
	TCHAR szCurDirectory[ MAX_PATH ] = {0, };
	GetCurrentDirectory( MAX_PATH, szCurDirectory );
	if( ( nsCSFILE::IsSameString( szCurDirectory, L"Z:\\Digimon Masters" ) == true )||
		( nsCSFILE::IsSameString( szCurDirectory, L"\\\\dmoserver2\\디지몬마스터즈\\digimon masters\\Digimon Masters" ) == true ) )
	{
		MessageBox( NULL, L"자기 컴퓨터에 받아서 실행해주세요.", NULL, MB_OK );
		g_bOnServerComputer = true;
		return FALSE;
	}
#endif

#ifdef NDEBUG
	//g_Dumper;
#endif

	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	//afxAmbientActCtx = FALSE;

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	// 주 창을 만들기 위해 이 코드에서는 새 프레임 창 개체를
	// 만든 다음 이를 응용 프로그램의 주 창 개체로 설정합니다.
	CMDIFrameWnd* pFrame = csnew CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 주 MDI 프레임 창을 만듭니다.
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// 공유 MDI 메뉴 및 액셀러레이터 키 테이블의 로드를 시도합니다.
	//TODO: 추가 멤버 변수를 추가하고 응용 프로그램에 필요한 추가 메뉴 형식에 대한
	//	호출을 로드합니다.
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_MapToolTYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_MapToolTYPE));

	// 주 창이 초기화되었으므로 이를 표시하고 업데이트합니다.
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();	
	
	return TRUE;
}

void CMapToolApp::_Init()
{
	g_Resist.Load();	
	g_Config.Load();
		
	_NewFrameWnd( nsMAPTOOL::MV_MAINTAB,L"Main Tab", CRect( VIEWER_WIDTH, 0, 1280, VIEWER_HEIGHT + 20 ), false );
	_NewFrameWnd( nsMAPTOOL::MV_VIEWER, L"Main Viewer", CRect( 0, 0, VIEWER_WIDTH, VIEWER_HEIGHT ), true );	
	_NewFrameWnd( nsMAPTOOL::MV_NODE, L"Terrain Node", CRect( 0, VIEWER_HEIGHT, 350, VIEWER_HEIGHT + 124 ), false );
	_NewFrameWnd( nsMAPTOOL::MV_SETUP, L"Setup", CRect( 0, VIEWER_HEIGHT + 124, 350, VIEWER_HEIGHT + 124 + 200 ), false );
	_NewFrameWnd( nsMAPTOOL::MV_OBJECT_CONTROL, L"Object Control", CRect( 350, VIEWER_HEIGHT, 700, 1024 - 80 ), false, SW_HIDE );
	_NewFrameWnd( nsMAPTOOL::MV_OBJECT_LIST, L"Object List", CRect( 700, VIEWER_HEIGHT, 1280-11, 1024 - 80 ), false, SW_HIDE );
}

CFrameBase* CMapToolApp::_NewFrameWnd( DWORD ChildID, LPCTSTR Caption, CRect rcView, bool bStatic, int nCmd /*SW_SHOW*/ )
{
	CMainFrame* pFrame		=	STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	//STATIC_DOWNCAST
	//pobject가 class_name으로 주어진 클래스와 직,간접적으로 상속관계에 있을때 변환된 오브젝트의 포인터를 리턴하고 그렇지 않을 경우 NULL을 리턴한다.
	//Debug 모드일 경우 STATIC_DOWNCAST는 AfxStaticDownCast를 호출하여 타입체크를 수행하지만 Debug 모드가 아닐경우 타입체크를 수행하지 않고 static_cast를 호출한다.

	CFrameBase* pChildFrm	=	NULL;
	if( bStatic )
	{
		pChildFrm	=	csnew CFrameStatic;
		pChildFrm->Create( Caption, Caption, WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_SIZEBOX| WS_THICKFRAME, rcView, pFrame ); //MDI Child 윈도우를 생성
	}					// WS_CHILD = 자식 윈도우를 만듬, WS_VISIBLE = 생성시 화면상에 바로 보이도록 하는 윈도우를 만듬
						// WS_OVERLAPPEDWINDOW =  윈도우 스타일.?
	else
	{
		pChildFrm	=	csnew CFrameDynamic;
		pChildFrm->Create( Caption, Caption, WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_SIZEBOX | WS_THICKFRAME, rcView, pFrame );

		m_listWindow.push_back( ChildID );
	}

	m_mapFrame[ ChildID ]  = pChildFrm;
	pChildFrm->_Init( ChildID, rcView );
	pChildFrm->ShowWindow( nCmd );
	
	return pChildFrm;
}

void CMapToolApp::_SortWindow( DWORD ChildID )
{
	std::list< DWORD >::iterator it = find( m_listWindow.begin(), m_listWindow.end(), ChildID );
	if( it != m_listWindow.end() )
	{
		m_listWindow.erase( it );
		m_listWindow.push_back( ChildID );
		m_mapFrame[ ChildID ]->_BringToTop();
	}
	else
	{
		it = m_listWindow.begin();
		for( ; it != m_listWindow.end(); ++it )
		{
			m_mapFrame[ *it ]->_BringToTop();
		}
	}	
}


// CMapToolApp 메시지 처리기

int CMapToolApp::ExitInstance() 
{
	if( g_bOnServerComputer == false )
	{
		g_Resist.Save();
		g_Config.Save();

		g_pRenderer = 0;
		NiCollisionSDM::Shutdown();
		NiShutdown();

		const NiInitOptions* pkInitOptions = NiStaticDataManager::GetInitOptions();
		NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
		NiExternalDelete pkInitOptions;
		NiExternalDelete pkAllocator;
	}

	//TODO: 추가한 추가 리소스를 처리합니다.
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);	

	return CWinApp::ExitInstance();
}


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
	CStatic m_static_Version;
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VERSION, m_static_Version);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	TCHAR szVersion[ 128 ] = {0, };
	VERSION_STRING( MAPTOOL_VERSION, szVersion, 128 );
	m_static_Version.SetWindowText( szVersion );
	return TRUE;  // return TRUE unless you set the focus to a control
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CMapToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();	//다이얼로그를 생성해서 뭔가 입력을 받고 저장을 한다
}

BOOL CMapToolApp::OnIdle(LONG lCount)
{
	MAP_CHILDFRM_IT it = m_mapFrame.begin();
	for( ; it != m_mapFrame.end(); ++it )
		it->second->_Update();	//업데이트를 해주면서 화면이 유지된다.

	CWinApp::OnIdle(lCount);	// 렌더링이 계속 유지되도록 한다.
	return TRUE;
}

BOOL CMapToolApp::PreTranslateMessage(MSG* pMsg)	//다이얼로그에서 키를 입력했는데 작동이 안된다면 이 함수로 재정의해서 사용한다.
{
	if( pMsg->message == WM_MOUSEWHEEL )
	{
		POINT pt;
		pt.x = LOWORD(pMsg->lParam);
		pt.y = HIWORD(pMsg->lParam);

		CRect rcView, rcMainTab;
		GET_MV_VIEW->GetWindowRect( &rcView );	//현재 윈도우의 위치와 크기를 반환한다. 인자값은 어떤 윈도우냐를 가리키는 거겟지.
		GET_MV_MAINTAB_DLG->GetWindowRect( &rcMainTab );
		if( PtInRect( &rcView, pt ) == TRUE )
		{
			GET_MV_VIEW->_OnMouseWheel( (short)HIWORD(pMsg->wParam) );
			return true;
		}
	}
	else if( pMsg->message == WM_KEYDOWN )
	{
		if( (DWORD)pMsg->wParam < VK_OEM_8)
		{
			if( GET_MV_VIEW->_KeyDown( (DWORD)pMsg->wParam ) == true )
				return true;
		}
	}
	else if( pMsg->message == WM_KEYUP )
	{
		if( (DWORD)pMsg->wParam < VK_OEM_8)
		{
			if( GET_MV_VIEW->_KeyUp( (DWORD)pMsg->wParam ) == true )
				return true;
		}
	}
	else if( pMsg->message == WM_SYSKEYDOWN )
	{
		GET_MV_VIEW->_SysKeyDown( (DWORD)pMsg->wParam );
	}
	else if( pMsg->message == WM_SYSKEYUP )
	{
		GET_MV_VIEW->_SysKeyUp( (DWORD)pMsg->wParam );
	}

	return CWinApp::PreTranslateMessage(pMsg);
}


void CMapToolApp::OnCameraReset()	//카메라를 리셋해주는거 같다.
{
	if( ( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_MAKE )||
		( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD_BYTOOL ) )
	{
		GET_MV_VIEW->_ResetCamera();
	}
}

void CMapToolApp::OnViewGridlinetoggle()
{
	if( ( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_MAKE )||
		( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD_BYTOOL ) )
	{
		GET_MV_VIEW->_GetSceneTool()->ToggleWireFrame();
	}
}
void CMapToolApp::ShowMonsterRegionToggle(void)
{

	if(ShowMosterRegion == true)
		ShowMosterRegion = false;
	else
		ShowMosterRegion = true;
}

void CMapToolApp::ShowPathObjectToggle()
{
	if(ShowPathObj == true)
		ShowPathObj = false;
	else
		ShowPathObj = true;
}
bool CMapToolApp::GetShowObj(void)
{
	return ShowPathObj;
}
bool CMapToolApp::GetShowMonsterRegion(void)
{
	return ShowMosterRegion;
}

void CMapToolApp::SetShowPathObj(bool TOF)
{
	ShowPathObj = TOF;
}
void CMapToolApp::SetShowMonsterRegion(bool TOF)
{
	ShowMosterRegion = TOF;
}

void CMapToolApp::OnConfig()
{
	_DlgConfig dc;
	dc.DoModal();
}

void CMapToolApp::OnFileLoadas()
{
	if( GET_MV_MAINTAB_DLG->_GetSelTabIndex() != _DlgMainTab::TT_MAIN )
	{
		MessageBox( NULL, L"메인 탭으로 가셔서 로드 해주세요", NULL, MB_OK );
		return;
	}

	if( ( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_MAKE )||
		( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD_BYTOOL ) )
	{
		GET_MV_MAINTAB_DLG->_GetDlgMain()->OnBnClickedBtnLoad();
	}
}

void CMapToolApp::OnFileSaveas()
{
	if( GET_MV_MAINTAB_DLG->_GetSelTabIndex() != _DlgMainTab::TT_MAIN )
	{
		MessageBox( NULL, L"메인 탭으로 가셔서 세이브 해주세요", NULL, MB_OK );
		return;
	}

	if( ( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_MAKE )||
		( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD_BYTOOL ) )
	{
		GET_MV_MAINTAB_DLG->_GetDlgMain()->OnBnClickedBtnSave();
	}
}
