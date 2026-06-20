#pragma once

#include "CashShop_Define.h"

class C2GS_SEND_CASHITEM_PURCHASE
{
public:
	C2GS_SEND_CASHITEM_PURCHASE():nProtocol(pCashShop::Buy),nItemCount(1), nTotalPrice(0), ui64OrderID(0){};
	~C2GS_SEND_CASHITEM_PURCHASE(){};

	WORD GetProtocol(){return nProtocol;}

public:
	WORD				nProtocol;
	n1					nItemCount;
	n4					nTotalPrice;
	uint64				ui64OrderID;
	std::list<DWORD>	itemList;
};

// 서버 아이템 구매 결과값
class GS2C_RECV_CASHITEM_PURCHASE
{
public:
	GS2C_RECV_CASHITEM_PURCHASE():m_dwResult(0),m_nBonusCash(0),m_nHaveCash(0),m_bCloseCashShop(false)
	{};

	~GS2C_RECV_CASHITEM_PURCHASE () {};

public:	
	DWORD					m_dwResult;
	int						m_nBonusCash;			// 보너스 캐쉬
	int						m_nHaveCash;			// 실제 캐쉬
	bool					m_bCloseCashShop;		// 아이템 구매 실패시 캐쉬샵을 닫을 것인가
};

class C2GS_SEND_CASHITEM_PURCHASE_STEAM_REQUEST
{
public:
	C2GS_SEND_CASHITEM_PURCHASE_STEAM_REQUEST():nProtocol(pCashShop::BuyRequest),nItemCount(1), nTotalPrice(0) {};
	~C2GS_SEND_CASHITEM_PURCHASE_STEAM_REQUEST(){};

	WORD GetProtocol(){return nProtocol;}

public:
	WORD				nProtocol;
	n1					nItemCount;
	n4					nTotalPrice;
	std::list<DWORD>	itemList;
};


class GS2C_RECV_CASHITEM_STORAGE
{
public:
	GS2C_RECV_CASHITEM_STORAGE(int size){
		CashItemStorage.reserve(size);
		CashItems.reserve(size);
	}
	~GS2C_RECV_CASHITEM_STORAGE() {};

	void AddCashItem(cItemData const& item)
	{
		CashItemStorage.push_back(item);
		CashItems.push_back(&CashItemStorage.back());
	}

	std::vector<cItemData> CashItemStorage;
	std::vector<cItemData*> CashItems;
};

class GS2C_RECV_CASHSHOP_BALANCE
{
public:
	GS2C_RECV_CASHSHOP_BALANCE():m_nResult(0),m_nBonusCash(0),m_nHaveCash(0),m_bCloseCashShop(false)
	{
	}

	~GS2C_RECV_CASHSHOP_BALANCE() {};

public:	
	n4						m_nResult;
	int						m_nBonusCash;			// 보너스 캐쉬
	int						m_nHaveCash;			// 실제 캐쉬
	bool					m_bCloseCashShop;		// 아이템 구매 실패시 캐쉬샵을 닫을 것인가
};


class C2GS_SEND_CASHSHOP_MULTIBUY
{
public:
	C2GS_SEND_CASHSHOP_MULTIBUY(): nProtocol(pCashShop::MultiBuy) , nTotalPrice(0) , nOrderID(0),
		nItemCnt(0) , nCashItemNo(0)
	{}

	~C2GS_SEND_CASHSHOP_MULTIBUY() {};

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	n4 nTotalPrice;	// 모든 아이템 가격
	u8 nOrderID;	// Steam용
	n1 nItemCnt;
	// x nItemCnt
	std::list<n4> nCashItemNo;
};


class GS2C_RECV_CASHSHOP_MULTIBUY
{
public:
	GS2C_RECV_CASHSHOP_MULTIBUY(): nProtocol(pCashShop::MultiBuy) ,nResult(0) ,nBonusCash(0) , nRealCash(0) ,
		nSuccessItemCnt(0) , nSuccessCashItemNo(0) , nFailedItemCnt(0) , nFailedCashItemNo(0)
	{}
	~GS2C_RECV_CASHSHOP_MULTIBUY () {};

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	u2 nResult;			//성공시 0 , 실패시 오류코드 , 실패시 아래쪽은 전송하지않음
	n4 nRealCash;		//잔액
	n4 nBonusCash;		//잔액
	n1 nSuccessItemCnt;
	// x nSuccessItemCnt
	std::list<n4> nSuccessCashItemNo;
	n1 nFailedItemCnt;
	// x nFailedItemCnt
	std::list<n4> nFailedCashItemNo;
};



class C2GS_SEND_CASHSHOP_GIFT
{
public:
	C2GS_SEND_CASHSHOP_GIFT(): nProtocol(pCashShop::Gift) , nProductIDX(0) , nPrice(0)
	{}
	~C2GS_SEND_CASHSHOP_GIFT() {};

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	n4 nPrice;
	n4 nProductIDX;	
	std::wstring szPeerTamerName;
};

class GS2C_RECV_CASHSHOP_GIFT
{
public:
	GS2C_RECV_CASHSHOP_GIFT(): nProtocol(pCashShop::Gift) ,nResult(0) ,nRealCash(0) , nBonusCash(0)
	{}
	~GS2C_RECV_CASHSHOP_GIFT() {};

	WORD GetProtocol(){return nProtocol;}

public:
	WORD			nProtocol;
	u2				nResult;		// 성공시 0 ,실패시 오류코드 , 실패시 아래쪽은 전송하지않음
	n4				nRealCash;
	n4				nBonusCash;
	std::wstring	szPeerTamerName;	// 서버에서 선물한 테이머 이름
	n4				nGiftItemProductIDX;
};


//////////////////////////////////////////////////////////////////////////
// 캐쉬 창고에서 아이템 꺼내기
class GS2C_RECV_CASHITEM_CROP
{
public:
	GS2C_RECV_CASHITEM_CROP():m_nResult(0),m_nRTime(0),m_nCashWarehouseSlotNum(0),m_nSlotType(0),m_nSlotNo(0)
		,m_nItemType(0),m_nItemCount(0),m_nItemRate(0),m_nItemEndTime(0),m_nRemainTradeLimitTime(0)
	{};
	~GS2C_RECV_CASHITEM_CROP(){};

public:	
	n4		m_nResult;
	n4		m_nRTime;//남은 시간(초)
	u2		m_nCashWarehouseSlotNum;

	u1		m_nSlotType;	// pItem.h ==> nItem::eTab 참조
	u1		m_nSlotNo;
	u4		m_nItemType;
	u4		m_nItemCount;
	u1		m_nItemRate;
	u4		m_nItemEndTime;
	u4		m_nRemainTradeLimitTime;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class C2GS_SEND_CASHITEM_BUYHISTORY
{
public:
	C2GS_SEND_CASHITEM_BUYHISTORY():nProtocol(pCashShop::BuyHistory) {};
	~C2GS_SEND_CASHITEM_BUYHISTORY() {};

	WORD GetProtocol(){return nProtocol;}

public:
	WORD				nProtocol;
};

class GS2C_RECV_CASHITEM_BUYHISTORY
{
public:
	GS2C_RECV_CASHITEM_BUYHISTORY():wResult(0) {};
	~GS2C_RECV_CASHITEM_BUYHISTORY() {};

public:
	WORD				wResult;
	std::list<n4>	listBuyProductID;
};
