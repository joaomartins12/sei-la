
#include "stdafx.h"
#include "Weather.h"

void CsWeather::Delete()
{
	LISTELE_IT it = m_listElement.begin();
	LISTELE_IT itEnd = m_listElement.end();
	for( ; it!=itEnd; ++it )
	{
		delete ( *it );
	}
	m_listElement.clear();
}

void CsWeather::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}

CsWeather::sELEMENT* CsWeather::AddElement( sELEMENT* pSrc )
{
	sELEMENT* pEle = csnew sELEMENT;
	memcpy( pEle, pSrc, sizeof( sELEMENT ) );
	m_listElement.push_back( pEle );
	m_Info.s_nElementCount = (int)m_listElement.size();
	return pEle;
}

void CsWeather::DelElement( sELEMENT* pSrc )
{
	LISTELE_IT it = m_listElement.begin();
	LISTELE_IT itEnd = m_listElement.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it) == pSrc )
		{
			delete (*it);
			m_listElement.erase( it );
			m_Info.s_nElementCount = (int)m_listElement.size();
			return;
		}
	}
}

CsWeather::sELEMENT* CsWeather::UpdateElement( UINT nCurTime )
{
	LISTELE_IT it = m_listElement.begin();
	LISTELE_IT itEnd = m_listElement.end();
	sELEMENT* pElemet = NULL;
	UINT nETime;
	for( ; it!=itEnd; ++it )
	{
		pElemet = *it;
		nETime = pElemet->s_nTime_Min*60;
		if( nCurTime <= nETime )
		{
			return pElemet;
		}
		nCurTime -= ( pElemet->s_nTime_Min*60 );
	}
	return NULL;
}