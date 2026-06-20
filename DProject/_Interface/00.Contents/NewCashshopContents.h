#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class AdaptFriendShoutOut;

class NewCashshopContents
	: public IUIContentSubject< NewCashshopContents >
{
private:
	enum eSate{
		eNone,		// 서버로부터 데이터를 받지 않은 상태
		eRecvWait,	// 서버로 데이터 요청 중인 상태
		eRecved,	// 서버로 부터 데이터를 받은 상태
	};

public:
	enum CASHSHOP_FUNTION
	{
		NONE = 0,			// 
		PRESENT,			// 선물 하기
		CART,				// 카트에 담기
		PURCHASE,			// 구매 하기
		CARTALLPURCHASE,	// 카트 담긴 아이템 모두 구매
		REPURCHASE_WINDOW,
		END,
	};

	//////////////////////////////////////////////////////////////////////////
	// 보유 캐쉬 정보 데이터
	struct sCashBalance
	{
	public:
		sCashBalance():m_cRecvedState(eNone),m_RealCash(0),m_BonusCash(0) {};
		~sCashBalance() {};

		void				ClearData();
		void				SetCashBalance( int const& nReal, int const& nBonus );
		bool				Request_CashBalance();
		void				Recve_CashBalance( int const& nReal, int const& nBonus );
		DWORD				GetTotalCash() const;
		DWORD				GetRealCash() const;
		DWORD				GetBonusCash() const;

	private:
		CsEnableTimeSeq		m_RefreshTime;
		eSate				m_cRecvedState;		
		DWORD				m_RealCash;			// 보유 캐쉬
		DWORD				m_BonusCash;			// 보유 보너스 캐쉬
	};
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
 	// 구매 아이템 정보
	typedef std::list<DWORD>				PurchaseItems;
	typedef PurchaseItems::iterator			PurchaseItemIter;
	typedef PurchaseItems::const_iterator	PurchaseItemConstIter;
	struct PurchaseItemInfo
	{
	public:
		PurchaseItemInfo():m_cRecvedState(eNone){};
		~PurchaseItemInfo() {};

		void					ClearData();
		bool					IsRecvWait() const;
		bool					AddPurchaseItem( DWORD const& dwProductIDX );
 		bool					SendPurchaseItem(DWORD const& dwTotalPrice, uint64 ui64OrderID = 0);
 		bool					SendPresentItem(DWORD const& dwTotalPrice, std::wstring const& wsTamerName, uint64 ui64OrderID = 0);
		PurchaseItems const&	GetList() const;
		size_t					GetPurchaseItemCount() const;
		DWORD					GetFirstProductIDX() const;
		bool					IsEmpty() const;
		bool					ChangeProductIDX(int const& nIdx, DWORD const& dwProductIDX);
		bool					IsSameProductIDX( DWORD const& dwCheckProductIDX ) const;

	private:
		PurchaseItems	m_PurchaseItems;
		eSate			m_cRecvedState;		
	};
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 장바구니 컨테이너
	typedef std::map<int,DWORD>			Baskets;
	typedef Baskets::iterator			BasketIter;
	typedef Baskets::const_iterator		BasketCIter;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 캐쉬 아이템 구매 히스토리
	typedef std::list<DWORD>				BuyItemHistory;
	typedef PurchaseItems::iterator			BuyItemHistoryIter;
	typedef PurchaseItems::const_iterator	BuyItemHistoryConstIter;
	struct sBuyHistory
	{
	public:
		sBuyHistory(): m_cRecvedState(eNone) {};
		~sBuyHistory() {};

		void				ClearData();
		void				SetRecvedState();
		bool				AddHistoryData( DWORD const& dwProductIDX );
		bool				Request_HistoryData();
		void				RecvedHistoryData( std::list<n4> const& dataList );
		BuyItemHistory		GetList() const;

	private:
		//CsEnableTimeSeq		m_RefreshTime;
		eSate				m_cRecvedState;
		BuyItemHistory		m_HistoryList;
	};
	//////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// 캐쉬 창고 아이템 정보
	typedef std::vector<cItemInfo*>				CashStorageItems;
	typedef CashStorageItems::iterator			CashStorageIter;
	typedef CashStorageItems::const_iterator	CashStorageConstIter;
	struct sCashWarehouseInfo
	{
	public:
		sCashWarehouseInfo(): m_cRecvedState(eNone) {};
		~sCashWarehouseInfo() {};

		void					ClearData();		
		bool					Request_CashWarehouseInfo();
								// 캐쉬샵 창고에서 해당 슬롯을 제거 하는 함수
		bool					RemoveCashWarehouse(int const& nCashInvenSlotNum);
		void					Recv_CashWarehouseInfo( std::vector<cItemData*> & vecData );
		CashStorageItems const& GetCashWarehouseInfo() const;
		bool					Request_CashItemCrop(int const& nCashInvenSlotNum);
		cItemInfo*				GetItemInfo( int const& nSlotIdx );

	private:
		CsEnableTimeSeq			m_RefreshTime;
		eSate					m_cRecvedState;
		CashStorageItems		m_ItemList;
	};
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// 현재 착용 중인 테이머 장비 데이터
	typedef std::map<int, CsC_PartObject::sCHANGE_PART_INFO> Equipments;
	typedef Equipments::iterator EquipmentIter;
	typedef Equipments::const_iterator EquipmentCIter;
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	typedef std::list<sCashshop_Item const* >					CASHITEM_TABLE;
	typedef std::list<sCashshop_Item const* >::iterator			CASHITEM_TABLE_IT;
	typedef std::list<sCashshop_Item const* >::const_iterator	CASHITEM_TABLE_CIT;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	enum eCategoryType
	{
		eMain = 1,		// 테이블 상에 메인 인덱스 번호가 1번부터 시작함.
		eSub,
	};

	//////////////////////////////////////////////////////////////////////////
	// 캐쉬샵 테이블 데이터 컨테이너	
	struct sProductItemInfo
	{
		sProductItemInfo():dwGroupID(0),nStartTime(0),nEndTime(0),nIconID(0),nMaskType(0), dwFirstProductIDX(0),bIsFileringShow(true),nStandardPrice(0),nSaleIconIdx(0)
		{};
		~sProductItemInfo(){};

		bool					IsPurchaseEnable() const;
		bool					IsIncludeKeyword( std::wstring const& wsFileringKeyword );
		void					ChangeFirstProductIDX( DWORD const& dwProductIDX );

		DWORD					dwGroupID;
		std::wstring			wsName;				// 판매 물품 이름
		std::wstring			wsComment;			// 판매 물품 설명
		bool					bIsFileringShow;
		uint					nStartTime;	// 판매 시작 시간
		uint					nEndTime;	// 판매 종료 기간
		int						nIconID;	// 아이콘 ID
		int						nShowPrice;	// 첫번째 아이템의 가격을 표시 -할인률 적용된 가격
		int						nStandardPrice;	// 할인률이 적용안된 가격
		int						nSaleIconIdx;
		int						nMaskType;	// 1 : New, 2 : Hot, 3 : Event
		DWORD					dwFirstProductIDX;
		CASHITEM_TABLE			pTableInfo;	// 실제 테이블 포인터
	};

	typedef std::list<sProductItemInfo>						LIST_PRODUCTGROUP_INFO;
	typedef std::list<sProductItemInfo>::iterator			LIST_PRODUCTGROUP_INFO_IT;
	typedef std::list<sProductItemInfo>::const_iterator		LIST_PRODUCTGROUP_INFO_CIT;

	struct sSubCategoryInfo
	{
		sSubCategoryInfo() {};
		~sSubCategoryInfo() {};

		void				SetFileringKeyword( std::wstring const& wsFileringKeyword );
		size_t				GetShowItemCount() const;

		std::wstring				name;					// 서브 카테고리 이름
		LIST_PRODUCTGROUP_INFO		itemgroup;	// 서브 카테고리에 포함된 아이템 그룹.
	};

	typedef std::map<int, sSubCategoryInfo>					MAP_SUBCATEGORY_INFO;
	typedef std::map<int, sSubCategoryInfo>::iterator		MAP_SUBCATEGORY_INFO_IT;
	typedef std::map<int, sSubCategoryInfo>::const_iterator	MAP_SUBCATEGORY_INFO_CIT;

	struct sMainCategoryInfo
	{
		sMainCategoryInfo() {};
		~sMainCategoryInfo() {};

		void				SetFileringKeyword( std::wstring const& wsFileringKeyword );

		std::wstring		 name;
		MAP_SUBCATEGORY_INFO subCategoryinfo;
	};

	typedef std::map<int, sMainCategoryInfo>					MAP_MAINCATEGORY_INFO;
	typedef std::map<int, sMainCategoryInfo>::iterator			MAP_MAINCATEGORY_INFO_IT;
	typedef std::map<int, sMainCategoryInfo>::const_iterator	MAP_MAINCATEGORY_INFO_CIT;
	//////////////////////////////////////////////////////////////////

public:
	static int const IsContentsIdentity(void);

	enum eEventType
	{
		eEventType_End,
		OnShowCashshop,
		Changed_MainCategory,
		Changed_SubCategory,
		Changed_Page,
		Move_Page,
		Refresh_CurrentPage,
		Update_FilteringKeyword,
		Update_Cash_Storage,
		CashShop_Cart_InsertItem,
		CashShop_Cart_DeleteItem,
		Update_Cash_Balance,
		Upadte_Equipment,
		Show_PurchaseViewer,
		Hide_PurchaseViewer,
		Update_Cash_Storage_Item,
		PurchaseViewer_ChangeSelect,
		Update_CashItem_Buy_History,
	};
public:
		NewCashshopContents(void);
		virtual ~NewCashshopContents(void);

public:
	virtual int const				GetContentsIdentity(void) const;
	virtual bool					Initialize(void);
	virtual void					UnInitialize(void);
	virtual bool					IntraConnection(ContentsSystem& System);
	virtual void					Update(float const& fElapsedTime);
	virtual void					NotifyEvent(int const& iNotifiedEvt);
	virtual void					NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void					MakeMainActorData(void);
	virtual void					ClearMainActorData(void);
	virtual void					MakeWorldData(void);
	virtual void					ClearWorldData(void);

public:
	void							GetFriendList(std::list<std::wstring>& friendList);
	bool							IsMyFriend( std::wstring const& wsTamerName );

	void							Req_ShowCashshop(void* pkData);
	void							Req_HideCashshop(void* pkData);
	void							Req_ToggleCashshop(void* pkData);
	void							ToggleCashWarehouse(void* pData);

private:
	void 							_ClearAllData(void);
	bool							_OpenCashshopWindow();
	bool							_CloseCashshopWindow();

	/////////////////////////////////////////////////////////////////////////
	// 캐쉬샵 테이블 관련 함수
	//////////////////////////////////////////////////////////////////////////
public:	
	void							GetMainCategoryName( std::map<int,std::wstring>& categoryInfo );// 메인 카테고리 이름을 얻는 함수	
	void							GetSubCategoryName( int const& nMainType, std::map<int,std::wstring>& categoryInfo );// 서브 카테고리 이름을 얻는 함수
	LIST_PRODUCTGROUP_INFO const *	GetCategoryItems( int const& nMainType, int const& nSubType );// 카테고리로 해당 카테고리에 있는 모든 목록을 얻는 함수
	void							GetRandomCategoryItemGroup( int const& nMainType, int const& nSubType, std::list<DWORD>& result );
	sSubCategoryInfo const *		GetSubCategoryInfos( int const& nMainType, int const& nSubType );
	sCashshop_Item const*			GetProductItemInfo( DWORD const& dwProductItem ) const;
	sCashshop_Item const*			GetProductItemInfo( DWORD const& dwGroup, DWORD const& dwProductItem ) const;
	sProductItemInfo const*			GetCashshop_ItemGropInfo( DWORD const& dwProductItemID ) const;
	sProductItemInfo const*			GetCashshop_ItemGropInfo( int const& nMainType, int const& nSubType, DWORD const& dwItemGroupID ) const;
	bool							CheckVailedItem( DWORD const& dwProductItemID ) const;
	void							Move_Item(DWORD const& ProductID);// 재품 번호로 재품이 있는 페이지로 이동 시킴.

private:
	void							_LoadCashShopTable();
	LIST_PRODUCTGROUP_INFO *		_GetCategoryItems( int const& nMainType, int const& nSubType );
	void							_ShowCashshopItemSelect(DWORD const& dwProductID);

	//////////////////////////////////////////////////////////////////////////
	// 캐쉬 아이템 구매 관련 함수
	//////////////////////////////////////////////////////////////////////////
public:
	PurchaseItems const&			GetPurchaseItems() const;
	void							CloseBuyWindow(void* pData);
									//구매 아이템 창을 띄운다.
	void							OnClick_ShowPurchaseItem(NewCashshopContents::CASHSHOP_FUNTION const& nType, DWORD const& dwProductIDX);
	void							OnClick_ClosePurchaceItem(void);

									//구매 아이템 창에서 구매 버튼을 누른다.
	void							OnClick_PurchaseItem(void);
	void							OnClick_PurchaseAllItem(void);
	void							OnClick_PurchasePresent(std::wstring const& wsPresentTargetName);
	void							OnChange_PurchaseItem(int IndexInGroup);

	DWORD							GetTotalPurchasePrice() const;
	bool							IsHaveEndofSaleItem();

private:
	void							Recv_PurchaseItem(void* pkData);
	void							Recv_PresentItem(void* pkData);
	void							Send_CashShopItemPurchase(void* pData);
	void							Send_CashShopItemPresent(void* pData);
	void							Callback_CashItemPurchase_Steam(void* pData);
	void							Recv_RequestCashItemPurchase_Steam(void*pData);


	void							_RePurchaseItems( std::list<n4> const& reItems );
	bool							_SendPurchaseItemAll();
	bool							_SendPurchaseItem_Present( std::wstring const& wsTamerName, DWORD const& dwPresentItemIDX );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 캐쉬 샵 창고 관련 함수
	//////////////////////////////////////////////////////////////////////////
public:
	void							Req_RefreshCashStorage();
	void							OnClick_ShowCashStorage();
									// 	캐쉬 창고 아이템을 찾을 수 있는 상태인지 체크 함수
	void 							CheckCropCashItem( int const& nCashInvenSlotNum, DWORD const& dwItemCode );

	CashStorageItems const&			GetCashStorage(void) const;
	cItemInfo*						GetCashStorage(int const& nSlotNum);
	cItemInfo*						GetCashStorageItem( int const& nSlotNum );

private:
	void							Recv_CashStorage(void* pkData);
	void 							Send_CashItemCrop(void* pData);
	void 							Recv_CashItemCrop(void* pData);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 장바구니 관련 함수
	//////////////////////////////////////////////////////////////////////////
public:
	void							OnClick_AddBasket();//장바구니에 집어넣는다
	void							OnClick_RemoveItemInBasket(int const& nSlotNum,DWORD const& dwProductIDX);
	void							OnClick_PurchaseAll_Cart(void);
	Baskets const&					GetBasketsItemAll(void) const;

private:
	bool							_RemoveBasketItems(std::list<n4> const& removeItems );
	bool							_RemoveBasketItem( DWORD const& dwProductIDX );
	int								_InsertBasketsItem( DWORD const& dwProductIDX );
	void							_RefreshBasketEndofSale();
	bool							_IsBasketItemEndOfSale() const;

	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 내 캐쉬 정보 관련 함수
	//////////////////////////////////////////////////////////////////////////
public:
	const DWORD 						GetTotalCash(void) const;
	const DWORD 						GetRealCash(void) const;
	const DWORD							GetBonusCash(void) const;
	void								Req_RefrashBalance();

private:
	void								Recv_CashBalance(void* pkData);
	//////////////////////////////////////////////////////////////////////////
private:

	//////////////////////////////////////////////////////////////////////////
	// 캐쉬 아이템 구매 내역 관련 함수
	//////////////////////////////////////////////////////////////////////////
public:
	int									GetHistoryIdx() const;

private:
	void								Recv_Cashshop_Buy_History(void* pData);

	void								_AddCashHistory( n4 const& nAddProductIDX, bool bNotify = true );
	void								_AddCashHistory( std::list<n4> const& nList );
	void								_RecvCashHistory( std::list<n4> const& nList );

	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 기타
	//////////////////////////////////////////////////////////////////////////
public:
	LIST_CASHSHOP_WEBDATA const*		GetCashshopWebData();
										//캐시 충전 페이지를 엽니다.
	void								OnClick_ChargingCash(void);
	void								OnClick_OpenBanner(std::string const& sOpenUrl);
										//검색
	void								Clear_SetFilteringKeyword();
	void								OnClick_SetFilteringKeyword(std::wstring const& pData);

	//장비창 관련 
	Equipments const &					GetEquipments_Origin(void) const;
	Equipments const &					GetEquipments_Changed(void) const;
	void								OnClick_TakeOffEquipmentItem(int PartID, int ItemID);
	void 								OnClick_TakeOffAll(void);
	void 								OnClick_ReturnOriginally(void);
	void 								OnClick_SellingItem(DWORD const& dwGroupID, DWORD const& dwProductID);

	void								GetAllIncludePackageItemsFromProductIDX( DWORD const& dwProductID, std::wstring & wsProductName, LIST_PACKAGE_ITEMS & listData );

	std::wstring						GetProductItemName( DWORD const& dwProductID ) const;
	std::wstring						GetAllIncludeProductItemName( DWORD const& dwProductID ) const;

private:
	bool								_CheckEnableCashshop();
	bool 								_InitCharacterEquipment();
	//////////////////////////////////////////////////////////////////////////

private:
	MAP_MAINCATEGORY_INFO					mMainCategoryInfos;	// 캐쉬샵 테이블 정보
	int										m_nHistoryIdx;		// 구매 내역 탭의 인덱스 번호

	std::wstring							SerchingKeyword;	// 문자열 검색 값

	Equipments								OriginEquipments;	// 원래의 장비 정보
	Equipments								ChangedEquip;		// 변경된 장비 정보

	//////////////////////////////////////////////////////////////////////////
	
	Baskets									mBaskets;			// 장바구니에 담겨 있는 아이템 정보
 	PurchaseItemInfo						mPurchaseItems;		// 구매 하려는 아이템 정보들
 	sBuyHistory								mBuyHistory;		// 아이템 구매 이력
 	sCashWarehouseInfo						mCashStorageItems;	// 캐쉬 창고 아이템 정보
 	sCashBalance							mCashBalance;		// 현재 보유 캐쉬 정보
	
	//////////////////////////////////////////////////////////////////////////
	CsEnableTimeSeq							m_OnOffTimeSeq;

	std::map<DWORD,sCashshop_Item const*>	m_mapInactiveProductItems;
					
	TOwnershipGuard<AdaptFriendShoutOut>	m_pFriendContents;
	TOwnershipGuard<AdaptTutorialQuest>		m_AdaptTutorialSystem;
	TOwnershipGuard<AdaptBattleSystem>		m_AdaptBattleSystem;
};

#include "NewCashshopContents.inl"