#include "stdafx.h"
#include "ItemMng.h"

void CsItemMng::_LoadItemXAI_SystemTable( char* cExcel )
{
	//Option
	cExcelReader excel;	
	bool bSuccess = excel.Open( cExcel, "XAI_Data" );
	if( !bSuccess )
	{
		CsMessageBoxA( MB_OK, "%s 파일에 %s 씨트가 존재 하지 않습니다.", cExcel, "XAI_Data" );
		return;
	}

	// 2줄은 버린다.
	bSuccess = excel.FirstOfSkip(2);
	if( !bSuccess )
	{
		assert_cs( bSuccess == true );
		return;
	}

	while(excel.MoveNext())
	{
		DWORD dwItemCode = 0;
		excel.Read_Value( 0, dwItemCode );
		if( 0 == dwItemCode )
			break;

		TB::sXAIStruct addInfo;
		excel.Read_Value( 2, addInfo.s_unMaxXG );
		excel.Read_Value( 3, addInfo.s_ucMaxCrystal );
		auto rib = m_XaiItem_Data.insert( std::make_pair( dwItemCode, addInfo ) );
		if( !rib.second )
			CsMessageBoxA( MB_OK, "중복된 아이템이 있습니다. ItemCode : %d", dwItemCode );
	}

	excel.Close();
}

void CsItemMng::_SaveBin_ItemXAISystemTable( FILE* fp )
{
	int nCount = (int)m_XaiItem_Data.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	TB::MAP_XAIITEM_DATA_IT it = m_XaiItem_Data.begin();
	for( ; it!=m_XaiItem_Data.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(it->first) );
		CmUtil::CmBinSave::Save_Value( fp, &(it->second.s_unMaxXG) );
		CmUtil::CmBinSave::Save_Value( fp, &(it->second.s_ucMaxCrystal) );
	}
}


void CsItemMng::_LoadBin_ItemXAISystemTable( FILE* fp )
{
	int nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		DWORD dwItemCode = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwItemCode );
		if( 0 == dwItemCode )
			break;

		TB::sXAIStruct addInfo;
		CmUtil::CmBinRead::Read_Value( fp, &addInfo.s_unMaxXG );
		CmUtil::CmBinRead::Read_Value( fp, &addInfo.s_ucMaxCrystal );

		auto rib = m_XaiItem_Data.insert( std::make_pair( dwItemCode, addInfo ) );
		if( !rib.second )
			CsMessageBoxA( MB_OK, "중복된 아이템이 있습니다. ItemCode : %d", dwItemCode );
	}
}

void CsItemMng::_LoadFilePack_ItemXAISystemTable( int nHandle )
{
	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		DWORD dwItemCode = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwItemCode );

		TB::sXAIStruct addInfo;
		CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.s_unMaxXG );
		CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.s_ucMaxCrystal );

		auto rib = m_XaiItem_Data.insert( std::make_pair( dwItemCode, addInfo ) );
		if( !rib.second )
			CsMessageBoxA( MB_OK, "중복된 아이템이 있습니다. ItemCode : %d", dwItemCode );
	}
}
//////////////////////////////////////////////////////////////////////////

bool CsItemMng::IsXaiItem(DWORD const& dwItemCode) const
{
	TB::MAP_XAIITEM_DATA_CIT it = m_XaiItem_Data.find( dwItemCode );
	if( it == m_XaiItem_Data.end() )
		return false;
	return true;
}

TB::sXAIStruct const* CsItemMng::GetXaiItem_Info( DWORD const& dwItemCode ) const
{
	TB::MAP_XAIITEM_DATA_CIT it = m_XaiItem_Data.find( dwItemCode );
	if( it == m_XaiItem_Data.end() )
		return NULL;
	
	return &(it->second);
}