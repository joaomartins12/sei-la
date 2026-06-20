#pragma once 

#include "CsGotcha.h"

class CsGotchaMng
{
public:
	void			Delete();
	bool			Init( char* cPath );

protected:
	CsGotcha_Machine::MAP	m_mapGotcha_Machine;	
	CsGotcha_Item::MAP		m_mapGotcha_Item;
	CsGotcha_RareItem::MAP	m_mapGotcha_RareItem;

	CsGotcha_Mystery_Item::MAP	m_mapGotcha_Mystery_Item;
	CsGotcha_Mystery_Coin::MAP	m_mapGotcha_Mystery_Coin;

protected:
	void			_LoadExcel();
	void			_LoadGotcha_Machine();
	void			_LoadGotcha_Item();
	void			_LoadGotcha_Mystery_Item();
	void			_LoadGotcha_Mystery_Coin();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	bool			IsGotcha_NPC( int nID ){ return ( m_mapGotcha_Machine.find( nID ) != m_mapGotcha_Machine.end() ); }
	bool			IsGotcha_Item(int nID ){ return ( m_mapGotcha_Item.find( nID ) != m_mapGotcha_Item.end() ); }
	CsGotcha_Machine*			GetGotcha_Machine( int n_Npc_ID, int nIndex );
	USHORT			GetGotcha_Item( int n_ItemID );
 	CsGotcha_RareItem*			GetGotcha_RareItem(int nindex );
	bool						GetGotcha_Mystery_IsEffect( int nItem );
	CsGotcha_Mystery_Coin*		GetGotcha_Mystery_Coin( int nOrder );
	void			SetRareItem(CsGotcha_RareItem* m_Rare, int nIndex );
	int				GetRareItemIndex(int n_Npc_ID, int nItemID);

	DWORD			GetGotchaMachineFIDX( DWORD dwNpcID ) const;
	bool			IsEnableGotchaMachine( DWORD dwFIDX ) const;
	bool			IsEnableLevel( DWORD dwFIDX, int nCurLevel ) const;

#ifdef EGG_PREFERMENT
	UINT			 Get_Req_Money( int nMachineNo );	// 요구 금액 확인
#endif

public:
	void			SaveBin( char* cPath );	

	UINT			GetDate(char* nDate );
	UINT			GetTime(char* nTime );

	int R_Idx ;//레어아이템 인덱스
};

