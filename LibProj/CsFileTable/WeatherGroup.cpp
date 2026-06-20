
#include "stdafx.h"
#include "WeatherGroup.h"

void CsWeatherGroup::Delete()
{
	CsWeather::LIST_IT it = m_list.begin();
	CsWeather::LIST_IT itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		delete *it;
	}
	m_list.clear();
}

void CsWeatherGroup::Init()
{	
}

CsWeather* CsWeatherGroup::Add( CsWeather::sINFO* pSrcInfo )
{
	CsWeather* pElement = csnew CsWeather;
	pElement->Init( pSrcInfo );
	m_list.push_back( pElement );
	return pElement;
}

void CsWeatherGroup::Delete( CsWeather* pElement )
{
	CsWeather::LIST_IT it = m_list.begin();
	CsWeather::LIST_IT itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it) == pElement )
		{
			pElement->Delete();
			delete pElement;
			m_list.erase( it );
			return;
		}
	}
	assert_cs( false );
}

CsWeather::sELEMENT* CsWeatherGroup::UpdateWeather( UINT nTime, int x, int y )
{
	if( m_list.empty() == true )
		return NULL;

	CsWeather::LIST_IT it = m_list.begin();
	CsWeather::LIST_IT itEnd = m_list.end();
	CsWeather::sINFO* pInfo = NULL;
	UINT dx, dy;	

	CsWeather* pNearWeather = NULL;
	UINT nMinDist = UINT_MAX;
	UINT nCurDist;

	for( ; it!=itEnd; ++it )
	{
		pInfo = (*it)->GetInfo();

		dx = abs( pInfo->s_nCenterX - x );
		dy = abs( pInfo->s_nCenterY - y );

		if( ( dx > nMinDist )||( dy > nMinDist ) )
			continue;

		// 임시공간 조금을 주겠다
		nCurDist = (UINT)sqrt( (long double)( dx*dx ) + (long double)( dy*dy ) );
		if( nMinDist > nCurDist )
		{
			nMinDist = nCurDist;
			pNearWeather = *it;
		}
	}

	if( pNearWeather != NULL )
	{
		if( nMinDist < (UINT)pNearWeather->GetInfo()->s_nRadius )
		{
			UINT nCurTime = nTime % ( pNearWeather->GetInfo()->s_nTotalLoopMin*60 );
			return pNearWeather->UpdateElement( nCurTime );
		}
	}
	return NULL;
}

//======================================================================================================
//
//		클론
//
//======================================================================================================

void CsWeatherGroup::Clone( CsWeatherGroup* pSrcGroup )
{
	assert_cs( m_list.empty() == true );

	CsWeather::LIST* pSrc = pSrcGroup->GetList();
	CsWeather::LIST_IT itSrc = pSrc->begin();
	CsWeather::LIST_IT itSrcEnd = pSrc->end();
	for( ; itSrc!=itSrcEnd; ++itSrc )
	{
		CsWeather* pChild = csnew CsWeather;
		pChild->Init( (*itSrc)->GetInfo() );
		m_list.push_back( pChild );
	}
}




