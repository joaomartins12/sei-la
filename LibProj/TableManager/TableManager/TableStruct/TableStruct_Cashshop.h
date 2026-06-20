
#pragma once 

BEGIN_NAME_TABLE

namespace neCashsopHitType
{
	enum eHiteType
	{
		None = 0,
		New = 1,
		Hot,
		Event,
		Best,
		LimitedTime//기간 한정
	};
};

namespace neCashshopCashType
{
	enum eType
	{
		eCash = 1,	
		eBonus,
	};
}

namespace neCashshopPurchaseType
{
	enum eType
	{
		DT_NONE,
		DT_COUNT,
		DT_DAY,
		DT_PERENNITY,		// 영구
	};
}

struct sPackageItem
{
	sPackageItem():s_nItemID(0), s_nItemCnt(0)	{};
	~sPackageItem(){};

	DWORD	s_nItemID;		
	int		s_nItemCnt;		
};

typedef std::list<sPackageItem>						LIST_PACKAGE_ITEMS;
typedef std::list<sPackageItem>::iterator			LIST_PACKAGE_ITEMS_IT;
typedef std::list<sPackageItem>::const_iterator		LIST_PACKAGE_ITEMS_CIT;

struct sCashshop_Item
{
	sCashshop_Item():bActive(false),dwProductID(0),nPurchaseCashType(0),nStandardSellingPrice(0),nRealSellingPrice(0)
		,nSalePersent(0),nIconID(0),nMaskType(0),nDispType(0),nDispCount(0)
	{
	}
	~sCashshop_Item() {};

	std::wstring	wsName;				// 판매 물품 이름
	std::wstring	wsComment;			// 판매 물품 설명
	bool			bActive;			// 활성화 여부
	DWORD			dwProductID;		// 판매 번호	- 키값
	std::string		szStartTime;	// 판매 시작 시간
	std::string		szEndTime;	// 판매 종료 기간
	int				nPurchaseCashType;	// 아이템 구매 캐쉬 타입. 1 : 캐쉬, 2 : 보너스 캐쉬
	int				nStandardSellingPrice;	// 기준 판매 가격
	int				nRealSellingPrice;		// 할인 판매 가격
	int				nSalePersent;		// 할인 퍼센트
	int				nIconID;			// 아이콘 ID
	int				nMaskType;			// 1 : New, 2 : Hot, 3 : Event
	int				nDispType;			// 판매 타입, 기간제 및 갯수
	int				nDispCount;			// 기간 및 판매 갯수
	LIST_PACKAGE_ITEMS packageItems;// 지급 아이템 목록
};

//////////////////////////////////////////////////////////////////////////
typedef std::list<sCashshop_Item>					LIST_CASHSHOP_PRODUCT;
typedef std::list<sCashshop_Item>::iterator			LIST_CASHSHOP_PRODUCT_IT;
typedef std::list<sCashshop_Item>::const_iterator	LIST_CASHSHOP_PRODUCT_CIT;

struct sCashShop_ProductGroup
{
	sCashShop_ProductGroup():dwItemGroupID(0) {}
	sCashShop_ProductGroup(DWORD dwGroupID):dwItemGroupID(dwGroupID) {}

	DWORD					dwItemGroupID;	// 키값
	LIST_CASHSHOP_PRODUCT	ProductGroupList;
};

typedef std::list<sCashShop_ProductGroup>					LIST_CASHSHOP_PRODUCT_GROUP;
typedef std::list<sCashShop_ProductGroup>::iterator			LIST_CASHSHOP_PRODUCT_GROUP_IT;
typedef std::list<sCashShop_ProductGroup>::const_iterator	LIST_CASHSHOP_PRODUCT_GROUP_CIT;

struct sCashshop_SubCategory
{
	sCashshop_SubCategory() {}

	std::wstring							wsCategoryName;
	LIST_CASHSHOP_PRODUCT_GROUP				listProductData;
	size_t GetAllItemCount() const
	{
		size_t count = 0;
		LIST_CASHSHOP_PRODUCT_GROUP_CIT it = listProductData.begin();
		for( ; it != listProductData.end(); ++it )
			count += (*it).ProductGroupList.size();
		return count;
	}
};

typedef std::map<int, sCashshop_SubCategory>						MAP_CASHSHOP_SUBCATEGORY;
typedef std::map<int, sCashshop_SubCategory>::iterator				MAP_CASHSHOP_SUBCATEGORY_IT;
typedef std::map<int, sCashshop_SubCategory>::const_iterator		MAP_CASHSHOP_SUBCATEGORY_CIT;

struct sCashshop_MainCategory
{
	sCashshop_MainCategory() {}

	std::wstring				wsCategoryName;
	MAP_CASHSHOP_SUBCATEGORY	mapSubCategoryInfo;
};

typedef std::map<int/*대분류 키*/, sCashshop_MainCategory >						MAP_CASHSHOP_CATEGORY;
typedef std::map<int/*대분류 키*/, sCashshop_MainCategory >::iterator			MAP_CASHSHOP_CATEGORY_IT;
typedef std::map<int/*대분류 키*/, sCashshop_MainCategory >::const_iterator		MAP_CASHSHOP_CATEGORY_CIT;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

struct sCashshop_WebData
{
	sCashshop_WebData(){};
	~sCashshop_WebData(){};

	std::string		sWebImageFile;
	std::string		sWebLinkUrl;
};

typedef std::list<sCashshop_WebData>					LIST_CASHSHOP_WEBDATA;
typedef std::list<sCashshop_WebData>::iterator			LIST_CASHSHOP_WEBDATA_IT;
typedef std::list<sCashshop_WebData>::const_iterator	LIST_CASHSHOP_WEBDATA_CIT;

END_NAME_TABLE