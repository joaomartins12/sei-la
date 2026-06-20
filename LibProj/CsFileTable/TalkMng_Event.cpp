


#include "stdafx.h"
#include "TalkMng.h"


//======================================================================================================
//
//		이벤트
//
//======================================================================================================

void CsTalkMng::_Delete_Event()
{
	CsTalk_Event::MAP_IT it = m_mapEvent.begin();
	CsTalk_Event::MAP_IT itEnd = m_mapEvent.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapEvent.clear();	
}

CsTalk_Event* CsTalkMng::GetTalk_Event( DWORD dwID )
{
	assert_csm1( m_mapEvent.find( dwID ) != m_mapEvent.end(), _T( "이벤트 대사가 존재하지 않습니다. id = %d" ), dwID );
	if( m_mapEvent.find( dwID ) == m_mapEvent.end() )
		return NULL;

	return m_mapEvent[ dwID ];
}

void CsTalkMng::_LoadExcel_Event()
{
	_Delete_Event();

#define TALK_SHEET_NAME				"DM_EVentTalk"

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Talk(), TALK_SHEET_NAME );
	assert_cs( bSuccess == true );
	// 3줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsTalk_Event::sINFO info;
	DWORD dwID;

	while( excel.MoveNext() == true )
	{
		dwID = atoi( excel.GetData( 0, cBuffer ) );
		if( dwID == 0 )
			continue;

		info.s_dwTalkNum = dwID;

		excel.GetData( 1, cBuffer );
		if( strlen( cBuffer ) < 3 )
			continue;

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( info.s_szText, excel.GetData( 2, cBuffer ), FT_EVENTTALK_LEN );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			{
//				M2W( info.s_szText, excel.GetData( 4, cBuffer ), FT_EVENTTALK_LEN );
				//테이블 제한 글자 수 체크 수정 chu8820
				TCHAR szBuffer[ 2048 ];
				M2W( szBuffer, excel.GetData( 4, cBuffer ), 2048 );
				if( (int)_tcslen( szBuffer ) >= FT_EVENTTALK_LEN - 1 )
					CsMessageBox( MB_OK, L"DM_Event : %d, 의 TitleName 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d", info.s_dwTalkNum , (int)_tcslen( szBuffer ), FT_EVENTTALK_LEN );
				else
					_tcscpy_s( info.s_szText, FT_EVENTTALK_LEN, szBuffer  );
			}
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			//M2W( info.s_szText, excel.GetData( 4, cBuffer ), FT_EVENTTALK_LEN );
			wcscpy_s(info.s_szText, FT_EVENTTALK_LEN, excel.GetData( 4, wBuffer ));
			break;
		default:
			assert_cs( false );
		}		

		assert_cs( m_mapEvent.find( dwID ) == m_mapEvent.end() );
		CsTalk_Event* pDTalk = csnew CsTalk_Event;
		pDTalk->Init( &info );
		m_mapEvent[ dwID ] = pDTalk;
	}
	excel.Close();
}


void CsTalkMng::_SaveBin_Event( FILE* fp )
{
	int nCount = (int)m_mapEvent.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	DWORD dwID;
	CsTalk_Event::MAP_IT it = m_mapEvent.begin();
	CsTalk_Event::MAP_IT itEnd = m_mapEvent.end();
	for( ; it!=itEnd; ++it )
	{
		dwID = it->first;
		fwrite( &dwID, sizeof( DWORD ), 1, fp );
		fwrite( it->second->GetInfo(), sizeof( CsTalk_Event::sINFO ), 1, fp );
	}
}

void CsTalkMng::_LoadBin_Event( FILE* fp )
{
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	DWORD dwID;
	CsTalk_Event::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &dwID, sizeof( DWORD ), 1, fp );
		fread( &info, sizeof( CsTalk_Event::sINFO ), 1, fp );

		CsTalk_Event* pObject = csnew CsTalk_Event;
		pObject->Init( &info );
		assert_cs( m_mapEvent.find( dwID ) == m_mapEvent.end() );
		m_mapEvent[ dwID ] = pObject;
	}
}

void CsTalkMng::_LoadFilePack_Event( int nHandle )
{
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	DWORD dwID;
	CsTalk_Event::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &dwID, sizeof( DWORD ) );
		_read( nHandle, &info, sizeof( CsTalk_Event::sINFO ) );

		CsTalk_Event* pObject = csnew CsTalk_Event;
		pObject->Init( &info );
		assert_cs( m_mapEvent.find( dwID ) == m_mapEvent.end() );
		m_mapEvent[ dwID ] = pObject;
	}
}
