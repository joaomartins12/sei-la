
#include "stdafx.h"
#include "MapRegionGroup.h"

void CsMapRegionGroup::Delete()
{
	CsMapRegion::LIST_IT it = m_list_MapRegion.begin();
	for( ; it!=m_list_MapRegion.end(); ++it )
	{
		(*it)->Delete();
		delete (*it );
	}
	m_list_MapRegion.clear();
}

void CsMapRegionGroup::Init()
{
}

void CsMapRegionGroup::AddRegion( CsMapRegion::sINFO* pSrcInfo )
{
	CsMapRegion* pMapRegion = csnew CsMapRegion;
	pMapRegion->Init( pSrcInfo );
	m_list_MapRegion.push_back( pMapRegion );
}

void CsMapRegionGroup::DeleteRegion( CsMapRegion* pMapRegion )
{
	CsMapRegion::LIST_IT it = m_list_MapRegion.begin();
	for( ; it!=m_list_MapRegion.end(); ++it )
	{
		if( (*it) == pMapRegion )
		{
			pMapRegion->Delete();
			delete pMapRegion;
			m_list_MapRegion.erase( it );
			return;
		}
	}
	assert_cs( false );
}

CsMapRegion* CsMapRegionGroup::Get( int nIndex )
{
	CsMapRegion::LIST_IT it = m_list_MapRegion.begin();
	CsMapRegion::LIST_IT itEnd = m_list_MapRegion.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return *it;
		--nIndex;
	}

	assert_cs( false );
	return NULL;
}

int CsMapRegionGroup::FindIndex( CsMapRegion* pFT )
{
	int nIndex = 0;
	CsMapRegion::LIST_IT it = m_list_MapRegion.begin();
	CsMapRegion::LIST_IT itEnd = m_list_MapRegion.end();
	for( ; it!=itEnd; ++it )
	{
		if( *it == pFT )
			return nIndex;

		++nIndex;
	}

	assert_cs( false );
	return 0;
}

//======================================================================================================
//
//		ХЌЗа
//
//======================================================================================================

void CsMapRegionGroup::Clone( CsMapRegionGroup* pSrcGroup )
{
	assert_cs( m_list_MapRegion.empty() == true );

	CsMapRegion::LIST* pSrc = pSrcGroup->GetRegionList();
	CsMapRegion::LIST_IT itSrc = pSrc->begin();
	for( ; itSrc!=pSrc->end(); ++itSrc )
	{
		CsMapRegion* pChild = csnew CsMapRegion;
		pChild->Init( (*itSrc)->GetInfo() );
		m_list_MapRegion.push_back( pChild );
	}
}












void CsMap_LimitEvoGroup::Delete()
{
	CsMap_LimitEvo::LIST_IT it = m_list_MapRegion.begin();
	for( ; it!=m_list_MapRegion.end(); ++it )
	{
		(*it)->Delete();
		delete (*it );
	}
	m_list_MapRegion.clear();
}

void CsMap_LimitEvoGroup::Init()
{
}

void CsMap_LimitEvoGroup::AddRegion( CsMap_LimitEvo::sINFO* pSrcInfo )
{
	CsMap_LimitEvo* pMapRegion = csnew CsMap_LimitEvo;
	pMapRegion->Init( pSrcInfo );
	m_list_MapRegion.push_back( pMapRegion );
}

void CsMap_LimitEvoGroup::DeleteRegion( CsMap_LimitEvo* pMapRegion )
{
	CsMap_LimitEvo::LIST_IT it = m_list_MapRegion.begin();
	for( ; it!=m_list_MapRegion.end(); ++it )
	{
		if( (*it) == pMapRegion )
		{
			pMapRegion->Delete();
			delete pMapRegion;
			m_list_MapRegion.erase( it );
			return;
		}
	}
	assert_cs( false );
}

CsMap_LimitEvo* CsMap_LimitEvoGroup::Get( int nIndex )
{
	CsMap_LimitEvo::LIST_IT it = m_list_MapRegion.begin();
	CsMap_LimitEvo::LIST_IT itEnd = m_list_MapRegion.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return *it;
		--nIndex;
	}

	assert_cs( false );
	return NULL;
}

int CsMap_LimitEvoGroup::FindIndex( CsMap_LimitEvo* pFT )
{
	int nIndex = 0;
	CsMap_LimitEvo::LIST_IT it = m_list_MapRegion.begin();
	CsMap_LimitEvo::LIST_IT itEnd = m_list_MapRegion.end();
	for( ; it!=itEnd; ++it )
	{
		if( *it == pFT )
			return nIndex;

		++nIndex;
	}

	assert_cs( false );
	return 0;
}

bool CsMap_LimitEvoGroup::IsInRegion( CsPoint pt )
{
	CsMap_LimitEvo::LIST_IT it = m_list_MapRegion.begin();
	CsMap_LimitEvo::LIST_IT itEnd = m_list_MapRegion.end();
	CsMap_LimitEvo::sINFO* pInfo = NULL;
	UINT x, y;
	for( ; it!=itEnd; ++it )
	{
		pInfo = (*it)->GetInfo();
		x = abs( pInfo->s_nCenterX - pt.x );
		y =	abs( pInfo->s_nCenterY - pt.y );

		if( ( x > (UINT)pInfo->s_nRadius )||( y > (UINT)pInfo->s_nRadius ) )
			continue;

		if( sqrt( (long double)(x*x) + (long double)(y*y) ) < pInfo->s_nRadius )
		{
			return true;
		}
	}
	return false;

}

//======================================================================================================
//
//		ХЌЗа
//
//======================================================================================================

void CsMap_LimitEvoGroup::Clone( CsMap_LimitEvoGroup* pSrcGroup )
{
	assert_cs( m_list_MapRegion.empty() == true );

	CsMap_LimitEvo::LIST* pSrc = pSrcGroup->GetList();
	CsMap_LimitEvo::LIST_IT itSrc = pSrc->begin();
	for( ; itSrc!=pSrc->end(); ++itSrc )
	{
		CsMap_LimitEvo* pChild = csnew CsMap_LimitEvo;
		pChild->Init( (*itSrc)->GetInfo() );
		m_list_MapRegion.push_back( pChild );
	}
}


void CsMap_Buff_Region::Delete()
{
	CsMap_BuffRegion::LIST_IT it = m_BuffRegions.begin();
	for( ; it!=m_BuffRegions.end(); ++it )
	{
		(*it)->Delete();
		delete (*it );
	}
	m_BuffRegions.clear();
}
void CsMap_Buff_Region::AddRegion( CsMap_BuffRegion::sINFO* pkBuffRegionInfo )
{
	CsMap_BuffRegion* pkBuffRegion = csnew CsMap_BuffRegion;
	pkBuffRegion->Init( pkBuffRegionInfo );
	m_BuffRegions.push_back( pkBuffRegion );
}
CsMap_BuffRegion*		CsMap_Buff_Region::AddRegionInfo( CsMap_BuffRegion::sINFO* pkBuffRegionInfo )
{
	CsMap_BuffRegion* pkBuffRegion = csnew CsMap_BuffRegion;
	pkBuffRegion->Init( pkBuffRegionInfo );
	m_BuffRegions.push_back( pkBuffRegion );
	return pkBuffRegion;

}
void CsMap_Buff_Region::DeleteRegion( CsMap_BuffRegion* pkBuffRegion )
{
	CsMap_BuffRegion::LIST_IT it = m_BuffRegions.begin();
	for( ; it!=m_BuffRegions.end(); ++it )
	{
		if( (*it) == pkBuffRegion )
		{
			pkBuffRegion->Delete();
			delete pkBuffRegion;
			m_BuffRegions.erase( it );
			return;
		}
	}
}

void CsMap_Buff_Region::Clone( CsMap_Buff_Region* pSrcGroup )
{
	assert_cs( m_BuffRegions.empty() == true );

	CsMap_BuffRegion::LIST* pSrc = pSrcGroup->GetList();
	CsMap_BuffRegion::LIST_IT itSrc = pSrc->begin();
	for( ; itSrc!=pSrc->end(); ++itSrc )
	{
		CsMap_BuffRegion* pChild = csnew CsMap_BuffRegion;
		pChild->Init( (*itSrc)->GetInfo() );
		m_BuffRegions.push_back( pChild );
	}
}

CsMap_BuffRegion* CsMap_Buff_Region::Get( int nIndex )
{
	CsMap_BuffRegion::LIST_IT it = m_BuffRegions.begin();
	CsMap_BuffRegion::LIST_IT itEnd = m_BuffRegions.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return *it;
		--nIndex;
	}

	assert_cs( false );
	return NULL;
}

int CsMap_Buff_Region::FindIndex( CsMap_BuffRegion* pFT )
{
	int nIndex = 0;
	CsMap_BuffRegion::LIST_IT it = m_BuffRegions.begin();
	CsMap_BuffRegion::LIST_IT itEnd = m_BuffRegions.end();
	for( ; it!=itEnd; ++it )
	{
		if( *it == pFT )
			return nIndex;

		++nIndex;
	}

	assert_cs( false );
	return 0;
}
bool CsMap_Buff_Region::IsInRegion( CsPoint pt )
{
	CsMap_BuffRegion::LIST_IT it = m_BuffRegions.begin();
	CsMap_BuffRegion::LIST_IT itEnd = m_BuffRegions.end();
	CsMap_BuffRegion::sINFO* pInfo = NULL;
	UINT x, y;
	for( ; it!=itEnd; ++it )
	{
		pInfo = (*it)->GetInfo();
		x = abs( pInfo->centerx - pt.x );
		y =	abs( pInfo->centery - pt.y );

		if( ( x > (UINT)pInfo->radius )||( y > (UINT)pInfo->radius ) )
			continue;

		if( sqrt( (long double)(x*x) + (long double)(y*y) ) < pInfo->radius )
		{
			return true;
		}
	}
	return false;

}