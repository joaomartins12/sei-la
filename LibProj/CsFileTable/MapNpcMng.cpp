
#include "stdafx.h"
#include "MapNpcMng.h"

void CsMapNpcMng::Delete()
{
	MAP_MAPNPC_IT itNpc = m_mapMapNpc.begin();
	MAP_MAPNPC_IT itNpcEnd = m_mapMapNpc.end();
	for( ; itNpc!=itNpcEnd; ++itNpc )
	{
		itNpc->second->Delete();
		delete itNpc->second;
	}
	m_mapMapNpc.clear();

	MAP_MAPNPCGROUP_IT itGroup = m_mapMapNpcGroup.begin();
	MAP_MAPNPCGROUP_IT itGroupEnd = m_mapMapNpcGroup.end();
	for( ; itGroup!=itGroupEnd; ++itGroup )
	{
		itGroup->second->Delete();
		delete itGroup->second;
	}
	m_mapMapNpcGroup.clear();
}

bool CsMapNpcMng::Init( char* cPath )
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
	}
	return true;
}

#define	MEN_TYPE_ID				0
#define	MEN_MAP_ID				1
#define MEN_MODEL_POS_X			2
#define MEN_MODEL_POS_Y			3
#define MEN_MODEL_ROT			4

void CsMapNpcMng::Save()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP() );
	assert_cs( bLoad == true );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_NPC_SHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->DeleteAllRows();

	pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_NPC_VERSION );
	
	pSheet->Cell( 1, MEN_TYPE_ID )->SetWString( _T( "NpcID" ) );
	pSheet->Cell( 1, MEN_MAP_ID )->SetWString( _T( "MapID" ) );
	pSheet->Cell( 1, MEN_MODEL_POS_X )->SetWString( _T( "Start PosX" ) );
	pSheet->Cell( 1, MEN_MODEL_POS_Y )->SetWString( _T( "Start PosY" ) );
	pSheet->Cell( 1, MEN_MODEL_ROT )->SetWString( _T( "Rot" ) );

	DWORD dwMaxColCount = 0;
	size_t siInputRow = 2;
	MAP_MAPNPCGROUP_IT itGroup = m_mapMapNpcGroup.begin();
	CsMapNpc::sINFO* pMapInfo = NULL;
	for( ; itGroup!=m_mapMapNpcGroup.end(); ++itGroup )
	{
		MAP_MAPNPC* pMap = itGroup->second->GetNpcMap();
		MAP_MAPNPC_IT itNpc = pMap->begin();
		for( ; itNpc != pMap->end(); ++itNpc )
		{
			pMapInfo = itNpc->second->GetInfo();
			pSheet->Cell( siInputRow, MEN_TYPE_ID )->SetInteger(  pMapInfo->s_dwNpcID );
			pSheet->Cell( siInputRow, MEN_MAP_ID )->SetInteger(  pMapInfo->s_dwMapID );
			pSheet->Cell( siInputRow, MEN_MODEL_POS_X )->SetInteger(  pMapInfo->s_nInitPosX );
			pSheet->Cell( siInputRow, MEN_MODEL_POS_Y )->SetInteger(  pMapInfo->s_nInitPosY );
			pSheet->Cell( siInputRow, MEN_MODEL_ROT )->SetDouble(  pMapInfo->s_fRotation );
			
			++siInputRow;
		}
	}

	excel.SaveAs( nsCsFileTable::GetPath_MAP() );
}

void CsMapNpcMng::_LoadExcel()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP(), ME_NPC_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_NPC_VERSION )
	{
		MessageBox( NULL, _T( "Map.xls Npc Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	// 데이터 입력
	CsMapNpc::sINFO info;
	CsMapNpc* pMapNpc = NULL;
	while( excel.MoveNext() == true )
	{
		if( atoi( excel.GetData( MEN_MAP_ID, cBuffer ) ) == 0 )
			continue;

		info.Init( atoi( excel.GetData( MEN_MAP_ID, cBuffer ) ), atoi( excel.GetData( MEN_TYPE_ID, cBuffer ) ) );
		info.s_nInitPosX = atoi( excel.GetData( MEN_MODEL_POS_X, cBuffer ) );
		info.s_nInitPosY = atoi( excel.GetData( MEN_MODEL_POS_Y, cBuffer ) );
		info.s_fRotation = (float)atof( excel.GetData( MEN_MODEL_ROT, cBuffer ) );

		AddNpc( &info );
	}
	excel.Close();
}

CsMapNpc* CsMapNpcMng::GetNpc( DWORD dwNpcID )
{
	MAP_MAPNPC_IT it = m_mapMapNpc.find( dwNpcID );
	if( it == m_mapMapNpc.end() )
		return NULL;
	return it->second;
}

CsMapNpc* CsMapNpcMng::AddNpc( CsMapNpc::sINFO* pInfo )
{
	assert_csm1( IsNpc( pInfo->s_dwNpcID ) == false, _T( "이미 NPC가 존재합!! Npc = %d" ), pInfo->s_dwNpcID );

	// 그룹이 존재 하지 않는다면 새로 생성
	if( m_mapMapNpcGroup.find( pInfo->s_dwMapID ) == m_mapMapNpcGroup.end() )
	{
		CsMapNpcGroup* pMapGroup = csnew CsMapNpcGroup;
		pMapGroup->Init();
		m_mapMapNpcGroup[ pInfo->s_dwMapID ] = pMapGroup;
	}

	CsMapNpc* pMapNpc = csnew CsMapNpc;
	pMapNpc->Init( pInfo );
	m_mapMapNpcGroup[ pInfo->s_dwMapID ]->AddNpc( pMapNpc );
	m_mapMapNpc[ pInfo->s_dwNpcID ] = pMapNpc;

	return pMapNpc;
}

void CsMapNpcMng::DeleteNpc( DWORD dwMapID, DWORD dwNpcID )
{
	assert_cs( IsNpc( dwNpcID ) == true );
	m_mapMapNpc[ dwNpcID ]->Delete();
	delete m_mapMapNpc[ dwNpcID ];
	m_mapMapNpc.erase( dwNpcID );

	assert_cs( m_mapMapNpcGroup.find( dwMapID ) != m_mapMapNpcGroup.end() );
	m_mapMapNpcGroup[ dwMapID ]->DeleteNpc( dwNpcID );
}

void CsMapNpcMng::DeleteGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
		return;

	assert_cs( m_mapMapNpcGroup.find( dwMapID ) != m_mapMapNpcGroup.end() );

	CsMapNpcGroup* pGroup = m_mapMapNpcGroup[ dwMapID ];
	MAP_MAPNPC* pmapNpc = pGroup->GetNpcMap();
	MAP_MAPNPC_IT it = pmapNpc->begin();
	DWORD dwDelNpcID;
	CsMapNpc* pNpc;
	for( ; it!=pmapNpc->end(); ++it )
	{
		dwDelNpcID = it->second->GetInfo()->s_dwNpcID;
		pNpc = m_mapMapNpc[ dwDelNpcID ];
		pNpc->Delete();
		delete pNpc;
		m_mapMapNpc.erase( dwDelNpcID );
	}

	m_mapMapNpcGroup[ dwMapID ]->Delete();
	delete m_mapMapNpcGroup[ dwMapID ];
	m_mapMapNpcGroup.erase( dwMapID );
}

CsMapNpcGroup* CsMapNpcMng::GetGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
	{
		CsMapNpcGroup* pGroup = csnew CsMapNpcGroup;
		pGroup->Init();
		m_mapMapNpcGroup[ dwMapID ] = pGroup;
	}
	return m_mapMapNpcGroup[ dwMapID ];
}

#define MAP_NPC_BIN		"MapNpc.bin"
void CsMapNpcMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_NPC_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapMapNpc.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	MAP_MAPNPC_IT itNpc = m_mapMapNpc.begin();
	MAP_MAPNPC_IT itNpcEnd = m_mapMapNpc.end();
	for( ; itNpc!=itNpcEnd; ++itNpc )
	{
		fwrite( itNpc->second->GetInfo(), sizeof( CsMapNpc::sINFO ), 1, fp );
	}
	fclose( fp );
}

bool CsMapNpcMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_NPC_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsMapNpc::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &info, sizeof( CsMapNpc::sINFO ), 1, fp );
		AddNpc( &info );
	}

	fclose( fp );
	return true;
}

void CsMapNpcMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_NPC_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	CsMapNpc::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info, sizeof( CsMapNpc::sINFO ) );
		AddNpc( &info );
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}
