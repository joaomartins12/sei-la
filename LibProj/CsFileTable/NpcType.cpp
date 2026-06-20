
#include "stdafx.h"
#include "NpcType.h"


//====================================================================================================
//
//		NONE
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_NONE::Clone()
{
	return csnew sNPC_TYPE_NONE;
}

void sNPC_TYPE_NONE::Delete()
{
}

void sNPC_TYPE_NONE::Init()
{
}


//====================================================================================================
//
//		DIGITAMA TRADE
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_DIGITAMA_TRADE::Clone()
{
	return csnew sNPC_TYPE_DIGITAMA_TRADE;
}

void sNPC_TYPE_DIGITAMA_TRADE::Delete()
{
}

void sNPC_TYPE_DIGITAMA_TRADE::Init()
{
}


//====================================================================================================
//
//		Trade
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_TRADE::Clone()
{
	sNPC_TYPE_TRADE* pTrade = csnew sNPC_TYPE_TRADE( s_eNpcType );
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		pTrade->AddItem( (*it) );
	}
	return pTrade;
}

void sNPC_TYPE_TRADE::Delete()
{
	s_list_Item.clear();
}

void sNPC_TYPE_TRADE::Init()
{
	assert_cs( s_list_Item.size() == 0 );
}

// 툴에서만 사용 하는거 추천.. 느리다..
bool sNPC_TYPE_TRADE::IsItem( DWORD dwItemID )
{
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( *it == dwItemID )
			return true;
	}
	return false;
}

void sNPC_TYPE_TRADE::AddItem( DWORD dwItemID )
{
	assert_cs( dwItemID != 0 );
	assert_cs( IsItem( dwItemID ) == false );
	s_list_Item.push_back( dwItemID );
}

void sNPC_TYPE_TRADE::DelItem( DWORD dwItemID )
{
	assert_cs( IsItem( dwItemID ) == true );
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it) == dwItemID )
		{
			s_list_Item.erase( it );
			return;
		}
	}
	assert_cs( false );	
}

CsItem* sNPC_TYPE_TRADE::GetItem( int nIndex )
{
	if( NULL == nsCsFileTable::g_pItemMng )
		return NULL;

	std::list< DWORD >::iterator it = s_list_Item.begin();
	for( int n = 0; it != s_list_Item.end(); ++it, ++n )
	{
		if( nIndex != n )
			continue;

		return nsCsFileTable::g_pItemMng->GetItem( *it );
	}
	return NULL;
// 	assert_cs( nIndex >= 0 );
// 
// 	std::list< DWORD >::iterator it = s_list_Item.begin();
// 	std::list< DWORD >::iterator itEnd = s_list_Item.end();
// 	for( ; it!=itEnd; ++it )
// 	{
// 		if( nIndex == 0 )
// 			return nsCsFileTable::g_pItemMng->GetItem( *it );
// 		--nIndex;
// 	}
// 	assert_cs( false );
// 	return NULL;
}

DWORD sNPC_TYPE_TRADE::GetItemID( int nIndex )
{
	std::list< DWORD >::iterator it = s_list_Item.begin();
	for( int n = 0; it != s_list_Item.end(); ++it, ++n )
	{
		if( n != nIndex )
			continue;
		return (*it);
	}
	return 0;
// 	assert_cs( nIndex >= 0 );
// 
// 	std::list< DWORD >::iterator it = s_list_Item.begin();
// 	std::list< DWORD >::iterator itEnd = s_list_Item.end();
// 	for( ; it!=itEnd; ++it )
// 	{
// 		if( nIndex == 0 )
// 			return (*it);
// 		--nIndex;
// 	}
// 	assert_cs( false );
// 	return 0;
}

//====================================================================================================
//
//		Portal
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_PORTAL::Clone()
{
	sNPC_TYPE_PORTAL* pType = csnew sNPC_TYPE_PORTAL;
	pType->s_nPortalCount = s_nPortalCount;
	pType->s_nPortalType = s_nPortalType;
	std::list< sPORTAL* >::iterator it = s_listPortal.begin();
	std::list< sPORTAL* >::iterator itEnd = s_listPortal.end();
	for( ; it!=itEnd; ++it )
	{
		sPORTAL* pNewPortal = pType->NewPortal( false );
		memcpy( pNewPortal, *it, sizeof( sPORTAL ) );
	}

	return pType;
}

void sNPC_TYPE_PORTAL::Delete()
{
	std::list< sPORTAL* >::iterator it = s_listPortal.begin();
	std::list< sPORTAL* >::iterator itEnd = s_listPortal.end();
	for( ; it!=itEnd; ++it )
	{
		delete *it;
	}
	s_listPortal.clear();
}

void sNPC_TYPE_PORTAL::Init()
{
}

void sNPC_TYPE_PORTAL::DelPortal( int nIndex )
{
	std::list< sPORTAL* >::iterator it = s_listPortal.begin();
	std::list< sPORTAL* >::iterator itEnd = s_listPortal.end();
	for( ; it!=itEnd; ++it )
	{
		if( --nIndex < 0 )
		{
			--s_nPortalCount;

			delete *it;
			s_listPortal.erase( it );
			return;
		}
	}

	assert_cs( false );
}

sNPC_TYPE_PORTAL::sPORTAL* sNPC_TYPE_PORTAL::NewPortal( bool bIncreasePortalCount )
{
	sPORTAL* pNew = new sPORTAL;
	s_listPortal.push_back( pNew );

	if( bIncreasePortalCount )
		++s_nPortalCount;
	return pNew;
}

sNPC_TYPE_PORTAL::sPORTAL* sNPC_TYPE_PORTAL::GetPortal( int nIndex )
{
	std::list< sPORTAL* >::iterator it = s_listPortal.begin();
	std::list< sPORTAL* >::iterator itEnd = s_listPortal.end();
	for( ; it!=itEnd; ++it )
	{
		if( --nIndex < 0 )
		{
			return *it;
		}
	}
	assert_cs( false );
	return NULL;
}

//====================================================================================================
//
//		Make Tactics
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_MAKE_TACTICS::Clone()
{
	return csnew sNPC_TYPE_MAKE_TACTICS;
}

void sNPC_TYPE_MAKE_TACTICS::Delete()
{
}

void sNPC_TYPE_MAKE_TACTICS::Init()
{
}


//====================================================================================================
//
//		속성 아이템 상인
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_ELEMENT_ITEM::Clone()
{
	return csnew sNPC_TYPE_ELEMENT_ITEM;
}

void sNPC_TYPE_ELEMENT_ITEM::Delete()
{
}

void sNPC_TYPE_ELEMENT_ITEM::Init()
{
}

//====================================================================================================
//
//		창고
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_WAREHOUSE::Clone()
{
	return csnew sNPC_TYPE_WAREHOUSE;
}

void sNPC_TYPE_WAREHOUSE::Delete()
{
}

void sNPC_TYPE_WAREHOUSE::Init()
{
}

//====================================================================================================
//
//		DATS PORTAL
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_DATS_PORTAL::Clone()
{
	return csnew sNPC_TYPE_DIGITAMA_TRADE;
}

void sNPC_TYPE_DATS_PORTAL::Delete()
{
}

void sNPC_TYPE_DATS_PORTAL::Init()
{
}

//====================================================================================================
//
//		물물교환상점
//
//====================================================================================================
CsItemExchange* sNPC_TYPE_EXCHANGE::GetExchange_Item( DWORD dwNpcID, int nIndex )
{
	assert_cs( nIndex >= 0 );

	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return nsCsFileTable::g_pItemMng->GetExchangeItem( dwNpcID, *it );
		--nIndex;
	}
	assert_cs( false );
	return NULL;
}

// 물물교환 상점 판매분류가 두개 이상일 때 첫번째 tab에 있는 메뉴의 아이템만(툴팁, 구매등..) 나타나는 현상이 있어 수정	15.01.13  lks007
CsItemExchange*	sNPC_TYPE_EXCHANGE::GetExchange_Item( DWORD dwNpcID, int nIndex, int nCurTab )
{
	assert_cs( nIndex >= 0 );

	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		CsItem::sINFO* pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( *it )->GetInfo();
		if( pFTItemInfo->s_nSellType != nCurTab )
			continue;

		if( nIndex == 0 )
			return nsCsFileTable::g_pItemMng->GetExchangeItem( dwNpcID, *it );

		--nIndex;
	}
	//assert_cs( false );
	return NULL;
}

sNPC_TYPE_BASE* sNPC_TYPE_EXCHANGE::Clone()
{
	sNPC_TYPE_EXCHANGE* pExchange = csnew sNPC_TYPE_EXCHANGE( s_eNpcType );
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		pExchange->AddItem( (*it) );
	}
	return pExchange;
}

void sNPC_TYPE_EXCHANGE::Delete()
{
	s_list_Item.clear();
}

void sNPC_TYPE_EXCHANGE::Init()
{
	assert_cs( s_list_Item.size() == 0 );
}

bool sNPC_TYPE_EXCHANGE::IsItem( DWORD dwItemID )
{
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( *it == dwItemID )
			return true;
	}
	return false;
}

void sNPC_TYPE_EXCHANGE::AddItem( DWORD dwItemID )
{
	assert_cs( dwItemID != 0 );
	assert_cs( IsItem( dwItemID ) == false );
	s_list_Item.push_back( dwItemID );
}

void sNPC_TYPE_EXCHANGE::DelItem( DWORD dwItemID )
{
	assert_cs( IsItem( dwItemID ) == true );
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it) == dwItemID )
		{
			s_list_Item.erase( it );
			return;
		}
	}
	assert_cs( false );
}

DWORD sNPC_TYPE_EXCHANGE::GetItemID( int nIndex )
{
	assert_cs( nIndex >= 0 );

	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return (*it);
		--nIndex;
	}
	assert_cs( false );
	return 0;
}

//====================================================================================================
//
//		DATS PORTAL
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_GOTCHAMACHINE::Clone()
{
	return csnew sNPC_TYPE_GOTCHAMACHINE;
}

void sNPC_TYPE_GOTCHAMACHINE::Delete()
{
}

void sNPC_TYPE_GOTCHAMACHINE::Init()
{
}

sNPC_TYPE_BASE* sNPC_TYPE_MASTER_MATCHING::Clone()
{
	sNPC_TYPE_MASTER_MATCHING* pMatch = csnew sNPC_TYPE_MASTER_MATCHING( s_eNpcType );
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		pMatch->AddItem( (*it) );
	}
	return pMatch;
}

void sNPC_TYPE_MASTER_MATCHING::Delete()
{
	s_list_Item.clear();
}

void sNPC_TYPE_MASTER_MATCHING::Init()
{
	assert_cs( s_list_Item.size() == 0 );
}
void sNPC_TYPE_MASTER_MATCHING::AddItem( DWORD dwItemID )
{
	assert_cs( dwItemID != 0 );
	assert_cs( IsItem( dwItemID ) == -1 );
	s_list_Item.push_back( dwItemID );
}

void sNPC_TYPE_MASTER_MATCHING::DelItem( DWORD dwItemID )
{
	assert_cs( IsItem( dwItemID ) == -1 );
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it) == dwItemID )
		{
			s_list_Item.erase( it );
			return;
		}
	}
	assert_cs( false );
}

CsItem* sNPC_TYPE_MASTER_MATCHING::GetItem( int nIndex )
{
	assert_cs( nIndex >= 0 );

	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return nsCsFileTable::g_pItemMng->GetItem( *it );
		--nIndex;
	}
	assert_cs( false );
	return NULL;
}
DWORD sNPC_TYPE_MASTER_MATCHING::GetItemID( int nIndex )
{
	assert_cs( nIndex >= 0 );

	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return (*it);
		--nIndex;
	}
	assert_cs( false );
	return 0;
}


int sNPC_TYPE_MASTER_MATCHING::IsItem( DWORD dwItemID )
{
	int nIdx = 0;
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( *it == dwItemID )
			return nIdx; // 몇번째 놈인지 알려줌
		nIdx++;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////
//
//	스피릿 진화
//
//////////////////////////////////////////////////////////////////////////
sNPC_TYPE_BASE* sNPC_TYPE_SPIRIT_EVO::Clone()
{
	return csnew sNPC_TYPE_SPIRIT_EVO;
}

void sNPC_TYPE_SPIRIT_EVO::Delete()
{
}

void sNPC_TYPE_SPIRIT_EVO::Init()
{
}

sNPC_TYPE_BASE* sNPC_TYPE_SPECIAL_EVENT::Clone()
{
	sNPC_TYPE_SPECIAL_EVENT* pEvent = csnew sNPC_TYPE_SPECIAL_EVENT;

	if( s_eSubType == EVENT_CARDGAME )	// sub 타입이 피에몬 카드 게임일 경우
		pEvent->s_eSubType = s_eSubType;

	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		pEvent->AddItem( (*it) );
	}
	return pEvent;
}

void sNPC_TYPE_SPECIAL_EVENT::Delete()
{
	s_list_Item.clear();
}

void sNPC_TYPE_SPECIAL_EVENT::Init()
{
	assert_cs( s_list_Item.size() == 0 );
}
void sNPC_TYPE_SPECIAL_EVENT::AddItem( DWORD dwItemID )
{
	assert_cs( dwItemID != 0 );
	assert_cs( IsItem( dwItemID ) == -1 );
	s_list_Item.push_back( dwItemID );
}

void sNPC_TYPE_SPECIAL_EVENT::DelItem( DWORD dwItemID )
{
	assert_cs( IsItem( dwItemID ) == -1 );
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it) == dwItemID )
		{
			s_list_Item.erase( it );
			return;
		}
	}
	assert_cs( false );
}

CsItem* sNPC_TYPE_SPECIAL_EVENT::GetItem( int nIndex )
{
	assert_cs( nIndex >= 0 );

	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return nsCsFileTable::g_pItemMng->GetItem( *it );
		--nIndex;
	}
	assert_cs( false );
	return NULL;
}
DWORD sNPC_TYPE_SPECIAL_EVENT::GetItemID( int nIndex )
{
	assert_cs( nIndex >= 0 );

	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( nIndex == 0 )
			return (*it);
		--nIndex;
	}
	assert_cs( false );
	return 0;
}


int sNPC_TYPE_SPECIAL_EVENT::IsItem( DWORD dwItemID )
{
	int nIdx = 0;
	std::list< DWORD >::iterator it = s_list_Item.begin();
	std::list< DWORD >::iterator itEnd = s_list_Item.end();
	for( ; it!=itEnd; ++it )
	{
		if( *it == dwItemID )
			return nIdx; // 몇번째 놈인지 알려줌
		nIdx++;
	}
	return -1;
}

//====================================================================================================
//
//		ITEM PRODUCTION NPC
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_ITEM_PRODUCTION::Clone()
{
	return csnew sNPC_TYPE_ITEM_PRODUCTION;
}

void sNPC_TYPE_ITEM_PRODUCTION::Delete()
{
}

void sNPC_TYPE_ITEM_PRODUCTION::Init()
{
}

//====================================================================================================
//
//		ITEM PRODUCTION NPC
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_BATTLEREGISTRANT::Clone()
{
	return csnew sNPC_TYPE_BATTLEREGISTRANT;
}

void sNPC_TYPE_BATTLEREGISTRANT::Delete()
{
}

void sNPC_TYPE_BATTLEREGISTRANT::Init()
{
}

//====================================================================================================
//
//		INFINITE WAR MANAGER NPC
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_INFINITEWAR_MANAGER::Clone()
{
	return csnew sNPC_TYPE_INFINITEWAR_MANAGER;
}

void sNPC_TYPE_INFINITEWAR_MANAGER::Delete()
{
}

void sNPC_TYPE_INFINITEWAR_MANAGER::Init()
{
}

//====================================================================================================
//
//		INFINITE WAR NOTICE BOARD NPC
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_INFINITEWAR_NOTICEBOARD::Clone()
{
	return csnew sNPC_TYPE_INFINITEWAR_NOTICEBOARD;
}

void sNPC_TYPE_INFINITEWAR_NOTICEBOARD::Delete()
{
}

void sNPC_TYPE_INFINITEWAR_NOTICEBOARD::Init()
{
}

//====================================================================================================
//
//		EXTRA EVOLUTION NPC
//
//====================================================================================================
sNPC_TYPE_BASE* sNPC_TYPE_EXTRA_EVOLUTION::Clone()
{
	return csnew sNPC_TYPE_EXTRA_EVOLUTION;
}

void sNPC_TYPE_EXTRA_EVOLUTION::Delete()
{
}

void sNPC_TYPE_EXTRA_EVOLUTION::Init()
{
}