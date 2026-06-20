
#include "stdafx.h"
#include "RewardMng.h"

void CsRewardMng::Delete()
{
	MAP_REWARD_IT it = m_mapReward.begin();
	MAP_REWARD_IT itEnd = m_mapReward.end();
	for( ; it != itEnd; ++it )
	{
		CsReward::MAP_IT subIt = it->second.begin();
		CsReward::MAP_IT subItEnd = it->second.end();
		for( ; subIt != subItEnd; ++subIt )
		{
			if( subIt->second )
			{
				subIt->second->Delete();				
				delete subIt->second;
			}
		}
	}
	m_mapReward.clear();
}

bool CsRewardMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel();
		break;
	case nsCsFileTable::FT_BIN:		
		if( _LoadBin( cPath ) == false )
			return false;
		break;
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack( cPath );
		break;
	default:
		assert_cs( false );
		return false;
	}

	return true;
}
bool CsRewardMng::IsReward( int nServerIdx, int nID )
{
	MAP_REWARD_IT it = m_mapReward.find( nServerIdx );
	if( it == m_mapReward.end() )
		return false;

	CsReward::MAP_IT subIT = it->second.find( nID );
	if( subIT == it->second.end() )
		return false;

	return true;
}

CsReward* CsRewardMng::GetReward( int nServerIdx, int nID )
{
	MAP_REWARD_IT it = m_mapReward.find( nServerIdx );
	if( it == m_mapReward.end() )
		return NULL;

	CsReward::MAP_IT subIT = it->second.find( nID );
	if( subIT == it->second.end() )
		return NULL;

	return subIT->second;
}

CsReward::MAP* CsRewardMng::GetReward( int nServerIdx )
{
	MAP_REWARD_IT it = m_mapReward.find( nServerIdx );
	if( it == m_mapReward.end() )
		return NULL;

	return &it->second;
}


void CsRewardMng::_LoadExcelData( char* pTabName, int nIdx )
{
	char cBuffer[ CEXCLE_CELL_LEN ] = {0, };
	wchar_t wBuffer[ CEXCLE_CELL_LEN ] = {0, };

	cExcelReader excel;
	bool bSuccess;
	bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Reward(), pTabName );
	assert_cs( bSuccess == true );

	if( !bSuccess )
		return;

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsReward::MAP reqardMap;
	CsReward::sINFO info;
	while( excel.MoveNext() == true )
	{
		info.s_nID = atoi( excel.GetData( 0, cBuffer ) );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			// name
			M2W( info.s_szCommnet, excel.GetData( 1, cBuffer ), 1024 );
			break;	
		case nsCsFileTable::THAILAND:					
			wcscpy_s(info.s_szCommnet, 1024, excel.GetData( 1, wBuffer ));
			break;
		default:	
			{
				assert_cs( false );
				excel.Close();
			}
			return;
		}

		excel.GetData( 2, cBuffer );
		strcpy_s( info.s_nStartDate, 64, cBuffer );

		excel.GetData( 3, cBuffer );
		strcpy_s( info.s_nExpDate, 64, cBuffer );

		info.s_nItemCode1 = atoi( excel.GetData( 4, cBuffer ) );
		info.s_nItemCount1 = atoi( excel.GetData( 5, cBuffer ) );

		CsReward* pReward = csnew CsReward;
		pReward->Init( &info );
		reqardMap.insert( make_pair( info.s_nID, pReward ) );
	}
	excel.Close();

	if( !reqardMap.empty() )
		m_mapReward.insert( make_pair( nIdx, reqardMap ) );
}

void CsRewardMng::_LoadExcel()
{
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:
		_LoadExcelData( "Lucemon", 1 );
		_LoadExcelData( "Leviamon", 2 );
		_LoadExcelData( "Lilithmon", 3 );
		_LoadExcelData( "Barbamon", 4 );
		break;
	case nsCsFileTable::ENGLISH:
		_LoadExcelData( "Leviamon", 1 );
		_LoadExcelData( "Lucemon", 2 );
		_LoadExcelData( "Lilithmon", 3 );
		_LoadExcelData( "Barbamon", 4 );
		_LoadExcelData( "Beelzemon", 5 );
		_LoadExcelData( "Omegamon", 6 );
		break;
	case nsCsFileTable::ENGLISH_A:
		_LoadExcelData( "Seraphimon", 1 );
		break;
	case nsCsFileTable::THAILAND:
		_LoadExcelData( "Leviamon", 1 );
		_LoadExcelData( "Belphemon", 2 );
		_LoadExcelData( "Lilithmon", 3 );
		break;
// 	case nsCsFileTable::TAIWAN:
// 		_LoadExcelData( "Leviamon", 1 );
// 		_LoadExcelData( "Beelzemon", 2 );
// 		_LoadExcelData( "Lilithmon", 3 );
// 		break;
// 	case nsCsFileTable::HONGKONG:
// 		_LoadExcelData( "Leviamon", 1 );
// 		_LoadExcelData( "Beelzemon", 2 );
// 		_LoadExcelData( "Lilithmon", 3 );
// 		break;
	default:
		return;
	}	
}

#define REWARD_BIN		"Reward.bin"
void CsRewardMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ] = {0,};
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, REWARD_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapReward.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	MAP_REWARD_IT it = m_mapReward.begin();
	MAP_REWARD_IT itEnd = m_mapReward.end();
	for( ; it != itEnd; ++it )
	{
		fwrite( &it->first, sizeof(int), 1, fp );

		int nMapSize = (int)it->second.size();
		fwrite( &nMapSize, sizeof(int), 1, fp );

		CsReward::MAP_IT subIt = it->second.begin();
		CsReward::MAP_IT subItEnd = it->second.end();
		for( ; subIt != subItEnd; ++subIt )
		{
			if( subIt->second )
				fwrite( subIt->second->GetInfo(), sizeof( CsReward::sINFO ), 1, fp );
		}
	}

	fclose( fp );
}

bool CsRewardMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, REWARD_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	for( int i=0; i<nCount; ++i )
	{
		int nIdx =0;
		fread( &nIdx, sizeof(int), 1, fp );

		CsReward::MAP insertData;
		int nMapCount;
		fread( &nMapCount, sizeof( int ), 1, fp );
		for( int j = 0; j < nMapCount; ++j )
		{
			CsReward::sINFO info;
			fread( &info, sizeof( CsReward::sINFO ), 1, fp );
			CsReward* pObject = csnew CsReward;
			pObject->Init( &info );
			insertData.insert( make_pair( info.s_nID, pObject ) );
		}		
		m_mapReward.insert( make_pair( nIdx, insertData ) );
	}

	fclose( fp );
	return true;
}

void CsRewardMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, REWARD_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		int nIdx =0;
		_read( nHandle, &nIdx, sizeof(int) );

		CsReward::MAP insertData;
		int nMapCount;
		_read( nHandle, &nMapCount, sizeof( int ) );
		for( int j = 0; j < nMapCount; ++j )
		{
			CsReward::sINFO info;
			_read( nHandle, &info, sizeof( CsReward::sINFO ) );
			CsReward* pObject = csnew CsReward;
			pObject->Init( &info );
			insertData.insert( make_pair( info.s_nID, pObject ) );
		}		
		m_mapReward.insert( make_pair( nIdx, insertData ) );
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );	
}

