
#pragma once 

struct sMONEY
{
	sMONEY();
	sMONEY( u8 nMoney );
	sMONEY( int nTera, int nMega, int nBit );
	sMONEY(int nPrPA, int nPrPB, int nPrPC, int nPrPD );
	sMONEY( sMONEY* src );
	~sMONEY();

	void	SetInvalideData();
	void	Init();

	void	IncreaseBit( int nBit );
	void	IncreaseMega( int nMega );
	void	IncreaseTera( int nTera );

	void	DecreaseBit( int nBit );
	void	DecreaseMega( int nMega );
	void	DecreaseTera( int nTera );

	void	SetInvalideDataProperty();
	void	InitProperty();

	void	IncreaseProperty_A( int nProperty_A );
	void	IncreaseProperty_B( int nProperty_B );
	void	IncreaseProperty_C( int nProperty_C );
	void	IncreaseProperty_D( int nProperty_D );

	void	DecreaseProperty_A( int nProperty_A );
	void	DecreaseProperty_B( int nProperty_B );
	void	DecreaseProperty_C( int nProperty_C );
	void	DecreaseProperty_D( int nProperty_D );

	int		GetProperty_A();
	int		GetProperty_B();
	int		GetProperty_C();
	int		GetProperty_D();

	void	SetProperty_A(int nMoney);
	void	SetProperty_B(int nMoney);
	void	SetProperty_C(int nMoney);
	void	SetProperty_D(int nMoney);

	int		GetProperty();

	int		GetTera();
	int		GetMega();
	int		GetBit();

	u8		GetInt();
	
protected:
	int		s_nTera;
	int		s_nMega;
	int		s_nBit;

protected:
	int		s_nProperty_A;
	int		s_nProperty_B;
	int		s_nProperty_C;
	int		s_nProperty_D;

public:
	void	operator=(const u8 nMoney){ s_nTera = (int)( nMoney/1000000 ); s_nMega = (int)( (nMoney%1000000)/1000 ); s_nBit=(int)(nMoney%1000); }

	sMONEY	operator+(const sMONEY src){ sMONEY m( *this ); m.IncreaseBit( src.s_nBit ); m.IncreaseMega( src.s_nMega ); m.IncreaseTera( src.s_nTera ); return m; }
	sMONEY	operator+=(const sMONEY src){ IncreaseBit( src.s_nBit ); IncreaseMega( src.s_nMega ); IncreaseTera( src.s_nTera ); return *this; }
	sMONEY	operator+(const u8 src){ return *this + sMONEY( src ); }
	sMONEY	operator+=(const u8 src){ return ( *this += sMONEY( src ) ); }

	sMONEY	operator-(const sMONEY src){ sMONEY m( *this ); m.DecreaseBit( src.s_nBit ); m.DecreaseMega( src.s_nMega ); m.DecreaseTera( src.s_nTera ); return m; }
	sMONEY	operator-=(const sMONEY src){ DecreaseBit( src.s_nBit ); DecreaseMega( src.s_nMega ); DecreaseTera( src.s_nTera ); return *this; }
	sMONEY	operator-(const u8 src){ return *this - sMONEY( src ); }
	sMONEY	operator-=(const u8 src){ return ( *this -= sMONEY( src ) ); }

	bool	operator==(const sMONEY src){ return ( (s_nTera==src.s_nTera)&&(s_nMega==src.s_nMega)&&(s_nBit==src.s_nBit) ); }
	bool	operator==(const u8 src){ return ( *this==sMONEY(src) ); }

	bool	operator>=(const sMONEY src){ return (s_nTera==src.s_nTera) ? (s_nMega==src.s_nMega) ? (s_nBit>=src.s_nBit) : s_nMega > src.s_nMega : s_nTera > src.s_nTera; }
	bool	operator>=(const u8 src){ return ( *this>=sMONEY(src) ); }
	bool	operator>(const sMONEY src){ return (s_nTera==src.s_nTera) ? (s_nMega==src.s_nMega) ? (s_nBit>src.s_nBit) : s_nMega > src.s_nMega : s_nTera > src.s_nTera; }
	bool	operator>(const u8 src){ return ( *this>sMONEY(src) ); }
	bool	operator<=(const sMONEY src){ return (s_nTera==src.s_nTera) ? (s_nMega==src.s_nMega) ? (s_nBit<=src.s_nBit) : s_nMega < src.s_nMega : s_nTera < src.s_nTera; }
	bool	operator<=(const u8 src){ return ( *this<=sMONEY(src) ); }
	bool	operator<(const sMONEY src){ return (s_nTera==src.s_nTera) ? (s_nMega==src.s_nMega) ? (s_nBit<src.s_nBit) : s_nMega < src.s_nMega : s_nTera < src.s_nTera; }
	bool	operator<(const u8 src){ return ( *this<sMONEY(src) ); }
};

class cData_Inven
{
public:
	enum eINVEN_INDEX{ INVALIDE_INVEN_INDEX = INT_MAX, };
public:
	cData_Inven();

public:
	void	Delete(){}
	void	Init(){}
	void	Reset();

	void	ResetAllItem();

	void	InitNetOff();

	void	LoadApply();

	static bool		IsTradeLimitedItem( cItemData const* pData );

protected:
	sMONEY				m_Money;

	uint				m_nInvenSlotCount;
	int					m_nInvenGroupCount;

	std::list< int >	m_listLock_ItemID;
	std::list< int >	m_listLock_ItemType;
	cItemInfo			m_pInven[nLimit::Inven];					// 000~099 1번탭, 700~799 8번탭
	cItemInfo			m_pInvenBackup[nLimit::Inven];				// 000~099 1번탭, 700~799 8번탭

public:
	void			Backup_Save();		// 현재 인벤 정보 저장
	void			Backup_Load();		// 백업 인벤 정보 로드

	void			SetMoney( n8 nMoney, bool bCheckAchieve );
	void			SetInvenSlotCount( int nCount );
	int				GetInvenGroupCount(){ return m_nInvenGroupCount; }

	//	2016.01.04 추가
	u4				GetTypeToIndex(int Type);	// 현재의 아이템 타입(ID)이 인벤토리 몇번째에 존재 하는지 알려준다.
	u4				GetTypeToConut(int Type);	// 현재의 아이템 타입(ID)이 인벤토리에 몇개 존재 하는지 알려준다.
	cItemInfo*		GetTypeToItem(int Type);	// 현재의 아이템 타입(ID)이 인벤토리에 존재하면 아이템을 알려준다.
	u4				GetTypeToIndex(nItem::eType eType);	// 현재의 아이템 타입(nItem::eType)이 인벤토리 몇번째에 존재 하는지 알려준다.

	//
	sMONEY			GetMoney(){ return m_Money; }
	sMONEY			IncreaseMoney( u8 nValue, bool bCheckAchieve );
	sMONEY			IncreaseMoney( sMONEY nValue, bool bCheckAchieve );
	sMONEY			DecreaseMoney( u8 nValue );
	sMONEY			DecreaseMoney( sMONEY nValue );

	uint			GetInvenSlotCount(){ return m_nInvenSlotCount; }
	cItemInfo*		GetDataArray(){ return m_pInven; }
	cItemInfo*		GetData( int nIndex );
	CsItem::sINFO*	GetFTData( int nIndex );

	void			SetInvenItemData( int nIndex ,const cItemData& rInfo );
	void			SetInvenItemEndTime( int nIndex ,int endtime );				//2017-04-04-nova 시간제 아이템 적용시 종료시간 설정

	bool			IsExistItem( int nIndex );
	bool			IsSlot();	

	//===============================================================================================================================
	//	ItemID
	//===============================================================================================================================
	bool			IsSlot( int nItemID, int nCount );
	int				FindEmptySlot( int nSearchIndex );							// nSearchIndex = -1이라면 처음부터 검색 ( 해당인덱스 자체는 무시되고 1증가된 인덱스부터 검색된다 )
	int				FindEmptySlot( int nSearchIndex, int nExceptSlot );			// nSearchIndex = -1이라면 처음부터 검색 ( 해당인덱스 자체는 무시되고 1증가된 인덱스부터 검색된다 )
	int				GetEmptySlotCount();
	int				GetFirstSlot_Item_ID( int nItemID );	
	int				GetSlot_Item_ID_ReverseToSlot_G(int nSlot, int nItemID );	
	int				GetFirstSlot_Item_ID_Rate( int nItemID, int nRate, bool bPassLimited );
	int				GetFirstSlot_Item_TypeLS( int nTypeL, int nTypeS );
	int				GetSlot_Item_ID_ReverseToSlot( int nSlot, int nItemID );
	int				GetSlot_Item_ID_Count( uint const& nItemID, uint const& nItemCount );

	int				GetFirstSlot_ItemID( int nItemID1, int nItemID2 );	
	DWORD			GetCount_Item_ID( uint const& dwItemID, bool const& bCheckLimitedTime = false ) const;
	void			GetItemDatas_ItemID( uint const& dwItemID, DWORD const& nTotalCount, std::map<int,int>& items, bool const& bCheckLimitedTime = false );
	void			GetItemDatas_ItemType( uint const& dwItemType, DWORD const& nTotalCount, std::map<int,cItemData>& items, bool const& bCheckLimitedTime = false );

	int				GetCount_Item_TypeL( int nType_L );
	int				GetCount_Item_TypeLS( int nType_L, int nType_S );
	int				GetCount_Item_ID_Rate( int nItemID, int nRate );
	int				GetCount_Item_EnableCrop( int nItemID );
	int				GetCount_Item_EnableCrop( const cItemData* pItemData );

	//int				GetSlotNumber_Crop( const cItemData* pItemData );


	void			Itemlock_InvenSlot( int nIndex );
	void			ItemUnlock_InvenSlot( int nIndex );

	void			ItemLock_ItemID( int nItemID );
	void			ItemLock_ItemID_ELSE( int nItemID );
	void			ItemUnlock_AllItem();

	void			ItemUnlock_ItemID( int nItemID );
	bool			UpdateInvenItem( int const& nInvenPos, cItemData const& cItemData, bool bCoolDown );

	bool			DecreaseItem( int const& nInvenPos, DWORD const& nItemID, int const& nCount, bool bCoolDown );
	void			DecreaseItem( int const& nItemID, int nCount, bool bCoolDown, bool bPassLimited );
	void			DecreaseItem( int const& nItemID, int nRate, int nCount, bool bCoolDown, bool bPassLimited );
	void			DecreaseItem_TypeLS( int nTypeL, int nTypeS, int nCount, bool bCoolDown, bool bPassLimited );	

	void			SetCoolTime( int const& nItemID );

protected:
	void			_InsertLock_ItemID( int nItemID );
	void			_ReleaseLock_ItemID( int nItemID );
	bool			_IsLock_ItemID( int nItemID );


	//===============================================================================================================================
	//	ItemType
	//===============================================================================================================================
public:
	int				GetFirstSlot_ItemType( int nType_L, int nType_S );

	void			ItemLock_ItemType( int nType_L, int nType_S );
	void			ItemUnlock_ItemType( int nType_L, int nType_S );

protected:
	void			_InsertLock_ItemType( int nType_L, int nType_S );
	void			_ReleaseLock_ItemType( int nType_L, int nType_S );
	bool			_IsLock_ItemType( int nType_L, int nType_S );




public:
	bool			ItemMove( int nSrcIndex, int nDestIndex, int nSrcCount );
protected:
	void			_ItemMoveAllCount( int nSrcIndex, int nDestIndex );
	bool			_ItemMoveCount( int nSrcIndex, int nDestIndex, int nSrcCount );

public:
#ifdef NEW_SHARESTASH
	int				CalItemSplit( int nSrcSrvID, int nDestSrvID, int nSrcCount );	
#else
	int				CalItemSplit( int nSrcIndex, int nDestIndex, int nSrcCount );
#endif
public:
	void			ItemDrop( int nIndex, int nCount );
#ifdef TRICK_ORTREAT
	void			ItemRemove_NoServer( int nItemID,  bool bCalQuest);
#endif
	void			ItemRemove_NoServer( int nItemID, int nCount, bool bCalQuest );
	bool			ItemRemove_NoServer( int const& nInvenSlotNum, int nItemID, int nCount, bool bCalQuest );
	void			ItemRemove_NoServer( int nItemID, int nRate, int nCount, bool bCalQuest, bool bRemoveLimited );
	void			ItemRemove_NoServer_Type( int nType_L, int nType_S, int nCount, bool bCalQuest );
	int				ItemCrop( int nItemID, int nCount, int nTradeLimitTime = 0 );
	int				ItemCrop( cItemData* pItemData, int nCount = 0 );
#ifdef REPURCHASE
	int				ItemCrop_Repurchase( cItemData* pItemData );
#endif

	void			SetItemClone( cItemInfo* pSrc ){ memcpy( m_pInven, pSrc, sizeof( cItemInfo )*nLimit::Inven ); }
	void			GetItemClone( cItemInfo* pDest ){ memcpy( pDest, m_pInven, sizeof( cItemInfo )*nLimit::Inven ); }




protected:
	int				m_nReserveCapsuleInvenIndex;
	cItemData		m_ReserveCapsuleItem;

public:
	void			SetCapsuleInvenIndex( int nIndex ){ m_nReserveCapsuleInvenIndex = nIndex; }
	int				GetCapsuleInvenIndex(){ return m_nReserveCapsuleInvenIndex; }

	cItemData*		GetCapsuleItem(){ return &m_ReserveCapsuleItem; }
	void			SetCapsuleItem( cItemData item ){ m_ReserveCapsuleItem = item; }
	void			ReleaseCapsuleItem(){ m_ReserveCapsuleItem.Clear(); }
#ifdef TRICK_ORTREAT
	float			GetEventTime();
#endif
	// [3/24/2016 hyun] 획득 메시지 출력
	void			OutputCropMsg(cItemData* const pItemData, int nCount = 0);

private: 
	TOwnershipGuard<AdaptWareHouse> m_pWarehouseAdapt; 
};