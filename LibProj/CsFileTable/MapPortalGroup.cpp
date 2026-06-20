
#include "stdafx.h"
#include "MapPortalGroup.h"

void CsMapPortalGroup::Delete()
{
	CsMapPortal::MAP_IT itPortal = m_mapMapPortal.begin();
	CsMapPortal::MAP_IT itPortalEnd = m_mapMapPortal.end();
	for( ; itPortal!=itPortalEnd; ++itPortal )
	{
		itPortal->second->Delete();
		delete itPortal->second;
	}
	m_mapMapPortal.clear();
}

void CsMapPortalGroup::DeleteNpcPortal_ByObjectID( DWORD dwUniqID, int nKindIndex )
{
	CsMapPortal::MAP_IT itPortal = m_mapMapPortal.begin();
	CsMapPortal::MAP_IT itPortalEnd = m_mapMapPortal.end();
	for( ; itPortal!=itPortalEnd; ++itPortal )
	{
		if( itPortal->second->GetInfo()->s_ePortalType != CsMapPortal::NPC )
			continue;

		if( ( itPortal->second->GetInfo()->s_dwUniqObjectID == dwUniqID )&&
			( itPortal->second->GetInfo()->s_nPortalKindIndex == nKindIndex ) )
		{
			itPortal->second->Delete();
			delete itPortal->second;
			m_mapMapPortal.erase( itPortal );
			return;
		}		
	}
}

void CsMapPortalGroup::Init()
{

}

void CsMapPortalGroup::AddPortal( CsMapPortal* pSrcPortal )
{
	m_mapMapPortal[ pSrcPortal->GetInfo()->s_dwPortalID ] = pSrcPortal;
}

CsMapPortal* CsMapPortalGroup::GetPortal_ByObjectID( DWORD dwUniqID )
{
	CsMapPortal::MAP_IT it = m_mapMapPortal.begin();
	CsMapPortal::MAP_IT itEnd = m_mapMapPortal.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->GetInfo()->s_dwUniqObjectID == dwUniqID )
			return it->second;
	}
	return NULL;
}

CsMapPortal* CsMapPortalGroup::GetNpcPortal_ByObjectID( DWORD dwUniqID, int nKindIndex )
{
	CsMapPortal::MAP_IT it = m_mapMapPortal.begin();
	CsMapPortal::MAP_IT itEnd = m_mapMapPortal.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->GetInfo()->s_ePortalType != CsMapPortal::NPC )
			continue;

		if( ( it->second->GetInfo()->s_dwUniqObjectID == dwUniqID )&&
			( it->second->GetInfo()->s_nPortalKindIndex == nKindIndex) )
			return it->second;
	}
	return NULL;
}

CsMapPortal* CsMapPortalGroup::GetPortal_ByPortalID( DWORD dwPortalID )
{
	if( m_mapMapPortal.find( dwPortalID ) != m_mapMapPortal.end() )
	{
		return m_mapMapPortal[ dwPortalID ];
	}
	return NULL;
}

CsMapPortal* CsMapPortalGroup::GetPortal_ByPos( int x, int y )
{
	int dx, dy;
	CsMapPortal::MAP_IT it = m_mapMapPortal.begin();
	CsMapPortal::MAP_IT itEnd = m_mapMapPortal.end();
	for( ; it!=itEnd; ++it )
	{
		dx = it->second->GetInfo()->s_nDestTargetX - x;
		dy = it->second->GetInfo()->s_nDestTargetY - y;
		if( dx*dx + dy*dy < 2000*2000 )
			return it->second;
	}
	return NULL;
}

CsMapPortal* CsMapPortalGroup::GetNearPortal( DWORD nTargetMap, int x, int y )
{
	UINT dx, dy;
	CsMapPortal::MAP_IT it = m_mapMapPortal.begin();
	CsMapPortal::MAP_IT itEnd = m_mapMapPortal.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->GetInfo()->s_dwDestMapID != nTargetMap )
			continue;

		dx = abs( it->second->GetInfo()->s_nDestTargetX - x );
		dy = abs( it->second->GetInfo()->s_nDestTargetY - y );

		UINT nRadius = it->second->GetInfo()->s_nDestRadius + 1000;
		if( ( dx > nRadius )||( dy > nRadius ) )
			continue;

		// 임시공간 조금을 주겠다
		if( sqrt( (long double)( dx*dx ) + (long double)( dy*dy ) ) < nRadius )
			return it->second;
	}
	return NULL;
}