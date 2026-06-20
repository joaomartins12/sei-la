
#include "stdafx.h"
#include "MapStartMng.h"


void CsMapStartMng::Delete()
{
	CsMapStartGroup::MAP_IT it = m_mapMapStartGroup.begin();
	CsMapStartGroup::MAP_IT itEnd = m_mapMapStartGroup.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapMapStartGroup.clear();
}

void CsMapStartMng::Init( nsCsFileTable::eFILE_TYPE ft /*=nsCsFileTable::g_eFileType*/ )
{
	switch( ft )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel();	
		break;
	case nsCsFileTable::FT_BIN:
		// 맵툴에서만 읽는다.
		_LoadExcel();
		break;
	case nsCsFileTable::FT_FILEPACK:
		break;
	default:
		assert_cs( false );
	}
}

CsMapStartGroup* CsMapStartMng::GetGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
	{
		CsMapStartGroup* pGroup = csnew CsMapStartGroup;
		pGroup->Init();
		m_mapMapStartGroup[ dwMapID ] = pGroup;
	}
	return m_mapMapStartGroup[ dwMapID ];
}


#define MES_COL_MAPID				0
#define MES_COL_CENTER_X			1
#define MES_COL_CENTER_Y			2
#define MES_COL_RADIUS				3
#define MES_COL_TYPE				4

void CsMapStartMng::_LoadExcel()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP(), ME_START_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_START_VERSION )
	{
		MessageBox( NULL, _T( "Map.xls Start Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	// 데이터 입력
	CsMapStartGroup* pMapGroup = NULL;
	CsMapStart::sINFO info;
	while( excel.MoveNext() == true )
	{
		info.s_dwMapID = atoi( excel.GetData( MES_COL_MAPID, cBuffer ) );
		if( info.s_dwMapID == 0 )
			continue;

		info.s_nCenterX = atoi( excel.GetData( MES_COL_CENTER_X, cBuffer ) );
		info.s_nCenterY = atoi( excel.GetData( MES_COL_CENTER_Y, cBuffer ) );
		info.s_nRadius = atoi( excel.GetData( MES_COL_RADIUS, cBuffer ) );
		info.s_nType = atoi( excel.GetData( MES_COL_TYPE, cBuffer ) );

		// 그룹이 존재 하지 않는다면 새로 생성
		GetGroup( info.s_dwMapID )->AddStart( &info );
	}

	excel.Close();
}

void CsMapStartMng::Save()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP() );
	assert_cs( bLoad == true );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_START_SHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->DeleteAllRows();

	pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_START_VERSION );
	for( int i=1; i<=MES_COL_TYPE; ++i )
	{
		pSheet->Cell( 0, i )->SetInteger( 1 );
	}

	pSheet->Cell( 1, MES_COL_MAPID )->SetWString( _T( "map idx" ) );
	pSheet->Cell( 1, MES_COL_CENTER_X )->SetWString( _T( "center x" ) );
	pSheet->Cell( 1, MES_COL_CENTER_Y )->SetWString( _T( "center y" ) );
	pSheet->Cell( 1, MES_COL_RADIUS )->SetWString( _T( "radius" ) );
	pSheet->Cell( 1, MES_COL_TYPE )->SetWString( _T( "type" ) );

	size_t siInputRow = 2;
	CsMapStartGroup::MAP_IT itGroup = m_mapMapStartGroup.begin();
	CsMapStart::sINFO* pMapInfo = NULL;
	for( ; itGroup!=m_mapMapStartGroup.end(); ++itGroup )
	{
		CsMapStart::LIST* pListStart = itGroup->second->GetStartList();
		CsMapStart::LIST_IT itStart = pListStart->begin();
		for( ; itStart != pListStart->end(); ++itStart )
		{
			pMapInfo = ( *itStart )->GetInfo();
			pSheet->Cell( siInputRow, MES_COL_MAPID )->SetInteger(  pMapInfo->s_dwMapID );
			pSheet->Cell( siInputRow, MES_COL_CENTER_X )->SetInteger( pMapInfo->s_nCenterX );
			pSheet->Cell( siInputRow, MES_COL_CENTER_Y )->SetInteger( pMapInfo->s_nCenterY );
			pSheet->Cell( siInputRow, MES_COL_RADIUS )->SetInteger( pMapInfo->s_nRadius );
			pSheet->Cell( siInputRow, MES_COL_TYPE )->SetInteger( pMapInfo->s_nType );
			++siInputRow;
		}		
	}

	excel.SaveAs( nsCsFileTable::GetPath_MAP() );
}

