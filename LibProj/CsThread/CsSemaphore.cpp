#include "stdafx.h"
#include "CsSemaphore.h"

CFSemaphore::CFSemaphore( int nInitialCount, int nMaximumCount,
						 LPCTSTR lpName/* = NULL*/, LPSECURITY_ATTRIBUTES lpsa/* = NULL*/ )
{
	m_hHandle = ::CreateSemaphore( lpsa, nInitialCount, nMaximumCount, lpName );

	if( CsKIsValidHandle( m_hHandle ) ){
		if( lpName )
			m_dwStatus = GetLastError();
		else
			m_dwStatus = NO_ERROR;
	}
	else{
		m_dwStatus = GetLastError();
		ThrowError( m_dwStatus );
	}
}

CFSemaphore::CFSemaphore( LPCTSTR lpName, BOOL bInheritHandle /*= FALSE*/,
						 DWORD dwDesiredAccess /*= SEMAPHORE_ALL_ACCESS*/ )
{
	m_hHandle = ::OpenSemaphore( dwDesiredAccess, bInheritHandle, lpName );
	if( CsKIsValidHandle( m_hHandle ) ){
		m_dwStatus = NO_ERROR;
	}
	else{
		m_dwStatus = GetLastError();
	}
}

BOOL CFSemaphore::Release( LONG lReleaseCount, LONG *plPreviousCount /*= NULL*/ )
{
	LONG lPreviousCount;
	BOOL bStatus = ::ReleaseSemaphore( m_hHandle, lReleaseCount, &lPreviousCount );

	if( bStatus && plPreviousCount ) {
		*plPreviousCount = lPreviousCount;
	}

	return bStatus;
}
