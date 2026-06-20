
#include "stdafx.h"
#include "ItemMng.h"

#define IL_COL_NPC_IDX					0
#define IL_COL_UNIQUE_ID				1
#define IL_COL_MAIN_CATEGORY			2
#define IL_COL_MAIN_CATEGORY_NAME		3
#define IL_COL_MAIN_CATEGORY_NAME_ENG	4

#define IL_COL_SUB_CATEGORY				5
#define IL_COL_SUB_CATEGORY_NAME		6
#define IL_COL_SUB_CATEGORY_NAME_ENG	7

#define IL_COL_PRODUCTION_ITEM_IDX      8
#define IL_COL_PRODUCTION_ITEM_COUNT    10
#define IL_COL_PRODUCTION_ITEM_SUCCESSPRECENT    11

#define IL_COL_PRODUCTION_PERCENTAGE_ASSIST	    12
#define IL_COL_PRODUCTION_PROTECT_ASSIST	   13

#define IL_COL_PRODUCTION_COST			14
#define IL_COL_NEED_MATERIAL_ITEM_IDX	15
#define IL_COL_NEED_MATERIAL_ITEM_COUNT	17

#define MAC_MATERIAL_ITEM_COUNT			10

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool sProductionItemList::LoadTable( cExcelReader& pExcelReader, int nCount )
{
	sMaterialItemList addMaterialItem;	
	pExcelReader.Read_Value( IL_COL_NEED_MATERIAL_ITEM_IDX + (nCount * 3), addMaterialItem.m_dwItemIdx );
	if( 0 == addMaterialItem.m_dwItemIdx )
		return false;

	pExcelReader.Read_Value( IL_COL_NEED_MATERIAL_ITEM_COUNT + (nCount * 3), addMaterialItem.m_nItemNum );
	m_MaterialList.push_back( addMaterialItem );
	return true;
}

void sProductionItemList::_SaveBin(FILE* fp)
{
	CmUtil::CmBinSave::Save_Value( fp, &m_nUniqueIdx );// 제작 고유 번호
	CmUtil::CmBinSave::Save_Value( fp, &m_dwItemIdx );// 제작할 아이템 인덱스 번호
	CmUtil::CmBinSave::Save_Value( fp, &m_nItemNum );// 제작하는 아이템의 갯수
	CmUtil::CmBinSave::Save_Value( fp, &m_nProbabilityofSuccess );// 성공확률
	CmUtil::CmBinSave::Save_Value( fp, &m_nPercentageAssistItemGroup );// 제작 확률 상승 아이템 그룹 번호
	CmUtil::CmBinSave::Save_Value( fp, &m_nProtectAssistItemGroup );// 제작 아이템 보호 아이템 그룹 번호
	CmUtil::CmBinSave::Save_Value( fp, &m_dwItemCost );// 제작 비용
	
	int nMaterialSize = (int)m_MaterialList.size();
	CmUtil::CmBinSave::Save_Value( fp, &nMaterialSize ); // 재료 아이템 목록 갯수

	LIST_MaterialList_IT it = m_MaterialList.begin();	// 재료 아이템 목록
	for( ; it != m_MaterialList.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(*it).m_dwItemIdx );// 재료 아이템 인덱스 번호
		CmUtil::CmBinSave::Save_Value( fp, &(*it).m_nItemNum );// 필요한 아이템의 갯수
	}

}

void sProductionItemList::_LoadBin(FILE* fp)
{
	CmUtil::CmBinRead::Read_Value( fp, &m_nUniqueIdx );				// 제작 고유 번호
	CmUtil::CmBinRead::Read_Value( fp, &m_dwItemIdx );				// 제작할 아이템 인덱스 번호
	CmUtil::CmBinRead::Read_Value( fp, &m_nItemNum );				// 제작하는 아이템의 갯수
	CmUtil::CmBinRead::Read_Value( fp, &m_nProbabilityofSuccess );	// 성공확률
	CmUtil::CmBinRead::Read_Value( fp, &m_nPercentageAssistItemGroup );	// 제작 확률 상승 아이템 그룹 번호
	CmUtil::CmBinRead::Read_Value( fp, &m_nProtectAssistItemGroup );	// 제작 아이템 보호 아이템 그룹 번호
	CmUtil::CmBinRead::Read_Value( fp, &m_dwItemCost );				// 제작 비용

	int nMaterialSize = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nMaterialSize ); // 재료 아이템 목록 갯수
	for( int n = 0; n < nMaterialSize; ++n )
	{
		sMaterialItemList addItem;
		CmUtil::CmBinRead::Read_Value( fp, &addItem.m_dwItemIdx );	// 재료 아이템 인덱스 번호
		CmUtil::CmBinRead::Read_Value( fp, &addItem.m_nItemNum );	// 필요한 아이템의 갯수
		m_MaterialList.push_back( addItem );
	}
}

void sProductionItemList::_LoadFilePack(int nHandle)
{
	CmUtil::CmPackRead::Read_Value( nHandle, &m_nUniqueIdx );			// 제작 고유 번호
	CmUtil::CmPackRead::Read_Value( nHandle, &m_dwItemIdx );			// 제작할 아이템 인덱스 번호
	CmUtil::CmPackRead::Read_Value( nHandle, &m_nItemNum );				// 제작하는 아이템의 갯수
	CmUtil::CmPackRead::Read_Value( nHandle, &m_nProbabilityofSuccess );// 제작 성공 확률
	CmUtil::CmPackRead::Read_Value( nHandle, &m_nPercentageAssistItemGroup );	// 제작 확률 상승 아이템 그룹 번호
	CmUtil::CmPackRead::Read_Value( nHandle, &m_nProtectAssistItemGroup );	// 제작 아이템 보호 아이템 그룹 번호
	CmUtil::CmPackRead::Read_Value( nHandle, &m_dwItemCost );			// 제작 비용
	
	int nMaterialSize = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nMaterialSize );		// 재료 아이템 목록 갯수
	for( int n = 0; n < nMaterialSize; ++n )
	{
		sMaterialItemList addItem;
		CmUtil::CmPackRead::Read_Value( nHandle, &addItem.m_dwItemIdx );// 재료 아이템 인덱스 번호
		CmUtil::CmPackRead::Read_Value( nHandle,  &addItem.m_nItemNum );// 필요한 아이템의 갯수
		m_MaterialList.push_back( addItem );
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void sSubCategoty::LoadTable( cExcelReader& pExcelReader )
{
	sProductionItemList addItem;
	pExcelReader.Read_Value( IL_COL_PRODUCTION_ITEM_IDX, addItem.m_dwItemIdx );
	pExcelReader.Read_Value( IL_COL_UNIQUE_ID, addItem.m_nUniqueIdx );
	pExcelReader.Read_Value( IL_COL_PRODUCTION_ITEM_COUNT, addItem.m_nItemNum );
	pExcelReader.Read_Value( IL_COL_PRODUCTION_ITEM_SUCCESSPRECENT, addItem.m_nProbabilityofSuccess );
	pExcelReader.Read_Value( IL_COL_PRODUCTION_PERCENTAGE_ASSIST, addItem.m_nPercentageAssistItemGroup);
	pExcelReader.Read_Value( IL_COL_PRODUCTION_PROTECT_ASSIST, addItem.m_nProtectAssistItemGroup );
	pExcelReader.Read_Value( IL_COL_PRODUCTION_COST, addItem.m_dwItemCost );
	pExcelReader.Read_Value( IL_COL_PRODUCTION_COST, addItem.m_dwItemCost );
	
	addItem.m_MaterialList.clear();
	for( int n = 0; n < MAC_MATERIAL_ITEM_COUNT; ++n )
	{
		if( !addItem.LoadTable( pExcelReader, n ) )
			break;
	}

	m_listProductionItems.push_back( addItem );
}

void sSubCategoty::_SaveBin(FILE* fp)
{
	CmUtil::CmBinSave::Save_Value( fp, &m_nCategotyType );
	CmUtil::CmBinSave::Save_Value( fp, m_wsCategotyName );

	int nproductionItems = (int)m_listProductionItems.size();
	CmUtil::CmBinSave::Save_Value( fp, &nproductionItems );

	LIST_ProductionList_IT it = m_listProductionItems.begin();
	for( ; it != m_listProductionItems.end(); ++it )
		(*it)._SaveBin( fp );
}

void sSubCategoty::_LoadBin(FILE* fp)
{
	CmUtil::CmBinRead::Read_Value( fp, &m_nCategotyType );
	CmUtil::CmBinRead::Read_Value( fp, m_wsCategotyName );	

	int nproductionItems = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nproductionItems );	
	for( int n = 0; n < nproductionItems; ++n )
	{
		sProductionItemList addItem;
		addItem._LoadBin( fp );
		m_listProductionItems.push_back( addItem );
	}		
}

void sSubCategoty::_LoadFilePack(int nHandle)
{
	CmUtil::CmPackRead::Read_Value( nHandle, &m_nCategotyType );
	CmUtil::CmPackRead::Read_Value( nHandle, m_wsCategotyName );
	
	int nproductionItems = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nproductionItems );
	for( int n = 0; n < nproductionItems; ++n )
	{
		sProductionItemList addItem;
		addItem._LoadFilePack( nHandle );
		m_listProductionItems.push_back( addItem );
	}	
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void sMainCategoty::LoadTable( cExcelReader& pExcelReader )
{
	int nSubCategoryIdx = 0;
	pExcelReader.Read_Value( IL_COL_SUB_CATEGORY, nSubCategoryIdx );
	MAP_Sub_Categoty_IT it = m_mapSubCategoty.find( nSubCategoryIdx );
	if( it == m_mapSubCategoty.end() )
	{
		auto Result = m_mapSubCategoty.insert( make_pair( nSubCategoryIdx, sSubCategoty(nSubCategoryIdx) ) );
		it = Result.first;

		switch( nsCsFileTable::g_eLanguage )	// name
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			pExcelReader.Read_Value( IL_COL_SUB_CATEGORY_NAME, it->second.m_wsCategotyName );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			pExcelReader.Read_Value( IL_COL_SUB_CATEGORY_NAME_ENG, it->second.m_wsCategotyName );
			break;
		default:
			assert_cs( false );
		}		
	}
	it->second.LoadTable( pExcelReader );
}

void sMainCategoty::_LoadBin(FILE* fp)
{
	CmUtil::CmBinRead::Read_Value( fp, &m_nCategotyType );
	CmUtil::CmBinRead::Read_Value( fp, m_wsCategotyName );

	int subCategotySize = 0;
	CmUtil::CmBinRead::Read_Value( fp, &subCategotySize );
	for( int n = 0; n < subCategotySize; ++n )
	{
		int nSubCategotyType = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nSubCategotyType );
		MAP_Sub_Categoty_IT it = m_mapSubCategoty.find( nSubCategotyType );
		if( it == m_mapSubCategoty.end() )
		{
			auto Result = m_mapSubCategoty.insert( make_pair( nSubCategotyType, sSubCategoty(nSubCategotyType) ) );
			it = Result.first;
		}

		it->second._LoadBin( fp );
	}		
}

void sMainCategoty::_LoadFilePack(int nHandle)
{
	CmUtil::CmPackRead::Read_Value( nHandle, &m_nCategotyType );
	CmUtil::CmPackRead::Read_Value( nHandle, m_wsCategotyName );

	int subCategotySize = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &subCategotySize );
	for( int n = 0; n < subCategotySize; ++n )
	{
		int nSubCategotyType = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nSubCategotyType );
		MAP_Sub_Categoty_IT it = m_mapSubCategoty.find( nSubCategotyType );
		if( it == m_mapSubCategoty.end() )
		{
			auto Result = m_mapSubCategoty.insert( make_pair( nSubCategotyType, sSubCategoty(nSubCategotyType) ) );
			it = Result.first;
		}

		it->second._LoadFilePack( nHandle );
	}	
}

void sMainCategoty::_SaveBin(FILE* fp)
{
	CmUtil::CmBinSave::Save_Value( fp, &m_nCategotyType );
	CmUtil::CmBinSave::Save_Value( fp, m_wsCategotyName );
	
	int subCategotySize = (int)m_mapSubCategoty.size();
	CmUtil::CmBinSave::Save_Value( fp, &subCategotySize );

	MAP_Sub_Categoty_IT it = m_mapSubCategoty.begin();
	for( ; it != m_mapSubCategoty.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(it->first) );
		it->second._SaveBin( fp );
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void sItemProductionNpcTable::LoadTable( cExcelReader& pExcelReader )
{
	// Main Categoty Type
	int nMainCategoryIdx = 0;
	pExcelReader.Read_Value( IL_COL_MAIN_CATEGORY, nMainCategoryIdx );
	
	MAP_Main_Categoty_IT it = m_mapMainCategoty.find( nMainCategoryIdx );
	if( it == m_mapMainCategoty.end() )
	{
		auto Result = m_mapMainCategoty.insert( make_pair( nMainCategoryIdx, sMainCategoty(nMainCategoryIdx) ) );
		it = Result.first;

		switch( nsCsFileTable::g_eLanguage )	// name
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			pExcelReader.Read_Value( IL_COL_MAIN_CATEGORY_NAME, it->second.m_wsCategotyName );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			pExcelReader.Read_Value( IL_COL_MAIN_CATEGORY_NAME_ENG, it->second.m_wsCategotyName );
			break;
		default:
			assert_cs( false );
		}		
	}	
	it->second.LoadTable( pExcelReader );
}

void sItemProductionNpcTable::_SaveBin(FILE* fp)
{
	int nCategotySize = (int)m_mapMainCategoty.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCategotySize );

	MAP_Main_Categoty_IT it = m_mapMainCategoty.begin();
	for( ; it != m_mapMainCategoty.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &(it->first) );
		it->second._SaveBin( fp );
	}
}

void sItemProductionNpcTable::_LoadBin(FILE* fp)
{
	int nCategotySize = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCategotySize );
	for( int n = 0; n < nCategotySize; ++n )
	{
		int nMainCategotyType = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nMainCategotyType );
		MAP_Main_Categoty_IT it = m_mapMainCategoty.find( nMainCategotyType );
		if( it == m_mapMainCategoty.end() )
		{
			auto Result = m_mapMainCategoty.insert( make_pair( nMainCategotyType, sMainCategoty(nMainCategotyType) ) );
			it = Result.first;
		}
		it->second._LoadBin( fp );
	}
}

void sItemProductionNpcTable::_LoadFilePack(int nHandle)
{
	int nCategotySize = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCategotySize );

	for( int n = 0; n < nCategotySize; ++n )
	{
		int nMainCategotyType = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nMainCategotyType );
		MAP_Main_Categoty_IT it = m_mapMainCategoty.find( nMainCategotyType );
		if( it == m_mapMainCategoty.end() )
		{
			auto Result = m_mapMainCategoty.insert( make_pair( nMainCategotyType, sMainCategoty(nMainCategotyType) ) );
			it = Result.first;
		}
		it->second._LoadFilePack( nHandle );
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CsItemMng::_LoadItemProductionTable( char* cExcel )
{
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );		
		return;
	}

	cExcelReader excel;		
	bool bSuccess = excel.Open( cExcel, "Item_Make" );
	if( !bSuccess )
	{
		CsMessageBoxA( MB_OK, "%s 파일에 %s 씨트가 존재 하지 않습니다.", cExcel, "Item_Make" );
		return;
	}

	excel.FirstOfSkip(3);

	while(excel.MoveNext() == true)
	{
		long	nNpcIdx = 0;
		excel.Read_Value( IL_COL_NPC_IDX, nNpcIdx );
		if( 0 == nNpcIdx )
			continue;

		STL_MAP_ITEMPRODUCTION_IT it = m_mapProductionItem.find( nNpcIdx );
		if( m_mapProductionItem.end() == it )
		{
			auto Result = m_mapProductionItem.insert( make_pair( nNpcIdx, sItemProductionNpcTable(nNpcIdx) ) );
			if( !Result.second )
				break;
			it = Result.first;
		}

		it->second.LoadTable( excel );
	}

	excel.Close();
}

void CsItemMng::_SaveBin_ItemProductionTable( FILE* fp )
{
	int nCount = (int)m_mapProductionItem.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	STL_MAP_ITEMPRODUCTION_IT it = m_mapProductionItem.begin();
	for( ; it!=m_mapProductionItem.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->second.m_dwNpcIdx );
		it->second._SaveBin( fp );
	}
}

void CsItemMng::_LoadBin_ItemProductionTable( FILE* fp )
{
	int nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		DWORD dwNpcIdx = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwNpcIdx );
		if( 0 == dwNpcIdx )
			continue;

		STL_MAP_ITEMPRODUCTION_IT it = m_mapProductionItem.find( dwNpcIdx );
		if( m_mapProductionItem.end() == it )
		{
			auto Result = m_mapProductionItem.insert( make_pair( dwNpcIdx, sItemProductionNpcTable(dwNpcIdx) ) );
			if( !Result.second )
				break;
			it = Result.first;
		}

		it->second._LoadBin( fp );
	}
}

void CsItemMng::_LoadFilePack_ItemProductionTable( int nHandle )
{
	int nCount;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		DWORD dwNpcIdx = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwNpcIdx );
		if( 0 == dwNpcIdx )
			continue;

		STL_MAP_ITEMPRODUCTION_IT it = m_mapProductionItem.find( dwNpcIdx );
		if( m_mapProductionItem.end() == it )
		{
			auto Result = m_mapProductionItem.insert( make_pair( dwNpcIdx, sItemProductionNpcTable(dwNpcIdx) ) );
			if( !Result.second )
				break;
			it = Result.first;
		}

		it->second._LoadFilePack( nHandle );
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

MAP_Main_Categoty const* CsItemMng::GetItemProductionCategotyTable( DWORD const& dwNpcIdx ) const
{
	STL_MAP_ITEMPRODUCTION_CIT it = m_mapProductionItem.find( dwNpcIdx );
	if( it == m_mapProductionItem.end() )
		return NULL;

	return &it->second.m_mapMainCategoty;
}

STL_MAP_ITEMPRODUCTION const* CsItemMng::GetItemProductionTable() const
{
	return &m_mapProductionItem;
}
//////////////////////////////////////////////////////////////////////////

void CsItemMng::_LoadItemProductionAssistTable( char* cExcel )
{
	cExcelReader excel;	
	bool bSuccess = excel.Open( cExcel, "Item_Make_Group" );
	if( bSuccess )
	{
		excel.FirstOfSkip(3);
		while(excel.MoveNext())
		{
			int	nIdx = 0;
			excel.Read_Value( 0, nIdx );
			if( 0 == nIdx )
				break;

			MAP_AssistItemGroup_IT it = m_mapAssistItemGroup.find( nIdx );
			if( it != m_mapAssistItemGroup.end() )
			{
				CsMessageBoxA( MB_OK, "Item_Make_Assist의 같은 그룹 번호가 존재합니다. Group ID : %d", nIdx );
				continue;
			}
			else
			{
				auto rib = m_mapAssistItemGroup.insert( std::make_pair( nIdx, sAssistanceItemGroup() ) );
				it = rib.first;
			}	


			excel.Read_Value( 1, it->second.nAssistType );
			int nCount = 0;
			while(1)
			{
				sAssistanceItem itemInfo;
				excel.Read_Value( 2 + (nCount * 3), itemInfo.dwItemCode);
				if( 0 == itemInfo.dwItemCode )
					break;

				excel.Read_Value( 4 + (nCount * 3), itemInfo.nSuccessPercentage);
				it->second.assisteItemList.push_back( itemInfo );
				nCount++;
			}			
		}
		excel.Close();
	}
}

void CsItemMng::_SaveBin_AssistItemGroupTable( FILE* fp )
{
	int nAssistSize = (int)m_mapAssistItemGroup.size();
	CmUtil::CmBinSave::Save_Value( fp, &nAssistSize );
	MAP_AssistItemGroup_CIT it = m_mapAssistItemGroup.begin();
	for( ; it != m_mapAssistItemGroup.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->first );
		CmUtil::CmBinSave::Save_Value( fp, &it->second.nAssistType );
		int nListSize = (int)it->second.assisteItemList.size();
		CmUtil::CmBinSave::Save_Value( fp, &nListSize );
		LIST_AssistItemList_CIT subIT = it->second.assisteItemList.begin();
		for( ; subIT != it->second.assisteItemList.end(); ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).dwItemCode );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).nSuccessPercentage );			
		}
	}	
}

void CsItemMng::_LoadBin_AssistItemGroupTable( FILE* fp )
{
	int nAssistSize = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nAssistSize );
	for( int n = 0; n < nAssistSize; ++n )
	{
		int nIdx = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nIdx );
		MAP_AssistItemGroup_IT it = m_mapAssistItemGroup.find( nIdx );
		if( it != m_mapAssistItemGroup.end() )
			continue;

		auto rib = m_mapAssistItemGroup.insert( std::make_pair( nIdx, sAssistanceItemGroup() ) );
		it = rib.first;

		CmUtil::CmBinRead::Read_Value( fp, &it->second.nAssistType );

		int nItemSize = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nItemSize );

		for( int m = 0; m < nItemSize; ++m )
		{
			sAssistanceItem addItem;
			CmUtil::CmBinRead::Read_Value( fp, &addItem.dwItemCode );
			CmUtil::CmBinRead::Read_Value( fp, &addItem.nSuccessPercentage );
			it->second.assisteItemList.push_back( addItem );
		}
	}
}

void CsItemMng::_LoadFilePack_AssistItemGroupTable( int nHandle )
{
	int nAssistSize = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nAssistSize );
	for( int n = 0; n < nAssistSize; ++n )
	{
		int nIdx = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nIdx );
		MAP_AssistItemGroup_IT it = m_mapAssistItemGroup.find( nIdx );
		if( it != m_mapAssistItemGroup.end() )
			continue;

		auto rib = m_mapAssistItemGroup.insert( std::make_pair( nIdx, sAssistanceItemGroup() ) );
		it = rib.first;

		CmUtil::CmPackRead::Read_Value( nHandle, &it->second.nAssistType );

		int nItemSize = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nItemSize );

		for( int m = 0; m < nItemSize; ++m )
		{
			sAssistanceItem addItem;
			CmUtil::CmPackRead::Read_Value( nHandle, &addItem.dwItemCode );
			CmUtil::CmPackRead::Read_Value( nHandle, &addItem.nSuccessPercentage );
			it->second.assisteItemList.push_back( addItem );
		}
	}
}

sAssistanceItemGroup const* CsItemMng::GetAssistItemGroup(int const& nGroupID) const
{
	MAP_AssistItemGroup_CIT it = m_mapAssistItemGroup.find(nGroupID);
	if( it == m_mapAssistItemGroup.end() )
		return NULL;

	return &(it->second);
}

LIST_AssistItemList const* CsItemMng::GetAssistGroupItemList(int const& nGroupID) const
{
	MAP_AssistItemGroup_CIT it = m_mapAssistItemGroup.find(nGroupID);
	if( it == m_mapAssistItemGroup.end() )
		return NULL;
	
	return &(it->second.assisteItemList);
}