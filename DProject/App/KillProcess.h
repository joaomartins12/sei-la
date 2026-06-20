//---------------------------------------------------------------------------
//
// 파일명 : KillProcess.h
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#pragma once

#include <psapi.h>
#include <tlhelp32.h>

#pragma comment (lib, "psapi")

#include "..\LibProj\CsFunc\CsMessageBox.h"

bool GetProcessModule( DWORD const& CurrentPid, DWORD const& dwPID, const _TCHAR *p_ProcessName )
{
	HANDLE hModuleSnap = NULL;
	MODULEENTRY32 me32 = {0};

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwPID);
	if(hModuleSnap == (HANDLE)-1)
		return false;

	me32.dwSize = sizeof(MODULEENTRY32);

	if(Module32First(hModuleSnap,&me32))
	{
		do 
		{
			if( me32.th32ProcessID != CurrentPid && wcscmp(me32.szModule , p_ProcessName) == 0)
			{
				CloseHandle(hModuleSnap);
				return true;
			}
		} while (Module32Next(hModuleSnap,&me32));
	}

	CloseHandle(hModuleSnap);

	return false;
}

bool GetCurrentProcessName( const DWORD& dwPID, WCHAR * szModuleName )
{
	bool bResult = false;
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,dwPID);
	if(hModuleSnap == (HANDLE)-1)
		return bResult;

	MODULEENTRY32 me32 = {0};
	me32.dwSize = sizeof(MODULEENTRY32);
	if(Module32First(hModuleSnap,&me32))
	{
		memcpy( szModuleName, &me32.szModule, sizeof( WCHAR ) * (MAX_MODULE_NAME32 + 1));
		bResult = true;
	}

	CloseHandle(hModuleSnap);
	return bResult;
}

bool ProcessKill( int nCount = 1,				// 중복 허용 갯수
				  bool bKillProcess = false )	// 중복된 놈들을 지울 것인지 물어본다
{
	
	WCHAR szProcessName[MAX_MODULE_NAME32 + 1];
	memset( szProcessName, 0, sizeof( WCHAR ) * (MAX_MODULE_NAME32 + 1) );
	DWORD dwPID = GetCurrentProcessId();
	if( !GetCurrentProcessName(dwPID, szProcessName) )
		return false;	// 자신의 프로세스 이름을 얻어오지 못한경우

	HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS,0 );	
	if( hProcessSnap == (HANDLE)-1 )
		return false;

	PROCESSENTRY32 pe32 = {0};
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if( Process32First(hProcessSnap, &pe32) )
	{
		BOOL bCurret = FALSE;
		MODULEENTRY32 me32 = {0};
		int nDuplicateCount = 0;	// 중복 실행된 프로세스 갯수
		do 
		{
			bCurret = GetProcessModule(dwPID, pe32.th32ProcessID, szProcessName);
			if( bCurret )
			{
				nDuplicateCount++;
				if( nDuplicateCount < nCount )
					continue;

				if( !bKillProcess ) // 중복된 프로세스가 존재하면 프로그램을 실행시키지 않는다.
				{
					CloseHandle(hProcessSnap);
					CsMessageBox( MB_OK, _LAN( "디지몬마스터즈가 이미 실행중에 있습니다.\n불법 프로그램 사용시 불이익을 당할 수 있습니다." ) );
					return false;
				}

				HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS , FALSE , pe32.th32ProcessID );
				if( hProcess )
				{
					int nValue = CsMessageBox( MB_OKCANCEL, _LAN( "하나 이상의 프로그램이 발견 되었습니다,\n이전 프로그램이 종료 됩니다 계속 하시겠습니까?" ) );
					if ( nValue == IDOK )
					{
						if(TerminateProcess(hProcess,0))
						{
							unsigned long nCode;
							GetExitCodeProcess(hProcess,&nCode);
						}
						CloseHandle(hProcess);
					}
					else
					{
						CloseHandle(hProcess);
						CloseHandle(hProcessSnap);
						return false;
					}
				}
			}

		} while (Process32Next(hProcessSnap,&pe32));
	}

	CloseHandle(hProcessSnap);

	return true;
}