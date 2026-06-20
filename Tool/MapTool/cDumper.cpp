

#include "StdAfx.h"

#if 0
#include "cDumper.hpp"

//#include "base.h"




// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)
(
	HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
);





cDumper::cDumper(int lv) : m_DumpLevel(lv)
{
	TRACE("Memory Dumper 가동\n");

    // 모듈 경로를 알아낸다.
    char szFilename[MAX_PATH];
    ::GetModuleFileNameA(NULL, szFilename, _MAX_PATH);

    // 확장자를 제거한 모듈 경로를 준비해두고...
    char* dot = strrchr(szFilename, '.');
    ::lstrcpynA(m_szAppName, szFilename, (int)(dot - szFilename + 1));

    // 예외 처리 핸들러를 설정한다.
    ::SetUnhandledExceptionFilter(TopLevelFilter);
}



cDumper::~cDumper(void)
{


//	TRACE("Memory Dumper 종료\n");

}



HANDLE cDumper::NewFileHandle(void)
{
    char szDumpPath[MAX_PATH] = {0,};

	SYSTEMTIME t;
    ::GetLocalTime(&t);

    // 시간 문자열을 준비한다.
    char szTail[MAX_PATH];
    sprintf_s
	(	szTail,
		"%04d-%02d-%02d %02d-%02d-%02d", 
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond
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
    LONG retval  = EXCEPTION_CONTINUE_SEARCH;
    HMODULE hDLL = NULL;
    char szDbgHelpPath[_MAX_PATH] = {0, };

    if(GetModuleFileNameA(NULL, szDbgHelpPath, MAX_PATH))
    {	// 같은 디렉토리에 있는 버전 로드
        char *pSlash = strrchr(szDbgHelpPath, '\\');
        if(pSlash)
		{
			lstrcpyA(pSlash + 1, "DBGHELP.DLL");
			hDLL = ::LoadLibraryA(szDbgHelpPath);
        }
    }

	if(hDLL == NULL) 
	{	// 시스템에 설치되어 있는 버전 로드
		hDLL = LoadLibraryA("DBGHELP.DLL");
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

	HANDLE hFile = g_Dumper.NewFileHandle();

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

	if(bResult)
    {
		return EXCEPTION_EXECUTE_HANDLER;
    }

	//xlog.out("Exception::%s\r\n", g_Dumper.GetFaultReason(pExPtrs));

	return retval;
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
        ::GetModuleHandleA("NTDLL.DLL"),
        pExPtrs->ExceptionRecord->ExceptionCode, 
        0,
        m_szFaultReason,
        0,
        NULL
	);

    return m_szFaultReason;
}

#endif
