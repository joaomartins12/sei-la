
#include "stdafx.h"
#include "ToolResource.h"

cToolResource		g_ToolResource;

#define TOOLRESOURCE_MAPPING_PATH	L".\\ToolData\\ToolResMapping.lst"
#define TOOLRESOURCE_OBJECT_PATH	L".\\ToolData\\ToolResObject.lst"

void cToolResource::SaveMapping()
{
	assert_cs( m_listMapping.empty() == true );
	
	TCHAR szFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, TOOLRESOURCE_MAPPING_PATH );
	// 폴더가 존재 하지 않는다면 폴더 생성
	nsCSFILE::CreateDirectory( szFolder );

	TCHAR szPath[ MAX_PATH ];
	CTreeCtrl* pTreeMapping = GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetTreeTexture();
	HTREEITEM hRootFind = pTreeMapping->GetRootItem();
	while( hRootFind != NULL )
	{
		_stprintf_s( szPath, MAX_PATH, pTreeMapping->GetItemText( hRootFind ) );
		HTREEITEM hChild = pTreeMapping->GetChildItem( hRootFind );
		while( hChild != NULL )
		{
			sMAPPING* pMapping = m_mempoolMapping.Pop();
			_stprintf_s( pMapping->s_szPath, MAX_PATH, L"%s\\%s", szPath, pTreeMapping->GetItemText( hChild ) );
			m_listMapping.push_back( pMapping );
			hChild = pTreeMapping->GetNextSiblingItem( hChild );				
		}
		hRootFind = pTreeMapping->GetNextSiblingItem( hRootFind );
	}

	sHEADER he;
	he.s_dwVersion = TOOLRESOURCE_VERSION;
	he.s_nCount = (int)m_listMapping.size();

	FILE* fp = NULL;
	_tfopen_s( &fp, TOOLRESOURCE_MAPPING_PATH, L"wb" );
	assert_cs( fp != NULL );
	fwrite( &he, sizeof( sHEADER ), 1, fp );
	std::list< sMAPPING* >::iterator it = m_listMapping.begin();
	for( ; it!=m_listMapping.end(); ++it )
	{
		fwrite( (*it), sizeof( sMAPPING ), 1, fp );
		m_mempoolMapping.Push( (*it) );
	}
	fclose( fp );
	m_listMapping.clear();
}

void cToolResource::LoadMapping()
{
	FILE* fp = NULL;
	_tfopen_s( &fp, TOOLRESOURCE_MAPPING_PATH, L"rb" );
	if( fp == NULL )
		return;

	sHEADER he;
	sMAPPING mapping;
	TCHAR szPathRead[ MAX_PATH ];
	TCHAR szName[ MAX_FILENAME ];
	fread( &he, sizeof( sHEADER ), 1, fp );
	for( int i=0; i<he.s_nCount; ++i )
	{
		fread( &mapping, sizeof( sMAPPING ), 1, fp );

		LPCTSTR szSub = nsCSFILE::GetSubPath( mapping.s_szPath, L"Data\\" );
		_stprintf_s( szPathRead, L"Data\\%s", szSub );
		if( _taccess( szPathRead, 0 ) != 0 )
			continue;
	
		nsCSFILE::GetFilePathOnly( szPathRead, MAX_PATH, mapping.s_szPath, true );
		_tcscpy_s( szName, MAX_FILENAME, nsCSFILE::GetFileName( mapping.s_szPath ) );
		GET_MV_MAINTAB_DLG->_GetDlgMapping()->__InsertTree( szPathRead, szName );
	}
	fclose( fp );
}

void cToolResource::SaveObject()
{
	assert_cs( m_listObject.empty() == true );

	TCHAR szFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, TOOLRESOURCE_OBJECT_PATH );
	// 폴더가 존재 하지 않는다면 폴더 생성
	nsCSFILE::CreateDirectory( szFolder );

	TCHAR szPath[ MAX_PATH ];
	CTreeCtrl* pTreeObject = GET_MV_MAINTAB_DLG->_GetDlgObejct()->_GetTreeObject();
	HTREEITEM hRootFind = pTreeObject->GetRootItem();
	while( hRootFind != NULL )
	{
		_stprintf_s( szPath, MAX_PATH, pTreeObject->GetItemText( hRootFind ) );
		HTREEITEM hChild = pTreeObject->GetChildItem( hRootFind );
		while( hChild != NULL )
		{
			sOBJECT* pObject = m_mempoolObject.Pop();
			_stprintf_s( pObject->s_szPath, MAX_PATH, L"%s\\%s", szPath, pTreeObject->GetItemText( hChild ) );
			m_listObject.push_back( pObject );
			hChild = pTreeObject->GetNextSiblingItem( hChild );				
		}
		hRootFind = pTreeObject->GetNextSiblingItem( hRootFind );
	}

	sHEADER he;
	he.s_dwVersion = TOOLRESOURCE_VERSION;
	he.s_nCount = (int)m_listObject.size();

	FILE* fp = NULL;
	_tfopen_s( &fp, TOOLRESOURCE_OBJECT_PATH, L"wb" );
	assert_cs( fp != NULL );
	fwrite( &he, sizeof( sHEADER ), 1, fp );
	std::list< sOBJECT* >::iterator it = m_listObject.begin();
	for( ; it!=m_listObject.end(); ++it )
	{
		fwrite( (*it), sizeof( sOBJECT ), 1, fp );
		m_mempoolObject.Push( (*it) );
	}
	fclose( fp );
	m_listObject.clear();
}

void cToolResource::LoadObject()
{
	FILE* fp = NULL;
	_tfopen_s( &fp, TOOLRESOURCE_OBJECT_PATH, L"rb" );
	if( fp == NULL )
		return;

	sHEADER he;
	sOBJECT object;
	TCHAR szPathRead[ MAX_PATH ];	
	TCHAR szName[ MAX_FILENAME ];
	fread( &he, sizeof( sHEADER ), 1, fp );
	for( int i=0; i<he.s_nCount; ++i )
	{
		fread( &object, sizeof( sOBJECT ), 1, fp );

		LPCTSTR szSub = nsCSFILE::GetSubPath( object.s_szPath, L"Data\\" );
		_stprintf_s( szPathRead, L"Data\\%s", szSub );
		if( _taccess( szPathRead, 0 ) != 0 )
			continue;

		nsCSFILE::GetFilePathOnly( szPathRead, MAX_PATH, object.s_szPath, true );
		_tcscpy_s( szName, MAX_FILENAME, nsCSFILE::GetFileName( object.s_szPath ) );
		
		GET_MV_MAINTAB_DLG->_GetDlgObejct()->__InsertTree( szPathRead, szName );
	}
	fclose( fp );
}