#pragma once

#ifndef __WIN_VERSION_CHECK_
#define __WIN_VERSION_CHECK_

namespace WVS
{
	#define VERSIONHELPERAPI inline bool

	#define _WIN32_WINNT_NT4                    0x0400
	#define _WIN32_WINNT_WIN2K                  0x0500
	#define _WIN32_WINNT_WINXP                  0x0501
	#define _WIN32_WINNT_WS03                   0x0502
	#define _WIN32_WINNT_WIN6                   0x0600
	#define _WIN32_WINNT_VISTA                  0x0600
	#define _WIN32_WINNT_WS08                   0x0600
	#define _WIN32_WINNT_LONGHORN               0x0600
	#define _WIN32_WINNT_WIN7                   0x0601
	#define _WIN32_WINNT_WIN8                   0x0602

	#define _WIN32_WINNT_WINBLUE                0x0603
	#define _WIN32_WINNT_WIN10                  0x0A00


	VERSIONHELPERAPI IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
	{
		RTL_OSVERSIONINFOEXW verInfo = { 0 };
		verInfo.dwOSVersionInfoSize = sizeof( verInfo );

		typedef LONG(WINAPI* tRtlGetVersion)(RTL_OSVERSIONINFOEXW*);

		HMODULE h_NtDll = GetModuleHandleW(L"ntdll.dll");
		tRtlGetVersion f_RtlGetVersion = (tRtlGetVersion)GetProcAddress(h_NtDll, "RtlGetVersion");
		if (!f_RtlGetVersion)
			return false; // This will never happen (all processes load ntdll.dll)

		if (f_RtlGetVersion != 0 && f_RtlGetVersion(&verInfo ) == 0)
		{
			if (verInfo.dwMajorVersion > wMajorVersion)
				return true;
			else if (verInfo.dwMajorVersion < wMajorVersion)
				return false;

			if (verInfo.dwMinorVersion > wMinorVersion)
				return true;
			else if (verInfo.dwMinorVersion < wMinorVersion)
				return false;

			if (verInfo.wServicePackMajor >= wServicePackMajor)
				return true;
		}

		return false;
	}


	VERSIONHELPERAPI IsWindowsXPOrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WINXP ), LOBYTE( _WIN32_WINNT_WINXP ), 0 );
	}

	VERSIONHELPERAPI IsWindowsXPSP1OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WINXP ), LOBYTE( _WIN32_WINNT_WINXP ), 1 );
	}

	VERSIONHELPERAPI IsWindowsXPSP2OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WINXP ), LOBYTE( _WIN32_WINNT_WINXP ), 2 );
	}

	VERSIONHELPERAPI IsWindowsXPSP3OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WINXP ), LOBYTE( _WIN32_WINNT_WINXP ), 3 );
	}

	VERSIONHELPERAPI IsWindowsVistaOrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_VISTA ), LOBYTE( _WIN32_WINNT_VISTA ), 0 );
	}

	VERSIONHELPERAPI IsWindowsVistaSP1OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_VISTA ), LOBYTE( _WIN32_WINNT_VISTA ), 1 );
	}

	VERSIONHELPERAPI IsWindowsVistaSP2OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_VISTA ), LOBYTE( _WIN32_WINNT_VISTA ), 2 );
	}

	VERSIONHELPERAPI IsWindows7OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WIN7 ), LOBYTE( _WIN32_WINNT_WIN7 ), 0 );
	}

	VERSIONHELPERAPI IsWindows7SP1OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WIN7 ), LOBYTE( _WIN32_WINNT_WIN7 ), 1 );
	}

	VERSIONHELPERAPI IsWindows8OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WIN8 ), LOBYTE( _WIN32_WINNT_WIN8 ), 0 );
	}

	VERSIONHELPERAPI IsWindows8Point1OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WINBLUE ), LOBYTE( _WIN32_WINNT_WINBLUE ), 0 );
	}

	VERSIONHELPERAPI IsWindows10OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE( _WIN32_WINNT_WIN10 ), LOBYTE( _WIN32_WINNT_WIN10 ), 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	typedef enum _APPCOMPATFLAG   
	{   
		apcfNone = -1  
		, apcfWin95  
		, apcfWin98  
		, apcfWinNT4SP5  
		, apcfWin2000   
		, acpfWinXpSP2
		, acpfWinXpSP3
		, acpfWinSRV03SP1
		, acpfWinSRV08SP1
		, acpfVistaRTM
		, acpfVistaSP1
		, acpfVistaSP2
		, acpfWin7RTM
		, acpfWin8RTM
	} APPCOMPATFLAG, *PAPPCOMPATFLAG;  


	APPCOMPATFLAG WINAPI GetAppCompatFlag(LPCTSTR szFileName);
	BOOL WINAPI SetAppCompatFlag(LPCTSTR szFileName, APPCOMPATFLAG apcfNewType);
	BOOL WINAPI RemoveAppCompatFlag(LPCTSTR szFileName);

}
#endif