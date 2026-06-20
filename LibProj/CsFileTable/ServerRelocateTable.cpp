
#include "stdafx.h"
#include "ServerRelocateTable.h"

CsServerRelocateTBMng::CsServerRelocateTBMng()
{

}

CsServerRelocateTBMng::~CsServerRelocateTBMng()
{
}

bool CsServerRelocateTBMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:		
		_LoadExcel();
		return true;
	case nsCsFileTable::FT_BIN:			
		_LoadBin( cPath );
		return true;
	case nsCsFileTable::FT_FILEPACK:	
		_LoadFilePack( cPath );
		return true;
	}

	return false;
}

void CsServerRelocateTBMng::Delete()
{
	m_TransferInfo.clear();
}

bool CsServerRelocateTBMng::_LoadExcel()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_ServerTransfer(), "Setup" );
	if( !bSuccess )
		return false;

	if( !excel.FirstOfSkip(2) )
		return false;

	while( excel.MoveNext() == true )
	{
		int nServerIDX = 0;
		excel.Read_Value( 0, nServerIDX );		// 서버 인덱스 
		if( 0 == nServerIDX )
			break;

		MAP_SERVERTRANSFER_INFO_IT it = m_TransferInfo.find( nServerIDX );
		if( it != m_TransferInfo.end() )
		{
			CsMessageBox( MB_OK, L"중복된 서버 인덱스가 존재합니다. : %d, %s", nServerIDX, it->second.m_serverName.c_str() );
			break;
		}
		else 
		{
			auto rib = m_TransferInfo.insert( std::make_pair( nServerIDX, sTransferInfo() ) );
			it = rib.first;
		}

		excel.Read_Value( 1, it->second.m_serverName );	// 서버 이름
		excel.Read_Value( 2, it->second.m_bEnable );		// 서버 이전 가능 여부
	}
	excel.Close();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	bSuccess = excel.Open( nsCsFileTable::GetPath_ServerTransfer(), "Limit" );
	if( !bSuccess )
		return false;

	if( !excel.FirstOfSkip(2) )
		return false;

	while( excel.MoveNext() == true )
	{
		int nServerIDX = 0;
		excel.Read_Value( 0, nServerIDX );		// 서버 인덱스 
		if( 0 == nServerIDX )
			break;

		MAP_SERVERTRANSFER_INFO_IT it = m_TransferInfo.find( nServerIDX );
		if( it == m_TransferInfo.end() )
		{
			CsMessageBox( MB_OK, L"서버 이전 제한 값을 적용할 서버를 찾을 수 없습니다. %d ", nServerIDX );
			break;
		}

		sTransferLimit addLimit;
		excel.Read_Value( 2, addLimit.m_nLimitType );	// 서버 이름
		excel.Read_Value( 4, addLimit.m_nLimitValue );	// 서버 이전 가능 여부

		it->second.m_LimitList.push_back( addLimit );
	}
	excel.Close();
	return true;
}

#define TRANSFER_BIN		"ServerTransfer.bin"

bool CsServerRelocateTBMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TRANSFER_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
		return false;

	int nCount;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		int nServerIDX;
		CmUtil::CmBinRead::Read_Value( fp, &nServerIDX );

		MAP_SERVERTRANSFER_INFO_IT it = m_TransferInfo.find( nServerIDX );
		if( it == m_TransferInfo.end() )
		{
			auto rib = m_TransferInfo.insert( std::make_pair( nServerIDX, sTransferInfo() ) );
			it = rib.first;
		}

		CmUtil::CmBinRead::Read_Value( fp, &it->second.m_bEnable );
		CmUtil::CmBinRead::Read_Value( fp, it->second.m_serverName );

		int nLimitCount;
		CmUtil::CmBinRead::Read_Value( fp, &nLimitCount );
		for( int j = 0; j < nLimitCount; ++j )
		{
			sTransferLimit addInfo;
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_nLimitType );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.m_nLimitValue );
			it->second.m_LimitList.push_back( addInfo );
		}
	}

	fclose( fp );
	return true;
}

bool CsServerRelocateTBMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TRANSFER_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return false;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );

	for( int i=0; i<nCount; ++i )
	{
		int nServerIDX;
		CmUtil::CmPackRead::Read_Value( nHandle, &nServerIDX );

		MAP_SERVERTRANSFER_INFO_IT it = m_TransferInfo.find( nServerIDX );
		if( it == m_TransferInfo.end() )
		{
			auto rib = m_TransferInfo.insert( std::make_pair( nServerIDX, sTransferInfo() ) );
			it = rib.first;
		}

		CmUtil::CmPackRead::Read_Value( nHandle, &it->second.m_bEnable );
		CmUtil::CmPackRead::Read_Value( nHandle, it->second.m_serverName );

		int nLimitCount;
		CmUtil::CmPackRead::Read_Value( nHandle, &nLimitCount );
		for( int j = 0; j < nLimitCount; ++j )
		{
			sTransferLimit addInfo;
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_nLimitType );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.m_nLimitValue );
			it->second.m_LimitList.push_back( addInfo );
		}
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );

	return true;
}

void CsServerRelocateTBMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TRANSFER_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_TransferInfo.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );
	MAP_SERVERTRANSFER_INFO_CIT it = m_TransferInfo.begin();
	for( ; it != m_TransferInfo.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->first );
		CmUtil::CmBinSave::Save_Value( fp, &it->second.m_bEnable );
		CmUtil::CmBinSave::Save_Value( fp, it->second.m_serverName );

		int nLimitCount = (int)it->second.m_LimitList.size();
		CmUtil::CmBinSave::Save_Value( fp, &nLimitCount );
		LIST_TRANSFERLIMIT_INFO_CIT limitIT = it->second.m_LimitList.begin();
		for( ; limitIT != it->second.m_LimitList.end(); ++limitIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &(*limitIT).m_nLimitType );
			CmUtil::CmBinSave::Save_Value( fp, &(*limitIT).m_nLimitValue );
		}
	}

	fclose( fp );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

MAP_SERVERTRANSFER_INFO const&	CsServerRelocateTBMng::GetTransferInfo() const
{
	return m_TransferInfo;
}

UINT64 CsServerRelocateTBMng::GetLimitValueFromType( int const& nType, int const& nServerIDX ) const
{
	MAP_SERVERTRANSFER_INFO_CIT it = m_TransferInfo.find(nServerIDX);
	if( it == m_TransferInfo.end() )
		return 0;

	LIST_TRANSFERLIMIT_INFO_CIT limitIT = it->second.m_LimitList.begin();
	for( ; limitIT != it->second.m_LimitList.end(); ++limitIT )
	{
		if( (*limitIT).m_nLimitType == nType )
			return (*limitIT).m_nLimitValue;
	}

	return 0;
}

bool CsServerRelocateTBMng::IsServerRelocateEnable( int const& nServerIDX ) const
{
	MAP_SERVERTRANSFER_INFO_CIT it = m_TransferInfo.find(nServerIDX);
	if( it == m_TransferInfo.end() )
		return false;
	return (*it).second.m_bEnable;
}