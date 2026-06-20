
#include "stdafx.h"
#include "QuestPreRequireGroup.h"


void CsQuestPreRequireGroup::Delete()
{
	CsQuestPreRequire::LIST_IT it = m_List.begin();
	for( ; it!=m_List.end(); ++it )
		SAFE_DELETE(*it);		
	m_List.clear();
}

void CsQuestPreRequireGroup::Init()
{
	assert_cs( m_List.empty() == true );
}

void CsQuestPreRequireGroup::Add( CsQuestPreRequire::eTYPE type, UINT nTypeID, int nTypeCount )
{
	CsQuestPreRequire* pPreRequire = csnew CsQuestPreRequire;
	pPreRequire->Init( type, nTypeID, nTypeCount );
	m_List.push_back( pPreRequire );
}

void CsQuestPreRequireGroup::Delete( int nIndex )
{
	CsQuestPreRequire::LIST_IT it = m_List.begin();
	for( int n = 0; it!=m_List.end(); ++it, ++n )
	{
		if( n != nIndex )
			continue;
		SAFE_DELETE(*it);			
		m_List.erase( it );
		break;
	}
}

CsQuestPreRequire* CsQuestPreRequireGroup::Get( int nIndex )
{
	CsQuestPreRequire::LIST_IT it = m_List.begin();
	for( int n = 0; it!=m_List.end(); ++it, ++n )
	{
		if( n != nIndex )
			continue;
		return (*it);
	}
	return NULL;
}

void CsQuestPreRequireGroup::Clone( CsQuestPreRequireGroup* pSrcGroup )
{
	Init();

	CsQuestPreRequire::LIST* pSrcList = pSrcGroup->GetList();
	CsQuestPreRequire::LIST_IT itSrc = pSrcList->begin();
	CsQuestPreRequire* pClonePreRequire = NULL;
	for( ; itSrc!=pSrcList->end(); ++itSrc )
	{
		pClonePreRequire = ( *itSrc )->Clone();
		m_List.push_back( pClonePreRequire );
	}
}