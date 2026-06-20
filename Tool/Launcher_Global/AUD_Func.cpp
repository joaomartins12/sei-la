
#include "stdafx.h"
#include "Option.h"
#include "AutoUpgradeDlg.h"
#include "WindowVersionChecker.h"
#include "UseRegistry.h"

//================================================================================================================
//
//		초기화
//
//================================================================================================================

void CAutoUpgradeDlg::_Init_Window()
{
	SetWindowText( STR_WINDOW_TITLE );

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
	m_bmpCancle.SetParent( this );

	m_bmpOption.SetParent(this);

	m_bmpStart.SetButtonKind( BMP_BUTTON_START );
	m_bmpCancle.SetButtonKind( BMP_BUTTON_CANCEL );
	m_bmpOption.SetButtonKind( BMP_BUTTON_OPTION );

	m_bmpStart.LoadButtonBitmaps(IDB_START_DEF, IDB_START_ON, IDB_START_DOWN, IDB_START_NO);
	m_bmpCancle.LoadButtonBitmaps(IDB_CANCLE_DEF, IDB_CANCLE_ON, IDB_CANCLE_DOWN);
	m_bmpOption.LoadButtonBitmaps(IDB_OPTION_NOMAL, IDB_OPTION_OVER, IDB_OPTION_CLICK, IDB_OPTION_DISABLE);

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

	// 옵션
	rect.left = 655;
	rect.top = 4;
	rect.right = rect.left + 60;
	rect.bottom = rect.top + 20;
	m_bmpOption.MoveWindow(&rect);

	//홈 계시판
	rect.left = 0; //24;
	rect.top = 26; // 58;	
	rect.right = 742;
	rect.bottom = rect.top + 390;

	GetDlgItem(IDC_WEBVIEW)->SetWindowPos(NULL,rect.left,rect.top, rect.right - rect.left, rect.bottom - rect.top, SW_SHOW);

	pHtmlView = new CMyHtmlView;	
	DWORD dwPlag = WS_CHILD | WS_VISIBLE;
	pHtmlView->Create(NULL, L"", dwPlag , rect, this, IDC_WEBVIEW ) ;

	int htmlViewBorder = 2;
	pHtmlView->SetLeft( -htmlViewBorder );
	pHtmlView->SetTop( -htmlViewBorder );
	pHtmlView->SetWidth( pHtmlView->GetWidth() + htmlViewBorder );
	pHtmlView->SetHeight( pHtmlView->GetHeight() + htmlViewBorder );
	pHtmlView->Navigate( m_launcherInfos.GetMainPageUrl(USER_TYPE_KEY) );//원본
	pHtmlView->OnInitialUpdate();

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

// 클라이언트 패치 버젼 셋팅
bool CAutoUpgradeDlg::_Init_Version()
{
	m_VersionFile.Init(VERSIONINFO_FILE);
	m_strClientVersion = m_VersionFile.GetString( L"VERSION", L"version" );
	
	if( m_strClientVersion.IsEmpty() || (0 == m_strClientVersion.Compare(_T("0"))) )
	{
		// 버젼 최하 값 검사 - 패치정보를 영원토록 보관 할 수 없으니. 일정시간 되면 제거 - 그에 따른 그전 버젼 유저는 패치 불가
		if( MessageBox( STR_FAIL_OLD_VERSION, STR_FAIL_VERSION, MB_OKCANCEL ) == IDOK )
		{
			CString fullClientDownloadPageUrl = m_launcherInfos.GetGameDownLoadUrl(USER_TYPE_KEY);
			if( !fullClientDownloadPageUrl.IsEmpty() )
				ShellExecute(NULL, L"open", L"explorer.exe", fullClientDownloadPageUrl, NULL, SW_SHOW);
		}
		return false;
	}

	m_nLastPatchNum =  _ttoi( m_strClientVersion );
	if( m_launcherInfos.GetMinPatchVersion() > m_nLastPatchNum )
	{
		// 버젼 최하 값 검사 - 패치정보를 영원토록 보관 할 수 없으니. 일정시간 되면 제거 - 그에 따른 그전 버젼 유저는 패치 불가
		if( MessageBox( STR_FAIL_OLD_VERSION, STR_FAIL_VERSION, MB_OKCANCEL ) == IDOK )
		{
			CString fullClientDownloadPageUrl = m_launcherInfos.GetGameDownLoadUrl(USER_TYPE_KEY);
			if( !fullClientDownloadPageUrl.IsEmpty() )
				ShellExecute(NULL, L"open", L"explorer.exe", fullClientDownloadPageUrl, NULL, SW_SHOW);			
		}
		return false;
	}
	
	return true;
}

bool CAutoUpgradeDlg::_Init_Down()
{	
	if( !m_launcherInfos.IsServerEnable() )
	{
		if( !PassServerCheck() )
		{
			CString msg;
			msg.Format(_T("%s%s"), m_launcherInfos.GetMsgTitle(), m_launcherInfos.GetMsg() );
			MessageBox( msg, STR_WINDOW_TITLE, MB_OK );
			PostMessage( WM_QUIT );
			return true;
		}
	}

	assert( m_hSession == NULL );
	std::string cInternetOpenName;
	m_hSession = InternetOpenA("DMO", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);	
	if (m_hSession == NULL) 
	{
		MessageBox( STR_FAIL_CONNECT_INTERNET, STR_FAIL, MB_OK );
		PostMessage( WM_QUIT );
		return true;
	}

	g_pThread->Get()->SetHInternet( &m_hSession );


	if( VersionCheck() == true )
	{
		NextFile();
		return true;
	}

	return true;
	//return _ServerVersionFile();
}

BOOL CAutoUpgradeDlg::PassServerCheck()	// 서버 점검중일 때도 패치가 가능한지 체크함.
{
	CString checkFile;
	checkFile.Format( _T("%s%s"), m_strCurrentPath, PASS_SERVER_CHECK );

	if( _taccess_s( checkFile, 0 ) == 0 )
		return TRUE;

	return FALSE;
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

	Sleep(100);
	
	CThread::GlobalShotDown();
	if( m_hSession != NULL )
	{
		InternetCloseHandle( m_hSession );
		m_hSession = NULL;
	}

// 	CString strDownPathPatchInfo;	
// 	strDownPathPatchInfo.Format(_T("%s%s"), m_strCurrentPath, m_launcherInfos.GetFileName_Info_PathInfo());
// 
// 	::DeleteFile(strDownPathPatchInfo);
}


//================================================================================================================
//
//		정보
//
//================================================================================================================

bool CAutoUpgradeDlg::_ServerVersionFile()
{
// 	CString strDownPathPatchInfo;	
// 	strDownPathPatchInfo.Format(_T("%s%s"), m_strCurrentPath, m_launcherInfos.GetFileName_Info_PathInfo());
// 	CString strServerNamePathPatchInfo;
// 	strServerNamePathPatchInfo.Format(_T("%s%s"), m_launcherInfos.GetDownloadUrl_Patch(), m_launcherInfos.GetFileName_Info_PathInfo() );
// 
// 	sTHREAD_INPUT ti;
// 	ti.s_eType = sTHREAD_INPUT::URL_DOWN;
// 	ti.s_nVersion = sTHREAD_INPUT::VU_VERSION;		// 버젼인포 파일
// 	ti.s_szURLPath = strServerNamePathPatchInfo;
// 	ti.s_szDownPath = strDownPathPatchInfo;
// 
// 	g_pThread->Add( &ti );	
	return true;
}

void CAutoUpgradeDlg::Set_FileInfo( ULONGLONG ullFileSize )
{
	_SetRange_SubDownProgress( (int)( ullFileSize/1000 ) );
	_SetPos_SubDownProgress( 0 );
	_SetPos_SubUnzipProgress( 0 );
}

bool CAutoUpgradeDlg::VersionCheck()
{
	int nCV = _ttoi( m_strClientVersion );
	int nSV = m_launcherInfos.GetPatchVersion();

	if( nCV > nSV )
	{
		MessageBox( STR_CVERSION_ERROR, STR_FAIL, MB_OK );
		//CheckLauncherUpdate( true );
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
	strServerFile.Format( L"%s%s%d.zip", m_launcherInfos.GetDownloadUrl_PatchFile(), FILE_WARD, m_nDownVersion );//원본

	CString strClient;
	strClient.Format( L"%s%s%d.zip", m_strCurrentPath, FILE_WARD, m_nDownVersion );

	if( _taccess_s( strClient, 0 ) == 0 )
	{
		BOOL bSuccess = DeleteFile( strClient );
		if( bSuccess == FALSE )
		{
			MessageBox( STR_FAIL_DELETE_FILE, STR_FAIL, MB_OK );
			PostMessage( WM_QUIT );			
		}
		Sleep( 200 );
	}

	g_pThread->Get()->SetVersion( U_WAIT );

	sTHREAD_INPUT ti;
	ti.s_eType = sTHREAD_INPUT::URL_DOWN;
	ti.s_nVersion = sTHREAD_INPUT::VU_DOWN;			// 다운로드 파일
	ti.s_dwPatchNum = m_nDownVersion;
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
		{
			m_szMainState = STR_DO_PACK;
		}break;
	case eSTATE_UPDATE_COMP:
		{
			m_szMainState = STR_COMP_UPDATE;
			//m_szMainState.Format(_T("%s( L.P.N. : %d)"), STR_COMP_UPDATE, m_nLastPatchNum);
		}break;
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
#ifdef PATCH_PACK
					IncreaseDownCompFileCount();
					NextFile();
#else
					_Comp_Download();
#endif
					
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

#ifdef PATCH_PACK
		DoPathPack_File();
#else
		// 다음 패치
		IncreaseDownCompFileCount();
		NextFile();
#endif
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
// 	CString strDownPathPatchInfo;	
// 	strDownPathPatchInfo.Format(_T("%s%s"), m_strCurrentPath, m_launcherInfos.GetFileName_Info_PathInfo());

//	DeleteFile( strDownPathPatchInfo );

	int l, h;
	m_ctrlProgressMain.GetRange( l, h );
	_SetPos_MainProgress( h );

#ifndef PATCH_PACK
	if( _Check_Pack() == false )
#endif
	{	
		m_ctrlProgressSubDown.SetPos( 0 );
		m_ctrlProgressSubUnzip.GetRange( l, h );
		_SetPos_SubUnzipProgress( h );

		SetState( eSTATE_UPDATE_COMP );
		Proc_MainState();

		m_bmpStart.SetButtonEable(true);
		_ResetButton();

// 		// 런처 업데이트 체크
// 		if( CheckLauncherUpdate( true ) )
// 			PostMessage( WM_QUIT );
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

	g_pThread->Get()->SetVersion( U_WAIT );

	sTHREAD_INPUT tu;
	tu.s_eType = sTHREAD_INPUT::UNZIP;
	tu.s_nVersion = m_nDownVersion;
	tu.s_szDownPath = m_strCurrentPath;

	g_pThread->Add( &tu );	
	
	SetTimer( ID_TIMER_UNZIP, ID_TIMER_UNZIP_TICK, NULL );
}

//================================================================================================================
//		Patch Pack
//================================================================================================================
void CAutoUpgradeDlg::DoPathPack_File()
{
	// 폴더가 존재 한다면 총 파일 갯수를 구한다.
	SetState( eSTATE_DO_PACK );
	Proc_MainState();

	g_pThread->Get()->ResetInfo();
	g_pThread->Get()->SetVersion( U_WAIT );
	Proc_SubState();

	// 쓰레드에 데이터 셋팅		
	sTHREAD_INPUT ti;
	ti.s_eType = sTHREAD_INPUT::DO_PACK;
	ti.s_nVersion = m_nDownVersion;
	ti.s_szDownPath = m_strCurrentPath;
	g_pThread->Add( &ti );

	// 디스플레이를 위한 타이머 셋팅
	SetTimer( ID_TIMER_DO_PACK, ID_TIMER_DO_PACK_TICK, NULL );
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

BOOL CAutoUpgradeDlg::_CheckRedist2015()
{
	// 재배포 패키지 설치 여부 체크 안할 경우 게임 실행
	if (!m_launcherInfos.IsRegCheckEnable())
		return TRUE;

	std::list<CString> const& checkList = m_launcherInfos.GetRegCheckList();
	for (auto const& it : checkList)
	{
		// 설치기 되어 있으면 게임 실행
		if (REG::IsRegstry(HKEY_LOCAL_MACHINE, it.operator LPCTSTR()))
			return TRUE;
	}

	CString exeCommand = m_launcherInfos.GetRegExecute();
	TCHAR szGamePath[512] = { 0, };
	_tcscat_s(szGamePath, 512, exeCommand.operator LPCTSTR());

	// 설치가 안되어 있으면 설치 
	SHELLEXECUTEINFO sei;
	::ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.lpFile = szGamePath;
	sei.nShow = SW_SHOW;
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = L"open";

	DWORD dwResult = ::ShellExecuteEx(&sei);
	if (sei.hProcess != NULL)
		::WaitForSingleObject(sei.hProcess, INFINITE);// 설치가 끝날 때까지 대기

	 // 설치가 되었는지 확인
	for (auto const& it : checkList)
	{
		if (REG::IsRegstry(HKEY_LOCAL_MACHINE, it.operator LPCTSTR()))
			return TRUE;
	}
	return FALSE;
}

bool CAutoUpgradeDlg::DoExe( TCHAR* cCommand )
{
	SHELLEXECUTEINFO si;                            
	ZeroMemory(&si, sizeof(SHELLEXECUTEINFO));      
	si.cbSize = sizeof(SHELLEXECUTEINFO);           
	
	si.fMask = SEE_MASK_NOCLOSEPROCESS;             
	si.lpVerb = L"runas";                           // 동작 형식(파일열기)
	si.lpFile = m_launcherInfos.GetRunExeFileName_Launcher();// 실행할 파일 경로 지정
	si.lpParameters = cCommand;						// 인자값 지정
	si.nShow = SW_SHOWNORMAL;                       // 표시 형식

	if( ShellExecuteEx(&si) )              // 외부 프로그램 호출
		return true;

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
			TCHAR eff[ 128 ] = {0,};
			_stprintf_s( eff, 128, STR_PLAYGAME_ERROR_DEFAULT, dwErr );
			MessageBox( eff, L"DMO", MB_ICONHAND | MB_OK);
		}			
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


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

#ifdef CHECK_WINDOW_10
	// 윈도우즈 10 호환성 검사
	if( WVS::IsWindows8OrGreater() )
	{
		CString strBuf;
		strBuf.Format(_T("%s%s"), m_strCurrentPath, m_launcherInfos.GetRunExeFileName_Launcher());
		WVS::APPCOMPATFLAG flag = WVS::GetAppCompatFlag(strBuf);
#ifdef GSP_STEAM
		if( WVS::apcfNone != flag )
			WVS::RemoveAppCompatFlag( strBuf );
#else
		if( WVS::acpfWin7RTM != flag )
			WVS::SetAppCompatFlag( strBuf, WVS::acpfWin7RTM );
#endif
	}
#endif

	// Vs2015 재배포 패키지 설치 확인
	if (!_CheckRedist2015())
	{
		PostMessage(WM_CLOSE);
		return;
	}

	CString strArg;
	strArg = L" ";
	strArg += DIMA_OAUTH_KEY_VALUE;

	DoExe( (TCHAR*)(LPCTSTR)strArg );	
	CDialog::OnOK();
}

void CAutoUpgradeDlg::ProcessButtonUp( int iKind ) 
{
	switch( iKind )
	{
	case BMP_BUTTON_START:		GameStart();			return;
	case BMP_BUTTON_CANCEL:		PostMessage( WM_QUIT ); return;
	case BMP_BUTTON_HOME:
	{
		CString url = m_launcherInfos.GetHomePageUrl(USER_TYPE_KEY);
		ShellExecute(NULL, _T("open"), _T("explorer.exe"), url.GetBuffer(), NULL, SW_SHOW);
	}return;
	case BMP_BUTTON_OPTION:		
		{
			char gamePath[MAX_PATH]={0,};
			W2M( gamePath, m_strCurrentPath, MAX_PATH );
			COption optionDlg(gamePath);
			optionDlg.DoModal();
		}return;
	}
}
