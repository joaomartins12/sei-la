
#include "stdafx.h"
#include "MapCharLightGroup.h"


void CsMapCharLightGroup::Delete()
{
	CsMapCharLight::LIST_IT it = m_List.begin();
	CsMapCharLight::LIST_IT itEnd = m_List.end();

	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		delete *it;
	}
	m_List.clear();
}

void CsMapCharLightGroup::Init()
{
}

void CsMapCharLightGroup::Add( CsMapCharLight::sINFO_BASE* pSrcInfo )
{
	CsMapCharLight* pDest = csnew CsMapCharLight;
	pDest->Init( pSrcInfo );
	m_List.push_back( pDest );
}

