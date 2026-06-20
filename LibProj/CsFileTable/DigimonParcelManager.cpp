
#include "stdafx.h"
#include "DigimonParcelManager.h"

CsDigimonParcelMng::CsDigimonParcelMng()
{

}

CsDigimonParcelMng::~CsDigimonParcelMng()
{

}

bool CsDigimonParcelMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:		_LoadExcel();	break;
	case nsCsFileTable::FT_BIN:			_LoadBin( cPath );	break;
	case nsCsFileTable::FT_FILEPACK:	_LoadFilePack( cPath ); break;
	default:
		assert_cs( false );
		return false;
	}

	return true;
}

void CsDigimonParcelMng::Delete()
{
}

bool CsDigimonParcelMng::_LoadExcel()
{
	if( !_LoadExcel_ParcelDigimon( nsCsFileTable::GetPath_DM_Parcel(), "Digimon" ) )
		return false;

	return true;
}

bool CsDigimonParcelMng::_LoadExcel_ParcelDigimon( char* pExcelFile, char* pExcelSheetName )
{
	cExcelReader excel;
	bool bSuccess = excel.Open( pExcelFile, pExcelSheetName );
	if( bSuccess == false )
		return false;

	excel.FirstOfSkip(2);// 2줄은 버린다.

	while( excel.MoveNext() )
	{
		DWORD dwDigimonIDX = 0;
		excel.Read_Value( 0, dwDigimonIDX );
		if( dwDigimonIDX == 0 )
			break;

		m_listDigimon.push_back( dwDigimonIDX );
	}
	excel.Close();
	return true;

}
#define DIGIMONPARCEL_BIN		"DigimonParcel.bin"
void CsDigimonParcelMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DIGIMONPARCEL_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_listDigimon.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	std::list<DWORD>::const_iterator  it = m_listDigimon.begin();
	for( ; it!=m_listDigimon.end(); ++it )
		CmUtil::CmBinSave::Save_Value( fp, &(*it) );

	fclose( fp );

}

bool CsDigimonParcelMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DIGIMONPARCEL_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	if( nCount == 0 )
		return true;

	for( int i=0; i<nCount; ++i )
	{
		DWORD dwDigimonIDX = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwDigimonIDX );
		m_listDigimon.push_back( dwDigimonIDX );
	}

	fclose( fp );
	
	return true;
}

bool CsDigimonParcelMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DIGIMONPARCEL_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return false;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		DWORD dwDigimonIDX = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwDigimonIDX );
		m_listDigimon.push_back( dwDigimonIDX );
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CsDigimonParcelMng::IsParcelDigimon( DWORD const& dwDigimonIDX ) const
{
	std::list<DWORD>::const_iterator it = m_listDigimon.begin();
	for( ; it != m_listDigimon.end(); ++it )
	{
		if( (*it) != dwDigimonIDX )
			continue;

		return true;
	}
	return false;
}