//2017-04-20-nova
#pragma once

class PersonStoreContents: public IUIContentSubject< PersonStoreContents >
{
public:
	struct sSellInfo
	{
		sSellInfo() : uPrice( 0 ) { ItemData.Clear(); }
		~sSellInfo() {}

		cItemData	ItemData;
		u8			uPrice;
	};

	enum eVALUE_TYPE
	{ 
		VT_RES_PERSON, 
		VT_RES_PERSON_START, 
		VT_RES_EMPLOYMENT, 
		VT_RES_EMPLOYMENT_START, 
		VT_OPEN_PERSON, 
		VT_OPEN_EMPLOYMENT,
		VT_CLOSE_STORE			// UI창이 닫혀있는 상태
	};

	enum eEvent
	{
		eBtnDynamicOK,
		eEnableWindow,
		eDisableWindow,
		eSetTamerName,
		eSetStoreName,
		eReady,
		eOpen,
		eUpdateItem
	};

public:
	PersonStoreContents(void);
	virtual ~PersonStoreContents(void);
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void)							{ return true; }
	virtual void UnInitialize(void)							{}
	virtual bool IntraConnection(ContentsSystem& System)	{ return true; }
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt)		{}
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void)					{}
	virtual void MakeWorldData(void)						{}
	virtual void ClearWorldData(void);

	static int const IsContentsIdentity(void);

protected:
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	int						iUsedItemID;
	int						iSelectIndex;
	UINT					iTargetUniqID;

	bool					m_bWaitRecvData;		// 서버에서 구매 응답 확인용

	std::wstring			wsTamerName;
	std::wstring			wsStoreName;

	eVALUE_TYPE				m_eValueType;
	std::vector< sSellInfo* >	m_vpSellInfo;

	CsC_AvObject*			m_pTarget;			// target 정보

	typedef CsBaseMng::sPERSON_STORE	PERSONSTORE;

	// ========================== 이벤트 함수 ==========================
	void					Req_PresonStoreOpen(void* pData);
	void					Sell_ResistItem(void* pData);
	void					Buy_TradeItem(void* pData);
	//void					Set_StoreName(void* pData);

	//void					Get_StoreName(void* pData);
	//void					Get_ValueType(void* pData);

	void					OpenPersonStore( void* pData );
	void					OpenCommissionStore( void* pData );

	void					ClosePersonStoreMsgBox( void* pData );

	// ===================================================================

	// ======================== 패킷 받는 함수 ========================
	void					RecvClosePersonStore( void* pData );
	void					RecvCloseCommissionStore( void* pData );
	void					RecvStoreReadyResult( void* pData );			// 개인/위탁 같이 씀
	void					RecvOpenPersonStore( void* pData );
	void					RecvOpenCommissionStore( void* pData );
	void					RecvBuyItemResult( void* pData );
	void					RecvPersonStoreItemList( void* pData );
	void					RecvCommissionStoreItemList( void* pData );
	// ===================================================================

	// ======================= 패킷 보내는 함수 =======================
	bool					SendOpenPersonStore( std::wstring const& wsShopName );
	bool					SendOpenCommissionStore( std::wstring const& wsShopName );
	void					SendRequestItemList();
	void					SendRequestReady( int nReqType, uint nInvenIndex = 0 );
	void					SendRequestWithdraw();
	// ===================================================================

	// cancel 눌렀을 때 호출됨
	void					ClosePersonStore();
	void					CloseCommissionStore();

	void					CheckTarget();
	bool					CheckTamerState();
	bool					CheckDistance();
	bool					CheckStoreName();
	bool					CheckSlotCount();
	
	void					LockInventory();
	void					UnlockInventory();

	bool					Resist_Item_Money(cItemData* pItem, u8 uPrice);
	void					Item_Buy( cItemData* pItem );
	void					ItemTrade_BuySuccess(int nSlotIndex, int nCount);
	void					ItemTrade_BuyFail( int nResult );
	sSellInfo*				GetSellItemInfo( cItemData* pCmpItem, u8 uPrice );

	u8						_CalTexMoney( u8 nNeedMoney );	

	void					SellResult( sSellInfo* pSellInfo, int nCount );
	void					BuyResult( sSellInfo* pSellInfo, int nCount );

	void					DelResistItem();

public:	
	void					RegisterSeparateItem( int const& nInvenIndex );

	void					SetStoreName( std::wstring wsName );

	eVALUE_TYPE				GetValueType();
	int						GetOpenSlotCount() const;
	int						GetSellInfoSize() const;
	sSellInfo*				GetSellInfo( int nIndex ) const;

	bool					IsRegisterItem() const;

	void					PressDynamicButton();
	void					PressItemButton( int nIndex );

	void					CloseWindowUI();
};
