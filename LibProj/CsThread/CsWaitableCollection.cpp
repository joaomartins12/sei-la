#include "stdafx.h"
#include "CsWaitableCollection.h"

CsWaitableCollection::CsWaitableCollection() 
: m_lObjects( 0 )
{
}

CsWaitableCollection::~CsWaitableCollection()
{

}

CsWaitableCollection::CsWaitableCollection( const CsWaitableCollection & rhs )
{
	m_lObjects = rhs.m_lObjects;
	memcpy( m_aHandles, rhs.m_aHandles, sizeof(HANDLE) * m_lObjects );
}

CsWaitableCollection& CsWaitableCollection::operator=( const CsWaitableCollection & rhs )
{
	m_lObjects = rhs.m_lObjects;
	memcpy( m_aHandles, rhs.m_aHandles, sizeof(HANDLE) * m_lObjects );

	return *this;
}

LONG CsWaitableCollection::GetCount() const
{
	return m_lObjects;
}

void CsWaitableCollection::Clear()
{
	m_lObjects = 0;
}

BOOL CsWaitableCollection::AddObject( const CsWaitableObject *pObject )
{
	if( m_lObjects == MAXIMUM_WAIT_OBJECTS )
		return FALSE;

	m_aHandles[ m_lObjects++ ] = pObject->GetHandle();
	
	return TRUE;
}

BOOL CsWaitableCollection::AddObject( const CsWaitableObject &object )
{
	if( m_lObjects == MAXIMUM_WAIT_OBJECTS )
		return FALSE;

	m_aHandles[ m_lObjects++ ] = object.GetHandle();
	
	return TRUE;
}

BOOL CsWaitableCollection::AddObject( const HANDLE handle )
{
	if( m_lObjects == MAXIMUM_WAIT_OBJECTS )
		return FALSE;

	m_aHandles[ m_lObjects++ ] = handle;

	return TRUE;
}

BOOL CsWaitableCollection::AddCollection( const CsWaitableCollection & rhs )
{
	if( m_lObjects + rhs.m_lObjects > MAXIMUM_WAIT_OBJECTS )
		return FALSE;

	LONG dwRhsObjects = rhs.m_lObjects;
	for( int i = 0; i <= dwRhsObjects; i++ )
		m_aHandles[ m_lObjects++ ] = rhs.m_aHandles[i];

	return TRUE;
}

DWORD CsWaitableCollection::Wait( BOOL bWaitAll, DWORD dwMilliseconds ) const
{
	if( m_lObjects == 0 )
		return WAIT_FAILED;

	return ::WaitForMultipleObjects( m_lObjects, m_aHandles, bWaitAll, dwMilliseconds );
}
