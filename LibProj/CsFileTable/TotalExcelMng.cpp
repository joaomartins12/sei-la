
#include "stdafx.h"
#include "TotalExcelMng.h"


void CsTotalExcelMng::Delete()
{
	std::map< DWORD, CsTotalExcel::MAP >::iterator it = m_Map.begin();
	std::map< DWORD, CsTotalExcel::MAP >::iterator itEnd = m_Map.end();
	for( ; it!=itEnd; ++it )
	{
		CsTotalExcel::MAP_IT itEle = it->second.begin();
		CsTotalExcel::MAP_IT itEleEnd = it->second.end();
		for( ; itEle!=itEleEnd; ++itEle )
		{
			itEle->second->Delete();
			delete itEle->second;
		}
		it->second.clear();
	}
	m_Map.clear();
}

bool CsTotalExcelMng::Init( char* cPath )
{
	m_Dummy.GetInfo()->s_nID = 1;
	_tcscpy_s( m_Dummy.GetInfo()->s_szKor, 512, _T( "번역없음(k)" ) );
	_tcscpy_s( m_Dummy.GetInfo()->s_szEng, 512, _T( "번역없음(e)" ) );	

#ifdef _DEBUG
	//_LoadBin();
	_LoadExcel();
#else
	_LoadExcel();
#endif

	return true;
}

char* CsTotalExcelMng::GetSheetName( int nSheetID )
{
	switch( nSheetID )
	{
	case SKILL:						return "Skill";
	case TALK_TIP:					return "Talk_Tip";
	case TALK_DIGIMON:				return "Talk_Digimon";
	case TALK_EVENT:				return "Talk_Event";
	case TALK_MESSAGE:				return "Talk_Message";
	case TACTICS_EXPLAIN:			return "Tactics_Explain";
	case ITEM_NAME:					return "ItemName";
	case ITEM_COMMENT:				return "ItemComment";
	case PARCINGWORD:				return "ParcingWord";
	case WORLDMAP:					return "WorldMap";
	case REGION:					return "Region";
	case NPC_PENNAME:				return "NpcPenName";
	case NPC:						return "Npc";
	case QUEST:						return "Quest";
	case HELPTEXT:					return "HelpText";
	case TAMER:						return "Tamer";
	default:						assert_cs( false );
	}

	return NULL;	
}

void CsTotalExcelMng::_LoadBin()
{
	FILE* fp = NULL;
	_tfopen_s( &fp, _T( "GameData\\English\\Total.bin" ), _T( "rb" ) );
	if( fp == NULL )
	{
		_LoadExcel();
		return;
	}

	int nSheetCount;
	fread( &nSheetCount, sizeof( int ), 1, fp );

	int nSheetID;
	int nDataCount;
	CsTotalExcel::sINFO info;
	for( int sheet=0; sheet<nSheetCount; ++sheet )
	{
		fread( &nSheetID, sizeof( int ), 1, fp );
		fread( &nDataCount, sizeof( int ), 1, fp );

		for( int data=0; data<nDataCount; ++data )
		{
			fread( &info, sizeof( CsTotalExcel::sINFO ), 1, fp );

			CsTotalExcel* pData = csnew CsTotalExcel;
			pData->Init( &info );

			assert_csm2( m_Map[ nSheetID ].find( info.s_nID ) == m_Map[ nSheetID ].end(), "토탈엑셀 중복\n쉬트ID : %s\nID = %d", nSheetID, info.s_nID );
			m_Map[ nSheetID ][ info.s_nID ] = pData;
		}	
	}

	fclose( fp );
}

void CsTotalExcelMng::_LoadExcel()
{
	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_DM_Total() );
	assert_cs( bLoad == true );

	//_LoadExcel( &excel, SKILL );
	//_LoadExcel( &excel, TALK_TIP );
	//_LoadExcel( &excel, TALK_DIGIMON );
	//_LoadExcel( &excel, TALK_EVENT );
	//_LoadExcel( &excel, TALK_MESSAGE );
	//_LoadExcel( &excel, TACTICS_EXPLAIN );
	//_LoadExcel( &excel, ITEM_NAME );
	//_LoadExcel( &excel, ITEM_COMMENT );
	//_LoadExcel( &excel, PARCINGWORD );
	//_LoadExcel( &excel, WORLDMAP );
	//_LoadExcel( &excel, REGION );
	//_LoadExcel( &excel, NPC );
	//_LoadExcel( &excel, QUEST );
	//_LoadExcel( &excel, HELPTEXT );
	//_LoadExcel( &excel, TAMER );
}

void CsTotalExcelMng::_LoadExcel( YExcel::BasicExcel* pExcel, eTYPE eType )
{
	YExcel::BasicExcelWorksheet* pSheet = pExcel->GetWorksheet( GetSheetName( eType ) );
	assert_cs( pSheet != NULL );

	// 데이터 입력
	size_t siTotalRow = pSheet->GetTotalRows(); 
	CsTotalExcel::sINFO info;
	TCHAR cTime[ 64 ];
	TCHAR c[ 3 ] = {0, };
	for( size_t row=1; row<siTotalRow; ++row )
	{
		// ID		
		info.s_nID = pSheet->Cell( row, 0 )->GetInteger();
		if( info.s_nID == 0 )
			continue;

		//  str
		_tcscpy_s( info.s_szKor, FT_TOTAL_EXCEL_KOR_LEN, pSheet->Cell( row, 1 )->GetWString() );
		_tcscpy_s( info.s_szEng, FT_TOTAL_EXCEL_ENG_LEN, pSheet->Cell( row, 2 )->GetWString() );
		_tcscpy_s( cTime, 64, pSheet->Cell( row, 3 )->GetWString() );

		c[ 0 ] = cTime[ 0 ];
		c[ 1 ] = cTime[ 1 ];
		info.s_CreateTime.s_nYear = _ttoi( c );
		c[ 0 ] = cTime[ 3 ];
		c[ 1 ] = cTime[ 4 ];
		info.s_CreateTime.s_nMonth = _ttoi( c );
		c[ 0 ] = cTime[ 6 ];
		c[ 1 ] = cTime[ 7 ];
		info.s_CreateTime.s_nDay = _ttoi( c );
		c[ 0 ] = cTime[ 13 ];
		c[ 1 ] = cTime[ 14 ];
		info.s_CreateTime.s_nHour = _ttoi( c );
		c[ 0 ] = cTime[ 16 ];
		c[ 1 ] = cTime[ 17 ];
		info.s_CreateTime.s_nMinute = _ttoi( c );

		CsTotalExcel* pData = csnew CsTotalExcel;
		pData->Init( &info );

		assert_csm2( m_Map[ eType ].find( info.s_nID ) == m_Map[ eType ].end(), "토탈엑셀 중복\n쉬트ID : %s\nID = %d", eType, info.s_nID );
		m_Map[ eType ][ info.s_nID ] = pData;
	}


	//char cBuffer[ CEXCLE_CELL_LEN ];
	//cExcelReader excel;
	//bool bSuccess = excel.Open( "GameData\\English\\Total.xls", GetSheetName( eType ) );
	//assert_cs( bSuccess == true );
	//// 1줄은 버린다.
	//bSuccess = excel.MoveFirst();
	//assert_cs( bSuccess == true );	

	//CsTotalExcel::sINFO info;
	//TCHAR cTime[ 64 ];
	//TCHAR c[ 3 ] = {0, };
	//while( excel.MoveNext() == true )
	//{
	//	// ID		
	//	info.s_nID = atoi( excel.GetData( 0, cBuffer ) );
	//	if( info.s_nID == 0 )
	//		continue;

	//	// str
	//	M2W( info.s_szKor, excel.GetData( 1, cBuffer ), FT_TOTAL_EXCEL_KOR_LEN );
	//	M2W( info.s_szEng, excel.GetData( 2, cBuffer ), FT_TOTAL_EXCEL_ENG_LEN );
	//	M2W( cTime, excel.GetData( 3, cBuffer ), 64 );


	//	c[ 0 ] = cTime[ 0 ];
	//	c[ 1 ] = cTime[ 1 ];
	//	info.s_CreateTime.s_nYear = _ttoi( c );
	//	c[ 0 ] = cTime[ 3 ];
	//	c[ 1 ] = cTime[ 4 ];
	//	info.s_CreateTime.s_nMonth = _ttoi( c );
	//	c[ 0 ] = cTime[ 6 ];
	//	c[ 1 ] = cTime[ 7 ];
	//	info.s_CreateTime.s_nDay = _ttoi( c );
	//	c[ 0 ] = cTime[ 13 ];
	//	c[ 1 ] = cTime[ 14 ];
	//	info.s_CreateTime.s_nHour = _ttoi( c );
	//	c[ 0 ] = cTime[ 16 ];
	//	c[ 1 ] = cTime[ 17 ];
	//	info.s_CreateTime.s_nMinute = _ttoi( c );

	//	CsTotalExcel* pData = csnew CsTotalExcel;
	//	pData->Init( &info );

	//	assert_csm2( m_Map[ eType ].find( info.s_nID ) == m_Map[ eType ].end(), "토탈엑셀 중복\n쉬트ID : %s\nID = %d", eType, info.s_nID );
	//	m_Map[ eType ][ info.s_nID ] = pData;
	//}
	//excel.Close();
}

bool CsTotalExcelMng::IsExist( eTYPE type, DWORD nID )
{
	if( m_Map.find( type ) == m_Map.end() )
	{
		return false;
	}

	return ( m_Map[ type ].find( nID ) != m_Map[ type ].end() );
}

bool CsTotalExcelMng::IsExistKor( eTYPE type, TCHAR* szKor )
{
	if( m_Map.find( type ) == m_Map.end() )
	{
		return false;
	}

	CsTotalExcel::MAP_IT it = m_Map[ type ].begin();
	CsTotalExcel::MAP_IT itEnd = m_Map[ type ].end();
	for( ; it!=itEnd; ++it )
	{
		if( _tcscmp( it->second->GetInfo()->s_szKor, szKor ) == 0 )
			return true;
	}

	return false;
}


bool CsTotalExcelMng::IsExistKor( eTYPE type, DWORD nID, TCHAR* szKor )
{
	if( m_Map.find( type ) == m_Map.end() )
	{
		return false;
	}

	CsTotalExcel::MAP_IT it = m_Map[ type ].begin();
	CsTotalExcel::MAP_IT itEnd = m_Map[ type ].end();
	for( ; it!=itEnd; ++it )
	{
		if( _tcscmp( it->second->GetInfo()->s_szKor, szKor ) == 0 )
		{
			if( nID == it->first )
				return true;
		}
	}

	return false;
}

void CsTotalExcelMng::New( eTYPE type, DWORD nID, TCHAR* szKor )
{
	assert_cs( IsExist( type, nID ) == false );
	CsTotalExcel* pNew = csnew CsTotalExcel;
	pNew->New( nID, szKor );
	assert_cs( m_Map[ type ].find( nID ) == m_Map[ type ].end() );
	m_Map[ type ][ nID ] = pNew;
}

CsTotalExcel* CsTotalExcelMng::GetData( eTYPE type, DWORD nKey )
{
	if( IsData( type, nKey ) )
		return m_Map[ type ].find( nKey )->second;

	return &m_Dummy;
}
