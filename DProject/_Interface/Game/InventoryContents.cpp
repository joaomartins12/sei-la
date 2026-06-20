#include "StdAfx.h"
#include "InventoryContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptTutorialQuest.h"

#define INVEN_IS_NOT_ENOUGTH_MSG_ID 11015	// 가방에 공간이 부족합니다.

cInventoryContents::cInventoryContents() : m_eCurFilter( eNone ), m_eState( eCloseUI )
, m_nPreRankNo( 0 )
#ifdef UI_INVENTORY_RENEWAL
, m_bRecvInvenInfo( true )
, m_AdaptTutorialQuest( 0 )
#endif
{
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::OPEN_INVENTORY,			this, &cInventoryContents::OpenInventory );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::CLOSE_INVENTORY,		this, &cInventoryContents::CloseInventory );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SET_ENABLE_INVENTORY,	this, &cInventoryContents::SetEnableInventory );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_POS_DEPENDON_INVEN,	this, &cInventoryContents::GetPosDependOnInven );
#ifdef UI_INVENTORY_RENEWAL
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ADD_NEWITEM_INVENTORY,	this, &cInventoryContents::AddNewItem );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_INVEN_SORT_END,	this, &cInventoryContents::RecvSortEnd );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_INVEN_REQ_RESULT,	this, &cInventoryContents::RecvInvenReqResult );
#endif
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SET_GIFTBOX_WINDOW,		this, &cInventoryContents::SetGiftBoxWindow );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UPDATE_GIFTBOX,			this, &cInventoryContents::UpdateGiftBox );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::REQ_CHANGETAMER_USINGITEM,	this, &cInventoryContents::RequestUsingChangeTamer );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::OPEN_CHANGETAMER_WINDOW,	this, &cInventoryContents::OpenChangeTamer );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SEND_CHANGETAMER_CHANGE,	this, &cInventoryContents::SendChangeTamer );
}

cInventoryContents::~cInventoryContents()
{
#ifdef UI_INVENTORY_RENEWAL
	m_AdaptTutorialQuest = 0;
#endif
	GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const cInventoryContents::IsContentsIdentity(void)
{
	return E_CT_INVENTORY_CONTENTS;
}

int const cInventoryContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool cInventoryContents::Initialize(void)
{
	return true;
}

void cInventoryContents::UnInitialize(void)
{

}

bool cInventoryContents::IntraConnection(ContentsSystem& System)
{
#ifdef UI_INVENTORY_RENEWAL
	m_AdaptTutorialQuest = new AdaptTutorialQuest;
	if( m_AdaptTutorialQuest && !m_AdaptTutorialQuest->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TUTORIAL_CONTENTS ) ) )
		return false;
#endif

	return true;
}

void cInventoryContents::Update(float const& fElapsedTime)
{

}

void cInventoryContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void cInventoryContents::MakeMainActorData(void)
{

}

void cInventoryContents::ClearMainActorData(void)
{

}

void cInventoryContents::MakeWorldData(void)
{

}

void cInventoryContents::ClearWorldData(void)
{
	m_mapFilterItem.clear();
	m_mapNewItem.clear();

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	pInven->Reset();
}

void cInventoryContents::OpenInventory(void* pData)
{
	SAFE_POINTER_RET( pData );
	bool bSound = *static_cast<bool*>( pData );

	SAFE_POINTER_RET( g_pGameIF );
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
#ifdef UI_INVENTORY_RENEWAL
	if( m_bRecvInvenInfo )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_INVENTORY, 0, 0, bSound );
#else
	int nInvenCount = pInven->GetInvenGroupCount();
	for( int i = 0; i < nInvenCount; ++i )
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_INVENTORY, i, i, bSound );
	}
#endif
	m_eState = eOpenUI;
}

void cInventoryContents::CloseInventory(void* pData)
{
#ifdef UI_INVENTORY_RENEWAL
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INVENTORY );
	
	m_mapFilterItem.clear();
	m_eCurFilter = eNone;
#else
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	int nInvenCount = pInven->GetInvenGroupCount();
	for( int i = 0; i < nInvenCount; ++i )
	{
		if( g_pGameIF )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INVENTORY, i );
	}
#endif

	m_eState = eCloseUI;
}

void cInventoryContents::SetEnableInventory(void* pData)
{
	SAFE_POINTER_RET( pData );
	bool bIsEnable = *static_cast< bool* >( pData );

	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_INVENTORY ) )
		return;

	ContentsStream kSend;
	kSend << bIsEnable;
	Notify( eEnableWindow, kSend );
}

void cInventoryContents::GetPosDependOnInven(void* pData)
{
	SAFE_POINTER_RET( pData );
	CsPoint& csPos = *static_cast< CsPoint* >( pData );

	SAFE_POINTER_RET( g_pGameIF );
	cBaseWindow* pInven = g_pGameIF->GetDynamicIF( cBaseWindow::WT_INVENTORY, 0, 0 );
	SAFE_POINTER_RET( pInven );

	bool bIsEnable = true;
	SetEnableInventory( &bIsEnable );

	CsPoint csInvenPos = pInven->GetRootClient();
	if( g_nScreenWidth / 2 < csInvenPos.x )//윈도우가 오른쪽에 있는 경우
		csInvenPos.x -= csPos.x;
	else
		csInvenPos.x += pInven->GetRootSize().x;
	csPos = csInvenPos;
}

#ifdef UI_INVENTORY_RENEWAL
void cInventoryContents::AddNewItem(void* pData)
{
	SAFE_POINTER_RET( pData );
	std::pair< int, bool >* pairRecv = static_cast< std::pair< int, bool >* >( pData );

	cItemInfo* pNewItem = GetInvenItem( pairRecv->first );
	if( pNewItem && pNewItem->IsEnable() )
	{
		SetNewItem( pairRecv->first, pairRecv->second );

		// 필터 중에 추가된 아이템 보여준다 - 필터된 아이템 아님
		if( eNone != m_eCurFilter )
		{
			if( !IsFilteringItem( pairRecv->first ) )
				m_mapFilterItem.insert( std::make_pair( m_mapFilterItem.size(), pairRecv->first ) );
		}
		bool bIsEnable = pairRecv->second;
		ContentsStream kSend;
		kSend << bIsEnable;
		NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_Inventory_NewItem, kSend );
	}
}

void cInventoryContents::RecvSortEnd(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_ITEMSORT* pRecv = static_cast< GS2C_RECV_ITEMSORT* >( pData );

	// 실패시 메시지
	if( eSORTRESULT::SUCCESS != pRecv->m_nResult )
	{
		cPrintMsg::PrintMsg( pRecv->m_nResult );
	}
	else
	{
		_UpdateInventory( pRecv->m_lItems );
		SetFilterMode( m_eCurFilter );			// 정렬된 걸로 갱신
	}

	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_INVENTORY ) )
		return;

	bool bIsEnable = true;
	ContentsStream kSend;
	kSend << bIsEnable;
	Notify( eEnableWindow, kSend );
}

void cInventoryContents::RecvInvenReqResult(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_INVENTORY* pRecv = static_cast< GS2C_RECV_INVENTORY* >( pData );
	
	m_bRecvInvenInfo = true;

	// 인벤 정보 못 받았을 때
	if( eINVENTORYRESULT::SUCCESS != pRecv->m_nResult )
	{
		m_bRecvInvenInfo = false;
		return;
	}

	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
		{
			pInven->SetMoney( pRecv->m_n8Money, false );
			pInven->SetInvenSlotCount( pRecv->m_u2InventorySlot );
		}
	}

	_UpdateInventory( pRecv->m_lItems );
	_CheckUserQuickSlot();

	// 퀘스트 아이템 체크
	SAFE_POINTER_RET( g_pDataMng );
	cData_Quest* pQuestData = g_pDataMng->GetQuest();
	SAFE_POINTER_RET( pQuestData );
	pQuestData->CalProcess();
}

void cInventoryContents::_UpdateInventory(std::list<cItemData>& lItems)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	pInven->ResetAllItem();

	std::list<cItemData>::iterator it = lItems.begin();
	std::list<cItemData>::iterator itEnd = lItems.end();
	for( int nIndex = 0; it != itEnd; ++it, ++nIndex )
	{
		cItemInfo* pItemInfo = GetInvenItem( nIndex );
		*pItemInfo = *it;
	}
}

void cInventoryContents::_CheckUserQuickSlot()
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	if( pTamerUser )
	{
		cSkill* pSkillMng = pTamerUser->GetSkillMng();
		if( pSkillMng )
			cData_QuickSlot::CheckAllSlot( pTamerUser, pTamerUser->GetQuickSlot(), pSkillMng->GetSkillCount() );
	}

	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	if( pDigimonUser )
	{
		cSkill* pSkillMng = pDigimonUser->GetSkillMng();
		if( pSkillMng )
			cData_QuickSlot::CheckAllSlot( pDigimonUser, pDigimonUser->GetQuickSlot(), pSkillMng->GetSkillCount() );
	}
}

#endif

void cInventoryContents::CloseWindow()
{
#ifdef UI_INVENTORY_RENEWAL
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INVENTORY );

	m_mapFilterItem.clear();
	m_eCurFilter = eNone;
#else
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	int nInvenCount = pInven->GetInvenGroupCount();
	for( int i = 0; i < nInvenCount; ++i )
	{
		if( g_pGameIF )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_INVENTORY, i );
	}
#endif

	m_eState = eCloseUI;
}

void cInventoryContents::UseItem(int const& nInvenIndex)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

#ifdef INVENCNT_CHECK
	if( nInvenIndex >= pInven->GetInvenSlotCount() )
		return;
#endif

	cItemInfo* pItem = pInven->GetData( nInvenIndex );
	SAFE_POINTER_RET( pItem );
	if( pItem->IsEnable() == false )
		return;

	SAFE_POINTER_RET( g_pGameIF );

	if( g_pDataMng->IsItemLock( TO_INVEN_SID( nInvenIndex ) ) == true )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	cSeparate::DeleteInstance( cBaseWindow::WT_INVENTORY, 0 );

	ContentsStream kSend;
	kSend << nInvenIndex;
	NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_Inventory_UseItem, kSend );

	// 위탁 상점 & 알 스캔/회수 npc 타입이 없음
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE )
		|| g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SCAN )
		|| g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SELL ) )
		return;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MASTERS_MATCHING) )
	{
		CsC_AvObject* pTargetNpc = g_pGameIF->GetMastersMatchTarget();
		SAFE_POINTER_RET( pTargetNpc );
		CNpc* pNpc = (CNpc*)pTargetNpc;
		//기부 아이템(NPC테이블)
		CsNpc* pFTNpc = pNpc->GetFTNpc();
		SAFE_POINTER_RET( pFTNpc );
		sNPC_TYPE_MASTER_MATCHING* m_pFTInsertItem = (sNPC_TYPE_MASTER_MATCHING*)pFTNpc->GetType();
		SAFE_POINTER_RET( m_pFTInsertItem );
		cMastersMatching* pMasterMach = g_pGameIF->GetMastersMatching();
		SAFE_POINTER_RET( pMasterMach );
		pMasterMach->SetInsertItem( m_pFTInsertItem );
		if( pMasterMach->IsResist( nInvenIndex ) )//기부아이템일 경우
		{
			if( pMasterMach->m_ResistItem.IsEnable() )//이미 등록된 놈은 해제해주고
				pMasterMach->_SetCount( 0, false );

			pMasterMach->SetResist( nInvenIndex );//등록
		}
		return;
	}

	// 현재 줌 되어 있는 npc의 타입에 따라 아이템 처리
	switch( g_pNpcMng->GetZoomNpcType() )
	{
	case cBaseWindow::WT_BARCODE_CARD_SCRATCH:
	case cBaseWindow::WT_TALK:
	case cBaseWindow::WT_QUEST_REV:
	case cBaseWindow::WT_QUEST_COMP:
	case cBaseWindow::WT_NONE:
	case cBaseWindow::WT_TACTICSHOUSE:
	case cBaseWindow::WT_ITEM_PRODUCTION_SHOP:
		{
			cItemInfo* pItem = pInven->GetData( nInvenIndex );
			SAFE_POINTER_RET( pItem );

			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
			SAFE_POINTER_RET( pFTItem );

			CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
			SAFE_POINTER_RET( pFTInfo );

#ifdef ITEM_USE_TIME_PASS
			if(pItem->m_nEndTime == 0xFFFFFFF0)
			{
				switch( pFTInfo->s_nBelonging )
				{
				case 0:
				case 2:	// 그외 일 경우
					cPrintMsg::PrintMsg( 30177, pFTInfo->s_szName );
					cMessageBox::GetFirstMessageBox()->SetValue1( nInvenIndex );
					break;

				case 1:// 사용시 귀속인 아이템일 경우
					cPrintMsg::PrintMsg( 30176, pFTInfo->s_szName );
					cMessageBox::GetFirstMessageBox()->SetValue1( nInvenIndex );
					break;
				}				
				return;
			}
			else
			{
				g_pDataMng->SendItemUse( TO_INVEN_SID( nInvenIndex ) );
			}	
#else
			g_pDataMng->SendItemUse( TO_INVEN_SID( nInvenIndex ) );
#endif
		}		
		break;
	case cBaseWindow::WT_MAKE_TACTICS:
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) )
		{
			cMakeTactics* pMakeTactics = g_pGameIF->GetMakeTactics();
			SAFE_POINTER_RET( pMakeTactics );
			pMakeTactics->Resist( nInvenIndex );
		}					
		break;
#ifndef LJW_EQUIPSYSTEM_190724
	case cBaseWindow::WT_ELEMENTITEM_CHECK:
		{
			int nIndex = nInvenIndex;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REGIST_ELEMENT_ITEM_CHECK, &nIndex );
		}
		break;
	case cBaseWindow::WT_ELEMENTITEM_ATTACH:
		{
			int nIndex = nInvenIndex;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REGIST_ELEMENT_ITEM_EQUIP, &nIndex );
		}
		break;
#endif
	default:
		break;
	}
}

CsItem::sINFO* cInventoryContents::GetFTItemInfo(uint nItemType) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, NULL );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemType );
	SAFE_POINTER_RETVAL( pFTItem, NULL );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, NULL );
	return pFTInfo;
}

cItemInfo* cInventoryContents::GetInvenItem(int nInvenIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, NULL );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, NULL );
	cItemInfo* pData = pInven->GetData( nInvenIndex );
	SAFE_POINTER_RETVAL( pData, NULL );
	return pData;
}

cItemInfo* cInventoryContents::GetEquipItem(int nIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, NULL );
	cData_TEquip* pEquip = g_pDataMng->GetTEquip();
	SAFE_POINTER_RETVAL( pEquip, NULL );
	cItemInfo* pData = pEquip->GetData( nIndex );
	SAFE_POINTER_RETVAL( pData, NULL );
	return pData;
}

uint cInventoryContents::GetInvenSlotCount() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, 0 );
	return pInven->GetInvenSlotCount();
}

#ifdef UI_INVENTORY_RENEWAL
const int cInventoryContents::GetFilteringItemIdx(int nIndex) const
{
	std::map< int, int >::const_iterator it = m_mapFilterItem.find( nIndex );
	if( it != m_mapFilterItem.end() )
		return it->second;
	return -1;
}

void cInventoryContents::SetNewItem(int nInvenIndex, bool bIsNew)
{
	bool bIsEnable = false;
	std::map< int, bool >::iterator it = m_mapNewItem.find( nInvenIndex );
	if( it != m_mapNewItem.end() )
	{
		if( !bIsNew )
		{
			m_mapNewItem.erase( it );
			if( m_mapNewItem.empty() )
				bIsEnable = false;
		}
		else
		{
			bIsEnable = true;
			it->second = bIsNew;
		}
	}
	else
	{
		if( bIsNew )
		{
			bIsEnable = true;
			m_mapNewItem.insert( std::make_pair( nInvenIndex, bIsNew ) );
		}
	}

	if( !m_mapNewItem.empty() )
		bIsEnable = true;

	ContentsStream kSend;
	kSend << bIsEnable;
	NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_Inventory_NewItem, kSend );
}

void cInventoryContents::SetFilterMode(eFilterMode eMode)
{
	m_mapFilterItem.clear();

	m_eCurFilter = eMode;
	if( eNone == m_eCurFilter )
		return;

	int nIndex = 0;
	// 인벤에서 필터링할 아이템을 찾아서 저장
	int nInvenCount = GetInvenSlotCount();
	for( int i = 0; i < nInvenCount; ++i )
	{
		cItemInfo* pItemInfo = GetInvenItem( i );
		SAFE_POINTER_CON( pItemInfo );
		if( !pItemInfo->IsEnable() )
			continue;

		CsItem::sINFO* pFTInfo = GetFTItemInfo( pItemInfo->GetType() );
		SAFE_POINTER_CON( pFTInfo );

		if( IsEqualItemType( m_eCurFilter, pFTInfo->s_nType_L ) )
			m_mapFilterItem.insert( std::make_pair( nIndex++, i ) );
	}
}

bool cInventoryContents::IsEqualItemType(eFilterMode eMode, int const& nType) const
{
	switch( eMode )
	{
	case eEquip:
		{
			switch( nType )
			{
			case nItem::Head:
			case nItem::Coat:
			case nItem::Glove:
			case nItem::Pants:
			case nItem::Shoes:
			case nItem::Costume:
			case nItem::Glass:
			case nItem::Necklace:
			case nItem::Ring:
			case nItem::Earring:
			case nItem::EquipAura:
			case nItem::XAI:
			case nItem::Bracelet:
			case nItem::Chipset:
			case nItem::Digivice:
			case nItem::AccStone:
			case nItem::EItem_All:
			case nItem::EItem_Coat:
			case nItem::EItem_Glove:
			case nItem::EItem_Pants:
			case nItem::EItem_Shoes:
			case nItem::EItem_Glass:
			case nItem::CrossWars:
			case nItem::CodeCrown:
			case nItem::CrossLoader:
			case nItem::AntiX:
				return true;
			}
		}
		break;
	case eConsume:
		{
			switch( nType )
			{
			case nItem::Skill:
			case nItem::LevelRequire:
			case nItem::Potion:
			case nItem::NoQuick_Consume:
			case nItem::Buff:
			case nItem::SKILL_Card:
			case nItem::Consume:
			case nItem::Portal:
			case nItem::Resurrection:
			case nItem::Guild:
			case nItem::NoticeItem:
			case nItem::Digitama2:
			case nItem::Cash_ChangeDName:
			case nItem::Cash_ChangeTName:
			case nItem::Cash_ExtraInven:
			case nItem::Cash_ExtraWarehouse	:
			case nItem::Cash_JumpBuster:
			case nItem::DigimonSlotInc:
			case nItem::Cash_DigimonCareSlot:
			case nItem::ChangeTamer:
			case nItem::Cash_Shouter_N:
			case nItem::Cash_Shouter_S:
			case nItem::Cash_Shouter_NT:
			case nItem::Cash_Shouter_ST:
			case nItem::GM_Shouter_T:
			case nItem::ScanUse_Item:
			case nItem::TimeSet:
			case nItem::Item_TamerSKillConsume:
			case nItem::Fire_Work:
			case nItem::Cash_CrossInvenSlot:
			case nItem::Debuff:
			case nItem::CardMaster:
			case nItem::UnSealCardMaster:
			case nItem::ReSealCardMaster:
			case nItem::Cash_Shouter_Ban:
			case nItem::RelocateTamer:
			case nItem::TamerSlotExpansion:
			case nItem::SkillLevelOpen:
				return true;
			}
		}
		break;
	case eDigimon:
		{
			switch( nType )
			{
			case nItem::TacticsEnchantReset:
			case nItem::DigimonDataChange:
			case nItem::DigimonSkillChipATK:
			case nItem::DigimonSkillChipDEF:
			case nItem::DigimonSkillChipAST:
			case nItem::DigimonSkillMemoryChip:
			case nItem::Matrial2:
			case nItem::Revision:
			case nItem::Rescale:
			case nItem::Digitama1:
			case nItem::Digitama3:
			case nItem::Digitama4:
			case nItem::Digitama5:
			case nItem::Digitama6:
			case nItem::Cash_Evoluter:
#if COMMON_LIB_FIXED
			case nItem::Cash_Evoluter2:
#endif
			case nItem::Cash_ResetSkillPoint:
			case nItem::Cash_BackupDisk:
			case nItem::Cash_Resurection:
			case nItem::Excelator:
			case nItem::BattleTag:
			case nItem::BackupDisk:
			case nItem::RClick_Hatch:
			case nItem::Incubater:
			case nItem::EvoCapsule:
			case nItem::Spirit:
			case nItem::SpiritEXP:
			case nItem::FriendShip_Increase:
			case nItem::AntiXEvoluter:
				return true;
			}
		}
		break;
	case eEtc:
		{
			switch( nType )
			{
			case nItem::Memory:
			case nItem::Capsule:
			case nItem::Card:
			case nItem::PersonStore:
			case nItem::Portable_Item:
			case nItem::Quest:
			case nItem::EventItem:
			case nItem::EventItem2:
			case nItem::UseQuest:
			case nItem::Matrial:
			case nItem::DropMoney:
			case nItem::Money:
			case nItem::Capsule_Coin:
			case nItem::Cash_DigiCore:
			case nItem::GoldBar:
			case nItem::AttributeItem:
			case nItem::GotchaCoin:
			case nItem::BuffEvent:
				return true;
			}
		}
		break;
	}

	return false;
}

bool cInventoryContents::IsOpenInventory() const
{
	return m_eState != eCloseUI ? true : false;
}

bool cInventoryContents::IsMapNewItem() const
{
	if( m_mapNewItem.empty() )
		return false;
	return true;
}

bool cInventoryContents::IsNewItem(int nInvenIndex)
{
	std::map< int, bool >::iterator it = m_mapNewItem.find( nInvenIndex );
	if( it != m_mapNewItem.end() )
	{
		cItemInfo* pItemInfo = GetInvenItem( nInvenIndex );
		if( !pItemInfo->IsEnable() )
		{
			m_mapNewItem.erase( it );
			return false;
		}
		return it->second;
	}

	return false;
}

bool cInventoryContents::IsFilteringItem(int nInvenIndex)
{
	std::map< int, int >::iterator it = m_mapFilterItem.begin();
	std::map< int, int >::iterator itEnd = m_mapFilterItem.end();
	for( ; it != itEnd; ++it )
	{
		if( nInvenIndex == it->second )
			return true;
	}
	return false;
}

bool cInventoryContents::IsPlayingTutorial() const
{
	if( m_AdaptTutorialQuest && m_AdaptTutorialQuest->IsTutorialPlaying() )
		return true;

	return false;
}

void cInventoryContents::SendSortStart(nItemSort::eSortFlag eFlag)
{
	m_mapNewItem.clear();
	bool bIsEnable = false;
	ContentsStream kSend;
	kSend << bIsEnable;
	NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_Inventory_NewItem, kSend );

	if( g_pDataMng )
	{
		CsCoolTimeSeq* pCoolTimeSeq = g_pDataMng->GetSortCoolTime();
		if( pCoolTimeSeq )
			pCoolTimeSeq->Start();
	}

	if( net::game )
		net::game->SendItemSort( static_cast<u1>(nItemSort::eINVEN) );
}

#endif

void cInventoryContents::CloseGiftBoxWindow()
{
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GIFTBOX );

	m_nPreRankNo = 0;
}

void cInventoryContents::SetGiftBoxWindow(void* pData)
{
	SAFE_POINTER_RET( pData );
	nsInventory::sGiftBox* pItem = static_cast< nsInventory::sGiftBox* >( pData );

	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
		{
			pInven->ItemCrop( &pItem->itemData );
			pInven->OutputCropMsg( &pItem->itemData );
		}
	}

	sRankInfo const * rankInfo = nsCsFileTable::g_pItemMng->GetRankInfo( pItem->nUsedItemType, pItem->itemData.m_nType );
	if( !rankInfo )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GIFTBOX );
		return;
	}

	if( 0 == rankInfo->nRank || m_nPreRankNo > rankInfo->nRank )
		return;

	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_GIFTBOX ) )
	{
		cBaseWindow* pWindow = g_pGameIF->GetDynamicIF( cBaseWindow::WT_GIFTBOX );
		SAFE_POINTER_RET( pWindow );
	}

	m_nPreRankNo = rankInfo->nRank;

	std::wstring wsItemName = nsCsFileTable::g_pItemMng->GetItemName( pItem->itemData.m_nType );
	if( wsItemName.empty() )
		wsItemName = L"???";
	ContentsStream kSend;
	kSend << rankInfo->dwIconNo << pItem->itemData.GetCount() << wsItemName << m_nPreRankNo;
	Notify( eAdd_GiftBoxWindow, kSend );
}

void cInventoryContents::UpdateGiftBox(void* pData)
{
	SAFE_POINTER_RET( pData );
	ContentsStream nRecv = *static_cast< ContentsStream* >( pData );

	uint nUsedItemType;
	uint nItemType;
	int nCount;
	nRecv >> nUsedItemType >> nItemType >> nCount;

	sRankInfo const * rankInfo = nsCsFileTable::g_pItemMng->GetRankInfo( nUsedItemType, nItemType );
	if( !rankInfo )
		return;

	if( 0 == rankInfo->nRank || m_nPreRankNo > rankInfo->nRank )
		return;

	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_GIFTBOX ) )
	{
		cBaseWindow* pWindow = g_pGameIF->GetDynamicIF( cBaseWindow::WT_GIFTBOX );
		SAFE_POINTER_RET( pWindow );
	}

	m_nPreRankNo = rankInfo->nRank;

	std::wstring wsItemName = nsCsFileTable::g_pItemMng->GetItemName( nItemType );
	if( wsItemName.empty() )
		wsItemName = L"???";
	ContentsStream kSend;
	kSend << rankInfo->dwIconNo << nCount << wsItemName << m_nPreRankNo;
	Notify( eAdd_GiftBoxWindow, kSend );
}


//////////////////////////////////////////////////////////////////////////
// Change Tamer

void cInventoryContents::RequestUsingChangeTamer(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nInvenIndex = *static_cast< int* >( pData );

	SAFE_POINTER_RET( g_pGameIF );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CHANGETAMER ) )
		return;

	cItemInfo* pInvenItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RET( pInvenItem );

	CsItem::sINFO* pFTInfo = GetFTItemInfo( pInvenItem->GetType() );
	SAFE_POINTER_RET( pFTInfo );

	if( 0 == pFTInfo->s_nType_S )
		cPrintMsg::PrintMsg( cPrintMsg::USE_ITEM_CHANGE_SAVERS ); // 세이버즈 테이머 체인지 아이템 메세지박스
	else
	{
		SAFE_POINTER_RET( g_pCharMng );
		CTamerUser* pUser = g_pCharMng->GetTamerUser();
		SAFE_POINTER_RET( pUser );
		// 동일한 어드벤쳐 테이머로 변경 불가능
		if( nTamer::Ikuto + pFTInfo->s_nType_S == pUser->GetFTID() )
		{
			cPrintMsg::PrintMsg( 10004 ); // 사용 할 수 없습니다.
			return;
		}
		cPrintMsg::PrintMsg( cPrintMsg::USE_ITEM_CHANGE_ADVENTURE ); // 어드벤처 테이머 체인지 아이템 메세지박스
	}

	cMessageBox::GetFirstMessageBox()->SetValue1( nInvenIndex );
}

void cInventoryContents::OpenChangeTamer(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nInvenIndex = *static_cast< int* >( pData );

	SAFE_POINTER_RET( g_pGameIF );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CHANGETAMER ) )
		return;

	cItemInfo* pInvenItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RET( pInvenItem );

	CsItem::sINFO* pFTInfo = GetFTItemInfo( pInvenItem->GetType() );
	SAFE_POINTER_RET( pFTInfo );

	std::list< int > lChangeList;
	_GetChangeTamerList( pFTInfo->s_nType_S, lChangeList );
	int nListCount = lChangeList.size();
	if( 0 == nListCount )
		return;

	cBaseWindow* pWin = g_pGameIF->GetDynamicIF( cBaseWindow::WT_CHANGETAMER, 0, nListCount );
	SAFE_POINTER_RET( pWin );

	if( g_pDataMng )
		g_pDataMng->ItemLock( TO_INVEN_SID( nInvenIndex ) );

	ContentsStream kSend;
	kSend << nInvenIndex << lChangeList;
	Notify( eOpenChangeTamer, kSend );
}

void cInventoryContents::SendChangeTamer(void* pData)
{
	SAFE_POINTER_RET( pData );
	std::pair<int, int> pairData = *static_cast< std::pair<int, int>* >( pData );

	SAFE_POINTER_RET( net::game );
	net::game->SendChangeTamer( pairData.first, pairData.second );

	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CHANGETAMER );

	SAFE_POINTER_RET( g_pCharMng );
	CsC_AvObject* pObject = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pObject );
	pObject->PlaySound( SOUND_LEVEL_UP );
	DWORD dwPlag = nsEFFECT::POS_CHARPOS;
	CsC_EffectProp* pEftProp = pObject->GetProp_Effect();
	SAFE_POINTER_RET( pEftProp );
	pEftProp->AddEffect( EFFECT_DIGIMON_LEVELUP, pObject->GetToolWidth()*0.02f, dwPlag );
}

bool cInventoryContents::_CheckEquipments()
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	cData_TEquip* pTEquip = g_pDataMng->GetTEquip();
	SAFE_POINTER_RETVAL( pTEquip, false );

	if( pInven->GetEmptySlotCount() < pTEquip->GetEquipedCount() )
	{
		cPrintMsg::PrintMsg( INVEN_IS_NOT_ENOUGTH_MSG_ID );	// 인벤토리에 공간이 부족합니다.
		return false;
	}

	return true;
}

void cInventoryContents::_GetChangeTamerList(USHORT const& nItemTypeS, std::list< int >& lChangList)
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );

	SAFE_POINTER_RET( nsCsFileTable::g_pTamerMng );

	// 세이버즈 테이머로 변경
	if( 0 == nItemTypeS )
	{
		for( int i = 0; i < 4/*Savers Count*/; ++i )
		{
			if( !nsCsFileTable::g_pTamerMng->IsTamer( nTamer::Masaru + i ) )
				continue;

			if( pUser->GetFTID() == nTamer::Masaru + i )
				continue;

			lChangList.push_back( nTamer::Masaru + i );
		}
	}
	// 어드벤처 테이머로 변경
	else
		lChangList.push_back( nTamer::Ikuto + nItemTypeS );
}

void cInventoryContents::SelectChangeList(int nTamerID)
{
	SAFE_POINTER_RET( nsCsFileTable::g_pTamerMng );
	if( !nsCsFileTable::g_pTamerMng->IsTamer( nTamerID ) )
		return;

	CsTamer* pFTTamer = nsCsFileTable::g_pTamerMng->GetTamer( nTamerID );
	SAFE_POINTER_RET( pFTTamer );
	CsTamer::sINFO* pFTInfo = pFTTamer->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	std::wstring wsName = pFTInfo->s_szName;
	if( wsName.empty() )
		wsName = L"Unknown Tamer";

	std::wstring wsComment = pFTInfo->s_szComment;
	if( wsComment.empty() )
		wsComment = L"???";

	ContentsStream kSend;
	kSend << wsName << wsComment;
	Notify( eChangeDiscript, kSend );
}

void cInventoryContents::ChangeTamer(int const& nInvenIndex, int const& nTamerID)
{
	// 착용 중인 장비를 넣을 공간이 있는지
	if( !_CheckEquipments() )
		return;

	SAFE_POINTER_RET( nsCsFileTable::g_pTamerMng );
	if( !nsCsFileTable::g_pTamerMng->IsTamer( nTamerID ) )
		return;

	CsTamer* pFTTamer = nsCsFileTable::g_pTamerMng->GetTamer( nTamerID );
	SAFE_POINTER_RET( pFTTamer );
	CsTamer::sINFO* pFTInfo = pFTTamer->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	cPrintMsg::PrintMsg( 30180, pFTInfo->s_szName );
	cMessageBox::GetFirstMessageBox()->SetValue1( pFTInfo->s_dwTamerID );
	cMessageBox::GetFirstMessageBox()->SetValue2( nInvenIndex );
}

void cInventoryContents::CloseChangeWindow(int const& nInvenIndex)
{
	if( g_pDataMng )
		g_pDataMng->ItemUnlock( TO_INVEN_SID( nInvenIndex ) );

	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CHANGETAMER );
}

std::string cInventoryContents::GetTamerIamgeS(int const& nTamerID) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTamerMng, "" );
	if( !nsCsFileTable::g_pTamerMng->IsTamer( nTamerID ) )
		return "";

	SAFE_POINTER_RETVAL( g_pModelDataMng, "" );
	return g_pModelDataMng->GetSmallModelIconFile( nTamerID );
}

std::wstring cInventoryContents::GetTamerName(int const& nTamerID) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTamerMng, L"" );
	if( !nsCsFileTable::g_pTamerMng->IsTamer( nTamerID ) )
		return L"";

	CsTamer* pFTTamer = nsCsFileTable::g_pTamerMng->GetTamer( nTamerID );
	SAFE_POINTER_RETVAL( pFTTamer, L"" );
	CsTamer::sINFO* pFTInfo = pFTTamer->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, L"" );

	return pFTInfo->s_szName;
}

std::wstring cInventoryContents::GetTamerGroup(int const& nTamerID) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTamerMng, L"" );
	if( !nsCsFileTable::g_pTamerMng->IsTamer( nTamerID ) )
		return L"";

	CsTamer* pFTTamer = nsCsFileTable::g_pTamerMng->GetTamer( nTamerID );
	SAFE_POINTER_RETVAL( pFTTamer, L"" );
	CsTamer::sINFO* pFTInfo = pFTTamer->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, L"" );

	return pFTInfo->s_szPart;
}

//////////////////////////////////////////////////////////////////////////