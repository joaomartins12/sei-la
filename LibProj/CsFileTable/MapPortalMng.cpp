
#include "stdafx.h"
#include "MapPortalMng.h"


void CsMapPortalMng::Delete()
{
	CsMapPortalGroup::MAP_IT itGroup = m_mapMapPortalGroup.begin();
	CsMapPortalGroup::MAP_IT itGroupEnd = m_mapMapPortalGroup.end();
	for( ; itGroup!=itGroupEnd; ++itGroup )
	{
		itGroup->second->Delete();
		delete itGroup->second;
	}
	m_mapMapPortalGroup.clear();
}


bool CsMapPortalMng::Init( char* cPath )
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

CsMapPortal* CsMapPortalMng::GetPortal_ByObjectID( DWORD dwMapID, DWORD dwUniqID )
{
	CsMapPortalGroup::MAP_IT it = m_mapMapPortalGroup.find( dwMapID );
	if( it == m_mapMapPortalGroup.end()) 
		return NULL;

	if( NULL == it->second )
		return NULL;

	return it->second->GetPortal_ByObjectID( dwUniqID );
}

CsMapPortal* CsMapPortalMng::GetNpcPortal_ByObjectID( DWORD dwMapID, DWORD dwUniqID, int nKindIndex )
{
	CsMapPortalGroup::MAP_IT it = m_mapMapPortalGroup.find( dwMapID );
	if( it == m_mapMapPortalGroup.end()) 
		return NULL;

	if( NULL == it->second )
		return NULL;

	return it->second->GetNpcPortal_ByObjectID( dwUniqID, nKindIndex );
}

CsMapPortal* CsMapPortalMng::GetPortal_ByPortalID( DWORD dwMapID, DWORD dwPortalID )
{
	CsMapPortalGroup::MAP_IT it = m_mapMapPortalGroup.find( dwMapID );
	if( it == m_mapMapPortalGroup.end()) 
		return NULL;

	if( NULL == it->second )
		return NULL;

	return it->second->GetPortal_ByPortalID( dwPortalID );
}

CsMapPortal* CsMapPortalMng::GetPortal_ByPos( DWORD dwMapID, int x, int y )
{
	CsMapPortalGroup::MAP_IT it = m_mapMapPortalGroup.find( dwMapID );
	if( it == m_mapMapPortalGroup.end()) 
		return NULL;

	if( NULL == it->second )
		return NULL;

	return it->second->GetPortal_ByPos( x, y );
}

bool CsMapPortalMng::IsPortal_Data( DWORD dwMapID )
{
	CsMapPortalGroup::MAP_IT it = m_mapMapPortalGroup.find( dwMapID );
	if( it == m_mapMapPortalGroup.end()) 
		return false;

	return true;
}

#define MEP_COL_PORTAL_ID			0
#define MEP_COL_PORTAL_TYPE			1
#define MEP_COL_SRC_MAPID			2
#define MEP_COL_SRC_TARGET_X		3
#define MEP_COL_SRC_TARGET_Y		4
#define MEP_COL_SRC_RADIUS			5
#define MEP_COL_DEST_MAPID			6
#define MEP_COL_DEST_TARGET_X		7
#define MEP_COL_DEST_TARGET_Y		8
#define MEP_COL_DEST_RADIUS			9
#define MEP_COL_TYPE				10
#define MEP_COL_UNIQ_OBJECTID		11
#define MEP_COL_KIND_INDEX			12
#define MEP_COL_VIEW_TARGET_X		13
#define MEP_COL_VIEW_TARGET_Y		14

void CsMapPortalMng::_LoadExcel()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP_Portal(), ME_PORTAL_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_PORTAL_VERSION )
	{
		MessageBox( NULL, _T( "MapPortal.xls Portal Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );


	// 데이터 입력
	CsMapPortal* pMapPortal = NULL;
	CsMapPortalGroup* pMapGroup = NULL;
	CsMapPortal::sINFO info;
	while( excel.MoveNext() == true )
	{
		info.s_dwPortalID = atoi( excel.GetData( MEP_COL_PORTAL_ID, cBuffer ) );
		if( info.s_dwPortalID == 0 )
			continue;
		info.s_dwPortalType = atoi( excel.GetData( MEP_COL_PORTAL_TYPE, cBuffer ) );
		info.s_dwSrcMapID = atoi( excel.GetData( MEP_COL_SRC_MAPID, cBuffer ) );
		info.s_nSrcTargetX = atoi( excel.GetData( MEP_COL_SRC_TARGET_X, cBuffer ) );
		info.s_nSrcTargetY = atoi( excel.GetData( MEP_COL_SRC_TARGET_Y, cBuffer ) );
		info.s_nSrcRadius = atoi( excel.GetData( MEP_COL_SRC_RADIUS, cBuffer ) );
		info.s_dwDestMapID = atoi( excel.GetData( MEP_COL_DEST_MAPID, cBuffer ) );
		info.s_nDestTargetX = atoi( excel.GetData( MEP_COL_DEST_TARGET_X, cBuffer ) );
		info.s_nDestTargetY = atoi( excel.GetData( MEP_COL_DEST_TARGET_Y, cBuffer ) );
		info.s_nDestRadius = atoi( excel.GetData( MEP_COL_DEST_RADIUS, cBuffer ) );
		info.s_ePortalType = (CsMapPortal::ePORTAL_TYPE)atoi( excel.GetData( MEP_COL_TYPE, cBuffer ) );
		info.s_dwUniqObjectID = atoi( excel.GetData( MEP_COL_UNIQ_OBJECTID, cBuffer ) );
		info.s_nPortalKindIndex = atoi( excel.GetData( MEP_COL_KIND_INDEX, cBuffer ) );
		info.s_nViewTargetX = atoi( excel.GetData( MEP_COL_VIEW_TARGET_X, cBuffer ) );
		info.s_nViewTargetY = atoi( excel.GetData( MEP_COL_VIEW_TARGET_Y, cBuffer ) );

		pMapPortal = csnew CsMapPortal;
		pMapPortal->Init( &info );

		GetGroup( info.s_dwSrcMapID )->AddPortal( pMapPortal );
	}
	excel.Close();
}

void CsMapPortalMng::Save()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP_Portal() );
	assert_cs( bLoad == true );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_PORTAL_SHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->DeleteAllRows();

	pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_PORTAL_VERSION );
	for( int i=1; i<=MEP_COL_VIEW_TARGET_Y+10; ++i )
	{
		pSheet->Cell( 0, i )->SetInteger( 1 );
	}

	pSheet->Cell( 1, MEP_COL_PORTAL_ID )->SetWString( _T( "portal idx" ) );
	pSheet->Cell( 1, MEP_COL_PORTAL_TYPE )->SetWString( _T( "portal_type_Test" ) );
	pSheet->Cell( 1, MEP_COL_SRC_MAPID )->SetWString( _T( "src map id" ) );
	pSheet->Cell( 1, MEP_COL_SRC_TARGET_X )->SetWString( _T( "src target x" ) );
	pSheet->Cell( 1, MEP_COL_SRC_TARGET_Y )->SetWString( _T( "src target y" ) );
	pSheet->Cell( 1, MEP_COL_SRC_RADIUS )->SetWString( _T( "src radius" ) );
	pSheet->Cell( 1, MEP_COL_DEST_MAPID )->SetWString( _T( "dest map id" ) );
	pSheet->Cell( 1, MEP_COL_DEST_TARGET_X )->SetWString( _T( "dest target x" ) );
	pSheet->Cell( 1, MEP_COL_DEST_TARGET_Y )->SetWString( _T( "dest target y" ) );
	pSheet->Cell( 1, MEP_COL_DEST_RADIUS )->SetWString( _T( "dest radius" ) );
	pSheet->Cell( 1, MEP_COL_TYPE )->SetWString( _T( "Type" ) );
	pSheet->Cell( 1, MEP_COL_UNIQ_OBJECTID )->SetWString( _T( "Uniq ObjectID" ) );
	pSheet->Cell( 1, MEP_COL_KIND_INDEX )->SetWString( _T( "Kind Index" ) );
	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_X )->SetWString( _T( "ViewTarget_X" ) );
	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y )->SetWString( _T( "ViewTarget_Y" ) );

	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y+1 )->SetWString( _T( "ReqCount" ) );

	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y+2 )->SetWString( _T( "Req1_Type" ) );
	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y+3 )->SetWString( _T( "Req1_ID" ) );
	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y+4 )->SetWString( _T( "Req1_Count" ) );

	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y+5 )->SetWString( _T( "Req2_Type" ) );
	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y+6 )->SetWString( _T( "Req2_ID" ) );
	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y+7 )->SetWString( _T( "Req2_Count" ) );

	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y+8 )->SetWString( _T( "Req3_Type" ) );
	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y+9 )->SetWString( _T( "Req3_ID" ) );
	pSheet->Cell( 1, MEP_COL_VIEW_TARGET_Y+10 )->SetWString( _T( "Req3_Count" ) );
	

	size_t siInputRow = 2;
	CsMapPortalGroup::MAP_IT itGroup = m_mapMapPortalGroup.begin();
	CsMapPortal::sINFO* pMapInfo = NULL;
	for( ; itGroup!=m_mapMapPortalGroup.end(); ++itGroup )
	{
		CsMapPortal::MAP* pMap = itGroup->second->GetPortalMap();
		CsMapPortal::MAP_IT itPortal = pMap->begin();
		for( ; itPortal != pMap->end(); ++itPortal )
		{
			pMapInfo = itPortal->second->GetInfo();
			pSheet->Cell( siInputRow, MEP_COL_PORTAL_ID )->SetInteger(  pMapInfo->s_dwPortalID );
			pSheet->Cell( siInputRow, MEP_COL_PORTAL_TYPE )->SetInteger(  pMapInfo->s_dwPortalType );
			pSheet->Cell( siInputRow, MEP_COL_SRC_MAPID )->SetInteger(  pMapInfo->s_dwSrcMapID );
			pSheet->Cell( siInputRow, MEP_COL_SRC_TARGET_X )->SetInteger(  pMapInfo->s_nSrcTargetX );
			pSheet->Cell( siInputRow, MEP_COL_SRC_TARGET_Y )->SetInteger(  pMapInfo->s_nSrcTargetY );
			pSheet->Cell( siInputRow, MEP_COL_SRC_RADIUS )->SetInteger(  pMapInfo->s_nSrcRadius );
			pSheet->Cell( siInputRow, MEP_COL_DEST_MAPID )->SetInteger(  pMapInfo->s_dwDestMapID );
			pSheet->Cell( siInputRow, MEP_COL_DEST_TARGET_X )->SetInteger(  pMapInfo->s_nDestTargetX );
			pSheet->Cell( siInputRow, MEP_COL_DEST_TARGET_Y )->SetInteger(  pMapInfo->s_nDestTargetY );
			pSheet->Cell( siInputRow, MEP_COL_DEST_RADIUS )->SetInteger(  pMapInfo->s_nDestRadius );
			pSheet->Cell( siInputRow, MEP_COL_TYPE )->SetInteger(  pMapInfo->s_ePortalType );
			pSheet->Cell( siInputRow, MEP_COL_UNIQ_OBJECTID )->SetInteger(  pMapInfo->s_dwUniqObjectID );
			pSheet->Cell( siInputRow, MEP_COL_KIND_INDEX )->SetInteger(  pMapInfo->s_nPortalKindIndex );
			pSheet->Cell( siInputRow, MEP_COL_VIEW_TARGET_X )->SetInteger(  pMapInfo->s_nViewTargetX );
			pSheet->Cell( siInputRow, MEP_COL_VIEW_TARGET_Y )->SetInteger(  pMapInfo->s_nViewTargetY );

			// 빈칸을 0 으로 다 채운다.
			int nTypeColIndex = MEP_COL_VIEW_TARGET_Y + 1;
			for( int i=nTypeColIndex; i<=MEP_COL_VIEW_TARGET_Y+10; ++i )
			{
				pSheet->Cell( siInputRow, i )->SetInteger( 0 );
			}

			if( pMapInfo->s_ePortalType == CsMapPortal::NPC )
			{
				sNPC_TYPE_PORTAL::sPORTAL* pPortal = itPortal->second->GetLinkNpcType();
				if( pPortal )
				{					
					int nPTypeCount = 0;
					for( int p=0; p<NPC_TYPE_PORTAL_MAX_REQ_COUNT; ++p )
					{
						if( pPortal->s_ReqArray[ p ].s_eEnableType != sNPC_TYPE_PORTAL::None )
						{
							++nPTypeCount;
						}
					}					
					pSheet->Cell( siInputRow, nTypeColIndex )->SetInteger( nPTypeCount );
					++nTypeColIndex;
					for( int p=0; p<NPC_TYPE_PORTAL_MAX_REQ_COUNT; ++p )
					{
						if( pPortal->s_ReqArray[ p ].s_eEnableType == sNPC_TYPE_PORTAL::None )
							continue;

						pSheet->Cell( siInputRow, nTypeColIndex )->SetInteger( pPortal->s_ReqArray[ p ].s_eEnableType );
						++nTypeColIndex;

						pSheet->Cell( siInputRow, nTypeColIndex )->SetInteger( pPortal->s_ReqArray[ p ].s_nEnableID );
						++nTypeColIndex;

						pSheet->Cell( siInputRow, nTypeColIndex )->SetInteger( pPortal->s_ReqArray[ p ].s_nEnableCount );
						++nTypeColIndex;
					}
				}
			}

			++siInputRow;
		}
	}

	excel.SaveAs( nsCsFileTable::GetPath_MAP_Portal() );
}


//==============================================================================================
//
//		Portal
//
//==============================================================================================
void CsMapPortalMng::AddPortal( CsMapPortal::sINFO* pInfo, sNPC_TYPE_PORTAL::sPORTAL* pLink )
{
	CsMapPortal* pMapPortal = csnew CsMapPortal;
	pMapPortal->Init( pInfo );

	GetGroup( pInfo->s_dwSrcMapID )->AddPortal( pMapPortal );
	pMapPortal->SetLinkNpcType( pLink );
}


//==============================================================================================
//
//		Group
//
//==============================================================================================
CsMapPortalGroup* CsMapPortalMng::GetGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
	{
		CsMapPortalGroup* pGroup = csnew CsMapPortalGroup;
		pGroup->Init();
		m_mapMapPortalGroup[ dwMapID ] = pGroup;
	}
	return m_mapMapPortalGroup[ dwMapID ];
}

void CsMapPortalMng::DeleteGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
	{
		return;
	}
	m_mapMapPortalGroup[ dwMapID ]->Delete();
	delete m_mapMapPortalGroup[ dwMapID ];
	m_mapMapPortalGroup.erase( dwMapID );
}

void CsMapPortalMng::DeleteGroup_PortalType( DWORD dwMapID, CsMapPortal::ePORTAL_TYPE type )
{
	if( IsGroup( dwMapID ) == false )
	{
		return;
	}

	CsMapPortal::MAP* pMap = m_mapMapPortalGroup[ dwMapID ]->GetPortalMap();
	CsMapPortal::MAP_IT it = pMap->begin();
	CsMapPortal::MAP_IT itDelete = it;
	CsMapPortal::MAP_IT itEnd = pMap->end();
	while( it!=itEnd )
	{
		itDelete = it;
		++it;
		if( itDelete->second->GetInfo()->s_ePortalType == type )
		{
			itDelete->second->Delete();
			delete itDelete->second;
			pMap->erase( itDelete );
		}
	}

	if( pMap->empty() == true )
	{
		delete m_mapMapPortalGroup[ dwMapID ];
		m_mapMapPortalGroup.erase( dwMapID );
	}	
}

#define MAP_PORTAL_BIN		"MapPortal.bin"

void CsMapPortalMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_PORTAL_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapMapPortalGroup.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsMapPortalGroup::MAP_IT itGroup = m_mapMapPortalGroup.begin();
	CsMapPortalGroup::MAP_IT itGroupEnd = m_mapMapPortalGroup.end();
	for( ; itGroup!=itGroupEnd; ++itGroup )
	{
		CsMapPortal::MAP* pMap = itGroup->second->GetPortalMap();
		nCount = (int)pMap->size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsMapPortal::MAP_IT it = pMap->begin();
		CsMapPortal::MAP_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsMapPortal::sINFO ), 1, fp );
		}
	}
	fclose( fp );
}

bool CsMapPortalMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_PORTAL_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount, nCountPortal;
	fread( &nCount, sizeof( int ), 1, fp );

	CsMapPortal::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &nCountPortal, sizeof( int ), 1, fp );
		for( int p=0; p<nCountPortal; ++p )
		{
			fread( &info, sizeof( CsMapPortal::sINFO ), 1, fp );
			AddPortal( &info, NULL );
		}		
	}

	fclose( fp );

	return true;
}

void CsMapPortalMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_PORTAL_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;
	
	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );
	
	int nCount, nCountPortal;
	_read( nHandle, &nCount, sizeof( int ) );

	CsMapPortal::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &nCountPortal, sizeof( int ) );
		for( int p=0; p<nCountPortal; ++p )
		{
			_read( nHandle, &info, sizeof( CsMapPortal::sINFO ) );
			AddPortal( &info, NULL );
		}		
	}
	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

