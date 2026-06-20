


#include "stdafx.h"
#include "MapListMng.h"

void CsMapListMng::Delete()
{
	CsMapList::MAP_IT it = m_mapMapList.begin();
	CsMapList::MAP_IT itEnd = m_mapMapList.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapMapList.clear();
}

bool CsMapListMng::Init( char* cPath, nsCsFileTable::eFILE_TYPE ft /*=nsCsFileTable::g_eFileType*/ )
{
	switch( ft )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel();
		break;
	case nsCsFileTable::FT_CLONE:
		assert_cs( nsCsMapTable::g_pMapListMng );
		Clone( nsCsMapTable::g_pMapListMng );
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

bool CsMapListMng::IsList( DWORD dwMapID )
{ 
	CsMapList::MAP::iterator it = m_mapMapList.find( dwMapID );
	if( it == m_mapMapList.end() )
		return false;
	return true;
}

void CsMapListMng::AddList( CsMapList::sINFO* pInfo )
{
	SAFE_POINTER_RET( pInfo );
	CsMapList::MAP::iterator it = m_mapMapList.find( pInfo->s_dwMapID );
	if( it != m_mapMapList.end() )
	{
		assert_csm1( false, _T( "이미 있는 맵입니다. 번호 : %d" ), pInfo->s_dwMapID );
		return;
	}
	CsMapList* pMapList = csnew CsMapList;
	pMapList->Init( pInfo );
	m_mapMapList.insert( make_pair( pInfo->s_dwMapID, pMapList ) );
}

std::string	CsMapListMng::GetMapPath( DWORD const& dwMapID ) const
{
	CsMapList::MAP::const_iterator it = m_mapMapList.find( dwMapID );
	if( it == m_mapMapList.end() )
		return "";

	SAFE_POINTER_RETVAL( it->second, "" );
	CsMapList::sINFO const* pInfo = it->second->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, "" );

	return pInfo->s_cMapPath;
}

CsMapList* CsMapListMng::GetList( DWORD dwMapID )
{
	//assert_csm1( IsList( dwMapID ) == true, _T( "맵번호 : %d" ), dwMapID );
	
	CsMapList::MAP::iterator it = m_mapMapList.find( dwMapID );
	if( it == m_mapMapList.end() )
	{
		assert_csm1( false, _T( "맵번호 : %d" ), dwMapID );
		return NULL;
	}
	return it->second;
}

CsMapList* CsMapListMng::GetList( const char* cMapPath )
{
	SAFE_POINTER_RETVAL(cMapPath, NULL);

	char const* cDestName = nsCSFILE::GetFileName( cMapPath );

	CsMapList::MAP_IT it = m_mapMapList.begin();
	CsMapList::MAP_IT itEnd = m_mapMapList.end();
	for( ; it!=itEnd; ++it )
	{
		CsMapList* pMapList = it->second;
		SAFE_POINTER_CON( pMapList );

		CsMapList::sINFO * pInfo = pMapList->GetInfo();
		SAFE_POINTER_CON( pInfo );

		if( _stricmp( nsCSFILE::GetFileName( pInfo->s_cMapPath.c_str() ), cDestName ) == 0 )
			return it->second;
	}
	return NULL;
}

CsMapList* CsMapListMng::GetListIndex( int nIndex )
{
	CsMapList::MAP_IT it = m_mapMapList.begin();
	CsMapList::MAP_IT itEnd = m_mapMapList.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return it->second;

		--nIndex;
	}
	assert_cs( false );
	return NULL;
}

int CsMapListMng::MapIDToMapRegionID( DWORD dwMapID )
{
	CsMapList* pMapList = GetList( dwMapID );
	SAFE_POINTER_RETVAL(pMapList, 0);
	CsMapList::sINFO * pInfo = pMapList->GetInfo();
	SAFE_POINTER_RETVAL(pInfo, 0);

	return pInfo->s_nMapRegionID;
}

//======================================================================================================
//
//		클론
//
//======================================================================================================

void CsMapListMng::Clone( CsMapListMng* pSrcMng )
{
	SAFE_POINTER_RET( pSrcMng );

	assert_cs( m_mapMapList.empty() == true );

	CsMapList::MAP* pSrc = pSrcMng->GetListMap();
	CsMapList::MAP_IT itSrc = pSrc->begin();
	CsMapList::MAP_IT itEnd = pSrc->end();
	for( ; itSrc!=itEnd; ++itSrc )
	{
		CsMapList* pChild = csnew CsMapList;
		pChild->Init( itSrc->second->GetInfo() );
		m_mapMapList.insert( make_pair( pChild->GetInfo()->s_dwMapID, pChild ) );
		//m_mapMapList[ pChild->GetInfo()->s_dwMapID ] = pChild;
	}
}

bool CsMapListMng::IsBattleTagUseMap( DWORD const& dwMapID ) const
{
	CsMapList::MAP::const_iterator it = m_mapMapList.find( dwMapID );
	if( it == m_mapMapList.end() )
		return false;

	SAFE_POINTER_RETVAL( it->second, false );

	CsMapList::sINFO const* pInfo = it->second->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, false );

	return !pInfo->s_bBattleTagUse;
}

#define MEL_COL_MAPID				0
#define MEL_COL_MAPPATH				1
#define MEL_COL_MAPNAME				2
#define MEL_COL_MAPWIDTH			3
#define MEL_COL_MAPHEIGHT			4
#define MEL_COL_MAPDESCRIPTION		5
#define MEL_COL_MAPDESCRIPTION_ENG	6
#define MEL_COL_MAPBGSOUND			7
#define MEL_COL_RESURRECTION_MAPID	8
#define MEL_COL_FATIGUE_TYPE		9
#define MEL_COL_FATIGUE_DEBUFF		10
#define MEL_COL_FATIGUE_STARTTIME	11
#define MEL_COL_FATIGUE_ADDTIME		12
#define MEL_COL_FATIGUE_POINT		13
#define MEL_COL_CAMERA_DIST_MAX_LEVEL	14
#define MEL_COL_XG_CONSUMETYPE	15
#define MEL_COL_BATTLETAGE_USE	16

void CsMapListMng::_LoadExcel()
{
	char cBuffer[ CEXCLE_CELL_LEN ];
//	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP_List(), ME_LIST_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_LIST_VERSION )
	{
		MessageBox( NULL, _T( "Map.xls list Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	// 데이터 입력
	CsMapList* pMapList = NULL;
	CsMapList::sINFO info;

	while( excel.MoveNext() == true )
	{
		info.Init();

		excel.Read_Value( MEL_COL_MAPID, info.s_dwMapID );
		if( info.s_dwMapID == 0 )
			continue;

		excel.Read_Value( MEL_COL_MAPWIDTH, info.s_nWidth );
		excel.Read_Value( MEL_COL_MAPHEIGHT, info.s_nHeight );
		excel.Read_Value( MEL_COL_MAPPATH, info.s_cMapPath);
		excel.Read_Value( MEL_COL_MAPNAME, info.s_cMapName);

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			excel.Read_Value( MEL_COL_MAPDESCRIPTION, info.s_szMapDiscript);
			excel.Read_Value( MEL_COL_MAPDESCRIPTION_ENG, info.s_szMapDiscript_Eng);		
			break;
		default:
			excel.Read_Value( MEL_COL_MAPDESCRIPTION_ENG, info.s_szMapDiscript);
			excel.Read_Value( MEL_COL_MAPDESCRIPTION, info.s_szMapDiscript_Eng);	
			break;
		}

		excel.Read_Value( MEL_COL_MAPBGSOUND, info.s_cBGSound );	

		excel.Read_Value( MEL_COL_RESURRECTION_MAPID, info.s_dwResurrectionMapID );

		excel.Read_Value( MEL_COL_FATIGUE_TYPE, info.s_nFatigue_Type );
		excel.Read_Value( MEL_COL_FATIGUE_DEBUFF, info.s_nFatigue_DeBuff );

		excel.Read_Value( MEL_COL_FATIGUE_STARTTIME, info.s_nFatigue_StartTime );

		excel.Read_Value( MEL_COL_FATIGUE_ADDTIME, info.s_nFatigue_AddTime );
		excel.Read_Value( MEL_COL_FATIGUE_POINT, info.s_nFatigue_AddPoint );

		excel.Read_Value( MEL_COL_CAMERA_DIST_MAX_LEVEL, info.s_nCamera_Max_Level );
		excel.Read_Value( MEL_COL_XG_CONSUMETYPE, info.s_bXgConsumeType );
		excel.Read_Value( MEL_COL_BATTLETAGE_USE, info.s_bBattleTagUse );
		 
		pMapList = csnew CsMapList;
		pMapList->Init( &info );
		assert_cs( m_mapMapList.find( info.s_dwMapID ) == m_mapMapList.end() );
		m_mapMapList[ info.s_dwMapID ] = pMapList;
	}
	excel.Close();


#define BASE_MAPINFO_SHEET_NAME			"MapInfo"

	cExcelReader excelReder;
	bSuccess = excelReder.Open( nsCsFileTable::GetPath_DM_Base(), BASE_MAPINFO_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excelReder.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excelReder.MoveNext();
	assert_cs( bSuccess == true );

	int nMapID;
	while( excelReder.MoveNext() == true )
	{
		nMapID = atoi( excelReder.GetData( 3, cBuffer ) );
		if( m_mapMapList.find( nMapID ) != m_mapMapList.end() )
		{
			m_mapMapList[ nMapID ]->GetInfo()->s_nMapRegionID = atoi( excelReder.GetData( 0, cBuffer ) );
		}
	}
	excelReder.Close();	
}

void CsMapListMng::Save()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP_List() );
	assert_cs( bLoad == true );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_LIST_SHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->DeleteAllRows();

	pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_LIST_VERSION );
	for( int i=1; i<=MEL_COL_CAMERA_DIST_MAX_LEVEL; ++i )
	{
		pSheet->Cell( 0, i )->SetInteger( 1 );
	}	

	pSheet->Cell( 1, MEL_COL_MAPID )->SetWString( _T( "idx" ) );
	pSheet->Cell( 1, MEL_COL_MAPPATH )->SetWString( _T( "path" ) );
	pSheet->Cell( 1, MEL_COL_MAPNAME )->SetWString( _T( "name" ) );
	pSheet->Cell( 1, MEL_COL_MAPWIDTH )->SetWString( _T( "width" ) );
	pSheet->Cell( 1, MEL_COL_MAPHEIGHT )->SetWString( _T( "height" ) );
	pSheet->Cell( 1, MEL_COL_MAPDESCRIPTION )->SetWString( _T( "descript" ) );
	pSheet->Cell( 1, MEL_COL_MAPDESCRIPTION_ENG )->SetWString( _T( "descript" ) );
	pSheet->Cell( 1, MEL_COL_MAPBGSOUND )->SetWString( _T( "sound" ) );
	pSheet->Cell( 1, MEL_COL_RESURRECTION_MAPID )->SetWString( _T( "Resurrection MapID" ) );
	pSheet->Cell( 1, MEL_COL_FATIGUE_TYPE )->SetWString( _T( "FG Type" ) );
	pSheet->Cell( 1, MEL_COL_FATIGUE_DEBUFF	)->SetWString( _T( "FG D-Buff" ) );
	pSheet->Cell( 1, MEL_COL_FATIGUE_STARTTIME )->SetWString( _T( "FG StartTime" ) );
	pSheet->Cell( 1, MEL_COL_FATIGUE_ADDTIME )->SetWString( _T( "FG AddTime" ) );
	pSheet->Cell( 1, MEL_COL_FATIGUE_POINT )->SetWString( _T( "FG Point" ) );
	pSheet->Cell( 1, MEL_COL_CAMERA_DIST_MAX_LEVEL )->SetWString( _T( "Camera Max Level" ) );
	pSheet->Cell( 1, MEL_COL_XG_CONSUMETYPE )->SetWString( _T( "XG Consume Type" ) );
	pSheet->Cell( 1, MEL_COL_BATTLETAGE_USE )->SetWString( _T( "BattleTag_Use" ) );

	size_t siInputRow = 2;
	CsMapList::MAP_IT it = m_mapMapList.begin();
	CsMapList::sINFO* pMapInfo = NULL;
	for( ; it!=m_mapMapList.end(); ++it )
	{
		pMapInfo = it->second->GetInfo();
		pSheet->Cell( siInputRow, MEL_COL_MAPID )->SetInteger(  pMapInfo->s_dwMapID );
		pSheet->Cell( siInputRow, MEL_COL_MAPPATH )->SetString( pMapInfo->s_cMapPath.c_str() );
		pSheet->Cell( siInputRow, MEL_COL_MAPNAME )->SetString( pMapInfo->s_cMapName.c_str() );
		pSheet->Cell( siInputRow, MEL_COL_MAPWIDTH )->SetInteger( pMapInfo->s_nWidth );
		pSheet->Cell( siInputRow, MEL_COL_MAPHEIGHT )->SetInteger( pMapInfo->s_nHeight );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			pSheet->Cell( siInputRow, MEL_COL_MAPDESCRIPTION )->SetWString( pMapInfo->s_szMapDiscript.c_str() );
			pSheet->Cell( siInputRow, MEL_COL_MAPDESCRIPTION_ENG )->SetWString( pMapInfo->s_szMapDiscript_Eng.c_str() );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			pSheet->Cell( siInputRow, MEL_COL_MAPDESCRIPTION )->SetWString( pMapInfo->s_szMapDiscript_Eng.c_str() );
			pSheet->Cell( siInputRow, MEL_COL_MAPDESCRIPTION_ENG )->SetWString( pMapInfo->s_szMapDiscript.c_str() );
			break;
		default:
			assert_cs( false );
		}
		
		pSheet->Cell( siInputRow, MEL_COL_MAPBGSOUND )->SetString( pMapInfo->s_cBGSound.c_str() );
		pSheet->Cell( siInputRow, MEL_COL_RESURRECTION_MAPID )->SetInteger( pMapInfo->s_dwResurrectionMapID );

		pSheet->Cell( siInputRow, MEL_COL_FATIGUE_TYPE )->SetInteger( pMapInfo->s_nFatigue_Type );
		pSheet->Cell( siInputRow, MEL_COL_FATIGUE_DEBUFF )->SetInteger( pMapInfo->s_nFatigue_DeBuff );
		pSheet->Cell( siInputRow, MEL_COL_FATIGUE_STARTTIME )->SetInteger( pMapInfo->s_nFatigue_StartTime );
		pSheet->Cell( siInputRow, MEL_COL_FATIGUE_ADDTIME )->SetInteger( pMapInfo->s_nFatigue_AddTime );
		pSheet->Cell( siInputRow, MEL_COL_FATIGUE_POINT )->SetInteger( pMapInfo->s_nFatigue_AddPoint );
		pSheet->Cell( siInputRow, MEL_COL_CAMERA_DIST_MAX_LEVEL )->SetInteger( pMapInfo->s_nCamera_Max_Level );
		pSheet->Cell( siInputRow, MEL_COL_XG_CONSUMETYPE )->SetInteger( pMapInfo->s_bXgConsumeType );
		pSheet->Cell( siInputRow, MEL_COL_BATTLETAGE_USE )->SetInteger( pMapInfo->s_bBattleTagUse );
		++siInputRow;
	}

	excel.SaveAs( nsCsFileTable::GetPath_MAP_List() );
}

#define MAP_LIST_BIN			"MapList.bin"
void CsMapListMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_LIST_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapMapList.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	CsMapList::MAP_IT it = m_mapMapList.begin();
	for( ; it != m_mapMapList.end(); ++it )
	{
		//fwrite( it->second->GetInfo(), sizeof( CsMapList::sINFO ), 1, fp );
		CsMapList::sINFO* pInfo = it->second->GetInfo();

		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_dwMapID );
		CmUtil::CmBinSave::Save_Value( fp, pInfo->s_cMapName );
		CmUtil::CmBinSave::Save_Value( fp, pInfo->s_cMapPath );
		CmUtil::CmBinSave::Save_Value( fp, pInfo->s_cBGSound );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_nWidth );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_nHeight );
		CmUtil::CmBinSave::Save_Value( fp, pInfo->s_szMapDiscript );
		CmUtil::CmBinSave::Save_Value( fp, pInfo->s_szMapDiscript_Eng );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_dwResurrectionMapID );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_nMapRegionID );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_nFatigue_Type );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_nFatigue_DeBuff );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_nFatigue_StartTime );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_nFatigue_AddTime );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_nFatigue_AddPoint );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_nCamera_Max_Level );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_bXgConsumeType );
		CmUtil::CmBinSave::Save_Value( fp, &pInfo->s_bBattleTagUse );
	}
	fclose( fp );
}

bool CsMapListMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_LIST_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );

	CsMapList::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		//fread( &info, sizeof( CsMapList::sINFO ), 1, fp );
		CsMapList::sINFO info;
		CmUtil::CmBinRead::Read_Value( fp, &info.s_dwMapID );
		CmUtil::CmBinRead::Read_Value( fp, info.s_cMapName );
		CmUtil::CmBinRead::Read_Value( fp, info.s_cMapPath );
		CmUtil::CmBinRead::Read_Value( fp, info.s_cBGSound );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_nWidth );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_nHeight );
		CmUtil::CmBinRead::Read_Value( fp, info.s_szMapDiscript );
		CmUtil::CmBinRead::Read_Value( fp, info.s_szMapDiscript_Eng );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_dwResurrectionMapID );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_nMapRegionID );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_nFatigue_Type );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_nFatigue_DeBuff );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_nFatigue_StartTime );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_nFatigue_AddTime );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_nFatigue_AddPoint );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_nCamera_Max_Level );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_bXgConsumeType );
		CmUtil::CmBinRead::Read_Value( fp, &info.s_bBattleTagUse );

		CsMapList* pObject = csnew CsMapList;
		pObject->Init( &info );

		auto rb = m_mapMapList.insert( make_pair( info.s_dwMapID, pObject ) );
		if( !(rb.second) )
			assert_cs( false );
	}

	fclose( fp );

	return true;
}


void CsMapListMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_LIST_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );

	CsMapList::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		//_read( nHandle, &info, sizeof( CsMapList::sINFO ) );
		CsMapList::sINFO info;

		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_dwMapID );
		CmUtil::CmPackRead::Read_Value( nHandle, info.s_cMapName );
		CmUtil::CmPackRead::Read_Value( nHandle, info.s_cMapPath );
		CmUtil::CmPackRead::Read_Value( nHandle, info.s_cBGSound );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nWidth );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nHeight );
		CmUtil::CmPackRead::Read_Value( nHandle, info.s_szMapDiscript );
		CmUtil::CmPackRead::Read_Value( nHandle, info.s_szMapDiscript_Eng );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_dwResurrectionMapID );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nMapRegionID );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nFatigue_Type );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nFatigue_DeBuff );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nFatigue_StartTime );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nFatigue_AddTime );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nFatigue_AddPoint );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nCamera_Max_Level );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_bXgConsumeType );
		CmUtil::CmPackRead::Read_Value( nHandle, &info.s_bBattleTagUse );

		CsMapList* pObject = csnew CsMapList;
		pObject->Init( &info );

		auto rb = m_mapMapList.insert( make_pair( info.s_dwMapID, pObject ) );
		if( !(rb.second) )
			assert_cs( false );
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}