

#pragma once

#include <dbghelp.h>
#include "cFtpClient.h"
class cDumper
{
public:
	enum eLevel	// or 연산으로.. 중복 설정 가능,
    {
		LEVEL_0  = MiniDumpNormal,		// 충분한 지식이 없다면, LEVEL_0 만 사용하세요.
		LEVEL_1  = MiniDumpWithDataSegs,
		LEVEL_2  = MiniDumpWithFullMemory,
/*
		LEVEL_3  = MiniDumpWithHandleData,
		LEVEL_4  = MiniDumpFilterMemory,
		LEVEL_5  = MiniDumpScanMemory,
		LEVEL_6  = MiniDumpWithUnloadedModules,
		LEVEL_7  = MiniDumpWithIndirectlyReferencedMemory,
		LEVEL_8  = MiniDumpFilterModulePaths,
		LEVEL_9  = MiniDumpWithProcessThreadData,
		LEVEL_10 = MiniDumpWithPrivateReadWriteMemory,
		LEVEL_11 = MiniDumpWithoutOptionalData
//*/
    };


private:
//	cDumper(int lv = LEVEL_0 | LEVEL_1 | LEVEL_2);
	cDumper(int lv = LEVEL_0);
   ~cDumper(void);

public:
	HANDLE NewFileHandle( char* szDumpPath );
	int GetDumpLevel(void) { return m_DumpLevel; }

private:
    // 예외에 대한 정보를 받아서, 미니 덤프 파일을 생성한다. 
	static LONG WINAPI TopLevelFilter(EXCEPTION_POINTERS* pExPtrs);

    // 예외의 원인에 대한 문자열을 반환한다.
	const char *GetFaultReason(EXCEPTION_POINTERS* pExPtrs);

	bool		SendFTPDump( char* szDump );
	int			GetCapacity( char* szDump );


public:
	enum eCODE_STATE{ NORMAL, SERVER, GBShutDown, };
	eCODE_STATE		m_eCodeState;

public:
	void		SendCriticalLog();
	void		SetCodeState( eCODE_STATE eCodeState ){ m_eCodeState = eCodeState; }

private:
	int m_DumpLevel;
	char m_szAppName[MAX_PATH];	// 덤프 파일 이름 문자열.
	char m_szFaultReason[2048];	// 알려지지 않은 오류 formatmessage 보관
	std::string m_strDumpReason;// 덤프가 발생한 오류
	
	
// 덤프 로그
private:
	CRITICAL_SECTION		m_Crt;			// 로그 전용 sync객체
	std::list<std::string>	m_LogStrList;	// 덤프 발생시 Write
	_locale_t				m_Locale;

	void		_SetDumpReason(const char* cstr);
	void		_SendLog(cFtpClient& ftp, char* szDumpPath, char const* pUserID);
	void		_SendPCInfo(cFtpClient& ftp,char* szDumpPath);
	void		_SendScreenShot(cFtpClient& ftp);
	std::string _GetTime();					// 시간 문자열 생성

private:
	class DumpSync
	{
		LPCRITICAL_SECTION	m_pCrt;
	public:
		DumpSync(LPCRITICAL_SECTION pCrt) : m_pCrt(pCrt)
		{
			EnterCriticalSection(m_pCrt);
		}
		~DumpSync()
		{
			LeaveCriticalSection(m_pCrt);
		}
	};
#ifdef USE_DUMPER
public:
	void DmpLogA(int nLine, char* szFuncName, const char* format, ...);
	void DmpLogW(int nLine, char* szFuncName, const wchar_t* format, ...);
#endif	


public:
	static cDumper &GetInstance(void)
	{
		static cDumper s_Dumper;
		return s_Dumper;
	}
};

#define g_Dumper cDumper::GetInstance()

#ifdef USE_DUMPER
	#define DUMPLOG( format, ... ) g_Dumper.DmpLogW(__LINE__, __FUNCTION__, TEXT(format), __VA_ARGS__ )	// ex) DUMPLOGW("%d %s", n, L"wstr");
	#define DUMPLOGA( format, ... ) g_Dumper.DmpLogA(__LINE__, __FUNCTION__, format, __VA_ARGS__ )	// ex) DUMPLOGA("%d %s", n, c);
	#define DUMPLOGW( format, ... ) g_Dumper.DmpLogW(__LINE__, __FUNCTION__, TEXT(format), __VA_ARGS__ )
	
#else
	#define DUMPLOG( format, ... )
	#define DUMPLOGA( format, ... )
	#define DUMPLOGW( format, ... )
#endif