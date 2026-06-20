
#pragma once  
class AdaptWareHouse; 
class AdaptDigimonArchiveSystem;
class AdaptEnchantOption;
#include "ContentsSystem/ContentsSystem.h"

#define ITEM_TYPELS_MACRO_CHECK			62, 20
#define ITEM_STR_MACRO_CHECK			nsCsFileTable::g_pItemMng->GetItem( 9000 )->GetInfo()->s_szName

#define ITEM_STR_CAPSULE_COIN			nsCsFileTable::g_pItemMng->GetItem( 6504 )->GetInfo()->s_szName

#define ITEM_TYPELS_GUILD_CREATE		75, 0
#define ITEM_STR_GUILD_CREATE			nsCsFileTable::g_pItemMng->GetItem( 26001 )->GetInfo()->s_szName

#define ITEM_TYPE_S_EVOL_LIMIT			2

#define PCBANG_COSTUME_ITEM_ID			17101

#define SERVER_DATA_INVEN_CONSTANT		( nItem::TabInven*1000 )
#define SERVER_DATA_TEQUIP_CONSTANT		( nItem::TabEquip*1000 )
#define SERVER_DATA_CHIPSET_CONSTANT	( nItem::TabChipset*1000 )
#define SERVER_DATA_WAREHOUSE_CONSTANT	( nItem::TabWarehouse*1000 )
#ifdef NEW_SHARESTASH
#define SERVER_DATA_SHARESTASH_CONSTANT	( nItem::TabShareStash*1000 )
#endif
#define SERVER_DATA_DIGIVICE_CONSTANT	( nItem::TabDigivice*1000 )
#define SERVER_DATA_TSKILL_CONSTANT		( nItem::TabSkill*1000 )

#ifdef CROSSWARS_SYSTEM
#define SERVER_DATA_INVENCROSS_CONSTANT		( nItem::TabCrossWars*1000 )
#define SERVER_DATA_MEMORY_CONSTANT		11000
#else
#define SERVER_DATA_MEMORY_CONSTANT		10000
#endif




#define TO_INVEN_SID( id )				( id )
#define TO_TEQUIP_SID( id )				( id + SERVER_DATA_TEQUIP_CONSTANT )
#define TO_MEMORY_SID( id )				( id + SERVER_DATA_MEMORY_CONSTANT )
#define TO_TSKILL_SID( id )				( id + SERVER_DATA_TSKILL_CONSTANT )
#define TO_CHIPSET_SID( id )			( id + SERVER_DATA_CHIPSET_CONSTANT )
#define TO_WAREHOUSE_SID( id )			( id + SERVER_DATA_WAREHOUSE_CONSTANT )
#ifdef NEW_SHARESTASH
#define TO_SHARESTASH_SID( id )			( id + SERVER_DATA_SHARESTASH_CONSTANT )
#endif
#define TO_DIGIVICE_SID( id )			( id + SERVER_DATA_DIGIVICE_CONSTANT )
#ifdef CROSSWARS_SYSTEM
#define TO_INVENCROSS_SID( id )			( id + SERVER_DATA_INVENCROSS_CONSTANT )
#endif
#define TO_ID( sid )					( sid % 1000 )
#define TO_CONSTANT( sid )				( sid / 1000 * 1000 )

struct sCHAR_IMAGE : public NiMemObject			// 쓰레드 상에서 호출 되면 안된다.
{
	sCHAR_IMAGE(){ s_nRef = 0; s_pLargeImg = s_pSmallImg = s_pBossImg = NULL; }

	void			Delete(){ assert_cs( s_nRef == 0 ); SAFE_NIDELETE( s_pLargeImg ); SAFE_NIDELETE( s_pSmallImg ); SAFE_NIDELETE( s_pBossImg ); }
	void			IncreaseRef(){ ++s_nRef; }
	void			DecreaseRef(){ if( --s_nRef == 0 ){ s_dwLastDeleteTime = GetTickCount(); }	assert_cs( s_nRef >= 0 ); }
	void			Render_L( CsPoint pos, CsPoint size = CsPoint::ZERO );
	void			Render_S( CsPoint pos, CsPoint size = CsPoint::ZERO );
	void			Render_B( CsPoint pos, CsPoint size = CsPoint::ZERO );
	void			ResetDevice();	

	DWORD			s_nID;
	cSprite*		s_pLargeImg;
	cSprite*		s_pSmallImg;
	cSprite*		s_pBossImg;
	DWORD			GetLastTime(){ return s_dwLastDeleteTime; }
	int				GetRefCount(){ return s_nRef; }	

private:
	int				s_nRef;
	DWORD			s_dwLastDeleteTime;
};

#include "Data_Inven.h"
#include "Data_PostLoad.h"
#include "Data_CoolTime.h"
#include "Data_TEquip.h"
#include "Data_Digivice.h"
#include "Data_QuickSlot.h"
#include "Data_Quest.h"
#include "Data_QuestOwner.h"
#include "Data_Tactics.h"
#include "Data_MapRegion.h"
#include "Data_ServerSync.h" 
#include "Data_Cash.h"
#include "Data_TacticsHouse.h"
#include "Data_GuildMark.h"
#include "Data_Attendance.h"
#include "Data_PCbang.h"

#ifdef CROSSWARS_SYSTEM
#include "Data_Inven_Cross.h"
#include "Data_Tactics_Cross.h"
#endif
//MASTERS_MATCHING -테이머 이름 옆 아이콘 뿌리기, 다른곳에서 쓸 수 있게 디파인은 안걸어둠 chu8820
#include "Data_TamerIcon.h"
#include "Data_MapObject.h"

//	nItem::Portable_Item
enum Portable		// 휴대용 아이템				77
{
	eHATCH=1,		// 휴대용 부화기				7701
	eWARE_HOUSE,	// 휴대용 창고					7702
	eTATICS_HOUSE,	// 휴대용 디지몬(용병) 보관소	7703
	eSCANNER,		// 휴대용 스캐너				7704
	ePAY_BACK,		// 휴대용 회수기				7705
	eANALYSOR,		// 휴대용 분석기				7706
	eFUSION,		// 휴대용 합성기				7707
};

class cDataMng : public NiMemObject
{
public:
	static void		GlobalShotdown();
	static void		GlobalInit();
protected:
public:
	void			Delete();
	void			Init();

	void			LogOut();

	void			Reset();
	
	void			Update();
	//void			Render();

protected:
	void			_InitNetOff();

public:
	void			DiedAlready();

public:
	NiColor			GetItemColor( CsItem::sINFO const* pFTItem );
	NiColor			GetItemColor( int nItemClass );
	//==============================================================================================
	//		아이템 闡?
	//==============================================================================================
protected:	
	int					m_nItemTryCount;
	CsVectorPB< int >	m_vpItemLockIndex;
public:
	void			ItemAllLock();
	void			ItemLock( int nLockIndex );
	void			ItemUnlock( int nLockIndex );
	bool			IsItemLock( int nLockIndex );
	bool			IsItemAllLock() { return ( m_vpItemLockIndex.Size() == 0 ) ? false : true; }
	void			ReleaseItemLock(){ m_vpItemLockIndex.ClearElement(); }

	//==============================================================================================
	//		아이템 사용
	//==============================================================================================
public:	
	bool			IsItemUse( int nSrvIndex );
	cItemInfo*		SrvID2ItemInfo( int nSrvID );

	void			SendItemUse( int nSrvID );
#ifdef CROSSWARS_SYSTEM
	void			ServerCrossItemUseSuccess( uint nTargetUID, int nInvenIndex );
#endif
	void			ServerItemUseSuccess( uint nTargetUID, int nInvenIndex );
	void			ServerItemUseFailed( int nInvenIndex );	
	void			SetChangeItemLimited(u2 Itempos, u1 limit);
protected:
	void			_SendItemUse_Inven( int nInvenIndex );
	bool			_CheckItemUseLevel( cItemInfo const* pItem );
#ifdef CROSSWARS_SYSTEM
	void			_SendItemUse_InvenCross( int nInvenIndex );
#endif
	

	//==============================================================================================
	//		아이템 이동에 관한 처리 ( 장착 포함 )
	//==============================================================================================

protected:
#ifdef CROSSWARS_SYSTEM
	std::list< DWORD >		m_CrossListItem;
#endif	
	std::list< DWORD >		m_ListItem;	

	//	심각한 버그 때문에 빠른 처리를 위해 선언..
	// 임시 코드.
	cSort					m_NewSort;	// 아이템 정렬 기능.

	CsCoolTimeSeq			m_SortSeq;

public:
	CsCoolTimeSeq*			GetSortCoolTime() { return &m_SortSeq; }

protected:
	int				m_nSort;		// 소트 하는 횟수 (공용)

public:	
	void			InvenSort();	
	void			_SendSort();	
	void			_NextSort();
	int				_GetMoveInvenSlot( DWORD dwItemID );
	void			_ReleaseSort(bool	Check=true);
	
	bool			IsSort() { return ( m_ListItem.empty() == false ); }

public:
	int				SendItemMove( int nSrcSrvID, int nDestSrvID, int nCount = 0 );
	void			SendItemMoveInven( int nSrcSrvID, int nCount = 0 );	
	void			ServerItemMoveSuccess( int nSrcSrvID, int nDestSrvID );
	void			ServerItemMoveFailed( int nSrcSrvID, int nDestSrvID );	
	void			ResetItemTryCount();
	void			ItemTryCount();

protected:
	bool			_IsEnableItemMove( int nSrcSrvID, int nDestSrvID );
	bool			_SendItemMove_Digivice( int nTEquipSrvID, int nInvenSrvID );
	bool			_SendItemMove_Equiped( int nTEquipSrvID, int nInvenSrvID );

	//==============================================================================================
	//		스킬 메모리
	//==============================================================================================
private:
	bool			SkillMemoryUse(CsItem::sINFO* pFTInfo, int InvenIndex);

	//==============================================================================================
	//		거래
	//==============================================================================================
public:	
	bool			IsTradeItem( cItemInfo* pItemInfo );
	bool			IsDropItem( cItemInfo* pItemInfo );

	bool			IsOpenStore();
	bool			IsScanItem( cItemInfo* pItemInfo );
	bool			IsSellItem( cItemInfo* pItemInfo );

	//==============================================================================================
	//		플레이 타임 제한
	//==============================================================================================
#ifdef PLAY_PANELTY
protected:	
	int					m_nPlay_PaneltyType;
	u4					m_nPlay_BaseFullTime;
	u4					m_nPlay_BasePaneltyTime;
	u4					m_nPlay_NormalTime;
	u4					m_nPlay_CashTime;

public:
	void				PlayTime_Set_PaneltyType( int nType ){ m_nPlay_PaneltyType = nType; }
	void				PlayTime_Set_BaseFullTime( u4 nTime ){ m_nPlay_BaseFullTime = nTime; }
	void				PlayTime_Set_BasePaneltyTime( u4 nTime ){ m_nPlay_BasePaneltyTime = nTime; }
	void				PlayTime_Set_NormalTime( u4 nTime ){ m_nPlay_NormalTime = nTime; }
	void				PlayTime_Set_CashTime( u4 nTime ){ m_nPlay_CashTime = nTime; }

	int					PlayTime_Get_PaneltyType(){ return m_nPlay_PaneltyType; }
	u4					PlayTime_Get_BaseFullTime(){ return m_nPlay_BaseFullTime; }
	u4					PlayTime_Get_BasePaneltyTime(){ return m_nPlay_BasePaneltyTime; }
	u4					PlayTime_Get_NormalTime(){ return m_nPlay_NormalTime; }
	u4					PlayTime_Get_CashTime(){ return m_nPlay_CashTime; }

	u4					PlayTime_Get_GameResetTime();

	void				PlayTime_GameResetTime_PrintMsg();
#endif

	//==============================================================================================
	//		데이터 객체
	//==============================================================================================
protected:
	cData_Inven		m_Inven;
public:
	cData_Inven*	GetInven(){ return &m_Inven; }

protected:
	cData_PostLoad	m_PostLoad;
public:
	cData_PostLoad*	GetPostLoad(){ return &m_PostLoad; }
protected:

protected:
	cData_TEquip		m_TEquip;
	cData_Digivice		m_Digivice;

	cData_QuickSlot		m_pQuickSlot[ 6 ];	// 테이머, 디지몬 4 , 추가 퀵슬롯

	cData_Quest			m_Quest;
	cData_QuestOwner	m_QuestOwner;
	cData_Tactics		m_Tactics;
	cData_MapRegion		m_MapRegion;
	cData_ServerSync	m_ServerSync;
	cData_Cash			m_Cash;
	cData_Attendance	m_Attendance;
	cData_PCBang		m_PCbang;

#ifdef GUILD_MARK_DOWN
	cData_GuildMark		m_GuildMark;	
#endif

	cResist::sCHAR_QUICKSLOT::sCHAR		m_TamerCashQuickSlot;	// 테이머 캐쉬 스킬 저장용 퀵슬롯

	cData_MapObject		m_MapObject;

public:
	cData_TEquip*		GetTEquip(){ return &m_TEquip; }
	cData_Digivice*		GetDigivice(){ return &m_Digivice; }
	cData_QuickSlot*	GetQuickSlot( int nIndex ){ return &m_pQuickSlot[ nIndex ]; }
	cData_Quest*		GetQuest(){ return &m_Quest; }
	cData_QuestOwner*	GetQuestOwner(){ return &m_QuestOwner; }
	cData_Tactics*		GetTactics(){ return &m_Tactics; }
#ifdef CROSSWARS_SYSTEM
	cData_CrossTactics*		GetCrossTactics(){ return &m_CrossTatics; }
#endif
	cData_MapRegion*	GetMapRegion(){ return &m_MapRegion; }
	cData_ServerSync*	GetServerSync(){ return &m_ServerSync; }
	cData_Cash*			GetCash(){ return &m_Cash; }
	cData_Attendance*	GetAttendance(){ return &m_Attendance; }
	cData_PCBang*		GetPCbang(){ return &m_PCbang; }

#ifdef GUILD_MARK_DOWN
	cData_GuildMark*	GetGuildMark(){ return &m_GuildMark; }
#endif

	cResist::sCHAR_QUICKSLOT::sCHAR*	GetTamerCashQuickSlot(){ return &m_TamerCashQuickSlot; }

	cData_MapObject*		GetMapObject(){ return &m_MapObject; }

private:
	bool	SystemBuffCheck(int	InvenIdx, CsItem* pFTItem, CsC_AvObject* pTarget, USHORT	BuffID);	// 시스템 버프인지 체크
	bool	AlreadyBuffCheck(int InvenIdx, CsItem* pFTItem, CsC_AvObject* pTarget, USHORT	BuffID);	// 이미 사용된 버프인지 체크
	void	OverlapBuffLevelCheck(int PrevBuffLevel, CsItem* pFTItem, int Value1, int Value2);			// 중복 버프 검사.

	TOwnershipGuard<AdaptWareHouse> m_pWarehouseAdapt; 
	TOwnershipGuard<AdaptDigimonArchiveSystem> m_pDigimonArchiveAdapt;
	TOwnershipGuard<AdaptEnchantOption> m_pEnchantOptionAdapt;
};

extern cDataMng*		g_pDataMng;