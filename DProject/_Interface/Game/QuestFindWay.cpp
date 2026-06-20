#include "stdafx.h"
#include "QuestFindWay.h"

cQuestFindWay::cQuestFindWay()
{

}

cQuestFindWay::~cQuestFindWay()
{

}

bool cQuestFindWay::Initialize( int nID, CsQuestCompRequire::eTYPE eQuestCompleteType, int nTargetID )
{
	bool bResult( false );
	switch( eQuestCompleteType ) // 목표 타입
	{
	case CsQuestCompRequire::NPCTALK:	// npc에 말걸기
		{
			m_nDestTableID = nID;
			m_nDestType	   = FindWay_Npc;

			bResult = InitNpcInfo( nID );
		}break;
	case CsQuestCompRequire::USE_ITEM_NPC: // 엔피시에 아이템 사용
		{
			m_nDestTableID = nTargetID;					  // 타겟ID를 길찾기 목표로 설정한다
			m_nDestType	   = FindWay_Npc; // 대상은 NPC

			bResult = InitNpcInfo( nTargetID );
		}break;
	case CsQuestCompRequire::ITEM: // 아이템 수집일때
		{
			m_nDestTableID = nTargetID; // 타겟ID를 길찾기 목표로 설정한다
			m_nDestType	   = FindWay_Monster; // 대상은 몬스터만 지정 가능

			bResult = InitMonsterInfo( nTargetID );
		}break;
	case CsQuestCompRequire::MONSTER: // 몬스터 사냥
		{
			m_nDestTableID = nID;
			m_nDestType	   = FindWay_Monster;

			bResult = InitMonsterInfo( nID );
		}break;
	case CsQuestCompRequire::USE_ITEM_MONSTER: // 몬스터에게 아이템 사용
		{
			m_nDestTableID = nTargetID;
			m_nDestType	   = FindWay_Monster;

			bResult = InitMonsterInfo( nTargetID );
		}break;
	case CsQuestCompRequire::REGION:
		{
			return false; // 임시 막음

			char  szRegionID[ 16 ] = {0, };
			itoa( nID, szRegionID, 10 );

			std::string strMapID(szRegionID);
			std::string strRegionID;
			strRegionID.assign( szRegionID, strMapID.length() - 3, strMapID.length() - 1);
			strMapID.replace( strMapID.length() - 3, strMapID.length() - 1,"" );

			int nMapID		= atoi( strMapID.c_str() );
			int nRegionID	= atoi( strRegionID.c_str() );

			m_nDestMapID   = nMapID;
			m_nDestTableID = nRegionID;
			m_nDestType	   = FindWay_Region;

			CsMapRegionGroup* pRegionGroup = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( nMapID ); // 지역 테이블 읽어온다
			SAFE_POINTER_RETVAL( pRegionGroup, false );

			CsMapRegion::LIST* pRegionList = pRegionGroup->GetRegionList(); // 지역리스트를 가져온다
			SAFE_POINTER_RETVAL( pRegionList, false);

			CsMapRegion::LIST_IT iter	 =	pRegionList->begin();
			CsMapRegion::LIST_IT enditer =	pRegionList->end();

			for( int i = 0; iter != enditer; ++iter, ++i )
			{
				if( nRegionID == i )
				{
					CsMapRegion::sINFO* pRegionInfo = (*iter)->GetInfo();
					SAFE_POINTER_RETVAL(pRegionInfo, false);
					m_strDestName.assign( pRegionInfo->s_szDiscript );
				}
			}
			return true;
		}break;
	}
	return bResult;
}

bool cQuestFindWay::InitNpcInfo( int nID )
{
	CsMapNpc* pFTMapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( nID );
	SAFE_POINTER_RETVAL( pFTMapNpc, false );

	CsMapNpc::sINFO* pMapInfo = pFTMapNpc->GetInfo();
	SAFE_POINTER_RETVAL( pMapInfo, false );

	if( pMapInfo->s_nInitPosX == 0 && pMapInfo->s_nInitPosY == 0) // 위치 정보가 없다면 초기화 실패
		return false;

	CsNpc*	pNpc = nsCsFileTable::g_pNpcMng->GetNpc( m_nDestTableID );
	SAFE_POINTER_RETVAL(pNpc, false);
	CsNpc::sINFO* pInfo = pNpc->GetInfo();
	SAFE_POINTER_RETVAL(pInfo, false);

	m_nDestMapID = pInfo->s_dwMapID;	// npc 위치 맵번호
	m_strDestName.assign( pInfo->s_szName ); // npc 이름
	return true;
}

bool cQuestFindWay::InitMonsterInfo( int nID )
{
	CsMonster* pMonster = nsCsFileTable::g_pMonsterMng->GetMonster( nID );	
	SAFE_POINTER_RETVAL( pMonster, false );

	CsMonster::sINFO* pMonsterInfo = pMonster->GetInfo();
	SAFE_POINTER_RETVAL( pMonsterInfo, false );

	CsMapMonsterMng::sMapMonster* pMapMonster = nsCsMapTable::g_pMapMonsterMng->GetMonster( nID );
	SAFE_POINTER_RETVAL( pMapMonster, false );

	std::map< DWORD, CsMapMonster::LIST >* pMonsterListInMap = pMapMonster->GetMonsterList();
	SAFE_POINTER_RETVAL( pMonsterListInMap, false );

	std::map< DWORD, CsMapMonster::LIST >::iterator mapiter = pMonsterListInMap->begin();
	m_nDestMapID = mapiter->first; // 맵 번호

	std::list< CsMapMonster* >::iterator iter = mapiter->second.begin();
	std::list< CsMapMonster* >::iterator Enditer = mapiter->second.end();
	for( ; iter != Enditer; ++iter )
	{
		CsMapMonster::sINFO* pInfo = (*iter)->GetInfo();
		SAFE_POINTER_RETVAL(pInfo, false);

		if( pInfo->s_dwMonsterTableID == nID )
		{
			if( pInfo->s_nCenterX == 0 && pInfo->s_nCenterY == 0 ) // 몬스터 위치 정보가 없다면 실패
				return false;
		}
	}
	m_strDestName.assign( pMonsterInfo->s_szName ); // 대상 이름
	return true;
}