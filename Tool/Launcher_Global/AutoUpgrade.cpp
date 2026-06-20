// AutoUpgrade.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "AutoUpgradeDlg.h"


BEGIN_MESSAGE_MAP(CAutoUpgradeApp, CWinApp)
END_MESSAGE_MAP()

CAutoUpgradeApp::CAutoUpgradeApp()
{
}

BOOL PassServerCheck(CString const& currentPath)	// 서버 점검중일 때도 패치가 가능한지 체크함.
{
	CString checkFile;
	checkFile.Format( _T("%s%s"), currentPath, PASS_SERVER_CHECK );

	if( _taccess_s( checkFile, 0 ) == 0 )
		return TRUE;

	return FALSE;
}

CAutoUpgradeApp theApp;
BOOL CAutoUpgradeApp::InitInstance()
{
	HWND hWnd = NULL;
	if( ( __wargv[ 1 ] == NULL )||( _tcscmp( __wargv[ 1 ], L"true" ) != 0 ) )
	{
		hWnd = FindWindow( 0, STR_WINDOW_TITLE );
		if( hWnd )
		{
			::MessageBox( m_pMainWnd->GetSafeHwnd(), STR_ALREADY_START, STR_FAIL, MB_OK|MB_ICONERROR ) ;
			return FALSE ;
		}
	}
	else
	{
		HANDLE k = OpenProcess( PROCESS_ALL_ACCESS, FALSE, _ttoi( __wargv[ 2 ] ) );
		if( k )
			WaitForSingleObject( k, INFINITE );
	}

	hWnd = FindWindow( L"DMO", NULL ) ;
	if( hWnd )
	{
		::MessageBox( m_pMainWnd->GetSafeHwnd(), STR_DO_EXIT_GAME, STR_FAIL, MB_OK|MB_ICONERROR ) ;
		return FALSE ;
	}

	// 트레이 아이콘 종료
	hWnd = FindWindow( 0, WISH_CLOSE_WINDOW );
	if( hWnd )
		SendMessage( hWnd, WM_CLOSE, 0, 0 );

	InitCommonControls();

	CWinApp::InitInstance();
	AfxEnableControlContainer();

	sConfiguration sLauncherInfo;
	// 웹 서버로 부터 데이터 정보를 받는다.
	if( !DownLoadLauncherInfoData( sLauncherInfo ) )
		return FALSE;

	CString currExeName;
	GetCurrentPath( m_currentPath, currExeName );

	CString LacucherPath;
	LacucherPath.Format( L"%s%s", m_currentPath, currExeName );
	WORD wVersion = GetFileRevisionNumber(LacucherPath);

 	CString sCmdLine = m_lpCmdLine;

	BOOL bIsGM = PassServerCheck(m_currentPath);
	if( bIsGM || sLauncherInfo.IsServerEnable() )// 서버 점검중이 아닐때
	{
		DWORD dwCurLauncherVersion = sLauncherInfo.GetCurrentVersion_Launcher();
		bool bUpdate = false;
		if (wVersion < dwCurLauncherVersion)
			bUpdate = true;
		else if (wVersion > dwCurLauncherVersion)
		{
			if (!bIsGM)
				bUpdate = true;
		}

		if (bUpdate)
		{
			// 업데이트 매니져 프로그램 버전 체크		
			if (!VersionCheck_UpdateManager(sLauncherInfo))
			{	// 업데이트 런처 다운로드 실패
				return FALSE;
			}

			CString sendMsg;
			sendMsg.Format(_T("%s%s"), CONFIGURATION_URL, CONFIGURATION_FILENAME);
			RunUpdateManager(sLauncherInfo.GetExeFileName_UpdateManager(), sendMsg, sCmdLine);
			return FALSE;
		}
	}

	CAutoUpgradeDlg dlg;
	dlg.Init( m_currentPath, wVersion, sLauncherInfo );
	
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}

BOOL CAutoUpgradeApp::CheckFile( CString const& strPath, CString const& checkFileName )
{
	CString checkFile;
	checkFile.Format( _T("%s%s"), strPath, checkFileName );
	if( _taccess_s( checkFile, 0 ) == 0 )
		return TRUE;
	return FALSE;
}

BOOL CAutoUpgradeApp::DownLoadLauncherInfoData( sConfiguration & sInfo )
{
	std::string strValue;
	CString downloadUrl;
	downloadUrl.Format(_T("%s%s"), CONFIGURATION_URL, CONFIGURATION_FILENAME);
	CsDownloadMng::eResult result = CsDownloadMng::DownloadToString( downloadUrl, strValue);
	switch( result )
	{
	case CsDownloadMng::INTERNET_NOT_CONNECT: // 인터넷 연결 실패
		AfxMessageBox( STR_FAIL_CONNECT_INTERNET );
		return FALSE;
	case CsDownloadMng::INTERNET_STATUS_ERROR:
	case CsDownloadMng::FILE_DOWNLOAD_FAIL: // 파일이 없다
		AfxMessageBox( STR_FAIL_ONLINE );		
		return FALSE;
	}

	if( !sInfo.Parser( strValue ) )
	{
		AfxMessageBox( STR_FAIL_ONLINE );		
		return FALSE;
	}
	
	return TRUE;
}

BOOL CAutoUpgradeApp::DownloadFile(CString const& Url, CString const& fileName )
{
	CString LastVersionFileName = fileName;
	CString DestdownloadDir;
	DestdownloadDir.Format(L"%s%s", m_currentPath, LastVersionFileName);
	CString UMDownloadUrl;
	UMDownloadUrl.Format(_T("%s%s"), Url, LastVersionFileName);

	::DeleteFile( DestdownloadDir );

	CsDownloadMng::eResult result = CsDownloadMng::DownloadToFile( UMDownloadUrl, DestdownloadDir );
	switch( result )
	{
	case CsDownloadMng::INTERNET_NOT_CONNECT: // 인터넷 연결 실패a
		AfxMessageBox( STR_FAIL_CONNECT_INTERNET );
		return FALSE;
	case CsDownloadMng::INTERNET_STATUS_ERROR:
	case CsDownloadMng::FILE_DOWNLOAD_FAIL: // 파일이 없다
		return FALSE;
	}

	if (DmCS::FileFn::IsSameFileExt(DestdownloadDir.operator LPCTSTR(), _T(".zip")))
	{
		CT2CA tempString(DestdownloadDir);
		std::string unzipFile(tempString);
		DmCS::CUnzipper unZip;
		bool bResult = unZip.OpenZip(unzipFile.c_str());
		if (bResult)
		{
			bResult = unZip.Unzip();
			unZip.CloseZip();
		}

		::DeleteFile(DestdownloadDir);
		
		if (!bResult)
		{
			CString msg;
			msg.Format(STR_UNZIP_ERROR, DestdownloadDir);
			AfxMessageBox(msg);
			return FALSE;
		}
	}

	::DeleteFile( DestdownloadDir );

	return TRUE;
}


BOOL CAutoUpgradeApp::VersionCheck_UpdateManager(sConfiguration const& downloadUrlInfo)
{
	CString filePath;
	filePath.Format( _T("%s%s"), m_currentPath, downloadUrlInfo.GetExeFileName_UpdateManager());// 설치된 런처의 버전 정보가 담겨있는 파일
	WORD wPackerRevisionNum = GetFileRevisionNumber( filePath.GetBuffer() );
	if( wPackerRevisionNum != downloadUrlInfo.GetCurrentVersion_UpdateManager())
		return DownloadFile( downloadUrlInfo.GetDownloadPath_UpdateManager(), downloadUrlInfo.GetCurrentVersionFileName_UpdateManager() );

	return TRUE;
}

BOOL CAutoUpgradeApp::RunUpdateManager(CString const& exeFileName, CString const& wsSendMsg, CString const& szParam)
{
	CString RunExitFile;
	RunExitFile.Format(_T("%s%s"), m_currentPath, exeFileName);

	SHELLEXECUTEINFO si;                            
	ZeroMemory(&si, sizeof(SHELLEXECUTEINFO));      
	si.cbSize = sizeof(SHELLEXECUTEINFO);           

	CString EnParam = wsSendMsg + L"||" + szParam;
	CString nValue = EnDe::Encryption( EnParam );

	si.fMask = SEE_MASK_NOCLOSEPROCESS;             
	si.lpVerb = L"open";                           // 동작 형식(파일열기)
	si.lpFile = RunExitFile;						// 실행할 파일 경로 지정
	si.lpParameters = nValue;						// 인자값 지정
	si.nShow = SW_SHOWNORMAL;                       // 표시 형식

	if( ShellExecuteEx(&si) == FALSE )              // 외부 프로그램 호출
	{
		DWORD dwErr = GetLastError();
		switch( dwErr )
		{				
		case ERROR_FILE_NOT_FOUND:   				
		case ERROR_PATH_NOT_FOUND:   				
		case ERROR_DDE_FAIL:    				
		case ERROR_NO_ASSOCIATION:   				
		case ERROR_ACCESS_DENIED:    				
		case ERROR_DLL_NOT_FOUND:    				
		case ERROR_CANCELLED:    				
		case ERROR_NOT_ENOUGH_MEMORY:				
		case ERROR_SHARING_VIOLATION:
			{
				TCHAR eff[ 128 ];
				//_stprintf_s( eff, 128, STR_PACKER_ERRO_DEFAULT, dwErr );
				AfxMessageBox( eff );
			}
			return FALSE;
		}
	}

	// 런처 패치가 성공하면 런처를 다시 실행해야 하기 때문에 패치 성공 후 종료 시킨다.
	// 런처 패치 성공시 런처 버전 업은 패커에서 한다.
	return FALSE;
}

void CAutoUpgradeApp::GetCurrentPath( CString & OutPut, CString & exeFileName )
{
	TCHAR szModulePath[MAX_PATH]={0,};
	GetModuleFileName(m_hInstance, szModulePath, MAX_PATH);

	TCHAR szDrive[_MAX_DRIVE] = _T("");
	TCHAR szDir[_MAX_DIR] = _T("");
	TCHAR szFName[_MAX_FNAME] = _T("");
	TCHAR szExt[_MAX_EXT] = _T("");

	_tsplitpath_s(szModulePath, szDrive, szDir, szFName, szExt);

	TCHAR szPath[MAX_PATH]={0,};
	_tmakepath_s( szPath, szDrive, szDir, NULL, NULL );

	OutPut = szPath;

	TCHAR szExeFileName[MAX_PATH]={0,};
	_tmakepath_s( szExeFileName, NULL, NULL, szFName, szExt );
	exeFileName = szExeFileName;
}


// 파일의 리비젼 번호를 가져온다
WORD CAutoUpgradeApp::GetFileRevisionNumber(CString const& FilePath)
{
	CString file = FilePath;
	WORD wRevisionNum = 0;
	DWORD dwHandle;
	// 파일로부터 버전정보데이터의 크기가 얼마인지를 구합니다.
	DWORD infoSize = GetFileVersionInfoSize(file, &dwHandle);
	if(infoSize==0) 
		return wRevisionNum;

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

				wRevisionNum = revisionNum;
			}
		}
		delete[] buffer;
	}

	return wRevisionNum;
}