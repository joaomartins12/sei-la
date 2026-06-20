
#include "stdafx.h"
#include "MapMonsterGroup.h"

void CsMapMonsterGroup::Delete()
{
	CsMapMonster::LIST_IT itMonster = m_listMapMonster.begin();
	CsMapMonster::LIST_IT itMonsterEnd = m_listMapMonster.end();
	for( ; itMonster!=itMonsterEnd; ++itMonster )
	{
		(*itMonster)->Delete();
		delete (*itMonster);
	}
	m_listMapMonster.clear();
}

void CsMapMonsterGroup::Init()
{

}

void CsMapMonsterGroup::AddMonster( CsMapMonster* pSrcMonster )
{
	m_listMapMonster.push_back( pSrcMonster );
}

CsMapMonster* CsMapMonsterGroup::GetMonster( int nListIndex )
{
	assert_cs( nListIndex >= 0 );
	CsMapMonster* pReturn = NULL;
	CsMapMonster::LIST_IT it = m_listMapMonster.begin();
	CsMapMonster::LIST_IT itEnd = m_listMapMonster.end();
	for( ; it!=itEnd; ++it )
	{		
		if( nListIndex == 0 )
		{
			return (*it);
		}
		--nListIndex;
	}
	assert_cs( false );
	return NULL;
}

CsMapMonster* CsMapMonsterGroup::GetMonster_ByMonsterID( DWORD dwMonsterID )
{
	CsMapMonster* pReturn = NULL;
	CsMapMonster::LIST_IT it = m_listMapMonster.begin();
	CsMapMonster::LIST_IT itEnd = m_listMapMonster.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetInfo()->s_dwMonsterTableID == dwMonsterID )
			return (*it);
	}
	return NULL;
}

void CsMapMonsterGroup::DeleteMonster( int nDeleteListIndex )
{
	assert_cs( nDeleteListIndex >= 0 );
	CsMapMonster* pReturn = NULL;
	CsMapMonster::LIST_IT it = m_listMapMonster.begin();
	for( ; it!=m_listMapMonster.end(); ++it )
	{		
		if( nDeleteListIndex == 0 )
		{
			(*it)->Delete();
			delete (*it);
			m_listMapMonster.erase( it );
			return;
		}
		--nDeleteListIndex;
	}
	assert_cs( false );	
}

//======================================================================================================
//
//		ХЌЗа
//
//======================================================================================================
void CsMapMonsterGroup::Clone( CsMapMonsterGroup* pSrcGroup )
{
	assert_cs( m_listMapMonster.empty() == true );

	CsMapMonster::LIST* pSrc = pSrcGroup->GetMonsterList();
	CsMapMonster::LIST_IT itSrc = pSrc->begin();
	for( ; itSrc!=pSrc->end(); ++itSrc )
	{
		CsMapMonster* pChild = csnew CsMapMonster;
		pChild->Init( (*itSrc)->GetInfo() );
		m_listMapMonster.push_back( pChild );
	}
}
