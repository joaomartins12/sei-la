
#pragma once 
#ifdef CROSSWARS_SYSTEM

class AdaptWareHouse; 
#include "ContentsSystem/ContentsSystem.h"

#define IF_CROSSINVENSLOT_COUNT		15
class cData_Inven_Cross : public NiMemObject
{
private: 
	TOwnershipGuard<AdaptWareHouse> m_pWarehouseAdapt; 
public:
	enum eINVEN_CROSSINDEX{ INVALIDE_INVENCROSS_INDEX = INT_MAX, };
public:
	cData_Inven_Cross();

public:
	void	Delete(){}
	void	Init(){}
	void	Reset();

	void	InitNetOff();

	void	LoadApply();

protected:	
	uint				m_nInvenSlotCount;
	bool				m_bCrossLoader;

	std::list< int >	m_listLock_ItemID;
	std::list< int >	m_listLock_ItemType;
	cItemInfo			m_pInven[IF_CROSSINVENSLOT_COUNT];
	

protected:

	void			_InsertLock_ItemID( int nItemID );
	void			_ReleaseLock_ItemID( int nItemID );
	bool			_IsLock_ItemID( int nItemID );

	void			_InsertLock_ItemType( int nType_L, int nType_S );
	void			_ReleaseLock_ItemType( int nType_L, int nType_S );
	bool			_IsLock_ItemType( int nType_L, int nType_S );
public:
	cItemInfo*		GetDataArray(){ return m_pInven; }
	cItemInfo*		GetData( int nIndex ){ assert_cs( nIndex < IF_CROSSINVENSLOT_COUNT ); return &m_pInven[ nIndex ]; }
	uint			GetInvenSlotCount(){ return m_nInvenSlotCount; }
	bool			IsCrossLoader(){return m_bCrossLoader;}

public:
	int				CalItemSplit( int nSrcSrvID, int nDestSrvID, int nSrcCount );

	void			SetInvenSlotCount( int nCount );
	void			SetCrossLoader( int nEnable );


	int				FindEmptySlot( );				
	int				ItemCrop( int nItemID, int nCount );
	int				ItemCrop( cItemData* pItemData );

	void			InvenToCross( int nSrcIndex, int nDestIndex, int nCount );
	void			CrossToInven( int nSrcIndex, int nDestIndex, int nCount );
	void			CrossToCross( int nSrcIndex, int nDestIndex, int nCount );
};
#endif