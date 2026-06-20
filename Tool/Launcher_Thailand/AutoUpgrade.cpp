// AutoUpgrade.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "AutoUpgradeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoUpgradeApp

BEGIN_MESSAGE_MAP(CAutoUpgradeApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoUpgradeApp 생성

CAutoUpgradeApp::CAutoUpgradeApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CAutoUpgradeApp 개체입니다.

CAutoUpgradeApp theApp;

//CsLog		g_Log;
// CAutoUpgradeApp 초기화

BOOL CAutoUpgradeApp::InitInstance()
{

#ifdef _DEBUG
	AllocConsole();
	FILE* pkFile;
	freopen_s( &pkFile, "CONOUT$", "wt", stdout );
#endif

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
		{
			WaitForSingleObject( k, INFINITE );			
		}		
	}

// 	g_Log.Init( _T( "CriticalLog" ), false, true );
// 	g_Log.Log( _T( "로그생성" ));

	_trename( NEW_EXE_TEMP, EXE_LAUNCHERFILENAME );
	_trename( NEW_EXE_JUST_TEMP, EXE_LAUNCHERFILENAME );
	hWnd = FindWindow( L"DMO", NULL ) ;
	if( hWnd )
	{
		::MessageBox( m_pMainWnd->GetSafeHwnd(), STR_DO_EXIT_GAME, STR_FAIL, MB_OK|MB_ICONERROR ) ;
		return FALSE ;
	}

	// 트레이 아이콘 종료
	hWnd = FindWindow( 0, WISH_CLOSE_WINDOW );
	if( hWnd )
	{
		SendMessage( hWnd, WM_CLOSE, 0, 0 );
	}

	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

	CWinApp::InitInstance();
	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성한 응용 프로그램"));


	CString currExeName;
	CString currentPath;
	GetCurrentPath( currentPath, currExeName );

	CString LacucherPath;
	LacucherPath.Format( L"%s%s", currentPath, currExeName );
	WORD wVersion = GetFileRevisionNumber(LacucherPath);

	CAutoUpgradeDlg dlg;
	dlg.SetCurrentVersion(wVersion);

	m_pMainWnd = &dlg;


	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 대화 상자가 확인을 눌러 없어지는 경우 처리할
		// 코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 대화 상자가 취소를 눌러 없어지는 경우 처리할
		// 코드를 배치합니다.
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고
	// 응용 프로그램을 끝낼 수 있도록 FALSE를 반환합니다.
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