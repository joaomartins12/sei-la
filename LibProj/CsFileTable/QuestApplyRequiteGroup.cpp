
#include "stdafx.h"
#include "QuestApplyRequiteGroup.h"


void CsQuestApplyRequiteGroup::Delete()
{
	CsQuestApplyRequite::LIST_IT it = m_List.begin();
	for( ; it!=m_List.end(); ++it )
		SAFE_DELETE(*it);		
	m_List.clear();
}

void CsQuestApplyRequiteGroup::Init()
{
	assert_cs( m_List.empty() == true );
}

void CsQuestApplyRequiteGroup::Add( CsQuestApplyRequite::eTYPE type, UINT nTypeID, int nTypeCount )
{
	CsQuestApplyRequite* pPreRequire = csnew CsQuestApplyRequite;
	pPreRequire->Init( type, nTypeID, nTypeCount );
	m_List.push_back( pPreRequire );
}

void CsQuestApplyRequiteGroup::Delete( int nIndex )
{
	CsQuestApplyRequite::LIST_IT it = m_List.begin();
	for( int n = 0 ; it!=m_List.end(); ++it, ++n )
	{
		if( n != nIndex )
			continue;
	
		SAFE_DELETE(*it);			
		m_List.erase( it );
		return;
	}
}

CsQuestApplyRequite* CsQuestApplyRequiteGroup::Get( int nIndex )
{
	CsQuestApplyRequite::LIST_IT it = m_List.begin();
	for( int n = 0; it!=m_List.end(); ++it, ++n )
	{
		if( n != nIndex )
			continue;
		return (*it);
	}
	return NULL;
}

void CsQuestApplyRequiteGroup::Clone( CsQuestApplyRequiteGroup* pSrcGroup )
{
	Init();

	CsQuestApplyRequite::LIST* pSrcList = pSrcGroup->GetList();
	CsQuestApplyRequite::LIST_IT itSrc = pSrcList->begin();
	CsQuestApplyRequite* pClonePreRequire = NULL;
	for( ; itSrc!=pSrcList->end(); ++itSrc )
	{
		pClonePreRequire = ( *itSrc )->Clone();
		m_List.push_back( pClonePreRequire );
	}
}