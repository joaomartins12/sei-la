


#include "stdafx.h"
#include "TalkMng.h"


//======================================================================================================
//
// 디지몬 대사 팁 
//
//======================================================================================================

void CsTalkMng::_Delete_Tip()
{
	CsTalk_Tip::MAP_IT it = m_mapTip.begin();
	CsTalk_Tip::MAP_IT itEnd = m_mapTip.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapTip.clear();	
}

CsTalk_Tip* CsTalkMng::GetRandom_Tip()
{	
	int nCount = (int)m_mapTip.size();
	if( nCount == 0 )
		return NULL;
	return m_mapTip[ rand()%nCount ];
}

void CsTalkMng::_LoadExcel_Tip()
{
#define TALK_SHEET_NAME				"DM_DigimonTip"

	int nTipIndex = 0;

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Talk(), TALK_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	TCHAR sz[ FT_TALK_TIP_LEN ];
	CsTalk_Tip::sINFO info;
	while( excel.MoveNext() == true )
	{
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( sz, excel.GetData( 0, cBuffer ), FT_TALK_TIP_LEN );
			_tcscpy_s( info.s_szTip, FT_TALK_TIP_LEN, __CsStringChange::NewLineCheck( sz ) );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			{
//			M2W( sz, excel.GetData( 1, cBuffer ), FT_TALK_TIP_LEN );
//			_tcscpy_s( info.s_szTip, FT_TALK_TIP_LEN, __CsStringChange::NewLineCheck( sz ) );

				//테이블 제한 글자 수 체크 수정 chu8820
				TCHAR szBuffer[ 2048 ];
				M2W( szBuffer, excel.GetData( 1, cBuffer ), 2048 );
				if( (int)_tcslen( szBuffer ) >= FT_TALK_TIP_LEN - 1 )
				{
					int nRow = (DWORD)m_mapTip.size() + 1;
					CsMessageBox( MB_OK, L"DM_Tip의 Row : %d, 의 Message 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d", nRow , (int)_tcslen( szBuffer ), FT_TALK_TIP_LEN );
				}
				else
					_tcscpy_s( info.s_szTip, FT_TALK_TIP_LEN, __CsStringChange::NewLineCheck( szBuffer )  );
			}
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			//M2W( sz, excel.GetData( 1, cBuffer ), FT_TALK_TIP_LEN );
			wcscpy_s(sz, FT_TALK_TIP_LEN, excel.GetData( 1, wBuffer ));
			_tcscpy_s( info.s_szTip, FT_TALK_TIP_LEN, __CsStringChange::NewLineCheck( sz ) );
			break;
		default:
			assert_cs( false );
		}		

		CsTalk_Tip* pTip = csnew CsTalk_Tip;
		pTip->Init( &info );
		m_mapTip[ (DWORD)m_mapTip.size() ] = pTip;
	}
	excel.Close();
}


void CsTalkMng::_SaveBin_Tip( FILE* fp )
{
	int nCount = (int)m_mapTip.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	DWORD dwID;
	CsTalk_Tip::MAP_IT it = m_mapTip.begin();
	CsTalk_Tip::MAP_IT itEnd = m_mapTip.end();
	for( ; it!=itEnd; ++it )
	{
		dwID = it->first;
		fwrite( &dwID, sizeof( DWORD ), 1, fp );
		fwrite( it->second->GetInfo(), sizeof( CsTalk_Tip::sINFO ), 1, fp );
	}
}

void CsTalkMng::_LoadBin_Tip( FILE* fp )
{
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	DWORD dwID;
	CsTalk_Tip::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &dwID, sizeof( DWORD ), 1, fp );
		fread( &info, sizeof( CsTalk_Tip::sINFO ), 1, fp );

		CsTalk_Tip* pObject = csnew CsTalk_Tip;
		pObject->Init( &info );
		assert_cs( m_mapTip.find( dwID ) == m_mapTip.end() );
		m_mapTip[ dwID ] = pObject;
	}
}

void CsTalkMng::_LoadFilePack_Tip( int nHandle )
{
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	DWORD dwID;
	CsTalk_Tip::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &dwID, sizeof( DWORD ) );
		_read( nHandle, &info, sizeof( CsTalk_Tip::sINFO ) );

		CsTalk_Tip* pObject = csnew CsTalk_Tip;
		pObject->Init( &info );
		assert_cs( m_mapTip.find( dwID ) == m_mapTip.end() );
		m_mapTip[ dwID ] = pObject;
	}
}

//======================================================================================================
//
// 로딩 팁 
//
//======================================================================================================

void CsTalkMng::_Delete_LoadingTip()
{
	CsLoading_Tip::MAP_IT it = m_mapLodingTip.begin();
	CsLoading_Tip::MAP_IT itEnd = m_mapLodingTip.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapLodingTip.clear();	
}

CsLoading_Tip* CsTalkMng::GetRandom_LoadingTip( int nLevel )
{	
	int nCount = (int)m_mapLodingTip.size();
	if( nCount == 0 )	
		return NULL;

	while( 1 )
	{
		int nRand = rand()%nCount;
		CsLoading_Tip::MAP::iterator it = m_mapLodingTip.find( nRand );
		if( it == m_mapLodingTip.end() )
			continue;

		SAFE_POINTER_CON( it->second );

		CsLoading_Tip::sINFO * pInfo = it->second->GetInfo();
		SAFE_POINTER_CON( pInfo );

		if( pInfo->s_nLevel < nLevel )
			return it->second;

// #ifdef CRASHDUMP_0305			//chu8820 - 로딩 시 도움말툴팁 랜덤 로딩 실패 ( C006 )
// 		if( m_mapLodingTip[ nRand ] == NULL )
// 		{
// 			DUMPLOG( " C006 - 1, %d, %d ", nRand, nCount );
// 			return NULL;
// 		}
// 		else if( m_mapLodingTip[ nRand ]->GetInfo() == NULL )
// 		{
// 			DUMPLOG( " C006 - 2, %d, %d ", nRand, nCount );
// 			return NULL;
// 		}
// #endif
// 		if( m_mapLodingTip[ nRand ]->GetInfo()->s_nLevel < nLevel )		
// 			return m_mapLodingTip[ nRand ];
	}	

	assert_cs( false );
	return NULL;
}

void CsTalkMng::_LoadExcel_LoadingTip()
{
#define TALK_SHEET_NAME2				"DM_Tip"

	int nTipIndex = 0;

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Talk(), TALK_SHEET_NAME2 );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	CsLoading_Tip::sINFO info;
	while( excel.MoveNext() == true )
	{
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( info.s_szLoadingTip, excel.GetData( 0, cBuffer ), FT_TALK_TIP_LEN );
			info.s_nLevel = atoi( excel.GetData( 2, cBuffer ) );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info.s_szLoadingTip, excel.GetData( 1, cBuffer ), FT_TALK_TIP_LEN );
			info.s_nLevel = atoi( excel.GetData( 2, cBuffer ) );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			//M2W( info.s_szLoadingTip, excel.GetData( 1, wBuffer ), FT_TALK_TIP_LEN );
			wcscpy_s(info.s_szLoadingTip, FT_TALK_TIP_LEN , excel.GetData( 1, wBuffer ));
			info.s_nLevel = atoi( excel.GetData( 2, cBuffer ) );
			break;
		default:
			assert_cs( false );
		}		

		CsLoading_Tip* pTip = csnew CsLoading_Tip;
		pTip->Init( &info );
		m_mapLodingTip[ (DWORD)m_mapLodingTip.size() ] = pTip;
	}
	excel.Close();
}


void CsTalkMng::_SaveBin_LoadingTip( FILE* fp )
{
	int nCount = (int)m_mapLodingTip.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	DWORD dwID;
	CsLoading_Tip::MAP_IT it = m_mapLodingTip.begin();
	CsLoading_Tip::MAP_IT itEnd = m_mapLodingTip.end();
	for( ; it!=itEnd; ++it )
	{
		dwID = it->first;
		fwrite( &dwID, sizeof( DWORD ), 1, fp );
		fwrite( it->second->GetInfo(), sizeof( CsLoading_Tip::sINFO ), 1, fp );
	}
}

void CsTalkMng::_LoadBin_LoadingTip( FILE* fp )
{
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	DWORD dwID;
	CsLoading_Tip::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &dwID, sizeof( DWORD ), 1, fp );
		fread( &info, sizeof( CsLoading_Tip::sINFO ), 1, fp );

		CsLoading_Tip* pObject = csnew CsLoading_Tip;
		pObject->Init( &info );
		assert_cs( m_mapLodingTip.find( dwID ) == m_mapLodingTip.end() );
		m_mapLodingTip[ dwID ] = pObject;
	}
}

void CsTalkMng::_LoadFilePack_LoadingTip( int nHandle )
{
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	DWORD dwID;
	CsLoading_Tip::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &dwID, sizeof( DWORD ) );
		_read( nHandle, &info, sizeof( CsLoading_Tip::sINFO ) );

		CsLoading_Tip* pObject = csnew CsLoading_Tip;
		pObject->Init( &info );
		assert_cs( m_mapLodingTip.find( dwID ) == m_mapLodingTip.end() );
		m_mapLodingTip[ dwID ] = pObject;
	}
}

