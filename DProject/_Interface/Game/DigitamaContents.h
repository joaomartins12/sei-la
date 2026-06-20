//2017-03-22-nova
#pragma once
#include "../Adapt/AdaptCommunity.h"

#define DEFAULT_ARROW_POSY			55
#define IF_DSCAN_ITEM_COUNT			5
#define IF_DIGITAMA_PROCESS_TIME	1.5f
#define IF_DIGITAMA_ICONSIZE		CsPoint( 32, 32 )
#define IF_DIGITAMA_SELL_ITEM_TYPE1	91
#define IF_DIGITAMA_SELL_ITEM_TYPE2	92
#define IF_DIGITAMA_SELL_ITEM_TYPE3	93

#define IF_SCAN_MAX_COUNT 999

class DigitamaContents: public IUIContentSubject< DigitamaContents >
{
public:
	enum eEvent
	{
		eScanFail,
		eScanStart,
		eScanCancel,
		eScanEnd,
		eUpdateScanItem,
		eRegisterScanItem,
		eSellFail,
		eSellStart,
		eSellCancel,
		eSellEnd,
		eRegisterSellItem,
	};

public:
	DigitamaContents(void);
	virtual ~DigitamaContents(void);
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void)							{ return true; }
	virtual void UnInitialize(void)							{}
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime)					{}
	virtual void NotifyEvent(int const& iNotifiedEvt)		{}
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void)					{}
	virtual void MakeWorldData(void)						{}
	virtual void ClearWorldData(void)						{}

	static int const IsContentsIdentity(void);

protected:
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	////////////////////////////////////////////////////////////////
	// Digitama Scan Info
	cItemInfo*					m_pRegistItem;
	std::list< cItemInfo >		m_lScanItem;

	int				m_nRegisterIndex;
	int				m_nPotableIndex;
	int				m_nSearchIndex;
	CsC_AvObject*	m_pTarget;
	NiPoint2		m_niTargetPos;
	////////////////////////////////////////////////////////////////

	bool			m_bRecvPacket;

private:
	////////////////////////////////////////////////////////////////
	// Scan
	bool			_SearchScanItem( int nBegin, int nEnd );
	bool			_SetScanItem( int nInvenIndex );
	u8				_CalcScanMoney( uint nItemType, bool bIsMsg = false );
	void			_ClearScanItem();

#ifdef UI_ITEMSCAN_RENEWAL
	void			_UpdateScannedItems( uint nUsedItem, std::list<nsItemScan::sScanInfo>& lScanItems, bool bIsSkip );
	void			_UpdateInvenMoney( u4 const& nUsedItem, u2 const& nUsedCount, n8 const& nRemainMoney, n8 const& nUsedMoney );
#endif

	bool			_SendScanItem( uint nCount );

#ifndef UI_ITEMSCAN_RENEWAL
	void			_AddScannedItem( cItemData& pData );
#endif
	////////////////////////////////////////////////////////////////
	// Sell
	bool			_SearchSellItem( int nBegin, int nEnd, int nType );
	bool			_SetSellItem( int nInvenIndex );
	u8				_CalcSellMoney( uint nItemType, uint nItemCount );

	////////////////////////////////////////////////////////////////
	// Register
	bool			_SetRegistItem( int nInvenIndex );
	void			_ResetRegistItem();
	bool			_CheckRegisterItem( bool bIsScan = true );
	bool			_CheckScanMoney( int& nCount );
	bool			_EnableRegistItem();

	void			_RegisterCancel();
	void			_UnLockRegistIndex();

	////////////////////////////////////////////////////////////////
	void			_LockInventory();
	void			_UnLockInventory();
	void			_InventoryEnable(bool enable);	//2017-04-18-nova 인벤토리 활성화/비활성화
	void			_SetDigitamaWindow( cBaseWindow::eWINDOW_TYPE eType, int nIndex );

public:
	//Regist Item
	cItemInfo*		Get_ResistItem_Ptr();
	bool			Get_ResistItemEnable();
	uint			Get_ResistItemCount() const;
	uint			Get_ResistItemType() const;

	////////////////////////////////////////////////////////////////
	// Scan
	void			RecvScanItem( void* pData );
	void			StartDigitamaScan(void* pData);

	void			ProcessStart();
	void			ProcessSuccess();
	void			ProcessCancel();
	void			ProcessSkip();
	void			AutoRegisterScan();

	std::list< cItemInfo >*		GetScanItem();

	////////////////////////////////////////////////////////////////
	// Sell
	void			Sell_Success(void* pData);
	void			StartDigitamaSell(void* pData);

	void			ProcessStartSell();
	void			ProcessSuccessSell();
	void			ProcessCancelSell();
	void			AutoRegisterItemSell();

	////////////////////////////////////////////////////////////////
	void			OpenDigitamaNpc(void* pData);
	void			OpenDigitamaPortable(void* pData);
	void			CloseDigitama(void* pData);

	void			RegisterItem(void* pData);
#ifdef UI_ITEMSCAN_RENEWAL
	void			AllScanItem(void* pData);
	int				GetItemRank( uint nScanItemType, uint nItemType );
#endif

	CsC_AvObject*	GetTargetNpc();
	NiPoint2		GetTargetPos();

	const TOwnershipGuard<AdaptCommunityContesnt>* GetAdaptCommunity() const;
private:
	TOwnershipGuard<AdaptCommunityContesnt>	m_pAdaptCommunity;
};
