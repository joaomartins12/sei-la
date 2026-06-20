
#include "stdafx.h"
#include "CharacterCreateListInfo.h"


CsCharCreateTableMng::CsCharCreateTableMng()
{

}

CsCharCreateTableMng::~CsCharCreateTableMng()
{

}

void CsCharCreateTableMng::Delete()
{
}

bool CsCharCreateTableMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel_TamerCreate();
		_LoadExcel_DigimonCreate();
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


LIST_TAMER_CREATE_INFO const&	CsCharCreateTableMng::GetTamerCreateInfo() const
{
	return m_listTamerCreateInfos;
}

LIST_DIGIMON_CREATE_INFO const&	CsCharCreateTableMng::GetDigimonCreateInfo() const
{
	return m_listDigimonCreateInfos;
}

void CsCharCreateTableMng::_LoadExcel_TamerCreate()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_CharCreate(), "TamerCreateList" );
	if( bSuccess == false )
	{
		CsMessageBox( MB_OK, L"TamerCreateList Sheet Open Fail" );
		return;
	}

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	while( excel.MoveNext() == true )
	{
		DWORD dwID = 0;
		excel.Read_Value( 0, dwID );
		if( 0 == dwID )
			break;

		sTamerCreateInfo addInfo;
		addInfo.m_dwTalbeID = dwID;
		excel.Read_Value( 3, addInfo.m_bShow );
		excel.Read_Value( 4, addInfo.m_bEnable );
		excel.Read_Value( 5, addInfo.m_nSeasonType );
		excel.Read_Value( 6, addInfo.m_sVoiceFile );
		excel.Read_Value( 7, addInfo.m_nIconIdx );

		int nCount = 0;
		do 
		{
			DWORD dwCostumeID = 0;
			excel.Read_Value( 8 + nCount, dwCostumeID );
			if( 0 == dwCostumeID )
				break;
			addInfo.m_listCostume.push_back( dwCostumeID );
			++nCount;
		} while ( 1 );

		m_listTamerCreateInfos.push_back(addInfo);
	}
	excel.Close();
}

void CsCharCreateTableMng::_LoadExcel_DigimonCreate()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_CharCreate(), "DigimonCreateList" );
	if( bSuccess == false )
	{
		CsMessageBox( MB_OK, L"DigimonCreateList Sheet Open Fail" );
		return;
	}

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	while( excel.MoveNext() == true )
	{
		DWORD dwID = 0;
		excel.Read_Value( 0, dwID );
		if( 0 == dwID )
			break;

		sDigimonCreateInfo addInfo;
		addInfo.m_dwTalbeID = dwID;
		excel.Read_Value( 3, addInfo.m_bShow );
		excel.Read_Value( 4, addInfo.m_bEnable );
		excel.Read_Value( 5, addInfo.m_sVoiceFile );

		m_listDigimonCreateInfos.push_back( addInfo );
	}
	excel.Close();
}

#define CHARCREATE_TABLE_BIN		"CharCreateTable.bin"

void CsCharCreateTableMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CHARCREATE_TABLE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );
	
	_SaveBin_TamerCreateInfo( fp );
	_SaveBin_DigimonCreateInfo( fp );

	fclose( fp );
}

void CsCharCreateTableMng::_SaveBin_TamerCreateInfo( FILE* fp )
{
	int nCount = (int)m_listTamerCreateInfos.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );
	LIST_TAMER_CREATE_INFO_CIT it = m_listTamerCreateInfos.begin();
	for( ; it != m_listTamerCreateInfos.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(*it).m_dwTalbeID );
		CmUtil::CmBinSave::Save_Value( fp, &(*it).m_bShow );
		CmUtil::CmBinSave::Save_Value( fp, &(*it).m_bEnable );
		CmUtil::CmBinSave::Save_Value( fp, &(*it).m_nSeasonType );
		CmUtil::CmBinSave::Save_Value( fp, (*it).m_sVoiceFile );
		CmUtil::CmBinSave::Save_Value( fp, &(*it).m_nIconIdx );

		int nCostumeCount = (int)(*it).m_listCostume.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCostumeCount );

		std::list<DWORD>::const_iterator subIT = (*it).m_listCostume.begin();
		for( ; subIT != (*it).m_listCostume.end(); ++subIT )
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT) );
	}
}

void CsCharCreateTableMng::_SaveBin_DigimonCreateInfo( FILE* fp )
{
	int nCount = (int)m_listDigimonCreateInfos.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );
	LIST_DIGIMON_CREATE_INFO_CIT it = m_listDigimonCreateInfos.begin();
	for( ; it != m_listDigimonCreateInfos.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(*it).m_dwTalbeID );
		CmUtil::CmBinSave::Save_Value( fp, &(*it).m_bShow );
		CmUtil::CmBinSave::Save_Value( fp, &(*it).m_bEnable );
		CmUtil::CmBinSave::Save_Value( fp, (*it).m_sVoiceFile );
	}
}

bool CsCharCreateTableMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CHARCREATE_TABLE_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}
	
	_LoadBin_TamerCreateInfo(fp);
	_LoadBin_DigimonCreateInfo(fp);

	fclose( fp );
	return true;
}

void CsCharCreateTableMng::_LoadBin_TamerCreateInfo( FILE *fp )
{
	int nCount;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		sTamerCreateInfo addInfo;
		CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_dwTalbeID );
		CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_bShow );
		CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_bEnable );
		CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_nSeasonType );
		CmUtil::CmBinRead::Read_Value( fp, addInfo.m_sVoiceFile );
		CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_nIconIdx );

		int nCostumeCount;
		CmUtil::CmBinRead::Read_Value( fp, &nCostumeCount );
		for( int n = 0; n < nCostumeCount; ++n )
		{
			DWORD dwCostumeID = 0;
			CmUtil::CmBinRead::Read_Value( fp, &dwCostumeID );
			addInfo.m_listCostume.push_back( dwCostumeID );
		}
		m_listTamerCreateInfos.push_back( addInfo );
	}
}

void CsCharCreateTableMng::_LoadBin_DigimonCreateInfo( FILE *fp )
{
	int nCount;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		sDigimonCreateInfo addInfo;
		CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_dwTalbeID );
		CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_bShow );
		CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_bEnable );
		CmUtil::CmBinRead::Read_Value( fp, addInfo.m_sVoiceFile );
		m_listDigimonCreateInfos.push_back( addInfo );
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CsCharCreateTableMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CHARCREATE_TABLE_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return false;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );
	_LoadFile_TamerCreateInfo(nHandle);
	_LoadFile_DigimonCreateInfo(nHandle);
	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
	return true;
}

void CsCharCreateTableMng::_LoadFile_TamerCreateInfo( int nHandle )
{
	int nCount;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		sTamerCreateInfo addInfo;
		CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_dwTalbeID );
		CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_bShow );
		CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_bEnable );
		CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_nSeasonType );
		CmUtil::CmPackRead::Read_Value( nHandle, addInfo.m_sVoiceFile );
		CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_nIconIdx );

		int nCostumeCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCostumeCount );
		for( int n = 0; n < nCostumeCount; ++n )
		{
			DWORD dwCostumeID = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &dwCostumeID );
			addInfo.m_listCostume.push_back( dwCostumeID );
		}
		m_listTamerCreateInfos.push_back( addInfo );
	}
}

void CsCharCreateTableMng::_LoadFile_DigimonCreateInfo( int nHandle )
{
	int nCount;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		sDigimonCreateInfo addInfo;
		CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_dwTalbeID );
		CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_bShow );
		CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_bEnable );
		CmUtil::CmPackRead::Read_Value( nHandle, addInfo.m_sVoiceFile );
		m_listDigimonCreateInfos.push_back( addInfo );
	}
}
