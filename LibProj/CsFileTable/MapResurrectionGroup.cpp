
#include "stdafx.h"
#include "MapResurrectionGroup.h"


void CsMapResurrectionGroup::Delete()
{
	CsMapResurrection::LIST_IT it = m_list_MapResurrection.begin();
	for( ; it!=m_list_MapResurrection.end(); ++it )
	{
		(*it)->Delete();
		delete (*it );
	}
	m_list_MapResurrection.clear();
}

void CsMapResurrectionGroup::Init()
{
}

void CsMapResurrectionGroup::AddResurrection( CsMapResurrection::sINFO* pSrcInfo )
{
	CsMapResurrection* pSrc = csnew CsMapResurrection;
	pSrc->Init( pSrcInfo );
	m_list_MapResurrection.push_back( pSrc );
}

void CsMapResurrectionGroup::DeleteResurrection( CsMapResurrection* pSrc )
{
	CsMapResurrection::LIST_IT it = m_list_MapResurrection.begin();
	for( ; it!=m_list_MapResurrection.end(); ++it )
	{
		if( (*it) == pSrc )
		{
			pSrc->Delete();
			delete pSrc;
			m_list_MapResurrection.erase( it );
			return;
		}
	}
}

//======================================================================================================
//
//		ХЌЗа
//
//======================================================================================================

void CsMapResurrectionGroup::Clone( CsMapResurrectionGroup* pSrcGroup )
{
	assert_cs( m_list_MapResurrection.empty() == true );

	CsMapResurrection::LIST* pSrc = pSrcGroup->GetResurrectionList();
	CsMapResurrection::LIST_IT itSrc = pSrc->begin();
	for( ; itSrc!=pSrc->end(); ++itSrc )
	{
		CsMapResurrection* pChild = csnew CsMapResurrection;
		pChild->Init( (*itSrc)->GetInfo() );
		m_list_MapResurrection.push_back( pChild );
	}
}




