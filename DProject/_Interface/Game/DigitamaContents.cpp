#include "StdAfx.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "DigitamaContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

int const DigitamaContents::IsContentsIdentity(void)
{
	return E_CT_DIGITAMA_CONTENTS;
}

void DigitamaContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_Inventory_UseItem:
		{
			int nInvenIndex = 0;
			kStream >> nInvenIndex;
			RegisterItem( &nInvenIndex );
		}
		break;
	}
}

DigitamaContents::DigitamaContents(void)
: m_pRegistItem( NULL ),
m_nRegisterIndex( 0 ),
m_nPotableIndex( 0 ),
m_nSearchIndex( 0 ),
m_pTarget( NULL ),
m_bRecvPacket( false )
{
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_SCAN_ADD_DATA, this, &DigitamaContents::RecvScanItem );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_SELL_SUCCESS, this, &DigitamaContents::Sell_Success );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_OPEN_NPC,			this, &DigitamaContents::OpenDigitamaNpc );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_OPEN_PORTABLE,		this, &DigitamaContents::OpenDigitamaPortable );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_CLOSE,				this, &DigitamaContents::CloseDigitama );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_SCAN_START,		this, &DigitamaContents::StartDigitamaScan );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_SELL_START,		this, &DigitamaContents::StartDigitamaSell );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_SETITEM,			this, &DigitamaContents::RegisterItem );
#ifdef UI_ITEMSCAN_RENEWAL
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::DIGITAMA_ALLSCAN,			this, &DigitamaContents::AllScanItem );
#endif

	if( EventRouter() )
	{
		EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_Inventory_UseItem, this );
	}
}

DigitamaContents::~DigitamaContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
}

int const DigitamaContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool DigitamaContents::IntraConnection(ContentsSystem& System)
{
	m_pAdaptCommunity = new AdaptCommunityContesnt;
	if( m_pAdaptCommunity )
		m_pAdaptCommunity->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_COMMUNITY ) );
	return true;
}

void DigitamaContents::MakeMainActorData(void)
{
	_UnLockRegistIndex();
	_ResetRegistItem();
	_ClearScanItem();
	m_bRecvPacket = false;
}

// Scan
bool DigitamaContents::_SearchScanItem(int nBegin, int nEnd)
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	for( int i = nBegin; i < nEnd; ++i )
	{
		cItemInfo* pItemInfo = pInven->GetData( i );
		SAFE_POINTER_CON( pItemInfo );

		if( pItemInfo->IsEnable() == false )
			continue;

		if( g_pDataMng->IsScanItem( pItemInfo ) )
		{
			m_nSearchIndex = i + 1;
			if( !_SetScanItem( i ) )
				continue;
			return true;
		}
	}

	return false;
}

bool DigitamaContents::_SetScanItem(int nInvenIndex)
{
	// 아이템 등록
	if( !_SetRegistItem( nInvenIndex ) )
		return false;

	if( !_EnableRegistItem() )
		return false;

	u8 nMoney = _CalcScanMoney( m_pRegistItem->GetType() );

	ContentsStream kTmp;
	kTmp << nMoney << m_pRegistItem->GetCount() << m_pRegistItem->GetType();
	Notify( eRegisterScanItem, kTmp );

	return true;
}

u8 DigitamaContents::_CalcScanMoney( uint nItemType, bool bIsMsg/* = false */ )
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, 0 );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemType );
	SAFE_POINTER_RETVAL( pFTItem, 0 );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, 0 );

	u8 nTotalMoney = pFTInfo->s_dwScanPrice;
#ifdef UI_ITEMSCAN_RENEWAL
	if( 0 == nTotalMoney )
		return nTotalMoney;
#endif

#ifdef GUILD_RENEWAL
	u8 nNeedMoney = nTotalMoney;
	int iReduceRate = 0;
	if(m_pAdaptCommunity != NULL)
		iReduceRate = m_pAdaptCommunity->GetUsingSkillValue( GUILD_SCANREDUCE );

	if( iReduceRate > 0 )// 감소되는 돈 계산
	{		
		u8 nReducedMoneyByGuildSkill = (u8)( ( nTotalMoney * iReduceRate ) / 10000 );
		nTotalMoney -= nReducedMoneyByGuildSkill;

		// “길드스킬로 인해 소모되는 비용이 [Money]만큼 감소합니다.”
		if( bIsMsg )
			cPrintMsg::PrintMsg( 30221, &sMONEY(nTotalMoney), &sMONEY(nNeedMoney), &sMONEY(nReducedMoneyByGuildSkill) );
	}
	else
	{
		// “스캔을 통해 [Money]가 소모되었습니다."
		if( bIsMsg )
			cPrintMsg::PrintMsg( 20098, &sMONEY(nTotalMoney) );
	}
#endif

	return nTotalMoney;
}

void DigitamaContents::_ClearScanItem()
{
	m_lScanItem.clear();
	Notify( eUpdateScanItem );
}

#ifdef UI_ITEMSCAN_RENEWAL
void DigitamaContents::_UpdateScannedItems(uint nUsedItem, std::list<nsItemScan::sScanInfo>& lScanItems, bool bIsSkip)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );

	// 같은 타입끼리 합치고 스캔해서 얻은 수량만 저장
	std::map< uint, cItemInfo > mapMerge;
	std::list<nsItemScan::sScanInfo>::iterator it = lScanItems.begin();
	for( ; it != lScanItems.end(); ++it )
	{
		cItemInfo* pInvenItem = pInven->GetData( it->uInvenIdx );
		SAFE_POINTER_CON( pInvenItem );
		cItemInfo itemInfo;
		itemInfo = it->itemData;
		if( pInvenItem->IsEnable() )
			itemInfo.SetCount( it->itemData.GetCount() - pInvenItem->GetCount() );
		else
			itemInfo.SetCount( it->itemData.GetCount() );

		std::map< uint, cItemInfo >::iterator mit = mapMerge.find( itemInfo.m_nType );
		if( mit == mapMerge.end() )
			mapMerge.insert( std::make_pair( itemInfo.GetType(), itemInfo));
		else
			mit->second.m_nCount += itemInfo.GetCount();
	}

	// 인벤토리에 스캔된 아이템 추가
	it = lScanItems.begin();
	for( ; it != lScanItems.end(); ++it )
	{
		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( it->itemData.m_nType );
		if( pFTItem )
		{
			CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
			if( pFTInfo )
			{
				// 사용 시간 적용(분)
				it->itemData.m_nEndTime = pFTInfo->s_nUseTime_Min * 60;

				switch( pFTInfo->s_btUseTimeType )
				{
				case 1:
				case 2:
				case 3:
					{
						if( it->itemData.m_nEndTime != 0 )
							it->itemData.m_nEndTime += _TIME_TS;
					}
					break;
				case 0:
				case 4:
					it->itemData.m_nEndTime = ITEMINFO_INVALIDE_TIME;
					break;
				default:
					assert_cs( false );
				}
			}
		}
		pInven->SetInvenItemData( it->uInvenIdx, it->itemData );
	}

	std::wstring wsUsedItemName;
	CsItem* pFTUsedItem = nsCsFileTable::g_pItemMng->GetItem( nUsedItem );
	if( pFTUsedItem )
	{
		CsItem::sINFO* pFTUsedInfo = pFTUsedItem->GetInfo();
		if( pFTUsedInfo )
			wsUsedItemName = pFTUsedInfo->s_szName;
	}

	std::map< uint, cItemInfo >::iterator mit = mapMerge.begin();
	for( ; mit != mapMerge.end(); ++mit )
	{
		m_lScanItem.push_front( mit->second );
		if( m_lScanItem.size() > IF_SCAN_MAX_COUNT )
			m_lScanItem.pop_back();

		uint nItemType = mit->second.m_nType;
		uint nCount = mit->second.GetCount();

		// ui 갱신
		ContentsStream kSend;
		kSend << nItemType;
		Notify( eUpdateScanItem, kSend );

		// 아이템 랭크 이펙트
		ContentsStream kTmp;
		kTmp << nUsedItem << nItemType << nCount;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_GIFTBOX, &kTmp );

		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemType );
		SAFE_POINTER_CON( pFTItem );
		CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
		SAFE_POINTER_CON( pFTInfo );

		switch( pFTInfo->s_nType_L )
		{
		case nItem::Money:
			cPrintMsg::PrintMsg( 32004, const_cast<TCHAR*>(wsUsedItemName.c_str()), pFTInfo->s_szName, &sMONEY( nCount * 1000 ) );
			break;
		default:
			cPrintMsg::PrintMsg( 32003, const_cast<TCHAR*>(wsUsedItemName.c_str()), pFTInfo->s_szName, &nCount );
			break;
		}

		if( bIsSkip )
			continue;

		switch( pFTInfo->s_nType_L )
		{
		case nItem::Money:
			{
				sMONEY money( nCount * 1000 );
				cPrintMsg::PrintMsg( 20097, pFTInfo->s_szName, &money );
			}
			break;
		default:
			{
				if( nCount > 1 )
					cPrintMsg::PrintMsg( 20096, pFTInfo->s_szName, &nCount );
				else
					cPrintMsg::PrintMsg( 20095, pFTInfo->s_szName );
			}
			break;
		}
	}

	_LockInventory();
}

void DigitamaContents::_UpdateInvenMoney(u4 const& nUsedItem, u2 const& nUsedCount, n8 const& nRemainMoney, n8 const& nUsedMoney)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	// 남은 돈 갱신
	pInven->SetMoney( nRemainMoney, true );

	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nUsedItem );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	u8 nTotalMoney = pFTInfo->s_dwScanPrice;

#ifdef GUILD_RENEWAL
	u8 nNeedMoney = nTotalMoney;
	int iReduceRate = 0;
	if(m_pAdaptCommunity != NULL)
		iReduceRate = m_pAdaptCommunity->GetUsingSkillValue( GUILD_SCANREDUCE );

	if( iReduceRate > 0 )// 감소되는 돈 계산
	{		
		u8 nReducedMoneyByGuildSkill = (u8)( ( nTotalMoney * iReduceRate ) / 10000 );

		// “스캔 비용으로 [money]를 소모하였습니다.\n([money] - (길드 스킬) [money])”
		cPrintMsg::PrintMsg( 30221, &sMONEY(nUsedMoney * -1), &sMONEY(nNeedMoney * nUsedCount), &sMONEY(nReducedMoneyByGuildSkill * nUsedCount) );
		return;
	}
#endif
	// “[Str] 스캔을 통해 [Money]가 소모되었습니다."
	cPrintMsg::PrintMsg( 20098, pFTInfo->s_szName, &sMONEY( nUsedMoney * -1 ) );
}

#endif

bool DigitamaContents::_SendScanItem(uint nCount)
{
	if( !_CheckRegisterItem( true ) )
		return false;

	_InventoryEnable( false );

	if( m_pTarget )	// npc
	{
		if( !m_pTarget->IsEnableObject() )
			return false;

		if( net::game )
		{
			if( m_pRegistItem && m_pRegistItem->IsEnable() )
			{
				net::game->SendDigitamaScanItem( false, 0, m_pTarget->GetFTID(), m_nRegisterIndex, nCount );
				m_bRecvPacket = true;
				return true;
			}
		}
	}
	else			// portable
	{
		if( net::game )
		{
			if( m_pRegistItem && m_pRegistItem->IsEnable() )
			{
#ifdef ITEM_USE_TIME_PASS
				if( -1 == m_nPotableIndex )
					net::game->SendDigitamaScanItem( true, 0, 0, m_nRegisterIndex, nCount );
				else
					net::game->SendDigitamaScanItem( false, m_nPotableIndex, 0, m_nRegisterIndex, nCount );
#else
				if( -1 == m_nPotableIndex )
					net::game->SendDigitamaScanItem( true, 0, 0, m_nRegisterIndex, nCount );
				else
					net::game->SendDigitamaScanItem( false, 0, 0, m_nRegisterIndex, nCount );
#endif
				m_bRecvPacket = true;
				return true;
			}
		}
	}

	return false;
}

#ifndef UI_ITEMSCAN_RENEWAL
void DigitamaContents::_AddScannedItem(cItemData& pData)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	// 아이템 추가
	pInven->ItemCrop( &pData );

	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pData.m_nType );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );
	pData.m_nEndTime = pFTInfo->s_nUseTime_Min * 60;

	switch( pFTInfo->s_btUseTimeType )
	{
	case 1:
	case 2:
	case 3:
		if( pData.m_nEndTime != 0 )
		{
			pData.m_nEndTime += _TIME_TS;	
		}
		break;
	case 0:
	case 4:
		pData.m_nEndTime = ITEMINFO_INVALIDE_TIME;
		break;
	default:
		assert_cs( false );
	}

	switch( pFTInfo->s_nType_L )
	{
	case nItem::Money:
		{
			sMONEY money( pData.GetCount()*1000 );
			cPrintMsg::PrintMsg( 20097, pFTInfo->s_szName, &money );
		}
		break;
	default:
		{
			if( pData.GetCount() > 1 )
			{
				int nCount = pData.GetCount();
				cPrintMsg::PrintMsg( 20096, pFTInfo->s_szName, &nCount );
			}
			else
			{
				cPrintMsg::PrintMsg( 20095, pFTInfo->s_szName );
			}
		}
		break;
	}

	cItemInfo itemInfo;
	itemInfo = pData;

	// 스캔된 아이템 추가
	m_lScanItem.push_front( itemInfo );
	if( m_lScanItem.size() > IF_DSCAN_ITEM_COUNT )
	{
		m_lScanItem.pop_back();
	}
	Notify( eUpdateScanItem );
}
#endif

// Sell
bool DigitamaContents::_SearchSellItem(int nBegin, int nEnd, int nType)
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	for( int i = nBegin; i < nEnd; ++i )
	{
		CsItem::sINFO* pFTInfo = pInven->GetFTData( i );
		SAFE_POINTER_CON( pFTInfo );

		if( pFTInfo->s_nType_L == nType )
		{
			if( pFTInfo->s_dwSale > 0 )
			{
				m_nSearchIndex = i + 1;
				if( !_SetSellItem( i ) )
					continue;
				return true;
			}
		}
	}

	return false;
}

bool DigitamaContents::_SetSellItem(int nInvenIndex)
{
	// 아이템 등록
	if( !_SetRegistItem( nInvenIndex ) )
		return false;

	if( !_EnableRegistItem() )
		return false;

	u8 nMoney = _CalcSellMoney( m_pRegistItem->GetType(), m_pRegistItem->GetCount() );

	ContentsStream kTmp;
	kTmp << nMoney << m_pRegistItem->GetCount() << m_pRegistItem->GetType();
	Notify( eRegisterSellItem, kTmp );

	return true;
}

u8 DigitamaContents::_CalcSellMoney(uint nItemType, uint nItemCount)
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, false );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemType );
	SAFE_POINTER_RETVAL( pFTItem, false );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, false );

	u8 nSellMoney = (u8)pFTInfo->s_dwSale * nItemCount;

#ifdef GUILD_RENEWAL
	int nVal = 0;
	if ( m_pAdaptCommunity != NULL )
		nVal = m_pAdaptCommunity->GetUsingSkillValue( GUILD_SCANBITUP );
	u8 nAddMoney = 0;
	if( nVal != 0 )
	{
		nAddMoney = (u8)( ( nSellMoney * nVal ) / 10000 );
	}
	nSellMoney += nAddMoney;	
#endif

	return nSellMoney;
}

// Regist
bool DigitamaContents::_SetRegistItem(int nInvenIndex)
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	// 기존에 등록되어 있던 아이템 언락
	if( m_pRegistItem && m_pRegistItem->IsEnable() )
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nRegisterIndex ) );

	cItemInfo* pItemInfo = pInven->GetData( nInvenIndex );
	SAFE_POINTER_RETVAL( pItemInfo, false );
	if( !pItemInfo->IsEnable() )
		return false;

	m_nRegisterIndex = nInvenIndex;
	m_pRegistItem = pItemInfo;

	g_pDataMng->ItemLock( TO_INVEN_SID( m_nRegisterIndex ) );
	return true;
}

void DigitamaContents::_ResetRegistItem()
{
	m_pRegistItem = NULL;
	m_nRegisterIndex = 0;
}

bool DigitamaContents::_CheckRegisterItem( bool bIsScan )
{
	if( !_EnableRegistItem() )
		return false;

	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	if( bIsScan )
	{
		if( !g_pDataMng->IsScanItem( m_pRegistItem ) )
		{
			cPrintMsg::PrintMsg( 11002 );
			return false;
		}

		// 인벤 남은 공간 확인
		int nEmptySlot = pInven->GetEmptySlotCount();
		if( --nEmptySlot < 0 )
		{
			cPrintMsg::PrintMsg( 11015 );
			return false;
		}

		u8 nMoney = _CalcScanMoney( m_pRegistItem->GetType() );

		// 돈이 충분한가
		if( pInven->GetMoney() < nMoney )
		{
			// 돈부족
			cPrintMsg::PrintMsg( cPrintMsg::SHOP_MONTY_SHORT );
			return false;
		}
	}
	else
	{
		if( !g_pDataMng->IsSellItem( m_pRegistItem ) )
		{
			cPrintMsg::PrintMsg( 11001 );
			return false;
		}
	}

	return true;
}

bool DigitamaContents::_CheckScanMoney(int& nCount)
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	if( !_EnableRegistItem() )
		return false;

	u8 nMoney = _CalcScanMoney( m_pRegistItem->GetType() );
	if( 0 == nMoney )
	{
		nCount = m_pRegistItem->GetCount();
		return true;
	}

	u8 nInvenMoney = pInven->GetMoney().GetInt();
	nCount = (nInvenMoney / nMoney);
	if( 0 == nCount )
	{
		cPrintMsg::PrintMsg( cPrintMsg::SHOP_MONTY_SHORT );
		return false;
	}

	if( nCount >= m_pRegistItem->GetCount() )
		nCount = m_pRegistItem->GetCount();

	return true;
}

bool DigitamaContents::_EnableRegistItem()
{
	SAFE_POINTER_RETVAL( m_pRegistItem, false );
	if( !m_pRegistItem->IsEnable() )
		return false;

	return true;
}

void DigitamaContents::_RegisterCancel()
{
	_InventoryEnable( true );
	_UnLockRegistIndex();
	_ResetRegistItem();
}

void DigitamaContents::_UnLockRegistIndex()
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	pInven->ItemUnlock_InvenSlot( m_nRegisterIndex );
}

// Inventory
void DigitamaContents::_LockInventory()
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	int nInvenSlotCount = pInven->GetInvenSlotCount();
	for( int i = 0; i < nInvenSlotCount; ++i )
	{
		cItemInfo* pItemInfo = pInven->GetData( i );
		SAFE_POINTER_CON( pItemInfo );
		if( !pItemInfo->IsEnable() )
			continue;

		if( !g_pDataMng->IsScanItem( pItemInfo ) )
			pInven->Itemlock_InvenSlot( TO_INVEN_SID( i ) );
	}
}

void DigitamaContents::_UnLockInventory()
{
	SAFE_POINTER_RET( g_pDataMng );

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	pInven->ItemUnlock_AllItem();
	pInven->ItemUnlock_ItemType( nItem::Portable_Item, eSCANNER );
	pInven->ItemUnlock_ItemType( nItem::Portable_Item, ePAY_BACK );
}

void DigitamaContents::_InventoryEnable(bool enable)
{
	bool bIsEnable = enable;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable );
}

void DigitamaContents::_SetDigitamaWindow(cBaseWindow::eWINDOW_TYPE eType, int nIndex)
{
	SAFE_POINTER_RET( g_pGameIF );

	// 스캔 / 회수 같이 열면 안 된다
	if( cBaseWindow::WT_DIGITAMA_SCAN == eType )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SELL ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DIGITAMA_SELL );
	}
	else
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SCAN ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DIGITAMA_SCAN );
	}

	cBaseWindow* pWindow = g_pGameIF->GetDynamicIF( eType, 0, nIndex, true );
	SAFE_POINTER_RET( pWindow );

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	// 휴대용 스캐너 lock
	pInven->ItemLock_ItemType( nItem::Portable_Item, eSCANNER);
	// 휴대용 회수기 lock
	pInven->ItemLock_ItemType( nItem::Portable_Item, ePAY_BACK);

	// Scanner 위치
	CsPoint csPos = pWindow->GetRootSize();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_POS_DEPENDON_INVEN, &csPos );
	pWindow->SetPos( csPos );
	pWindow->ResetDevice();

#ifdef UI_ITEMSCAN_RENEWAL
	if( cBaseWindow::WT_DIGITAMA_SCAN == eType )
		_LockInventory();
#endif
}

// Get Register Item
cItemInfo* DigitamaContents::Get_ResistItem_Ptr()
{
	return m_pRegistItem;
}

bool DigitamaContents::Get_ResistItemEnable()
{
	SAFE_POINTER_RETVAL( m_pRegistItem, false );
	return true;
}

uint DigitamaContents::Get_ResistItemCount() const
{
	SAFE_POINTER_RETVAL( m_pRegistItem, 0 );
	return m_pRegistItem->GetCount();
}

uint DigitamaContents::Get_ResistItemType() const
{
	SAFE_POINTER_RETVAL( m_pRegistItem, 0 );
	return m_pRegistItem->GetType();
}

#ifdef UI_ITEMSCAN_RENEWAL
void DigitamaContents::RecvScanItem( void* pData )
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ITEMSCAN* pRecv = static_cast< GS2C_RECV_ITEMSCAN* >( pData );

	m_bRecvPacket = false;

	if( 0 != pRecv->nResult )
	{
		// 스캔 실패
		// 동기화x - 서버 접속 종료
		// 그 외의 경우 - 메시지 출력할 것
		cPrintMsg::PrintMsg( pRecv->nResult );
	}

	// 등록한 아이템 유효 판정
	if( !_EnableRegistItem() )
	{
		_RegisterCancel();
		Notify( eScanEnd );
		return;
	}

	std::wstring wsUsedItem;
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pRecv->uBeforeScanType );
	if( pFTItem )
	{
		CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
		if( pFTInfo )
			wsUsedItem = pFTInfo->s_szName;
	}

	// 스캔 비용 출력
	_UpdateInvenMoney( pRecv->uBeforeScanType, pRecv->u2BeforeScanCount, pRecv->n8NowMoney, pRecv->n8ScanMoney );

	int nRegisterCount = m_pRegistItem->GetCount();
	// 등록한 아이템을 전부 스캔했으면 그 자리 언락
	if( 0 == nRegisterCount - pRecv->u2BeforeScanCount )
		_UnLockRegistIndex();

	// 등록한 아이템 감소
	m_pRegistItem->DecreaseCount( pRecv->u2BeforeScanCount );
	if( !m_pRegistItem->IsEnable() )
		_RegisterCancel();

	if( 0 != pRecv->u2BeforeScanCount && pRecv->lScanList.empty() )
	{
		cPrintMsg::PrintMsg( 11053 );
		cPrintMsg::PrintMsg( 32005, const_cast<TCHAR*>(wsUsedItem.c_str()) );
	}

	// 스캔된 아이템 인벤토리에 추가
	bool bIsSkip = pRecv->u2BeforeScanCount > 1 ? true : false;
	_UpdateScannedItems( pRecv->uBeforeScanType, pRecv->lScanList, bIsSkip );

	// 등록한 아이템 없거나 더이상 스캔 불가능
	if( !_CheckRegisterItem() )
	{
		_InventoryEnable( true );
		_UnLockRegistIndex();
		_ResetRegistItem();
		Notify( eScanEnd );
		return;
	}

	_InventoryEnable( false );
	Notify( eScanStart );
}
#else
void DigitamaContents::RecvScanItem( void* pData )
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_DIGITAMA_SCANITEM* pRecv = static_cast< GS2C_RECV_DIGITAMA_SCANITEM* >( pData );

	// 등록한 아이템 유효 판정
	if( !_EnableRegistItem() )
	{
		_RegisterCancel();
		Notify( eScanEnd );
		return;
	}

	// 스캔 비용 감소
	u8 nMoney = _CalcScanMoney( m_pRegistItem->GetType(), true );
	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->DecreaseMoney( nMoney );
	}

	int nRegisterCount = m_pRegistItem->GetCount();
	// 등록한 아이템을 전부 스캔했으면 그 자리 언락
	if( 0 == nRegisterCount - 1 )
		_UnLockRegistIndex();

	// 등록한 아이템 감소
	m_pRegistItem->DecreaseCount( 1 );
	if( !m_pRegistItem->IsEnable() )
		_RegisterCancel();

	if( pRecv->m_ItemList.empty() )
		cPrintMsg::PrintMsg( 11053 );

	// 스캔된 아이템 추가
	std::list<cItemInfo>::iterator it = pRecv->m_ItemList.begin();
	std::list<cItemInfo>::iterator itEnd = pRecv->m_ItemList.end();
	for( ; it != itEnd; ++it )
		_AddScannedItem( *it );


	// 등록한 아이템 없거나 더이상 스캔 불가능
	if( !_CheckRegisterItem( true ))
	{
		_RegisterCancel();
		Notify( eScanEnd );
		return;
	}

	_InventoryEnable( false );
	Notify( eScanStart );
}
#endif

void DigitamaContents::StartDigitamaScan(void* pData)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SCAN ) )
		return;

	_InventoryEnable( false );
	Notify( eScanStart );
}

void DigitamaContents::ProcessStart()
{
	// 스캔 유효판정
	if( !_CheckRegisterItem() )
	{
		_RegisterCancel();
		Notify( eScanEnd );
		return;
	}

	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_pRegistItem->GetType() );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	// 용병알이면서 케쉬템일 경우~ 메세지표현
#ifdef VERSION_USA
	if( pFTInfo->s_nType_L == nItem::Digitama3 )
	{
		cPrintMsg::PrintMsg( 20079, pFTInfo->s_szName );
		return;
	}
#else
	if( ( pFTInfo->s_nType_L == nItem::Digitama3 ) && pFTInfo->IsCashType() )
	{
		cPrintMsg::PrintMsg( 20080, pFTInfo->s_szName );
		return;
	}
#endif
	// 스캔 중 인벤 사용 불가
	_InventoryEnable( false );
	Notify( eScanStart );
}

#ifdef UI_ITEMSCAN_RENEWAL
void DigitamaContents::ProcessSuccess()
{
	// 패킷 기다리는 중
	if( m_bRecvPacket )
		return;

	if( !_SendScanItem( 1 ) )
	{
		_InventoryEnable( true );
		Notify( eScanEnd );
	}
}
#else
void DigitamaContents::ProcessSuccess()
{
	if( m_pTarget )	// npc
	{
		if( !m_pTarget->IsEnableObject() )
			return;

		if( net::game )
		{
			net::game->SendDigitamaToItem( m_pTarget->GetFTID(), m_nRegisterIndex );
			return;
		}
	}
	else			// portable
	{
		if( net::game )
		{
#ifdef ITEM_USE_TIME_PASS
			if( -1 == m_nPotableIndex )
				net::game->SendDigitamaToItem( 0, m_nRegisterIndex, 0, true );
			else
				net::game->SendDigitamaToItem( 0, m_nRegisterIndex, m_nPotableIndex );
#else
			if( -1 == m_nPotableIndex )
				net::game->SendDigitamaToItem( 0, m_nRegisterIndex, 0, true );
			else
				net::game->SendDigitamaToItem( 0, m_nRegisterIndex );
#endif
			return;
		}
	}

	// process fail!
	_InventoryEnable( true );
	Notify( eScanFail );
}
#endif

void DigitamaContents::ProcessCancel()
{
	_InventoryEnable( true );

	Notify( eScanCancel );
}

void DigitamaContents::ProcessSkip()
{
#ifdef UI_ITEMSCAN_RENEWAL
	ProcessCancel();
	cPrintMsg::PrintMsg( 20099 );
#endif
}

void DigitamaContents::AutoRegisterScan()
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	int nOrgIndex = m_nSearchIndex;

	if( _SearchScanItem( m_nSearchIndex, pInven->GetInvenSlotCount() ) )
		return;

	if( _SearchScanItem( 0, nOrgIndex ) )
		return;

	// 그래도 못찾았다면 없는거다
	m_nSearchIndex = 0;
	_InventoryEnable( true );
	_ResetRegistItem();
	Notify( eScanEnd );
}

std::list< cItemInfo >* DigitamaContents::GetScanItem()
{
	return &m_lScanItem;
}

void DigitamaContents::Sell_Success(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_DIGITAMA_TO_MONEY_SUCCESS* pRecv = static_cast<GS2C_RECV_DIGITAMA_TO_MONEY_SUCCESS*>(pData);

	u8 nTotalGetMoney = pRecv->nTotalPrice;
#ifdef GUILD_RENEWAL
	int nVal = 0;
	if(m_pAdaptCommunity != NULL)
		nVal = m_pAdaptCommunity->GetUsingSkillValue( GUILD_SCANBITUP );
	if( nVal > 0 )
	{		
		u8 nAddMoney = (u8)( ( pRecv->nTotalPrice * nVal ) / 10000 );// 증가되는 돈 계산
		nTotalGetMoney += nAddMoney;
		cPrintMsg::PrintMsg( 30220, &sMONEY(nTotalGetMoney), &sMONEY(pRecv->nTotalPrice), &sMONEY(nAddMoney) );	// “회수를 통해 [Money]를 획득하셨습니다.”
	}
	else
		cPrintMsg::PrintMsg( 20083, &sMONEY(nTotalGetMoney) );	// “회수를 통해 [Money]를 획득하셨습니다.”
#else
	cPrintMsg::PrintMsg( 20083, &sMONEY(pRecv->nTotalPrice) );	// “회수를 통해 [Money]를 획득하셨습니다.”
#endif
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	pInven->IncreaseMoney( nTotalGetMoney, true );

	// 아이템 락 먼저 풀고
	pInven->ItemUnlock_InvenSlot( m_nRegisterIndex );

	cItemInfo* pItemInfo = pInven->GetData( m_nRegisterIndex );
	SAFE_POINTER_RET( pItemInfo );
	if( !pItemInfo->IsEnable() )
		return;
	// 아이템 회수
	pItemInfo->Reset();

	_RegisterCancel();
	Notify( eSellEnd );
}

void DigitamaContents::StartDigitamaSell(void* pData)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SELL ) )
		return;

	_InventoryEnable( false );
	Notify( eSellStart );
}

void DigitamaContents::ProcessStartSell()
{
	// 회수 유효판정
	if( !_CheckRegisterItem( false ) )
	{
		_RegisterCancel();
		Notify( eSellEnd );
		return;
	}

	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_pRegistItem->GetType() );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

#ifdef VERSION_USA
	if( pFTInfo->s_nType_L == nItem::Digitama3 )
	{
		cPrintMsg::PrintMsg( 20079, pFTInfo->s_szName );
		return;
	}
#else
	if( ( pFTInfo->s_nType_L == nItem::Digitama3 ) && pFTInfo->IsCashType() )
	{
		cPrintMsg::PrintMsg( 20081, pFTInfo->s_szName );
		return;
	}
#endif

	// 회수 중 인벤 사용 불가
	_InventoryEnable( false );
	Notify( eSellStart );
}

void DigitamaContents::ProcessSuccessSell()
{
	if( m_pTarget )	// npc
	{
		if( !m_pTarget->IsEnableObject() )
			return;

		if( net::game )
		{
			net::game->SendDigitamaToMoney( m_pTarget->GetFTID(), m_nRegisterIndex );
			return;
		}
	}
	else			// portable
	{
		if( net::game )
		{
#ifdef ITEM_USE_TIME_PASS
			if( -1 == m_nPotableIndex )
				net::game->SendDigitamaToMoney( 0, m_nRegisterIndex, 0, true );
			else
				net::game->SendDigitamaToMoney( 0, m_nRegisterIndex, m_nPotableIndex );
#else
			if( -1 == m_nPotableIndex )
				net::game->SendDigitamaToMoney( 0, m_nRegisterIndex, 0, true );
			else
				net::game->SendDigitamaToMoney( 0, m_nRegisterIndex );
#endif
			return;
		}
	}

	// process fail!
	_InventoryEnable( true );
	Notify( eSellFail );
}

void DigitamaContents::ProcessCancelSell()
{
	_InventoryEnable(true);
	Notify(eSellCancel);
}

void DigitamaContents::AutoRegisterItemSell()
{
	int nOrgIndex = m_nSearchIndex;
	int nPreIndex = nOrgIndex - 1;

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	// 빈알먼저
	{
		if( _SearchSellItem( nOrgIndex, pInven->GetInvenSlotCount(), IF_DIGITAMA_SELL_ITEM_TYPE1 ) )
			return;

		if( _SearchSellItem( 0, nPreIndex, IF_DIGITAMA_SELL_ITEM_TYPE1 ) )
			return;
	}

	// 스캔용알
	{
		if( _SearchSellItem( nOrgIndex, pInven->GetInvenSlotCount(), IF_DIGITAMA_SELL_ITEM_TYPE2 ) )
			return;

		if( _SearchSellItem( 0, nPreIndex, IF_DIGITAMA_SELL_ITEM_TYPE2 ) )
			return;
	}

	if( _SearchSellItem( nPreIndex, nOrgIndex, IF_DIGITAMA_SELL_ITEM_TYPE1 ) ||
		_SearchSellItem( nPreIndex, nOrgIndex, IF_DIGITAMA_SELL_ITEM_TYPE2 ) )
		return;

	// 그래도 못찾았다면 없는거다
	m_nSearchIndex = 0;
	_RegisterCancel();
	Notify( eSellEnd );
}

void DigitamaContents::OpenDigitamaNpc(void* pData)
{
	SAFE_POINTER_RET( pData );
	ContentsStream kTmp = *static_cast< ContentsStream* >( pData );

	UINT nWindowType = 0;
	UINT nIDX = 0;
	kTmp >> nWindowType >> nIDX;

	m_nPotableIndex = 0;
	_SetDigitamaWindow( (cBaseWindow::eWINDOW_TYPE)nWindowType, m_nPotableIndex );

	SAFE_POINTER_RET( g_pNpcMng );
	CNpc* pNpc = g_pNpcMng->GetNpc( nIDX );
	SAFE_POINTER_RET( pNpc );
	m_pTarget = pNpc;
}

void DigitamaContents::OpenDigitamaPortable(void* pData)
{
	SAFE_POINTER_RET( pData);
	ContentsStream kTmp = *static_cast< ContentsStream* >( pData );

	UINT nWindowType = 0;
	int nIndex = 0;
	NiPoint2 niPos;
	kTmp >> nWindowType >> nIndex >> niPos;

	m_nPotableIndex = nIndex;
	_SetDigitamaWindow( (cBaseWindow::eWINDOW_TYPE)nWindowType, m_nPotableIndex );

	m_niTargetPos = niPos;
}

void DigitamaContents::CloseDigitama(void* pData)
{
	SAFE_POINTER_RET( pData);
	UINT nWindowType = *static_cast< UINT* >( pData );

	_InventoryEnable( true );
	_UnLockInventory();

	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->CloseDynamicIF( (cBaseWindow::eWINDOW_TYPE)nWindowType, 0, true );

	m_pTarget = NULL;
	m_niTargetPos = NiPoint2::ZERO;

	_ResetRegistItem();
	_ClearScanItem();
}

void DigitamaContents::RegisterItem(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nInvenIndex = *static_cast< int* >( pData );

	SAFE_POINTER_RET( g_pGameIF );
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	cItemInfo* pItemInfo = pInven->GetData( nInvenIndex );
	SAFE_POINTER_RET( pItemInfo );
	if( !pItemInfo->IsEnable() )
		return;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SCAN ) )
	{
		if( !g_pDataMng->IsScanItem( pItemInfo ) )
		{
			cPrintMsg::PrintMsg( 11002 );
			return;
		}

		_SetScanItem( nInvenIndex );
		return;
	}

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SELL ) )
	{
		if( !g_pDataMng->IsSellItem( pItemInfo ) )
		{
			cPrintMsg::PrintMsg( 11001 );
			return;
		}

		_SetSellItem( nInvenIndex );
		return;
	}
}

#ifdef UI_ITEMSCAN_RENEWAL
void DigitamaContents::AllScanItem(void* pData)
{
	if( m_bRecvPacket )
		return;

	int nCount = 0;
	if( !_CheckScanMoney( nCount ) )
		return;

	_SendScanItem( nCount );
}

int DigitamaContents::GetItemRank(uint nScanItemType, uint nItemType)
{
	if( nsCsFileTable::g_pItemMng )
	{
		sRankInfo const * rankInfo = nsCsFileTable::g_pItemMng->GetRankInfo( nScanItemType, nItemType );
		if( rankInfo )
			return rankInfo->nRank;
	}
	return 0;
}
#endif

CsC_AvObject* DigitamaContents::GetTargetNpc()
{
	return m_pTarget;
}

NiPoint2 DigitamaContents::GetTargetPos()
{
	return m_niTargetPos;
}