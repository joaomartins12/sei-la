
#include"stdafx.h"
#include"CsEvent.h"

CsEvent::CsEvent( BOOL bManualReset/*= FALSE*/, BOOL bInitState/*= FALSE*/,
						LPCTSTR lpName/*= NULL*/, LPSECURITY_ATTRIBUTES lpSA/*= NULL*/)
{
	m_hHandle = ::CreateEvent( lpSA, bManualReset, bInitState, lpName );

	if( CsKIsValidHandle( m_hHandle ) ) {
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

CsEvent::CsEvent( LPCTSTR lpName, BOOL bInheritHandle/*= FALSE*/, DWORD dwDesiredAccess/*= EVENT_ALL_ACCESS*/)
{
	m_hHandle = ::OpenEvent( dwDesiredAccess, bInheritHandle, lpName );

	if( CsKIsValidHandle( m_hHandle ) ){
		m_dwStatus = NO_ERROR;
	}
	else{
		m_dwStatus = GetLastError();
		ThrowError( m_dwStatus );
	}
}

BOOL CsEvent::Set() const 
{
	return ::SetEvent( m_hHandle );
}

BOOL CsEvent::Reset() const
{
	return ::ResetEvent( m_hHandle );
}