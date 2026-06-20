
#pragma once 

class cData_TEquip
{
public:
	cData_TEquip();

public:
	void	Delete(){}
	void	Init(){}
	void	Reset();

	void	InitNetOff();

protected:
	cItemInfo		m_pEquip[ nLimit::Equip ];
	cItemInfo		m_DigiviceItem;					// 차고 있는 디지바이스

public:	
	cItemInfo*		GetDataArray(){ return m_pEquip; }
	cItemInfo*		GetData( int nIndex );
	cItemInfo*		GetDigiviceItem(){ return &m_DigiviceItem; }
	
	void			SetDigiviceItem( cItemData* pDigiviceItem );
	void			SetData( int nIndex, cItemInfo* pItem, bool bCallChangePart );

	bool			IsExistItem( int nIndex );

	void			ToInven( int nEquipIndex, int nInvenIndex );
	void			ToInven_Digivice( int nInvenIndex );

	int				GetCount_Item_ID( int nItemID );

	int				GetEquipedCount();				// 현재 착용중인 테이머 장비 개수를 리턴합니다.

	void			SetImpute(cItemInfo* pItem);	// 아이템 귀속 여부
	//void			CheckBuff();
};