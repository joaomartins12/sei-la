
#include "stdafx.h"
#include "AutoUpgradeDlg.h"
#include "Option.h"

#include "WindowVersionChecker.h"
#include "UseRegistry.h"
#include "io.h"

bool g_bManualLauncherUpgrade = false;

// CAutoUpgradeDlg 대화 상자

//쓰레드로 돌릴 전역함수
UINT StartUnzip(LPVOID pParam);

//================================================================================================================
//
//		초기화
//
//================================================================================================================

//=====================================================================
// CLoginDlg 끝
//=====================================================================
void CAutoUpgradeDlg::_Init_Window()
{
	SetWindowText( STR_WINDOW_TITLE );

	// 삭제할 런처 삭제
	_tremove( DEL_LAUNCHERFILENAME );

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

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
}

void CAutoUpgradeDlg::_Init_Control()
{
	CRect rect;	
	//-------------------------------------------------------------------
	// Bitmap Button - 런처에 사용되는 Button 설정
	//-------------------------------------------------------------------
	m_bmpStart.SetParent( this );
	m_bmpStart.SetButtonKind( BMP_BUTTON_START );
	m_bmpStart.LoadButtonBitmaps(IDB_START_DEF_TH, IDB_START_ON_TH, IDB_START_DOWN_TH, IDB_START_NO_TH);

	m_bmpCancle.SetParent( this );
	m_bmpCancle.SetButtonKind( BMP_BUTTON_CANCEL );
	m_bmpCancle.LoadButtonBitmaps(IDB_CANClE_DEF_TH, IDB_CANCLE_ON_TH, IDB_CANCLE_DOWN_TH);


	// 게임시작 버튼 영역
	rect.left = 58;
	rect.top = 420;
	rect.right = rect.left + 97;
	rect.bottom = rect.top + 60;	

	m_bmpStart.MoveWindow(&rect);
	m_bmpStart.SetButtonEable( false );

	// 닫기
	rect.left = 715;
	rect.top = 7;
	rect.right = rect.left + 17;
	rect.bottom = rect.top + 17;
	m_bmpCancle.MoveWindow(&rect);


	//홈 계시판
	rect.left = 0; //24;
	rect.top = 26; // 58;	
	rect.right = 742;
	rect.bottom = rect.top + 390;

	GetDlgItem(IDC_WEBVIEW)->SetWindowPos(NULL,rect.left,rect.top, rect.right - rect.left, rect.bottom - rect.top, SW_SHOW);

	m_pHtmlView = new CMyHtmlView;	
	DWORD dwPlag = WS_CHILD | WS_VISIBLE;
	m_pHtmlView->Create(NULL, L"", dwPlag , rect, this, IDC_WEBVIEW ) ;

	int htmlViewBorder = 2;
	m_pHtmlView->SetLeft( -htmlViewBorder );
	m_pHtmlView->SetTop( -htmlViewBorder );
	m_pHtmlView->SetWidth( m_pHtmlView->GetWidth() + htmlViewBorder );
	m_pHtmlView->SetHeight( m_pHtmlView->GetHeight() + htmlViewBorder );
	m_pHtmlView->Navigate( BANER_URL );//원본
	m_pHtmlView->OnInitialUpdate();

	//-------------------------------------------------------------------
	// Prograss Bar
	//-------------------------------------------------------------------
	m_ctrlProgressMain.SubclassDlgItem(IDB_PRONOW,this);
	m_ctrlProgressMain.SetBitmap(IDB_PRONOW);
	m_ctrlProgressMain.SetRange( 0, 1 );
	m_ctrlProgressMain.SetPos( 0 );

	rect.left = _ProgressMainX;
	rect.top = _ProgressMainY;
	rect.right = _ProgressMainX + _ProgressWidth;
	rect.bottom = _ProgressMainY + _ProgressHeight;
	m_ctrlProgressMain.MoveWindow( &rect );


	m_ctrlProgressSubDown.SubclassDlgItem(IDB_PRONOW2,this);
	m_ctrlProgressSubDown.SetBitmap(IDB_PRONOW2);
	m_ctrlProgressSubDown.SetRange( 0, 1 );
	m_ctrlProgressSubDown.SetPos( 0 );

	rect.left = _ProgressSubDownX;
	rect.top = _ProgressSubDownY;
	rect.right = _ProgressSubDownX + _ProgressWidth;
	rect.bottom = _ProgressSubDownY + _ProgressHeight;
	m_ctrlProgressSubDown.MoveWindow( &rect );


	m_ctrlProgressSubUnzip.SubclassDlgItem(IDB_PRONOW3,this);
	m_ctrlProgressSubUnzip.SetBitmap( IDB_PRONOW3 );
	m_ctrlProgressSubUnzip.SetRange( 0, 1 );
	m_ctrlProgressSubUnzip.SetPos( 0 );

	rect.left = _ProgressSubUnzipX;
	rect.top = _ProgressSubUnzipY;
	rect.right = _ProgressSubUnzipX + _ProgressWidth;
	rect.bottom = _ProgressSubUnzipY + _ProgressHeight;
	m_ctrlProgressSubUnzip.MoveWindow( &rect );

	m_bmpOption.SetParent( this );
	m_bmpOption.SetButtonKind( BMP_BUTTON_OPTION );
	m_bmpOption.LoadButtonBitmaps(IDB_TH_OPTION_N, IDB_TH_OPTION_O, IDB_TH_OPTION_C, IDB_TH_OPTION_D);

	rect.left = 655;
	rect.top = 4;
	rect.right = rect.left + 60;
	rect.bottom = rect.top + 20;
	m_bmpOption.MoveWindow(&rect);

	_ResetButton();
	ResetError();
}

void CAutoUpgradeDlg::_Init_Member()
{
	m_nDownVersion = 0;
	m_nTotalFileCount = 0;
	m_nDownCompFileCount = 0;
	m_bAlreadyStart = false;
	m_bClickTitle = false;
}

bool CAutoUpgradeDlg::_Init_Version()
{
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_VersionFile.Init(VERSIONINFO_FILE);
	m_strClientVersion = m_VersionFile.GetString( L"VERSION", L"version" );
	//m_strOriginVersion = m_VersionFile.GetString( L"VERSION", L"version" );
	if(m_strClientVersion == INI_READ_DEFAULT_STRING )
	{
		// 기존 방식으로 만들었던거 제거
		DeleteFile( OLD_VERSIONINFO_FILE );
		m_strClientVersion = "0";
	}

	m_nLastPatchNum =  _ttoi( m_strClientVersion );

	// 버젼 최하 값 검사 - 패치정보를 영원토록 보관 할 수 없으니. 일정시간 되면 제거 - 그에 따른 그전 버젼 유저는 패치 불가
	// 자주 변경되지 않으니 런쳐에 define 으로 정의해 놓겠다.
	if( _ttoi( m_strClientVersion ) < PATCH_MIN_VERSION )
	{
		if( MessageBox( STR_FAIL_OLD_VERSION, STR_FAIL_VERSION, MB_OKCANCEL ) == IDOK )
		{
			ShellExecuteA(NULL, "open", "explorer.exe", HOME_DOWN_SITE, NULL, SW_SHOW);			
		}
		return false;
	}
	return true;
}

bool CAutoUpgradeDlg::_Init_Down()
{
	m_hSession = InternetOpenA("TDMO", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (m_hSession == NULL) 
	{
		MessageBox( STR_FAIL_CONNECT_INTERNET, STR_FAIL, MB_OK );
		PostMessage( WM_QUIT );
	}

	g_pThread->Get()->SetHInternet( &m_hSession );

	if (m_hSession == NULL) 
	{
		MessageBox( STR_FAIL_CONNECT_INTERNET, STR_FAIL, MB_OK );
		PostMessage( WM_QUIT );
	}

	g_pThread->Get()->SetHInternet( &m_hSession );

	return _ServerVersionFile();
}

//================================================================================================================
//
//		제거
//
//================================================================================================================

void CAutoUpgradeDlg::_DeleteDlg()
{
	KillTimer( ID_TIMER_URL_STATE );
	KillTimer( ID_TIMER_DO_PACK );
	KillTimer( ID_TIMER_DOWNLOAD );
	KillTimer( ID_TIMER_UNZIP );	

	if( _taccess_s( NEW_EXE_JUST_TEMP, 0 ) == 0 )
	{
		_trename( EXE_LAUNCHERFILENAME, DEL_LAUNCHERFILENAME );
		_trename( NEW_EXE_JUST_TEMP, EXE_LAUNCHERFILENAME );
	}
	else if( _taccess_s( NEW_EXE_TEMP, 0 ) == 0 )
	{
		_trename( EXE_LAUNCHERFILENAME, DEL_LAUNCHERFILENAME );
		_trename( NEW_EXE_TEMP, EXE_LAUNCHERFILENAME );
	}

	Sleep(100);
	
	CThread::GlobalShotDown();
	if( m_hSession != NULL )
	{
		InternetCloseHandle( m_hSession );
		m_hSession = NULL;
	}

	DeleteFileA((LPCSTR)UPGRADEINFO_FILE);
}


//================================================================================================================
//
//		정보
//
//================================================================================================================

bool CAutoUpgradeDlg::_ServerVersionFile()
{
	CString strDownPathPatchInfo = GetWorkPath() + UPGRADEINFO_FILE;
	CString strServerNamePathPatchInfo;
	strServerNamePathPatchInfo  = SERVER_NAME;
	strServerNamePathPatchInfo += UPGRADEINFO_FILE;

	sTHREAD_INPUT ti;
	ti.s_eType = sTHREAD_INPUT::URL_DOWN;
	ti.s_nVersion = sTHREAD_INPUT::VU_VERSION;		// 버젼인포 파일
	ti.s_szURLPath = strServerNamePathPatchInfo;
	ti.s_szDownPath = strDownPathPatchInfo;

	g_pThread->Add( &ti );
	
	return true;
}

bool CAutoUpgradeDlg::_GameCheck()
{
	memset( m_szCheck, 0, sizeof( m_szCheck ) );

	// URL (파일 주소 연결)
	CString strURL; 
	strURL += SERVER_NAME; //원본

	strURL += CHECK_FILE;
	HINTERNET hHttpFile = InternetOpenUrl( m_hSession, strURL, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
	if( hHttpFile == NULL )
	{			
		InternetCloseHandle( hHttpFile );		
		return true;
	}

	// 연결 정보 확인
	TCHAR szStatusCode[ 128 ];
	DWORD dwCodeSize = 128;
	HttpQueryInfo( hHttpFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwCodeSize, NULL );
	long nStatusCode = _ttol( szStatusCode );
	if( nStatusCode != HTTP_STATUS_OK )
	{						
		InternetCloseHandle( hHttpFile );		
		return true;
	}

	// 파일 용량 확인
	char sz[ 512 ] = { 0, };
	DWORD dwSize;
	DWORD dwRead = 512;
	InternetQueryDataAvailable( hHttpFile, &dwSize, 0, 0 );
	InternetReadFile( hHttpFile, sz, dwSize, &dwRead );
	sz[ dwRead ] = 0; 

	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, sz, -1, m_szCheck, 512 );
	//MultiByteToWideChar( CP_UTF8, 0, sz, -1, m_szCheck, 512 );	// check.txt 태국에서 적용할려면 해당 코드로 적용해야 함	lks007

	InternetCloseHandle( hHttpFile );			
	Sleep( 100 );	

	return false;
}

void CAutoUpgradeDlg::Set_FileInfo( ULONGLONG ullFileSize )
{
	_SetRange_SubDownProgress( (int)( ullFileSize/1000 ) );
	_SetPos_SubDownProgress( 0 );
	_SetPos_SubUnzipProgress( 0 );
}

bool CAutoUpgradeDlg::VersionCheck()
{	
	CIniFile ini;
	ini.Init( UPGRADEINFO_FILE );
	m_strServerVersion = ini.GetString( L"VERSION", L"version" );

	// 서버에서 받는 버젼 정보 실패
	if( m_strServerVersion == INI_READ_DEFAULT_STRING )
	{
		if( SetError( UNZ_FAIL_DOWN_VERSION ) == true )
		{
			if( _ServerVersionFile() == false )
			{
				PostMessage( WM_QUIT );
			}
		}
		else
		{
			DisplayError();
			PostMessage( WM_QUIT );
		}		
		return false;
	}

	int nCV = _ttoi( m_strClientVersion );
	int nSV = _ttoi( m_strServerVersion );

	DeleteFile( UPGRADEINFO_FILE );

	if( nCV > nSV )
	{
		MessageBox( STR_CVERSION_ERROR, STR_FAIL, MB_OK );
		CheckLauncherUpdate( true );
		PostMessage( WM_QUIT );
		return false;
	}
	m_nServerVersion = nSV;
	m_nTotalFileCount = nSV - nCV;
	// 패치 받을게 없다면
	if( m_nTotalFileCount == m_nDownCompFileCount )
	{
		_Comp_Download();
		return false;
	}

	_SetRange_MainProgress( m_nTotalFileCount );		

	return true;
}

void CAutoUpgradeDlg::NextFile()
{
	if( m_nTotalFileCount == m_nDownCompFileCount )
	{
		_Comp_Download();
		return;
	}

	//----------------------------------------------------------------------------------
	g_pDlg->SetState( eSTATE_FILE_DOWN );

	Proc_MainState();

	m_nDownVersion = _ttoi( m_strClientVersion ) + m_nDownCompFileCount + 1;

	CString strServerFile;
	strServerFile.Format( L"%s%s%d.zip", SERVER_NAME, FILE_WARD, m_nDownVersion );//원본

	CString strClient;
	strClient.Format( L"%s%s%d.zip", GetWorkPath(), FILE_WARD, m_nDownVersion );

	if( _taccess_s( strClient, 0 ) == 0 )
	{
		BOOL bSuccess = DeleteFile( strClient );
		if( bSuccess == FALSE )
		{
			MessageBox( STR_FAIL_DELETE_FILE, STR_FAIL, MB_OK );
			PostMessage( WM_QUIT );
			return;
		}
		Sleep( 200 );
	}
	g_pThread->Get()->SetVersion( U_WAIT );

	sTHREAD_INPUT ti;
	ti.s_eType = sTHREAD_INPUT::URL_DOWN;
	ti.s_nVersion = sTHREAD_INPUT::VU_DOWN;			// 다운로드 파일
	ti.s_szURLPath = strServerFile;
	ti.s_szDownPath = strClient;
	g_pThread->Add( &ti );
}

void CAutoUpgradeDlg::Proc_MainState()
{
	switch(m_eState)
	{
	case eSTATE_CONNECT:
		_SetPos_MainProgress( 0 );
		m_szMainState = STR_MS_CONNECT_SERVER;
		break;
	case eSTATE_VERSION_DOWN:
		_SetPos_MainProgress( 0 );
		m_szMainState = STR_MS_DOWN_VERSION;
		break;
	case eSTATE_VERSION_COMP:
		_SetPos_MainProgress( 1 );
		m_szMainState = STR_MS_COMP_VERSION;
		break;
	case eSTATE_FILE_DOWN:
		{
			_SetPos_MainProgress( m_nDownCompFileCount );
			m_szMainState.Format( L"%s [%d/%d]", STR_MS_UPDATING, m_nDownCompFileCount, m_nTotalFileCount );
		}		
		break;

	case eSTATE_DO_PACK:
		m_szMainState = STR_DO_PACK;
		break;

	case eSTATE_UPDATE_COMP:
		m_szMainState = STR_COMP_UPDATE;
		break;
	}

	// 랜더링 갱신
	CRect rect;
	rect.left		=	_MainState_Pos_x;
	rect.top		=	_MainState_Pos_y;
	rect.right		=	_MainState_Pos_x + _MainState_Size_x;
	rect.bottom		=	_MainState_Pos_y + _MainState_Size_y;
	InvalidateRect(&rect, TRUE);	//파일 경로
}

void CAutoUpgradeDlg::Proc_SubState()
{
	switch(m_eState)
	{
	case eSTATE_VERSION_DOWN:
		{
			if( m_nErrorCount == 0 )
				m_szSubState.Format( STR_DOWN_VERSION );
			else
				m_szSubState.Format( STR_DOWN_VERSION_RT, m_nErrorCount );
		}		
		break;
	case eSTATE_FILE_DOWN:
		{
			CThread_Data::sINFO info;
			g_pThread->Get()->GetInfo( &info );

			int nPos = (int)( info.s_nProcSize / 1000 );
			int nTPos = (int)( info.s_nTotalSize / 1000 );
			_SetPos_SubDownProgress( nPos );

			if( m_nErrorCount == 0 )
				m_szSubState.Format( STR_DOWN_PATCH, m_nDownVersion, nPos*0.001f, nTPos*0.001f );
			else
				m_szSubState.Format( STR_DOWN_PATCH_RT, m_nDownVersion, nPos*0.001f, nTPos*0.001f, m_nErrorCount );
		}
		break;

	case eSTATE_DO_PACK:
		{
			CThread_Data::sINFO info;
			g_pThread->Get()->GetInfo( &info );

			if( info.s_nTotalSize != 0 )
			{
				m_szSubState.Format( STR_DO_COPY, info.s_nProcSize, info.s_nTotalSize );
				_SetRange_SubUnzipProgress( info.s_nTotalSize );
				_SetPos_SubUnzipProgress( info.s_nProcSize );
			}
			else
			{
				m_szSubState = "";
			}

			switch( info.s_nVersion )
			{
			case PACK_SUCCESS:
				{
					m_szSubState = "";

					KillTimer( ID_TIMER_DO_PACK );
					Sleep( 200 );

					_Comp_Download();
				}
				break;
			case PACK_FAILED_OPENFILE:
				{
					m_szSubState = "";

					KillTimer( ID_TIMER_DO_PACK );

					SetError( info.s_nVersion );
					DisplayError();
					PostMessage( WM_QUIT );
				}
				break;
			}			
		}
		break;
	default:
		{
			//m_szSubState = "";
		}		
	}

	// 랜더링 갱신
	CRect rect;
	rect.left		=	_SubState_Pos_x;
	rect.top		=	_SubState_Pos_y;
	rect.right		=	_SubState_Pos_x + _SubState_Size_x;
	rect.bottom		=	_SubState_Pos_y + _SubState_Size_y;
	InvalidateRect(&rect, TRUE);
}

void CAutoUpgradeDlg::Proc_UnzipState()
{
	CThread_Data::sINFO info;
	g_pThread->Get()->GetInfo( &info );

	_SetRange_SubUnzipProgress( info.s_nTotalSize/100 );
	_SetPos_SubUnzipProgress( info.s_nProcSize/100 );

	TCHAR c[ MAX_PATH ] = { 0, };
	if( info.s_nVersion > 0 )
	{
		if( m_nErrorCount == 0 )
			_stprintf_s( c, MAX_PATH, STR_UNZIP, info.s_nVersion );
		else
			_stprintf_s( c, MAX_PATH, STR_UNZIP_RT, info.s_nVersion, m_nErrorCount );
	}
	else
	{
		_tcscpy_s( c, MAX_PATH, L"" );
	}

	if( _tcscmp( c, m_szSubState ) != 0 )
	{
		m_szSubState = c;
		// 랜더링 갱신
		CRect rect;
		rect.left		=	_SubState_Pos_x;
		rect.top		=	_SubState_Pos_y;
		rect.right		=	_SubState_Pos_x + _SubState_Size_x;
		rect.bottom		=	_SubState_Pos_y + _SubState_Size_y;
		InvalidateRect(&rect, TRUE);
	}
	
	// 언집 성공
	if( info.s_nVersion == UNZ_SUCCESS )
	{
		_SetRange_SubUnzipProgress( 100 );
		_SetPos_SubUnzipProgress( 100 );

		KillTimer( ID_TIMER_UNZIP );
		ResetError();
		Sleep( 200 );

		// 런처 업데이트 체크
		if( CheckLauncherUpdate( false ) )
		{
			PostMessage( WM_QUIT );
		}
		else
		{
			// 다음 패치
			IncreaseDownCompFileCount();
			NextFile();
		}		
	}
	// 언집 실패
	else if( info.s_nVersion < U_ERROR_MIN )
	{
		_SetRange_SubUnzipProgress( 100 );
		_SetPos_SubUnzipProgress( 100 );
		KillTimer( ID_TIMER_UNZIP );

		if( SetError( info.s_nVersion ) == true )
		{
			NextFile();
		}
		else
		{
			DisplayError();
			PostMessage( WM_QUIT );
		}
	}	
}

void CAutoUpgradeDlg::_Comp_Download()
{
	DeleteFile( UPGRADEINFO_FILE );

	int l, h;
	m_ctrlProgressMain.GetRange( l, h );
	_SetPos_MainProgress( h );

	if( _Check_Pack() == false )
	{	
		m_ctrlProgressSubDown.SetPos( 0 );
		m_ctrlProgressSubUnzip.GetRange( l, h );
		_SetPos_SubUnzipProgress( h );

		SetState( eSTATE_UPDATE_COMP );
		Proc_MainState();

		m_bmpStart.SetButtonEable(true);
		m_bmpOption.SetButtonEable( true );

		_ResetButton();

		// 런처 업데이트 체크
		if( CheckLauncherUpdate( true ) )
		{
			PostMessage( WM_QUIT );
		}
	}
}

//================================================================================================================
//
//		Unzip
//
//================================================================================================================

void CAutoUpgradeDlg::UnZip_File()
{
	_SetPos_SubDownProgress( 0 );
	_SetPos_SubUnzipProgress( 0 );

	//int nFailCount = 0;
	//CString strDown;
	//strDown.Format( "%s%d.zip_DOWN_", GetWorkPath(), m_nDownVersion );
	//CString strCRCData;
	//strCRCData.Format( "%s%d.zip_CRCDATA_", GetWorkPath(), m_nDownVersion );
	//while( ( _access_s( strDown, 0 ) == 0 )||( _access_s( strCRCData, 0 ) == 0 ) )
	//{
	//	Sleep( 300 );
	//	++nFailCount;
	//	// 실패로 보고 다시 받자
	//	if( nFailCount > 4 )
	//	{
	//		if( SetError( UNZ_FAIL_DOWNLOAD ) == true )
	//		{
	//			NextFile();
	//		}
	//		else
	//		{
	//			DisplayError();
	//			PostMessage( WM_QUIT );
	//		}
	//		return;
	//	}
	//}

	g_pThread->Get()->SetVersion( U_WAIT );

	sTHREAD_INPUT tu;
	tu.s_eType = sTHREAD_INPUT::UNZIP;
	tu.s_nVersion = m_nDownVersion;
	g_pThread->Add( &tu );	
	
	SetTimer( ID_TIMER_UNZIP, ID_TIMER_UNZIP_TICK, NULL );
}

//================================================================================================================
//
//		에러
//
//================================================================================================================
void CAutoUpgradeDlg::ResetError()
{
	m_nErrorCount = 0;
	for( int i=0; i<MAX_ERROR_CODE_COUNT; ++i )
	{
		m_nErrorCode[ i ] = 0;
	}
}

bool CAutoUpgradeDlg::SetError( int nCode )
{
	for( int i=0; i<MAX_ERROR_CODE_COUNT; ++i )
	{
		if( m_nErrorCode[ i ] == 0 )
		{
			++m_nErrorCount;
			m_nErrorCode[ i ] = nCode;
			// 에러코드가 가득 찼다면 리턴 패일즈
			if( m_nErrorCount == MAX_ERROR_CODE_COUNT )
			{
				return false;
			}
			return true;
		}
	}
	return false;
}

int CAutoUpgradeDlg::GetLastError()
{
	for( int i=MAX_ERROR_CODE_COUNT-1; i>-1; --i )
	{
		if( m_nErrorCode[ i ] != 0 )
		{
			return m_nErrorCode[ i ];
		}
	}
	return 0;
}

void CAutoUpgradeDlg::DisplayError()
{
	TCHAR cIndex[ 16 ];
	TCHAR msg[ 2048 ] = STR_MULTY_ERROR;
	for( int i=0; i<m_nErrorCount; ++i )
	{
		_tcscat_s( msg, 2048, STR_MULTY_ERROR_CODE );
		_stprintf_s( cIndex, 10, L"[%d] : ", i+1 );
		_tcscat_s( msg, 2048, cIndex );
		_tcscat_s( msg, 2048, _GetErrorStr( m_nErrorCode[ i ] ) );
	}
	MessageBox( msg, STR_FAIL, MB_OK );
}

TCHAR* CAutoUpgradeDlg::_GetErrorStr( int nCode )
{
	switch( nCode )
	{
	case UNZ_PARAMERROR:
		return STR_ERROR_CODE_UNZ_PARAMERROR;
	case UNZ_BADZIPFILE:
		return STR_ERROR_CODE_UNZ_BADZIPFILE;
	case UNZ_CRCERROR:
		return STR_ERROR_CODE_UNZ_CRCERROR;
	case UNZ_NOT_FILE:
		return STR_ERROR_CODE_UNZ_NOT_FILE;
	case UNZ_OPENFILE_ERROR:
		return STR_ERROR_CODE_UNZ_OPENFILE_ERROR;
	case UNZ_FAIL_DOWNLOAD:
		return STR_ERROR_CODE_UNZ_FAIL_DOWNLOAD;
	case UNZ_FAIL_DOWN_VERSION:
		return STR_ERROR_CODE_UNZ_FAIL_DOWN_VERSION;

	case URL_NOT_FILE:
		return STR_ERROR_CODE_URL_NOT_FILE;
	case URL_OPENFILE_ERROR:
		return STR_ERROR_CODE_URL_OPENFILE_ERROR;

	case PACK_FAILED_OPENFILE:
		return STR_ERROR_CODE_PACK_FAILED_OPENFILE;
	}
	assert( false );
	return L"";
}

//================================================================================================================
//
//		기타 함수
//
//================================================================================================================
bool CAutoUpgradeDlg::CheckLauncherUpdate( bool bEndCall )
{
	if( _taccess_s( NEW_EXE_JUST_TEMP, 0 ) == 0 )
	{
		DeleteFile( NEW_EXE_TEMP );

		// 기존 런처 파일 이름 변경	
		AfxMessageBox( STR_UPDATE_LUNCHER );
		return true;
	}
	else if( ( bEndCall == true )&&( _taccess_s( NEW_EXE_TEMP, 0 ) == 0 ) )
	{
		DeleteFile( NEW_EXE_JUST_TEMP );

		// 기존 런처 파일 이름 변경	
		AfxMessageBox( STR_UPDATE_LUNCHER );
		return true;
	}
	
	return false;
}

CString CAutoUpgradeDlg::GetWorkPath()
{
	CString strPath;

	TCHAR szCurPath[MAX_PATH];
	::GetCurrentDirectory(sizeof(szCurPath), szCurPath);
	strPath.Format( L"%s\\", szCurPath);

	return strPath;
}

bool CAutoUpgradeDlg::DoExe( TCHAR* cExe, TCHAR* cCommand )
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWNORMAL;	

	if( CreateProcess( cExe, cCommand, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi) == FALSE )
	{
		DWORD dwErr = GetLastError();
		switch( dwErr )
		{
		case NO_ERROR:
			break;
		case ERROR_FILE_NOT_FOUND:
			MessageBox( STR_PLAYGAME_ERROR_FILE_NOT_FOUND, L"DMO", MB_ICONHAND | MB_OK);
			break;
		case ERROR_PATH_NOT_FOUND:
			MessageBox( STR_PLAYGAME_ERROR_PATH_NOT_FOUND, L"DMO", MB_ICONHAND | MB_OK);
			break;
		case ERROR_NOT_ENOUGH_MEMORY:
		case ERROR_OUTOFMEMORY:
			MessageBox( STR_PLAYGAME_ERROR_NOT_ENOUGH_MEMORY, L"DMO", MB_ICONHAND | MB_OK);
			break;
		case ERROR_BAD_EXE_FORMAT:
			MessageBox( STR_PLAYGAME_ERROR_BAD_EXE_FORMAT, L"DMO", MB_ICONHAND | MB_OK);
			break;
		default:
			{
				TCHAR eff[ 128 ];
				_stprintf_s( eff, 128, STR_PLAYGAME_ERROR_DEFAULT, dwErr );
				MessageBox( eff, L"DMO", MB_ICONHAND | MB_OK);
			}			
		}
		return false;
	}
	return true;
}


BOOL CAutoUpgradeDlg::_CheckRedist2015()
{
	// Vs 2015 Update3 용 재배포 패키지 설치 확인
	BOOL bReg = REG::IsRegstry(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{e2803110-78b3-4664-a479-3611a381656a}");
	if (bReg)// 설치기 되어 있으면 게임 실행
		return true;

	// 설치가 안되어 있으면 설치 
	SHELLEXECUTEINFO sei;
	::ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.lpFile = L"vc_redist.x86.exe";
	sei.nShow = SW_SHOW;
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = L"open";


	DWORD dwResult = ::ShellExecuteEx(&sei);
	if (sei.hProcess != NULL)
		::WaitForSingleObject(sei.hProcess, INFINITE);// 설치가 끝날 때까지 대기

													  // 설치가 되었는지 확인
	return REG::IsRegstry(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{e2803110-78b3-4664-a479-3611a381656a}");
}

void CAutoUpgradeDlg::GameStart()
{
	if( m_bAlreadyStart == true )
		return;
	m_bAlreadyStart = true;

	Sleep( 300 );
	if( m_eState != eSTATE_UPDATE_COMP )
	{
		TCHAR szErr[256];
		_stprintf_s(szErr, 256, STR_FAIL_PATCH );
		MessageBox( szErr, L"DMOLauncher", MB_ICONHAND | MB_OK);
		PostMessage(WM_CLOSE);
		return;
	}

	// 윈도우즈 10 호환성 검사
	if( WVS::IsWindows8OrGreater() )
	{
		CString strBuf;
		strBuf.Format(_T("%s%s"), GetWorkPath(), EXECFILE);
		WVS::APPCOMPATFLAG flag = WVS::GetAppCompatFlag(strBuf);
		if( WVS::acpfWin7RTM != flag )
			WVS::SetAppCompatFlag( strBuf, WVS::acpfWin7RTM );
	}

	// Vs2015 재배포 패키지 설치 확인
	if (!_CheckRedist2015())
	{
		PostMessage(WM_CLOSE);
		return;
	}
	

	CString strArg;
	strArg = L" true";
	
	DoExe( EXECFILE, (TCHAR*)(LPCTSTR)strArg );	
	CDialog::OnOK();
}

void CAutoUpgradeDlg::ProcessButtonUp( int iKind ) 
{
	switch ( iKind )
	{
	case BMP_BUTTON_START:	GameStart();				break;
	case BMP_BUTTON_CANCEL:	PostMessage( WM_QUIT );		break;
	case BMP_BUTTON_HOME:	ShellExecuteA(NULL, "open", "explorer.exe", HOME_PAGE_SITE, NULL, SW_SHOW);	break;
	case BMP_BUTTON_OPTION:
		{
			COption optionDlg;
			optionDlg.DoModal();
		}
		break;
	}
}

// void CAutoUpgradeDlg::OnStnClickedTaiwanOption()
// {
// 	COption optionDlg;
// 	optionDlg.DoModal();
// 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// }
