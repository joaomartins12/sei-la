#include "stdafx.h"
#include "SceneDataMng.h"

CsSceneDataMng*	g_pSceneDataMng = NULL;

CsSceneDataMng::CsSceneDataMng(void)
{

}

CsSceneDataMng::~CsSceneDataMng(void)
{
	Delete();
}

void CsSceneDataMng::Delete()
{
	MAP_IT it = m_mapSceneData.begin();
	MAP_IT itEnd = m_mapSceneData.end();

	for( ;it != itEnd; it++ )
	{
		it->second->Delete();
		SAFE_DELETE( it->second );		
	}
	m_mapSceneData.clear();

	MAP_TAR_IT it2 = m_mapTargeData.begin();
	MAP_TAR_IT it2End = m_mapTargeData.end();
	for( ;it2 != it2End; it2++ )
	{
		SAFE_DELETE( it2->second );		
	}
	m_mapTargeData.clear();
}

bool CsSceneDataMng::Init( char* cPath )
{
	CsQuest::MAP_IT it = nsCsFileTable::g_pQuestMng->GetQuestMap()->begin();
	CsQuest::MAP_IT itEnd = nsCsFileTable::g_pQuestMng->GetQuestMap()->begin();
	for( ; it != itEnd; it++ )
	{
		memset( it->second->m_nEvent, 0 , sizeof( int ) * CsQuest::EVENT_MAX_COUNT );
	}

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

void CsSceneDataMng::_LoadExcel()
{
	_LoadExcelTargetData();
	_LoadExcelSceneData();
	_LoadExcelSceneInfo();   // 무조건 _LoadExcelSceneData() 다음 로드
}

void CsSceneDataMng::_LoadExcelSceneInfo()
{
	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// Tamer
	{		
#define SCENE_INFO_SHEET_NAME			"scene_info"

		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Scene(), SCENE_INFO_SHEET_NAME );
		assert_cs( bSuccess == true );
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );			
				
		int nScene;
		int nQuest;
		while( excel.MoveNext() == true )
		{
			int Col=0;
			if( atoi( excel.GetData( Col++, cBuffer ) ) == 0 )
				continue;

			nScene = atoi( excel.GetData( Col++, cBuffer ) );
			Col += 1;
			
			//M2W( GetSceneData( nScene )->GetInfo()->s_szTitle, excel.GetData( Col++, cBuffer ), 64 );
			switch( nsCsFileTable::g_eLanguage )
			{
			case nsCsFileTable::KOREA_ORIGINAL:
			case nsCsFileTable::KOREA_TRANSLATION:
			case nsCsFileTable::ENGLISH:
			case nsCsFileTable::ENGLISH_A:
				//assert_csm1( strlen( excel.GetData( Col, cBuffer ) ) < 32, L"이벤트씬 대상의 이름을 32자미만으로 해주세요 (대상UID : %d)", pInfo->s_dwUID );
				M2W( GetSceneData( nScene )->GetInfo()->s_szTitle, excel.GetData( Col++, cBuffer ), 64 );
				break;
			case nsCsFileTable::THAILAND:
			case nsCsFileTable::TAIWAN:
			case nsCsFileTable::HONGKONG:
				//assert_csm1( wcslen( excel.GetData( Col, wBuffer ) ) < 32, L"이벤트씬 대상의 이름을 32자미만으로 해주세요 (대상UID : %d)", pInfo->s_dwUID );
				wcscpy_s(GetSceneData( nScene )->GetInfo()->s_szTitle, 64, excel.GetData( Col++, wBuffer ));
				break;
			}

			// 맵,지역 조건이벤트 로드
			GetSceneData( nScene )->GetInfo()->s_nMap = atoi( excel.GetData( Col++, cBuffer ) );			
			GetSceneData( nScene )->GetInfo()->s_nResion = atoi( excel.GetData( Col++, cBuffer ) );	
			
			// 퀘스트 NPC에게 받을때 이벤트 발동 조건
			nQuest = atoi( excel.GetData( Col++, cBuffer ) );
			if( nsCsFileTable::g_pQuestMng->IsQuest( nQuest ) == true )
				nsCsFileTable::g_pQuestMng->GetQuest( nQuest )->m_nEvent[ CsQuest::EVENT_QUEST_START ] = nScene;

			// 퀘스트 NPC에게 보상 받을때 이벤트 발동 조건
			nQuest = atoi( excel.GetData( Col++, cBuffer ) );
			if( nsCsFileTable::g_pQuestMng->IsQuest( nQuest ) == true )
				nsCsFileTable::g_pQuestMng->GetQuest( nQuest )->m_nEvent[ CsQuest::EVENT_QUEST_REQUITE ] = nScene;

			// 이벤트 스킵 여부
			GetSceneData( nScene )->GetInfo()->s_bSkip = ( atoi( excel.GetData( Col++, cBuffer ) ) == 1 );	
			strcpy_s( GetSceneData( nScene )->GetInfo()->s_szMusic, 64, excel.GetData( Col++, cBuffer ) );	

		}
		excel.Close();
	}
}

void CsSceneDataMng::_LoadExcelTargetData()
{
#define IL_COL_START				0

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// Tamer
	{		
#define TARGET_SHEET_NAME			"target"

		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Scene(), TARGET_SHEET_NAME );
		assert_cs( bSuccess == true );
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );			

		while( excel.MoveNext() == true )
		{
			int Col=0;
			if( atoi( excel.GetData( IL_COL_START, cBuffer ) ) == 0 )
				continue;

			sTargetData* pInfo = csnew sTargetData;
			
			pInfo->s_dwUID = atoi( excel.GetData( Col++, cBuffer ) );
			pInfo->s_dwModelID = atoi( excel.GetData( Col++, cBuffer ) );

			switch( nsCsFileTable::g_eLanguage )
			{
				case nsCsFileTable::KOREA_ORIGINAL:
				case nsCsFileTable::KOREA_TRANSLATION:
				case nsCsFileTable::ENGLISH:
				case nsCsFileTable::ENGLISH_A:
					assert_csm1( strlen( excel.GetData( Col, cBuffer ) ) < 32, L"이벤트씬 대상의 이름을 32자미만으로 해주세요 (대상UID : %d)", pInfo->s_dwUID );
					M2W( pInfo->s_szName, excel.GetData( Col++, cBuffer ), 32 );
					break;
				case nsCsFileTable::THAILAND:
				case nsCsFileTable::TAIWAN:
				case nsCsFileTable::HONGKONG:
					assert_csm1( wcslen( excel.GetData( Col, wBuffer ) ) < 32, L"이벤트씬 대상의 이름을 32자미만으로 해주세요 (대상UID : %d)", pInfo->s_dwUID );
					wcscpy_s(pInfo->s_szName, 32, excel.GetData( Col++, wBuffer ));
					break;
			}

			pInfo->s_fDist = (float)atof( excel.GetData( Col++, cBuffer ) );
			pInfo->s_fHeight = (float)atof( excel.GetData( Col++, cBuffer ) );			

			m_mapTargeData[ pInfo->s_dwUID ] = pInfo;
		}
		excel.Close();
	}
}

void CsSceneDataMng::_LoadExcelSceneData()
{
	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// Tamer
	{		
#define SCEND_SHEET_NAME			"scene_data"

		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Scene(), SCEND_SHEET_NAME );
		assert_cs( bSuccess == true );
		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );			
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );			

		int nCurRow = 3;
		int nSceneNum = 0;
		CSceneData*	pInfo = NULL;
		while( excel.MoveNext() == true )
		{
			int Col=1;			
			if( atoi( excel.GetData( IL_COL_START, cBuffer ) ) == 0 )
				continue;	

			if( nSceneNum != atoi( excel.GetData( IL_COL_START, cBuffer ) ) )
			{
				if( pInfo != NULL )
					m_mapSceneData[ nSceneNum ] = pInfo;

				nSceneNum = atoi( excel.GetData( IL_COL_START, cBuffer ) );
				pInfo = csnew CSceneData;
				pInfo->GetInfo()->s_dwSceneID = nSceneNum;				
			}			

			CSceneData::sData* pData = csnew CSceneData::sData;
			Col += 1;
			pData->s_dwMainTarget = atoi( excel.GetData( Col++, cBuffer ) );
			Col += 1;
			pData->s_dwTarget[ 0 ] = atoi( excel.GetData( Col++, cBuffer ) );
			Col += 1;
			pData->s_dwTarget[ 3 ] = atoi( excel.GetData( Col++, cBuffer ) );
			Col += 1;
			pData->s_dwTarget[ 1 ] = atoi( excel.GetData( Col++, cBuffer ) );
			Col += 1;
			pData->s_dwTarget[ 4 ] = atoi( excel.GetData( Col++, cBuffer ) );
			Col += 1;
			pData->s_dwTarget[ 2 ] = atoi( excel.GetData( Col++, cBuffer ) );
			Col += 1;

			pData->s_dwAni = atoi( excel.GetData( Col++, cBuffer ) );			 
			Col += 1;

			strcpy_s( pData->s_szEffect, 64, excel.GetData( Col++, cBuffer ) );
			strcpy_s( pData->s_szSound, 64, excel.GetData( Col++, cBuffer ) );

			switch( nsCsFileTable::g_eLanguage )
			{
				case nsCsFileTable::KOREA_ORIGINAL:
				case nsCsFileTable::KOREA_TRANSLATION:
					M2W( pData->s_szTalk, excel.GetData( Col++, cBuffer ), sizeof(pData->s_szTalk) );
					break;
				case nsCsFileTable::ENGLISH:
				case nsCsFileTable::ENGLISH_A:
					{
//						M2W( pData->s_szTalk, excel.GetData( Col++, cBuffer ), 256 );
						//테이블 제한 글자 수 체크 수정 chu8820
						TCHAR szBuffer[ 2048 ];
						M2W( szBuffer, excel.GetData( Col++, cBuffer ), sizeof(szBuffer) );
						if( (int)_tcslen( szBuffer ) >= sizeof(pData->s_szTalk)-1 )
							CsMessageBox( MB_OK, L"Scene_data의  Row : %d, 의 대사 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d", nCurRow , (int)_tcslen( szBuffer ), sizeof(pData->s_szTalk) );
						else
							_tcscpy_s( pData->s_szTalk, sizeof(pData->s_szTalk), szBuffer  );
					}
					break;
				case nsCsFileTable::THAILAND:
				case nsCsFileTable::TAIWAN:
				case nsCsFileTable::HONGKONG:
					wcscpy_s(pData->s_szTalk, sizeof(pData->s_szTalk), excel.GetData( Col++, wBuffer ));
					break;
			}
			
			pInfo->InsertListData( pData );
			nCurRow++;
		}
		
		m_mapSceneData[ nSceneNum ] = pInfo;

		excel.Close();
	}
}

#define SCENE_BIN		"Scene.bin"
void CsSceneDataMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, SCENE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapSceneData.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsSceneDataMng::MAP_IT it = m_mapSceneData.begin();
	CsSceneDataMng::MAP_IT itEnd = m_mapSceneData.end();
	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second->GetInfo(), sizeof( CSceneData::sSceneInfo ), 1, fp );
		
		std::list< CSceneData::sData* >::iterator it2 = it->second->GetList()->begin();
		std::list< CSceneData::sData* >::iterator it2End = it->second->GetList()->end();

		nCount = (int)it->second->GetList()->size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		for( ; it2 != it2End; ++it2 )
		{
			fwrite( *it2, sizeof( CSceneData::sData ), 1, fp );
		}		
	}

	nCount = (int)m_mapTargeData.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsSceneDataMng::MAP_TAR_IT it2 = m_mapTargeData.begin();
	CsSceneDataMng::MAP_TAR_IT it2End = m_mapTargeData.end();
	for( ; it2!=it2End; ++it2 )
	{
		fwrite( it2->second, sizeof( CsSceneDataMng::sTargetData ), 1, fp );
	}

	fclose( fp );
}

bool CsSceneDataMng::_LoadBin( char* cPath )
{		
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, SCENE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	int	nCount2;
	fread( &nCount, sizeof( int ), 1, fp );
	
	for( int i=0; i<nCount; ++i )
	{
		CSceneData* pData = csnew CSceneData;		
		
		fread( pData->GetInfo(), sizeof( CSceneData::sSceneInfo ), 1, fp );
		
		fread( &nCount2, sizeof( int ), 1, fp );
		for( int j = 0; j < nCount2; j++ )
		{
			CSceneData::sData* pDataInfo = csnew CSceneData::sData;		
			fread( pDataInfo, sizeof( CSceneData::sData ), 1, fp );			
			pData->InsertListData( pDataInfo );
		}

		assert_cs( m_mapSceneData.find( pData->GetInfo()->s_dwSceneID ) == m_mapSceneData.end() );
		m_mapSceneData[ pData->GetInfo()->s_dwSceneID ] = pData;
	}

	fread( &nCount, sizeof( int ), 1, fp );

	for( int i=0; i<nCount; ++i )
	{
		CsSceneDataMng::sTargetData* pData = csnew CsSceneDataMng::sTargetData;		
		fread( pData, sizeof( CsSceneDataMng::sTargetData ), 1, fp );

		assert_cs( m_mapTargeData.find( pData->s_dwUID ) == m_mapTargeData.end() );
		m_mapTargeData[ pData->s_dwUID ] = pData;
	}

	fclose( fp );

	return true;
}

void CsSceneDataMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, SCENE_BIN );	

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	int	nCount2;
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		CSceneData* pData = csnew CSceneData;		

		_read( nHandle, pData->GetInfo(), sizeof( CSceneData::sSceneInfo ) );		

		_read( nHandle, &nCount2, sizeof( int ) );
		for( int j = 0; j < nCount2; j++ )
		{
			CSceneData::sData* pDataInfo = csnew CSceneData::sData;		
			_read( nHandle, pDataInfo, sizeof( CSceneData::sData ) );			
			pData->InsertListData( pDataInfo );
		}

		assert_cs( m_mapSceneData.find( pData->GetInfo()->s_dwSceneID ) == m_mapSceneData.end() );
		m_mapSceneData[ pData->GetInfo()->s_dwSceneID ] = pData;
	}

	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		CsSceneDataMng::sTargetData* pData = csnew CsSceneDataMng::sTargetData;		
		_read( nHandle, pData, sizeof( CsSceneDataMng::sTargetData ) );		

		assert_cs( m_mapTargeData.find( pData->s_dwUID ) == m_mapTargeData.end() );
		m_mapTargeData[ pData->s_dwUID ] = pData;
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );	
}

void CsSceneDataMng::ReLoad()
{
	Delete();
	_LoadExcel();
}

CsSceneDataMng::sTargetData* CsSceneDataMng::GetSceneTarget( DWORD dwID )
{	
	assert_cs( IsTarget( dwID ) == true );

	MAP_TAR_IT it = m_mapTargeData.find( dwID );
	if( it == m_mapTargeData.end() )
		return NULL;

	return it->second;
	//return m_mapTargeData[ dwID ];
}

CSceneData* CsSceneDataMng::GetSceneData( DWORD dwID )
{	
	assert_cs( IsData( dwID ) == true );

	MAP_IT it = m_mapSceneData.find( dwID );
	if( it == m_mapSceneData.end() )
		return NULL;

	return it->second;

	//return m_mapSceneData[ dwID ];
}