#include "StdAfx.h"
#include "AddExp.h"

CsAddExpMng::CsAddExpMng(void)
{
}

CsAddExpMng::~CsAddExpMng(void)
{
	Delete();
}

void CsAddExpMng::Delete()
{
	std::map< DWORD, sAddExp* >::iterator it = m_mapExp.begin();
	std::map< DWORD, sAddExp* >::iterator itEnd = m_mapExp.end();

	for( ; it!=itEnd; ++it )
	{		
		delete it->second;
	}
	m_mapExp.clear();		
}

bool CsAddExpMng::Init( char* cPath )
{	
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

CsAddExpMng::sAddExp* CsAddExpMng::FindData_FromSkill( DWORD dwSkill )
{
	std::map< DWORD, sAddExp* >::iterator it = m_mapExp.begin();
	std::map< DWORD, sAddExp* >::iterator itEnd = m_mapExp.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->s_nSkillID == dwSkill )
			return it->second;
	}
	return NULL;
}

void CsAddExpMng::_LoadExcel()
{
#define IL_NO_COL				1
#define IL_NO_SKILLNUM			3
#define IL_NO_TITLE				4
#define IL_NO_ICON				5
#define IL_NO_COMMENT			6

	DWORD dwStartID = 0;
	char cBuffer[ CEXCLE_CELL_LEN ];	
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_AddEffect(), "Exp_Buff" );		

	assert_cs( bSuccess == true );
	// 3줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	excel.MoveNext();
	excel.MoveNext();

	DWORD nID;
	DWORD nSkillID;
	while( excel.MoveNext() == true )
	{
		nID = atol( excel.GetData( IL_NO_COL, cBuffer ) );
		if( nID == 0 )
			continue;

		nSkillID = atoi( excel.GetData( IL_NO_SKILLNUM, cBuffer ) );
		if( nSkillID == 0 )
			continue;

		sAddExp* info = csnew sAddExp;
		info->s_nID = nID;
		info->s_nSkillID = nSkillID;
		info->s_nIconID = atoi( excel.GetData( IL_NO_ICON, cBuffer ) );

		excel.GetData( IL_NO_TITLE, cBuffer );
		M2W( info->s_szTitle, cBuffer, ITEM_NAME_LEN );

		excel.GetData( IL_NO_COMMENT, cBuffer );
		M2W( info->s_szComment, cBuffer, ITEM_COMMENT_LEN );

		m_mapExp[ nID ] = info;
	}	

	excel.Close();
}


#define ADDEXP_BIN		"AddExp.bin"
bool CsAddExpMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, ADDEXP_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );		
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );	
	for( int j = 0; j < nCount; j++ )
	{						
		sAddExp* sInfo = csnew sAddExp;
		fread( sInfo, sizeof( sAddExp ), 1, fp );						

		m_mapExp[ sInfo->s_nID ] = sInfo;	
	}

	fclose( fp );

	return true;	
}

void CsAddExpMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, ADDEXP_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );	
	for( int j = 0; j < nCount; j++ )
	{	
		sAddExp* sInfo = csnew sAddExp;
		_read( nHandle, sInfo, sizeof( sAddExp ) );

		m_mapExp[ sInfo->s_nID ] = sInfo;	
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

void CsAddExpMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, ADDEXP_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapExp.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::map< DWORD, sAddExp* >::iterator it = m_mapExp.begin();
	std::map< DWORD, sAddExp* >::iterator itEnd = m_mapExp.end();

	for( ; it != itEnd; it++ )
	{	
		fwrite( it->second, sizeof( sAddExp ), 1, fp );								
	}	

	fclose( fp );
}
