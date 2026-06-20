
#include "stdafx.h"

#include "MapObjectGroup.h"

void CsMapObjectGroup::Delete()
{
	if( m_mapObject.empty() )
		return;

	OBJECT_MAP_IT itObj = m_mapObject.begin();
	OBJECT_MAP_IT itObjEnd = m_mapObject.end();
	for( ; itObj!=itObjEnd; ++itObj )
	{
		LIST_OBJECT_IT itSec = itObj->second.begin();
		for( ; itSec != itObj->second.end(); ++itSec )
			SAFE_DELETE( *itSec );
	}

	m_mapObject.clear();
}

void CsMapObjectGroup::Init()
{

}

bool CsMapObjectGroup::IsObject( DWORD const& dwObjecIdx ) const
{
	OBJECT_MAP_CIT cIT  = m_mapObject.find( dwObjecIdx );
	if( cIT == m_mapObject.end() )
		return false;
	return true;
}

CsMapObjectGroup::LIST_OBJECT const* CsMapObjectGroup::FindObjectList( DWORD const& dwObjecIdx ) const
{
	OBJECT_MAP_CIT cIT  = m_mapObject.find( dwObjecIdx );
	if( cIT == m_mapObject.end() )
		return NULL;

	return &cIT->second;
}

CsMapObjectGroup::LIST_OBJECT * CsMapObjectGroup::AddNewObject(DWORD const& dwObjecIdx)
{
	auto result = m_mapObject.insert( make_pair(dwObjecIdx, LIST_OBJECT() ) );
	if( !result.second )
		return NULL;

	return &result.first->second;
}

void CsMapObjectGroup::GetActorObjectID( std::list<DWORD>& findObjID ) const
{
	findObjID.clear();

	if( m_mapObject.empty() )
		return;

	OBJECT_MAP_CIT it = m_mapObject.begin();
	for( ; it != m_mapObject.end(); ++it )
		findObjID.push_back( it->first );
}

void CsMapObjectGroup::DeleteObjectFormIdx( DWORD const& dwObjecIdx )
{
	OBJECT_MAP_IT it = m_mapObject.find( dwObjecIdx );
	if( it == m_mapObject.end() )
		return;

	LIST_OBJECT_IT objIT = it->second.begin();
	for( ; objIT != it->second.end(); ++objIT )
		SAFE_DELETE( *objIT );

	m_mapObject.erase( it );
}

void CsMapObjectGroup::DeleteActorFactor( DWORD const& dwObjecIdx, BYTE const& byFactor1 )
{
	OBJECT_MAP_IT it = m_mapObject.find( dwObjecIdx );
	if( it == m_mapObject.end() )
		return;

	LIST_OBJECT_IT objIT = it->second.begin();
	for( int nFirst = 0; objIT != it->second.end(); ++objIT, ++nFirst )
	{
		if( nFirst == byFactor1 )
		{
			SAFE_DELETE( *objIT );
			it->second.erase( objIT );
			break;
		}
	}
		
	if( it->second.empty() )
		m_mapObject.erase( it );
}

void CsMapObjectGroup::DeleteActorFactor( DWORD const& dwObjecIdx, BYTE const& byFactor1, BYTE const& byFactor2 )
{
	OBJECT_MAP_IT it = m_mapObject.find( dwObjecIdx );
	if( it == m_mapObject.end() )
		return;

	LIST_OBJECT_IT objIT = it->second.begin();
	for( int nFirst = 0; objIT != it->second.end(); ++objIT, ++nFirst )
	{
		if( nFirst == byFactor1 )
		{
			(*objIT)->DeleteFacotr( byFactor2 );
			if( (*objIT)->IsEmpty() )
			{
				SAFE_DELETE( *objIT );
				it->second.erase( objIT );
			}
			break;
		}
	}

	if( it->second.empty() )
		m_mapObject.erase( it );
}

int CsMapObjectGroup::GetMaxFactorCount() const
{
	int nMaxFactorCount = 0;
	OBJECT_MAP_CIT it = m_mapObject.begin();
	for( ; it != m_mapObject.end(); ++it )
	{
		LIST_OBJECT_CIT ITobj = it->second.begin();
		for( ; ITobj != it->second.end(); ++ITobj )
		{
			int nCount = (*ITobj)->GetMaxFactorCount();
			if( nMaxFactorCount < nCount )
				nMaxFactorCount = nCount;
		}
	}

	return nMaxFactorCount;
}

CsMapObjectGroup::OBJECT_MAP const&	CsMapObjectGroup::GetMapObject() const
{
	return m_mapObject;
}
