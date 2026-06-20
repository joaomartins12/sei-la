#pragma once
#include "PersonContents_Define.h"

class GS2C_RECV_SELL_PERSONITEM
{
public:
	GS2C_RECV_SELL_PERSONITEM()
	{
		ItemData.Clear();
		MoneyPerItem = 0;
	}
	~GS2C_RECV_SELL_PERSONITEM(){}

public:	
	cItemInfo ItemData;
	u8 MoneyPerItem;
};

class GS2C_RECV_BUY_SUCCESS_PERSONITEM
{
public:
	GS2C_RECV_BUY_SUCCESS_PERSONITEM()
	{
		SlotIndex = 0;
		Count = 0;
	}
	~GS2C_RECV_BUY_SUCCESS_PERSONITEM(){}

public:	
	int SlotIndex;
	int Count;
};

class GS2C_RECV_BUY_FAIL_PERSONITEM
{
public:
	GS2C_RECV_BUY_FAIL_PERSONITEM()
	{
		Result = 0;
	}
	~GS2C_RECV_BUY_FAIL_PERSONITEM(){}

public:	
	int Result;
};

class GS2C_RECV_BUY_PERSONITEM
{
public:
	GS2C_RECV_BUY_PERSONITEM()
	{
		buyItemData.Clear();
	}
	~GS2C_RECV_BUY_PERSONITEM(){}

public:	
	cItemData buyItemData;
};

class GS2C_RECV_USED_ITEMID
{
public:
	GS2C_RECV_USED_ITEMID()
	{
		itemID = 0;
	}
	~GS2C_RECV_USED_ITEMID(){}

public:	
	n4 itemID;
};

class GS2C_RECV_TAMER_NAME
{
public:
	GS2C_RECV_TAMER_NAME()
	{
		sTamerName.clear();
	}
	~GS2C_RECV_TAMER_NAME(){}

public:	
	std::wstring sTamerName;
};

class GS2C_RECV_EDIT_NAME
{
public:
	GS2C_RECV_EDIT_NAME()
	{
		sEditName.clear();
	}
	~GS2C_RECV_EDIT_NAME(){}

public:	
	std::wstring sEditName;
};

class GS2C_RECV_STORE_NAME
{
public:
	GS2C_RECV_STORE_NAME()
	{
		szShopTitle.clear();
	}
	~GS2C_RECV_STORE_NAME(){}

public:	
	std::wstring	szShopTitle;
};

///////////////////////////////////////////////////////////////////////////////
class GS2C_RECV_STORE_READY_RESULT
{
public:
	GS2C_RECV_STORE_READY_RESULT() : nResult( 0 ), nItemType( 0 ) {}
	~GS2C_RECV_STORE_READY_RESULT() {};

public:
	n4		nResult;
	u4		nItemType;	// 상점 개설에 사용된 아이템 타입
};

class GS2C_RECV_STORE_OPEN_CLOSE_RESULT
{
public:
	GS2C_RECV_STORE_OPEN_CLOSE_RESULT() : nResult( 0 ) {}
	~GS2C_RECV_STORE_OPEN_CLOSE_RESULT() {};

public:
	n4 nResult;
};

class GS2C_RECV_BUY_RESULT_PERSONITEM
{
public:
	GS2C_RECV_BUY_RESULT_PERSONITEM() : nResult( 0 ), nSlotIndex( 0 ), nCount( 0 ) {}
	~GS2C_RECV_BUY_RESULT_PERSONITEM() {};

public:	
	n4	nResult;
	u4	nSlotIndex;
	u4	nCount;
};

class GS2C_RECV_PERSONSTORE_ITEMLIST
{
public:
	GS2C_RECV_PERSONSTORE_ITEMLIST() : nResult( 0 ), nItemCount( 0 ), nItemPrice( 0 )
	{
		lItemInfo.clear();
		szTitle.clear();
	}
	~GS2C_RECV_PERSONSTORE_ITEMLIST() {}

public:
	n4				nResult;
	std::wstring	szTitle;
	n4				nItemCount;
	std::list< nsPersonStore::sSellItemInfo >		lItemInfo;
	n4				nItemPrice;
};

class GS2C_RECV_COMMISSIONSTORE_ITEMLIST
{
public:
	GS2C_RECV_COMMISSIONSTORE_ITEMLIST() : nResult( 0 ), nUsedItemType( 0 ), nItemCount( 0 ), nItemPrice( 0 ),
	bMyShop( false )
	{
		lItemInfo.clear();
		szShopName.clear();
		szTamerName.clear();
	}
	~GS2C_RECV_COMMISSIONSTORE_ITEMLIST(){}

public:
	n4				nResult;
	n4				nUsedItemType;
	std::wstring	szShopName;
	std::wstring	szTamerName;
	u4				nItemCount;
	std::list< nsPersonStore::sSellItemInfo >		lItemInfo;
	n4				nItemPrice;
	bool			bMyShop;
};