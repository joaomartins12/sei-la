#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
class CStoreContents
	: public IUIContentSubject< CStoreContents >
{
public:
	static int const IsContentsIdentity(void);

	struct BaseStoreItemData
	{
	public:
		BaseStoreItemData() :
			m_nItemID(0),
			m_nItemShowCount(0),
			m_nSellType(0),
			m_nSale(0),
			m_nClass(0)
		{
			m_strName.clear();
		};

		std::wstring	m_strName;
		int				m_nItemID;
		int				m_nItemShowCount;
		int				m_nSellType;
		int				m_nSale;
		int				m_nClass;
	};

	struct NormalStoreItemData : public BaseStoreItemData
	{
	public:
		NormalStoreItemData() : m_nScanPrice(0)
		{

		};

		int				m_nScanPrice;
	};
	struct DigicoreStoreItemData : public BaseStoreItemData
	{
	public:
		DigicoreStoreItemData() : m_nDigiCorePrice(0), m_nDispID(0)
		{
			m_strCurrencyName.clear();
		};

		std::wstring	m_strCurrencyName;
		int				m_nDigiCorePrice;
		int				m_nDispID;
	};
	struct EventStoreItemData : public BaseStoreItemData
	{
	public:
		EventStoreItemData() : m_nEventItemType(0), m_nEventItemPrice(0)
		{
			m_strEventItemName.clear();
		};
		std::wstring	m_strEventItemName;
		int				m_nEventItemType;
		int				m_nEventItemPrice;
	};

	struct PropertyStoreItemData : public BaseStoreItemData
	{
	public:
		PropertyStoreItemData()
		{
			m_NeedItemsList.clear();
		};

		struct NeedItemInfo
		{
			NeedItemInfo( int nID, int nCount, std::wstring strName ) : m_nNeedItemID(nID), m_nNeedItemCount(nCount)
			{
				m_strNeedItemName.assign( strName );
			}
			NeedItemInfo() : m_nNeedItemID(0), m_nNeedItemCount(0)
			{
				m_strNeedItemName.clear();
			};

			int m_nNeedItemID;
			int m_nNeedItemCount;
			std::wstring m_strNeedItemName;
		};

		void			SetNeedItemInfo( int nID, int nCount );

		std::list<NeedItemInfo> m_NeedItemsList;

	};

	struct RepurchaseItemData : public BaseStoreItemData
	{
	public:
		RepurchaseItemData() 
		{

		};
	};

	enum eEventType
	{
		eEnable_Window,		// 상점창 활성화
		eSet_StoreItemList, // 상점 아이템 세팅
		eSet_RepurchaseList,// 재구매 아이템 세팅
		eDelete_Repurchase, // 재구매 아이템 삭제
		eAdd_Repurchase,	// 재구매 아이템 추가
		eUpdate_Currency,	// 화폐 갱신
	};

public:
	CStoreContents(void);
	virtual ~CStoreContents(void);

	virtual int const		GetContentsIdentity(void) const;
	virtual bool 			Initialize(void);
	virtual void 			UnInitialize(void);
	virtual bool 			IntraConnection(ContentsSystem& System);
	virtual void 			Update(float const& fElapsedTime);
	virtual void 			NotifyEvent(int const& iNotifiedEvt);
	virtual void 			NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void 			MakeMainActorData(void);
	virtual void 			ClearMainActorData(void);
	virtual void 			MakeWorldData(void);
	virtual void 			ClearWorldData(void);
	
	void					CompleteItemBuy( ST_RECV_ItemBuySuccessInfo& BuyInfo );
	void					CompleteItemSell( ST_RECV_ItemSellSuccessInfo& SellInfo );

	void					CalcChangeItemInfo( std::map< int, int >& mapAcquire, std::map< int, int >& mapConsume, cItemData OldInvenInfo, cItemData NewInvenInfo);

	bool					IsEnableSellItem( int nInvenIndex );
	int						GetEnableBuyCnt( int nItemID );

	void					SetPassiveDiscount(CDigimonUser* pkMyPartnerMon, DWORD dwDigimonID);
	float					GetDiscountRate() const;

	BaseStoreItemData* 		GetStoreItemData( int nStoreButtonIndex );
	BaseStoreItemData*		GetStoreItemDataByID( int nItemID );

	std::list< BaseStoreItemData* >	GetStoreItemDataList() const; // 상점 아이템 목록
	std::list< BaseStoreItemData* >	GetStoreItemDataList( int nTabType );

	int						GetStoreIndex( int nItemTypeID ) const;

	void					RequestStoreItemRepurchase( int nIndex, int nItemID, int nItemCount );
	void					RequestNormalItemBuy( int nItemID );

	void					RequestRepurchaseOpen();

	void					SetInteractNPC( CsC_AvObject* pNPC );
	CsC_AvObject*			GetInteractNPC() const;

	int						GetInteractNPCType() const;
	cData_Inven*			GetInventory() const;

	void					SetBuyButtonIndex( int nIndex );

	void					CloseStoreWindow();

private:
	void					InitContentsData(void);
	 // 이벤트 함수
	void					OpenStoreWindow( void* pData );

	void					SendStoreItemBuy( void* pData );
	void					RecvStoreItemBuyInfo( void* pData );

	void					SendStoreItemSell( void* pData );
	void					RecvStoreItemSellInfo( void* pData );

	void					ApplyDiscountRate( void* pData );

	void					ChangeMyMoney( void* pData );
	void					ChangeMyItem( void* pData );

	void					SetRepurchaseItemInfo( void* pData );
	void					CompleteItemRepurchase( void* pData );
	void					FailedItemRepurchase( void* pData );

	void					CheckEnableSellItem( int const& nInvenIndex );
	// @@ 여기까지 이벤트 함수

	void					SetInteractNPCType( const int nType );
	
	bool					SetStoreInfo( CsC_AvObject* pInteractNPC );
	bool					OpenInventory();
	void					SetStoreItemDataList( const std::list< BaseStoreItemData* >& rItemIndexList );

	bool					IsEnableBuy( int& nEnableCount, int nItemID );
	void					OpenItemSeparateWindow( const int nEnableCount, const int nItemID );

	bool					IsEnableRepurchase( const BaseStoreItemData& rItemData );

	void					ApplyMyDigimonDiscount();
	void					SetInventory();

	bool					CreateStoreWindow();

private: // 비트 상점 할인율
	void					SetDiscountRate(int nValue);

private:
	int								m_nNPCType;			// npc 타입. 상점 타입
	CsC_AvObject*					m_pInteractNPC;		// 상호작용 npc
	cData_Inven*					m_pInven;			// 인벤토리
	float							m_fDiscountRate;	// 할인률
	int								m_nBuyButtonIndex;	// 구매를 위해 누른 버튼의 인덱스

	std::list< BaseStoreItemData* >	m_StoreItemDataList;	// 상점 아이템 목록
	std::list< BaseStoreItemData* >	m_RepurchaseItemList;	// 재구매 아이템 정보

// 재구매 정보
public:
	int						GetRepurchaseItemCount() const;
	BaseStoreItemData*		GetRepurchaseItemData(int nItemID, int nItemCount);
	int						GetRepurchaseItemIndex(int nItemID, int nItemCount);

	const std::list< BaseStoreItemData* >&	GetRepurchaseItemList() const;	// 재구매 아이템 정보 얻기(재구매 시)

private:
	void					AddRepurchaseItemdata( int nID, int nCount );
	RepurchaseItemData*		MakeRepurchaseItemdata( const cItemData& rItemInfo );
	void					DeleteRepurchaseItemData( const int nItemTypeID, const int nBuyCount );
// @@ 재구매 정보 여기까지

};
