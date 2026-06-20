#pragma once
#include "../../ContentsSystem/ContentsSystem.h"

class WareHouseContents
	: public IUIContentSubject< WareHouseContents >
{
public:
	enum eWAREHOUSE_TYPE{ WAREHOUSE_NORMAL,WAREHOUSE_SHARE,};
	enum eWAREHOUSE_INDEX{ INVALIDE_WAREHOUSE_INDEX = INT_MAX, };
	struct sWAREHOUSE
	{
		void Reset(){nIdx = 0; nDecreaseCount = 0; nItemCode = 0; };

		int nIdx;
		int nDecreaseCount;
		unsigned int nItemCode;
	};
public:
	WareHouseContents(void);
	~WareHouseContents(void);

private:
	uint			m_nSlotCount;
	map<int, cItemInfo> m_mapData;
	int				m_nSort;
	bool			m_bFirstConnect;
 
	uint			m_nShareSlotCount; 
	map<int, cItemInfo> m_mapShareData; 
	CsCoolTimeSeq	m_ShareStashSortSeq;
	std::list< DWORD >	m_ShareStashListItem; 
 
	std::list< DWORD >		m_WareHouseListItem;	
	CsCoolTimeSeq			m_WareHouseSortSeq; 

//	cItemInfo*		m_pDataArray;


public:	
	static int const IsContentsIdentity(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

protected:
	void	WareHouse_ToggleCheck(void* pData);

	void	WareHouse_ItemRemoveNoserver(void* pData); 

	void	WareHouse_Set_NormalData(void* pData);
	void	WareHouse_Set_ShareData(void* pData); 
	void	WareHouse_Item_Delete( void* pData );

	void	WareHouse_SetNormalSlotCount(void* pData);
	void	WareHouse_SetShareSlotCount(void* pData);

	void	WareHouse_NormalDecreaseCount(void* pData);
	void	WareHouse_ShareDecreaseCount(void* pData);

	void	WareHouse_Reset(void* pData);
	void	WareHouse_EndTime(void* pData);

	void	WareHouse_GetCount(void* pData);
	void	WareHouse_GetEventTime(void* pData);

	void	WareHouse_TradeLimitedItem_Timeout(void* pData);

#ifdef WAREHOUSE_SERVER_SORT
	void	WareHouse_RecvItemSortEnd(void* pData);
	void	WareHouse_RecvReqResult(void* pData);

	void	_UpdateWareHouse( std::list< cItemData >& lItems, int nSlotCount );
#ifdef NEW_SHARESTASH
	void	_UpdateShareStash( std::list< cItemData >& lItems, int nSlotCount );
#endif //NEW_SHARESTASH
#endif

public:
	void	Reset();
 
	cItemInfo* GetData(int nIdx); 
	cItemInfo* GetShareData(int nIdx);

//	CsCoolTimeSeq*	WareHouseSortCoolTime() ;
	int		GetSlotCount() const;
	int		GetShareSlotCount() const;

	int		FindEmptySlot();
	int		FindEmptyShareSlot();

public:
	//		하나의 함수로 통일 하자
	void			InvenToWarehouse( int nInvenIndex, int nWarehouseIndex, int nCount );
	void			WarehouseToWarehouse( int nSrcIndex, int nDestIndex, int nCount );
	void			WarehouseToInven( int nWarehouseIndex, int nInvenIndex, int nCount );
#ifdef NEW_SHARESTASH
	void			InvenToShareStash( int nInvenIndex, int nShareIndex, int nCount );	//
	void			WarehouseToShareStash( int nWarehouseIndex, int nShareIndex, int nCount );	//

	void			ShareStashToShareStash( int nSrcIndex, int nDestIndex, int nCount );
	void			ShareStashToInven( int nShareIndex, int nInvenIndex, int nCount );
	void			ShareStashToWarehouse( int nShareIndex, int nWarehouseIndex, int nCount );	//

	int				GetCount_Item_ID2( int nItemID ) const;
	int				GetSlot_Item_ID_ReverseToSlot2( int nSlot, int nItemID ) const;
#endif

public:
#ifdef NEW_SHARESTASH
	int				CalItemSplit( int nSrcSrvID, int nDestSrvID, int nSrcCount );
	int				CalItemSplit_ShareStash( int nSrcSrvID, int nDestSrvID, int nSrcCount );
#else
	int				CalItemSplit( int nSrcIndex, int nDestIndex, int nSrcCount );
#endif
	int				GetCount_Item_ID( int nItemID ) const; 
#ifdef NEW_WAREHOUSESORT
	int				GetSlot_Item_ID_ReverseToSlot( int nSlot, int nItemID ) const;
#endif

#ifdef TRICK_ORTREAT
	void			ItemRemove_NoServer( int nItemID,  bool bCalQuest );
	float			GetEventTime();
#endif

	//	심각한 버그 때문에 빠른 처리를 위해 선언..
	// 임시 코드.
	cSort					m_NewWareSort;			// 창고 정렬 기능.
	cSort					m_NewShareStashSort;	// 공유 창고 정렬 기능.

#ifdef NEW_WAREHOUSESORT
public:
	void			WareHouseSort();
	void			_SendWareHouseSort();
	void			_NextWareHouseSort();
	int				_GetMoveWareHouseSlot( DWORD dwItemID );
	void			_ReleaseWareHouseSort(bool	Check=true);
	bool			IsWareHouseSort();

	CsCoolTimeSeq*	GetWareHouseSortCoolTime();
#endif
#ifdef NEW_SHARESTASH
	void			ShareStashSort();
	void			_SendShareStashSort();
	int				_GetMoveShareStashSlot( DWORD dwItemID );
	void			_NextShareStashSort();
	void			_ReleaseShareStashSort(bool Check=true);
	bool			IsShareStashSort();

	CsCoolTimeSeq*	GetShareStashSortCoolTime();
#endif

private:
	bool			_DeleteWareHouseItem( map<int,cItemInfo>& mapData, int const& nIdx, int const& nDeleteCount = 0, DWORD const& dwItemCode = 0 );
};
