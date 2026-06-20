// ResourceIntegrityTool.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "ResourceIntegrityTool.h"
#include "IntegrityTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CResourceIntegrityToolApp

BEGIN_MESSAGE_MAP(CResourceIntegrityToolApp, CWinApp)
END_MESSAGE_MAP()


// CResourceIntegrityToolApp 생성

CResourceIntegrityToolApp::CResourceIntegrityToolApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CResourceIntegrityToolApp 개체입니다.

CResourceIntegrityToolApp theApp;


void __stdcall UnicodeToUTF8(std::wstring const& wsSource, std::string& sDest)
{
	if( wsSource.empty() )
		return ;

	int iConverted = ::WideCharToMultiByte( CP_UTF8, 0, wsSource.c_str(), (int)wsSource.size(), NULL, 0, NULL, NULL );
	sDest.resize( iConverted );

	::WideCharToMultiByte( CP_UTF8, 0, wsSource.c_str(), (int)wsSource.size(), &sDest.at( 0 ), iConverted, NULL, NULL );
}

void __stdcall UnicodeToMultiByte(std::wstring const& wsSource, std::string& sDest)
{
	if( wsSource.empty() )
		return ;

	int iConverted = ::WideCharToMultiByte( PO_CP_CODE, 0, wsSource.c_str(), (int)wsSource.size(), NULL, 0, NULL, NULL );
	sDest.resize( iConverted );

	::WideCharToMultiByte( PO_CP_CODE, 0, wsSource.c_str(), (int)wsSource.size(), &sDest.at( 0 ), iConverted, NULL, NULL );
}

void __stdcall UTF8ToUnicode(std::string const& sSource, std::wstring& wsDest)
{
	if( sSource.empty() )
		return ;

	int iConverted = ::MultiByteToWideChar( CP_UTF8, 0, sSource.c_str(), (int)sSource.size(), NULL, 0 );
	wsDest.resize( iConverted );

	::MultiByteToWideChar( CP_UTF8, 0, sSource.c_str(), (int)sSource.size(), &wsDest.at( 0 ), iConverted );
}

void __stdcall MultiByteToUnicode(std::string const& sSource, std::wstring& wsDest)
{
	if( sSource.empty() )
		return ;

	int iConverted = ::MultiByteToWideChar( PO_CP_CODE, 0, sSource.c_str(), (int)sSource.size(), NULL, 0 );
	wsDest.resize( iConverted );

	::MultiByteToWideChar( PO_CP_CODE, 0, sSource.c_str(), (int)sSource.size(), &wsDest.at( 0 ), iConverted );
}

void SetCurDir2ExeFilePos(void)
{
	WCHAR MaxPath[MAX_PATH] = {0,};  // 파일이 있는 위치를 current directory로 해서 처리한다.	
	GetModuleFileName(NULL, MaxPath, MAX_PATH);

	WCHAR path[MAX_PATH] = {0,};  // 파일이 있는 위치를 current directory로 해서 처리한다.	
	nsCSFILE::GetFilePathOnly( path, MAX_PATH, MaxPath );

	//wcsrchr(path, L"\\")[0] = NULL; // 항상 full path값이므로 \\가 존재하지 않을 수 없다.

	SetCurrentDirectory(path);

	//GetCurrentDirectory(MAX_PATH, path);
}

BOOL CResourceIntegrityToolApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	// 문자열 변환 함수 등록
	DmCS::StringFn::SetConvFn( &UnicodeToUTF8,	&UnicodeToMultiByte, &UTF8ToUnicode, &MultiByteToUnicode );

	//CsMessageBoxA( MB_OK, "기다리는 중");

	int iCnt = 0;
	std::wstring hashDir;
	std::wstring makehashOutDir;
	std::wstring uploadZipFile;
	LPWSTR* pStr = CommandLineToArgvW( GetCommandLine(), &iCnt);
	switch(iCnt)
	{
	case 2: uploadZipFile = pStr[1];		break;
	case 4:	makehashOutDir = pStr[1];	hashDir = pStr[2]; uploadZipFile = pStr[3];		break;
	default:
		break;
	}

	SetCurDir2ExeFilePos();
#ifdef	USECONSOLE
	FILE	*pStream = NULL; 
	AllocConsole();
	freopen_s(&pStream, "CONIN$", "r", stdin);
	freopen_s(&pStream, "CONOUT$", "w", stderr);
	freopen_s(&pStream, "CONOUT$", "w", stdout);

	//_crtBreakAlloc = 232352;	//{,,msvcr100d.dll}	_CrtSetBreakAlloc(num);
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	CIntegrityTool* pFrame = new CIntegrityTool;
	if (!pFrame)
		return FALSE;

	pFrame->SetMakeHashDirFile( hashDir, makehashOutDir, uploadZipFile );

	m_pMainWnd = pFrame;
	INT_PTR intptr = pFrame->DoModal();
#ifdef	USECONSOLE
	fclose( pStream );
	FreeConsole();		//할당된 콘솔 해제
	_CrtCheckMemory();
	_CrtMemDumpAllObjectsSince(0);
#endif
	return TRUE;
}

