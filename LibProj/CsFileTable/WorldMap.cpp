
#include "stdafx.h"
#include "WorldMap.h"


CsWorldMap::CsWorldMap()
{

}

CsWorldMap::~CsWorldMap()
{

}

CsAreaMap* CsWorldMap::GetArea( int nIndex )
{
	std::list< CsAreaMap*>::iterator it = m_listArea.begin();
	std::list< CsAreaMap*>::iterator itEnd = m_listArea.end();
	for( ; it!=itEnd; ++it )
	{
		if( ( --nIndex ) < 0 )
			return *it;
	}
	return NULL;
}

void CsWorldMap::Delete()
{ 
	m_listArea.clear(); 
}

void CsWorldMap::Init( sINFO* pSrcInfo )
{ 
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); 
}

CsWorldMap::sINFO* CsWorldMap::GetInfo()
{ 
	return &m_Info; 
}

void CsWorldMap::AddArea( CsAreaMap* pArea )
{ 
	m_listArea.push_back( pArea ); 
}

std::list< CsAreaMap* >* CsWorldMap::GetAreaList()
{
	return &m_listArea; 
}	