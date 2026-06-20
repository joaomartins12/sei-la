
#include "stdafx.h"
#include "MapStartGroup.h"

void CsMapStartGroup::Delete()
{
	CsMapStart::LIST_IT it = m_list_MapStart.begin();
	CsMapStart::LIST_IT itEnd = m_list_MapStart.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		delete *it;
	}
	m_list_MapStart.clear();
}

void CsMapStartGroup::Init()
{
}

void CsMapStartGroup::AddStart( CsMapStart::sINFO* pSrcInfo )
{
	CsMapStart* pMapStart = csnew CsMapStart;
	pMapStart->Init( pSrcInfo );
	m_list_MapStart.push_back( pMapStart );
}

void CsMapStartGroup::DeleteStart( CsMapStart* pMapStart )
{
	CsMapStart::LIST_IT it = m_list_MapStart.begin();
	CsMapStart::LIST_IT itEnd = m_list_MapStart.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it) == pMapStart )
		{
			pMapStart->Delete();
			delete pMapStart;
			m_list_MapStart.erase( it );
			return;
		}
	}
	assert_cs( false );
}

//======================================================================================================
//
//		ХЌЗа
//
//======================================================================================================

void CsMapStartGroup::Clone( CsMapStartGroup* pSrcGroup )
{
	assert_cs( m_list_MapStart.empty() == true );

	CsMapStart::LIST* pSrc = pSrcGroup->GetStartList();
	CsMapStart::LIST_IT itSrc = pSrc->begin();
	CsMapStart::LIST_IT itSrcEnd = pSrc->end();
	for( ; itSrc!=itSrcEnd; ++itSrc )
	{
		CsMapStart* pChild = csnew CsMapStart;
		pChild->Init( (*itSrc)->GetInfo() );
		m_list_MapStart.push_back( pChild );
	}
}




