#pragma once

class IUIContentBase;
class WareHouseContents;

class AdaptWareHouse
{
public:
	explicit AdaptWareHouse(void);
	virtual ~AdaptWareHouse(void);

	bool			SetContents(IUIContentBase* pContents);  

	cItemInfo*		GetData(int nIdx);
	cItemInfo*		GetShareData(int nIdx) const;

	int				GetSlotCount() const;
	int				GetShareSlotCount() const;

public:
	//		하나의 함수로 통일 하자
	void			InvenToWarehouse( int nInvenIndex, int nWarehouseIndex, int nCount );
	void			WarehouseToWarehouse( int nSrcIndex, int nDestIndex, int nCount );
	void			WarehouseToInven( int nWarehouseIndex, int nInvenIndex, int nCount );
	int				GetCount_Item_ID( int nItemID ) const; 

#ifdef NEW_SHARESTASH
	void			InvenToShareStash( int nInvenIndex, int nShareIndex, int nCount );	//
	void			WarehouseToShareStash( int nWarehouseIndex, int nShareIndex, int nCount );	//

	void			ShareStashToShareStash( int nSrcIndex, int nDestIndex, int nCount );
	void			ShareStashToInven( int nShareIndex, int nInvenIndex, int nCount );
	void			ShareStashToWarehouse( int nShareIndex, int nWarehouseIndex, int nCount );	//

	int				CalItemSplit( int nSrcSrvID, int nDestSrvID, int nSrcCount ) const;
	int				CalItemSplit_ShareStash( int nSrcSrvID, int nDestSrvID, int nSrcCount ) const;

	int				GetCount_Item_ID2( int nItemID ) const;
	int				GetSlot_Item_ID_ReverseToSlot2( int nSlot, int nItemID ) const;

	void			_SendShareStashSort();
	void			_ReleaseShareStashSort(bool Check=true);
#else
	int				CalItemSplit( int nSrcIndex, int nDestIndex, int nSrcCount ) const;
#endif

#ifdef NEW_WAREHOUSESORT
	int				GetSlot_Item_ID_ReverseToSlot( int nSlot, int nItemID ) const;
	void			_SendWareHouseSort();
	void			_ReleaseWareHouseSort(bool	Check=true);
#endif

private:
	WareHouseContents* m_pWarehouseContents;
};