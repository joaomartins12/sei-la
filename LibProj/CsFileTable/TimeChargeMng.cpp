
#include "stdafx.h"
#include "TimeChargeMng.h"

CsTimeChargeMng::CsTimeChargeMng()
{

}

CsTimeChargeMng::~CsTimeChargeMng()
{

}

bool CsTimeChargeMng::Init(char* cPath)
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

void CsTimeChargeMng::Delete()
{
	m_mapChargeItem.clear();
	m_mapBitCode.clear();
	m_mapCashCode.clear();
}

void CsTimeChargeMng::_LoadExcel()
{
	cExcelReader excel;	
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Item_Charge(), "Charge_Item" );
	if( !bSuccess )
		return;

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
	{
		excel.Close();
		return;
	}

	while( excel.MoveNext() == true )
	{
		DWORD nItemCode = 0;
		excel.Read_Value( 0, nItemCode );

		if( 0 == nItemCode )
			break;

		sChargeItemInfo sChargeItem;
		excel.Read_Value( 2, sChargeItem.nBitCode );
		excel.Read_Value( 6, sChargeItem.nCashCode );

		m_mapChargeItem.insert( std::make_pair( nItemCode, sChargeItem ) );
	}

	excel.Close();

	//////////////////////////////////////////////////////////////////////////
	bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Item_Charge(), "Bit_Code" );
	if( !bSuccess )
		return;

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
	{
		excel.Close();
		return;
	}

	while( excel.MoveNext() == true )
	{
		int nBitCode = 0;
		excel.Read_Value( 0, nBitCode );

		if( 0 == nBitCode )
			break;

		sBitCodeInfo sBitCode;
		excel.Read_Value( 1, sBitCode.nKeepTime );
		excel.Read_Value( 2, sBitCode.nChargeTime );
		excel.Read_Value( 3, sBitCode.dwKeepPrice );

		m_mapBitCode.insert( std::make_pair( nBitCode, sBitCode ) );
	}

	excel.Close();

	//////////////////////////////////////////////////////////////////////////
	bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Item_Charge(), "Cash_Code" );
	if( !bSuccess )
		return;

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
	{
		excel.Close();
		return;
	}

	while( excel.MoveNext() == true )
	{
		int nCashCode = 0;
		excel.Read_Value( 0, nCashCode );

		if( 0 == nCashCode )
			break;

		MAP_NEEDINFO mapNeedCount;
		// B열( 1 )부터 읽음
		for( int i = 1; ; ++i )
		{
			int nNeedCount = 0;
			excel.Read_Value( i, nNeedCount );
			if( 0 == nNeedCount )
				break;

			mapNeedCount.insert( std::make_pair( i, nNeedCount ) );
		}
		m_mapCashCode.insert( std::make_pair( nCashCode, mapNeedCount ) );
	}

	excel.Close();
}

bool CsTimeChargeMng::_LoadBin(char* cPath)
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, "TimeCharge.bin" );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		//CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return true;
	}

	int nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i = 0; i < nCount; ++i )
	{
		DWORD dwItemCode = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwItemCode );
		if( 0 == dwItemCode )
			continue;

		MAP_CHARGEITEM_IT it = m_mapChargeItem.find( dwItemCode );
		if( m_mapChargeItem.end() == it )
		{
			auto ib = m_mapChargeItem.insert( std::make_pair( dwItemCode, sChargeItemInfo() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "ChargeItem insertion failure." ) );
				continue;
			}
			it = ib.first;
		}

		CmUtil::CmBinRead::Read_Value( fp, &it->second.nBitCode );
		CmUtil::CmBinRead::Read_Value( fp, &it->second.nCashCode );
	}

	//////////////////////////////////////////////////////////////////////////

	nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i = 0; i < nCount; ++i )
	{
		int nBitCode = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nBitCode );
		if( 0 == nBitCode )
			continue;

		MAP_BITCODE_IT it = m_mapBitCode.find( nBitCode );
		if( m_mapBitCode.end() == it )
		{
			auto ib = m_mapBitCode.insert( std::make_pair( nBitCode, sBitCodeInfo() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "BitCode insertion failure." ) );
				continue;
			}
			it = ib.first;
		}

		CmUtil::CmBinRead::Read_Value( fp, &it->second.nKeepTime );
		CmUtil::CmBinRead::Read_Value( fp, &it->second.nChargeTime );
		CmUtil::CmBinRead::Read_Value( fp, &it->second.dwKeepPrice );
	}

	//////////////////////////////////////////////////////////////////////////

	nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i = 0; i < nCount; ++i )
	{
		int nCashCode = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nCashCode );
		if( 0 == nCashCode )
			continue;

		MAP_CASHCODE_IT it = m_mapCashCode.find( nCashCode );
		if( m_mapCashCode.end() == it )
		{
			auto ib = m_mapCashCode.insert( std::make_pair( nCashCode, MAP_NEEDINFO() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "CashCode insertion failure." ) );
				continue;
			}
			it = ib.first;
		}

		int nCashCodeCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nCashCodeCount );
		for( int j = 0; j < nCashCodeCount; ++j )
		{
			int nTimeIndex = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nTimeIndex );
			int nNeedCount = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nNeedCount );
			it->second.insert( std::make_pair( nTimeIndex, nNeedCount ) );
		}
	}

	fclose( fp );

	return true;
}

void CsTimeChargeMng::_LoadFilePack(char* cPath)
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, "TimeCharge.bin" );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i = 0; i < nCount; ++i )
	{
		DWORD dwItemCode = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwItemCode );
		if( 0 == dwItemCode )
			continue;

		MAP_CHARGEITEM_IT it = m_mapChargeItem.find( dwItemCode );
		if( m_mapChargeItem.end() == it )
		{
			auto ib = m_mapChargeItem.insert( std::make_pair( dwItemCode, sChargeItemInfo() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "ChargeItem insertion failure." ) );
				continue;
			}
			it = ib.first;
		}

		CmUtil::CmPackRead::Read_Value( nHandle, &it->second.nBitCode );
		CmUtil::CmPackRead::Read_Value( nHandle, &it->second.nCashCode );
	}

	//////////////////////////////////////////////////////////////////////////

	nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i = 0; i < nCount; ++i )
	{
		int nBitCode = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nBitCode );
		if( 0 == nBitCode )
			continue;

		MAP_BITCODE_IT it = m_mapBitCode.find( nBitCode );
		if( m_mapBitCode.end() == it )
		{
			auto ib = m_mapBitCode.insert( std::make_pair( nBitCode, sBitCodeInfo() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "BitCode insertion failure." ) );
				continue;
			}
			it = ib.first;
		}

		CmUtil::CmPackRead::Read_Value( nHandle, &it->second.nKeepTime );
		CmUtil::CmPackRead::Read_Value( nHandle, &it->second.nChargeTime );
		CmUtil::CmPackRead::Read_Value( nHandle, &it->second.dwKeepPrice );
	}

	//////////////////////////////////////////////////////////////////////////

	nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i = 0; i < nCount; ++i )
	{
		int nCashCode = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCashCode );
		if( 0 == nCashCode )
			continue;

		MAP_CASHCODE_IT it = m_mapCashCode.find( nCashCode );
		if( m_mapCashCode.end() == it )
		{
			auto ib = m_mapCashCode.insert( std::make_pair( nCashCode, MAP_NEEDINFO() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "CashCode insertion failure." ) );
				continue;
			}
			it = ib.first;
		}

		int nCashCodeCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCashCodeCount );
		for( int j = 0; j < nCashCodeCount; ++j )
		{
			int nTimeIndex = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nTimeIndex );
			int nNeedCount = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nNeedCount );
			it->second.insert( std::make_pair( nTimeIndex, nNeedCount ) );
		}
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

void CsTimeChargeMng::SaveBin(char* cPath)
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, "TimeCharge.bin" );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return;
	}

	int nCount = static_cast<int>( m_mapChargeItem.size() );
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	MAP_CHARGEITEM_IT it = m_mapChargeItem.begin();
	MAP_CHARGEITEM_IT itEnd = m_mapChargeItem.end();
	for( ; it != itEnd; ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->first );
		CmUtil::CmBinSave::Save_Value( fp, &it->second.nBitCode );
		CmUtil::CmBinSave::Save_Value( fp, &it->second.nCashCode );
	}

	//////////////////////////////////////////////////////////////////////////

	nCount = static_cast<int>( m_mapBitCode.size() );
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	MAP_BITCODE_IT itBit = m_mapBitCode.begin();
	MAP_BITCODE_IT itBitEnd = m_mapBitCode.end();
	for( ; itBit != itBitEnd; ++itBit )
	{
		CmUtil::CmBinSave::Save_Value( fp, &itBit->first );
		CmUtil::CmBinSave::Save_Value( fp, &itBit->second.nKeepTime );
		CmUtil::CmBinSave::Save_Value( fp, &itBit->second.nChargeTime );
		CmUtil::CmBinSave::Save_Value( fp, &itBit->second.dwKeepPrice );
	}

	//////////////////////////////////////////////////////////////////////////

	nCount = static_cast<int>( m_mapCashCode.size() );
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	MAP_CASHCODE_IT itCash = m_mapCashCode.begin();
	MAP_CASHCODE_IT itCashEnd = m_mapCashCode.end();
	for( ; itCash != itCashEnd; ++itCash )
	{
		CmUtil::CmBinSave::Save_Value( fp, &itCash->first );

		int nNeedCount = static_cast< int >( itCash->second.size() );
		CmUtil::CmBinSave::Save_Value( fp, &nNeedCount );

		MAP_NEEDINFO_IT itNeed = itCash->second.begin();
		MAP_NEEDINFO_IT itNeedEnd = itCash->second.end();
		for( ; itNeed != itNeedEnd; ++itNeed )
		{
			CmUtil::CmBinSave::Save_Value( fp, &itNeed->first );
			CmUtil::CmBinSave::Save_Value( fp, &itNeed->second );
		}
	}

	fclose( fp );
}

sChargeItemInfo const* CsTimeChargeMng::GetChargeItemInfo(DWORD dwItemCode) const
{
	MAP_CHARGEITEM_CIT cit = m_mapChargeItem.find( dwItemCode );
	if( m_mapChargeItem.end() == cit )
		return NULL;

	return &(cit->second);
}

sBitCodeInfo const* CsTimeChargeMng::GetBitCodeInfo(int nBitCode) const
{
	MAP_BITCODE_CIT cit = m_mapBitCode.find( nBitCode );
	if( m_mapBitCode.end() == cit )
		return NULL;

	return &(cit->second);
}

sBitCodeInfo const* CsTimeChargeMng::GetBitCodeInfo(DWORD dwItemCode) const
{
	sChargeItemInfo const* pCInfo = GetChargeItemInfo( dwItemCode );
	SAFE_POINTER_RETVAL( pCInfo, NULL );

	return GetBitCodeInfo( pCInfo->nBitCode );
}

int CsTimeChargeMng::GetCashCodeCount(DWORD dwItemCode) const
{
	sChargeItemInfo const* pCInfo = GetChargeItemInfo( dwItemCode );
	SAFE_POINTER_RETVAL( pCInfo, 0 );

	MAP_CASHCODE_CIT cit = m_mapCashCode.find( pCInfo->nCashCode );
	if( m_mapCashCode.end() == cit )
		return 0;

	return cit->second.size();
}

int CsTimeChargeMng::GetCashNeedCount(int nCashCode, int nTimeIndex) const
{
	MAP_CASHCODE_CIT cit = m_mapCashCode.find( nCashCode );
	if( m_mapCashCode.end() == cit )
		return 0;

	MAP_NEEDINFO_CIT citNeed = cit->second.find( nTimeIndex );
	if( cit->second.end() == citNeed )
		return 0;

	return citNeed->second;
}

DWORD CsTimeChargeMng::GetUnlockMoney(int nSlotIndex) const
{
	if( 0 == nSlotIndex )
		return 0;

	DWORD nBaseMoney = 3700000;
	for( int i = 1; i < nSlotIndex; ++i )
	{
		int nCeil = nBaseMoney * 0.9f;
		if( 0 != nCeil % 100000 )
		{
			nCeil -= nCeil % 100000;
			nCeil += 100000;
		}
		nBaseMoney += nCeil;
	}

	return nBaseMoney;
}

bool CsTimeChargeMng::IsEnableChargeItem(DWORD dwItemCode) const
{
	MAP_CHARGEITEM_CIT cit = m_mapChargeItem.find( dwItemCode );
	if( m_mapChargeItem.end() == cit )
		return false;

	return true;
}
