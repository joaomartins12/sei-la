
#include "stdafx.h"
#include "TableClass_InfiniteWar.h"

BEGIN_NAME_TABLE

CInfiniteWarTable::CInfiniteWarTable()
{
}

CInfiniteWarTable::~CInfiniteWarTable()
{
}

void CInfiniteWarTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"InfiniteWar_SummonsMain.bin", this, &CInfiniteWarTable::Load_SummonsMain );
	AddLoadTableEvent( L"InfiniteWar_SummonsMonster.bin", this, &CInfiniteWarTable::Load_SummonsMonster );
	AddLoadTableEvent( L"InfiniteWar_DailyReward.bin", this, &CInfiniteWarTable::Load_DailyReward );
	AddLoadTableEvent( L"InfiniteWar_WeekReward.bin", this, &CInfiniteWarTable::Load_Rank_Reward );
	AddLoadTableEvent( L"InfiniteWar_MonthlyReward.bin", this, &CInfiniteWarTable::Load_Rank_Reward );
	AddLoadTableEvent( L"InfiniteWar_SeasonReward.bin", this, &CInfiniteWarTable::Load_Rank_Reward);
	AddLoadTableEvent( L"InfiniteWar_Awards.bin", this, &CInfiniteWarTable::Load_Awards );
}

void CInfiniteWarTable::ReleaseTable()
{
	m_mapInfiniteNpcInfos.clear();
	m_mapSummonMonsterInfo.clear();
	m_mapDailyRewardInfos.clear();
	m_mapRankRewardInfos.clear();
}

CInfiniteWarTable::InfinityNPCContainer const& CInfiniteWarTable::GetInfiniteWarInfos() const
{
	return m_mapInfiniteNpcInfos;
}

CInfiniteWarTable::StageInfos const* CInfiniteWarTable::GetStageInfo( DWORD const& dwNpcIDX ) const
{
	InfinityNPCContainer_CIT it = m_mapInfiniteNpcInfos.find( dwNpcIDX );
	if( it == m_mapInfiniteNpcInfos.end() )
		return NULL;

	return &it->second;
}

CInfiniteWarTable::DailyRewardContiner const& CInfiniteWarTable::GetRewardData_Daily() const
{
	return m_mapDailyRewardInfos;
}

CInfiniteWarTable::RankRewardItemsContanier const& CInfiniteWarTable::GetRewardData_Rank() const
{
	return m_mapRankRewardInfos;
}

sInfiniteAwardNpcInfo const* CInfiniteWarTable::GetAwardNpcInfo(int const& nServerIDX, DWORD const& dwTableID) const
{
	AwardNpcInfoContanier_CIT it = m_mapAwardNpcInfos.find( nServerIDX );
	if( it == m_mapAwardNpcInfos.end() )
		return NULL;

	AwardNpcInfoMap_CIT subIT = it->second.find(dwTableID);
	if( subIT == it->second.end() )
		return NULL;

	return &subIT->second;
}

//////////////////////////////////////////////////////////////////////////

bool CInfiniteWarTable::Load_SummonsMain( DataStream& pReader )
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwNpcCode = 0;
		pReader.pop( dwNpcCode );
		if( dwNpcCode == 0 )
			break;

		InfinityNPCContainer_IT it = m_mapInfiniteNpcInfos.find( dwNpcCode );
		if( it == m_mapInfiniteNpcInfos.end() )
		{
			InfinityNPCContainer::_Pairib rib = m_mapInfiniteNpcInfos.insert( std::make_pair( dwNpcCode, StageInfos() ) );
			it = rib.first;
		}

		int nStage = 0;
		pReader.pop( nStage );
		if( nStage == 0 )
			continue;

		StageInfos_IT subIT = it->second.find( nStage );
		if( subIT == it->second.end() )
		{
			StageInfos::_Pairib rib = it->second.insert( std::make_pair( nStage, sStageInfos() ) );
			subIT = rib.first;
		}
		else
		{
		//	CsMessageBox(MB_OK, L"같은 스테이지가 존재함.");
			break;
		}

		pReader.pop( subIT->second.s_nSummon_Type );
		pReader.pop( subIT->second.s_nMonsterInfoIndex );
		pReader.pop( subIT->second.s_dwRewardItemIdx );
		pReader.pop( subIT->second.s_nWinPoint );
		pReader.pop( subIT->second.s_nLosePoint );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

bool CInfiniteWarTable::Load_SummonsMonster( DataStream& pReader )
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nSummonKey = 0;
		pReader.pop( nSummonKey );

		SummonMonsterInfoContainer_IT it = m_mapSummonMonsterInfo.find( nSummonKey );
		if( it == m_mapSummonMonsterInfo.end() )
		{
			SummonMonsterInfoContainer::_Pairib rib = m_mapSummonMonsterInfo.insert( std::make_pair( nSummonKey, SummonMonsters() ) );
			it = rib.first;
		}

		sSummonMonsterInfo addData;
		pReader.pop( addData.s_dwMonsterIDX );
		pReader.pop( addData.s_nCount );
		it->second.push_back( addData );
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CInfiniteWarTable::Load_DailyReward( DataStream& pReader )
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nRewardType = 0;
		pReader.pop( nRewardType );
		if( nRewardType != 0 )
			continue;

		int nRewardSubType = 0;
		pReader.pop( nRewardSubType );

		DailyRewardContiner_IT it = m_mapDailyRewardInfos.find( nRewardSubType );
		if( it == m_mapDailyRewardInfos.end() )
		{
			DailyRewardContiner::_Pairib rib = m_mapDailyRewardInfos.insert( std::make_pair( nRewardSubType, DailyRewardItems() ) );
			it = rib.first;
		}


		sDaliyRewardInfo addInfo;		
		pReader.pop( addInfo.s_dwNeedItemCode );
		pReader.pop( addInfo.s_nNeedPoint );

		while(1)
		{
			sRewardItemInfo addItem;
			pReader.pop( addItem.s_dwItemCode );
			if( addItem.s_dwItemCode == 0 )
				break;
			pReader.pop( addItem.s_nCount );
			if( addItem.s_nCount == 0 )
				break;
			addInfo.s_listRewardItems.push_back( addItem );
		}

		it->second.push_back( addInfo );
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CInfiniteWarTable::Load_Rank_Reward( DataStream& pReader )
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nRewardType = 0;
		pReader.pop( nRewardType );

		RankRewardItemsContanier_IT it = m_mapRankRewardInfos.find( nRewardType );
		if( it == m_mapRankRewardInfos.end() )
		{
			RankRewardItemsContanier::_Pairib rib = m_mapRankRewardInfos.insert( std::make_pair( nRewardType, RankRewardItems() ) );
			it = rib.first;
		}

		sRankRewardInfo addData;		
		pReader.pop( addData.s_nRankType );
		pReader.pop( addData.s_nRankMin );
		pReader.pop( addData.s_nRankMax );

		while(1)
		{
			sRewardItemInfo addRewardItems;
			pReader.pop( addRewardItems.s_dwItemCode );
			if( 0 == addRewardItems.s_dwItemCode )
				break;
			pReader.pop( addRewardItems.s_nCount );
			if( 0 == addRewardItems.s_nCount )
				break;

			addData.s_listRewardItems.push_back( addRewardItems );
		}

		it->second.push_back( addData );
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CInfiniteWarTable::Load_Awards( DataStream& pReader )
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nServerIdx = 0;
		pReader.pop( nServerIdx );
		if( 0 == nServerIdx )
			break;

		DWORD dwNpcTableIDX = 0;
		pReader.pop( dwNpcTableIDX );
		if( 0 == dwNpcTableIDX )
			break;	

		AwardNpcInfoContanier_IT it = m_mapAwardNpcInfos.find( nServerIdx );
		if( it == m_mapAwardNpcInfos.end() )
		{
			AwardNpcInfoContanier::_Pairib rib = m_mapAwardNpcInfos.insert( std::make_pair( nServerIdx, AwardNpcInfoMap() ) );
			it = rib.first;
		}

		sInfiniteAwardNpcInfo addData;
		pReader.pop( addData.s_wTitleName );
		pReader.pop( addData.s_wTamerName );
		pReader.pop( addData.s_wGuildName );
		pReader.pop( addData.s_dwModelID );
		pReader.pop( addData.s_wTalkMsg );

		it->second.insert( std::make_pair( dwNpcTableIDX, addData ) );
		bSuccess = pReader.MoveNext();
	}
	return true;
}

END_NAME_TABLE