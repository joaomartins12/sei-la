
#include "stdafx.h"
#include "MapRegionMng.h"


void CsMapRegionMng::Delete()
{
	{
		CsMapRegionGroup::MAP_IT it = m_mapMapRegionGroup.begin();
		CsMapRegionGroup::MAP_IT itEnd = m_mapMapRegionGroup.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapMapRegionGroup.clear();
	}

	{
		CsMap_LimitEvoGroup::MAP_IT it = m_mapMapLimitEvoGroup.begin();
		CsMap_LimitEvoGroup::MAP_IT itEnd = m_mapMapLimitEvoGroup.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapMapLimitEvoGroup.clear();
	}	
}

bool CsMapRegionMng::Init( char* cPath, nsCsFileTable::eFILE_TYPE ft /*=nsCsFileTable::g_eFileType*/ )
{
	switch( ft )
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

CsMapRegionGroup* CsMapRegionMng::GetRegionGroup( DWORD dwMapID )
{
	if( IsRegionGroup( dwMapID ) == false )
	{
		CsMapRegionGroup* pGroup = csnew CsMapRegionGroup;
		pGroup->Init();
		m_mapMapRegionGroup[ dwMapID ] = pGroup;
	}
	return m_mapMapRegionGroup[ dwMapID ];
}

CsMap_LimitEvoGroup* CsMapRegionMng::GetLimitEvoGroup( DWORD dwMapID )
{
	if( IsLimitEvoGroup( dwMapID ) == false )
	{
		CsMap_LimitEvoGroup* pGroup = csnew CsMap_LimitEvoGroup;
		pGroup->Init();
		m_mapMapLimitEvoGroup[ dwMapID ] = pGroup;
	}
	return m_mapMapLimitEvoGroup[ dwMapID ];
}
CsMap_Buff_Region* CsMapRegionMng::GetBuffRegionGroup( DWORD dwMapID )
{
	if( IsBuffRegionGroup( dwMapID ) == false )
	{
		CsMap_Buff_Region* pGroup = csnew CsMap_Buff_Region;
		//pGroup->Init();
		m_BuffRegions[ dwMapID ] = pGroup;
	}
	return m_BuffRegions[ dwMapID ];
}



#define MES_COL_MAPID				0
#define MES_COL_DISCRIPT			1
#define MES_COL_DISCRIPT_ENG		2
#define MES_COL_CENTER_X			3
#define MES_COL_CENTER_Y			4
#define MES_COL_RADIUS				5
#define MES_COL_BGSOUND				6
#define MEL_COL_FATIGUE_TYPE		7
#define MEL_COL_FATIGUE_DEBUFF		8
#define MEL_COL_FATIGUE_STARTTIME	9
#define MEL_COL_FATIGUE_ADDTIME		10
#define MEL_COL_FATIGUE_POINT		11

void CsMapRegionMng::_LoadExcel()
{
	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;

	{
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP(), ME_REGION_SHEET_NAME );
		assert_cs( bSuccess == true );
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );

		if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_REGION_VERSION )
		{
			MessageBox( NULL, _T( "Map.xls Region Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
			return;
		}

		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		// 데이터 입력
		CsMapRegionGroup* pMapGroup = NULL;
		CsMapRegion::sINFO info;
		while( excel.MoveNext() == true )
		{
			info.s_dwMapID = atoi( excel.GetData( MES_COL_MAPID, cBuffer ) );
			if( info.s_dwMapID == 0 )
				continue;

			info.s_nCenterX = atoi( excel.GetData( MES_COL_CENTER_X, cBuffer ) );
			info.s_nCenterY = atoi( excel.GetData( MES_COL_CENTER_Y, cBuffer ) );
			info.s_nRadius = atoi( excel.GetData( MES_COL_RADIUS, cBuffer ) );

			strcpy_s( info.s_cBGSound, MAX_FILENAME, excel.GetData( MES_COL_BGSOUND, cBuffer ) );

			switch( nsCsFileTable::g_eLanguage )
			{
			case nsCsFileTable::KOREA_ORIGINAL:
			case nsCsFileTable::KOREA_TRANSLATION:
				M2W( info.s_szDiscript, excel.GetData( MES_COL_DISCRIPT, cBuffer ), FT_MAP_REGION_DISC_LEN );
				M2W( info.s_szDiscript_Eng, excel.GetData( MES_COL_DISCRIPT_ENG, cBuffer ), FT_MAP_REGION_DISC_LEN );
				break;
			case nsCsFileTable::ENGLISH:
			case nsCsFileTable::ENGLISH_A:
				M2W( info.s_szDiscript, excel.GetData( MES_COL_DISCRIPT_ENG, cBuffer ), FT_MAP_REGION_DISC_LEN );
				M2W( info.s_szDiscript_Eng, excel.GetData( MES_COL_DISCRIPT, cBuffer ), FT_MAP_REGION_DISC_LEN );
				break;
			case nsCsFileTable::THAILAND:
			case nsCsFileTable::TAIWAN:
			case nsCsFileTable::HONGKONG:
				//M2W( info.s_szDiscript, excel.GetData( MES_COL_DISCRIPT_ENG, cBuffer ), FT_MAP_REGION_DISC_LEN );
				//M2W( info.s_szDiscript_Eng, excel.GetData( MES_COL_DISCRIPT, cBuffer ), FT_MAP_REGION_DISC_LEN );
				wcscpy_s(info.s_szDiscript, FT_MAP_REGION_DISC_LEN, excel.GetData( MES_COL_DISCRIPT_ENG, wBuffer ));
				wcscpy_s(info.s_szDiscript_Eng, FT_MAP_REGION_DISC_LEN, excel.GetData( MES_COL_DISCRIPT, wBuffer ));
				break;
			default:
				assert_cs( false );
			}

			info.s_nFatigue_Type = atoi( excel.GetData( MEL_COL_FATIGUE_TYPE, cBuffer ) );
			info.s_nFatigue_DeBuff = atoi( excel.GetData( MEL_COL_FATIGUE_DEBUFF, cBuffer ) );
			info.s_nFatigue_StartTime = atoi( excel.GetData( MEL_COL_FATIGUE_STARTTIME, cBuffer ) );
			info.s_nFatigue_AddTime = atoi( excel.GetData( MEL_COL_FATIGUE_ADDTIME, cBuffer ) );
			info.s_nFatigue_AddPoint = atoi( excel.GetData( MEL_COL_FATIGUE_POINT, cBuffer ) );

			// 그룹이 존재 하지 않는다면 새로 생성
			GetRegionGroup( info.s_dwMapID )->AddRegion( &info );
		}
		excel.Close();
	}
	
	{
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP(), ME_LIMITEVO_SHEET_NAME );
		assert_cs( bSuccess == true );
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );

		if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_LIMITEVO_VERSION )
		{
			MessageBox( NULL, _T( "Map.xls Region Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
			return;
		}

		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		// 데이터 입력
		CsMap_LimitEvoGroup* pMapGroup = NULL;
		CsMap_LimitEvo::sINFO info;
		while( excel.MoveNext() == true )
		{
			info.s_dwMapID = atoi( excel.GetData( 0, cBuffer ) );
			if( info.s_dwMapID == 0 )
				continue;

			info.s_nCenterX = atoi( excel.GetData( 1, cBuffer ) );
			info.s_nCenterY = atoi( excel.GetData( 2, cBuffer ) );
			info.s_nRadius = atoi( excel.GetData( 3, cBuffer ) );

			// 그룹이 존재 하지 않는다면 새로 생성
			GetLimitEvoGroup( info.s_dwMapID )->AddRegion( &info );
		}
		excel.Close();
	}
	{
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP(), ME_BUFF_REGION_SHEET_NAME );
		//assert_cs( bSuccess == true );
		if(bSuccess == false)
			return;

		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );

		if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_BUFFREGION_VERSION )
		{
			MessageBox( NULL, _T( "Map.xls Region Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
			return;
		}

		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		// 데이터 입력
		CsMap_Buff_Region* pMapGroup = NULL;
		CsMap_BuffRegion::sINFO info;
		int i = 0;
		while( excel.MoveNext() == true )
		{
			info.regionIdx =  atoi( excel.GetData( i++, cBuffer ) );
			info.mapidx = atoi( excel.GetData( i++, cBuffer ) );
			if( info.mapidx == 0 )
				continue;
			info.centerx = atoi( excel.GetData( i++, cBuffer ) );
			info.centery  = atoi( excel.GetData( i++, cBuffer ) );
			info.radius  = atoi( excel.GetData( i++, cBuffer ) );
			info.act  = atoi( excel.GetData( i++, cBuffer ) );
			info.buffidx  = atoi( excel.GetData( i++, cBuffer ) );
			info.applytype  = atoi( excel.GetData( i++, cBuffer ) );
			info.requirement_1  = atoi( excel.GetData( i++, cBuffer ) );
			info.requirement_2  = atoi( excel.GetData( i++, cBuffer ) );
			info.time  = atoi( excel.GetData( i++, cBuffer ) );
			info.layerno  = atoi( excel.GetData( i++, cBuffer ) );
			info.objuniqid =  atoi( excel.GetData( i++, cBuffer ) );

			// 그룹이 존재 하지 않는다면 새로 생성
			GetBuffRegionGroup( info.mapidx )->AddRegion( &info );
			i = 0;
		}
		excel.Close();
	}
}

void CsMapRegionMng::Save()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP() );
	assert_cs( bLoad == true );

	{
		YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_REGION_SHEET_NAME );
		assert_cs( pSheet != NULL );

		pSheet->DeleteAllRows();

		pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_REGION_VERSION );
		for( int i=1; i<=MEL_COL_FATIGUE_POINT; ++i )
		{
			pSheet->Cell( 0, i )->SetInteger( 1 );
		}

		pSheet->Cell( 1, MES_COL_MAPID )->SetWString( _T( "map idx" ) );
		pSheet->Cell( 1, MES_COL_DISCRIPT )->SetWString( _T( "discript" ) );
		pSheet->Cell( 1, MES_COL_DISCRIPT_ENG )->SetWString( _T( "discript" ) );
		pSheet->Cell( 1, MES_COL_CENTER_X )->SetWString( _T( "center x" ) );
		pSheet->Cell( 1, MES_COL_CENTER_Y )->SetWString( _T( "center y" ) );
		pSheet->Cell( 1, MES_COL_RADIUS )->SetWString( _T( "radius" ) );
		pSheet->Cell( 0, MES_COL_BGSOUND )->SetWString( _T( "bg_sound" ) );
		pSheet->Cell( 1, MES_COL_BGSOUND )->SetWString( _T( "bg_sound" ) );

		pSheet->Cell( 1, MEL_COL_FATIGUE_TYPE )->SetWString( _T( "FG Type" ) );
		pSheet->Cell( 1, MEL_COL_FATIGUE_DEBUFF	)->SetWString( _T( "FG D-Buff" ) );
		pSheet->Cell( 1, MEL_COL_FATIGUE_STARTTIME )->SetWString( _T( "FG StartTime" ) );
		pSheet->Cell( 1, MEL_COL_FATIGUE_ADDTIME )->SetWString( _T( "FG AddTime" ) );
		pSheet->Cell( 1, MEL_COL_FATIGUE_POINT )->SetWString( _T( "FG Point" ) );

		size_t siInputRow = 2;
		CsMapRegionGroup::MAP_IT itGroup = m_mapMapRegionGroup.begin();
		CsMapRegion::sINFO* pMapInfo = NULL;
		for( ; itGroup!=m_mapMapRegionGroup.end(); ++itGroup )
		{
			CsMapRegion::LIST* pListRegion = itGroup->second->GetRegionList();
			CsMapRegion::LIST_IT itRegion = pListRegion->begin();
			for( ; itRegion != pListRegion->end(); ++itRegion )
			{
				pMapInfo = ( *itRegion )->GetInfo();
				pSheet->Cell( siInputRow, MES_COL_MAPID )->SetInteger(  pMapInfo->s_dwMapID );

				switch( nsCsFileTable::g_eLanguage )
				{
				case nsCsFileTable::KOREA_ORIGINAL:
				case nsCsFileTable::KOREA_TRANSLATION:
					pSheet->Cell( siInputRow, MES_COL_DISCRIPT )->SetWString(  pMapInfo->s_szDiscript );
					pSheet->Cell( siInputRow, MES_COL_DISCRIPT_ENG )->SetWString(  pMapInfo->s_szDiscript_Eng );
					break;
				case nsCsFileTable::ENGLISH:
				case nsCsFileTable::ENGLISH_A:
				case nsCsFileTable::THAILAND:
				case nsCsFileTable::TAIWAN:
				case nsCsFileTable::HONGKONG:
					pSheet->Cell( siInputRow, MES_COL_DISCRIPT )->SetWString(  pMapInfo->s_szDiscript_Eng );
					pSheet->Cell( siInputRow, MES_COL_DISCRIPT_ENG )->SetWString(  pMapInfo->s_szDiscript );
					break;
				default:
					assert_cs( false );
				}

				pSheet->Cell( siInputRow, MES_COL_CENTER_X )->SetInteger( pMapInfo->s_nCenterX );
				pSheet->Cell( siInputRow, MES_COL_CENTER_Y )->SetInteger( pMapInfo->s_nCenterY );
				pSheet->Cell( siInputRow, MES_COL_RADIUS )->SetInteger( pMapInfo->s_nRadius );
				pSheet->Cell( siInputRow, MES_COL_BGSOUND )->SetString( pMapInfo->s_cBGSound );

				pSheet->Cell( siInputRow, MEL_COL_FATIGUE_TYPE )->SetInteger( pMapInfo->s_nFatigue_Type );
				pSheet->Cell( siInputRow, MEL_COL_FATIGUE_DEBUFF )->SetInteger( pMapInfo->s_nFatigue_DeBuff );
				pSheet->Cell( siInputRow, MEL_COL_FATIGUE_STARTTIME )->SetInteger( pMapInfo->s_nFatigue_StartTime );
				pSheet->Cell( siInputRow, MEL_COL_FATIGUE_ADDTIME )->SetInteger( pMapInfo->s_nFatigue_AddTime );
				pSheet->Cell( siInputRow, MEL_COL_FATIGUE_POINT )->SetInteger( pMapInfo->s_nFatigue_AddPoint );

				++siInputRow;
			}		
		}
	}

	{
		YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_LIMITEVO_SHEET_NAME );
		assert_cs( pSheet != NULL );

		pSheet->DeleteAllRows();

		pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_LIMITEVO_VERSION );
		for( int i=1; i<3; ++i )
		{
			pSheet->Cell( 0, i )->SetInteger( 1 );
		}

		pSheet->Cell( 1, 0 )->SetWString( _T( "map idx" ) );
		pSheet->Cell( 1, 1 )->SetWString( _T( "center x" ) );
		pSheet->Cell( 1, 2 )->SetWString( _T( "center y" ) );
		pSheet->Cell( 1, 3 )->SetWString( _T( "radius" ) );

		size_t siInputRow = 2;
		CsMap_LimitEvoGroup::MAP_IT itGroup = m_mapMapLimitEvoGroup.begin();
		CsMap_LimitEvo::sINFO* pMapInfo = NULL;
		for( ; itGroup!=m_mapMapLimitEvoGroup.end(); ++itGroup )
		{
			CsMap_LimitEvo::LIST* pListRegion = itGroup->second->GetList();
			CsMap_LimitEvo::LIST_IT itRegion = pListRegion->begin();
			for( ; itRegion != pListRegion->end(); ++itRegion )
			{
				pMapInfo = ( *itRegion )->GetInfo();
				pSheet->Cell( siInputRow, 0 )->SetInteger(  pMapInfo->s_dwMapID );
				pSheet->Cell( siInputRow, 1 )->SetInteger( pMapInfo->s_nCenterX );
				pSheet->Cell( siInputRow, 2 )->SetInteger( pMapInfo->s_nCenterY );
				pSheet->Cell( siInputRow, 3 )->SetInteger( pMapInfo->s_nRadius );
				++siInputRow;
			}		
		}
	}
	{
		YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_BUFF_REGION_SHEET_NAME );
		assert_cs( pSheet != NULL );
		pSheet->DeleteAllRows();

		pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_BUFFREGION_VERSION );
		for( int i=1; i<ME_BUFF_REGION_SHEET_INDEX; ++i )
		{
			pSheet->Cell( 0, i )->SetInteger( 1 );
		}
		int StartIdx =0;
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "idx" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "map idx" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "center x" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "center y" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "radius" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "act" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "buff idx" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "Apply type" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "Requirements_1" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "Requirements_2" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "Time" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "Layer no" ) );
		pSheet->Cell( 1, StartIdx++ )->SetWString( _T( "ObjectID" ) );
	

		size_t siInputRow = 2;
		StartIdx = 0;
		//m_BuffRegions.insert(CsMap_Buff_Region::MAP::value_type(1400, new CsMap_BuffRegion::sINFO()));
		CsMap_Buff_Region::MAP_IT itGroup = m_BuffRegions.begin();
		CsMap_BuffRegion::sINFO* pMapInfo = NULL;
		for( ; itGroup!=m_BuffRegions.end(); ++itGroup )
		{
			CsMap_BuffRegion::LIST* pListRegion = itGroup->second->GetList();
			CsMap_BuffRegion::LIST_IT itRegion = pListRegion->begin();
			for( ; itRegion != pListRegion->end(); ++itRegion )
			{
				pMapInfo = ( *itRegion )->GetInfo();
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->regionIdx);
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger(  pMapInfo->mapidx);
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->centerx );
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->centery );
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->radius );
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->act );
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->buffidx );
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->applytype );
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->requirement_1 );
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->requirement_2 );
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->time );
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->layerno );
				pSheet->Cell( siInputRow, StartIdx++ )->SetInteger( pMapInfo->objuniqid );

				StartIdx =0;
				++siInputRow;
			}		
		}
	}
	excel.SaveAs( nsCsFileTable::GetPath_MAP() );
}

#define MAP_REGION_BIN			"MapRegion.bin"


void CsMapRegionMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_REGION_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	{
		int nCount = (int)m_mapMapRegionGroup.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsMapRegionGroup::MAP_IT itGroup = m_mapMapRegionGroup.begin();
		CsMapRegionGroup::MAP_IT itGroupEnd = m_mapMapRegionGroup.end();
		for( ; itGroup!=itGroupEnd; ++itGroup )
		{
			CsMapRegion::LIST* pList = itGroup->second->GetRegionList();

			int nCountRegion = (int)pList->size();
			fwrite( &nCountRegion, sizeof( int ), 1, fp );

			CsMapRegion::LIST_IT it = pList->begin();
			CsMapRegion::LIST_IT itEnd = pList->end();
			for( ; it!=itEnd; ++it )
			{
				fwrite( (*it)->GetInfo(), sizeof( CsMapRegion::sINFO ), 1, fp );
			}
		}
	}

	{
		int nCount = (int)m_mapMapLimitEvoGroup.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsMap_LimitEvoGroup::MAP_IT itGroup = m_mapMapLimitEvoGroup.begin();
		CsMap_LimitEvoGroup::MAP_IT itGroupEnd = m_mapMapLimitEvoGroup.end();
		for( ; itGroup!=itGroupEnd; ++itGroup )
		{
			CsMap_LimitEvo::LIST* pList = itGroup->second->GetList();

			int nCountRegion = (int)pList->size();
			fwrite( &nCountRegion, sizeof( int ), 1, fp );

			CsMap_LimitEvo::LIST_IT it = pList->begin();
			CsMap_LimitEvo::LIST_IT itEnd = pList->end();
			for( ; it!=itEnd; ++it )
			{
				fwrite( (*it)->GetInfo(), sizeof( CsMap_LimitEvo::sINFO ), 1, fp );
			}
		}
	}
	{
		int nCount = (int)m_BuffRegions.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsMap_Buff_Region::MAP_IT itGroup = m_BuffRegions.begin();
		CsMap_Buff_Region::MAP_IT itGroupEnd = m_BuffRegions.end();
		for( ; itGroup!=itGroupEnd; ++itGroup )
		{
			CsMap_BuffRegion::LIST* pList = itGroup->second->GetList();

			int nCountRegion = (int)pList->size();
			fwrite( &nCountRegion, sizeof( int ), 1, fp );

			CsMap_BuffRegion::LIST_IT it = pList->begin();
			CsMap_BuffRegion::LIST_IT itEnd = pList->end();
			for( ; it!=itEnd; ++it )
			{
				fwrite( (*it)->GetInfo(), sizeof( CsMap_BuffRegion::sINFO ), 1, fp );
			}
		}
	}


	fclose( fp );
}

bool CsMapRegionMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_REGION_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );
		CsMapRegion::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			int nCountRegion;
			fread( &nCountRegion, sizeof( int ), 1, fp );
			for( int r=0; r<nCountRegion; ++r )
			{
				fread( &info, sizeof( CsMapRegion::sINFO ), 1, fp );
				GetRegionGroup( info.s_dwMapID )->AddRegion( &info );
			}
		}
	}

	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );
		CsMap_LimitEvo::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			int nCountRegion;
			fread( &nCountRegion, sizeof( int ), 1, fp );
			for( int r=0; r<nCountRegion; ++r )
			{
				fread( &info, sizeof( CsMap_LimitEvo::sINFO ), 1, fp );
				GetLimitEvoGroup( info.s_dwMapID )->AddRegion( &info );
			}
		}
	}

	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );
		CsMap_BuffRegion::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			int nCountRegion;
			fread( &nCountRegion, sizeof( int ), 1, fp );
			for( int r=0; r<nCountRegion; ++r )
			{
				fread( &info, sizeof( CsMap_BuffRegion::sINFO ), 1, fp );
				GetBuffRegionGroup( info.mapidx )->AddRegion( &info );
			}
		}
	}

	fclose( fp );

	return true;
}

void CsMapRegionMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MAP_REGION_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );
	{
		int nCount;
		_read( nHandle, &nCount, sizeof( int ) );

		CsMapRegion::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			int nCountRegion;
			_read( nHandle, &nCountRegion, sizeof( int ) );
			for( int r=0; r<nCountRegion; ++r )
			{
				_read( nHandle, &info, sizeof( CsMapRegion::sINFO ) );
				GetRegionGroup( info.s_dwMapID )->AddRegion( &info );
			}
		}
	}

	{
		int nCount;
		_read( nHandle, &nCount, sizeof( int ) );

		CsMap_LimitEvo::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			int nCountRegion;
			_read( nHandle, &nCountRegion, sizeof( int ) );
			for( int r=0; r<nCountRegion; ++r )
			{
				_read( nHandle, &info, sizeof( CsMap_LimitEvo::sINFO ) );
				GetLimitEvoGroup( info.s_dwMapID )->AddRegion( &info );
			}
		}
	}
	{
		int nCount;
		_read( nHandle, &nCount, sizeof( int ) );

		CsMap_BuffRegion::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			int nCountRegion;
			_read( nHandle, &nCountRegion, sizeof( int ) );
			for( int r=0; r<nCountRegion; ++r )
			{
				_read( nHandle, &info, sizeof( CsMap_BuffRegion::sINFO ) );
				GetBuffRegionGroup( info.mapidx )->AddRegion( &info );
			}
		}
	}
	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}


