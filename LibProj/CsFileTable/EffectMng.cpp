
#include "stdafx.h"
#include "EffectMng.h"

CsEffectMng::CsEffectMng()
{

}

CsEffectMng::~CsEffectMng()
{

}

bool CsEffectMng::Init( char* cPath )
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


void CsEffectMng::Delete()
{
}

void CsEffectMng::_LoadExcel()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_EffectMng(), "EquipEffect" );
	if( !bSuccess )
		return;

	bSuccess = excel.FirstOfSkip(2);
	if( !bSuccess )
		return;

	while( excel.MoveNext() )
	{
		DWORD dwItemCode = 0;
		excel.Read_Value( 1, dwItemCode );
		if( 0 == dwItemCode )
			continue;

		std::string effectFile;
		excel.Read_Value( 3, effectFile );
		if( effectFile.empty() )
			continue;
		std::map<DWORD,std::string>::iterator it = m_EquipEffect.find( dwItemCode );
		if( it == m_EquipEffect.end() )
			m_EquipEffect.insert( std::make_pair( dwItemCode, effectFile ) );
		else
			CsMessageBoxA( MB_OK, "이미 있는 아이템 코드 %d", dwItemCode );
	}
	excel.Close();
}

void CsEffectMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, "EffectList.bin" );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );

	int nCount = (int)m_EquipEffect.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );
	std::map<DWORD,std::string>::const_iterator it = m_EquipEffect.begin();
	for( ; it!=m_EquipEffect.end(); ++it )
	{		
		CmUtil::CmBinSave::Save_Value( fp, &it->first );
		CmUtil::CmBinSave::Save_Value( fp, it->second );
	}
	fclose( fp );
}

bool CsEffectMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, "EffectList.bin" );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );	
	for( int i=0; i<nCount; ++i )
	{
		DWORD dwItemCode = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwItemCode );
		std::string fileName;
		CmUtil::CmBinRead::Read_Value( fp, fileName );	

		std::map<DWORD,std::string>::iterator it = m_EquipEffect.find( dwItemCode );
		if( it == m_EquipEffect.end() )
			m_EquipEffect.insert( std::make_pair( dwItemCode, fileName ) );
	}

	fclose( fp );
	return true;
}

void CsEffectMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, "EffectList.bin" );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		DWORD dwItemCode = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwItemCode );
		std::string fileName;
		CmUtil::CmPackRead::Read_Value( nHandle, fileName );

		std::map<DWORD,std::string>::iterator it = m_EquipEffect.find( dwItemCode );
		if( it == m_EquipEffect.end() )
			m_EquipEffect.insert( std::make_pair( dwItemCode, fileName ) );		
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}


std::string	CsEffectMng::GetEquipEffectFile(DWORD const& dwItemCode) const
{
	std::map<DWORD,std::string>::const_iterator it = m_EquipEffect.find( dwItemCode );
	if( it == m_EquipEffect.end() )
		return "";
	return it->second;
}