// LauncherDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Launcher.h"
#include "LauncherDlg.h"
#include "BmpButton.h"
#include "StrButton.h"
#include "Progressbar.h"
#include "Option.h"
#include <TlHelp32.h>

#ifdef CHECK_WINDOW_10
	#include "WindowVersionChecker.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLauncherDlg 대화 상자

CLauncherDlg*	g_pDlg = NULL;

static int CALLBACK BrowseForFolder_CallbackProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	if( uMsg == BFFM_INITIALIZED )
		SendMessage( hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpData );
	return 0;
}

void CLauncherDlg::CallBack_File_OnFileInfo(IN void* pThisPointer, IN CHAR* pszFileNameA, IN WCHAR* pszFileNameW, IN ULONG ulFileTotalCount, IN ULONGLONG ullFileSize, IN ULONG ulCPCode, IN ULONG ulGameCode, IN ULONG ulFileID, IN ULONG ulFileType)
{
	g_pThread->GetProc()->SetCurTotalSize( ullFileSize );
	g_pThread->GetProc()->SetTotalTotalSize( ullFileSize );
}

void CLauncherDlg::CallBack_File_OnDownloadStart(IN void* pThisPointer)
{	
	g_pDlg->m_bDownStart = true;
}

void CLauncherDlg::CallBack_File_OnDownloadComplete(IN void* pThisPointer)
{	
	ULONG nExtraTime, nLapseTime;
	ULONG nDownCompleteCount = 0;	
	ULONG nDownloadSpeed;
	ULONGLONG nDownloadFileSize;
	g_pDlg->m_DownEngine.GetDownloadInfo( &nDownCompleteCount, &nDownloadSpeed, &nDownloadFileSize, &nExtraTime, &nLapseTime );		

	g_pThread->GetProc()->SetCurProcSize( nDownloadFileSize );		
	g_pThread->GetProc()->SetTotalProcSize( nDownloadFileSize );

	g_pDlg->m_bDownStart = false;	
	g_pDlg->m_strGameState->ChangeStr( STR_IN_INSTALL );

	g_pDlg->m_strTime->ChangeStr(_T(""));
	Sleep(200);
		
	ShellExecute(NULL, L"runas", g_pDlg->m_szFullDown, NULL, NULL, SW_SHOWNORMAL );

	sTHREAD_INPUT* pData = new sTHREAD_INPUT;
	pData->s_eType = sTHREAD_INPUT::INSTALL;	
	g_pThread->GetProc()->Add( pData );
}


CLauncherDlg::CLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLauncherDlg::IDD, pParent), m_bClickTitle(false)/*,m_bStartPage(false)*/
{
	g_pDlg = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

BOOL CLauncherDlg::Init( CString const& strCurrentPath, WORD const& wLauncherCurrentVersion, sConfiguration const& info )
{
	m_bClickTitle = false;
	m_strCurrentPath = strCurrentPath;
	m_launcherInfos = info;
	m_nLauncherVer = wLauncherCurrentVersion;

	return TRUE;
}

void CLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);		
}

BEGIN_MESSAGE_MAP(CLauncherDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()	
	ON_MESSAGE( WM_KICKIDLE, OnKickIdle )	
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()	
	ON_WM_MOUSEMOVE()	
	ON_WM_ERASEBKGND()		
END_MESSAGE_MAP()


LRESULT CLauncherDlg::OnKickIdle( WPARAM, LPARAM lCount ) 
{	
	_Update();
	_Render();
	
	Sleep( 1 );	

	UpdateDialogControls(this, FALSE);
	
	return (LRESULT)1;
}

BOOL CLauncherDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	DeleteWaitQueue();

	InternetCloseHandle( m_hSession );

	CThread::GlobalShotDown();

	SAFE_DELETE( g_GameInfo );

	SAFE_DELETE( g_pStrMng );
	SAFE_DELETE( m_pProgressCur );
	SAFE_DELETE( m_pProgressTotal );

	SAFE_DELETE( m_pRun );
	SAFE_DELETE( m_pInstall );
	SAFE_DELETE( m_pUpdate );
	SAFE_DELETE( m_pFileCheck );
	SAFE_DELETE( m_pMinimalize );
	SAFE_DELETE( m_pClose );

	SAFE_DELETE( m_pOptionPage );
	SAFE_DELETE( m_pHomePage );
	SAFE_DELETE( m_pPatch );
/*	SAFE_DELETE( m_pCheckbox );*/
	
	for( int i = GAME_DIGIMONMASTERS; i < GAME_END; i++ )
	{
		SAFE_DELETE( m_pGameButton[ i ] );
	}

	m_DownEngine.Close();

	if( m_dlgSeverCheck )
		m_dlgSeverCheck.SendMessage( WM_CLOSE );	

	if( m_dlgMessageBox )
		m_dlgMessageBox.SendMessage( WM_CLOSE );	

	return CDialog::DestroyWindow();
}

// CLauncherDlg 메시지 처리기
BOOL CLauncherDlg::OnInitDialog()
{	
	CDialog::OnInitDialog();			

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);				// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.		

	// 메세지 창
	m_dlgMessageBox.Create( IDD_MESSAGEBOX, this );
	m_dlgMessageBox.ShowWindow( SW_HIDE );	

	Init();	


	// 외형 라운드주기
	CRect rect;
	CRgn rgn;

	GetClientRect(rect);
	rgn.CreateRoundRectRgn( rect.left, rect.top, rect.right, rect.bottom, 10, 10 );
	::SetWindowRgn( this->m_hWnd, (HRGN)rgn, TRUE );

	// 서버체크 창		
	m_dlgSeverCheck.Create( IDD_SERVERCHECK, this );
	m_dlgSeverCheck.ShowWindow( SW_HIDE );

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

BOOL CLauncherDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//--------------------------------------------------------------------------
	// Background Bitmap
	//--------------------------------------------------------------------------
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_MAIN);
	BITMAP bm;
	bitmap.GetBitmap(&bm);

	CDC cdc;
	cdc.CreateCompatibleDC(pDC);
	CBitmap* pBit = cdc.SelectObject(&bitmap);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &cdc, 0, 0, SRCCOPY);
	cdc.SelectObject(pBit);
	cdc.DeleteDC();	

	return FALSE;

	//return CDialog::OnEraseBkgnd(pDC);
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLauncherDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return;
	//CDialog::OnOK();
}

void CLauncherDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CThreadProc::sINFO TotalInfo;
	g_pThread->GetProc()->GetTotalInfo( &TotalInfo );

	if( TotalInfo.s_nTotalSize != 0 )
	{				
		g_pThread->GetProc()->StopThread();		
		g_pDlg->Quit( STR_MESSAGE_CANCEL );
	}
	else
	{
		CDialog::OnCancel();
	}
}

void CLauncherDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_dlgMessageBox.IsWindowVisible() == TRUE )
	{
		m_dlgMessageBox.OnLButtonDown( nFlags, point ); 
		return;
	}

	if( m_dlgSeverCheck.IsWindowVisible() == TRUE )
	{
		m_dlgSeverCheck.OnLButtonDown( nFlags, point );
		return;
	}	

	SetCapture();

	m_eMouseState = eMOUSE_DOWN;
	m_bClickTitle = false;
	if( point.y < 46 )
	{
		m_bClickTitle = true;
		GetCursorPos( &m_ptBackupDownPos );
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CLauncherDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_dlgMessageBox.IsWindowVisible() == TRUE )
	{
		m_dlgMessageBox.OnLButtonUp( nFlags, point ); 
		return;
	}

	if( m_dlgSeverCheck.IsWindowVisible() == TRUE )
	{
		m_dlgSeverCheck.OnLButtonUp( nFlags, point );
		return;
	}	

	ReleaseCapture();

	m_eMouseState = eMOUSE_DEFAULT;
	m_bClickTitle = false;

	// 스트링 버튼들
	if( m_pHomePage->GetState() == eMOUSE_DOWN )  // 홈페이지 바로가기
	{
		ShellExecute(NULL, L"open", L"iexplore.exe", g_GameInfo->GetGameInfo()->s_szHomePageUrl, NULL, SW_SHOWNORMAL );
	}
	else if( m_pOptionPage->GetState() == eMOUSE_DOWN )// 옵션창 오픈
	{
		TCHAR Default[ 512 ];
		TCHAR sz[ 512 ];

		if( REG::RegReadString( HKEY_CURRENT_USER, g_GameInfo->GetGameInfo()->s_szRegistryPath, REG_CLIENT_PATH_VALUE, Default, sz, 512 ) == FALSE )
		{
			if( REG::RegReadString( HKEY_LOCAL_MACHINE, g_GameInfo->GetGameInfo()->s_szRegistryPath, REG_CLIENT_PATH_VALUE, Default, sz, 512 ) == FALSE )
			{
				MyMessageBox( STR_FAIL_GAME_REG );
				return;
			}
		}

		char gamePath[MAX_PATH]={0,};
		W2M( gamePath, sz, MAX_PATH );

		COption optionDlg(gamePath);
		optionDlg.DoModal();
	}
	else if( m_pPatch->GetState() == eMOUSE_DOWN ) // 패치내용 바로가기
	{
		ShellExecute(NULL, L"open", L"iexplore.exe", g_GameInfo->GetGameInfo()->s_szPatchPageUrl, NULL, SW_SHOWNORMAL );
	}	
	else if( m_pFileCheck->GetState() == eMOUSE_DOWN )
	{

	}

	else if( m_pMinimalize->GetState() == eMOUSE_DOWN )
	{
		PostMessage( WM_SYSCOMMAND, (WPARAM)SC_MINIMIZE, 0 );
	}
	else if( m_pClose->GetState() == eMOUSE_DOWN )
	{		
		OnCancel();
	}
	else
	{
		if( _IsExecute() == true )
		{
			MyMessageBox( STR_IN_ISEXECUTE );
		}
		else
		{

			// 게임 시작 버튼 눌렀을때!!
			switch( m_eGameState )
			{	
			case eINSTALL:
				if( m_pInstall->GetState() == eMOUSE_DOWN  )
				{	
// 					if( _IsInstaller() == true )
// 					{				
// 						MyMessageBox( STR_MESSAGE_INSTALL_URL, CMyMessageBox::MT_INSTALL );
// 					}
// 					else
// 					{
						GoThread();						
					//}
				}
				break;
			case eUPDATE:
				if( m_pUpdate->GetState() == eMOUSE_DOWN )
				{
					if( _IsServerMaintenance() && !PassServerCheck() )
						ShowMaintenanceWindow();
					else
						GoThread();
				}
				break;
			case eRUN: 
				if( m_pRun->GetState() == eMOUSE_DOWN )
				{
					if( !_IsServerMaintenance() || PassServerCheck() )
					{
						TCHAR Default[ 512 ];
						
						TCHAR szGameDir[512] = {0,};

						if( REG::RegReadString( HKEY_CURRENT_USER, g_GameInfo->GetGameInfo()->s_szRegistryPath, REG_CLIENT_PATH_VALUE, Default, szGameDir, 512 ) == FALSE )
						{
							if( REG::RegReadString( HKEY_LOCAL_MACHINE, g_GameInfo->GetGameInfo()->s_szRegistryPath, REG_CLIENT_PATH_VALUE, Default, szGameDir, 512 ) == FALSE )
							{
								Quit( STR_FAIL_GAME_REG );
								break;
							}
						}
						
						_tcscat_s(szGameDir, 512, L"\\" );
						
						TCHAR szExeFullPath[512] = {0,};
						_tcscpy_s(szExeFullPath, 512, szGameDir);
						_tcscat_s(szExeFullPath, 512, g_GameInfo->GetGameInfo()->s_szGameFileName );

						switch( g_GameInfo->GetGameType() )
						{
						case GAME_DIGIMONMASTERS:
							{
								if( _CheckExeFile(szExeFullPath) == false )
								{
									ReturnVersion();
									Quit( STR_FAIL_EXEFILE_VERSION );
								}
								else
								{
#ifdef CHECK_WINDOW_10
									// 윈도우 8이상일 경우 클라이언트 winxpsp3 호환 모드 실행하게 함.
									if( WVS::IsWindows8OrGreater() )
									{
										WVS::APPCOMPATFLAG flag = WVS::GetAppCompatFlag(szExeFullPath);
										if( WVS::acpfWin7RTM > flag )
											WVS::SetAppCompatFlag(szExeFullPath, WVS::acpfWin7RTM );
									}
#endif

									// VS2015 재배포 패키지가 설치 되어 있는지 확인함.
									if (_CheckRedist2015(szGameDir))
									{
										// 프로세서에 넘길 인자 만들기
										CString strArg;
										strArg += g_GameInfo->GetID(); // 유저 아이디
										strArg += " ";
										strArg += g_GameInfo->GetKey();	// 유저 키값
										strArg += " ";
										strArg += g_GameInfo->GetUT();	// 유저 타입
										strArg += " ";
										strArg += g_GameInfo->GetST();	// 서버 타입						

										if (_PlayGame(szExeFullPath, (TCHAR*)(LPCTSTR)strArg) == true)
											PostMessage(WM_QUIT);
									}
									else
										PostMessage(WM_QUIT);
								}
							}				 	
							break;					
						default:
							{
								assert_cs( false );
							}
							break;
						}				
					}
					else
					{				
						ShowMaintenanceWindow();
					}			
				}
				break;
			case ePACK:
				break;
			default:
				Quit( STR_ERROR );		
				break;
			}
		}
	}

// 	if( m_pCheckbox && m_pCheckbox->GetState() == eMOUSE_DOWN )
// 		m_bStartPage = !m_bStartPage;		//시작페이지 설정

// #ifdef TOTAL_LAUNCHER
// 	if( g_GameInfo->GetUserType() == UT_DIGITALIC ) // 우리 유저
// 	{
// 		// 다른게임 선택
// 		for( int i = GAME_DIGIMONMASTERS; i < GAME_END; i++ )
// 		{
// 			if( m_pGameButton[ i ]->GetState() == eMOUSE_DOWN )
// 			{
// 				g_GameInfo->SetGameType( (eGameType)i );
// 				_SelectGame();			
// 			}
// 		}
// 	}
// #endif

	CDialog::OnLButtonUp(nFlags, point);
}

void CLauncherDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_dlgMessageBox.IsWindowVisible() == TRUE )
		return;		

	if( m_dlgSeverCheck.IsWindowVisible() == TRUE )
	{
		m_dlgSeverCheck.OnMouseMove( nFlags, point );
		return;	
	}

	// 윈도우 이동
	if( m_bClickTitle == true )
	{
		CPoint ptCurPos;
		GetCursorPos( &ptCurPos );
		m_ptWindowPos += ( ptCurPos - m_ptBackupDownPos );
		m_ptBackupDownPos = ptCurPos;

		SetWindowPos( NULL, m_ptWindowPos.x, m_ptWindowPos.y, 0, 0, SWP_NOSIZE );
	}	

	CDialog::OnMouseMove(nFlags, point);
}

/*-------------------------------------------------------------------------------------------------

	일반 함수들

---------------------------------------------------------------------------------------------------*/
void CLauncherDlg::Init()
{	
	m_bQuit = false;
	m_bDownStart = false;
	m_pProgressCur = NULL;
	m_pProgressTotal = NULL;
	m_pRun = NULL;
	m_pInstall = NULL;
	m_pUpdate = NULL;
	m_pFileCheck = NULL;
	m_pMinimalize = NULL;
	m_pClose = NULL;		

	for( int i = GAME_DIGIMONMASTERS; i < GAME_END; i++ )
	{
		m_pGameButton[ i ] = NULL;
	}	

	// 인터넷 연결
	m_hSession = InternetOpen( L"Digitalic", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );	
	if ( m_hSession == NULL ) 
		Quit( STR_FAIL_CONNECT_INTERNET );

	if( CThread::GlobalInit() == false )
		Quit( STR_FAIL_THREAD );

	g_pThread->GetProc()->SetInternet( &m_hSession );

	CLauncherApp * pApp = &theApp;
	g_GameInfo = new cGameInfo;
	TCHAR pParam[MAX_PATH] = {0,};
	_stprintf_s( pParam, MAX_PATH, L"%s", pApp->m_wsCmdLine.GetBuffer() );
	if( !g_GameInfo->Init( pParam, m_launcherInfos ) )
		g_pDlg->Quit( RUN_FAIL );

	g_pStrMng = new cStrMng;
	g_pStrMng->Init();

	_Init_Control();	// UI 컨트롤러 
	_Init_Str();		// 문자열
	_Init_Web();		// 웹 컨트롤러	
	_Init_Window();		// 다이얼로그 윈도우
	_SelectGame();		// 버천 체크
}

// void CLauncherDlg::LoadConfigFile( TCHAR* pConfigFileName )
// {
// 	CString strPath;
// 	strPath += g_szLauncherPath;
// 	strPath += L"/";
// 	strPath += pConfigFileName;
// 
// 	if( !PathFileExists( strPath ) )
// 		return;
// 
// 	TCHAR pCdnPath[MAX_PATH] = {0,};
// 	GetPrivateProfileString( L"CDN", L"Path", L"", pCdnPath, MAX_PATH, strPath );
// 	
// 
// }

void CLauncherDlg::_Init_Window()
{
	SetWindowText( STR_IN_WINDOW_TITLE );

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX); 
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	//-------------------------------------------------------------------
	// Set Bitmap Background
	//-------------------------------------------------------------------
	// Main 창
	int cxWindow = GetSystemMetrics(SM_CXSCREEN);
	int cyWindow = GetSystemMetrics(SM_CYSCREEN);
	m_ptWindowPos.x = (cxWindow - _BackGround_Width)/2;
	m_ptWindowPos.y = (cyWindow - _BackGround_Height)/2;
	SetWindowPos(NULL, m_ptWindowPos.x, m_ptWindowPos.y, _BackGround_Width, _BackGround_Height, SWP_SHOWWINDOW);

	DWORD dwStyle = GetWindowLong( m_hWnd, GWL_STYLE );
	SetWindowLong( m_hWnd, GWL_STYLE, dwStyle | WS_CLIPCHILDREN );		
}

void CLauncherDlg::_Init_Str()
{
	TCHAR sz[ 64 ];
	_tcscpy_s( sz, 64, g_GameInfo->GetID() );
	_tcscat_s( sz, 64, L" 님" );

	// 전체진행률
	sStrInfo* pInfo;
	pInfo = new sStrInfo;
	pInfo->Init( sz, CPoint( 32, 52 ) );
	pInfo->s_nFontSize = 16;	
	pInfo->s_bBold = true;	
	g_pStrMng->SetText( pInfo );					

	pInfo = new sStrInfo;
#ifdef VERSION_QA
	pInfo->Init( L"환영 합니다(QA 버젼)", CPoint( 32, 72 ) );
#else
	pInfo->Init( L"환영 합니다", CPoint( 32, 72 ) );
#endif
	pInfo->s_nFontSize = 15;	
	pInfo->s_bBold = true;	
	g_pStrMng->SetText( pInfo );		

	// 런처 버젼
	pInfo = new sStrInfo;
	_stprintf_s( sz, 64, L"Ver %.2f", m_nLauncherVer / 100.0f );

	pInfo->Init( sz, CPoint( 735, 583 ) );
	pInfo->s_nFontSize = 15;	
	pInfo->s_bBold = true;	
	g_pStrMng->SetText( pInfo );

	m_strClientVersion = new sStrInfo;
	_stprintf_s(sz, 64, STR_CLIENT_PATH_NUMBER, 0);
	m_strClientVersion->Init(sz, CPoint(20, 583));
	m_strClientVersion->s_nFontSize = 15;
	m_strClientVersion->s_bBold = true;
	g_pStrMng->SetText(m_strClientVersion);


	pInfo = new sStrInfo;
	pInfo->Init( STR_IN_FULL_INSTALL, CPoint( 208, 555 ) );	
	g_pStrMng->SetText( pInfo );	

	// 현재진행 스트링
	m_strGameState = new sStrInfo;	
	m_strGameState->Init( STR_IN_NOTTEXT, CPoint( 208, 537 ) );
	g_pStrMng->SetText( m_strGameState );		

	pInfo = new sStrInfo;
	pInfo->Init( STR_IN_CUR_FILE, CPoint(208, 515 ) );
	g_pStrMng->SetText( pInfo );

	// 선택한 게임이름 스트링
	m_strGameName = new sStrInfo;
	m_strGameName->s_nFontSize = 16;
	m_strGameName->s_bOutline = true;
	m_strGameName->s_bBold = true;
	m_strGameName->Init( STR_IN_NOTTEXT, CPoint( 212, 486 ) );
	g_pStrMng->SetText( m_strGameName );			

	// 파일명 스트링
	m_strGameFile = new sStrInfo;	
	m_strGameFile->Init( STR_IN_NOTTEXT, CPoint( 292, 515 ) );
	g_pStrMng->SetText( m_strGameFile );	

	// 진행률 표시
	m_strCurPer = new sStrInfo;	
	m_strCurPer->Init( STR_IN_NOTTEXT, CPoint( 625, 538 ) );
	m_strCurPer->s_nFontSize = 12;
	g_pStrMng->SetText( m_strCurPer );

	m_strTotalPer = new sStrInfo;	
	m_strTotalPer->Init( STR_IN_NOTTEXT, CPoint( 625, 556 ) );
	m_strTotalPer->s_nFontSize = 12;
	g_pStrMng->SetText( m_strTotalPer );

	m_strTime = new sStrInfo;	
	m_strTime->Init( STR_IN_NOTTEXT, CPoint( 520, 515 ) );
	m_strTime->s_nFontSize = 13;
	g_pStrMng->SetText( m_strTime );
}

void CLauncherDlg::_Init_Web()
{	
	CRect rect;

	rect.left = 201;
	rect.top = 39;
	rect.right = rect.left + 579;
	rect.bottom = rect.top + 426;			

	DWORD dwPlag = WS_CHILD | WS_VISIBLE;
	int htmlViewBorder = 2;

	m_pMainHtmlView = new CMyHtmlView;		
	m_pMainHtmlView->Create(NULL, L"", dwPlag , rect, this, IDC_WEB_MAIN ) ;
	
	m_pMainHtmlView->SetLeft( -htmlViewBorder );
	m_pMainHtmlView->SetTop( -htmlViewBorder );
	m_pMainHtmlView->SetWidth( m_pMainHtmlView->GetWidth() + htmlViewBorder );
	m_pMainHtmlView->SetHeight( m_pMainHtmlView->GetHeight() + htmlViewBorder );

	m_pMainHtmlView->Navigate( g_GameInfo->GetGameInfo()->s_szMainUrl );
	m_pMainHtmlView->OnInitialUpdate();

	//if( _tcscmp( g_pDlg->m_szUsrType, L"DMOM" ) == 0 ) // 우리 유저
	//{
	//	rect.left = 22;
	//	rect.top = 268;
	//	rect.right = rect.left + 172;
	//	rect.bottom = rect.top + 312;					
	//}
	//else	
	{
		rect.left = 22;
		rect.top = 160;
		rect.right = rect.left + 172;
		rect.bottom = rect.top + 422;				
	}

	m_pSubHtmlView = new CMyHtmlView;		
	m_pSubHtmlView->Create(NULL, L"", dwPlag , rect, this, IDC_WEB_SUB ) ;

	m_pSubHtmlView->SetLeft( -htmlViewBorder );
	m_pSubHtmlView->SetTop( -htmlViewBorder );
	m_pSubHtmlView->SetWidth( m_pSubHtmlView->GetWidth() + htmlViewBorder );
	m_pSubHtmlView->SetHeight( m_pSubHtmlView->GetHeight() + htmlViewBorder );

	m_pSubHtmlView->Navigate( g_GameInfo->GetGameInfo()->s_szSubUrl );
	m_pSubHtmlView->OnInitialUpdate();		
}

void CLauncherDlg::_Init_Control()
{
	m_BackBitmap.LoadBitmap( IDB_MAIN );	
// 	m_CheckBoxOn.LoadBitmap( IDB_CHECKBOX1 );
// 	m_CheckBoxOff.LoadBitmap( IDB_CHECKBOX2 );	

	m_pProgressTotal = new cProgressbar;
	m_pProgressTotal->Init( IDB_PRO1, CPoint( 288, 557 ), CPoint( 331, 8 ) );	

	m_pProgressCur = new cProgressbar;
	m_pProgressCur->Init( IDB_PRO2, CPoint( 288, 540 ), CPoint( 331, 8 ) );	

	m_pRun = new cBmpButton;
	m_pRun->Init( this, CPoint( 660, 520 ), CPoint( 109, 45 ), IDB_RUN1, IDB_RUN2, IDB_RUN3, IDB_RUN4 );	
	
	m_pUpdate = new cBmpButton;
	m_pUpdate->Init( this, CPoint( 660, 520 ), CPoint( 109, 45 ), IDB_UPDATE1, IDB_UPDATE2, IDB_UPDATE3, IDB_UPDATE4 );	
	
	m_pInstall = new cBmpButton;
	m_pInstall->Init( this, CPoint( 660, 520 ), CPoint( 109, 45 ), IDB_INST1, IDB_INST2, IDB_INST3, IDB_INST4 );	

	// 지금은 뺏음 랜더링 업데이트만 안함
	m_pFileCheck = new cBmpButton;
	m_pFileCheck->Init( this, CPoint( 660, 540 ), CPoint( 109, 25 ), IDB_CHECK1, IDB_CHECK2, IDB_CHECK3, IDB_CHECK4 );
	m_pFileCheck->SetButton( eMOUSE_DISABLE );

	m_pMinimalize = new cBmpButton;
	m_pMinimalize->Init( this, CPoint( 752, 12 ), CPoint( 16, 16 ), IDB_MINI1, IDB_MINI2, IDB_MINI3, IDB_MINI4 );

	m_pClose = new cBmpButton;
	m_pClose->Init( this, CPoint( 772, 12 ), CPoint( 16, 16 ), IDB_CLOSE1, IDB_CLOSE2, IDB_CLOSE3, IDB_CLOSE4 );

	m_pGameButton[ GAME_DIGIMONMASTERS ] = new cBmpButton;
	m_pGameButton[ GAME_DIGIMONMASTERS ]->Init( this, CPoint( 22, 185 ), CPoint( 174, 26 ), IDB_MASTER1, IDB_MASTER2, IDB_MASTER3, IDB_MASTER4 );
	m_pGameButton[ GAME_DIGIMONRPG ] = new cBmpButton;
	m_pGameButton[ GAME_DIGIMONRPG ]->Init( this, CPoint( 22, 213 ), CPoint( 174, 26 ), IDB_RPG1, IDB_RPG2, IDB_RPG3, IDB_RPG4 );
	m_pGameButton[ GAME_NOIR ] = new cBmpButton;
	m_pGameButton[ GAME_NOIR ]->Init( this, CPoint( 22, 241 ), CPoint( 174, 26 ), IDB_NOIR1, IDB_NOIR2, IDB_NOIR3, IDB_NOIR4 );

#ifdef TEST_VERSION
	for( int i = GAME_DIGIMONMASTERS; i < GAME_END; i++ )
	{
		m_pGameButton[ i ]->SetButton( eMOUSE_DISABLE );		
	}	
#endif

	m_pHomePage = new cStrButton;
	m_pHomePage->Init( this, CPoint( 687, 15 ), STR_BUTTON_HOMEPAGE, true, FONT_WHITE, FONT_OVER );

	m_pOptionPage = new cStrButton;
	m_pOptionPage->Init( this, CPoint( 418, 487 ), STR_BUTTON_OPTION, true, FONT_WHITE, FONT_OVER );

	m_pPatch = new cStrButton;
	m_pPatch->Init( this, CPoint( 348, 487 ), STR_BUTTON_PATCH, true, FONT_WHITE, FONT_OVER );		
	
// 	m_pCheckbox = NULL;
// 	if( g_GameInfo->IsUserType(_T("PNET")) || g_GameInfo->IsUserType(_T("PN")) )
// 	{
// 		m_pCheckbox = new cStrButton;
// 		m_pCheckbox->Init( this, CPoint( 534, 15 ), STR_IN_STARTPAGE_NAVER, false, FONT_WHITE, FONT_OVER );	
// 	}
	/*else
	{
		m_pCheckbox->Init( this, CPoint( 534, 15 ), STR_IN_STARTPAGE_OUR, false, FONT_WHITE, FONT_OVER );		
	}*/

	CString setStartHomepage;
	setStartHomepage += m_strCurrentPath;
	setStartHomepage += CHECK_BOX;

// 	m_bStartPage = false;	
// 	if( GetPrivateProfileIntW( L"STARTPAGE", L"value", 0, setStartHomepage ) == 1 )
// 		m_bStartPage = true;
}

void CLauncherDlg::_CheckRegestry()
{	
	if( m_nServerVersion == m_nClientVersion )
	{
		m_eGameState = eRUN;
		m_strGameState->ChangeStr( STR_IN_START );
		m_strGameFile->ChangeStr( L"" );

		m_pProgressCur->SetPersent( 1.0f );
		m_pProgressTotal->SetPersent( 1.0f );				

		m_strCurPer->ChangeStr( L"100%" );
		m_strTotalPer->ChangeStr( L"100%" );
	}
	else
	{
		m_eGameState = eUPDATE;
		m_strGameState->ChangeStr( STR_IN_UPDATE );
	}	

	if( eINSTALL != m_eGameState && _IsServerMaintenance() )
		m_strGameState->ChangeStr( STR_IN_CHECK );			
}


void CLauncherDlg::_Update()
{
	if( m_dlgMessageBox.IsWindowVisible() == TRUE )
	{
		m_dlgMessageBox.Update();
		return;
	}

	if( m_dlgSeverCheck.IsWindowVisible() == TRUE )
	{
		m_dlgSeverCheck.Update();
		return;
	}	

	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED); 	

	if( m_bDownStart == true )
	{
		ULONG nExtraTime, nLapseTime = 0;
		ULONG nDownCompleteCount = 0;	
		ULONG nDownloadSpeed;
		ULONGLONG nDownloadFileSize;
		m_DownEngine.GetDownloadInfo( &nDownCompleteCount, &nDownloadSpeed, &nDownloadFileSize, &nExtraTime, &nLapseTime );		

		g_pThread->GetProc()->SetCurProcSize( nDownloadFileSize );		
		g_pThread->GetProc()->SetTotalProcSize( nDownloadFileSize );

		TCHAR sz[ 128 ];
		_stprintf_s( sz, 128, L"%d분%d초 남음 (%dKB)", nExtraTime / 60, nExtraTime % 60, nDownloadSpeed );
		m_strTime->ChangeStr( sz );
	}
	
	if( m_bQuit == false )
	{
		CThreadProc::sINFO CurInfo;
		g_pThread->GetProc()->GetCurInfo( &CurInfo );

		TCHAR sz[ 16 ];
		if( CurInfo.s_nTotalSize > 0 )
		{				
			_stprintf_s( sz, 16, L"%d%%", (int)( ( (float)CurInfo.s_nProcSize / (float)CurInfo.s_nTotalSize ) * 100 ) );
			m_strCurPer->ChangeStr( sz );
			m_pProgressCur->SetPersent( (float)CurInfo.s_nProcSize / (float)CurInfo.s_nTotalSize );			
		}

		CThreadProc::sINFO TotalInfo;
		g_pThread->GetProc()->GetTotalInfo( &TotalInfo );

		if( TotalInfo.s_nTotalSize > 0 )
		{		
			_stprintf_s( sz, 16, L"%d%%", (int)( ( (float)TotalInfo.s_nProcSize / (float)TotalInfo.s_nTotalSize ) * 100 )  );
			m_strTotalPer->ChangeStr( sz );
			m_pProgressTotal->SetPersent( (float)TotalInfo.s_nProcSize / (float)TotalInfo.s_nTotalSize );			
		}		
	}	

	// 스트링 버튼들
	m_pHomePage->Update( m_eMouseState );
	m_pOptionPage->Update(m_eMouseState);
	m_pPatch->Update( m_eMouseState );

// 	if( m_pCheckbox )
// 		m_pCheckbox->Update( m_eMouseState );	

	// 비트맵 버튼들
	switch( m_eGameState )
	{
	case eINSTALL:
		m_pInstall->Update( m_eMouseState );
		break;
	case eUPDATE:
	case ePACK:
		m_pUpdate->Update( m_eMouseState );
		break;
	case eRUN:	
		m_pRun->Update( m_eMouseState );
		break;	
	default:		
		Quit( STR_ERROR );		
		break;
	}	

	//m_pFileCheck->Update( m_eMouseState );
	m_pMinimalize->Update( m_eMouseState );
	m_pClose->Update( m_eMouseState );	

	//if( _tcscmp( g_pDlg->m_szUsrType, L"DMOM" ) == 0 ) // 우리 유저
	//{
	//	for( int i = GAME_DIGIMONMASTERS; i < GAME_END; i++ )
	//	{	
	//		m_pGameButton[ i ]->Update( m_eMouseState );		
	//	}
	//}	
}

void CLauncherDlg::_Render()
{
	if( m_dlgMessageBox.IsWindowVisible() == TRUE )
	{
		m_dlgMessageBox.Render();		
	}

	if( m_dlgSeverCheck.IsWindowVisible() == TRUE )
	{
		m_dlgSeverCheck.Render();		
	}

	CClientDC dc( this );

	CRect	rect;
	GetClientRect( &rect );

	CBitmap memBitmap;
	memBitmap.DeleteObject();
	memBitmap.CreateCompatibleBitmap( &dc, rect.Width(), rect.Height() );

	CDC MemDC; 
	CBitmap* pOldBitmap;		
	
	MemDC.CreateCompatibleDC( &dc );	
	pOldBitmap = MemDC.SelectObject( &memBitmap );	

	// 배경 화면 
	BMP_RENDER::Render_Bmp( &MemDC, 0, 0, &m_BackBitmap, false );					

	// 프로그래스바
	m_pProgressCur->Render( &dc, &MemDC );
	m_pProgressTotal->Render( &dc, &MemDC );
	
	// 실행 버튼 랜더
	switch( m_eGameState )
	{
	case eINSTALL:
		m_pInstall->Render( &dc, &MemDC );
		break;
	case eUPDATE:
	case ePACK:
		m_pUpdate->Render( &dc, &MemDC );
		break;
	case eRUN:			
		m_pRun->Render( &dc, &MemDC );
		break;
	default:
		Quit( STR_ERROR );
		break;
	}
	//m_pFileCheck->Render( &dc, &MemDC );
	m_pMinimalize->Render( &dc, &MemDC );
	m_pClose->Render( &dc, &MemDC );	

	// 게임선택 버튼
	for( int i = GAME_DIGIMONMASTERS; i < GAME_END; i++ )
	{
		m_pGameButton[ i ]->Render( &dc, &MemDC );				
	}		

	// 현재 선택되어있는 게임 버튼
	BMP_RENDER::Render_Bmp( &MemDC, 22, 131, m_pGameButton[ g_GameInfo->GetGameType() ]->GetCurBmpUp(), false );	
	
// 	if( m_pCheckbox )
// 	{
// 		if( m_bStartPage == true )
// 			BMP_RENDER::Render_Bmp( &MemDC, 531, 16, &m_CheckBoxOn, false );
// 		else
// 			BMP_RENDER::Render_Bmp( &MemDC, 531, 16, &m_CheckBoxOff, false );
// 
// 		// 스트링 버튼들
// 		m_pCheckbox->Render( &MemDC );
// 	}

	m_pOptionPage->Render(&MemDC);
	m_pHomePage->Render( &MemDC );
	m_pPatch->Render( &MemDC );	
	
	// 스트링 랜더
	g_pStrMng->Render( &MemDC );			

	dc.BitBlt( 0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY );		
	MemDC.SelectObject( pOldBitmap );	
}

void CLauncherDlg::_SelectGame()
{	
	m_pMainHtmlView->Navigate( g_GameInfo->GetGameInfo()->s_szMainUrl );
	m_pSubHtmlView->Navigate( g_GameInfo->GetGameInfo()->s_szSubUrl );

	switch( g_GameInfo->GetGameType() )
	{
	case GAME_DIGIMONMASTERS:	m_strGameName->ChangeStr( STR_DIGIMONMASTERS_KOR );	break;
	case GAME_DIGIMONRPG:		m_strGameName->ChangeStr( STR_DIGIMONRPG_KOR );		break;
	case GAME_NOIR:				m_strGameName->ChangeStr( STR_NOIR_KOR );			break;
	}	
	
	_VersionCheck();	// 버전 체크함	
	InitVersion();	
}

void CLauncherDlg::InitVersion()
{	
	cGameInfo::sGame* pGameInfo =  g_GameInfo->GetGameInfo();
	if( NULL == pGameInfo )
		return;

	// 게임이 설치되어있지 않는경우
	if( REG::RegReadString( HKEY_CURRENT_USER, pGameInfo->s_szRegistryPath, REG_CLIENT_PATH_VALUE,
		NULL, pGameInfo->s_szGameDirectory, MAX_PATH ) == FALSE )	// 게임이 설치되있나 체크
	{
		if( REG::RegReadString( HKEY_LOCAL_MACHINE, pGameInfo->s_szRegistryPath, REG_CLIENT_PATH_VALUE,
			NULL, pGameInfo->s_szGameDirectory, MAX_PATH ) == FALSE )	// 게임이 설치되있나 체크
		{				
			m_eGameState = eINSTALL;	// 인스톨 상태로 설정
			m_strGameState->ChangeStr( STR_IN_READY_INSTALL );	// "설치준비중" 문자열 출력
			return;
		}	
	}

	// 게임이 설치되어있는경우
	CString strExecuteFile;
	strExecuteFile += pGameInfo->s_szGameDirectory;
	strExecuteFile += L"/";
	strExecuteFile += pGameInfo->s_szGameFileName;
	if( PathFileExists( strExecuteFile ) == 0 )
	{
		MyMessageBox( STR_MESSAGE_NO_EXE );
		m_eGameState = eINSTALL;
		m_strGameState->ChangeStr( STR_IN_READY_INSTALL );
		return;
	}
	
	// 게임이 설치되어 있는 폴더 저장
	TCHAR szVERSIONINFO_FILE[ 256 ] = { 0, };
	TCHAR pBuf[1000];		

	_tcscpy_s( pBuf, 1000, L"" );
	_tcscat_s( szVERSIONINFO_FILE, 256, pGameInfo->s_szGameDirectory );
	_tcscat_s( szVERSIONINFO_FILE, 256, pGameInfo->s_szPatchInfo );		
	GetPrivateProfileString( L"VERSION", L"version", NULL, pBuf, sizeof(pBuf), szVERSIONINFO_FILE );		

	CString strBuf;
	strBuf.Format(L"%s", pBuf);
	m_nClientVersion = _ttoi( strBuf );		

	_tcscpy_s( m_szOldClientVersion, 32, pBuf );

	// 버튼 상태 설정
	_CheckRegestry();
		
	CString sClientVersion;
	sClientVersion.Format(STR_CLIENT_PATH_NUMBER, m_nClientVersion);
	m_strClientVersion->ChangeStr(sClientVersion.GetBuffer());
}

void CLauncherDlg::_VersionCheck()
{	
	m_nServerVersion = m_launcherInfos.GetPatchVersion();
	// 서버에서 받는 버젼 정보 실패
 	if( m_nServerVersion <= 0 )
 		Quit( STR_FAIL_PATCHINFO );
}
// 
// bool CLauncherDlg::_IsInstaller()
// {	
// 	// URL (파일 주소 연결)
// 	CString strURL;
// 
// 	// 임시적으로 처리한 구문	이 파일이 있으면 다이얼로그를 띄운다.
// 	if(g_GameInfo->IsUserType(_T("IMBC")))
// 		strURL += L"http://digimonmasters.nowcdn.co.kr/IMBCLauncher/InstallerDown.txt";
// 	else if(g_GameInfo->IsUserType(_T("MGAME")))
// 		strURL += L"http://digimonmasters.nowcdn.co.kr/MGAMELauncher/InstallerDown.txt";
// 	else
// 		strURL += L"http://digimonmasters.nowcdn.co.kr/Launcher/InstallerDown.txt";
// 
// 	HINTERNET hHttpFile = InternetOpenUrl( m_hSession, strURL, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
// 	if( hHttpFile == NULL )
// 	{			
// 		InternetCloseHandle( hHttpFile );		
// 		return false;
// 	}
// 
// 	// 연결 정보 확인
// 	TCHAR szStatusCode[ 128 ];
// 	DWORD dwCodeSize = 128;
// 	HttpQueryInfo( hHttpFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwCodeSize, NULL );
// 	long nStatusCode = _ttol( szStatusCode );
// 	if( nStatusCode != HTTP_STATUS_OK )
// 	{						
// 		InternetCloseHandle( hHttpFile );		
// 		return false;
// 	}	
// 
// 	InternetCloseHandle( hHttpFile );			
// 	Sleep( 100 );	
// 
// 	return true;
// }

// void CLauncherDlg::_SetStartPage()
// {
// 	CString PatchInfo;
// 	PatchInfo += m_strCurrentPath;
// 	PatchInfo += CHECK_BOX;
// 
// 	//ini값 셋팅
// 	TCHAR sz[2];
// 	if( m_bStartPage == true )
// 	{
// 		_tcscpy_s( sz, 2, L"1" );
// 
// 		size_t nLen = _tcslen( g_GameInfo->GetGameInfo()->s_szHomePageUrl );
// 		if( nLen > 0 )
// 			REG::RegWriteString( HKEY_CURRENT_USER, L"Software\\Microsoft\\Internet Explorer\\Main", L"Start Page", g_GameInfo->GetGameInfo()->s_szHomePageUrl );
// 		else 
// 			return;
// 	}
// 	else
// 		_tcscpy_s( sz, 2, L"0" );
// 
// 	WritePrivateProfileString( L"STARTPAGE", L"value", sz, PatchInfo );	
// }

bool CLauncherDlg::_PlayGame( TCHAR* cExe, TCHAR* cCommand )
{
// 	if( m_pCheckbox )
// 		_SetStartPage();	

	SHELLEXECUTEINFO si;                             // SHELLEXECUTEINFO 구조체를 하나 만든다
	ZeroMemory(&si, sizeof(SHELLEXECUTEINFO));       // si 구조체 초기화 
	si.cbSize = sizeof(SHELLEXECUTEINFO);            // 크기 설정

	si.fMask = SEE_MASK_NOCLOSEPROCESS;              // ??
	si.lpVerb = L"runas";                             // 동작 형식(파일열기)
	si.lpFile = cExe;							     // szPath;  // 실행할 파일 경로 지정
	si.lpParameters = cCommand;						 // szArg;    // 인자값 지정	
	si.nShow = SW_SHOWNORMAL;                        // 표시 형식	

	if( ShellExecuteEx(&si) == FALSE )               // 외부 프로그램 호출
	{
		DWORD dwErr = GetLastError();
		switch( dwErr )
		{				
		case ERROR_FILE_NOT_FOUND:		Quit( STR_PLAYGAME_ERROR_FILE_NOT_FOUND );		break;
		case ERROR_PATH_NOT_FOUND:		Quit( STR_PLAYGAME_ERROR_PATH_NOT_FOUND );		break;		
		case ERROR_NOT_ENOUGH_MEMORY:	Quit( STR_PLAYGAME_ERROR_NOT_ENOUGH_MEMORY );	break;		
		case ERROR_BAD_EXE_FORMAT:		Quit( STR_PLAYGAME_ERROR_BAD_EXE_FORMAT );		break;
		default:
			{
				TCHAR eff[ 128 ];
				_stprintf_s( eff, 128, STR_PLAYGAME_ERROR_DEFAULT, dwErr );
				MyMessageBox( eff );
			}
			break;
		}
		return false;
	}	
	return true;
}



void CLauncherDlg::UpdateVersion()
{
	m_nClientVersion++;

	TCHAR szVERSIONINFO_FILE[ 256 ] = { 0, };
	TCHAR pBuf[1000];		

	_tcscpy_s( pBuf, 1000, L"" );
	_tcscat_s( szVERSIONINFO_FILE, 256, g_GameInfo->GetGameInfo()->s_szGameDirectory );
	_tcscat_s( szVERSIONINFO_FILE, 256, g_GameInfo->GetGameInfo()->s_szPatchInfo );
	
	TCHAR szVer[ 8 ];
	_itot_s( m_nClientVersion, szVer, 10 );	

	WritePrivateProfileString( L"VERSION", L"version", szVer, szVERSIONINFO_FILE );

	CString sClientVersion;
	sClientVersion.Format(STR_CLIENT_PATH_NUMBER, m_nClientVersion);
	m_strClientVersion->ChangeStr(sClientVersion.GetBuffer());
}

void CLauncherDlg::ReturnVersion()
{
	TCHAR szVERSIONINFO_FILE[ 256 ] = { 0, };
	TCHAR pBuf[1000];		

	_tcscpy_s( pBuf, 1000, L"" );
	_tcscat_s( szVERSIONINFO_FILE, 256, g_GameInfo->GetGameInfo()->s_szGameDirectory );
	_tcscat_s( szVERSIONINFO_FILE, 256, g_GameInfo->GetGameInfo()->s_szPatchInfo );

	WritePrivateProfileString( L"VERSION", L"version", m_szOldClientVersion, szVERSIONINFO_FILE );
}

void CLauncherDlg::MainButtonEnable( bool bEnable )
{
	switch( m_eGameState )
	{
	case eINSTALL:
		if( bEnable == true )
			m_pInstall->SetButton( eMOUSE_DEFAULT );
		else
			m_pInstall->SetButton( eMOUSE_DISABLE );
		break;
	case eUPDATE:
	case ePACK:
		if( bEnable == true )
			m_pUpdate->SetButton( eMOUSE_DEFAULT );
		else
			m_pUpdate->SetButton( eMOUSE_DISABLE );		
		break;
	case eRUN:	
		if( bEnable == true )
			m_pRun->SetButton( eMOUSE_DEFAULT );
		else
			m_pRun->SetButton( eMOUSE_DISABLE );		
		break;
	default:
		Quit( STR_ERROR );		
		break;
	}	
}

bool CLauncherDlg::IsInstllEnd()
{	
	if( REG::IsRegstry( HKEY_CURRENT_USER, g_GameInfo->GetGameInfo()->s_szRegistryPath ) == FALSE )
	{
		if( REG::IsRegstry( HKEY_LOCAL_MACHINE, g_GameInfo->GetGameInfo()->s_szRegistryPath ) == FALSE )
		{
			return false;
		}
	}

	return true;
}

void CLauncherDlg::Install_End()
{
	// 메인 버튼 다시 활성화
	MainButtonEnable( true );	

	m_strGameFile->ChangeStr( STR_IN_SUCCESS_INSTALL );

	g_pThread->GetProc()->ResetCurInfo();
	g_pThread->GetProc()->ResetTotalInfo();

	// 버튼 상태 설정
	InitVersion();
}

void CLauncherDlg::Update_End()
{
	m_eGameState = ePACK;
	GoThread();
}

void CLauncherDlg::Pack_End()
{
	// 메인 버튼 다시 활성화
	MainButtonEnable( true );	

	m_strGameFile->ChangeStr( STR_IN_SUCCESS_UPDATE );
	g_pThread->GetProc()->ResetCurInfo();
	g_pThread->GetProc()->ResetTotalInfo();

	// 버튼 상태 설정
	_CheckRegestry();
}

/*---------------------------------------------------------------------

						파일 다운로드 초기과정 	

----------------------------------------------------------------------*/
void CLauncherDlg::DeleteWaitQueue()
{
	sTHREAD_INPUT* pData;
	while( m_WaitQueue.Get( pData ) )
	{
		SAFE_DELETE( pData );
	}	
}

void CLauncherDlg::GoThread()
{
	MainButtonEnable( false );

	g_pThread->GetProc()->ResetCurInfo();
	g_pThread->GetProc()->ResetTotalInfo();

	switch( m_eGameState )
	{
	case eINSTALL:
		{
#ifdef USE_DOWNENGINE
 			CString strDll;
 			strDll += m_strCurrentPath;
 			strDll += DE_DLL_FILE;			
 
 			int nRes = m_DownEngine.Create( strDll );
 			if( nRes != 0 )
 			{
 				TCHAR sz[ 128 ];
 				_stprintf_s( sz, 128, L"%s : %d", STR_FAIL_DOWNSDK, nRes );
 				Quit( sz );
 			}	
#endif
// 			TCHAR szExecute[ 256 ] = {0, };
// 			if( _Findfolder( szExecute ) == false )
// 			{
// 				MainButtonEnable( true );
// 				return;
// 			}

			TCHAR szCurDir[MAX_PATH] = { 0, };
			GetCurrentDirectory(MAX_PATH, szCurDir);

			_stprintf_s( m_szFullDown, 256, L"%s/%s", szCurDir, g_GameInfo->GetGameInfo()->s_szInstallerName );
			if( PathFileExists( m_szFullDown ) != 0 )
			{	// 설치 파일이 존재하면 설치 시작
#ifdef USE_DOWNENGINE
				ShellExecute(NULL, L"runas", m_szFullDown, NULL, NULL, SW_SHOWNORMAL );

				g_pThread->GetProc()->SetCurTotalSize( 100 );
				g_pThread->GetProc()->SetCurProcSize( 100 );
				g_pThread->GetProc()->SetTotalTotalSize( 100 );
				g_pThread->GetProc()->SetTotalProcSize( 100 );

				sTHREAD_INPUT* pData = new sTHREAD_INPUT;
				pData->s_eType = sTHREAD_INPUT::INSTALL;	
				g_pThread->GetProc()->Add(pData);
				break;
#else
				DeleteFile(m_szFullDown);
				//_FullClientDownLoadComplete();
				//break;
#endif
			}
#ifdef USE_DOWNENGINE
			//없을 경우 서버에서 다운로드
			m_DownEngine.Begin( strDll );			
			m_DownEngine.SetParentWindow((void*)this);			
			// 콜백 함수 연결			
			m_DownEngine.SetNotifyDownStartCallBack(CLauncherDlg::CallBack_File_OnDownloadStart);			
			m_DownEngine.SetNotifyDownCompleteCallBack(CLauncherDlg::CallBack_File_OnDownloadComplete);			
			m_DownEngine.SetDrawFileInfoCallback(CLauncherDlg::CallBack_File_OnFileInfo);					

			CString strPath;
			nsCSFILE::CreateDirectory( szExecute );
			strPath.Format( _T( "%s/%s" ), szExecute, g_GameInfo->GetGameInfo()->s_szInstallerName );
	
			CString strURL;
			strURL.Format( _T( "%s/%s" ), g_GameInfo->GetGameInfo()->s_szInstallServer, g_GameInfo->GetGameInfo()->s_szInstallerName  );			

			m_DownEngine.SetDownloadFileInfo( strPath, strURL );
			// 오토 패치 스타트
			if( m_DownEngine.AutoPatchStart() != 0 )
			{
				m_DownEngine.Close();

				TCHAR sz[ 256 ];
				_stprintf_s( sz, 256, STR_FAIL_CONNECT_FILE_SERVER, g_GameInfo->GetGameInfo()->s_szInstallerName );
				Quit( sz ); 												
			}	
#else
			DWORD dwSize = 0;
			if( !CheckFile( g_GameInfo->GetGameInfo()->s_szInstallerName, dwSize, 0,sTHREAD_INPUT::INSTALL ) )
			{	
				Quit( STR_FAIL_PATCHINFO_OLD );
				DeleteWaitQueue();
				break;
			}

			// 쓰레드 작동
			g_pThread->GetProc()->SetTotalTotalSize( dwSize );	
			sTHREAD_INPUT* pData;
			while( m_WaitQueue.Get( pData ) )
			{
				g_pThread->GetProc()->Add( pData );
			}
#endif
			m_strGameState->ChangeStr( STR_IN_DOWN );
			m_strGameFile->ChangeStr( g_GameInfo->GetGameInfo()->s_szInstallerName );
		}
		break;
	case eUPDATE:
		{		
			TCHAR szUpdateFile[ 128 ];
			DWORD dwTotalSize = 0;			
			int nCheckVer = 0;

			if( m_nClientVersion == 0 )
			{
				MyMessageBox( STR_FAIL_PATCHINFO_CLIENT );
				break;
			}
			else if( m_nServerVersion < m_nClientVersion ) // 클라이언트 버전이 서버 버젼보다 높을 경우
			{
				MyMessageBox( STR_FAIL_PATCHINFO_CLIENT );
				break;
			}

			// 업데이트할 파일 체크
			for( int i = 1; i <= m_nServerVersion - m_nClientVersion; i++ )
			{				
				DWORD dwSize = 0;				
				_stprintf_s( szUpdateFile, 128, L"%d.zip", m_nClientVersion + i );
				if( CheckFile( szUpdateFile, dwSize, m_nClientVersion + i, sTHREAD_INPUT::UPDATE ) == true )
				{	
					dwTotalSize += dwSize;					
					nCheckVer++;
				}
				else
				{
					Quit( STR_FAIL_PATCHINFO_OLD );
					DeleteWaitQueue();
					break;
				}
			}
			
			// 쓰레드 작동
			g_pThread->GetProc()->SetTotalTotalSize( dwTotalSize );			
			sTHREAD_INPUT* pData;
			while( m_WaitQueue.Get( pData ) )
			{
				g_pThread->GetProc()->Add( pData );
			}						
		}
		break;
	case ePACK:
		{
			sTHREAD_INPUT* pData = new sTHREAD_INPUT;
			pData->s_eType = sTHREAD_INPUT::DO_PACK;
			g_pThread->GetProc()->Add( pData );			
		}
		break;
	default:
		Quit( STR_ERROR );
		break;
	}
}

bool CLauncherDlg::CheckFile( TCHAR* szFileName, DWORD& dwSize, DWORD dwPatchNum, sTHREAD_INPUT::eTYPE eThread )
{
	CString szURL;
	switch( eThread )
	{
	case sTHREAD_INPUT::INSTALL:szURL.Format( _T( "%s%s" ), g_GameInfo->GetGameInfo()->s_szInstallServer ,szFileName );	break;
	case sTHREAD_INPUT::UPDATE:	szURL.Format( _T( "%s%s" ), g_GameInfo->GetGameInfo()->s_szServerName ,szFileName );	break;
	}	


	// URL (파일 주소 연결)	
	HINTERNET hHttpFile = InternetOpenUrl( m_hSession, szURL, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
	if( hHttpFile == NULL )
	{			
		InternetCloseHandle( hHttpFile );		
		Quit( STR_FAIL_CONNECT_INTERNET );
		return false;
	}

	TCHAR sz[ 256 ] = {0, };

	// 연결 정보 확인
	TCHAR szStatusCode[ 128 ];
	DWORD dwCodeSize = 128;
	HttpQueryInfo( hHttpFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwCodeSize, NULL );
	long nStatusCode = _ttol( szStatusCode );
	if( nStatusCode != HTTP_STATUS_OK )
	{	
		_stprintf_s( sz, 256, STR_FAIL_CONNECT_SERVER, szFileName, nStatusCode );
		Quit( sz );

		InternetCloseHandle( hHttpFile );
		return false;
	}

	// 파일 용량 확인
	TCHAR bufQuery[ 32 ];
	DWORD dwLengthBufQuery = sizeof(bufQuery);
	BOOL bConnect = ::HttpQueryInfo( hHttpFile, HTTP_QUERY_CONTENT_LENGTH, bufQuery, &dwLengthBufQuery, NULL);

	DWORD dwTotalSize = _ttol( bufQuery );	
	if( dwTotalSize == 0 )
	{			
		_stprintf_s( sz, 256, STR_FAIL_CONNECT_FILE_SERVER, szURL.GetBuffer() );
		Quit( sz );

		InternetCloseHandle( hHttpFile );
		return false;	
	}
	
	dwSize += dwTotalSize;
	InternetCloseHandle( hHttpFile );	
	
	// 다운로드 대기큐에 넣기
	sTHREAD_INPUT* pTdata = new sTHREAD_INPUT;
	pTdata->s_eType = eThread;
	pTdata->s_dwSize = dwTotalSize;
	switch( eThread )
	{
	case sTHREAD_INPUT::INSTALL:
		_tcscpy_s( pTdata->s_szURLPath, 512, g_GameInfo->GetGameInfo()->s_szInstallServer );
		break;
	case sTHREAD_INPUT::UPDATE:
		_tcscpy_s( pTdata->s_szURLPath, 512, g_GameInfo->GetGameInfo()->s_szServerName );
		break;
	}	
	_tcscpy_s( pTdata->s_szDownFile, 512, szFileName );
	pTdata->s_dwPatchNum = dwPatchNum;

	m_WaitQueue.Put( pTdata );

	return true;
}


///////////////////////////////////////////////////////////////////////////
//
//		추가 기능 함수들..
//
///////////////////////////////////////////////////////////////////////////

bool CLauncherDlg::_CheckExeFile( TCHAR* szExeName )
{
	m_strGameState->ChangeStr( STR_IN_FILE_CHECK );

	// 버전정보를 담을 버퍼
	TCHAR szFileVer[ 128 ];
	TCHAR* buffer = NULL;
	DWORD infoSize = 0;

	// 파일로부터 버전정보데이터의 크기가 얼마인지를 구합니다.
	infoSize = GetFileVersionInfoSize(szExeName, 0);
	if(infoSize==0) 
		return false;

	// 버퍼할당
	buffer = new TCHAR[infoSize];
	if(buffer)
	{
		// 버전정보데이터를 가져옵니다.
		if(GetFileVersionInfo(szExeName,0,infoSize, buffer)!=0)
		{
			VS_FIXEDFILEINFO* pFineInfo = NULL;
			UINT bufLen = 0;
			// buffer로 부터 VS_FIXEDFILEINFO 정보를 가져옵니다.
			if(VerQueryValue(buffer,L"\\",(LPVOID*)&pFineInfo, &bufLen) !=0)
			{    
				WORD majorVer, minorVer, buildNum, revisionNum;
				majorVer = HIWORD(pFineInfo->dwFileVersionMS);
				minorVer = LOWORD(pFineInfo->dwFileVersionMS);
				buildNum = HIWORD(pFineInfo->dwFileVersionLS);
				revisionNum = LOWORD(pFineInfo->dwFileVersionLS);

				// 파일버전 출력
				_stprintf_s( szFileVer, 128, L"%d,%d,%d,%d", majorVer, minorVer, buildNum, revisionNum );
			}
		}
		delete[] buffer;
	}
	
	return true;
}

bool CLauncherDlg::_IsExecute()
{	
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	PROCESSENTRY32 pe32; 
	pe32.dwSize = sizeof( PROCESSENTRY32 ); 

	Process32First( hProcessSnap, &pe32 ); 
	do
	{ 
		TCHAR process[255]; 
		_stprintf_s( process, 255, L"%s %5d", pe32.szExeFile, pe32.th32ProcessID ); 		

		if( _tcsicmp( pe32.szExeFile, g_GameInfo->GetGameInfo()->s_szGameFileName ) == 0 )
		{					
			CloseHandle(hProcessSnap);
			return true;
		}										

	} while( Process32Next( hProcessSnap, &pe32 ) ); 					

	CloseHandle(hProcessSnap);	

	return false;
}

bool CLauncherDlg::_Findfolder( TCHAR* sz )
{
	TCHAR szExecute[ 256 ];
	GetWindowsDirectory( szExecute, 256 );
	memset( &szExecute[3], 0, sizeof( TCHAR ) * (256-3) );

	TCHAR Buffer[ 256 ] = { 0, };
	TCHAR lpszPath[ 256 ] = { 0, };			

	LPITEMIDLIST    pDirList;
	BROWSEINFO      browseInfo;
	browseInfo.hwndOwner = m_hWnd; 
	browseInfo.pidlRoot = NULL;
	browseInfo.lpszTitle = STR_IN_GAME_PATH;
	browseInfo.pszDisplayName = Buffer;
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_USENEWUI;			
	browseInfo.lParam = (LPARAM)(LPCTSTR)szExecute;			
	browseInfo.lpfn = BrowseForFolder_CallbackProc;

	// SHBrowseForFolder()는 윈도우즈 시스템이 기본적으로 제공하는 
	// 디렉토리 설정 다이얼로그를 출력한다.
	pDirList = SHBrowseForFolder( &browseInfo );

	if(pDirList != NULL)
	{
		//사용자가 선택한 경로를 buffer에 저장
		SHGetPathFromIDList( pDirList , Buffer );
		_tcscat_s( lpszPath, 256, Buffer );  // 새로 선택한 폴더명 저장	

		int nlen = (int)_tcslen(lpszPath);		
		_tcscat_s( sz, 256, lpszPath );		

		return true;
	}

	return false;
}

bool CLauncherDlg::PassServerCheck()	// 서버 점검중일 때도 패치가 가능한지 체크함.
{
	TCHAR szPassFile[ MAX_PATH ] = {0,};
	_stprintf_s( szPassFile, MAX_PATH, L"%s%s", m_strCurrentPath.GetBuffer(), PASS_SERVER_CHECK );

	if( PathFileExists( szPassFile ) )
		return true;

	return false;
}

void CLauncherDlg::_FullClientDownLoadComplete()
{
	m_bDownStart = false;	
	m_strGameState->ChangeStr(STR_IN_INSTALL);
	ShellExecute(NULL, L"runas", m_szFullDown, NULL, NULL, SW_SHOWNORMAL);

	g_pThread->GetProc()->SetCurTotalSize( 100 );
	g_pThread->GetProc()->SetCurProcSize( 100 );
	g_pThread->GetProc()->SetTotalTotalSize( 100 );
	g_pThread->GetProc()->SetTotalProcSize( 100 );

	sTHREAD_INPUT* pData = new sTHREAD_INPUT;
	pData->s_eType = sTHREAD_INPUT::DOWNLOADED_INSTALL;	
	g_pThread->GetProc()->Add( pData );
}

void CLauncherDlg::Quit( TCHAR* sz )
{ 
	if( m_bQuit == true ) 
		return; 

	m_bQuit = true;  
	m_dlgMessageBox.Init( sz, CMyMessageBox::MT_QUIT ); 
	m_dlgMessageBox.ShowWindow( SW_SHOW ); 
}

void CLauncherDlg::MyMessageBox( TCHAR* sz, CMyMessageBox::eMessageType eMT )
{ 
	m_dlgMessageBox.Init( sz, eMT ); 
	m_dlgMessageBox.ShowWindow( SW_SHOW ); 
}

bool CLauncherDlg::_IsServerMaintenance()
{
	return !m_launcherInfos.IsServerEnable();
}

void CLauncherDlg::ShowMaintenanceWindow()
{
	m_dlgSeverCheck.Init( m_launcherInfos.GetMsgTitle(), m_launcherInfos.GetMsg() );
	m_dlgSeverCheck.ShowWindow( SW_SHOW );
}

BOOL CLauncherDlg::_IsRedistInstalled()
{
	std::list<CString> const& checkList = m_launcherInfos.GetRegCheckList();
	for (auto const& it : checkList)
	{
		// 설치기 되어 있으면 게임 실행
		if (REG::IsRegstry(HKEY_LOCAL_MACHINE, it.operator LPCTSTR()))
			return TRUE;
	}
	return FALSE;
}

BOOL CLauncherDlg::_CheckRedist2015(TCHAR const* pPath)
{
	if (NULL == pPath)
		return FALSE;

	// 재배포 패키지 설치 여부 체크 안할 경우 게임 실행
	if (!m_launcherInfos.IsRegCheckEnable())
		return TRUE;

	if (_IsRedistInstalled())
		return TRUE;
	
	CString filePath = pPath;
	filePath += m_launcherInfos.GetRegExecute();
	// 재배포 패키지 파일이 있는지 검사
	if (0 != _taccess_s(filePath, 0))
	{
		MyMessageBox(STR_FAIL_RUN_EXEFILE);
		return FALSE;
	}

	// 설치가 안되어 있으면 설치 
	SHELLEXECUTEINFO sei;
	::ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
	sei.lpDirectory = pPath;
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.lpFile = m_launcherInfos.GetRegExecute();
	sei.nShow = SW_SHOW;
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = L"open";
	sei.lpParameters = m_launcherInfos.GetRegExecuteCommand();

	DWORD dwResult = ::ShellExecuteEx(&sei);
	if (sei.hProcess != NULL)
		::WaitForSingleObject(sei.hProcess, INFINITE);// 설치가 끝날 때까지 대기

	return _IsRedistInstalled();	
}