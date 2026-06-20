#include "stdafx.h"
#include "CsMutex.h"

CsMutex::CsMutex( BOOL bInitOwner/*= FALSE*/, LPCTSTR lpName/*= NULL*/, LPSECURITY_ATTRIBUTES lpSA/*= NULL*/)
{
	m_hHandle = ::CreateMutex( lpSA, bInitOwner, lpName );

	if( CsKIsValidHandle( m_hHandle ) )
	{
		if( lpName ){
			m_dwStatus = GetLastError();
		}
		else{
			m_dwStatus = NO_ERROR;
		}
	}
	else
	{
		m_dwStatus = GetLastError();
		ThrowError( m_dwStatus );
	}
}

CsMutex::CsMutex( LPCTSTR lpName, BOOL bInheritHandle/*= FALSE*/, DWORD dwDesiredAccess/*= MUTEX_ALL_ACCESS*/)
{
	m_hHandle = ::OpenMutex( dwDesiredAccess, bInheritHandle, lpName );

	if( CsKIsValidHandle( m_hHandle ) ){
		m_dwStatus = NO_ERROR;
	}
	else{
		m_dwStatus = GetLastError();
		ThrowError( m_dwStatus );
	}
}

BOOL CsMutex::Release()
{
	return ::ReleaseMutex( m_hHandle );
}