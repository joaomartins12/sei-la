
#include "stdafx.h"
#include "CsLog.h"
#include "..\CsFilePack\stdafx.h"

CsLastError		g_CsLastError;

void CsLog::Init( TCHAR const* szLogName, bool bSystemTime, bool bNewFile /*=true*/ )
{
	if( m_bInit == true )
		return;

	m_bInit = true;

	nsCSFILE::CreateDirectory( _T( "Log" ) );

	if( bSystemTime == true )
	{
		SYSTEMTIME st;
		GetLocalTime( &st );
		_stprintf_s( m_szLogName, MAX_PATH, _T( "Log\\%s_%0.2d%0.2d_%0.2d%0.2d%0.2d_%0.3d.txt" ), szLogName, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds );
	}
	else
	{
		_stprintf_s( m_szLogName, MAX_PATH, _T( "Log\\%s.txt" ), szLogName );
	}

	if( bNewFile == true )
	{
		FILE* fp;
		_tfopen_s( &fp, m_szLogName, _T( "wt" ) );	
		fclose( fp );
	}
}

void CsLog::Log( TCHAR const* szMsg, ... )
{
	if( m_bInit == false )
		return;

	TCHAR App[512] = {0 };
	va_list args;
	va_start(args, szMsg);
	_vsntprintf_s( App, 512, szMsg, args );
	va_end(args);

	FILE* fp = _tfsopen( /*&fp, */m_szLogName, _T( "a" ), _SH_DENYNO );
	char cOut[ 512 ];
	W2M( cOut, App, 512 );
	strcat_s( cOut, 512, "\n" );
	fwrite( cOut, sizeof( char ), strlen( cOut ), fp );
	fclose( fp );
}

void CsLog::DeleteFile()
{
	if( m_bInit == false )
		return;

	::DeleteFile( m_szLogName );
}