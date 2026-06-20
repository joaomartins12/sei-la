
#include "stdafx.h"
#include "InfiniteWarManager.h"

//======================================================================================================
//
// UI String 
//
//======================================================================================================
bool CsInfiniteWarMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:		_LoadExcel();	break;
	case nsCsFileTable::FT_BIN:			_LoadBin( cPath );	break;
	case nsCsFileTable::FT_FILEPACK:	_LoadFilePack( cPath ); break;
	default:
		assert_cs( false );
		return false;
	}

	return true;
}

void CsInfiniteWarMng::Delete()
{
	_Delete();
}

bool CsInfiniteWarMng::_LoadExcel()
{
	_LoadExcel_Main( nsCsFileTable::GetPath_DM_InfiniteWar(), "Summons_Main" );
	_LoadExcel_Monster( nsCsFileTable::GetPath_DM_InfiniteWar(), "Summons_Monster" );
	_LoadExcel_Daily_Reward( nsCsFileTable::GetPath_DM_InfiniteWar(), "Daily_Reward" );
	_LoadExcel_Rank_Reward( nsCsFileTable::GetPath_DM_InfiniteWar(), "Week_Reward" );
	_LoadExcel_Rank_Reward( nsCsFileTable::GetPath_DM_InfiniteWar(), "Monthly_Reward" );
	_LoadExcel_Rank_Reward( nsCsFileTable::GetPath_DM_InfiniteWar(), "Season_Reward" );
	_LoadExcel_AwardNpcInfo( nsCsFileTable::GetPath_DM_InfiniteWar(), "Awards" );
	return true;
}

bool CsInfiniteWarMng::_LoadExcel_Main(char* pExcelFile, char* pExcelSheetName)
{
	cExcelReader excel;
	bool bSuccess = excel.Open( pExcelFile, pExcelSheetName );
	if( bSuccess == false )
		return false;

	excel.FirstOfSkip(2);// 2줄은 버린다.
	
	while( excel.MoveNext() )
	{
		DWORD dwNpcCode = 0;
		excel.Read_Value( 0, dwNpcCode );
		if( dwNpcCode == 0 )
			break;

		InfinityNPCContainer_IT it = m_mapInfiniteNpcInfos.find( dwNpcCode );
		if( it == m_mapInfiniteNpcInfos.end() )
		{
			auto rib = m_mapInfiniteNpcInfos.insert( std::make_pair( dwNpcCode, StageInfos() ) );
			it = rib.first;
		}
		
		int nStage = 0;
		excel.Read_Value( 1, nStage );
		if( nStage == 0 )
			continue;

		StageInfos_IT subIT = it->second.find( nStage );
		if( subIT == it->second.end() )
		{
			auto rib = it->second.insert( std::make_pair( nStage, TB::sStageInfos() ) );
			subIT = rib.first;
		}
		else
		{
			CsMessageBox(MB_OK, L"같은 스테이지가 존재함.");
			break;
		}
		
		excel.Read_Value( 2, subIT->second.s_nSummon_Type );
		excel.Read_Value( 3, subIT->second.s_nMonsterInfoIndex );
		excel.Read_Value( 4, subIT->second.s_dwRewardItemIdx );
		excel.Read_Value( 5, subIT->second.s_nWinPoint );	// 승리시 획득 포인트
		excel.Read_Value( 6, subIT->second.s_nLosePoint );	// 패배시 차감 포인트
	}
	excel.Close();
	return true;
}

bool CsInfiniteWarMng::_LoadExcel_Monster(char* pExcelFile, char* pExcelSheetName)
{
	cExcelReader excel;
	bool bSuccess = excel.Open( pExcelFile, pExcelSheetName );
	if( bSuccess == false )
		return false;

	excel.FirstOfSkip(2);// 2줄은 버린다.

	while( excel.MoveNext() )
	{
		int nSummonKey = 0;
		excel.Read_Value( 0, nSummonKey );

		SummonMonsterInfoContainer_IT it = m_mapSummonMonsterInfo.find( nSummonKey );
		if( it == m_mapSummonMonsterInfo.end() )
		{
			auto rib = m_mapSummonMonsterInfo.insert( std::make_pair( nSummonKey, SummonMonsters() ) );
			it = rib.first;
		}

		TB::sSummonMonsterInfo addData;
		excel.Read_Value( 1, addData.s_dwMonsterIDX );
		excel.Read_Value( 2, addData.s_nCount );
		it->second.push_back( addData );
	}
	excel.Close();
	return true;
}

bool CsInfiniteWarMng::_LoadExcel_Daily_Reward(char* pExcelFile, char* pExcelSheetName)
{
	cExcelReader excel;
	bool bSuccess = excel.Open( pExcelFile, pExcelSheetName );
	if( bSuccess == false )
		return false;

	excel.FirstOfSkip(2);// 2줄은 버린다.

	while( excel.MoveNext() )
	{
		int nRewardType = 0;
		excel.Read_Value( 0, nRewardType );
		if( nRewardType != 0 )
			continue;
		
		int nRewardSubType = 0;
		excel.Read_Value( 1, nRewardSubType );

		DailyRewardContiner_IT it = m_mapDailyRewardInfos.find( nRewardSubType );
		if( it == m_mapDailyRewardInfos.end() )
		{
			auto rib = m_mapDailyRewardInfos.insert( std::make_pair( nRewardSubType, DailyRewardItems() ) );
			it = rib.first;
		}

		
		TB::sDaliyRewardInfo addInfo;		
		excel.Read_Value( 3, addInfo.s_dwNeedItemCode );
		excel.Read_Value( 4, addInfo.s_nNeedPoint );

		int nIdx = 5;
		while(1)
		{
			TB::sRewardItemInfo addItem;
			excel.Read_Value( nIdx, addItem.s_dwItemCode );
			if( addItem.s_dwItemCode == 0 )
				break;
			excel.Read_Value( nIdx + 1, addItem.s_nCount );
			if( addItem.s_nCount == 0 )
				break;

			addInfo.s_listRewardItems.push_back( addItem );

			nIdx += 2;
		}

		it->second.push_back( addInfo );
	}
	excel.Close();
	return true;
}

bool CsInfiniteWarMng::_LoadExcel_Rank_Reward(char* pExcelFile, char* pExcelSheetName)
{
	cExcelReader excel;
	bool bSuccess = excel.Open( pExcelFile, pExcelSheetName );
	if( bSuccess == false )
		return false;

	excel.FirstOfSkip(2);// 2줄은 버린다.

	while( excel.MoveNext() )
	{
		int nRewardType = 0;
		excel.Read_Value( 0, nRewardType );

		RankRewardItemsContanier_IT it = m_mapRankRewardInfos.find( nRewardType );
		if( it == m_mapRankRewardInfos.end() )
		{
			auto rib = m_mapRankRewardInfos.insert( std::make_pair( nRewardType, RankRewardItems() ) );
			it = rib.first;
		}

		TB::sRankRewardInfo addData;		
		excel.Read_Value( 2, addData.s_nRankType );
		excel.Read_Value( 3, addData.s_nRankMin );
		excel.Read_Value( 4, addData.s_nRankMax );

		int nIdx = 5;
		while(1)
		{
			TB::sRewardItemInfo addRewardItems;
			excel.Read_Value( nIdx, addRewardItems.s_dwItemCode );
			if( 0 == addRewardItems.s_dwItemCode )
				break;
			excel.Read_Value( nIdx + 1, addRewardItems.s_nCount );
			if( 0 == addRewardItems.s_nCount )
				break;
			
			addData.s_listRewardItems.push_back( addRewardItems );

			nIdx += 2;
		}

		it->second.push_back( addData );
	}
	excel.Close();
	return true;

}

bool CsInfiniteWarMng::_LoadExcel_AwardNpcInfo(char* pExcelFile, char* pExcelSheetName)
{
	cExcelReader excel;
	bool bSuccess = excel.Open( pExcelFile, pExcelSheetName );
	if( bSuccess == false )
		return false;

	excel.FirstOfSkip(2);// 2줄은 버린다.

	while( excel.MoveNext() )
	{
		int nServerIdx = 0;
		excel.Read_Value( 0, nServerIdx );
		if( 0 == nServerIdx )
			break;

		DWORD dwTableIDX = 0;
		excel.Read_Value( 1, dwTableIDX );	
		if( 0 == dwTableIDX )
			break;	

		AwardNpcInfoContanier_IT it = m_mapAwardNpcInfos.find( nServerIdx );
		if( it == m_mapAwardNpcInfos.end() )
		{
			auto rib = m_mapAwardNpcInfos.insert( std::make_pair( nServerIdx, AwardNpcInfoMap() ) );
			it = rib.first;
		}

		TB::sInfiniteAwardNpcInfo addData;
		excel.Read_Value( 2, addData.s_wTitleName );
		excel.Read_Value( 3, addData.s_wTamerName );
		excel.Read_Value( 4, addData.s_wGuildName );
		excel.Read_Value( 5, addData.s_dwModelID );
		excel.Read_Value( 13, addData.s_wTalkMsg );

		it->second.insert( std::make_pair( dwTableIDX, addData ) );
	}
	excel.Close();
	return true;
}


#define INFINITEWAR_BIN		"InfiniteWar.bin"
void CsInfiniteWarMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, INFINITEWAR_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	_SaveBin( fp );
	fclose( fp );
}

void CsInfiniteWarMng::_SaveBin( FILE* fp )
{
	int nCount = (int)m_mapInfiniteNpcInfos.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	InfinityNPCContainer_CIT it = m_mapInfiniteNpcInfos.begin();
	InfinityNPCContainer_CIT itEnd = m_mapInfiniteNpcInfos.end();
	for( ; it!=itEnd; ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(it->first) );

		int nCount = (int)it->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );
		StageInfos_CIT subIT = it->second.begin();
		StageInfos_CIT subITEnd = it->second.end();
		for( ; subIT != subITEnd; ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->first) );
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->second.s_nSummon_Type) );	
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->second.s_dwRewardItemIdx) );	
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->second.s_nWinPoint) );	
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->second.s_nLosePoint) );	
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->second.s_nMonsterInfoIndex) );	
		}
	}
	//////////////////////////////////////////////////////////////////////////
	nCount = (int)m_mapSummonMonsterInfo.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	SummonMonsterInfoContainer_CIT mit = m_mapSummonMonsterInfo.begin();
	SummonMonsterInfoContainer_CIT mitEnd = m_mapSummonMonsterInfo.end();
	for( ; mit!=mitEnd; ++mit )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(mit->first) );

		int nCount = (int)mit->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );
		SummonMonsters_CIT subIT = mit->second.begin();
		SummonMonsters_CIT subITEnd = mit->second.end();
		for( ; subIT != subITEnd; ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->s_dwMonsterIDX) );	
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->s_nCount) );	
		}
	}
	//////////////////////////////////////////////////////////////////////////
	nCount = (int)m_mapDailyRewardInfos.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	DailyRewardContiner_CIT dit = m_mapDailyRewardInfos.begin();
	DailyRewardContiner_CIT ditEnd = m_mapDailyRewardInfos.end();
	for( ; dit!=ditEnd; ++dit )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(dit->first) );

		int nCount = (int)dit->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );
		DailyRewardItems_CIT subIT = dit->second.begin();
		DailyRewardItems_CIT subITEnd = dit->second.end();
		for( ; subIT != subITEnd; ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->s_dwNeedItemCode) );			
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->s_nNeedPoint) );	

			int nItemCount = (int)subIT->s_listRewardItems.size();
			CmUtil::CmBinSave::Save_Value( fp, &nItemCount );

			std::list<TB::sRewardItemInfo>::const_iterator secIT =  subIT->s_listRewardItems.begin();
			std::list<TB::sRewardItemInfo>::const_iterator secITEnd =  subIT->s_listRewardItems.end();
			for( ; secIT != secITEnd; ++secIT )
			{
				CmUtil::CmBinSave::Save_Value( fp, &(secIT->s_dwItemCode) );
				CmUtil::CmBinSave::Save_Value( fp, &(secIT->s_nCount) );				
			}			
		}
	}	
	//////////////////////////////////////////////////////////////////////////
	nCount = (int)m_mapRankRewardInfos.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );
	RankRewardItemsContanier_CIT rit = m_mapRankRewardInfos.begin();
	RankRewardItemsContanier_CIT ritEnd = m_mapRankRewardInfos.end();
	for( ; rit!=ritEnd; ++rit )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(rit->first) );

		int nCount = (int)rit->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );
		RankRewardItems_CIT subIT = rit->second.begin();
		RankRewardItems_CIT subITEnd = rit->second.end();
		for( ; subIT != subITEnd; ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->s_nRankType) );	
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->s_nRankMin) );	
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->s_nRankMax) );	

			int nItemCount = (int)subIT->s_listRewardItems.size();
			CmUtil::CmBinSave::Save_Value( fp, &nItemCount );

			std::list<TB::sRewardItemInfo>::const_iterator secIT =  subIT->s_listRewardItems.begin();
			std::list<TB::sRewardItemInfo>::const_iterator secITEnd =  subIT->s_listRewardItems.end();
			for( ; secIT != secITEnd; ++secIT )
			{
				CmUtil::CmBinSave::Save_Value( fp, &(secIT->s_dwItemCode) );
				CmUtil::CmBinSave::Save_Value( fp, &(secIT->s_nCount) );				
			}			
		}
	}	
	//////////////////////////////////////////////////////////////////////////
	nCount = (int)m_mapAwardNpcInfos.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );
	AwardNpcInfoContanier_CIT awardit = m_mapAwardNpcInfos.begin();
	for( ; awardit!= m_mapAwardNpcInfos.end(); ++awardit )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(awardit->first) );

		int nCount = (int)awardit->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );

		AwardNpcInfoMap_CIT subIT = awardit->second.begin();
		for( ; subIT != awardit->second.end(); ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &(subIT->first) );

			CmUtil::CmBinSave::Save_Value( fp, &(subIT->second.s_dwModelID) );
			CmUtil::CmBinSave::Save_Value( fp, subIT->second.s_wTamerName );
			CmUtil::CmBinSave::Save_Value( fp, subIT->second.s_wTitleName );
			CmUtil::CmBinSave::Save_Value( fp, subIT->second.s_wGuildName );
			CmUtil::CmBinSave::Save_Value( fp, subIT->second.s_wTalkMsg );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}

bool CsInfiniteWarMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, INFINITEWAR_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		//CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	int nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		DWORD dwNPCIdx = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwNPCIdx );
		InfinityNPCContainer_IT it = m_mapInfiniteNpcInfos.find( dwNPCIdx );
		if( it == m_mapInfiniteNpcInfos.end() )
		{
			auto rib = m_mapInfiniteNpcInfos.insert( std::make_pair( dwNPCIdx, StageInfos() ) );
			it = rib.first;
		}

		int nStageCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nStageCount );
		for( int n = 0; n < nStageCount; ++n )
		{
			int nStage = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nStage );
			StageInfos_IT subIT = it->second.find( nStage );
			if( subIT == it->second.end() )
			{
				auto rib = it->second.insert( std::make_pair( nStage, TB::sStageInfos() ) );
				subIT = rib.first;
			}

			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.s_nSummon_Type );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.s_dwRewardItemIdx );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.s_nWinPoint );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.s_nLosePoint );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.s_nMonsterInfoIndex );
		}			
	}
	//////////////////////////////////////////////////////////////////////////
	nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nKey = 0; 
		CmUtil::CmBinRead::Read_Value( fp, &nKey );

		SummonMonsterInfoContainer_IT it = m_mapSummonMonsterInfo.find( nKey );
		if( it == m_mapSummonMonsterInfo.end() )
		{
			auto rib = m_mapSummonMonsterInfo.insert( std::make_pair( nKey, SummonMonsters() ) );
			it = rib.first;
		}

		int nMonsterCount = 0; 
		CmUtil::CmBinRead::Read_Value( fp, &nMonsterCount );
		for( int m = 0; m < nMonsterCount; ++m )
		{
			TB::sSummonMonsterInfo addData;
			CmUtil::CmBinRead::Read_Value( fp, &addData.s_dwMonsterIDX );	
			CmUtil::CmBinRead::Read_Value( fp, &addData.s_nCount );
			it->second.push_back( addData );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nKeyValue = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nKeyValue );

		DailyRewardContiner_IT it = m_mapDailyRewardInfos.find( nKeyValue );
		if( it == m_mapDailyRewardInfos.end() )
		{
			auto rib = m_mapDailyRewardInfos.insert( std::make_pair( nKeyValue, DailyRewardItems() ) );
			it = rib.first;
		}

		int nRewardCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nRewardCount );
		for( int m = 0; m < nRewardCount; ++m )
		{
			TB::sDaliyRewardInfo addInfo;		
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.s_dwNeedItemCode );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.s_nNeedPoint );

			int nItemCount = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nItemCount );
			for( int l = 0; l < nItemCount; ++l )
			{
				TB::sRewardItemInfo addData;
				CmUtil::CmBinRead::Read_Value( fp, &addData.s_dwItemCode );
				CmUtil::CmBinRead::Read_Value( fp, &addData.s_nCount );
				addInfo.s_listRewardItems.push_back( addData );
			}
			it->second.push_back( addInfo );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nKeyValue = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nKeyValue );
		RankRewardItemsContanier_IT it = m_mapRankRewardInfos.find( nKeyValue );
		if( it == m_mapRankRewardInfos.end() )
		{
			auto rib = m_mapRankRewardInfos.insert( std::make_pair( nKeyValue, RankRewardItems() ) );
			it = rib.first;
		}

		int nSubCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			TB::sRankRewardInfo addInfo;
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.s_nRankType );	
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.s_nRankMin );	
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.s_nRankMax );	

			int nItemCount = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nItemCount );
			for( int l = 0; l < nItemCount; ++l )
			{
				TB::sRewardItemInfo addItem;
				CmUtil::CmBinRead::Read_Value( fp, &addItem.s_dwItemCode );
				CmUtil::CmBinRead::Read_Value( fp, &addItem.s_nCount );	
				addInfo.s_listRewardItems.push_back( addItem );
			}
			it->second.push_back( addInfo );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nKeyValue = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nKeyValue );
		AwardNpcInfoContanier_IT it = m_mapAwardNpcInfos.find( nKeyValue );
		if( it == m_mapAwardNpcInfos.end() )
		{
			auto rib = m_mapAwardNpcInfos.insert( std::make_pair( nKeyValue, AwardNpcInfoMap() ) );
			it = rib.first;
		}

		int nSubCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			DWORD dwTableID = 0;
			CmUtil::CmBinRead::Read_Value( fp, &dwTableID );

			TB::sInfiniteAwardNpcInfo addItem;			
			CmUtil::CmBinRead::Read_Value( fp, &addItem.s_dwModelID );
			CmUtil::CmBinRead::Read_Value( fp, addItem.s_wTamerName );
			CmUtil::CmBinRead::Read_Value( fp, addItem.s_wTitleName );
			CmUtil::CmBinRead::Read_Value( fp, addItem.s_wGuildName );
			CmUtil::CmBinRead::Read_Value( fp, addItem.s_wTalkMsg );

			it->second.insert( std::make_pair( dwTableID, addItem ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	fclose( fp );
	return true;
}

bool CsInfiniteWarMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, INFINITEWAR_BIN );

	if( !CsFPS::CsFPSystem::IsExistOnlyPack( FT_PACKHANDLE, cName ) )
		return false;

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return false;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );	

	//////////////////////////////////////////////////////////////////////////
	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		DWORD dwNPCIdx = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwNPCIdx );
		InfinityNPCContainer_IT it = m_mapInfiniteNpcInfos.find( dwNPCIdx );
		if( it == m_mapInfiniteNpcInfos.end() )
		{
			auto rib = m_mapInfiniteNpcInfos.insert( std::make_pair( dwNPCIdx, StageInfos() ) );
			it = rib.first;
		}

		int nStageCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nStageCount );
		for( int n = 0; n < nStageCount; ++n )
		{
			int nStage = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nStage );
			StageInfos_IT subIT = it->second.find( nStage );
			if( subIT == it->second.end() )
			{
				auto rib = it->second.insert( std::make_pair( nStage, TB::sStageInfos() ) );
				subIT = rib.first;
			}

			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.s_nSummon_Type );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.s_dwRewardItemIdx );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.s_nWinPoint );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.s_nLosePoint );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.s_nMonsterInfoIndex );
		}			
	}
	//////////////////////////////////////////////////////////////////////////
	nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nKey = 0; 
		CmUtil::CmPackRead::Read_Value( nHandle, &nKey );

		SummonMonsterInfoContainer_IT it = m_mapSummonMonsterInfo.find( nKey );
		if( it == m_mapSummonMonsterInfo.end() )
		{
			auto rib = m_mapSummonMonsterInfo.insert( std::make_pair( nKey, SummonMonsters() ) );
			it = rib.first;
		}

		int nMonsterCount = 0; 
		CmUtil::CmPackRead::Read_Value( nHandle, &nMonsterCount );
		for( int m = 0; m < nMonsterCount; ++m )
		{
			TB::sSummonMonsterInfo addData;
			CmUtil::CmPackRead::Read_Value( nHandle, &addData.s_dwMonsterIDX );	
			CmUtil::CmPackRead::Read_Value( nHandle, &addData.s_nCount );
			it->second.push_back( addData );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nKeyValue = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nKeyValue );

		DailyRewardContiner_IT it = m_mapDailyRewardInfos.find( nKeyValue );
		if( it == m_mapDailyRewardInfos.end() )
		{
			auto rib = m_mapDailyRewardInfos.insert( std::make_pair( nKeyValue, DailyRewardItems() ) );
			it = rib.first;
		}

		int nRewardCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nRewardCount );
		for( int m = 0; m < nRewardCount; ++m )
		{
			TB::sDaliyRewardInfo addInfo;		
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.s_dwNeedItemCode );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.s_nNeedPoint );

			int nItemCount = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nItemCount );
			for( int l = 0; l < nItemCount; ++l )
			{
				TB::sRewardItemInfo addData;
				CmUtil::CmPackRead::Read_Value( nHandle, &addData.s_dwItemCode );
				CmUtil::CmPackRead::Read_Value( nHandle, &addData.s_nCount );
				addInfo.s_listRewardItems.push_back( addData );
			}
			it->second.push_back( addInfo );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nKeyValue = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nKeyValue );
		RankRewardItemsContanier_IT it = m_mapRankRewardInfos.find( nKeyValue );
		if( it == m_mapRankRewardInfos.end() )
		{
			auto rib = m_mapRankRewardInfos.insert( std::make_pair( nKeyValue, RankRewardItems() ) );
			it = rib.first;
		}

		int nSubCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			TB::sRankRewardInfo addInfo;
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.s_nRankType );	
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.s_nRankMin );	
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.s_nRankMax );	

			int nItemCount = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nItemCount );
			for( int l = 0; l < nItemCount; ++l )
			{
				TB::sRewardItemInfo addItem;
				CmUtil::CmPackRead::Read_Value( nHandle, &addItem.s_dwItemCode );
				CmUtil::CmPackRead::Read_Value( nHandle, &addItem.s_nCount );	
				addInfo.s_listRewardItems.push_back( addItem );
			}
			it->second.push_back( addInfo );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nKeyValue = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nKeyValue );
		AwardNpcInfoContanier_IT it = m_mapAwardNpcInfos.find( nKeyValue );
		if( it == m_mapAwardNpcInfos.end() )
		{
			auto rib = m_mapAwardNpcInfos.insert( std::make_pair( nKeyValue, AwardNpcInfoMap() ) );
			it = rib.first;
		}

		int nSubCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			DWORD dwTableID = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &dwTableID );

			TB::sInfiniteAwardNpcInfo addItem;			
			CmUtil::CmPackRead::Read_Value( nHandle, &addItem.s_dwModelID );
			CmUtil::CmPackRead::Read_Value( nHandle, addItem.s_wTamerName );
			CmUtil::CmPackRead::Read_Value( nHandle, addItem.s_wTitleName );
			CmUtil::CmPackRead::Read_Value( nHandle, addItem.s_wGuildName );
			CmUtil::CmPackRead::Read_Value( nHandle, addItem.s_wTalkMsg );

			it->second.insert( std::make_pair( dwTableID, addItem ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////


	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );

	return true;
}

void CsInfiniteWarMng::_Delete()
{
	m_mapInfiniteNpcInfos.clear();
	m_mapSummonMonsterInfo.clear();
	m_mapDailyRewardInfos.clear();
	m_mapRankRewardInfos.clear();
}

CsInfiniteWarMng::InfinityNPCContainer const& CsInfiniteWarMng::GetInfiniteWarInfos() const
{
	return m_mapInfiniteNpcInfos;
}

CsInfiniteWarMng::StageInfos const*	CsInfiniteWarMng::GetStageInfo( DWORD const& dwNpcIDX ) const
{
	InfinityNPCContainer_CIT it = m_mapInfiniteNpcInfos.find( dwNpcIDX );
	if( it == m_mapInfiniteNpcInfos.end() )
		return NULL;

	return &it->second;
}

CsInfiniteWarMng::DailyRewardContiner const& CsInfiniteWarMng::GetRewardData_Daily() const
{
	return m_mapDailyRewardInfos;
}

CsInfiniteWarMng::RankRewardItemsContanier const& CsInfiniteWarMng::GetRewardData_Rank() const
{
	return m_mapRankRewardInfos;
}

TB::sInfiniteAwardNpcInfo const* CsInfiniteWarMng::GetAwardNpcInfo(int const& nServerIDX, DWORD const& dwTableID) const
{
	AwardNpcInfoContanier_CIT it = m_mapAwardNpcInfos.find( nServerIDX );
	if( it == m_mapAwardNpcInfos.end() )
		return NULL;

	AwardNpcInfoMap_CIT subIT = it->second.find(dwTableID);
	if( subIT == it->second.end() )
		return NULL;

	return &subIT->second;
}