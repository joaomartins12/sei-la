

#pragma once

#if 0

#include <dbghelp.h>

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
	HANDLE NewFileHandle(void);
	int GetDumpLevel(void) { return m_DumpLevel; }

private:
    // 예외에 대한 정보를 받아서, 미니 덤프 파일을 생성한다. 
	static LONG WINAPI TopLevelFilter(EXCEPTION_POINTERS* pExPtrs);

    // 예외의 원인에 대한 문자열을 반환한다.
	const char *GetFaultReason(EXCEPTION_POINTERS* pExPtrs);


private:
	int m_DumpLevel;
	char m_szAppName[MAX_PATH];	// 덤프 파일 이름 문자열.
	char m_szFaultReason[2048];	// 덤프가 발생한 이유



public:
	static cDumper &GetInstance(void)
	{
		static cDumper s_Dumper;
		return s_Dumper;
	}
};

#define g_Dumper cDumper::GetInstance()

#endif
