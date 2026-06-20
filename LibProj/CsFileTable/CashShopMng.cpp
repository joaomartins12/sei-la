
#include "stdafx.h"
#include "CashShopMng.h"

CsCashShopMng::CsCashShopMng()
{

}

CsCashShopMng::~CsCashShopMng()
{

}


void CsCashShopMng::Delete()
{

}

bool CsCashShopMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:	
		{
			switch( nsCsFileTable::g_eLanguage )
			{
			case nsCsFileTable::ENGLISH:
				_Load_Excel(nsCsFileTable::GetPath_DM_CashShop(), eDefault);
				_Load_Excel(nsCsFileTable::GetPath_DM_CashShop_Steam(), eSteam);
				return true;
			default:
				_Load_Excel(nsCsFileTable::GetPath_DM_CashShop(), eDefault);
				return true;
			}
		}		
		break;
	case nsCsFileTable::FT_BIN:			return	_Load_Bin( cPath );
	case nsCsFileTable::FT_FILEPACK:	return _Load_Pack( cPath );
	}

	return false;
}

bool CsCashShopMng::_Load_Excel(char const* pFilePath, int const& nTableType)
{
	cExcelReader excel;
	std::map<int, MAP_CASHSHOP_CATEGORY>::iterator itMap = m_mapCashshopData.find( nTableType );
	if( itMap == m_mapCashshopData.end() )
	{
		auto rib = m_mapCashshopData.insert( std::make_pair( nTableType, MAP_CASHSHOP_CATEGORY() ) );
		itMap = rib.first;
	}

	{
		bool bSuccess = excel.Open( pFilePath, "Category" );
		if( !bSuccess )
			return false;

		if( !excel.FirstOfSkip(2) )
			return false;

		// 대분류
		while( excel.MoveNext() )
		{
			int ntype;
			excel.Read_Value( 0, ntype );
			if( 0 == ntype )
				break;

			sCashshop_MainCategory mainC;
			excel.Read_Value(2, mainC.wsCategoryName );
			itMap->second.insert( std::make_pair( ntype, mainC ) );
		}

		// 소분류
		MAP_CASHSHOP_CATEGORY_IT it = itMap->second.begin();
		for( int n = 0; it != itMap->second.end(); ++it, ++n )
		{
			excel.FirstOfSkip(2);

			while( excel.MoveNext() )
			{
				int nCol = (n * 3) + 4;
				int ntype = 0;
				excel.Read_Value( nCol, ntype );
				if( 0 == ntype )
					break;

				sCashshop_SubCategory addItem;
				excel.Read_Value(nCol + 1, addItem.wsCategoryName );
				it->second.mapSubCategoryInfo.insert( std::make_pair( ntype, addItem ) );
			}
		}	

		excel.Close();
	}
	{
		bool bSuccess = excel.Open( pFilePath, "Main" );
		if( !bSuccess )
			return false;

		if( !excel.FirstOfSkip(2) )
			return false;

		MAP_CASHSHOP_CATEGORY_IT it = itMap->second.begin();
		if( it != itMap->second.end() )
		{
			while( excel.MoveNext() )
			{
				int ntype = 0;
				excel.Read_Value( 4, ntype );
				if( 0 == ntype )
					break;

				DWORD dwProdcutIdx = 0;
				excel.Read_Value( 2, dwProdcutIdx );
				if( 0 == dwProdcutIdx )
					continue;

				MAP_CASHSHOP_SUBCATEGORY_IT subIT = it->second.mapSubCategoryInfo.find( ntype );
				if( subIT == it->second.mapSubCategoryInfo.end() )
				{
					auto rib = it->second.mapSubCategoryInfo.insert( std::make_pair( ntype, sCashshop_SubCategory() ) );
					subIT = rib.first;
				}
				
				LIST_CASHSHOP_PRODUCT_GROUP_IT secIT = subIT->second.listProductData.begin();
				for( ; secIT != subIT->second.listProductData.end(); ++secIT )
				{
					if( dwProdcutIdx == (*secIT).dwItemGroupID )
						break;
				}

				if( secIT == subIT->second.listProductData.end() )
					subIT->second.listProductData.push_back( sCashShop_ProductGroup(dwProdcutIdx) );
			}
		}

		
		excel.Close();
	}

	{

		bool bSuccess = excel.Open( pFilePath, "Best" );
		if( !bSuccess )
			return false;

		if( !excel.FirstOfSkip(2) )
			return false;

		MAP_CASHSHOP_CATEGORY_IT it = itMap->second.begin();
		if( it != itMap->second.end() )
		{
			MAP_CASHSHOP_SUBCATEGORY_IT subIT = it->second.mapSubCategoryInfo.find( neCashsopHitType::Best );
			if( subIT == it->second.mapSubCategoryInfo.end() )
			{
				auto rib = it->second.mapSubCategoryInfo.insert( std::make_pair( neCashsopHitType::Best, sCashshop_SubCategory() ) );
				subIT = rib.first;
			}

			while( excel.MoveNext() )
			{
				DWORD dwProdcutIdx = 0;
				excel.Read_Value( 2, dwProdcutIdx );
				if( 0 == dwProdcutIdx )
					break;

				LIST_CASHSHOP_PRODUCT_GROUP_IT secIT = subIT->second.listProductData.begin();
				for( ; secIT != subIT->second.listProductData.end(); ++secIT )
				{
					if( dwProdcutIdx == (*secIT).dwItemGroupID )
						break;
				}

				if( secIT == subIT->second.listProductData.end() )
					subIT->second.listProductData.push_back( sCashShop_ProductGroup(dwProdcutIdx) );
			}
		}
		excel.Close();
	}

	_Load_Excel_CashshopItems(pFilePath,"Tamer",itMap->second);
	_Load_Excel_CashshopItems(pFilePath,"Digimon",itMap->second);
	_Load_Excel_CashshopItems(pFilePath,"Avatar",itMap->second);
	_Load_Excel_CashshopItems(pFilePath,"Packages",itMap->second);


	std::map<int, LIST_CASHSHOP_WEBDATA>::iterator itWeb = m_mapWebData.find( nTableType );
	if( itWeb == m_mapWebData.end() )
	{
		auto rib = m_mapWebData.insert( std::make_pair( nTableType, LIST_CASHSHOP_WEBDATA() ) );
		itWeb = rib.first;
	}

	_Load_Excel_WebData( pFilePath, "Weblink", itWeb->second );
	return true;
}

void CsCashShopMng::_Load_Excel_CashshopItems( char const* cFilePath, char const* cSheetName, MAP_CASHSHOP_CATEGORY & insertCategory )
{
	cExcelReader excel;
	bool bSuccess = excel.Open( cFilePath, cSheetName );
	if( !bSuccess )
		return;

	if( !excel.FirstOfSkip(2) )
		return;

	while( excel.MoveNext() )
	{
// 		bool bIsActive = false;
// 		excel.Read_Value( 6, bIsActive );// 표시여부
// 		if( !bIsActive )
// 			continue;

		DWORD dwGroupID = 0;
		excel.Read_Value( 3, dwGroupID );
		if( 0 == dwGroupID )
			break;

		sCashshop_Item addItem;
		excel.Read_Value( 2, addItem.dwProductID );
		if( 0 == addItem.dwProductID )
			break;

		excel.Read_Value( 1, addItem.nPurchaseCashType );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:	excel.Read_Value( 4, addItem.wsName );		break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:			excel.Read_Value( 5, addItem.wsName );		break;
			break;
		default:
			assert_cs( false );
		}

		excel.Read_Value( 6, addItem.bActive );// 표시여부

		excel.Read_String( 7, addItem.szStartTime, sizeof( addItem.szStartTime ) );// 시작일
		excel.Read_String( 8, addItem.szEndTime, sizeof( addItem.szEndTime ) );// 종료일

		excel.Read_Value( 9, addItem.nStandardSellingPrice );	// 기준 판매 가격
		excel.Read_Value( 10, addItem.nRealSellingPrice );		// 할인된 판매 가격
		excel.Read_Value( 11, addItem.nSalePersent );

		excel.Read_Value( 12, addItem.nIconID );
		excel.Read_Value( 13, addItem.nMaskType );
		excel.Read_Value( 14, addItem.nDispType );
		excel.Read_Value( 15, addItem.nDispCount );

		for( int n = 0; ; ++n )
		{
			sPackageItem packageItem;
			excel.Read_Value( 19 + (n * 3), packageItem.s_nItemID );
			if( 0 == packageItem.s_nItemID )
				break;

			excel.Read_Value( 21 + (n * 3), packageItem.s_nItemCnt );
			addItem.packageItems.push_back( packageItem );
		}

		excel.Read_Value( 43, addItem.wsComment );

		int nLargeType = GetLargeType( addItem.dwProductID );//대분류
		
		MAP_CASHSHOP_CATEGORY_IT it = insertCategory.find( nLargeType );
		if( it == insertCategory.end() )
			continue;// 오류 분류가 없다

		int nSmallType = GetSmallType( addItem.dwProductID );//소분류
		MAP_CASHSHOP_SUBCATEGORY_IT subIT = it->second.mapSubCategoryInfo.find( nSmallType );
		if( subIT == it->second.mapSubCategoryInfo.end() )
			continue;// 오류 분류가 없다

		LIST_CASHSHOP_PRODUCT_GROUP_IT secIT = subIT->second.listProductData.begin();
		for( ; secIT != subIT->second.listProductData.end(); ++secIT )
		{
			if( dwGroupID == (*secIT).dwItemGroupID )
				break;
		}

		if( secIT == subIT->second.listProductData.end() )
		{
			sCashShop_ProductGroup newItem;
			newItem.dwItemGroupID = dwGroupID;
			newItem.ProductGroupList.push_back( addItem );
			subIT->second.listProductData.push_back( newItem );
		}
		else
		{
			(*secIT).ProductGroupList.push_back( addItem );
		}
	}
	excel.Close();
}

void CsCashShopMng::_Load_Excel_WebData( char const* cFilePath, char const* cSheetName, LIST_CASHSHOP_WEBDATA & insertCategory )
{
	cExcelReader excel;
	bool bSuccess = excel.Open( cFilePath, cSheetName );
	if( !bSuccess )
		return;

	if( !excel.FirstOfSkip(2) )
		return;

	while( excel.MoveNext() )
	{
		bool bShow = false;
		excel.Read_Value( 0, bShow );
		if( !bShow )
			continue;

		sCashshop_WebData addData;
		excel.Read_Value( 1, addData.sWebImageFile );
		if( addData.sWebImageFile.empty() )
			break;

		excel.Read_Value( 2, addData.sWebLinkUrl );
		insertCategory.push_back( addData );
	}	
}

#define CASHSHOP_BIN			"CashShop.bin"
#define CASHSHOP_BIN_STEAM		"CashShopSteam.bin"
void CsCashShopMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CASHSHOP_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapCashshopData.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	std::map<int, MAP_CASHSHOP_CATEGORY>::const_iterator mainIt = m_mapCashshopData.begin();
	for( ; mainIt != m_mapCashshopData.end(); ++mainIt )
	{
		CmUtil::CmBinSave::Save_Value( fp, &mainIt->first );
		int nCount = (int)mainIt->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );
		
		MAP_CASHSHOP_CATEGORY_CIT it = mainIt->second.begin();
		for( ; it != mainIt->second.end(); ++it )
		{
			CmUtil::CmBinSave::Save_Value( fp, &it->first );
			CmUtil::CmBinSave::Save_Value( fp, it->second.wsCategoryName );

			int nCount = (int)it->second.mapSubCategoryInfo.size();
			CmUtil::CmBinSave::Save_Value( fp, &nCount );

			MAP_CASHSHOP_SUBCATEGORY_CIT subIT = it->second.mapSubCategoryInfo.begin();
			for( ; subIT != it->second.mapSubCategoryInfo.end(); ++subIT )
			{
				CmUtil::CmBinSave::Save_Value( fp, &subIT->first );
				CmUtil::CmBinSave::Save_Value( fp, subIT->second.wsCategoryName );

				int nCount = (int)subIT->second.listProductData.size();
				CmUtil::CmBinSave::Save_Value( fp, &nCount );

				LIST_CASHSHOP_PRODUCT_GROUP_CIT secIT = subIT->second.listProductData.begin();
				for( ; secIT != subIT->second.listProductData.end(); ++secIT )
				{					
					CmUtil::CmBinSave::Save_Value( fp, &(*secIT).dwItemGroupID );

					int nCount = (int)(*secIT).ProductGroupList.size();
					CmUtil::CmBinSave::Save_Value( fp, &nCount );

					LIST_CASHSHOP_PRODUCT_CIT fourthIT= (*secIT).ProductGroupList.begin();
					for( ; fourthIT != (*secIT).ProductGroupList.end(); ++fourthIT )
					{
						CmUtil::CmBinSave::Save_Value( fp, (*fourthIT).wsName );
						CmUtil::CmBinSave::Save_Value( fp, (*fourthIT).wsComment );

						CmUtil::CmBinSave::Save_Value( fp, &(*fourthIT).bActive );
						CmUtil::CmBinSave::Save_Value( fp, &(*fourthIT).dwProductID );
						CmUtil::CmBinSave::Save_String( fp, (*fourthIT).szStartTime, sizeof( (*fourthIT).szStartTime ) );
						CmUtil::CmBinSave::Save_String( fp, (*fourthIT).szEndTime, sizeof( (*fourthIT).szEndTime ) );
						CmUtil::CmBinSave::Save_Value( fp, &(*fourthIT).nPurchaseCashType );
						CmUtil::CmBinSave::Save_Value( fp, &(*fourthIT).nStandardSellingPrice );
						CmUtil::CmBinSave::Save_Value( fp, &(*fourthIT).nRealSellingPrice );
						CmUtil::CmBinSave::Save_Value( fp, &(*fourthIT).nSalePersent );
						CmUtil::CmBinSave::Save_Value( fp, &(*fourthIT).nIconID );
						CmUtil::CmBinSave::Save_Value( fp, &(*fourthIT).nMaskType );
						CmUtil::CmBinSave::Save_Value( fp, &(*fourthIT).nDispType );
						CmUtil::CmBinSave::Save_Value( fp, &(*fourthIT).nDispCount );

						int nCount = (int)(*fourthIT).packageItems.size();
						CmUtil::CmBinSave::Save_Value( fp, &nCount );

						LIST_PACKAGE_ITEMS_CIT fifthIT = (*fourthIT).packageItems.begin();// 지급 아이템 목록
						for( ; fifthIT != (*fourthIT).packageItems.end(); ++fifthIT )
						{
							CmUtil::CmBinSave::Save_Value( fp, &(*fifthIT).s_nItemID );
							CmUtil::CmBinSave::Save_Value( fp, &(*fifthIT).s_nItemCnt );
						}
					}				
				}
			}
		}
	}	

	nCount = (int)m_mapWebData.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );
	std::map<int/*캐쉬 샵 테이블*/, LIST_CASHSHOP_WEBDATA>::const_iterator webIT = m_mapWebData.begin();
	for( ; webIT != m_mapWebData.end(); ++webIT )
	{
		CmUtil::CmBinSave::Save_Value( fp, &webIT->first );

		nCount = (int)webIT->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );
		LIST_CASHSHOP_WEBDATA_CIT secIT = webIT->second.begin();
		for( ; secIT != webIT->second.end(); ++secIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, (*secIT).sWebImageFile );
			CmUtil::CmBinSave::Save_Value( fp, (*secIT).sWebLinkUrl );
		}
	}

	fclose( fp );
}

bool CsCashShopMng::_Load_Bin(char* cPath)
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CASHSHOP_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
		return false;

	//////////////////////////////////////////////////////////////////////////
	int nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		int nTableType = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nTableType );
		std::map<int, MAP_CASHSHOP_CATEGORY>::iterator mainIt = m_mapCashshopData.find( nTableType );
		if( mainIt == m_mapCashshopData.end() )
		{
			auto rib = m_mapCashshopData.insert( std::make_pair( nTableType, MAP_CASHSHOP_CATEGORY() ) );
			mainIt = rib.first;
		}

		int nTableTypeCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nTableTypeCount );
		for( int m = 0; m < nTableTypeCount; ++m )
		{
			int nMainType = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nMainType );
			MAP_CASHSHOP_CATEGORY_IT it = mainIt->second.find( nMainType );
			if( it == mainIt->second.end() )
			{
				auto rib = mainIt->second.insert( std::make_pair( nMainType, sCashshop_MainCategory() ) );
				it = rib.first;
			}

			CmUtil::CmBinRead::Read_Value( fp, it->second.wsCategoryName );

			int nCount = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nCount );
			for( int j = 0; j < nCount; ++j )
			{
				int nSubType = 0;
				CmUtil::CmBinRead::Read_Value( fp, &nSubType );
				MAP_CASHSHOP_SUBCATEGORY_IT subIT = it->second.mapSubCategoryInfo.find( nSubType );
				if( subIT == it->second.mapSubCategoryInfo.end() )
				{
					auto rib = it->second.mapSubCategoryInfo.insert( std::make_pair( nSubType, sCashshop_SubCategory() ) );
					subIT = rib.first;
				}

				CmUtil::CmBinRead::Read_Value( fp, subIT->second.wsCategoryName );

				int nCount = 0;
				CmUtil::CmBinRead::Read_Value( fp, &nCount );
				for( int k = 0; k < nCount; ++k )
				{
					sCashShop_ProductGroup addNewItem;
					CmUtil::CmBinRead::Read_Value( fp, &addNewItem.dwItemGroupID );						
		
					int nCount = 0;
					CmUtil::CmBinRead::Read_Value( fp, &nCount );
					for( int k = 0; k < nCount; ++k )
					{
						sCashshop_Item addItem;
						CmUtil::CmBinRead::Read_Value( fp, addItem.wsName );
						CmUtil::CmBinRead::Read_Value( fp, addItem.wsComment );

						CmUtil::CmBinRead::Read_Value( fp, &addItem.bActive );
						CmUtil::CmBinRead::Read_Value( fp, &addItem.dwProductID );
						CmUtil::CmBinRead::Read_String( fp, addItem.szStartTime, sizeof( addItem.szStartTime ) );
						CmUtil::CmBinRead::Read_String( fp, addItem.szEndTime, sizeof( addItem.szEndTime ) );
						CmUtil::CmBinRead::Read_Value( fp, &addItem.nPurchaseCashType );
						CmUtil::CmBinRead::Read_Value( fp, &addItem.nStandardSellingPrice );
						CmUtil::CmBinRead::Read_Value( fp, &addItem.nRealSellingPrice );
						CmUtil::CmBinRead::Read_Value( fp, &addItem.nSalePersent );
						CmUtil::CmBinRead::Read_Value( fp, &addItem.nIconID );
						CmUtil::CmBinRead::Read_Value( fp, &addItem.nMaskType );
						CmUtil::CmBinRead::Read_Value( fp, &addItem.nDispType );
						CmUtil::CmBinRead::Read_Value( fp, &addItem.nDispCount );

						int nCount = 0;
						CmUtil::CmBinRead::Read_Value( fp, &nCount );
						for( int l = 0; l < nCount; ++l )
						{
							sPackageItem itemInfo;
							CmUtil::CmBinRead::Read_Value( fp, &itemInfo.s_nItemID );
							CmUtil::CmBinRead::Read_Value( fp, &itemInfo.s_nItemCnt );
							addItem.packageItems.push_back( itemInfo );
						}

						addNewItem.ProductGroupList.push_back( addItem );
					}
					subIT->second.listProductData.push_back( addNewItem );
				}
			}
		}		
	}

	//////////////////////////////////////////////////////////////////////////
	// Web Data
	//////////////////////////////////////////////////////////////////////////
	nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		int nTableType = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nTableType );
		std::map<int, LIST_CASHSHOP_WEBDATA>::iterator it = m_mapWebData.find( nTableType );
		if( it == m_mapWebData.end() )
		{
			auto rib = m_mapWebData.insert( std::make_pair( nTableType, LIST_CASHSHOP_WEBDATA() ) );
			it = rib.first;
		}

		int nListCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nListCount );
		for( int n = 0; n < nListCount; ++n )
		{
			sCashshop_WebData addItems;
			CmUtil::CmBinRead::Read_Value( fp, addItems.sWebImageFile );
			CmUtil::CmBinRead::Read_Value( fp, addItems.sWebLinkUrl );
			it->second.push_back( addItems );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	fclose( fp );
	return true;
}

bool CsCashShopMng::_Load_Pack(char* cPath)
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CASHSHOP_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return false;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	//////////////////////////////////////////////////////////////////////////
	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		int nTableType = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nTableType );
		std::map<int, MAP_CASHSHOP_CATEGORY>::iterator mainIt = m_mapCashshopData.find( nTableType );
		if( mainIt == m_mapCashshopData.end() )
		{
			auto rib = m_mapCashshopData.insert( std::make_pair( nTableType, MAP_CASHSHOP_CATEGORY() ) );
			mainIt = rib.first;
		}

		int nTableTypeCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nTableTypeCount );
		for( int m = 0; m < nTableTypeCount; ++m )
		{
			int nMainType = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nMainType );
			MAP_CASHSHOP_CATEGORY_IT it = mainIt->second.find( nMainType );
			if( it == mainIt->second.end() )
			{
				auto rib = mainIt->second.insert( std::make_pair( nMainType, sCashshop_MainCategory() ) );
				it = rib.first;
			}

			CmUtil::CmPackRead::Read_Value( nHandle, it->second.wsCategoryName );

			int nCount = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
			for( int j = 0; j < nCount; ++j )
			{
				int nSubType = 0;
				CmUtil::CmPackRead::Read_Value( nHandle, &nSubType );
				MAP_CASHSHOP_SUBCATEGORY_IT subIT = it->second.mapSubCategoryInfo.find( nSubType );
				if( subIT == it->second.mapSubCategoryInfo.end() )
				{
					auto rib = it->second.mapSubCategoryInfo.insert( std::make_pair( nSubType, sCashshop_SubCategory() ) );
					subIT = rib.first;
				}

				CmUtil::CmPackRead::Read_Value( nHandle, subIT->second.wsCategoryName );

				int nCount = 0;
				CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
				for( int k = 0; k < nCount; ++k )
				{
					sCashShop_ProductGroup addNewItem;
					CmUtil::CmPackRead::Read_Value( nHandle, &addNewItem.dwItemGroupID);						

					int nCount = 0;
					CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
					for( int k = 0; k < nCount; ++k )
					{
						sCashshop_Item addItem;
						CmUtil::CmPackRead::Read_Value( nHandle, addItem.wsName );
						CmUtil::CmPackRead::Read_Value( nHandle, addItem.wsComment );

						CmUtil::CmPackRead::Read_Value( nHandle, &addItem.bActive );
						CmUtil::CmPackRead::Read_Value( nHandle, &addItem.dwProductID );
						CmUtil::CmPackRead::Read_String( nHandle, addItem.szStartTime, sizeof( addItem.szStartTime ) );
						CmUtil::CmPackRead::Read_String( nHandle, addItem.szEndTime, sizeof( addItem.szEndTime ) );
						CmUtil::CmPackRead::Read_Value( nHandle, &addItem.nPurchaseCashType );
						CmUtil::CmPackRead::Read_Value( nHandle, &addItem.nStandardSellingPrice );
						CmUtil::CmPackRead::Read_Value( nHandle, &addItem.nRealSellingPrice );
						CmUtil::CmPackRead::Read_Value( nHandle, &addItem.nSalePersent );
						CmUtil::CmPackRead::Read_Value( nHandle, &addItem.nIconID );
						CmUtil::CmPackRead::Read_Value( nHandle, &addItem.nMaskType );
						CmUtil::CmPackRead::Read_Value( nHandle, &addItem.nDispType );
						CmUtil::CmPackRead::Read_Value( nHandle, &addItem.nDispCount );

						int nCount = 0;
						CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
						for( int l = 0; l < nCount; ++l )
						{
							sPackageItem itemInfo;
							CmUtil::CmPackRead::Read_Value( nHandle, &itemInfo.s_nItemID );
							CmUtil::CmPackRead::Read_Value( nHandle, &itemInfo.s_nItemCnt );
							addItem.packageItems.push_back( itemInfo );
						}

						addNewItem.ProductGroupList.push_back( addItem );
					}
					subIT->second.listProductData.push_back( addNewItem );
				}
			}
		}		
	}

	//////////////////////////////////////////////////////////////////////////
	// Web Data
	//////////////////////////////////////////////////////////////////////////
	nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i=0; i<nCount; ++i )
	{
		int nTableType = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nTableType );
		std::map<int, LIST_CASHSHOP_WEBDATA>::iterator it = m_mapWebData.find( nTableType );
		if( it == m_mapWebData.end() )
		{
			auto rib = m_mapWebData.insert( std::make_pair( nTableType, LIST_CASHSHOP_WEBDATA() ) );
			it = rib.first;
		}

		int nListCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nListCount );
		for( int n = 0; n < nListCount; ++n )
		{
			sCashshop_WebData addItems;
			CmUtil::CmPackRead::Read_Value( nHandle, addItems.sWebImageFile );
			CmUtil::CmPackRead::Read_Value( nHandle, addItems.sWebLinkUrl );
			it->second.push_back( addItems );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
	return true;
}

MAP_CASHSHOP_CATEGORY const* CsCashShopMng::GetCashShopInfo(int const& nTableType) const	
{
	std::map<int, MAP_CASHSHOP_CATEGORY>::const_iterator it = m_mapCashshopData.find( nTableType );
	if( it != m_mapCashshopData.end() )
		return &it->second;
		
	return NULL;
}

LIST_CASHSHOP_WEBDATA const* CsCashShopMng::GetCashshopWebData(int const& nTableType) const
{
	std::map<int, LIST_CASHSHOP_WEBDATA>::const_iterator it = m_mapWebData.find( nTableType );
	if( it != m_mapWebData.end() )
		return &it->second;

	return NULL;
}