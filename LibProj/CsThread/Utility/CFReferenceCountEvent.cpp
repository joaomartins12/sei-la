#include "stdafx.h"
#include "CFReferenceCountEvent.h"

CFReferenceCountEvent::CFReferenceCountEvent()
: m_lReferenceCount( 0 )
{
	m_hHandle = ::CreateEvent( NULL, TRUE, FALSE, NULL );

	if( CsKIsValidHandle( m_hHandle ) ) {
			m_dwStatus = NO_ERROR;
	}
	else{
		m_dwStatus = GetLastError();
		ThrowError( m_dwStatus );
	}
}

void CFReferenceCountEvent::Reference()
{
	_SYNCHRONIZE_THREAD_( &m_cs );
	m_lReferenceCount++;
	if( m_lReferenceCount == 1 ){
		::ResetEvent( m_hHandle );
	}
}

void CFReferenceCountEvent::Release()
{
	_SYNCHRONIZE_THREAD_( &m_cs );
	m_lReferenceCount--;
	if( m_lReferenceCount == 0 ){
		::SetEvent( m_hHandle );
	}
}