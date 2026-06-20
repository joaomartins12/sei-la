
#include "StdAfx.h"

#include "cDumper.hpp"

#include "ClientSystemInfo.h"

#ifdef DEF_CORE_NPROTECT
#include "../nProtect/Client_nProtect.h"
#endif

#ifdef SDM_DEF_XIGNCODE3_20181107
#include "../Xigncode3/Client_Xigncode3.h"
#endif

#include "../../SimpleSecurity/SecuritySelf.h"

#include <psapi.h>
#include <tlhelp32.h>

#define MB_10 10485760	// 10MB
#define MB_20 20971520	// 20MB

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)
(
	HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam

);



void GetProcessList( std::list<std::wstring>& processList )
{
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnapshot)
	{
		PROCESSENTRY32 ProcessEntry32;
		BOOL bProcessFound;
		ProcessEntry32.dwSize=sizeof(PROCESSENTRY32);
		bProcessFound=Process32First(hSnapshot, &ProcessEntry32);
		while(bProcessFound)
		{
			processList.push_back( ProcessEntry32.szExeFile );
			bProcessFound=Process32Next(hSnapshot, &ProcessEntry32);
		}
		CloseHandle(hSnapshot);
	}
}	



void GetLoadDllList(std::list<std::wstring>& loadDllList)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;
	unsigned int i;

	// Get a list of all the modules in this process.
	if(EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.
			if (GetModuleFileNameEx(GetCurrentProcess(),hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				loadDllList.push_back( szModName );
				// Print the module name and handle value.
				//TRACE(_T("\t%s (0x%08X)\n"), szModName, hMods[i]);
			}
		}
	}
}


cDumper::cDumper(int lv) : m_DumpLevel(lv)
{
    // 모듈 경로를 알아낸다.
    char szFilename[MAX_PATH];
    ::GetModuleFileNameA(NULL, szFilename, _MAX_PATH);

    // 확장자를 제거한 모듈 경로를 준비해두고...
    char* dot = strrchr(szFilename, '.');
    ::lstrcpynA(m_szAppName, szFilename, (int)(dot - szFilename + 1));

    // 예외 처리 핸들러를 설정한다.
    ::SetUnhandledExceptionFilter(TopLevelFilter);

	m_eCodeState = NORMAL;

	InitializeCriticalSection(&m_Crt);
	m_Locale = _create_locale(LC_ALL, "Kor");

	memset(m_szFaultReason, 0, sizeof(m_szFaultReason));
}



cDumper::~cDumper(void)
{
	m_eCodeState = NORMAL;
	DeleteCriticalSection(&m_Crt);

//	TRACE("Memory Dumper 종료\n");

}

HANDLE cDumper::NewFileHandle( char* szDumpPath )
{
	SYSTEMTIME t;
    ::GetLocalTime(&t);

    // 시간 문자열을 준비한다.
    char szTail[MAX_PATH];
    sprintf_s
	(	szTail,
		"%04d-%02d-%02d %02d-%02d-%02d-%03d", 
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond , t.wMilliseconds
	);

    // 덤프 파일 이름 += 시간 문자열
	::lstrcpyA(szDumpPath, m_szAppName);
	::lstrcatA(szDumpPath, szTail);

	// 덤프 파일 이름 += 확장자
	::lstrcatA(szDumpPath, ".dmp");


    HANDLE hFile = CreateFileA
	(
		szDumpPath, 
		GENERIC_WRITE, 
		FILE_SHARE_WRITE, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL
	);

	return hFile;
}


//////////////////////////////////////////////////////////////////////////////
/// \brief 예외에 대한 정보를 받아서, 미니 덤프 파일을 생성한다. 
/// 
/// SetUnhandledExceptionFilter() API에 의해서 설정되고, 프로세스 내부에서 
/// Unhandled Exception이 발생될 경우, 호출되게 된다. 단 디버거가 붙어있는 
/// 경우, Unhandled Exception Filter는 호출되지 않는다. 이 말은 이 함수 
/// 내부를 디버깅할 수는 없다는 말이다. 이 함수 내부를 디버깅하기 위해서는 
/// 메시지 박스 또는 파일을 이용해야한다.
/// 
/// \param pExceptionInfo 예외 정보
/// \return LONG 이 함수를 실행하고 난 다음, 취할 행동값. 자세한 것은 SEH
/// 문서를 참고하도록.
//////////////////////////////////////////////////////////////////////////////
LONG WINAPI cDumper::TopLevelFilter(EXCEPTION_POINTERS* pExPtrs)
{
#ifdef DEF_CORE_NPROTECT
	nProtect_End();
#endif

#ifdef SDM_DEF_XIGNCODE3_20181107
	XignCode_UnInitialize();
#endif

	return EXCEPTION_CONTINUE_SEARCH;

	/*
    LONG retval  = EXCEPTION_CONTINUE_SEARCH;
    HMODULE hDLL = NULL;
    char szDbgHelpPath[_MAX_PATH] = {0, };

    if(GetModuleFileNameA(NULL, szDbgHelpPath, MAX_PATH))
    {	// 같은 디렉토리에 있는 버전 로드
        char *pSlash = strrchr(szDbgHelpPath, '\\');
        if(pSlash)
		{
			lstrcpyA(pSlash + 1, "DBGHELP.dll");
			hDLL = ::LoadLibraryA(szDbgHelpPath);
        }
    }

	if(hDLL == NULL) 
	{	// 시스템에 설치되어 있는 버전 로드
		hDLL = LoadLibraryA("DBGHELP.dll");
	}

	if(hDLL == NULL)
    {
		return retval;
	}

    // DLL 내부에서 MiniDumpWriteDump API를 찾는다.
	MINIDUMPWRITEDUMP pfnMiniDumpWriteDump = 
		(MINIDUMPWRITEDUMP)GetProcAddress(hDLL, "MiniDumpWriteDump");

	if(pfnMiniDumpWriteDump == NULL)
    {	// 미니덤프 함수를 찾을 수 없음
		return retval;
	}


#ifndef USE_DUMPER	// 덤프 미 사용 시 아예 Write하지 않도록 함.!!!
	return retval;
#endif

	char szDumpPath[ MAX_PATH ] = {0, };
	HANDLE hFile = g_Dumper.NewFileHandle( szDumpPath );

	if (hFile == INVALID_HANDLE_VALUE)
    {
        return retval;
    }

    MINIDUMP_EXCEPTION_INFORMATION ExceptionParam;

    ExceptionParam.ThreadId = ::GetCurrentThreadId();
    ExceptionParam.ExceptionPointers = pExPtrs;
    ExceptionParam.ClientPointers = FALSE;

	BOOL bResult = pfnMiniDumpWriteDump
	(
		GetCurrentProcess(),
		GetCurrentProcessId(), 
		hFile,
		(MINIDUMP_TYPE)g_Dumper.GetDumpLevel(),
		&ExceptionParam,
		NULL,
		NULL
		);	// dump file 생성

	CloseHandle(hFile);

	// 덤프 오류 문자열
	g_Dumper._SetDumpReason( g_Dumper.GetFaultReason(pExPtrs) );

	size_t Byte = g_Dumper.GetCapacity(szDumpPath);
	if( Byte <= MB_10 )	// 10MB 보다 작으면 무조건 덤프 전송
		g_Dumper.SendFTPDump( szDumpPath );
	else
	{
		int nRetval = CsMessageBoxA_Title(MB_OKCANCEL, "버그 리포트", "오류 개선을 위해 개발팀으로 버그 리포트 파일을 전송하시겠습니까?");
		if( nRetval == IDOK )
		{
			g_Dumper.SendFTPDump( szDumpPath );
			CsMessageBoxA_Title(MB_OK, "버그 리포트", "발생한 오류는 빠른 시일 내에 해결할 수 있도록 노력하겠습니다.");
		}
	}
	
	DeleteFileA(szDumpPath);	// 생성이 되었을테니 무조건 덤프 삭제

	if(bResult)
    	return EXCEPTION_EXECUTE_HANDLER;
    
	return retval;
	*/
}

void GetIP( char* cOut, int nOutLen )
{
	WSADATA WsaData;
	if( WSAStartup( 0x202, &WsaData ) == SOCKET_ERROR ) //소켓초기화
	{
		strcpy_s( cOut, nOutLen, "UnKnow" );
		return;
	}

	char myaddr[256];
	PHOSTENT pHostInfo;
	struct sockaddr_in addr;

	//로컬pc의 호스트이름을 표준양식으로받아옴(MSDN)
	gethostname(myaddr, sizeof(myaddr));
	//호스트이름에 대응되는Databas를 PHOSTENT구조체로 리턴
	pHostInfo = gethostbyname(myaddr);

	//호스트데이터베이스에서ip만카피
	if( pHostInfo == NULL )
	{
		strcpy_s( cOut, nOutLen, "UnKnow" );
		return;
	}

	if( pHostInfo->h_addr_list[ 0 ] == NULL )
	{
		strcpy_s( cOut, nOutLen, "UnKnow" );
		return;
	}

	memcpy( &addr.sin_addr, pHostInfo->h_addr_list[ 0 ], pHostInfo->h_length );
	strcpy_s( cOut, nOutLen, inet_ntoa( addr.sin_addr ) );
}


bool cDumper::SendFTPDump( char* szDump )
{	
/*
	cFtpClient ftp;
	if( ftp.Init() == false )
		return false;

	std::string host;	//"15.165.6.107";
	SimpleSecurity::SecuritySelf::DecriptHex("5159645e6d407c6f324970314f2d56264c7a7f393e5f43603b6a5d4622306e25682e6634534d5c4b3c74585548697929782a3877287247732c4e575b27362f21244a7d334150615a6b7e373a653d45233f7b6c5444762b522062427167753568203c56687b677d6a67265a39", host);
	std::string id;//"dumpuser"
	SimpleSecurity::SecuritySelf::DecriptHex( "20335d3e2a267c7f6b5f362e533d6835506d616647783f402c2f6025693965426f3c49592b244a4f4857447d563b623176436a5828387a73467e2d7027523271304d7b6e41455e6c4e5172224c5a345c215b29544b553a64677937752377746520474179704c5844", id);
	std::string pw;//"0W7EM03L+7g7"
	SimpleSecurity::SecuritySelf::DecriptHex( "724f6a5937647d6d3d42506f4570684b356261402b4d4c5d473f753c316c4e785b534622523e237b4976364a276638397f737958282d2f7e33412c44296b3274777c3b435f6e25712151343a57652e55307a2026692a5c5a67545e482456606d766f6c3f706f2b69483f4158", pw);
	if( !ftp.Connect( host.c_str(), 14147, id.c_str(), pw.c_str(), true ) )
	{
		ftp.Close();
		return false;
	}

	char szFTPDir[ 1024 ] = {0, };
	int nVersion = GLOBALDATA_ST.GetDownloadPatchVersion();
	switch( m_eCodeState )
	{
	case NORMAL:		sprintf( szFTPDir, "v1.%04d", nVersion );				break;
	case SERVER:		sprintf( szFTPDir, "SC_v1.%04d", nVersion );			break;
	case GBShutDown:	sprintf( szFTPDir, "GBShutDown_v1.%04d", nVersion );	break;
	}

	ftp.NewDirectory( szFTPDir );
	
	strcat_s( szFTPDir, 1024, "\\" );
	std::string userID = GLOBALDATA_ST.GetAccountID();

	if( userID.empty() )
	{
		char ip[ 64 ] = {0, };
		GetIP( ip, 64 );
		strcat_s( szFTPDir, 1024, ip );
		userID = szFTPDir;
	}

	std::string userSha256 = DmCS::sha256( userID );
	strcat_s( szFTPDir, 1024, userSha256.c_str() );

	ftp.NewDirectory( szFTPDir );
	if( !ftp.SetCurDir( szFTPDir ) )
	{
		ftp.Close();
		return false;
	}

	if( !ftp.PutFile( nsCSFILE::GetFileName( szDump ), szDump ) )
	{
		ftp.Close();
		return false;
	}

	{	//PC 사양 전송
		_SendScreenShot( ftp );
		_SendPCInfo(ftp, szDump);
		_SendLog(ftp,szDump, userID.c_str());


// 		GbDxDiagInfo* pDeviceInfo = xnew GbDxDiagInfo;
// 		if( pDeviceInfo )
// 		{
// 			pDeviceInfo->Initialize();
// 			pDeviceInfo->QueryDxDiagValue();
// 			//pDeviceInfo->Write( szInfo );							// txt 파일 Local에 쓰지 않음.
// 			pDeviceInfo->Destroy();
// 			SAFE_DELETE( pDeviceInfo );
// 		}	
// 
// 		//ftp.PutFile( nsCSFILE::GetFileName( szInfo ), szInfo );	// txt 파일 전송하지 않음.
// 		
	}

	ftp.Close();
	//SendCriticalLog();

	return true;
	*/
	return false;
}

void cDumper::SendCriticalLog()
{
	/*
	if( _taccess( g_CriticalLog.GetLogName(), 0 ) != 0 )
		return;

	cFtpClient ftp;
	if( ftp.Init() == false )
		return;

	std::string host;	//"15.165.6.107";
	SimpleSecurity::SecuritySelf::DecriptHex("2f57762a3b3e7a777d64666b424a21377e20616f5c49324c2c27716d285d7b48225e3d4556517c2d593544673a793f7836472969686a50724665526c6e5b413374395a5f3c4b70624f5854555375347f382b242e402623434d2560314e7330225876242025502d5e392f733c", host);
	std::string id;//"dmuser"
	SimpleSecurity::SecuritySelf::DecriptHex( "4a5347737628694f54273e216f42607755596740623c6e30507e785d35417d36747c4d465144312d452e726a243d49682270657558792c342f32384c33436c6171563b4b7a292b5f665b262557233f6d64396b4e2048525c7b2a375e5a7f3a5265692f254d55", id);
	std::string pw;//"0W7EM03L+7g7"
	SimpleSecurity::SecuritySelf::DecriptHex( "27765b40255f476d4e61297034412f6238596f4237394d6b655d603267304c46435e6c452e4b3a2d496e536826337875353b5274285c2a5658732c2351547d227c7264774a4f50212b24363e713c5569573d7e797f44317a3f7b48206a665a6d6e5e3d2d27703e3b343c344e", pw);
	if( ftp.Connect( host.c_str(), 14147, id.c_str(), pw.c_str(), true ) == false )
		return;

	char szFTPDir[ MAX_PATH ] = {0, };
	strcpy( szFTPDir, "_CriticalLog" );

	ftp.NewDirectory( szFTPDir );
	if( ftp.SetCurDir( szFTPDir ) == false )
		return;

	char szLocalPath[ MAX_PATH ] = {0, };
	GetCurrentDirectoryA( MAX_PATH, szLocalPath );

	char szLocal[ MAX_PATH ] = {0, };
	sprintf_s( szLocal, MAX_PATH, "%s\\%s", szLocalPath, nBase::w2m( g_CriticalLog.GetLogName() ) );

	ftp.PutFile( nsCSFILE::GetFileName( nBase::w2m( g_CriticalLog.GetLogName() ) ), szLocal );

	g_CriticalLog.DeleteFile();
	*/
}

const char *cDumper::GetFaultReason(EXCEPTION_POINTERS* pExPtrs)
{
	static const char s_szFaultReason[][64] = 
	{
		"BAD EXCEPTION POINTERS",
		"EXCEPTION_ACCESS_VIOLATION",
		"EXCEPTION_DATATYPE_MISALIGNMENT",
		"EXCEPTION_BREAKPOINT",
		"EXCEPTION_SINGLE_STEP",
		"EXCEPTION_ARRAY_BOUNDS_EXCEEDED",
		"EXCEPTION_FLT_DENORMAL_OPERAND",
		"EXCEPTION_FLT_DIVIDE_BY_ZERO",
		"EXCEPTION_FLT_INEXACT_RESULT",
		"EXCEPTION_FLT_INVALID_OPERATION",
		"EXCEPTION_FLT_OVERFLOW",
		"EXCEPTION_FLT_STACK_CHECK",
		"EXCEPTION_FLT_UNDERFLOW",
		"EXCEPTION_INT_DIVIDE_BY_ZERO",
		"EXCEPTION_INT_OVERFLOW",
		"EXCEPTION_PRIV_INSTRUCTION",
		"EXCEPTION_IN_PAGE_ERROR",
		"EXCEPTION_ILLEGAL_INSTRUCTION",
		"EXCEPTION_NONCONTINUABLE_EXCEPTION",
		"EXCEPTION_STACK_OVERFLOW",
		"EXCEPTION_INVALID_DISPOSITION",
		"EXCEPTION_GUARD_PAGE",
		"EXCEPTION_INVALID_HANDLE",
		"Microsoft C++ Exception"
	};
    if(IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS))) 
	{
        return s_szFaultReason[0];
	}

    // 간단한 에러 코드라면 그냥 변환할 수 있다.
    switch (pExPtrs->ExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:        return s_szFaultReason[1];
    case EXCEPTION_DATATYPE_MISALIGNMENT:   return s_szFaultReason[2];
    case EXCEPTION_BREAKPOINT:              return s_szFaultReason[3];
    case EXCEPTION_SINGLE_STEP:             return s_szFaultReason[4];
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:   return s_szFaultReason[5];
    case EXCEPTION_FLT_DENORMAL_OPERAND:    return s_szFaultReason[6];
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:      return s_szFaultReason[7];
    case EXCEPTION_FLT_INEXACT_RESULT:      return s_szFaultReason[8];
    case EXCEPTION_FLT_INVALID_OPERATION:   return s_szFaultReason[9];
    case EXCEPTION_FLT_OVERFLOW:            return s_szFaultReason[10];
    case EXCEPTION_FLT_STACK_CHECK:         return s_szFaultReason[11];
    case EXCEPTION_FLT_UNDERFLOW:           return s_szFaultReason[12];
    case EXCEPTION_INT_DIVIDE_BY_ZERO:      return s_szFaultReason[13];
    case EXCEPTION_INT_OVERFLOW:            return s_szFaultReason[14];
    case EXCEPTION_PRIV_INSTRUCTION:        return s_szFaultReason[15];
    case EXCEPTION_IN_PAGE_ERROR:           return s_szFaultReason[16];
    case EXCEPTION_ILLEGAL_INSTRUCTION:     return s_szFaultReason[17];
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:return s_szFaultReason[18];
    case EXCEPTION_STACK_OVERFLOW:          return s_szFaultReason[19];
    case EXCEPTION_INVALID_DISPOSITION:     return s_szFaultReason[20];
    case EXCEPTION_GUARD_PAGE:              return s_szFaultReason[21];
    case EXCEPTION_INVALID_HANDLE:          return s_szFaultReason[22];
    case 0xE06D7363:                        return s_szFaultReason[23];
    default:								break;
    }

    // 뭔가 좀 더 복잡한 에러라면...
	::lstrcpyA(m_szFaultReason, "Unknown"); 
    ::FormatMessageA
	(
        FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
        ::GetModuleHandleA("NTDLL.dll"),
        pExPtrs->ExceptionRecord->ExceptionCode, 
        0,
        m_szFaultReason,
        0,
        NULL
	);

    return m_szFaultReason;
}

int cDumper::GetCapacity( char* szDump )
{
	size_t nByte = 0;

	FILE* pFile;
	fopen_s(&pFile, szDump, "rb");

	if( !pFile )
	{
		return 0;	// 덤프 생성 실패
	}

	fseek(pFile, 0, SEEK_SET);
	nByte = ftell(pFile);			// 용량만 검사
	fclose(pFile);

	return nByte;
}


void cDumper::_SendScreenShot(cFtpClient& ftp)
{
	/*
	SAFE_POINTER_RET( g_pEngine );
	SAFE_POINTER_RET( g_pEngine->m_spRenderer );
	// 폴더가 존재 하지 않는다면 폴더 생성
	nsCSFILE::CreateDirectory( _T( ".\\ScreenShot" ) );

	// 파일 이름 생성
	TCHAR szFileName[ MAX_FILENAME ];
	SYSTEMTIME st;
	GetLocalTime( &st );
	_stprintf_s( szFileName, MAX_FILENAME, _T("ScreenShot\\%.2d%.2d%.2d_%.2d%.2d%.2d.jpg"), st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );	
	std::string cc = nBase::w2m( szFileName );
	NiRenderer::GetRenderer()->LockRenderer();
	bool bResult = g_pEngine->m_spRenderer->SaveScreenShot( cc.c_str(), NiRenderer::FORMAT_JPEG );
	NiRenderer::GetRenderer()->UnlockRenderer();
	ftp.PutFile( nsCSFILE::GetFileName( cc.c_str() ), cc.c_str() );
	DeleteFileA(cc.c_str());
	*/
}

void cDumper::_SendLog( cFtpClient& ftp, char* szDumpPath, char const* pUserID )
{
	/*
	* 
	char szInfo[ MAX_PATH ] = {0, };
	strcpy( szInfo, szDumpPath );
	size_t nLen = strlen( szInfo );
	szInfo[ nLen - 4 ] = NULL;	// .까지 NULL
	strcat( szInfo, "_dmpLog.txt" );

	FILE* fp = NULL;
	fopen_s( &fp, szInfo, "wt" );

	if( fp )
	{
		fprintf(fp, "===============================================================================================\n");
		fprintf(fp, "\t\t\t\t\tDump Log File\n");
		fprintf(fp, "User ID : %s\n", pUserID);
		
		// FormatMessage가 있을 경우
		if( strlen(m_szFaultReason) > 0 )
		{
			fprintf(fp, " EXCEPTION ID : %s\n", m_szFaultReason);
		}
		// 일반 덤프 오류 메시지가 있을 경우
		else if( !m_strDumpReason.empty() )
		{
			fprintf(fp, " EXCEPTION ID : %s\n", m_strDumpReason.c_str());
		}

		fprintf(fp, "===============================================================================================\n");
		
		DumpSync localSync(&m_Crt);
		std::list<std::string>::iterator iter = m_LogStrList.begin();
		std::list<std::string>::iterator iter_end = m_LogStrList.end();

		for( ; iter != iter_end; ++iter )
		{
			fprintf(fp, "%s\n", iter->c_str());
		}

		fprintf(fp, "===============================================================================================\n");

		TCHAR name[MAX_PATH] = {0,};
		GetModuleFileName( NULL, name, MAX_PATH);		
		std::string exeHash = DmCS::sha256_file( name );
		fprintf(fp, "Dump Hash : %s\n", exeHash.c_str());

// 		std::list<std::wstring> processList;
// 		GetProcessList( processList );
// 		std::list<std::wstring>::iterator pcIT = processList.begin();
// 		for( ; pcIT != processList.end(); ++pcIT )
// 			fwprintf(fp, L"%s\n", (*pcIT).c_str());
// 
// 		fprintf(fp, "===============================================================================================\n");
// 
// 		std::list<std::wstring> dllList;
// 		GetLoadDllList( dllList );
// 		std::list<std::wstring>::iterator pcIT2 = dllList.begin();
// 		for( ; pcIT2 != dllList.end(); ++pcIT2 )
// 			fwprintf(fp, L"%s\n", (*pcIT2).c_str());
// 
// 		fprintf(fp, "===============================================================================================\n");
		fclose(fp);

 		ftp.PutFile( nsCSFILE::GetFileName( szInfo ), szInfo );
		DeleteFileA(szInfo);
	}
	*/
}

void cDumper::_SendPCInfo( cFtpClient& ftp, char* szDumpPath )
{
	/*
	SAFE_POINTER_RET( g_pEngine );
	SAFE_POINTER_RET( g_pEngine->m_spRenderer );

	char szInfo[ MAX_PATH ] = {0, };
	strcpy( szInfo, szDumpPath );
	size_t nLen = strlen( szInfo );
	szInfo[ nLen - 4 ] = NULL;	// .까지 NULL
	strcat( szInfo, "_pc.txt" );

	ClientSystemInfo txtInfo;
	txtInfo.Write(szInfo);

 	ftp.PutFile( nsCSFILE::GetFileName( szInfo ), szInfo );
 	DeleteFileA(szInfo);
	*/
}

std::string cDumper::_GetTime()
{
	SYSTEMTIME	st;
	GetLocalTime(&st);

	char buf2[1024] ={0};

	sprintf_s(buf2, "[%4d-%02d-%02d %02d:%02d:%02d]\t", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return std::string(buf2);
}
#ifdef USE_DUMPER
void cDumper::DmpLogA(int nLine, char* szFuncName, const char* format, ... )
{
	DumpSync localSync(&m_Crt);

	if(m_LogStrList.size() > 10000)	// string이 너무 많이 쌓이는 것을 방지하기 위해 10000개 마다 삭제 처리(용량 과부하, 메모리 낭비)
		m_LogStrList.clear();

	char buf[1024] = {0,};

	va_list ap;	
	va_start(ap, format);
	vsprintf_s(buf, format, ap);
	va_end(ap);

	char signString[1024] = {0,};

	// [시간] [DUMPLOG] class::func(line) buffer

	sprintf_s(signString, "%04d. %s[DUMPLOG] %s(%d) %s", m_LogStrList.size(), _GetTime().c_str(), szFuncName, nLine, buf);
	m_LogStrList.push_back(signString);
}

void cDumper::DmpLogW( int nLine, char* szFuncName, const wchar_t* format, ... )
{
	DumpSync localSync(&m_Crt);

	if(m_LogStrList.size() > 10000)	// string이 너무 많이 쌓이는 것을 방지하기 위해 10000개 마다 삭제 처리(용량 과부하, 메모리 낭비)
		m_LogStrList.clear();

	wchar_t buf[1024] = {0,};

	va_list ap;
	va_start(ap, format);
	_vsnwprintf_s_l(buf, _countof(buf), _TRUNCATE, format, m_Locale, ap);
	va_end(ap);

	char cStr[ 2024 ] = {0,};
	W2M( cStr, buf, 1024 );

	char signString[1024] = {0,};

	// [시간] [DUMPLOG] class::func(line) buffer
	sprintf_s(signString, "%04d. %s[DUMPLOG] %s(%d) %s", m_LogStrList.size(), _GetTime().c_str(), szFuncName, nLine, cStr);
	m_LogStrList.push_back(signString);
}
#endif

void cDumper::_SetDumpReason( const char* cstr )
{
	m_strDumpReason = cstr;
}
