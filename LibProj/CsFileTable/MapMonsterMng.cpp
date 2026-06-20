
#include "stdafx.h"
#include "MapMonsterMng.h"

void CsMapMonsterMng::Delete()
{
	CsMapMonsterGroup::MAP_IT itGroup = m_mapMapMonsterGroup.begin();
	for( ; itGroup!=m_mapMapMonsterGroup.end(); ++itGroup )
	{
		itGroup->second->Delete();
		delete itGroup->second;
	}
	m_mapMapMonsterGroup.clear();

	MAP_IT it = m_mapMapMonster.begin();
	for( ; it != m_mapMapMonster.end(); ++it )
	{
		it->second->Delete();
		SAFE_DELETE( it->second );
	}
	m_mapMapMonster.clear();

}

bool CsMapMonsterMng::Init( char* cPath, nsCsFileTable::eFILE_TYPE ft /*=nsCsFileTable::g_eFileType*/ )
{
	switch( ft )
	{	
	case nsCsFileTable::FT_EXCEL:	
		_LoadExcel();				
		break;
	case nsCsFileTable::FT_BIN:
		switch( nsCsMapTable::g_eModeMapMonster )
		{
		case nsCsMapTable::eMode_Client:
			if( _LoadBin( cPath ) == false ) //게임에서 위치찾기로 쓰일데이터 로드
				return false;
			break;
		case nsCsMapTable::eMode_Tool:
			_LoadExcel(); // 맵툴에서만 읽는다.
			break;
		}		
		break;
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack( cPath );
		break;
	default:
		assert_cs( false );
	}
	return true;
}


#define MEMON_COL_MAPID				0
#define MEMON_COL_MONSTERTABLE_ID	1
#define MEMON_COL_CENTER_X			2
#define MEMON_COL_CENTER_Y			3
#define MEMON_COL_RADIUS			4
#define MEMON_COL_MONCOUNT			5
#define MEMON_COL_MONRESPONTIME		6
#define MEMON_COL_KILLGEN_FTID		7
#define MEMON_COL_KILLGEN_CNT		8
#define MEMON_COL_KILLGEN_VIEWCNT	9
#define MEMON_COL_MOVE	10
#define MONSTER_RESPON	11
void CsMapMonsterMng::_LoadExcel()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP(), ME_MONSTER_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_MONSTER_VERSION )
	{
		MessageBox( NULL, _T( "Map.xls Monster Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	// 데이터 입력
	CsMapMonsterGroup* pMapMonGroup = NULL;
	CsMapMonster* pMapMonster = NULL;
	CsMapMonster::sINFO info;

	while( excel.MoveNext() == true )
	{
		info.s_dwMapID = atoi( excel.GetData( MEMON_COL_MAPID, cBuffer ) );
		if( info.s_dwMapID == 0 )
			continue;

		info.s_dwMonsterTableID = atoi( excel.GetData( MEMON_COL_MONSTERTABLE_ID, cBuffer ) );
		info.s_nCenterX = atoi( excel.GetData( MEMON_COL_CENTER_X, cBuffer ) );
		info.s_nCenterY = atoi( excel.GetData( MEMON_COL_CENTER_Y, cBuffer ) );
		info.s_nRadius = atoi( excel.GetData( MEMON_COL_RADIUS, cBuffer ) );
		info.s_nCount = atoi( excel.GetData( MEMON_COL_MONCOUNT, cBuffer ) );
		info.s_nResponTime = atoi( excel.GetData( MEMON_COL_MONRESPONTIME, cBuffer ) );
		info.s_nKillgen_MonFTID = atoi( excel.GetData( MEMON_COL_KILLGEN_FTID, cBuffer ) );
		info.s_nKillgen_Count = atoi( excel.GetData( MEMON_COL_KILLGEN_CNT, cBuffer ) );
		info.s_nKillgen_ViewCnt = atoi( excel.GetData( MEMON_COL_KILLGEN_VIEWCNT, cBuffer ) );
		info.s_nMoveType = atoi( excel.GetData( MEMON_COL_MOVE, cBuffer ) );		
		info.s_bInsRespon = (bool)atoi( excel.GetData( MONSTER_RESPON, cBuffer ) );		
		pMapMonster = csnew CsMapMonster;
		pMapMonster->Init( &info );

		GetGroup( info.s_dwMapID )->AddMonster( pMapMonster );
	}
	excel.Close();
}

void CsMapMonsterMng::Save()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP() );
	assert_cs( bLoad == true );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_MONSTER_SHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->DeleteAllRows();

	pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_MONSTER_VERSION );
	for( int i=1; i<=MEMON_COL_MONRESPONTIME; ++i )
	{
		pSheet->Cell( 0, i )->SetInteger( 1 );
	}

	pSheet->Cell( 1, MEMON_COL_MAPID )->SetWString( _T( "mapID" ) );
	pSheet->Cell( 1, MEMON_COL_MONSTERTABLE_ID )->SetWString( _T( "monstor table ID" ) );
	pSheet->Cell( 1, MEMON_COL_CENTER_X )->SetWString( _T( "center x" ) );
	pSheet->Cell( 1, MEMON_COL_CENTER_Y )->SetWString( _T( "center y" ) );
	pSheet->Cell( 1, MEMON_COL_RADIUS )->SetWString( _T( "radius" ) );
	pSheet->Cell( 1, MEMON_COL_MONCOUNT )->SetWString( _T( "count" ) );
	pSheet->Cell( 1, MEMON_COL_MONRESPONTIME )->SetWString( _T( "respon time" ) );
	pSheet->Cell( 1, MEMON_COL_KILLGEN_FTID )->SetWString( _T( "Killgen Mon ID" ) );
	pSheet->Cell( 1, MEMON_COL_KILLGEN_CNT )->SetWString( _T( "Killgen Count" ) );
	pSheet->Cell( 1, MEMON_COL_KILLGEN_VIEWCNT )->SetWString( _T( "Killgen ViewCount" ) );
	pSheet->Cell( 1, MEMON_COL_MOVE )->SetWString( _T( "npc_move_type" ) );
	pSheet->Cell( 1, MONSTER_RESPON )->SetWString( _T( "Ins_Respon" ) );

	size_t siInputRow = 2;
	CsMapMonsterGroup::MAP_IT itGroup = m_mapMapMonsterGroup.begin();
	CsMapMonster::sINFO* pMapInfo = NULL;
	for( ; itGroup!=m_mapMapMonsterGroup.end(); ++itGroup )
	{
		CsMapMonster::LIST* pList = itGroup->second->GetMonsterList();
		CsMapMonster::LIST_IT itMonster = pList->begin();
		for( ; itMonster != pList->end(); ++itMonster )
		{
			pMapInfo = ( *itMonster )->GetInfo();
			pSheet->Cell( siInputRow, MEMON_COL_MAPID )->SetInteger(  pMapInfo->s_dwMapID );
			pSheet->Cell( siInputRow, MEMON_COL_MONSTERTABLE_ID )->SetInteger( pMapInfo->s_dwMonsterTableID );
			pSheet->Cell( siInputRow, MEMON_COL_CENTER_X )->SetInteger( pMapInfo->s_nCenterX );
			pSheet->Cell( siInputRow, MEMON_COL_CENTER_Y )->SetInteger( pMapInfo->s_nCenterY );
			pSheet->Cell( siInputRow, MEMON_COL_RADIUS )->SetInteger( pMapInfo->s_nRadius );
			pSheet->Cell( siInputRow, MEMON_COL_MONCOUNT )->SetInteger( pMapInfo->s_nCount );
			pSheet->Cell( siInputRow, MEMON_COL_MONRESPONTIME )->SetInteger( pMapInfo->s_nResponTime );

			pSheet->Cell( siInputRow, MEMON_COL_KILLGEN_FTID )->SetInteger( pMapInfo->s_nKillgen_MonFTID );
			pSheet->Cell( siInputRow, MEMON_COL_KILLGEN_CNT )->SetInteger( pMapInfo->s_nKillgen_Count );
			pSheet->Cell( siInputRow, MEMON_COL_KILLGEN_VIEWCNT )->SetInteger( pMapInfo->s_nKillgen_ViewCnt );
			pSheet->Cell( siInputRow, MEMON_COL_MOVE )->SetInteger( pMapInfo->s_nMoveType );
			pSheet->Cell( siInputRow, MONSTER_RESPON )->SetInteger( pMapInfo->s_bInsRespon );
			++siInputRow;
		}
	}

	excel.SaveAs( nsCsFileTable::GetPath_MAP() );
}

#define MAP_LIST_MONSTER			"MapMonsterList.bin"
bool CsMapMonsterMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_LIST_MONSTER );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	for( int i = 0; i < nCount; i++)
	{
		DWORD dwFileID;
		fread( &dwFileID, sizeof( DWORD ), 1, fp );

		int nSize;
		fread( &nSize, sizeof( int ), 1, fp );		

		for( int j = 0; j < nSize; j++ )
		{
			DWORD dwMapNum;
			fread( &dwMapNum, sizeof( dwMapNum ), 1, fp );	

			int nMapNum;
			fread( &nMapNum, sizeof( int ), 1, fp );

			for( int k = 0 ; k < nMapNum; k++ )
			{
				CsMapMonster::sINFO sInfo;
				fread( &sInfo, sizeof( CsMapMonster::sINFO ), 1, fp );

				CsMapMonster* Mon = csnew CsMapMonster;
				Mon->Init( &sInfo );

				bool bResult = _AddMapMonster( dwFileID, Mon );
				assert_cs( bResult );
				AddMonster( &sInfo );
			}			
		}
	}	

	fclose( fp );

	return true;
}

void CsMapMonsterMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_LIST_MONSTER );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		DWORD dwFileID;
		_read( nHandle, &dwFileID, sizeof( DWORD ) );
		int nSize;
		_read( nHandle, &nSize, sizeof( int ) );

		for( int j=0; j<nSize; ++j )
		{
			DWORD dwMapNum;
			_read( nHandle, &dwMapNum, sizeof( dwMapNum ) );

			int nMapNum;
			_read( nHandle, &nMapNum, sizeof( int ) );

			for( int k = 0 ; k < nMapNum; k++ )
			{
				CsMapMonster::sINFO sInfo;
				_read( nHandle, &sInfo, sizeof( CsMapMonster::sINFO ) );

				CsMapMonster* Mon = csnew CsMapMonster;
				Mon->Init( &sInfo );

				bool bResult = _AddMapMonster( dwFileID, Mon );
				assert_cs( bResult );
				AddMonster( &sInfo );
			}			
		}
	}
	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

void CsMapMonsterMng::SaveBin( char* cPath )
{
	_SaveData();

	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_LIST_MONSTER );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapMapMonster.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	MAP_IT it = m_mapMapMonster.begin();
	MAP_IT itEnd = m_mapMapMonster.end();
	
	for( ; it != itEnd; it++ )
	{
		DWORD nFileTableID = it->first;
		fwrite( &nFileTableID, sizeof( DWORD ), 1, fp );

		int nSize = (int)it->second->GetMapSize();
		fwrite( &nSize, sizeof( int ), 1, fp );
		
		std::map< DWORD, CsMapMonster::LIST >::iterator it2 = it->second->GetMonsterList()->begin();
		std::map< DWORD, CsMapMonster::LIST >::iterator it2End = it->second->GetMonsterList()->end();

		for( ; it2 != it2End; it2++ )
		{
			CsMapMonster::LIST_IT itMonList = it2->second.begin();
			CsMapMonster::LIST_IT itMonListEnd = it2->second.end();
			
			DWORD dwMapID = it2->first;
			fwrite( &dwMapID, sizeof( DWORD ), 1, fp );

			int nListNum = (int)it2->second.size();
			fwrite( &nListNum, sizeof( int ), 1, fp );

			for( ; itMonList != itMonListEnd; ++itMonList )
			{
				fwrite( (*itMonList)->GetInfo(), sizeof( CsMapMonster::sINFO ), 1, fp );
			}
		}				
	}
	
	fclose( fp );
}

void CsMapMonsterMng::_SaveData()
{
	CsMapMonsterGroup::MAP_IT it = m_mapMapMonsterGroup.begin();
	CsMapMonsterGroup::MAP_IT itEnd = m_mapMapMonsterGroup.end();

	for( ; it != itEnd; it++ )
	{
		CsMapMonster::LIST_IT ItMon = it->second->GetMonsterList()->begin();
		CsMapMonster::LIST_IT ItMonEnd = it->second->GetMonsterList()->end();

		for( ; ItMon != ItMonEnd; ItMon++ )
		{
			CsMapMonster* Mon = csnew CsMapMonster;
			Mon->Init( (*ItMon)->GetInfo() );

			DWORD dwFileTableID = Mon->GetInfo()->s_dwMonsterTableID;
			assert_cs( _AddMapMonster( dwFileTableID, Mon ) );
		}		
	}
}

bool CsMapMonsterMng::_AddMapMonster( DWORD dwId, CsMapMonster* Mon )
{
	if( Mon == NULL )
		return false;

	MAP_IT it = m_mapMapMonster.find( dwId );
	if( it == m_mapMapMonster.end() )
	{
		sMapMonster* sMon = csnew sMapMonster;
		sMon->Init( Mon->GetInfo()->s_dwMapID, Mon );

		m_mapMapMonster[ dwId ] = sMon;
	}
	else
	{
		it->second->Init( Mon->GetInfo()->s_dwMapID, Mon  );
	}
	return true;
}

void CsMapMonsterMng::sMapMonster::Delete()
{
	std::map< DWORD, CsMapMonster::LIST >::iterator it = s_MonList.begin();
	std::map< DWORD, CsMapMonster::LIST >::iterator itEnd = s_MonList.end();

	for( ; it != itEnd; it++ )
	{
		CsMapMonster::LIST_IT it2 = it->second.begin();
		CsMapMonster::LIST_IT it2End = it->second.end();

		for( ;it2 != it2End; it2++ )
		{
			(*it2)->Delete();
			delete *it2;
		}
		it->second.clear();
	}
	s_MonList.clear();
}

void CsMapMonsterMng::sMapMonster::Init( DWORD dwMapID, CsMapMonster* Mon )
{
	std::map< DWORD, CsMapMonster::LIST >::iterator it = s_MonList.find( dwMapID );
	if( it == s_MonList.end() )
	{
		CsMapMonster::LIST MonList;
		MonList.push_back( Mon );		

		s_MonList[ dwMapID ] = MonList;
	}
	else
	{
		it->second.push_back( Mon );
	}
}


//==============================================================================================
//
//		Monster
//
//==============================================================================================
void CsMapMonsterMng::AddMonster( CsMapMonster::sINFO* pInfo )
{
	CsMapMonster* pMapMonster = csnew CsMapMonster;
	pMapMonster->Init( pInfo );

	GetGroup( pInfo->s_dwMapID )->AddMonster( pMapMonster );
}

void CsMapMonsterMng::DeleteMonster( DWORD dwMapID, int nDeleteListIndex )
{
	GetGroup( dwMapID )->DeleteMonster( nDeleteListIndex );	
}

CsMapMonsterMng::sMapMonster* CsMapMonsterMng::GetMonster( DWORD dwMonID )
{
	MAP_IT it = m_mapMapMonster.find( dwMonID );
	if(	it != m_mapMapMonster.end() )
		return it->second;
	else
		return NULL;
}

//==============================================================================================
//
//		Group
//
//==============================================================================================
CsMapMonsterGroup* CsMapMonsterMng::GetGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
	{
		CsMapMonsterGroup* pGroup = csnew CsMapMonsterGroup;
		pGroup->Init();
		m_mapMapMonsterGroup[ dwMapID ] = pGroup;
	}
	return m_mapMapMonsterGroup[ dwMapID ];
}

void CsMapMonsterMng::DeleteGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
	{
		return;
	}

	m_mapMapMonsterGroup[ dwMapID ]->Delete();
	delete m_mapMapMonsterGroup[ dwMapID ];
	m_mapMapMonsterGroup.erase( dwMapID );
}

//#define MAP_MONSTER_BIN_PATH		"GameData\\bin\\MAP_MONSTER.bin"
//void CsMapMonsterMng::SaveBinaryFile()
//{
//	FILE *fp = NULL;
//	fopen_s( &fp, MAP_MONSTER_BIN_PATH, "wb");	 /* 이진 모드 파일 열기 */   
//
//	if( !fp )	
//	{
//		MessageBox( NULL, _T( "GameData\\bin\\ 폴더는 존재 하지 않음." ), NULL, MB_OK );
//		return;
//	}
//
//	int nCnt = (int)m_mapMapMonsterGroup.size();
//	fwrite( &nCnt, sizeof(int), 1, fp );			// 총 갯수
//
//	CsMapMonsterGroup::MAP_IT itGroup = m_mapMapMonsterGroup.begin();
//	for( ; itGroup!=m_mapMapMonsterGroup.end(); ++itGroup )
//	{			
//		CsMapMonster::LIST* pList = itGroup->second->GetMonsterList();
//
//		nCnt = (int)pList->size();
//		fwrite( &nCnt, sizeof(int), 1, fp );			// 총 갯수
//
//		CsMapMonster::LIST_IT itMonster = pList->begin();
//		for( ; itGroup!=m_mapMapMonsterGroup.end(); ++itGroup )
//		{		
//			CsMapMonster::sINFO* pInfo = (*itMonster)->GetInfo();
//
//			fwrite( &pInfo->s_dwMapID, sizeof(DWORD), 1, fp );			
//			fwrite( &pInfo->s_dwMonsterTableID, sizeof(DWORD), 1, fp );			
//			fwrite( &pInfo->s_nCenterX, sizeof(int), 1, fp );			
//			fwrite( &pInfo->s_nCenterY, sizeof(int), 1, fp );			
//			fwrite( &pInfo->s_nRadius, sizeof(int), 1, fp );			
//			fwrite( &pInfo->s_nCount, sizeof(int), 1, fp );			
//			fwrite( &pInfo->s_nResponTime, sizeof(int), 1, fp );			
//		}		
//	}	
//	fclose(fp);
//}
//
//void CsMapMonsterMng::LoadBinaryFile()
//{
//	if( _access_s( MAP_MONSTER_BIN_PATH, 0 ) != 0 )
//	{
//		MessageBox( NULL, LGameData\\bin\\MAP_MONSTER.bin 이 존재 하지 않습니다.", NULL, MB_OK );
//		return;
//	}
//
//
//	FILE *fp = NULL;
//	fopen_s( &fp, MAP_MONSTER_BIN_PATH, "rb");    /* 이진 모드 파일 열기 */   
//
//	if( !fp )	
//		return;
//
//
//	int nCnt;
//	fread( &nCnt, sizeof(int), 1, fp );
//
//	for(int i=0; i<nCnt; ++i)
//	{		
//		int nCount;
//		fread( &nCount, sizeof(int), 1, fp );
//		for(int n=0; n<nCount; ++n)
//		{		
//
//			CsMapMonster::sINFO* pInfo;
//			pInfo = csnew CsMapMonster::sINFO;
//
//			fread( &pInfo->s_dwMapID, sizeof(DWORD), 1, fp );			
//			fread( &pInfo->s_dwMonsterTableID, sizeof(DWORD), 1, fp );			
//			fread( &pInfo->s_nCenterX, sizeof(int), 1, fp );			
//			fread( &pInfo->s_nCenterY, sizeof(int), 1, fp );			
//			fread( &pInfo->s_nRadius, sizeof(int), 1, fp );			
//			fread( &pInfo->s_nCount, sizeof(int), 1, fp );			
//			fread( &pInfo->s_nResponTime, sizeof(int), 1, fp );				
//
//			CsMapMonsterGroup* pMapMonGroup = NULL;
//			CsMapMonster* pMapMonster = NULL;		
//
//			pMapMonster = CsMapMonster::m_MemPool.Pop();
//			pMapMonster->Init( pInfo );		
//
//			GetGroup( pInfo->s_dwMapID )->AddMonster( pMapMonster );	
//
//			SAFE_DELETE(pInfo);	
//		}
//	}	
//
//	fclose(fp);
//}