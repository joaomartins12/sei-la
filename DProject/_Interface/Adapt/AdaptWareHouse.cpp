#include "StdAfx.h"
#include "AdaptWareHouse.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Game/WareHouseContents.h"

AdaptWareHouse::AdaptWareHouse(void):m_pWarehouseContents(NULL)
{
}

AdaptWareHouse::~AdaptWareHouse(void)
{
}

bool AdaptWareHouse::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, NULL );
	if( E_CT_WAREHOUSE != pContents->GetContentsIdentity() )
		return false;

	m_pWarehouseContents = static_cast< WareHouseContents* >( pContents );
	m_pWarehouseContents->Reset();

	return true;
}

cItemInfo* AdaptWareHouse::GetData(int nIdx)
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents,NULL);
	SAFE_POINTER_RETVAL(m_pWarehouseContents->GetData(nIdx), NULL);

	return m_pWarehouseContents->GetData(nIdx);
}

cItemInfo* AdaptWareHouse::GetShareData(int nIdx) const
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents,NULL); 
	SAFE_POINTER_RETVAL(m_pWarehouseContents->GetShareData(nIdx), NULL);

	return m_pWarehouseContents->GetShareData(nIdx);
}

int AdaptWareHouse::GetSlotCount() const
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents, NULL);
	return m_pWarehouseContents->GetSlotCount();
}

int AdaptWareHouse::GetShareSlotCount() const
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents, NULL);
	return m_pWarehouseContents->GetShareSlotCount();
}

void AdaptWareHouse::InvenToWarehouse( int nInvenIndex, int nWarehouseIndex, int nCount )
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->InvenToWarehouse( nInvenIndex, nWarehouseIndex, nCount );
}

void AdaptWareHouse::WarehouseToWarehouse( int nSrcIndex, int nDestIndex, int nCount )
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->WarehouseToWarehouse( nSrcIndex, nDestIndex, nCount );
}

void AdaptWareHouse::WarehouseToInven( int nWarehouseIndex, int nInvenIndex, int nCount )
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->WarehouseToInven( nWarehouseIndex, nInvenIndex, nCount );
}

int AdaptWareHouse::GetCount_Item_ID( int nItemID ) const
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents, NULL);
	return m_pWarehouseContents->GetCount_Item_ID(nItemID);
}

#ifdef NEW_SHARESTASH

void AdaptWareHouse::InvenToShareStash( int nInvenIndex, int nShareIndex, int nCount )
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->InvenToShareStash( nInvenIndex, nShareIndex, nCount );
}

void AdaptWareHouse::WarehouseToShareStash( int nWarehouseIndex, int nShareIndex, int nCount )
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->WarehouseToShareStash( nWarehouseIndex, nShareIndex, nCount );
}

void AdaptWareHouse::ShareStashToShareStash( int nSrcIndex, int nDestIndex, int nCount )
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->ShareStashToShareStash( nSrcIndex, nDestIndex, nCount );
}

void AdaptWareHouse::ShareStashToInven( int nShareIndex, int nInvenIndex, int nCount )
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->ShareStashToInven( nShareIndex, nInvenIndex, nCount );
}

void AdaptWareHouse::ShareStashToWarehouse( int nShareIndex, int nWarehouseIndex, int nCount )
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->ShareStashToWarehouse( nShareIndex, nWarehouseIndex, nCount );
}

int AdaptWareHouse::CalItemSplit( int nSrcSrvID, int nDestSrvID, int nSrcCount ) const
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents, NULL);
	return m_pWarehouseContents->CalItemSplit( nSrcSrvID, nDestSrvID, nSrcCount );
}

int AdaptWareHouse::CalItemSplit_ShareStash( int nSrcSrvID, int nDestSrvID, int nSrcCount ) const
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents, NULL);
	return m_pWarehouseContents->CalItemSplit_ShareStash( nSrcSrvID, nDestSrvID, nSrcCount );
}

int AdaptWareHouse::GetCount_Item_ID2( int nItemID ) const
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents, NULL);
	return m_pWarehouseContents->GetCount_Item_ID2(nItemID);
}	

int AdaptWareHouse::GetSlot_Item_ID_ReverseToSlot2( int nSort, int dwItemID ) const
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents, NULL);
	return m_pWarehouseContents->GetSlot_Item_ID_ReverseToSlot2(nSort, dwItemID);

}

void AdaptWareHouse::_SendShareStashSort()
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->_SendShareStashSort();
}

void AdaptWareHouse::_ReleaseShareStashSort(bool Check)
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->_ReleaseShareStashSort(Check);
}

#else

int AdaptWareHouse::CalItemSplit( int nSrcIndex, int nDestIndex, int nSrcCount ) const
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents, NULL);
	return m_pWarehouseContents->CalItemSplit( nSrcIndex, nDestIndex, nSrcCount );
}

#endif

#ifdef NEW_WAREHOUSESORT
int AdaptWareHouse::GetSlot_Item_ID_ReverseToSlot( int nSort, int dwItemID ) const
{
	SAFE_POINTER_RETVAL(m_pWarehouseContents, NULL);
	return m_pWarehouseContents->GetSlot_Item_ID_ReverseToSlot(nSort, dwItemID);
}

void AdaptWareHouse::_SendWareHouseSort()
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->_SendWareHouseSort();
}

void AdaptWareHouse::_ReleaseWareHouseSort(bool	Check)
{
	SAFE_POINTER_RET(m_pWarehouseContents);
	m_pWarehouseContents->_ReleaseWareHouseSort(Check);
}
#endif
