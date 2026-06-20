#include "stdafx.h"
#include "UpdateManager.h"
#include "UpdateManagerDlg.h"

CUpdateManagerDlg::CUpdateManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateManagerDlg::IDD, pParent),m_nPersent(0), m_nCurVersion(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUpdateManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUpdateManagerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	//}}AFX_MSG_MAP
	ON_WM_MOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_NCMOUSEMOVE()
END_MESSAGE_MAP()


BOOL CUpdateManagerDlg::Init( CString const& nExeFilePath, CString const& szConfigUrl, CString const& szParam )
{
	m_szParam = szParam;
	m_strExeFilePath = nExeFilePath;//현재 경로
	m_strConfigUrl = szConfigUrl;
	return TRUE;
}

// 파일의 리비젼 번호를 가져온다
BOOL CUpdateManagerDlg::GetFileRevisionNumber(CString const& FilePath, WORD & curVersion)
{
	CString file = FilePath;
	BOOL bResult = FALSE;
	DWORD dwHandle;
	// 파일로부터 버전정보데이터의 크기가 얼마인지를 구합니다.
	DWORD infoSize = GetFileVersionInfoSize(file, &dwHandle);
	if(infoSize==0) 
		return bResult;

	// 버퍼할당
	char* buffer = new char[infoSize];
	if(buffer)
	{
		// 버전정보데이터를 가져옵니다.
		if(GetFileVersionInfo(file,dwHandle, infoSize, buffer)!=0)
		{
			VS_FIXEDFILEINFO* pFineInfo;
			UINT bufLen = 0;
			// buffer로 부터 VS_FIXEDFILEINFO 정보를 가져옵니다.
			if(VerQueryValue(buffer,L"\\",(LPVOID*)&pFineInfo, &bufLen) !=0)
			{    
				WORD majorVer, minorVer, buildNum, revisionNum;
				majorVer = HIWORD(pFineInfo->dwFileVersionMS);
				minorVer = LOWORD(pFineInfo->dwFileVersionMS);
				buildNum = HIWORD(pFineInfo->dwFileVersionLS);
				revisionNum = LOWORD(pFineInfo->dwFileVersionLS);

				curVersion = revisionNum;

				bResult = TRUE;
			}
		}
		delete[] buffer;
	}

	return bResult;
}

LRESULT CUpdateManagerDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{		
	Render( 0 );
	Setup();	

	UpdateDialogControls(this, FALSE);
	return (LRESULT)1;
}


// CUpdateManagerDlg 메시지 처리기

BOOL CUpdateManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();		

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// Main 배경 화면
	BITMAP bmpInfo; // 비트맵 정보
	m_bmpMainBG.LoadBitmap(IDB_BITMAP2); // Main 배경 이미지 Load
	m_bmpMainBG.GetBitmap(&bmpInfo); // Main 배경 비트맵 정보 읽기
	m_nMainWidth = bmpInfo.bmWidth; // Main 윈도우 폭
	m_nMainHeight = bmpInfo.bmHeight; // Main 윈도우 높이
	SetWindowPos( NULL, 0, 0, m_nMainWidth, m_nMainHeight, SWP_NOZORDER | SWP_NOMOVE );

	m_bmpBar.LoadBitmap(IDB_BITMAP1); // Main 배경 이미지 Load
	m_bmpBar.GetBitmap(&bmpInfo); // Main 배경 비트맵 정보 읽기	
	m_nBarWidth = bmpInfo.bmWidth; // Main 윈도우 폭
	m_nBarHeight = bmpInfo.bmHeight; // Main 윈도우 높이

	// 폰트 셋팅
	m_font.CreateFont( 15, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, _T("Tahoma") );	

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CUpdateManagerDlg::OnPaint()
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

void CUpdateManagerDlg::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return;
	//CDialog::OnOK();
}

void CUpdateManagerDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return;
	//CDialog::OnCancel();
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CUpdateManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUpdateManagerDlg::Setup()
{
	Install();

	if( m_nPersent != 100 )
		AfxMessageBox( UPDATE_FAIL );
	else
		ExecuteLauncher();

	Delete();
	PostMessage( WM_QUIT );		
}

void CUpdateManagerDlg::Render( int nPer )
{
	m_nPersent = nPer;

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


	// Main 배경 그림 그리기
	CDC cdc;
	cdc.CreateCompatibleDC(&dc);
	cdc.SelectObject(m_bmpMainBG);
	MemDC.BitBlt(0, 0, m_nMainWidth, m_nMainHeight, &cdc, 0, 0, SRCCOPY);
	cdc.DeleteDC();

	CDC cdc2;
	cdc2.CreateCompatibleDC(&dc);
	cdc2.SelectObject(m_bmpBar);

	int nSize = (int)( 220 * ( m_nPersent / 100.0f ) );
	for( int i = 0; i < nSize; i++ )
	{
		MemDC.BitBlt( i + 15, 59, m_nBarWidth, m_nBarHeight, &cdc2, 0, 0, SRCCOPY);
	}
	cdc2.DeleteDC();

	// 글자 쓰기
	CFont* pFont = MemDC.SelectObject( &m_font );
	MemDC.SetBkMode( TRANSPARENT );		

	CString UpdateTitle;
	UpdateTitle.Format( STR_UPDATE );
	MemDC.TextOut( 27, 11, UpdateTitle );								
	
	TCHAR sz[ 64 ];
	_stprintf_s( sz, 64, STR_PERSENT, m_nPersent );
	MemDC.TextOut( 17, 43, sz );

	
	MemDC.SelectObject( pFont );	

	dc.BitBlt( 0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY );		
	MemDC.SelectObject( pOldBitmap );	

	Sleep( 10 );
}

void CUpdateManagerDlg::Delete()
{
	m_font.DeleteObject();
}

void CUpdateManagerDlg::Install()
{
	if( !DownLoadLauncherVersionInfo() )
		return;

	Render( 25 );		
	
	//서버 점검 중인지 체크는 하지 않음.
	if( !CheckServerEnable() )
		return;

	Render( 50 );

	if( !GetLauncherVersion() )
		return;

	Render( 75 );

	// 런처 미설치자 런처설치
	if( !VersionCheck() )
		return;

	Render( 100 );
}

BOOL CUpdateManagerDlg::PassServerCheck()	// 서버 점검중일 때도 패치가 가능한지 체크함.
{
	CString checkFile;
	checkFile.Format( _T("%s%s"), m_strExeFilePath, PASS_SERVER_CHECK );

	if( _taccess_s( checkFile, 0 ) == 0 )
		return TRUE;

	return FALSE;
}

bool CUpdateManagerDlg::CheckServerEnable()
{
	if( m_VersionList.IsServerEnable() )
		return true;

	if( PassServerCheck() )
		return true;

	CString msg;
	msg.Format(_T("%s%s"), m_VersionList.GetMsgTitle(), m_VersionList.GetMsg() );
	MessageBox( msg, _T("UpdateManager"), MB_OK );
	PostMessage( WM_QUIT );
	return false;
}

bool CUpdateManagerDlg::DownLoadLauncherVersionInfo()
{
	if( m_VersionList._IsValidData() )
		return true;

	std::string jsonData;	
	CsDownloadMng::eResult result = CsDownloadMng::DownloadToString( m_strConfigUrl, jsonData );
	if( result != CsDownloadMng::SUCCESS )
		return false;// 파일 다운로드 실패
	
	if( !m_VersionList.Parser( jsonData ) )
		return false;

	return true;
}

bool CUpdateManagerDlg::GetLauncherVersion()
{
	CString strLaucherPath;
	strLaucherPath.Format(_T("%s%s"), m_strExeFilePath, m_VersionList.GetExeFileName_Launcher() );

	if( !GetFileRevisionNumber( strLaucherPath, m_nCurVersion ) )
	{
		if( IDOK == AfxMessageBox( NO_PATH, MB_OKCANCEL ) )
		{
			m_nCurVersion = 0;
			return true;
		}
		return false;
	}

	return true;
}

bool CUpdateManagerDlg::VersionCheck()
{
	if (m_nCurVersion == m_VersionList.GetCurrentVersion_Launcher())
		return true;

	CString sDownloadFile = m_VersionList.GetLastVersionFile_Launcher();
	if (sDownloadFile.IsEmpty())
		return false;

	CString sDownloadUrl = m_VersionList.GetDownloadUrl_Launcher();
	if (sDownloadUrl.IsEmpty())
		return false;

	CString DestDir;
	DestDir.Format(_T("%s%s"), m_strExeFilePath, sDownloadFile);
	::DeleteFile(DestDir);
	
	CString Url;
	Url.Format(_T("%s%s"), sDownloadUrl, sDownloadFile );
	CsDownloadMng::eResult result = CsDownloadMng::DownloadToFile(Url, DestDir);
	if (result != CsDownloadMng::SUCCESS)
	{
		CString errMsg;
		errMsg.Format(ERROR_DOWNLOAD_FILE, sDownloadFile);
		AfxMessageBox(errMsg);
		return false;
	}

	if (DmCS::FileFn::IsSameFileExt(DestDir.operator LPCTSTR(), _T(".zip")))
	{
		CT2CA tempString(DestDir);
		std::string unzipFile(tempString);
		
		DmCS::CUnzipper unZip;
		bool bResult = unZip.OpenZip(unzipFile.c_str());
		if (bResult)
		{
			bResult = unZip.Unzip();
			unZip.CloseZip();
		}

		::DeleteFile(DestDir);
		if (!bResult)
		{
			CString errMsg;
			errMsg.Format( ERROR_UNZIP_FILE, sDownloadFile);
			AfxMessageBox(errMsg);
			return false;
		}
	}

	Render(70);

// 	std::list<CString> downloadFiles;
// 
// 	m_VersionList.GetLauncher_DownloadFiles( m_nCurVersion, downloadFiles );
// 
// 	CString sDownloadUrl = m_VersionList.GetDownloadUrl_Launcher();
// 	int nPer = 70 / downloadFiles.size();
// 	std::list<CString>::iterator it = downloadFiles.begin();
// 	for( int n = 1; it != downloadFiles.end(); ++it, ++n )
// 	{
// 		CString DestDir;
// 		DestDir.Format(_T("%s%s"), m_strExeFilePath, *it);
// 		::DeleteFile( DestDir );
// 
// 		CString Url;
// 		Url.Format(_T("%s%s"), sDownloadUrl, *it );
// 
// 		CsDownloadMng::eResult result = CsDownloadMng::DownloadToFile( Url, DestDir );
// 		if( result != CsDownloadMng::SUCCESS )
// 		{
// 			// 파일 다운로드 실패
// 
// 			CString errMsg;
// 			errMsg.Format( ERROR_DOWNLOAD_FILE, *it );
// 			AfxMessageBox( errMsg );			
// 			return false;
// 		}
// 
// 		if (DmCS::FileFn::IsSameFileExt(DestDir.operator LPCTSTR(), _T(".zip")))
// 		{
// 			CT2CA tempString(DestDir);
// 			std::string unzipFile(tempString);
// 			if (!DmCS::CUnzipper::Unzip(unzipFile.c_str()))
// 			{
// 				CString errMsg;
// 				errMsg.Format( ERROR_UNZIP_FILE, *it );
// 				return false;
// 			}
// 		}
// 
// 		Render( 20 + (nPer * n) );
// 	}

	return true;
}

void CUpdateManagerDlg::ExecuteLauncher()
{
	CString str= m_strExeFilePath; 
	str += m_VersionList.GetExeFileName_Launcher();

	SHELLEXECUTEINFO si;                             // SHELLEXECUTEINFO 구조체를 하나 만든다
	ZeroMemory(&si, sizeof(SHELLEXECUTEINFO));       // si 구조체 초기화 
	si.cbSize = sizeof(SHELLEXECUTEINFO);            // 크기 설정

	si.fMask = SEE_MASK_NOCLOSEPROCESS;              // ??
	si.lpVerb = _T("open");                             // 동작 형식(파일열기)
	si.lpFile = str;							     // szPath;  // 실행할 파일 경로 지정
	si.lpParameters = m_szParam;						//szArg;    // 인자값 지정
	si.nShow = SW_SHOWNORMAL;                        // 표시 형식

	if( ShellExecuteEx(&si) )                              // 외부 프로그램 호출
		return;

	DWORD dwErr = GetLastError();
	switch( dwErr )
	{				
	case ERROR_FILE_NOT_FOUND:		AfxMessageBox( LAUNCHER_FILE_NOT_FOUND );		break;		
	case ERROR_PATH_NOT_FOUND:		AfxMessageBox( LAUNCHER_PATH_NOT_FOUND );		break;			
	case ERROR_NOT_ENOUGH_MEMORY:	AfxMessageBox( LAUNCHER_NOT_ENOUGH_MEMORY );	break;	
	default:
		{
			TCHAR eff[ 128 ];
			_stprintf_s( eff, 128, STR_LAUNCHER_ERRO_DEFAULT, dwErr );
			AfxMessageBox( eff );
		}break;
	}
}