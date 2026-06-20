
#include "stdafx.h"
#include "TamerMng.h"

// Bibliotecas para salvar TXT
#include <fstream>
#include <string>
#include <sstream>
#include <codecvt>
#include <locale>

// Função para converter TCHAR para std::string
std::string TCHARToString(const TCHAR* tcharArray, size_t size) {
#ifdef UNICODE
	std::wstring wstr(tcharArray, size);
	std::wstring::size_type nullPos = wstr.find(L'\0');
	if (nullPos != std::wstring::npos) {
		wstr = wstr.substr(0, nullPos);
	}
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wstr);
#else
	std::string str(tcharArray, size);
	std::string::size_type nullPos = str.find('\0');
	if (nullPos != std::string::npos) {
		str = str.substr(0, nullPos);
	}
	return str;
#endif
}

// Função para converter char array para std::string
std::string CharArrayToString(const char* charArray, size_t size) {
	std::string str(charArray, size);
	std::string::size_type nullPos = str.find('\0');
	if (nullPos != std::string::npos) {
		str = str.substr(0, nullPos);
	}
	return str;
}

// ---------------------------------------------------

void CsTamerMng::Delete()
{
	{
		CsTamer::MAP_IT it = m_mapTamer.begin();
		CsTamer::MAP_IT itEnd = m_mapTamer.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapTamer.clear();
	}

	{
		CsEmotion::MAP_IT it = m_mapEmotion.begin();
		CsEmotion::MAP_IT itEnd = m_mapEmotion.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapEmotion.clear();
	}
}

bool CsTamerMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		{
			//CsMessageBoxA(MB_OK, "Loading --- FT_EXCEL");
			_LoadExcel();
		}
		break;
	case nsCsFileTable::FT_BIN:
		{
			//CsMessageBoxA(MB_OK, "Loading --- FT_BIN");
			if (_LoadBin(cPath) == false)
				return false;
		}
		break;
	case nsCsFileTable::FT_FILEPACK:
		{
			//CsMessageBoxA(MB_OK, "Loading --- FT_FILEPACK");
			_LoadFilePack( cPath );
		}
		break;
	default:
		assert_cs( false );
	}

	return true;
}

CsEmotion::sINFO* CsTamerMng::FindEmotion( TCHAR const* szStr, int& nFindIndex )
{
	CsEmotion::MAP_IT it = m_mapEmotion.begin();
	CsEmotion::MAP_IT itEnd = m_mapEmotion.end();
	CsEmotion::sINFO* pInfo = NULL;
	for( ; it!=itEnd; ++it )
	{
		pInfo = it->second->GetInfo();
		for( int i=0; i<pInfo->s_nUseCmdCount; ++i )
		{
			assert_cs( pInfo->s_szCmd[ i ][ 0 ] != NULL );
			assert_cs( pInfo->s_szCmd[ i ][ 1 ] != NULL );
			assert_cs( _tcslen( pInfo->s_szCmd[ i ] ) > 1 );

			std::wstring wsCmp = &pInfo->s_szCmd[ i ][ 1 ];
			std::transform( wsCmp.begin(), wsCmp.end(), wsCmp.begin(), tolower );

			if( wsCmp.compare( szStr ) == 0 )
			{
				nFindIndex = i;
				return pInfo;
			}
		}
	}
	return NULL;
}

// ---------- LOAD EXCEL FILE ----------

void CsTamerMng::_LoadExcel()
{
	_LoadExcel_Tamer( nsCsFileTable::GetPath_DM_Tamer() );
	_LoadExcel_Emotion( nsCsFileTable::GetPath_DM_Tamer() );
}

void CsTamerMng::_LoadExcel_Tamer( char* cExcel )
{
#define TAMER_LIST_SHEET_NAME		"DM_Tamer_List"
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}

	cExcelReader excel;
	bool bSuccess = excel.Open( cExcel, TAMER_LIST_SHEET_NAME );
	assert_cs( bSuccess == true );

#define DL_COL_TAMERID					1
#define DL_COL_NAME						DL_COL_TAMERID + 2
#define DL_COL_NAME_ENG					DL_COL_NAME + 1
#define DL_COL_TAMER_TYPE				DL_COL_NAME_ENG + 1
#define DL_COL_SKILL_START				DL_COL_TAMER_TYPE + 2

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsTamer::sINFO info;
	while( excel.MoveNext() == true )
	{
		// ID
		info.s_dwTamerID = atoi( excel.GetData( DL_COL_TAMERID, cBuffer ) );

		if( info.s_dwTamerID == 0 )
			continue;		

		// ModelID
		assert_cs( info.s_dwTamerID == atoi( excel.GetData( DL_COL_TAMERID + 1, cBuffer ) ) );

		// Name
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info.s_szName, excel.GetData( DL_COL_NAME_ENG, cBuffer ), MAX_FILENAME );
			break;
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info.s_szName, MAX_FILENAME, excel.GetData( DL_COL_NAME_ENG, wBuffer ));
			break;
		default:
			CsMessageBoxA(MB_OK, "Language not found: %s", nsCsFileTable::g_eLanguage);
			assert_cs( false );
		}

		// Sound Directory Name
		strcpy_s( info.s_cSoundDirName, MAX_FILENAME, excel.GetData( DL_COL_NAME_ENG, cBuffer ) );

		// 케릭터 타입
		info.s_nTamerType = atoi( excel.GetData( DL_COL_TAMER_TYPE, cBuffer ) );

		// 스킬
		long Index = DL_COL_SKILL_START;
		for( int i=0; i<TAMER_MAX_SKILL_COUNT; ++i )
		{
			info.s_Skill[ i ].s_dwID = atoi( excel.GetData( Index, cBuffer ) );
			Index += 4;
		}

		// 설명
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( info.s_szComment, excel.GetData( Index, cBuffer ), TAMER_COMMENT_LEN );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			{
//			M2W( info.s_szComment, excel.GetData( Index, cBuffer ), TAMER_COMMENT_LEN );

				//테이블 제한 글자 수 체크 수정 chu8820
				TCHAR szBuffer[ 2048 ];
				M2W( szBuffer, excel.GetData( Index, cBuffer ), 2048 );
				if( (int)_tcslen( szBuffer ) >= TAMER_COMMENT_LEN - 1 )
					CsMessageBox( MB_OK, L"Tamer : %d, 의 Message 테이머설명이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d", info.s_dwTamerID , (int)_tcslen( szBuffer ), TAMER_COMMENT_LEN );
				else
					_tcscpy_s( info.s_szComment, TAMER_COMMENT_LEN, szBuffer  );
			}
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info.s_szComment, TAMER_COMMENT_LEN, excel.GetData( Index, wBuffer ));
			break;
		default:
			assert_cs( false );
		}
		// 위 Index값이 변경되면 여기도 수정해야한다. 신규 테이머 아이템 2013/01/16 vf00
		Index++;		
		// 소속 
		//M2W( info.s_szPart, excel.GetData( Index++, cBuffer ), MAX_FILENAME );
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info.s_szPart, excel.GetData( Index++, cBuffer ), MAX_FILENAME );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::HONGKONG:
		case nsCsFileTable::TAIWAN:
			wcscpy_s(info.s_szPart, MAX_FILENAME, excel.GetData( Index++, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

#ifdef COSTUME_GENDER	
		// 성별
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info.s_szGender, excel.GetData( Index++, cBuffer ), MAX_FILENAME );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::HONGKONG:
		case nsCsFileTable::TAIWAN:
			wcscpy_s(info.s_szGender, MAX_FILENAME, excel.GetData( Index++, wBuffer ));
			break;
		default:
			assert_cs( false );
		}
#endif
		CsTamer* pTamer = csnew CsTamer;
		pTamer->Init( &info );

		m_mapTamer[ info.s_dwTamerID ] = pTamer;
	}

	excel.Close();
}

void CsTamerMng::_LoadExcel_Emotion( char* cExcel )
{
	#define EMOTION_LIST_SHEET_NAME		"Tamer_Emotion"
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}

	cExcelReader excel;
	bool bSuccess = excel.Open( cExcel, EMOTION_LIST_SHEET_NAME );
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];


	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsEmotion::sINFO info;	
	while( excel.MoveNext() == true )
	{
		// ID		
		info.s_nID = atoi( excel.GetData( 0, cBuffer ) );

		if( info.s_nID == 0 )
			continue;		

		// Name
		int nCol = 1;
		//M2W( info.s_szName, excel.GetData( nCol, cBuffer ), EMOTION_STR_LEN );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info.s_szName, excel.GetData( nCol, cBuffer ), EMOTION_STR_LEN );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info.s_szName, EMOTION_STR_LEN, excel.GetData( nCol, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

		++nCol;

		info.s_nUseCmdCount = 0;
		for( int i=0; i<EMOTION_CMD_COUNT; ++i )
		{
			//M2W( info.s_szCmd[ i ], excel.GetData( nCol, cBuffer ), EMOTION_STR_LEN );
			switch( nsCsFileTable::g_eLanguage )
			{
			case nsCsFileTable::KOREA_ORIGINAL:
			case nsCsFileTable::KOREA_TRANSLATION:
			case nsCsFileTable::ENGLISH:
			case nsCsFileTable::ENGLISH_A:
				M2W( info.s_szCmd[ i ], excel.GetData( nCol, cBuffer ), EMOTION_STR_LEN );
				break;
			case nsCsFileTable::THAILAND:
			case nsCsFileTable::TAIWAN:
			case nsCsFileTable::HONGKONG:
				wcscpy_s(info.s_szCmd[ i ], EMOTION_STR_LEN, excel.GetData( nCol, wBuffer ));
				break;
			default:
				assert_cs( false );
			}

			if( info.s_szCmd[ i ][ 0 ] == L'/' )
			{
				++info.s_nUseCmdCount;
			}
			++nCol;
		}

		info.s_nAniID = atoi( excel.GetData( 7, cBuffer ) );

		CsEmotion* pEmotion = csnew CsEmotion;
		pEmotion->Init( &info );

		m_mapEmotion[ info.s_nID ] = pEmotion;
	}

	excel.Close();
}

// ---------------------------------------

CsTamer* CsTamerMng::GetTamer( DWORD dwTamerID )
{
	assert_csm1( IsTamer( dwTamerID ), _T( "dwTamerID = %d" ), dwTamerID );
	return m_mapTamer[ dwTamerID ];
}

#define TAMER_TABLE_BIN			"TamerList.bin"
void CsTamerMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TAMER_TABLE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	{
		int nCount = (int)m_mapTamer.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsTamer::MAP_IT it = m_mapTamer.begin();
		CsTamer::MAP_IT itEnd = m_mapTamer.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsTamer::sINFO ), 1, fp );
		}
	}	

	{
		int nCount = (int)m_mapEmotion.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsEmotion::MAP_IT it = m_mapEmotion.begin();
		CsEmotion::MAP_IT itEnd = m_mapEmotion.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsEmotion::sINFO ), 1, fp );
		}
	}	

	fclose( fp );
}

bool CsTamerMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TAMER_TABLE_BIN );

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

		CsTamer::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsTamer::sINFO ), 1, fp );

			CsTamer* pObject = csnew CsTamer;
			pObject->Init( &info );
			assert_cs( m_mapTamer.find( info.s_dwTamerID ) == m_mapTamer.end() );
			m_mapTamer[ info.s_dwTamerID ] = pObject;
		}
	}

	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		CsEmotion::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsEmotion::sINFO ), 1, fp );

			CsEmotion* pObject = csnew CsEmotion;
			pObject->Init( &info );
			assert_cs( m_mapEmotion.find( info.s_nID ) == m_mapEmotion.end() );
			m_mapEmotion[ info.s_nID ] = pObject;
		}
	}

	fclose( fp );
	return true;
}

// ---------- LOAD FILE PACK ----------

void CsTamerMng::_LoadFilePack(char* cPath)
{
	char cName[MAX_PATH];
	sprintf_s(cName, MAX_PATH, "%s\\%s", cPath, TAMER_TABLE_BIN);		// Data/Bin/English/TamerList.bin

	if (!CsFPS::CsFPSystem::SeekLock(FT_PACKHANDLE))
	{
		CsMessageBoxA(MB_OK, "CsTamerMng::_LoadFilePack SeekLock failed. file=%s", cName);
		return;
	}

	int nHandle = CsFPS::CsFPSystem::GetFileHandle(FT_PACKHANDLE, cName);
	if (nHandle <= 0)
	{
		CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
		CsMessageBoxA(MB_OK, "CsTamerMng::_LoadFilePack GetFileHandle failed. file=%s", cName);
		return;
	}

	{
		int nCount = 0;
		int nRead = _read(nHandle, &nCount, sizeof(int));
		if (nRead != sizeof(int))
		{
			CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
			CsMessageBoxA(MB_OK, "CsTamerMng::_LoadFilePack failed reading tamer count. file=%s", cName);
			return;
		}

		if (nCount < 0 || nCount > 10000)
		{
			CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
			CsMessageBoxA(MB_OK, "CsTamerMng::_LoadFilePack invalid tamer count: %d. file=%s", nCount, cName);
			return;
		}

		CsTamer::sINFO info;
		for (int i = 0; i < nCount; ++i)
		{
			nRead = _read(nHandle, &info, sizeof(CsTamer::sINFO));
			if (nRead != sizeof(CsTamer::sINFO))
			{
				CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
				CsMessageBoxA(MB_OK, "CsTamerMng::_LoadFilePack failed reading tamer info. index=%d file=%s", i, cName);
				return;
			}

			CsTamer* pObject = csnew CsTamer;
			if (pObject == NULL)
			{
				CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
				CsMessageBoxA(MB_OK, "CsTamerMng::_LoadFilePack failed allocating CsTamer. index=%d file=%s", i, cName);
				return;
			}

			pObject->Init(&info);

			if (m_mapTamer.find(info.s_dwTamerID) != m_mapTamer.end())
			{
				pObject->Delete();
				delete pObject;
				continue;
			}

			m_mapTamer[info.s_dwTamerID] = pObject;
		}
	}

	{
		int nCount = 0;
		int nRead = _read(nHandle, &nCount, sizeof(int));
		if (nRead != sizeof(int))
		{
			CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
			CsMessageBoxA(MB_OK, "CsTamerMng::_LoadFilePack failed reading emotion count. file=%s", cName);
			return;
		}

		if (nCount < 0 || nCount > 10000)
		{
			CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
			CsMessageBoxA(MB_OK, "CsTamerMng::_LoadFilePack invalid emotion count: %d. file=%s", nCount, cName);
			return;
		}

		CsEmotion::sINFO info;
		for (int i = 0; i < nCount; ++i)
		{
			nRead = _read(nHandle, &info, sizeof(CsEmotion::sINFO));
			if (nRead != sizeof(CsEmotion::sINFO))
			{
				CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
				CsMessageBoxA(MB_OK, "CsTamerMng::_LoadFilePack failed reading emotion info. index=%d file=%s", i, cName);
				return;
			}

			CsEmotion* pObject = csnew CsEmotion;
			if (pObject == NULL)
			{
				CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
				CsMessageBoxA(MB_OK, "CsTamerMng::_LoadFilePack failed allocating CsEmotion. index=%d file=%s", i, cName);
				return;
			}

			pObject->Init(&info);

			if (m_mapEmotion.find(info.s_nID) != m_mapEmotion.end())
			{
				pObject->Delete();
				delete pObject;
				continue;
			}

			m_mapEmotion[info.s_nID] = pObject;
		}
	}

	CsFPS::CsFPSystem::SeekUnLock(FT_PACKHANDLE);
}

// ---------------------------------------
