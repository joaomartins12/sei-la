
#include "stdafx.h"
#include "BattleTableMng.h"


CsBattleTableMng::CsBattleTableMng()
{

}

CsBattleTableMng::~CsBattleTableMng()
{

}

void CsBattleTableMng::Delete()
{
	m_BattleTableInfos.clear();
	m_BattleEnterableDigimons.clear();
	m_BattleRewardInfo.clear();
}

bool CsBattleTableMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel_BattleInfo();			// 배틀 맵 정보
		_LoadExcel_BattleRewardInfo();		// 배틀 보상 정보
		_LoadExcel_BattleEnterableInfo();	// 배틀 참가 가능 디지몬 정보
		break;
	case nsCsFileTable::FT_BIN:		
		if( _LoadBin( cPath ) == false )
			return false;
		break;
	case nsCsFileTable::FT_FILEPACK:
		if( _LoadFilePack( cPath ) == false )
			return false;
		break;
	default:
		assert_cs( false );
	}
	return true;
}

TB_BATTLE_TALBE_INFO const& CsBattleTableMng::GetBattleInfo() const
{
	return m_BattleTableInfos;
}

TB_BATTLE_ENTERABLE_INFO const& CsBattleTableMng::GetBattleEnterableDigimonInfo() const
{
	return m_BattleEnterableDigimons;
}

TB_BATTLE_REWARD_INFO const& CsBattleTableMng::GetBattleRewardInfo() const
{
	return m_BattleRewardInfo;
}

void CsBattleTableMng::_LoadExcel_BattleInfo()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_Battle(), "Base_Set" );
	if( !bSuccess )
		return;

	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	if( !bSuccess )
		return;

	while( excel.MoveNext() == true )
	{
		int nBattleMode = 0;
		excel.Read_Value( 1, nBattleMode );

		TB_BATTLE_TALBE_INFO_IT it = m_BattleTableInfos.find( nBattleMode );
		if( it == m_BattleTableInfos.end() )
		{
			auto rib = m_BattleTableInfos.insert( std::make_pair( nBattleMode, TB_BATTLE_MODE_INFO() ) );
			it = rib.first;
		}

		sBattleMode AddInfo;
		excel.Read_Value( 0, AddInfo.m_bActive );
		excel.Read_Value( 3, AddInfo.m_VsType );
		excel.Read_Value( 5, AddInfo.m_dwMapID );
		excel.Read_Value( 7, AddInfo.m_strMapImgFile );

		excel.Read_Value( 8, AddInfo.m_nLimitDLv );
		excel.Read_Value( 9, AddInfo.m_nNeedTicket );
		excel.Read_Value( 10, AddInfo.m_nLimitJoinCount );

		excel.Read_Value( 12, AddInfo.m_WinRankPoint );
		excel.Read_Value( 13, AddInfo.m_LoseRankPoint );

		excel.Read_Value( 17, AddInfo.m_WinRewardTalbeIdx );
		excel.Read_Value( 18, AddInfo.m_LoseRewardTalbeIdx );

		it->second.push_back( AddInfo );
	}
	excel.Close();
}

void CsBattleTableMng::_LoadExcel_BattleRewardInfo()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_Battle(), "Reward" );
	if( !bSuccess )
		return;

	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );
	if( !bSuccess )
		return;

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );	

	while( excel.MoveNext() == true )
	{
		int nSeasonNumber = 0;
		excel.Read_Value( 0, nSeasonNumber );

		TB_BATTLE_REWARD_INFO_IT it = m_BattleRewardInfo.find( nSeasonNumber );
		if( it == m_BattleRewardInfo.end() )
		{
			auto rib = m_BattleRewardInfo.insert( std::make_pair( nSeasonNumber, TB_BATTLE_REWARD_ITEM() ) );
			it = rib.first;			
		}

		sTbBattleReward addInfo;
		excel.Read_Value( 2, addInfo.m_dwItemCode );
		excel.Read_Value( 4, addInfo.m_Count );

		it->second.push_back( addInfo );
	}
}

void CsBattleTableMng::_LoadExcel_BattleEnterableInfo()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_Battle(), "Enterable" );
	if( !bSuccess )
		return;

	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );
	if( !bSuccess )
		return;

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );	

	while( excel.MoveNext() == true )
	{
		int nSeasonNumber = 0;
		excel.Read_Value( 0, nSeasonNumber );
		TB_BATTLE_ENTERABLE_INFO_IT it = m_BattleEnterableDigimons.find( nSeasonNumber );
		if( it == m_BattleEnterableDigimons.end() )
		{
			auto rib = m_BattleEnterableDigimons.insert( std::make_pair( nSeasonNumber, std::list<DWORD>() ) );
			it = rib.first;			
		}

		DWORD dwDigimonID = 0;
		excel.Read_Value( 2, dwDigimonID );

		it->second.push_back( dwDigimonID );
	}
}

#define BATTLETABLE_BIN		"BattleTable.bin"
void CsBattleTableMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, BATTLETABLE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	_SaveBin_BattleInfo( fp );
	_SaveBin_BattleRewardInfo( fp );
	_SaveBin_BattleEnterableInfo( fp );

	fclose( fp );
}

void CsBattleTableMng::_SaveBin_BattleInfo( FILE* fp )
{
	int nCount = (int)m_BattleTableInfos.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	TB_BATTLE_TALBE_INFO_CIT it = m_BattleTableInfos.begin();
	for( ; it != m_BattleTableInfos.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->first );

		int nModeCount = (int)it->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nModeCount );

		TB_BATTLE_MODE_INFO_CIT subIT = it->second.begin();
		for( ; subIT != it->second.end(); ++subIT )
		{ 
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_bActive );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_VsType );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_dwMapID );
			CmUtil::CmBinSave::Save_Value( fp, (*subIT).m_strMapImgFile );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_nLimitDLv );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_nNeedTicket );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_nLimitJoinCount );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_WinRankPoint );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_LoseRankPoint );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_WinRewardTalbeIdx );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_LoseRewardTalbeIdx );
		}		
	}
}

void CsBattleTableMng::_SaveBin_BattleRewardInfo( FILE* fp )
{
	int nCount = (int)m_BattleRewardInfo.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	TB_BATTLE_REWARD_INFO_CIT it = m_BattleRewardInfo.begin();
	for( ; it != m_BattleRewardInfo.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->first );

		int nTypeCount = (int)it->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nTypeCount );

		TB_BATTLE_REWARD_ITEM_CIT subIT = it->second.begin();
		for( ; subIT != it->second.end(); ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_dwItemCode );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).m_Count );
		}
	}
}

void CsBattleTableMng::_SaveBin_BattleEnterableInfo( FILE* fp )
{
	int nCount = (int)m_BattleEnterableDigimons.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	TB_BATTLE_ENTERABLE_INFO_CIT it = m_BattleEnterableDigimons.begin();
	for( ; it != m_BattleEnterableDigimons.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->first );

		int nListCount = (int)it->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nListCount );

		std::list<DWORD>::const_iterator subIT = it->second.begin();
		for( ; subIT != it->second.end(); ++subIT )
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT) );
	}
}


bool CsBattleTableMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, BATTLETABLE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	_LoadBin_BattleInfo( fp );
	_LoadBin_BattleRewardInfo( fp );
	_LoadBin_BattleEnterableInfo( fp );

	fclose( fp );
	return true;
}

void CsBattleTableMng::_LoadBin_BattleInfo( FILE *fp )
{
	int nCount;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		int nModeType;
		CmUtil::CmBinRead::Read_Value( fp, &nModeType );

		TB_BATTLE_TALBE_INFO_IT it = m_BattleTableInfos.find( nModeType );
		if( it == m_BattleTableInfos.end() )
		{
			auto rib = m_BattleTableInfos.insert( std::make_pair( nModeType, TB_BATTLE_MODE_INFO() ) );
			it = rib.first;
		}

		int nModeCount;
		CmUtil::CmBinRead::Read_Value( fp, &nModeCount );
		for( int j = 0; j < nModeCount; ++j )
		{
			sBattleMode addInfo;
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_bActive );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_VsType );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_dwMapID );
			CmUtil::CmBinRead::Read_Value( fp, addInfo.m_strMapImgFile );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_nLimitDLv );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_nNeedTicket );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_nLimitJoinCount );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_WinRankPoint );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_LoseRankPoint );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_WinRewardTalbeIdx );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_LoseRewardTalbeIdx );
			it->second.push_back( addInfo );
		}
	}
}

void CsBattleTableMng::_LoadBin_BattleRewardInfo( FILE *fp )
{
	int nCount;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		int nIdx;
		CmUtil::CmBinRead::Read_Value( fp, &nIdx );

		TB_BATTLE_REWARD_INFO_IT it = m_BattleRewardInfo.find( nIdx );
		if( it == m_BattleRewardInfo.end() )
		{
			auto rib = m_BattleRewardInfo.insert( std::make_pair( nIdx, TB_BATTLE_REWARD_ITEM() ) );
			it = rib.first;			
		}

		int nItemCount;
		CmUtil::CmBinRead::Read_Value( fp, &nItemCount );

		for( int j = 0; j < nItemCount; ++j )
		{
			sTbBattleReward addInfo;
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_dwItemCode );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_Count );
			it->second.push_back( addInfo );
		}
	}
}

void CsBattleTableMng::_LoadBin_BattleEnterableInfo( FILE *fp )
{
	int nCount;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		int nIdx;
		CmUtil::CmBinRead::Read_Value( fp, &nIdx );

		TB_BATTLE_ENTERABLE_INFO_IT it = m_BattleEnterableDigimons.find( nIdx );
		if( it == m_BattleEnterableDigimons.end() )
		{
			auto rib = m_BattleEnterableDigimons.insert( std::make_pair( nIdx, std::list<DWORD>() ) );
			it = rib.first;			
		}

		int nDigimonCount;
		CmUtil::CmBinRead::Read_Value( fp, &nDigimonCount );
		for( int j = 0; j < nDigimonCount; ++j )
		{
			DWORD dwDigimonIdx;
			CmUtil::CmBinRead::Read_Value( fp, &dwDigimonIdx );
			it->second.push_back( dwDigimonIdx );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CsBattleTableMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, BATTLETABLE_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return false;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	_LoadPack_BattleInfo( nHandle );
	_LoadPack_BattleRewardInfo( nHandle );
	_LoadPack_BattleEnterableInfo( nHandle );

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
	return true;
}


void CsBattleTableMng::_LoadPack_BattleInfo( int nHandle )
{
	int nCount;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		int nModeType;
		CmUtil::CmPackRead::Read_Value( nHandle, &nModeType );

		TB_BATTLE_TALBE_INFO_IT it = m_BattleTableInfos.find( nModeType );
		if( it == m_BattleTableInfos.end() )
		{
			auto rib = m_BattleTableInfos.insert( std::make_pair( nModeType, TB_BATTLE_MODE_INFO() ) );
			it = rib.first;
		}

		int nModeCount;
		CmUtil::CmPackRead::Read_Value( nHandle, &nModeCount );
		for( int j = 0; j < nModeCount; ++j )
		{
			sBattleMode addInfo;
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_bActive );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_VsType );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_dwMapID );
			CmUtil::CmPackRead::Read_Value( nHandle, addInfo.m_strMapImgFile );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_nLimitDLv );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_nNeedTicket );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_nLimitJoinCount );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_WinRankPoint );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_LoseRankPoint );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_WinRewardTalbeIdx );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_LoseRewardTalbeIdx );
			it->second.push_back( addInfo );
		}
	}
}

void CsBattleTableMng::_LoadPack_BattleRewardInfo( int nHandle )
{
	int nCount;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		int nIdx;
		CmUtil::CmPackRead::Read_Value( nHandle, &nIdx );

		TB_BATTLE_REWARD_INFO_IT it = m_BattleRewardInfo.find( nIdx );
		if( it == m_BattleRewardInfo.end() )
		{
			auto rib = m_BattleRewardInfo.insert( std::make_pair( nIdx, TB_BATTLE_REWARD_ITEM() ) );
			it = rib.first;			
		}

		int nItemCount;
		CmUtil::CmPackRead::Read_Value( nHandle, &nItemCount );

		for( int j = 0; j < nItemCount; ++j )
		{
			sTbBattleReward addInfo;
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_dwItemCode );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_Count );
			it->second.push_back( addInfo );
		}
	}
}

void CsBattleTableMng::_LoadPack_BattleEnterableInfo( int nHandle )
{
	int nCount;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		int nIdx;
		CmUtil::CmPackRead::Read_Value( nHandle, &nIdx );

		TB_BATTLE_ENTERABLE_INFO_IT it = m_BattleEnterableDigimons.find( nIdx );
		if( it == m_BattleEnterableDigimons.end() )
		{
			auto rib = m_BattleEnterableDigimons.insert( std::make_pair( nIdx, std::list<DWORD>() ) );
			it = rib.first;			
		}

		int nDigimonCount;
		CmUtil::CmPackRead::Read_Value( nHandle, &nDigimonCount );

		for( int j = 0; j < nDigimonCount; ++j )
		{
			DWORD dwDigimonIdx;
			CmUtil::CmPackRead::Read_Value( nHandle, &dwDigimonIdx );
			it->second.push_back( dwDigimonIdx );
		}
	}
}

