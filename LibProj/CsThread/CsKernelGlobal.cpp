#include "stdafx.h"
#include"CsKernelGlobal.h"

void CsKInternalThrowError( DWORD dwStatus, const char* lpFileName, int line )
{
#ifndef _DEBUG
	char string[2 * MAX_PATH];
	sprintf_s( string, "CsKernel Error 0x%08x(%d) at %s line %d\n",
		dwStatus, dwStatus, lpFileName, line );
#endif

#ifndef __CsKERNEL_THROW_EXCEPTION__
	throw CsKException( dwStatus );
#endif

}

BOOL CsKIsValidHandle( HANDLE hHandle )
{
	return ( ( hHandle != NULL ) && ( hHandle != INVALID_HANDLE_VALUE ) );
}

BOOL CsKWaitSucceeded( DWORD dwWaitResult, DWORD dwHandleCount )
{
	return ( ( dwWaitResult >= WAIT_OBJECT_0 ) && ( dwWaitResult < WAIT_OBJECT_0 + dwHandleCount ) );
}

BOOL CsKWaitAbandoned( DWORD dwWaitResult, DWORD dwHandleCount )
{
	return ( ( dwWaitResult >= WAIT_ABANDONED_0 ) && ( dwWaitResult < WAIT_ABANDONED_0 + dwHandleCount ) );
}

BOOL CsKWaitTimeout( DWORD dwWaitResult )
{
	return ( dwWaitResult == WAIT_TIMEOUT );
}

BOOL CsKWaitFailed( DWORD dwWaitResult )
{
	return ( dwWaitResult == WAIT_FAILED );
}

DWORD CsKWaitSucceededIndex( DWORD dwWaitResult )
{
	return ( dwWaitResult - WAIT_OBJECT_0 );
}

DWORD CsKWaitAbandonedIndex( DWORD dwWaitResult )
{
	return ( dwWaitResult - WAIT_ABANDONED_0 );
}