#include "StdAfx.h"
#include "ClientSystemInfo.h"
#include <NiDX9SystemDesc.h>
ClientSystemInfo::ClientSystemInfo(void)
{
}

ClientSystemInfo::~ClientSystemInfo(void)
{
	if( m_pkDirectDraw7 )
		m_pkDirectDraw7->Release();
}

void ClientSystemInfo::Write( char* szPath )
{
	FILE* fp = NULL;
	fopen_s( &fp, szPath, "wt" );

	if( !fp )
		return;

	_WriteGraphicInfo(fp);
	_Write_OS_CPUInfo(fp);

	fclose(fp);
}

void ClientSystemInfo::_WriteGraphicInfo( FILE* pkFile )
{
	D3DADAPTER_IDENTIFIER9 d3dAdapter = g_pEngine->m_spRenderer->GetAdapterDesc()->GetAdapterIdentifier();

	if(FAILED(DirectDrawCreateEx(NULL, (VOID**)&m_pkDirectDraw7, IID_IDirectDraw7, NULL)))
	{
		fprintf(pkFile, "Undefined Graphic Device Info");
		return;
	}

	DDSCAPS2	ddsCaps;
	memset(&ddsCaps, 0, sizeof(DDSCAPS2));
	ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;

	DWORD dwTotalMem , dwFreeMem;
	m_pkDirectDraw7->GetAvailableVidMem(&ddsCaps, &dwTotalMem, &dwFreeMem);
	dwTotalMem /= 1024;
	dwFreeMem /= 1024;

	// Write
	fprintf(pkFile, "Graphic Device - BEGIN\n");
	fprintf(pkFile, "========================================================================================\n");
	fprintf(pkFile, "Graphic Card : %s\n", d3dAdapter.Description);
	fprintf(pkFile, "Total Video RAM : %d / Free Video RAM : %d\n", dwTotalMem, dwFreeMem);
	fprintf(pkFile, "========================================================================================\n");
	fprintf(pkFile, "Graphic Device - END\n\n\n");
}

void ClientSystemInfo::_Write_OS_CPUInfo( FILE* pkFile )
{
	SYSTEM_INFO	SysInfo;
	GetSystemInfo(&SysInfo);

	DWORD dwProcessorNum = SysInfo.dwNumberOfProcessors;	// 프로세서 개수

	fprintf(pkFile, "OS & CPU Performance - BEGIN\n");
	fprintf(pkFile, "========================================================================================\n");
	fprintf(pkFile, "OS : %s %s\n", _GetWindowsVersion().c_str() , _GetBit().c_str());
	fprintf(pkFile, "CPU : %s x %d\n", _GetCPUModel().c_str(), dwProcessorNum);
	fprintf(pkFile, _GetMemoryInfo().c_str());
	fprintf(pkFile, "========================================================================================\n");
	fprintf(pkFile, "OS & CPU Performance - END\n");
}

std::string ClientSystemInfo::_GetCPUModel()
{
	char Cpu_info[128];
	memset(Cpu_info, 0, sizeof(Cpu_info));
	{
		HKEY hKey;	
		DWORD c_size = sizeof(Cpu_info);

		if( RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey ) )
		{
			return std::string("Undefined CPU");
		}
		
		RegQueryValueExA(hKey, "ProcessorNameString", NULL, NULL, (LPBYTE)Cpu_info, &c_size );
		RegCloseKey(hKey);
	}

	return std::string(Cpu_info);
}

std::string ClientSystemInfo::_GetWindowsVersion()
{
	char szBuf[128];
	memset(szBuf, 0, sizeof(szBuf));
	std::string strResult;

	HKEY hKey;	
	DWORD c_size = sizeof(szBuf);

	if( RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey ) != ERROR_SUCCESS )
	{
		return std::string("Undefined OS");
	}

	// 성공이면 작성
	if( RegQueryValueExA(hKey, "ProductName", NULL, NULL, (LPBYTE)szBuf, &c_size ) == ERROR_SUCCESS )
	{
		strResult += szBuf;
	}

	if( RegQueryValueExA(hKey, "CSDVersion", NULL, NULL, (LPBYTE)szBuf, &c_size ) == ERROR_SUCCESS )
	{
		strResult += "  ";
		strResult += szBuf;
	}
	RegCloseKey(hKey);

	return strResult;
}

std::string ClientSystemInfo::_GetMemoryInfo()
{
	#define DEVIDE 1024

	MEMORYSTATUS	MemStatus;
	MemStatus.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&MemStatus);

	std::string strMem;

	{
		char szBuf[128];
		sprintf_s(szBuf, "Physical Memory : %d / %d\n", MemStatus.dwAvailPhys/DEVIDE , MemStatus.dwTotalPhys/DEVIDE);
		strMem += szBuf;

	}

	{
		char szBuf[128];
		sprintf_s(szBuf, "Virtual Memory : %d / %d\n", MemStatus.dwAvailVirtual/DEVIDE , MemStatus.dwTotalVirtual/DEVIDE);
		strMem += szBuf;

	}

	{
		char szBuf[128];
		sprintf_s(szBuf, "PageFile Memory : %d / %d\n", MemStatus.dwAvailPageFile/DEVIDE , MemStatus.dwTotalPageFile/DEVIDE);
		strMem += szBuf;

	}

	return strMem;
}

std::string ClientSystemInfo::_GetBit()
{
	char szBuf[128];
	memset(szBuf, 0, sizeof(szBuf));
	std::string strResult;

	HKEY hKey;	
	DWORD c_size = sizeof(szBuf);

	if( RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKey ) )
	{
		return std::string("Undefined Bit");
	}

	
	if( RegQueryValueExA(hKey, "Identifier", NULL, NULL, (LPBYTE)szBuf, &c_size ) == ERROR_SUCCESS )
	{
 		if( strstr(szBuf, "86") )
 		{
 			strResult = " 32Bit";
 		}
 
 		if( strstr(szBuf, "64"))
 		{
 			strResult = " 64Bit";
 		}
	}

	RegCloseKey(hKey);

	return strResult;
}


