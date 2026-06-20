
#include "stdafx.h"
#include "QuestCompRequireGroup.h"

void CsQuestCompRequireGroup::Delete()
{
	CsQuestCompRequire::LIST_IT it = m_List.begin();
	for( ; it!=m_List.end(); ++it )
		SAFE_DELETE( *it )
	m_List.clear();
}

void CsQuestCompRequireGroup::Init()
{
	assert_cs( m_List.empty() == true );
}

void CsQuestCompRequireGroup::Add( CsQuestCompRequire::eTYPE type, UINT nTypeID, int nTypeCount, UINT nTargetID, int nSubValue1, int nSubValue2 )
{
	CsQuestCompRequire* pRequire = csnew CsQuestCompRequire;
	pRequire->Init( type, nTypeID, nTypeCount, nTargetID, nSubValue1, nSubValue2 );
	m_List.push_back( pRequire );
}

void CsQuestCompRequireGroup::Delete( int nIndex )
{
	CsQuestCompRequire::LIST_IT it = m_List.begin();
	for( int n = 0; it!=m_List.end(); ++it, ++n )
	{
		if( n != nIndex)
			continue;
		SAFE_DELETE( *it )
		m_List.erase( it );
		return;
	}
}

CsQuestCompRequire* CsQuestCompRequireGroup::Get( int nIndex )
{
	CsQuestCompRequire::LIST_IT it = m_List.begin();
	for( int n = 0; it!=m_List.end(); ++it, ++n )
	{
		if( n != nIndex )
			continue;
		return (*it);
	}
	assert_csm( false, _T( "아마도 사용 인자의 연결이 잘못된거 같습니다. qs 부분을 확인해 보십시오." ) );
	return NULL;
}

void CsQuestCompRequireGroup::Clone( CsQuestCompRequireGroup* pSrcGroup )
{
	Init();

	CsQuestCompRequire::LIST* pSrcList = pSrcGroup->GetList();
	CsQuestCompRequire::LIST_IT itSrc = pSrcList->begin();
	CsQuestCompRequire* pCloneCompRequire = NULL;
	for( ; itSrc!=pSrcList->end(); ++itSrc )
	{
		pCloneCompRequire = ( *itSrc )->Clone();
		m_List.push_back( pCloneCompRequire );
	}
}

bool CsQuestCompRequireGroup::IsCompleate()
{
	CsQuestCompRequire::LIST_IT it = m_List.begin();
	for( ; it!=m_List.end(); ++it )
	{
		if( (*it)->IsCompleat() == false )
			return false;
	}
	return true;
}

bool CsQuestCompRequireGroup::IncreaseCount( CsQuestCompRequire::eTYPE type, UINT nTypeID )
{
	CsQuestCompRequire::LIST_IT it = m_List.begin();
	for( ; it!=m_List.end(); ++it )
	{
		if( ( (*it)->GetType() == type )&&( (*it)->GetTypeID() == nTypeID ) )
		{
			if( (*it)->IncreaseCurCount() == true )
			{
				return IsCompleate();
			}
		}
	}
	return false;
}