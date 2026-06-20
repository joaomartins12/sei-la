


#include "stdafx.h"
#include "TalkMng.h"


//======================================================================================================
//
// 디지몬 대화
//
//======================================================================================================

void CsTalkMng::_Delete_Digimon()
{
	CsTalk_Digimon::MAP_IT it = m_mapDigimon.begin();
	CsTalk_Digimon::MAP_IT itEnd = m_mapDigimon.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapDigimon.clear();	
}


CsTalk_Digimon* CsTalkMng::GetTalk_Digimon( DWORD dwID )
{
	assert_csm1( m_mapDigimon.find( dwID ) != m_mapDigimon.end(), _T( "id = %d" ), dwID );
	if( m_mapDigimon.find( dwID ) == m_mapDigimon.end() )
		return NULL;
	return m_mapDigimon[ dwID ];
}

bool CsTalkMng::IsExist_Digimon( DWORD dwID )
{
	return ( m_mapDigimon.find( dwID ) != m_mapDigimon.end() );
}


void CsTalkMng::_LoadExcel_Digimon()
{
#define TALK_SHEET_NAME				"DM_DigimonTalk"

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

	CsTalk_Digimon::sINFO info;
	DWORD dwID;

	while( excel.MoveNext() == true )
	{
		dwID = atoi( excel.GetData( 0, cBuffer ) );
		if( dwID == 0 )
			continue;

		info.s_nType = atoi( excel.GetData( 2, cBuffer ) );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( info.s_szText, excel.GetData( 5, cBuffer ), FT_TALK_DIGIMON_LEN );
			M2W( info.s_szList, excel.GetData( 7, cBuffer ), FT_TALK_DIGIMON_LEN );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			{
//				M2W( info.s_szText, excel.GetData( 6, cBuffer ), FT_TALK_DIGIMON_LEN );
//				M2W( info.s_szList, excel.GetData( 8, cBuffer ), FT_TALK_DIGIMON_LEN );

				//테이블 제한 글자 수 체크 수정 chu8820
				TCHAR szBuffer[ 2048 ];
				M2W( szBuffer, excel.GetData( 6, cBuffer ), 2048 );
				if( (int)_tcslen( szBuffer ) >= FT_TALK_DIGIMON_LEN - 1 )
					CsMessageBox( MB_OK, L"DM_DigimonTalk : %d, 의 공통대사의 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d", dwID , (int)_tcslen( szBuffer ), FT_TALK_DIGIMON_LEN );
				else
					_tcscpy_s( info.s_szText, FT_TALK_DIGIMON_LEN, szBuffer  );

				M2W( szBuffer, excel.GetData( 8, cBuffer ), 2048 );
				if( (int)_tcslen( szBuffer ) >= FT_TALK_DIGIMON_LEN - 1 )
					CsMessageBox( MB_OK, L"DM_DigimonTalk : %d, 의 출력상황의 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d", dwID , (int)_tcslen( szBuffer ), FT_TALK_DIGIMON_LEN );
				else
					_tcscpy_s( info.s_szList, FT_TALK_DIGIMON_LEN, szBuffer  );
			}
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			//M2W( info.s_szText, excel.GetData( 6, cBuffer ), FT_TALK_DIGIMON_LEN );
			//M2W( info.s_szList, excel.GetData( 8, cBuffer ), FT_TALK_DIGIMON_LEN );
			wcscpy_s(info.s_szText, FT_TALK_DIGIMON_LEN, excel.GetData( 6, wBuffer ));
			wcscpy_s(info.s_szList, FT_TALK_DIGIMON_LEN, excel.GetData( 8, wBuffer ));
			break;
		default:
			assert_cs( false );
		}		

		DWORD dwParam = 0;
		if( atoi( excel.GetData( 10, cBuffer ) ) == 1 )
		{
			//QUEST
			dwParam |= CsTalk_Digimon::TP_QUEST;			
		}		

		if( atoi( excel.GetData( 11, cBuffer ) ) == 1 )
		{
			//NPC
			dwParam |= CsTalk_Digimon::TP_NPC;
		}

		if( atoi( excel.GetData( 12, cBuffer ) ) == 1 )
		{
			//DIGIMON
			dwParam |= CsTalk_Digimon::TP_DIGIMON;			
		}

		if( atoi( excel.GetData( 13, cBuffer ) ) == 1 )
		{
			//SKILL
			dwParam |= CsTalk_Digimon::TP_SKILL;			
		}		

		info.s_dwParam = dwParam;
		assert_cs( m_mapDigimon.find( dwID ) == m_mapDigimon.end() );
		CsTalk_Digimon* pDTalk = csnew CsTalk_Digimon;
		pDTalk->Init( &info );
		m_mapDigimon[ dwID ] = pDTalk;
	}
	excel.Close();
}


void CsTalkMng::_SaveBin_Digimon( FILE* fp )
{
	int nCount = (int)m_mapDigimon.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	DWORD dwID;
	CsTalk_Digimon::MAP_IT it = m_mapDigimon.begin();
	CsTalk_Digimon::MAP_IT itEnd = m_mapDigimon.end();
	for( ; it!=itEnd; ++it )
	{
		dwID = it->first;
		fwrite( &dwID, sizeof( DWORD ), 1, fp );
		fwrite( it->second->GetInfo(), sizeof( CsTalk_Digimon::sINFO ), 1, fp );
	}
}

void CsTalkMng::_LoadBin_Digimon( FILE* fp )
{
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	DWORD dwID;
	CsTalk_Digimon::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &dwID, sizeof( DWORD ), 1, fp );
		fread( &info, sizeof( CsTalk_Digimon::sINFO ), 1, fp );

		CsTalk_Digimon* pObject = csnew CsTalk_Digimon;
		pObject->Init( &info );
		assert_cs( m_mapDigimon.find( dwID ) == m_mapDigimon.end() );
		m_mapDigimon[ dwID ] = pObject;
	}
}

void CsTalkMng::_LoadFilePack_Digimon( int nHandle )
{
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	DWORD dwID;
	CsTalk_Digimon::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &dwID, sizeof( DWORD ) );
		_read( nHandle, &info, sizeof( CsTalk_Digimon::sINFO ) );

		CsTalk_Digimon* pObject = csnew CsTalk_Digimon;
		pObject->Init( &info );
		assert_cs( m_mapDigimon.find( dwID ) == m_mapDigimon.end() );
		m_mapDigimon[ dwID ] = pObject;
	}
}
