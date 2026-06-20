
#include "stdafx.h"
#include "UI_Text.h"

//======================================================================================================
//
// UI String 
//
//======================================================================================================

void CsUITextMng::Delete()
{
	_Delete_UI_Text();
}

bool CsUITextMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel_UI_Text();
		break;
	case nsCsFileTable::FT_BIN:		
		if( _LoadBin( cPath ) == false )
			return false;
		break;
	case nsCsFileTable::FT_FILEPACK:
		if( _LoadFilePack( cPath ) == false )
			return false;
		break;
	default:
		assert_cs( false );
	}
	return true;
}

#define UITEXT_BIN		"UIText.bin"
void CsUITextMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, UITEXT_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	_SaveBin_UI_Text( fp );
	fclose( fp );
}

bool CsUITextMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, UITEXT_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	_LoadBin_UI_Text( fp );

	fclose( fp );
	return true;
}

bool CsUITextMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, UITEXT_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return false;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	_LoadFilePack_UI_Text( nHandle );

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
	return true;
}

void CsUITextMng::_Delete_UI_Text()
{
	m_mapUI_Text.clear();
}

void CsUITextMng::_LoadExcel_UI_Text()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_UIText(), "DM_UiText" );
	if( bSuccess == false )
	{
		CsMessageBox( MB_OK, L"DM_UI_Text Sheet Open Fail" );
		return;
	}

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

#define IL_COL_TAG					0
#define IL_COL_MESSAGE_TEXT			IL_COL_TAG + 1

	while( excel.MoveNext() == true )
	{
		std::wstring strKey;
		excel.Read_Value( IL_COL_TAG, strKey );
		if( strKey.empty() )
			continue;

		UINT64 hashKey = CsFPS::GetHashCode( strKey.c_str() );
		CsUI_Text::MAP_IT it  = m_mapUI_Text.find( hashKey );
		if( it != m_mapUI_Text.end() )
		{
			CsMessageBox( MB_OK, L"이미 사용중인 TAG (%s)", strKey.c_str() );
			continue;
		}		

		std::wstring wsMessage;
		excel.Read_Value( IL_COL_MESSAGE_TEXT, wsMessage );
		auto _rib = m_mapUI_Text.insert( make_pair( hashKey, wsMessage ) );
		if( !_rib.second )
			CsMessageBox( MB_OK, L"메시지를 넣지 못하였습니다. Tag : %s, Message : %s", strKey.c_str(), wsMessage.c_str() );
	}
	excel.Close();
}

void CsUITextMng::_LoadBin_UI_Text( FILE* fp )
{
	int nCount;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	if( nCount == 0 )
		return;

	for( int i=0; i<nCount; ++i )
	{
		INT64 hashKey = 0;
		CmUtil::CmBinRead::Read_Value( fp, &hashKey );
		std::wstring msg;
		CmUtil::CmBinRead::Read_Value( fp, msg );
		m_mapUI_Text.insert( make_pair( hashKey, msg ) );
	}
}

void CsUITextMng::_LoadFilePack_UI_Text( int nHandle )
{
	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	if( nCount == 0 )
		return;

	for( int i=0; i<nCount; ++i )
	{
		INT64 hashKey = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &hashKey );
		std::wstring msg;
		CmUtil::CmPackRead::Read_Value( nHandle, msg );
		m_mapUI_Text.insert( make_pair( hashKey, msg ) );
	}
}

void CsUITextMng::_SaveBin_UI_Text( FILE* fp )
{
	int nCount = (int)m_mapUI_Text.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	CsUI_Text::MAP_IT it = m_mapUI_Text.begin();
	CsUI_Text::MAP_IT itEnd = m_mapUI_Text.end();
	for( ; it!=itEnd; ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(it->first) );
		CmUtil::CmBinSave::Save_Value( fp, it->second );	
	}
}

std::wstring CsUITextMng::GetUIText( char* pKey )
{
	UINT64 hashKey = CsFPS::GetHashCode( pKey );
	CsUI_Text::MAP_IT it = m_mapUI_Text.find( hashKey );
	if( it == m_mapUI_Text.end() )
	{
		return std::wstring(pKey, pKey + strlen(pKey));
		//return L"";
	}
	return it->second;
}

std::wstring CsUITextMng::GetUIText_VA( char* pKey, ... )
{
	std::wstring resultStr = GetUIText( pKey );
	if( resultStr.empty() )
		return resultStr;

	WCHAR tmpStr[1024] = {0,};
	va_list argList;
	va_start( argList, pKey );
	_vstprintf_s( tmpStr, 1024, resultStr.c_str(), argList );
	tmpStr[1023] = L'\0';
	va_end( argList );

	resultStr = tmpStr;
	return resultStr;
}
