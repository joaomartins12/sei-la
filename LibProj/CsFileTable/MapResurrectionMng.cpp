
#include "stdafx.h"
#include "MapResurrectionMng.h"


void CsMapResurrectionMng::Delete()
{
	CsMapResurrectionGroup::MAP_IT it = m_mapMapGroup.begin();
	for( ; it!=m_mapMapGroup.end(); ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapMapGroup.clear();
}


void CsMapResurrectionMng::Init( nsCsFileTable::eFILE_TYPE ft /*=nsCsFileTable::g_eFileType*/ )
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
	default:
		assert_cs( false );
	}
}

CsMapResurrectionGroup* CsMapResurrectionMng::GetGroup( DWORD dwMapID )
{
	if( IsGroup( dwMapID ) == false )
	{
		CsMapResurrectionGroup* pGroup = csnew CsMapResurrectionGroup;
		pGroup->Init();
		m_mapMapGroup[ dwMapID ] = pGroup;
	}
	return m_mapMapGroup[ dwMapID ];
}


#define MES_COL_MAPID			0
#define MES_COL_CENTER_X		1
#define MES_COL_CENTER_Y		2
#define MES_COL_RADIUS			3

void CsMapResurrectionMng::_LoadExcel()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_MAP(), ME_RESURRECTION_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != ME_RESURRECTION_VERSION )
	{
		MessageBox( NULL, _T( "Map.xls Resurrection Sheet 의 버젼이 틀립니다.\n송명수에게 문의해주세요." ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	// 데이터 입력
	CsMapResurrectionGroup* pMapGroup = NULL;
	CsMapResurrection::sINFO info;
	while( excel.MoveNext() == true )
	{
		info.s_dwMapID = atoi( excel.GetData( MES_COL_MAPID, cBuffer ) );
		if( info.s_dwMapID == 0 )
			continue;

		info.s_nCenterX = atoi( excel.GetData( MES_COL_CENTER_X, cBuffer ) );
		info.s_nCenterY = atoi( excel.GetData( MES_COL_CENTER_Y, cBuffer ) );
		info.s_nRadius = atoi( excel.GetData( MES_COL_RADIUS, cBuffer ) );

		// 그룹이 존재 하지 않는다면 새로 생성
		GetGroup( info.s_dwMapID )->AddResurrection( &info );
	}
	excel.Close();
}

void CsMapResurrectionMng::Save()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_MAP() );
	assert_cs( bLoad == true );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( ME_RESURRECTION_SHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->DeleteAllRows();

	pSheet->Cell( 0, ME_VERSION_COL )->SetInteger( ME_RESURRECTION_VERSION );
	for( int i=1; i<=MES_COL_RADIUS; ++i )
	{
		pSheet->Cell( 0, i )->SetInteger( 1 );
	}

	pSheet->Cell( 1, MES_COL_MAPID )->SetWString( _T( "map idx" ) );
	pSheet->Cell( 1, MES_COL_CENTER_X )->SetWString( _T( "center x" ) );
	pSheet->Cell( 1, MES_COL_CENTER_Y )->SetWString( _T( "center y" ) );
	pSheet->Cell( 1, MES_COL_RADIUS )->SetWString( _T( "radius" ) );

	size_t siInputRow = 2;
	CsMapResurrectionGroup::MAP_IT itGroup = m_mapMapGroup.begin();
	CsMapResurrection::sINFO* pMapInfo = NULL;
	for( ; itGroup!=m_mapMapGroup.end(); ++itGroup )
	{
		CsMapResurrection::LIST* pList = itGroup->second->GetResurrectionList();
		CsMapResurrection::LIST_IT it = pList->begin();
		for( ; it != pList->end(); ++it )
		{
			pMapInfo = ( *it )->GetInfo();
			pSheet->Cell( siInputRow, MES_COL_MAPID )->SetInteger(  pMapInfo->s_dwMapID );
			pSheet->Cell( siInputRow, MES_COL_CENTER_X )->SetInteger( pMapInfo->s_nCenterX );
			pSheet->Cell( siInputRow, MES_COL_CENTER_Y )->SetInteger( pMapInfo->s_nCenterY );
			pSheet->Cell( siInputRow, MES_COL_RADIUS )->SetInteger( pMapInfo->s_nRadius );
			++siInputRow;
		}		
	}

	excel.SaveAs( nsCsFileTable::GetPath_MAP() );
}

