
#include "stdafx.h"
#include "WeatherMng.h"


void CsWeatherMng::Delete()
{
	CsWeatherGroup::MAP_IT it = m_mapGroup.begin();
	CsWeatherGroup::MAP_IT itEnd = m_mapGroup.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapGroup.clear();
}

bool CsWeatherMng::Init( char* cPath, nsCsFileTable::eFILE_TYPE ft /*=nsCsFileTable::g_eFileType*/ )
{
	m_CheckTime.SetDeltaTime( 1500 );
	m_CheckTime.Reset();

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

CsWeatherGroup* CsWeatherMng::GetGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
	{
		CsWeatherGroup* pGroup = csnew CsWeatherGroup;
		pGroup->Init();
		m_mapGroup[ dwMapID ] = pGroup;
	}
	return m_mapGroup[ dwMapID ];
}

void CsWeatherMng::_LoadExcel()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP(), ME_WEATHER_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_WEATHER_VERSION )
	{
		MessageBox( NULL, _T( "Map.xls Weather Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );


	// 데이터 입력
	CsWeather* pWeather = NULL;
	CsWeather::sINFO info;
	while( excel.MoveNext() == true )
	{
		info.s_dwMapID = atoi( excel.GetData( 0, cBuffer ) );
		if( info.s_dwMapID == 0 )
			continue;

		info.s_nTotalLoopMin = atoi( excel.GetData( 1, cBuffer ) );
		info.s_nCenterX = atoi( excel.GetData( 2, cBuffer ) );
		info.s_nCenterY = atoi( excel.GetData( 3, cBuffer ) );
		info.s_nRadius = atoi( excel.GetData( 4, cBuffer ) );
		info.s_nElementCount = atoi( excel.GetData( 5, cBuffer ) );

		// 그룹이 존재 하지 않는다면 새로 생성
		int nCol = 6;
		pWeather = GetGroup( info.s_dwMapID )->Add( &info );
		CsWeather::sELEMENT ele;
		for( int i=0; i<info.s_nElementCount; ++i )
		{
			ele.s_Type = atoi( excel.GetData( nCol, cBuffer ) );
			++nCol;
			ele.s_nLevel = atoi( excel.GetData( nCol, cBuffer ) );
			++nCol;
			ele.s_nTime_Min = atoi( excel.GetData( nCol, cBuffer ) );
			++nCol;
			ele.s_nFatigue = atoi( excel.GetData( nCol, cBuffer ) );
			++nCol;

			pWeather->AddElement( &ele );
		}
	}
	excel.Close();
}

void CsWeatherMng::Save()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP() );
	assert_cs( bLoad == true );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_WEATHER_SHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->DeleteAllRows();

	pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_WEATHER_VERSION );

	pSheet->Cell( 1, 0 )->SetWString( _T( "map idx" ) );
	pSheet->Cell( 1, 1 )->SetWString( _T( "Total Loop Min" ) );
	pSheet->Cell( 1, 2 )->SetWString( _T( "center x" ) );
	pSheet->Cell( 1, 3 )->SetWString( _T( "center y" ) );
	pSheet->Cell( 1, 4 )->SetWString( _T( "radius" ) );
	pSheet->Cell( 1, 5 )->SetWString( _T( "Count" ) );
	pSheet->Cell( 1, 6 )->SetWString( _T( "타입" ) );
	pSheet->Cell( 1, 7 )->SetWString( _T( "레벨" ) );
	pSheet->Cell( 1, 8 )->SetWString( _T( "시간" ) );
	pSheet->Cell( 1, 9 )->SetWString( _T( "피로도" ) );
	pSheet->Cell( 1, 10 )->SetWString( _T( "이후타입부터 반복" ) );

	size_t siInputRow = 2;

	int nMaxCol = 0;
	CsWeatherGroup::MAP_IT itGroup = m_mapGroup.begin();
	CsWeatherGroup::MAP_IT itGroupEnd = m_mapGroup.end();
	CsWeather* pWeather  = NULL;
	for( ; itGroup!=itGroupEnd; ++itGroup )
	{
		CsWeather::LIST* pListWeather = itGroup->second->GetList();
		CsWeather::LIST_IT itWeather = pListWeather->begin();
		CsWeather::LIST_IT itWeatherEnd = pListWeather->end();
		for( ; itWeather != itWeatherEnd; ++itWeather )
		{
			pWeather = *itWeather;
			pSheet->Cell( siInputRow, 0 )->SetInteger( pWeather->GetInfo()->s_dwMapID );
			pSheet->Cell( siInputRow, 1 )->SetInteger( pWeather->GetInfo()->s_nTotalLoopMin );
			pSheet->Cell( siInputRow, 2 )->SetInteger( pWeather->GetInfo()->s_nCenterX );
			pSheet->Cell( siInputRow, 3 )->SetInteger( pWeather->GetInfo()->s_nCenterY );
			pSheet->Cell( siInputRow, 4 )->SetInteger( pWeather->GetInfo()->s_nRadius );
			pSheet->Cell( siInputRow, 5 )->SetInteger( pWeather->GetInfo()->s_nElementCount );

			CsWeather::LISTELE* pListElement = pWeather->GetElementList();
			CsWeather::LISTELE_IT itElement = pListElement->begin();
			CsWeather::LISTELE_IT itElementEnd = pListElement->end();
			int nCol = 6;
			for( ; itElement != itElementEnd; ++itElement )
			{
				CsWeather::sELEMENT* pElement = *itElement;
				pSheet->Cell( siInputRow, nCol )->SetInteger( pElement->s_Type );
				++nCol;
				pSheet->Cell( siInputRow, nCol )->SetInteger( pElement->s_nLevel );
				++nCol;
				pSheet->Cell( siInputRow, nCol )->SetInteger( pElement->s_nTime_Min );
				++nCol;
				pSheet->Cell( siInputRow, nCol )->SetInteger( pElement->s_nFatigue );
				++nCol;
			}

			if( nMaxCol < nCol )
			{
				nMaxCol = nCol;
			}
			++siInputRow;
		}		
	}

	for( int i=1; i<=nMaxCol; ++i )
	{
		pSheet->Cell( 0, i )->SetInteger( 1 );
	}
	excel.SaveAs( nsCsFileTable::GetPath_MAP() );
}


#define WEATHER_BIN		"Weather.bin"

void CsWeatherMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, WEATHER_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapGroup.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsWeatherGroup::MAP_IT itGroup = m_mapGroup.begin();
	CsWeatherGroup::MAP_IT itGroupEnd = m_mapGroup.end();
	for( ; itGroup!=itGroupEnd; ++itGroup )
	{
		CsWeather::LIST* pListWeather = itGroup->second->GetList();
		nCount = (int)pListWeather->size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsWeather::LIST_IT itWeather = pListWeather->begin();
		CsWeather::LIST_IT itWeatherEnd = pListWeather->end();
		for( ; itWeather != itWeatherEnd; ++itWeather )
		{
			fwrite( (*itWeather)->GetInfo(), sizeof( CsWeather::sINFO ), 1, fp );
			CsWeather::LISTELE* pListElement = (*itWeather)->GetElementList();
			CsWeather::LISTELE_IT itElement = pListElement->begin();
			CsWeather::LISTELE_IT itElementEnd = pListElement->end();
			for( ; itElement!=itElementEnd; ++itElement )
			{
				fwrite( (*itElement), sizeof( CsWeather::sELEMENT ), 1, fp );
			}
		}
	}
	fclose( fp );
}

bool CsWeatherMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, WEATHER_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount, nWeatherCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsWeather::sINFO info;
	CsWeather::sELEMENT ele;
	CsWeather* pWeather;
	for( int i=0; i<nCount; ++i )
	{
		fread( &nWeatherCount, sizeof( int ), 1, fp );
		for( int w=0; w<nWeatherCount; ++w )
		{
			fread( &info, sizeof( CsWeather::sINFO ), 1, fp );
			pWeather = GetGroup( info.s_dwMapID )->Add( &info );
			for( int e=0; e<info.s_nElementCount; ++e )
			{
				fread( &ele, sizeof( CsWeather::sELEMENT ), 1, fp );
				pWeather->AddElement( &ele );
			}
		}
	}

	fclose( fp );

	return true;
}

void CsWeatherMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, WEATHER_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount, nWeatherCount;
	_read( nHandle, &nCount, sizeof( int ) );	

	CsWeather::sINFO info;
	CsWeather::sELEMENT ele;
	CsWeather* pWeather;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &nWeatherCount, sizeof( int ) );
		for( int w=0; w<nWeatherCount; ++w )
		{
			_read( nHandle, &info, sizeof( CsWeather::sINFO ) );
			pWeather = GetGroup( info.s_dwMapID )->Add( &info );
			for( int e=0; e<info.s_nElementCount; ++e )
			{
				_read( nHandle, &ele, sizeof( CsWeather::sELEMENT ) );
				pWeather->AddElement( &ele );
			}
		}
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

