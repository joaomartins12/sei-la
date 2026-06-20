
#include "stdafx.h"
#include "QuestRequiteGroup.h"

void CsQuestRequiteGroup::Delete()
{
	CsQuestRequite::LIST_IT it = m_List.begin();
	for( ; it!=m_List.end(); ++it )
		SAFE_DELETE(*it);		
	m_List.clear();
}

void CsQuestRequiteGroup::Init()
{
	assert_cs( m_List.empty() == true );
}

void CsQuestRequiteGroup::Add( CsQuestRequite::eMETHOD method, CsQuestRequite::eTYPE type, UINT nTypeID, int nTypeCount )
{
	CsQuestRequite* pRequite = csnew CsQuestRequite;
	pRequite->Init( method, type, nTypeID, nTypeCount );
	m_List.push_back( pRequite );
}

void CsQuestRequiteGroup::Delete( int nIndex )
{
	CsQuestRequite::LIST_IT it = m_List.begin();
	for( int n = 0; it!=m_List.end(); ++it, ++n )
	{
		if( nIndex != n )
			continue;
		SAFE_DELETE(*it);
		m_List.erase( it );
		break;
	}
}

CsQuestRequite* CsQuestRequiteGroup::Get( int nIndex )
{
	CsQuestRequite::LIST_IT it = m_List.begin();
	for( int n = 0; it!=m_List.end(); ++it, ++n )
	{
		if( nIndex != n )
			continue;

		return (*it);
	}
	return NULL;
}

void CsQuestRequiteGroup::Clone( CsQuestRequiteGroup* pSrcGroup )
{
	Init();

	CsQuestRequite::LIST* pSrcList = pSrcGroup->GetList();
	CsQuestRequite::LIST_IT itSrc = pSrcList->begin();
	CsQuestRequite* pCloneChild = NULL;
	for( ; itSrc!=pSrcList->end(); ++itSrc )
	{
		pCloneChild = ( *itSrc )->Clone();
		m_List.push_back( pCloneChild );
	}
}

