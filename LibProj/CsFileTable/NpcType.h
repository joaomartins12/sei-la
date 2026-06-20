
#pragma once 

class CsItem;
class CsItemExchange;

struct sNPC_TYPE_BASE
{
	nsCsNpcTable::eNPC_TYPE		s_eNpcType;
	virtual sNPC_TYPE_BASE*	Clone() = 0;
	virtual void			Delete() = 0;
	virtual void			Init() = 0;
};

struct sNPC_TYPE_NONE : public sNPC_TYPE_BASE
{
	sNPC_TYPE_NONE(){ s_eNpcType = nsCsNpcTable::NT_NONE; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};


struct sNPC_TYPE_DIGITAMA_TRADE : public sNPC_TYPE_BASE
{
	sNPC_TYPE_DIGITAMA_TRADE(){ s_eNpcType = nsCsNpcTable::NT_DIGITAMA_TRADE; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};

struct sNPC_TYPE_TRADE : public sNPC_TYPE_BASE
{
	sNPC_TYPE_TRADE( nsCsNpcTable::eNPC_TYPE eType ){ s_eNpcType = eType; }
	std::list< DWORD >		s_list_Item;

	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();

	bool					IsItem( DWORD dwItemID );
	void					AddItem( DWORD dwItemID );
	void					DelItem( DWORD dwItemID );

	int						GetItemCount(){ return (int)s_list_Item.size(); }
	DWORD					GetItemID( int nIndex );
	CsItem*					GetItem( int nIndex );
};

struct sNPC_TYPE_EXCHANGE : public sNPC_TYPE_BASE	//물물교환상점
{
	sNPC_TYPE_EXCHANGE( nsCsNpcTable::eNPC_TYPE eType ){ s_eNpcType = eType; }
	std::list< DWORD >		s_list_Item;

	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();

	bool					IsItem( DWORD dwItemID );
	void					AddItem( DWORD dwItemID );
	void					DelItem( DWORD dwItemID );

	int						GetItemCount(){ return (int)s_list_Item.size(); }
	DWORD					GetItemID( int nIndex );
	CsItemExchange*			GetExchange_Item( DWORD dwNpcID, int nIndex );

	// 물물교환 상점 판매분류가 두개 이상일 때 첫번째 tab에 있는 메뉴의 아이템만(툴팁, 구매등..) 나타나는 현상이 있어 수정	15.01.13  lks007
	CsItemExchange*			GetExchange_Item( DWORD dwNpcID, int nIndex, int nCurTab );
};

struct sNPC_TYPE_PORTAL : public sNPC_TYPE_BASE
{
#define NPC_TYPE_PORTAL_MAX_REQ_COUNT			3

	enum eENABLE_TYPE{ None, Item, Quest, TMinLevel, Money, TMaxLevel, DMinLevel, DMaxLevel };

	sNPC_TYPE_PORTAL(){ 
		s_eNpcType = nsCsNpcTable::NT_PORTAL;
		s_nPortalCount = 0;	
		s_nPortalType = 0;
	}

	struct sPORTAL_REQ
	{
		sPORTAL_REQ(){ s_eEnableType = None; s_nEnableID = s_nEnableCount = 0; }

		eENABLE_TYPE	s_eEnableType;
		DWORD			s_nEnableID;
		DWORD			s_nEnableCount;
	};

	struct sPORTAL
	{
		sPORTAL(){ s_dwEventID = 0; }
		DWORD			s_dwEventID;
		sPORTAL_REQ		s_ReqArray[ NPC_TYPE_PORTAL_MAX_REQ_COUNT ];
	};

	virtual sNPC_TYPE_BASE*	Clone();
	virtual void			Delete();
	virtual void			Init();

	void					DelPortal( int nIndex );
	sPORTAL*				NewPortal( bool bIncreasePortalCount );
	sPORTAL*				GetPortal( int nIndex );

	int						s_nPortalCount;
	int						s_nPortalType;
	std::list< sPORTAL* >	s_listPortal;
};


struct sNPC_TYPE_MAKE_TACTICS : public sNPC_TYPE_BASE
{
	sNPC_TYPE_MAKE_TACTICS(){ s_eNpcType = nsCsNpcTable::NT_MAKE_TACTICS; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};

struct sNPC_TYPE_ELEMENT_ITEM : public sNPC_TYPE_BASE
{
	sNPC_TYPE_ELEMENT_ITEM(){ s_eNpcType = nsCsNpcTable::NT_ELEMENT_ITEM; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};

struct sNPC_TYPE_WAREHOUSE : public sNPC_TYPE_BASE
{
	sNPC_TYPE_WAREHOUSE(){ s_eNpcType = nsCsNpcTable::NT_WAREHOUSE; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};

struct sNPC_TYPE_TACTICSHOUSE : public sNPC_TYPE_BASE
{
	sNPC_TYPE_TACTICSHOUSE(){ s_eNpcType = nsCsNpcTable::NT_TACTICSHOUSE; }
	virtual sNPC_TYPE_BASE* Clone(){ return csnew sNPC_TYPE_TACTICSHOUSE; }
	virtual void			Delete(){}
	virtual void			Init(){}
};

struct sNPC_TYPE_CAPSULE_MACHINE : public sNPC_TYPE_BASE
{
	sNPC_TYPE_CAPSULE_MACHINE(){ s_eNpcType = nsCsNpcTable::NT_CAPSULE_MACHINE; }
	virtual sNPC_TYPE_BASE* Clone(){ return csnew sNPC_TYPE_CAPSULE_MACHINE; }
	virtual void			Delete(){}
	virtual void			Init(){}
};

struct sNPC_TYPE_SKILL : public sNPC_TYPE_BASE
{
	sNPC_TYPE_SKILL(){ s_eNpcType = nsCsNpcTable::NT_SKILL; }
	virtual sNPC_TYPE_BASE* Clone(){ return csnew sNPC_TYPE_SKILL; }
	virtual void			Delete(){}
	virtual void			Init(){}
};

struct sNPC_TYPE_DATS_PORTAL : public sNPC_TYPE_BASE
{
	sNPC_TYPE_DATS_PORTAL(){ s_eNpcType = nsCsNpcTable::NT_DATS_PORTAL; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};
struct sNPC_TYPE_GOTCHAMACHINE : public sNPC_TYPE_BASE
{
	sNPC_TYPE_GOTCHAMACHINE(){ s_eNpcType = nsCsNpcTable::NT_GOTCHA_MACHINE; }
 	virtual sNPC_TYPE_BASE* Clone();
 	virtual void			Delete();
 	virtual void			Init();
};

struct sNPC_TYPE_MYSTERYMACHINE : public sNPC_TYPE_BASE
{
	sNPC_TYPE_MYSTERYMACHINE(){ s_eNpcType = nsCsNpcTable::NT_MYSTERY_MACHINE; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};

struct sNPC_TYPE_MASTER_MATCHING : public sNPC_TYPE_BASE
{
	sNPC_TYPE_MASTER_MATCHING( nsCsNpcTable::eNPC_TYPE eType ){ s_eNpcType = eType; }
	std::list< DWORD >		s_list_Item;

	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();

	int						IsItem( DWORD dwItemID );
	void					AddItem( DWORD dwItemID );
	void					DelItem( DWORD dwItemID );

	int						GetItemCount(){ return (int)s_list_Item.size(); }
	DWORD					GetItemID( int nIndex );
	CsItem*					GetItem( int nIndex );
};

struct sNPC_TYPE_SPIRIT_EVO : public sNPC_TYPE_BASE
{
	sNPC_TYPE_SPIRIT_EVO(){ s_eNpcType = nsCsNpcTable::NT_SPIRIT_EVO; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};

struct sNPC_TYPE_SPECIAL_EVENT : public sNPC_TYPE_BASE
{
	enum eSUBTYPE{
		EVENT_NONE			= 0,
		EVENT_CARDGAME		= 1,	// 피에몬 카드 게임
		EVENT_PINOKIMON		= 2,	// 2014 겨울(크리스마스)이벤트
	};

	sNPC_TYPE_SPECIAL_EVENT(){ s_eNpcType = nsCsNpcTable::NT_SPECIAL_EVENT; s_eSubType = EVENT_NONE; }
	std::list< DWORD >		s_list_Item;
	eSUBTYPE				s_eSubType;

	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();

	int						IsItem( DWORD dwItemID );
	void					AddItem( DWORD dwItemID );
	void					DelItem( DWORD dwItemID );

	eSUBTYPE				GetSubType(){ return s_eSubType; }
	void					SetSubType( eSUBTYPE nSubType ){ s_eSubType = nSubType; }
	int						GetItemCount(){ return (int)s_list_Item.size(); }
	DWORD					GetItemID( int nIndex );
	CsItem*					GetItem( int nIndex );
};

struct sNPC_TYPE_ITEM_PRODUCTION : public sNPC_TYPE_BASE
{
	sNPC_TYPE_ITEM_PRODUCTION(){ s_eNpcType = nsCsNpcTable::NT_ITEM_PRODUCTION_NPC; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};

struct sNPC_TYPE_BATTLEREGISTRANT : public sNPC_TYPE_BASE
{
	sNPC_TYPE_BATTLEREGISTRANT(){ s_eNpcType = nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};

struct sNPC_TYPE_INFINITEWAR_MANAGER : public sNPC_TYPE_BASE
{
	sNPC_TYPE_INFINITEWAR_MANAGER(){ s_eNpcType = nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};

struct sNPC_TYPE_INFINITEWAR_NOTICEBOARD : public sNPC_TYPE_BASE
{
	sNPC_TYPE_INFINITEWAR_NOTICEBOARD(){ s_eNpcType = nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};


struct sNPC_TYPE_EXTRA_EVOLUTION : public sNPC_TYPE_BASE
{
	sNPC_TYPE_EXTRA_EVOLUTION(){ s_eNpcType = nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC; }
	virtual sNPC_TYPE_BASE* Clone();
	virtual void			Delete();
	virtual void			Init();
};
