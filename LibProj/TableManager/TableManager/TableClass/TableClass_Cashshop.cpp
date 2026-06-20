
#include "stdafx.h"
#include "TableClass_Cashshop.h"

BEGIN_NAME_TABLE

CCashshopTable::CCashshopTable()
{

}

CCashshopTable::~CCashshopTable()
{

}

void CCashshopTable::InitTable(int const& nServiceType)
{
	if( eSteam == nServiceType )
	{
		AddLoadTableEvent(L"cashshop_Steam_Category.bin", this, &CCashshopTable::LoadData_Category);
		AddLoadTableEvent(L"cashshop_Steam_main.bin", this, &CCashshopTable::LoadData_Main);
		AddLoadTableEvent(L"cashshop_Steam_best.bin", this, &CCashshopTable::LoadData_Best);
		AddLoadTableEvent(L"cashshop_Steam_tamer.bin", this, &CCashshopTable::LoadData_Items);
		AddLoadTableEvent(L"cashshop_Steam_digimon.bin", this, &CCashshopTable::LoadData_Items);
		AddLoadTableEvent(L"cashshop_Steam_avatar.bin", this, &CCashshopTable::LoadData_Items);
		AddLoadTableEvent(L"cashshop_Steam_packages.bin", this, &CCashshopTable::LoadData_Items);
		AddLoadTableEvent(L"cashshop_Steam_weblink.bin", this, &CCashshopTable::LoadData_Weblink);
	}
	else
	{
		AddLoadTableEvent(L"cashshop_Category.bin", this, &CCashshopTable::LoadData_Category);
		AddLoadTableEvent(L"cashshop_main.bin", this, &CCashshopTable::LoadData_Main);
		AddLoadTableEvent(L"cashshop_best.bin", this, &CCashshopTable::LoadData_Best);
		AddLoadTableEvent(L"cashshop_tamer.bin", this, &CCashshopTable::LoadData_Items);
		AddLoadTableEvent(L"cashshop_digimon.bin", this, &CCashshopTable::LoadData_Items);
		AddLoadTableEvent(L"cashshop_avatar.bin", this, &CCashshopTable::LoadData_Items);
		AddLoadTableEvent(L"cashshop_packages.bin", this, &CCashshopTable::LoadData_Items);
		AddLoadTableEvent(L"cashshop_weblink.bin", this, &CCashshopTable::LoadData_Weblink);
	}
	AddChangeStringEvent( ESTRING_TABLE_CASHSHOP, this, &CCashshopTable::InsertString );
}

void CCashshopTable::ReleaseTable()
{
	m_mapCashshopData.clear();
	m_mapWebData.clear();
}

bool CCashshopTable::InsertString(ITbImpl const* pData)
{
 	CSTable_Cashshop const* pStr = static_cast<CSTable_Cashshop const*>( pData );
	CSTable_Cashshop::ItemCont msgData = pStr->GetItem();

	MAP_CASHSHOP_CATEGORY_IT it = m_mapCashshopData.begin();
	for( ; it != m_mapCashshopData.end(); ++it )
	{
		it->second.wsCategoryName = pStr->GetCategoryNameMain( it->first );
		MAP_CASHSHOP_SUBCATEGORY_IT subIT = it->second.mapSubCategoryInfo.begin();
		for( ; subIT != it->second.mapSubCategoryInfo.end(); ++subIT )
		{
			subIT->second.wsCategoryName = pStr->GetCategoryNameSub( it->first, subIT->first );
			LIST_CASHSHOP_PRODUCT_GROUP_IT itemIt = subIT->second.listProductData.begin();
			for( ; itemIt != subIT->second.listProductData.end(); ++itemIt )
			{
				LIST_CASHSHOP_PRODUCT_IT proIT = itemIt->ProductGroupList.begin();
				for( ; proIT != itemIt->ProductGroupList.end(); ++proIT )
				{
					CSTable_Cashshop::ItemCont::const_iterator msgIT = msgData.find( (*proIT).dwProductID );
					if( msgIT == msgData.end() )
						continue;
					
					(*proIT).wsName = msgIT->second.s_szName;
					(*proIT).wsComment = msgIT->second.s_szComment;
				}
			}
		}
	}
	return true;
}


MAP_CASHSHOP_CATEGORY const& CCashshopTable::GetCashShopInfo() const	
{
	return m_mapCashshopData;
}

LIST_CASHSHOP_WEBDATA const& CCashshopTable::GetCashshopWebData() const
{
	return m_mapWebData;
}

//////////////////////////////////////////////////////////////////////////

bool CCashshopTable::LoadData_Category(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nMainType = 0;
		pReader.Read_Value( 0, nMainType );
		if( 0 == nMainType )
			break;

		m_mapCashshopData.insert( std::make_pair( nMainType, sCashshop_MainCategory() ) );
		bSuccess = pReader.MoveNext();
	}

	MAP_CASHSHOP_CATEGORY_IT it = m_mapCashshopData.begin();
	for( int nCol = 1; it != m_mapCashshopData.end(); ++it, ++nCol )
	{
		// 소분류 
		bSuccess = pReader.MoveFirst();	
		while( bSuccess )
		{
			int nKey;
			pReader.Read_Value( nCol, nKey );
			if( 0 == nKey )
				break;

			it->second.mapSubCategoryInfo.insert( std::make_pair( nKey, sCashshop_SubCategory()) );
			bSuccess = pReader.MoveNext();
		}
	}

	return true;
}

bool CCashshopTable::LoadData_Main(DataStream& pReader)
{
	sCashshop_MainCategory & pMainTab = _GetMainCategory( 1 );

	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwProdcutIdx = 0;
		pReader.pop( dwProdcutIdx );
		if( 0 == dwProdcutIdx )
			continue;

		int ntype = 0;
		pReader.pop( ntype );
		if( 0 == ntype )
			break;

		MAP_CASHSHOP_SUBCATEGORY_IT subIT = pMainTab.mapSubCategoryInfo.find( ntype );
		if( subIT == pMainTab.mapSubCategoryInfo.end() )
		{
			MAP_CASHSHOP_SUBCATEGORY::_Pairib rib = pMainTab.mapSubCategoryInfo.insert( std::make_pair( ntype, sCashshop_SubCategory() ) );
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

		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CCashshopTable::LoadData_Best(DataStream& pReader)
{
	sCashshop_MainCategory & pMainTab = _GetMainCategory( 1 );
	MAP_CASHSHOP_SUBCATEGORY_IT subIT = pMainTab.mapSubCategoryInfo.find( neCashsopHitType::Best );
	if( subIT == pMainTab.mapSubCategoryInfo.end() )
	{
		MAP_CASHSHOP_SUBCATEGORY::_Pairib rib = pMainTab.mapSubCategoryInfo.insert( std::make_pair( neCashsopHitType::Best, sCashshop_SubCategory() ) );
		subIT = rib.first;
	}

	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwProdcutIdx = 0;
		pReader.pop( dwProdcutIdx );
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
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CCashshopTable::LoadData_Items(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		sCashshop_Item addItem;
		pReader.pop( addItem.nPurchaseCashType );
		pReader.pop( addItem.dwProductID );
		if( 0 == addItem.dwProductID )
			break;

		DWORD dwGroupID = 0;
		pReader.pop( dwGroupID );
		if( 0 == dwGroupID )
			break;

		pReader.pop( addItem.bActive );// 표시여부

		pReader.pop( addItem.szStartTime );// 시작일
		pReader.pop( addItem.szEndTime );// 종료일

		pReader.pop( addItem.nStandardSellingPrice );	// 기준 판매 가격
		pReader.pop( addItem.nRealSellingPrice );		// 할인된 판매 가격
		pReader.pop( addItem.nSalePersent );

		pReader.pop( addItem.nIconID );
		pReader.pop( addItem.nMaskType );
		pReader.pop( addItem.nDispType );
		pReader.pop( addItem.nDispCount );

		do 
		{
			sPackageItem packageItem;
			pReader.pop( packageItem.s_nItemID );
			if( 0 == packageItem.s_nItemID )
				break;

			pReader.pop( packageItem.s_nItemCnt );
			addItem.packageItems.push_back( packageItem );
		} while ( 1 );

		int nLargeType = GetLargeType( addItem.dwProductID );//대분류
		int nSmallType = GetSmallType( addItem.dwProductID );//소분류

		_InsertItem( nLargeType, nSmallType, dwGroupID, addItem );	

		bSuccess = pReader.MoveNext();
	}
	return true;
}

void CCashshopTable::_InsertItem( int const& LType, int const& SType, DWORD const& dwGroupID,  sCashshop_Item const & addItem )
{	
	MAP_CASHSHOP_CATEGORY_IT it = m_mapCashshopData.find( LType );
	if( it == m_mapCashshopData.end() )
		return;// 오류 분류가 없다

	MAP_CASHSHOP_SUBCATEGORY_IT subIT = it->second.mapSubCategoryInfo.find( SType );
	if( subIT == it->second.mapSubCategoryInfo.end() )
		return;// 오류 분류가 없다

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

sCashshop_MainCategory & CCashshopTable::_GetMainCategory(int const& nHitType)
{
	MAP_CASHSHOP_CATEGORY_IT it = m_mapCashshopData.find(nHitType);
	if( it == m_mapCashshopData.end() )
	{
		MAP_CASHSHOP_CATEGORY::_Pairib rib = m_mapCashshopData.insert( std::make_pair( nHitType, sCashshop_MainCategory() ) );
		it = rib.first;
	}
	return it->second;
}

//////////////////////////////////////////////////////////////////////////

bool CCashshopTable::LoadData_Weblink(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		sCashshop_WebData info;
		pReader.pop( info.sWebImageFile );
		pReader.pop( info.sWebLinkUrl );
		m_mapWebData.push_back( info );
		bSuccess = pReader.MoveNext();
	}
	return true;
}
END_NAME_TABLE

