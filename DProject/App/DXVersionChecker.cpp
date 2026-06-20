//---------------------------------------------------------------------------
//
// 파일명 : DXVersionChecker.cpp
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#include "stdafx.h"
#include "DXVersionChecker.h"
#include "Dsetup.h"
#include <shellapi.h>


//---------------------------------------------------------------------------
DXVersionChecker::DXVersionChecker()
{
}
//---------------------------------------------------------------------------
DXVersionChecker::~DXVersionChecker()
{
}
//---------------------------------------------------------------------------
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 

BOOL IsWow64()
{
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle(TEXT("kernel32")),"IsWow64Process" );
	if( NULL != fnIsWow64Process )
	{
		if( !fnIsWow64Process(GetCurrentProcess(),&bIsWow64) )
		{
				
		}
	}
	return bIsWow64;
}
DXVersionChecker::VERSION_RESULT DXVersionChecker::CheckDXVersionRequirement( DWORD p_dwDXVersion , DWORD p_dwRevision )
{
	DWORD dwVersion = 0;
	DWORD dwRevision = 0;
	INT nResult = DirectXSetupGetVersion( &dwVersion, &dwRevision );
	if( 0 == nResult )
		return DX_RESULT_DO_NOT_DXINSTALL;

	// 리비전 체크
	if( p_dwDXVersion > LOWORD(dwVersion) || p_dwRevision > LOWORD(dwRevision) )
		return DX_RESULT_NEED_DXVERSION_UP;
	
	wchar_t cSystemPath[MAX_PATH]={0,};
	if( IsWow64() )
		GetSystemWow64DirectoryW( cSystemPath, MAX_PATH );
	else
		GetSystemDirectoryW(cSystemPath,MAX_PATH);

	if( lstrlenW( cSystemPath ) == 0 )
		return DX_RESULT_DO_NOT_DXINSTALL;

	// D3DX9_xx.DLL 찾기
	wstring DLLFiles[5]={ L"D3DCompiler_43.dll",L"D3DX9_40.DLL",L"D3DX9_41.DLL",L"D3DX9_42.DLL",L"D3DX9_43.DLL"};
	for( int i = 0 ; i < 5 ; i++ ) 
	{
		wstring NeedFilePath = cSystemPath;
		NeedFilePath+=L"\\";
		NeedFilePath+=DLLFiles[i].c_str();

		// 버전정보를 담을 버퍼
		const wchar_t *pFileName = NeedFilePath.c_str();
		// 파일로부터 버전정보데이터의 크기가 얼마인지를 구합니다.
		DWORD infoSize = GetFileVersionInfoSizeW(pFileName, 0);
		if(infoSize==0) 
			return DX_RESULT_NEED_DXVERSION_UP;
	}
	return DX_RESULT_Ok_DXVERSION;
}

//---------------------------------------------------------------------------
DXVersionChecker::VERSION_RESULT DXVersionChecker::CheckOSVersionRequirement()
{
	// OS 버전에 따라 최소 버전 이 다름
	OSVERSIONINFO ver;
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&ver);

	// 비스타 || XP 서비스팩 3
	if( ver.dwMajorVersion >= 6 || (ver.dwMajorVersion == 5 && ver.dwMinorVersion == 1 && _tcsstr(ver.szCSDVersion, _T("Service Pack 3"))))
		return OS_RESULT_OK;

	// 그이하
	return OS_RESULT_UNSUPPORTED_OS;
}

bool DXVersionChecker::DoDxSettup( TCHAR* pDoFileName )
{
	SHELLEXECUTEINFO si;                            
	ZeroMemory(&si, sizeof(SHELLEXECUTEINFO));      
	si.cbSize = sizeof(SHELLEXECUTEINFO);           

	si.fMask = SEE_MASK_NOCLOSEPROCESS;             
	si.lpVerb = L"runas";                   // 동작 형식(파일열기)
	si.lpFile =pDoFileName;					// 실행할 파일 경로 지정
	si.nShow = SW_SHOWNORMAL;               // 표시 형식

	if( ShellExecuteEx(&si) )              // 외부 프로그램 호출
		return true;

	DWORD dwErr = GetLastError();
	switch( dwErr )
	{
	case NO_ERROR:
		return true;
	case ERROR_FILE_NOT_FOUND:		CsMessageBox( MB_OKCANCEL, L"ERROR_FILE_NOT_FOUND : %s", pDoFileName );	return false;
	case ERROR_PATH_NOT_FOUND:		CsMessageBox( MB_OKCANCEL, L"ERROR_PATH_NOT_FOUND : %s", pDoFileName );return false;
	case ERROR_NOT_ENOUGH_MEMORY:	CsMessageBox( MB_OKCANCEL, L"ERROR_NOT_ENOUGH_MEMORY : %s", pDoFileName );	return false;
	case ERROR_OUTOFMEMORY:			CsMessageBox( MB_OKCANCEL, L"ERROR_OUTOFMEMORY : %s", pDoFileName );	return false;
	case ERROR_BAD_EXE_FORMAT:		CsMessageBox( MB_OKCANCEL, L"ERROR_BAD_EXE_FORMAT : %s", pDoFileName );	return false;
	default:
		break;
	}
	return true;
}
//----------------------------------------------------------------