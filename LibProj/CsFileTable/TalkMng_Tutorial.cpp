
#include "stdafx.h"
#include "TalkMng.h"

void CsTalkMng::Delete_Tutorial()
{
	CsTalk_Tutorial::MAP_IT it = m_mapTutorial.begin();
	CsTalk_Tutorial::MAP_IT itEnd = m_mapTutorial.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapTutorial.clear();
}

CsTalk_Tutorial* CsTalkMng::GetTalk_Tutorial( DWORD dwID )
{
	assert_cs( m_mapTutorial.find( dwID ) != m_mapTutorial.end() );
	return m_mapTutorial[ dwID ];
}


bool CsTalkMng::Load_Tutorial()
{
	char cPath[ MAX_PATH ];
	nsCsFileTable::g_FileTableMng.GetLanguagePath( nsCsFileTable::g_eLanguage, cPath );

	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel_Tutorial();
		break;
	case nsCsFileTable::FT_BIN:
#ifdef _DEBUG
		_LoadExcel_Tutorial();
#else
		if( _LoadBin_Tutorial( cPath ) == false )
			return false;
#endif
		break;
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack_Tutorial( cPath );
		break;
	default:
		assert_cs( false );
	}

	return true;
}


void CsTalkMng::_LoadExcel_Tutorial()
{
	Delete_Tutorial();

#define IL_COL_START				1
#define TALK_SHEET_NAME				"Tutorial"

	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Talk(), TALK_SHEET_NAME );
	assert_cs( bSuccess == true );
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	

	CsTalk_Tutorial::sINFO info;
	while( excel.MoveNext() == true )
	{
		M2W( info.s_szText, excel.GetData( IL_COL_START, cBuffer ), FT_TALK_TUTORIAL_LEN );

		CsTalk_Tutorial* pTalk = csnew CsTalk_Tutorial;
		pTalk->Init( &info );
		m_mapTutorial[ (DWORD)m_mapTutorial.size() ] = pTalk;
	}
	excel.Close();
}

#define TUTORIAL_BIN		"Tutorial.bin"
void CsTalkMng::_SaveBin_Tutorial( char* cPath )
{
	Delete_Tutorial();
	Load_Tutorial();

	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TUTORIAL_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapTutorial.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsTalk_Tutorial::MAP_IT it = m_mapTutorial.begin();
	CsTalk_Tutorial::MAP_IT itEnd = m_mapTutorial.end();
	for( ; it!=itEnd; ++it )
	{				
		fwrite( it->second->GetInfo(), sizeof( CsTalk_Tutorial::sINFO ), 1, fp );
	}

	fclose( fp );
}

bool CsTalkMng::_LoadBin_Tutorial( char* cPath )
{
	Delete_Tutorial();

	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TUTORIAL_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsTalk_Tutorial::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &info, sizeof( CsTalk_Tutorial::sINFO ), 1, fp );

		CsTalk_Tutorial* pObject = csnew CsTalk_Tutorial;
		pObject->Init( &info );
		m_mapTutorial[ (DWORD)m_mapTutorial.size() ] = pObject;
	}

	fclose( fp );

	return true;
}

void CsTalkMng::_LoadFilePack_Tutorial( char* cPath )
{
	Delete_Tutorial();

	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TUTORIAL_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	CsTalk_Tutorial::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info, sizeof( CsTalk_Tutorial::sINFO ) );

		CsTalk_Tutorial* pObject = csnew CsTalk_Tutorial;
		pObject->Init( &info );
		m_mapTutorial[ (DWORD)m_mapTutorial.size() ] = pObject;
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}
