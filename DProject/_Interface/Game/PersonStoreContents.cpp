#include "StdAfx.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "PersonStoreContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

PersonStoreContents::PersonStoreContents(void) 
: iUsedItemID( 0 ), iSelectIndex( 0 ), iTargetUniqID( 0 ), m_eValueType( VT_CLOSE_STORE ), m_pTarget( NULL ) 

{
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::REQ_PERSON_OPEN,		  this, &PersonStoreContents::Req_PresonStoreOpen );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::PERSON_ITEM_SELL_ADD,	  this, &PersonStoreContents::Sell_ResistItem );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::PERSON_ITEM_BUY,		  this, &PersonStoreContents::Buy_TradeItem );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::OPEN_PERSONSTORE,		  this, &PersonStoreContents::OpenPersonStore );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::OPEN_COMMISSIONSTORE,	  this, &PersonStoreContents::OpenCommissionStore );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::CLOSE_PERSONSTORE_MSGBOX, this, &PersonStoreContents::ClosePersonStoreMsgBox );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_CLOSE_PERSONSTORE,				this, &PersonStoreContents::RecvClosePersonStore );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_CLOSE_COMMISSIONSTORE,			this, &PersonStoreContents::RecvCloseCommissionStore );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_STORE_READY_RESULT,			this, &PersonStoreContents::RecvStoreReadyResult );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_OPEN_PERSONSTORE,				this, &PersonStoreContents::RecvOpenPersonStore );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_OPEN_COMMISSIONSTORE,			this, &PersonStoreContents::RecvOpenCommissionStore );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_BUY_ITEM_RESULT,				this, &PersonStoreContents::RecvBuyItemResult );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_PERSONSTORE_ITEMLIST,			this, &PersonStoreContents::RecvPersonStoreItemList );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_COMMISSIONSTORE_ITEMLIST,		this, &PersonStoreContents::RecvCommissionStoreItemList );

	if( EventRouter() )
	{
		EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_Inventory_UseItem, this );
	}
}

PersonStoreContents::~PersonStoreContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);

	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
}

int const PersonStoreContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

int const PersonStoreContents::IsContentsIdentity(void)
{
	return E_CT_PERSONSTORE_CONTENTS;
}

void PersonStoreContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_Inventory_UseItem:
		{
			int nInvenIndex = 0;
			kStream >> nInvenIndex;
			RegisterSeparateItem( nInvenIndex );
		}
		break;
	}
}

void PersonStoreContents::MakeMainActorData(void)
{
	DelResistItem();	// 맵을 이동할 때마다 초기화
}

void PersonStoreContents::ClearWorldData(void)
{
	DelResistItem();
}

void PersonStoreContents::Update(float const& fElapsedTime)
{
	if( m_eValueType == VT_CLOSE_STORE ) return;

	if( !CheckTamerState() ) 
	{
		CloseWindowUI();
		return;
	}

	CheckTarget();
}

void PersonStoreContents::Req_PresonStoreOpen(void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( g_pDataMng );
	SAFE_POINTER_RET(nsCsFileTable::g_pItemMng);

	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_BATTLE:
		cPrintMsg::PrintMsg( 30336 );
		return;
	}

	// 개인 상정 창이 열려 있으면 안됨.
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
		return;

	if( !g_pDataMng->IsOpenStore() )
		return;

	int nInvenSlotNum = *static_cast<int*>(pData);

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	cItemInfo* pItem = pInven->GetData( nInvenSlotNum );
	SAFE_POINTER_RET( pItem );

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	if( pFTInfo->s_nType_S != 1 && pFTInfo->s_nType_S != 3 )
		return;

#ifdef ITEM_USE_TIME_PASS
	if(pItem->m_nEndTime == 0xFFFFFFF0)
	{
		g_pDataMng->ItemLock( nInvenSlotNum );
		cPrintMsg::PrintMsg( 30176, pFTInfo->s_szName );									
		cMessageBox::GetFirstMessageBox()->SetValue1( nInvenSlotNum );
		return;
	}
#endif

	if( pFTInfo->s_nType_S == 3 )
	{
		int iMapID = 0;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_GET_CHAR_MAPID, &iMapID);

		// 이미 고용상점이 배치되어 잇는지 검사
		if( iMapID != 0 )
		{
			cPrintMsg::PrintMsg( 30376 );	
			return;
		}
	}

	if( pFTInfo->s_nUseTimeGroup != 0 )
		pFTInfo->GetCoolTimeSeq()->Start();

	// 움직이는거 정지
	if( g_pCharResMng )
		g_pCharResMng->ReleaseMovePoint();

	if( g_pCharMng )
	{
		CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
		if( pDigimon )
		{
			pDigimon->SetDigimonUserState( CDigimonUser::DU_FOLLOW );
		}
		CTamerUser* pTamer = g_pCharMng->GetTamerUser();
		if( pTamer )
		{
			pTamer->ReleaseArriveTarget();
			pTamer->DeletePath();
		}
	}	

	// 아이템 闡?
	pInven->ItemLock_ItemType( nItem::PersonStore, 1 );
	pInven->ItemLock_ItemType( nItem::PersonStore, 3 );	

	LockInventory();
	
	// 동기화
	if( net::game )
	{
		if( pFTInfo->s_nType_S == 1 )
			SendRequestReady( 1, nInvenSlotNum );
		else if( pFTInfo->s_nType_S == 3 )
			SendRequestReady( 4, nInvenSlotNum );


	}
}

void PersonStoreContents::Sell_ResistItem(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_SELL_PERSONITEM* pRecv = static_cast<GS2C_RECV_SELL_PERSONITEM*>(pData);

	Resist_Item_Money(&pRecv->ItemData, pRecv->MoneyPerItem);

	Notify( eUpdateItem );
}

void PersonStoreContents::Buy_TradeItem(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_BUY_PERSONITEM* pRecv = static_cast<GS2C_RECV_BUY_PERSONITEM*>(pData);

	Item_Buy(&pRecv->buyItemData);
}

// void PersonStoreContents::Set_StoreName(void* pData)
// {
// 	SAFE_POINTER_RET(pData);
// 	GS2C_RECV_STORE_NAME* pRecv = static_cast<GS2C_RECV_STORE_NAME*>(pData);
// 	SetStoreName( pRecv->szShopTitle );
// }
// 
// void PersonStoreContents::Get_StoreName(void* pData)
// {
// 	SAFE_POINTER_RET( pData );
// 	std::wstring* nData = static_cast<std::wstring*>(pData);
// 
// 	*nData = wsStoreName;
// }

//void PersonStoreContents::Get_ValueType(void* pData)
//{
//	SAFE_POINTER_RET( pData );
//	PersonStoreContents::eVALUE_TYPE* nData = static_cast<PersonStoreContents::eVALUE_TYPE*>(pData);
//
//	*nData = m_eValueType;
//}

void PersonStoreContents::OpenPersonStore(void* pData)
{
	SAFE_POINTER_RET( pData );
	CsC_AvObject* pTarget = static_cast< CsC_AvObject* >( pData );

	m_eValueType = VT_OPEN_PERSON;

	// 창 생성
	cBaseWindow* pWindow = g_pGameIF->GetDynamicIF( cBaseWindow::WT_PERSONSTORE );
	SAFE_POINTER_RET( pWindow );

	m_pTarget = pTarget;
	iTargetUniqID = m_pTarget->GetUniqID();

	SendRequestItemList();
}

void PersonStoreContents::OpenCommissionStore(void* pData)
{
	SAFE_POINTER_RET( pData );
	CsC_AvObject* pTarget = static_cast< CsC_AvObject* >( pData );

	m_eValueType = VT_OPEN_EMPLOYMENT;

	// 창 생성
	cBaseWindow* pWindow = g_pGameIF->GetDynamicIF( cBaseWindow::WT_PERSONSTORE );
	SAFE_POINTER_RET( pWindow );

	m_pTarget = pTarget;
	iTargetUniqID = m_pTarget->GetUniqID();

	SendRequestItemList();
}

void PersonStoreContents::RecvClosePersonStore(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_STORE_OPEN_CLOSE_RESULT* pRecv = static_cast< GS2C_RECV_STORE_OPEN_CLOSE_RESULT* >( pData );

	if( pRecv->nResult == 100 )
	{
		cPrintMsg::PrintMsg( 30354 );
		UnlockInventory();
		Notify( eEnableWindow );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PERSONSTORE );
		m_eValueType = VT_CLOSE_STORE;
	}
}

void PersonStoreContents::RecvCloseCommissionStore(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_STORE_OPEN_CLOSE_RESULT* pRecv = static_cast< GS2C_RECV_STORE_OPEN_CLOSE_RESULT* >( pData );

	switch( pRecv->nResult )
	{
	case 99:	// 상점이 이미 폐쇄 진행중.
	case 100:	// 성공	
		SendRequestWithdraw();
		break;
	
	case 101:	// 판매가 완료되어 상점이 닫혔습니다.
	case 102:	// 상점이 만료 되었습니다.
		Notify( eEnableWindow );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PERSONSTORE );
		break;
	
	default:
		assert_cs( false );
	}

	m_eValueType = VT_CLOSE_STORE;

	// 위탁상점 정보 제거
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_RELEASE_DATA, NULL );
}

void PersonStoreContents::RecvStoreReadyResult(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_STORE_READY_RESULT* pRecv = static_cast< GS2C_RECV_STORE_READY_RESULT* >( pData );
	
	switch( pRecv->nResult )
	{
	case 1 :	// 이미 상점이 오픈되어 있는 상태 입니다.
	case 2 :	// 이미 오픈 대기중
		assert_cs( false );		// 들어와서는 안되는 루틴
		break;
	case 4 :	// 근처에 다른 상점이 존재 합니다.
		cPrintMsg::PrintMsg( 30350 );		
		break;
	case 5:		// 아이템 사용 시간이 만료됨
		cPrintMsg::PrintMsg( 30362 );
		break;
	case 6:		// 피로도가 가득참
		cPrintMsg::PrintMsg( 30358 );
		break;
	case 7:		// 조합 상인이 보관 중인 아이템이 있어 오픈할 수 없습니다.
		cPrintMsg::PrintMsg( 30371 );
		break;
	case 8:
		cPrintMsg::PrintMsg( 30378 );
		break;
	case 100 :  // 상점 대기 상태
		{
			m_eValueType = VT_RES_PERSON;
			iUsedItemID = 0;
			
			DelResistItem();

			cBaseWindow* pWindow = g_pGameIF->GetDynamicIF( cBaseWindow::WT_PERSONSTORE );
			SAFE_POINTER_RET( pWindow );		
		}
		return;

	case 101:	//	오픈 상태에서 대기 상태로
		{
			if( m_eValueType == VT_RES_PERSON_START )
			{
				m_eValueType = VT_RES_PERSON;
				cPrintMsg::PrintMsg( 30352 );
			}
			else if( m_eValueType == VT_RES_EMPLOYMENT_START )
			{
				m_eValueType = VT_RES_EMPLOYMENT;
				cPrintMsg::PrintMsg( 30353 );
			}
			Notify( eReady );
		}
		return;

	case 200 :  // 고용상점 대기 상태
		{
			m_eValueType = VT_RES_EMPLOYMENT;
			iUsedItemID = pRecv->nItemType;

			DelResistItem();

			cBaseWindow* pWindow = g_pGameIF->GetDynamicIF( cBaseWindow::WT_PERSONSTORE );
			SAFE_POINTER_RET( pWindow );
		
			GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_SET_MODELID, &iUsedItemID );
		}
		return;

	case 102:	// 상점 대기 상태에서 종료
	case 202:	// 고용상점 대기 상태에서 종료
		{
			UnlockInventory();
			Notify( eEnableWindow );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PERSONSTORE );
			m_eValueType = VT_CLOSE_STORE;
		}
		return;

	default:
		assert_csm1( false, _ERROR_MSG( "nResult = %d" ), pRecv->nResult );
		return;
	}

	// 아이템 언락
	g_pDataMng->GetInven()->ItemUnlock_ItemType( nItem::PersonStore, 1 );
	g_pDataMng->GetInven()->ItemUnlock_ItemType( nItem::PersonStore, 3 );
	UnlockInventory();
}

void PersonStoreContents::RecvOpenPersonStore(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_STORE_OPEN_CLOSE_RESULT* pRecv = static_cast< GS2C_RECV_STORE_OPEN_CLOSE_RESULT* >( pData );

	switch( pRecv->nResult )
	{
	case 0: // 이미 상점 오픈중
	case 1: // 개인 상점 허가증이 없습니다.
		assert_cs( false );
		break;
	case 2: // 개인 상점 허가증 사용시간 만료
	case 3: // 개인 상점 이름 에러
		break;
	case 4: // 판매등록 아이탬 개수 에러
	case 5: // 판매등록 가능한 아이템 개수가 오바 되었습니다.
	case 6: // 판매 불가 아이템 등록 요청
	case 7: // 판매 등록한 개수가 현재 가지고 있는 아이템 개수 보다 많습니다.
	case 8: // 판매금액 등록 오류 너무 작거나 너무 크거나
	case 9: // 아이템 오류 내부 오류 ( 아이템을 못찾았거나 아이템 소스 정보를 못찾거나 )
		assert_cs( false );
		break;
	case 100: // 정상 등록
		{
			m_eValueType = VT_RES_PERSON_START;
			Notify( eOpen );
		}
		break;
	}
}

void PersonStoreContents::RecvOpenCommissionStore(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_STORE_OPEN_CLOSE_RESULT* pRecv = static_cast< GS2C_RECV_STORE_OPEN_CLOSE_RESULT* >( pData );

	// 위탁 상점은 오픈 후 UI 닫히기 때문에 인벤토리 락 풀어줘야 함
	UnlockInventory();

	switch( pRecv->nResult )
	{
	case 10:		// 아이템 시간 만료
		Notify( eEnableWindow );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PERSONSTORE );
		break;
	case 100: // 정상 등록 - 위탁상점은 등록 되는순간 창 닫아 준다.
		Notify( eEnableWindow );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PERSONSTORE );
		break;
	default:
		assert_cs( false );
		break;
	}

	m_eValueType = VT_CLOSE_STORE;
}

void PersonStoreContents::RecvBuyItemResult(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_BUY_RESULT_PERSONITEM* pRecv = static_cast<GS2C_RECV_BUY_RESULT_PERSONITEM*>( pData );

	switch( pRecv->nResult )
	{
	case 0:		// 상대방을 찾을 수 없음
		{
			cPrintMsg::PrintMsg( 30374 );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PERSONSTORE );
			m_eValueType = VT_CLOSE_STORE;
		}
		break;

	case 1:		// 판매 수량 < 구매 수량
		ItemTrade_BuyFail( pRecv->nResult );
		break;

	case 100:	// 정상 구매
		pRecv->nSlotIndex -= 1;		// 동기화
		ItemTrade_BuySuccess( pRecv->nSlotIndex, pRecv->nCount );
		break;

	case 101:	// 위탁 상점 구매 / 판매 둘 다 들어옴
		{
			CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( pRecv->nSlotIndex );
			SAFE_POINTER_RET( pItem );
			CsItem::sINFO* pInfo = pItem->GetInfo();
			SAFE_POINTER_RET( pInfo );
			cPrintMsg::PrintMsg( 30373, pInfo->s_szName, &pRecv->nCount );
		}
		break;
	}
}

void PersonStoreContents::RecvPersonStoreItemList(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_PERSONSTORE_ITEMLIST* pRecv = static_cast< GS2C_RECV_PERSONSTORE_ITEMLIST* >( pData );

	if( pRecv->nResult == 0 )
	{
		cPrintMsg::PrintMsg( 30374 );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PERSONSTORE );
		m_eValueType = VT_CLOSE_STORE;
		return;
	}

	SetStoreName( pRecv->szTitle );

	DelResistItem();

	ContentsStream kTmp;
	kTmp << wsStoreName;
	Notify(eSetStoreName, kTmp);

	int nItemCount = pRecv->nItemCount;
	std::list< nsPersonStore::sSellItemInfo >::iterator it = pRecv->lItemInfo.begin();
	for( ; it != pRecv->lItemInfo.end(); ++it )
	{
		Resist_Item_Money( &it->ItemData, it->nItemPrice );
	}

	Notify( eUpdateItem );
	Notify( eEnableWindow );
}

void PersonStoreContents::RecvCommissionStoreItemList(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_COMMISSIONSTORE_ITEMLIST* pRecv = static_cast< GS2C_RECV_COMMISSIONSTORE_ITEMLIST* >( pData );

	if( pRecv->nResult == 0 )
	{
		cPrintMsg::PrintMsg( 30374 );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PERSONSTORE );
		m_eValueType = VT_CLOSE_STORE;
		return;
	}

	iUsedItemID = pRecv->nUsedItemType;

	SetStoreName( pRecv->szShopName );

	DelResistItem();

	ContentsStream kTmp;
	kTmp << wsStoreName;
	Notify(eSetStoreName, kTmp);

#ifdef EMPLOYMENT_TAMERNAME
	wsTamerName = pRecv->szTamerName;
	ContentsStream kTmp2;
	kTmp2 << wsTamerName;
	Notify(eSetTamerName, kTmp2);
#endif

	if( pRecv->bMyShop == true )
		m_eValueType = VT_RES_EMPLOYMENT_START;

	int nItemCount = pRecv->nItemCount;
	std::list< nsPersonStore::sSellItemInfo >::iterator it = pRecv->lItemInfo.begin();
	for( ; it != pRecv->lItemInfo.end(); ++it )
	{
		Resist_Item_Money( &it->ItemData, it->nItemPrice );
	}
	Notify( eUpdateItem );
	Notify( eOpen );
}

bool PersonStoreContents::SendOpenPersonStore(std::wstring const& wsShopName)
{
	int nCnt = m_vpSellInfo.size();
	if( nCnt == 0 )
		return false;

	nTrade::Item* pArray = csnew nTrade::Item[ nCnt ];
	for( int i = 0; i < nCnt; ++i )
	{
		sSellInfo* pSellInfo = m_vpSellInfo.at( i );
		SAFE_POINTER_RETVAL( pSellInfo, false );

		pArray[ i ].m_nIsSell = 1;
		memcpy( &pArray[ i ].m_sItem, &pSellInfo->ItemData, sizeof( cItemData ) );
		pArray[ i ].m_nPrice = (n8)pSellInfo->uPrice;
	}

	Notify( eDisableWindow );

	if( net::game )
		net::game->SendTamerShopOpen( wsShopName.c_str(), nCnt, pArray );

	delete[] pArray;
	return true;
}

bool PersonStoreContents::SendOpenCommissionStore(std::wstring const& wsShopName)
{
	int nCnt = m_vpSellInfo.size();
	if( nCnt == 0 )
		return false;

	nTrade::Item* pArray = csnew nTrade::Item[ nCnt ];
	int nBuyVecCnt = 0;
	for( int i=0; i<nCnt; ++i )
	{
		sSellInfo* pSellInfo = m_vpSellInfo.at( i );
		SAFE_POINTER_CON( pSellInfo );
		
		cItemData* pItemData = &pSellInfo->ItemData;
		SAFE_POINTER_CON( pItemData );

		pArray[ i+nBuyVecCnt ].m_nIsSell = 1;
		memcpy( &pArray[ i+nBuyVecCnt ].m_sItem, pItemData, sizeof( cItemData ) );
		pArray[ i+nBuyVecCnt ].m_nPrice = (n8)pSellInfo->uPrice;

		cData_Inven* pInven = g_pDataMng->GetInven();
		SAFE_POINTER_CON( pInven );

		pInven->ItemUnlock_ItemID( pItemData->GetType() );

		//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
		int m_nInvenSlotCount = pInven->GetInvenSlotCount();
		uint nRemoveCount = pItemData->GetCount();
		for( int k = 0; k < m_nInvenSlotCount; ++k )
		{
			cItemInfo* pItem = pInven->GetData( k );
			SAFE_POINTER_CON( pItem );

			if( pItemData->GetType() != pItem->m_nType )
				continue;

			if( pItemData->GetEndTime() != pItem->m_nEndTime )
				continue;

			if( pItem->m_nLimited != 0 )
				continue;

			if( pItemData->m_nRate != pItem->m_nRate )
				continue;

			if( cData_Inven::IsTradeLimitedItem( pItem ) )
				continue;

			if( pItem->GetCount() < nRemoveCount )
			{
				nRemoveCount -= pItem->GetCount();
				pItem->Reset();
			}
			else
			{
				pItem->DecreaseCount( nRemoveCount );
				nRemoveCount = 0;
			}

			if( nRemoveCount == 0 )
				break;
		}				
	}

	Notify( eDisableWindow );

	if( net::game )
	{
		nSync::Pos pos;
		CTamerUser* pUser = g_pCharMng->GetTamerUser();
		pos.m_nX = (int)pUser->GetPos2D().x;
		pos.m_nY = (int)pUser->GetPos2D().y;
		net::game->SendCommissionShopOpen( pos, pUser->GetCurRot(), wsShopName.c_str(), 0, nCnt, pArray );
	}

	delete[] pArray;
	return true;
}

void PersonStoreContents::SendRequestItemList()
{
	Notify( eDisableWindow );

	// ItemList Open 요청
	switch(m_eValueType)
	{
	case VT_OPEN_PERSON:
	case VT_RES_PERSON_START:
		if( net::game )
			net::game->SendTamerShopItemList( iTargetUniqID, 0 );
		break;
	case VT_OPEN_EMPLOYMENT:
	case VT_RES_EMPLOYMENT_START:
		if( net::game )
			net::game->SendTamerShopItemList( iTargetUniqID, 1 );
		break;
	default:
		assert_cs(false);
		return;
	}
}

void PersonStoreContents::SendRequestReady(int nReqType, uint nInvenIndex)
{
	Notify( eDisableWindow );

	if( net::game )
		net::game->SendTamerShopReady( nReqType, nInvenIndex );
}

void PersonStoreContents::SendRequestWithdraw()
{
	SAFE_POINTER_RET( g_pDataMng );

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	// 회수하기 전에 걸린 아이템 락 풀어준다
	int nSellSize = m_vpSellInfo.size();
	for( int i = 0; i < nSellSize; ++i )
	{
		sSellInfo* pSellInfo = m_vpSellInfo.at( i );
		SAFE_POINTER_CON( pSellInfo );
		cItemData* pItemData = &pSellInfo->ItemData;
		SAFE_POINTER_CON( pItemData );

		pInven->ItemUnlock_ItemID( pItemData->GetType() );
	}

	SAFE_POINTER_RET( g_pGameIF );

	Notify( eEnableWindow );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PERSONSTORE );
	m_eValueType = VT_CLOSE_STORE;

	if( net::game )
		net::game->SendCommissionShopNpcItemList();
}

void PersonStoreContents::CheckTarget()
{
	SAFE_POINTER_RET( m_pTarget );
	int nTargetIndex = m_pTarget->GetIDX();

	// 개인 상점 열려 있는지 체크
	switch( m_eValueType )
	{
	case PersonStoreContents::VT_OPEN_PERSON:
		{
			if( !CheckDistance() )
			{
				CloseWindowUI();
				return;
			}
			
			CTamer* pTamer = g_pCharMng->GetTamer( nTargetIndex );
			if( pTamer == NULL )
			{
				if( cItemSeparate::IsInstance( cBaseWindow::WT_PERSONSTORE, 0 ) )
					cItemSeparate::DeleteInstance( cBaseWindow::WT_PERSONSTORE, 0 );

				cPrintMsg::PrintMsg( 30354 );
				CloseWindowUI();
				return;
			}

			cCondition* pTamerCondition = pTamer->GetCondition();
			SAFE_POINTER_RET( pTamerCondition );

			if( pTamerCondition->IsCondition( nSync::Shop ) == false )
			{
				if( cItemSeparate::IsInstance( cBaseWindow::WT_PERSONSTORE, 0 ) )
					cItemSeparate::DeleteInstance( cBaseWindow::WT_PERSONSTORE, 0 );

				cPrintMsg::PrintMsg( 30354 );
				CloseWindowUI();
				return;
			}
		}
		break;
	case PersonStoreContents::VT_RES_EMPLOYMENT_START:
	case PersonStoreContents::VT_OPEN_EMPLOYMENT:
		{
			if( !CheckDistance() )
			{
				CloseWindowUI();
				return;
			}

			CEmployment* pEmployment = g_pCharMng->GetEmployment( nTargetIndex );
			if( pEmployment == NULL )
			{
				//2017-05-24-nova 구매창이 떠 있을경우 창이 안닫히는 버그수정
				if( cItemSeparate::IsInstance( cBaseWindow::WT_PERSONSTORE, 0 ) )
					cItemSeparate::DeleteInstance( cBaseWindow::WT_PERSONSTORE, 0 );

				cPrintMsg::PrintMsg( 30354 );
				CloseWindowUI();
				return;
			}
		}
		break;
	}
}

bool PersonStoreContents::CheckTamerState()
{
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	if( pTamer == NULL )
	{
		m_eValueType = VT_CLOSE_STORE;
		return false;
	}

	if( pTamer->IsBattle() )
		return false;

	return true;
}

// 타겟 상점과 거리 체크
bool PersonStoreContents::CheckDistance()
{
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pTamer, false );

	if( ( m_pTarget->GetPos2D() - pTamer->GetPos2D() ).Length() > IF_TARGET_TAMER_MAX_DIST )
		return false;

	return true;
}

bool PersonStoreContents::CheckStoreName()
{
	switch( Language::CheckShopName( wsStoreName ) )
	{
	case Language::STRING_EMPTY:		cPrintMsg::PrintMsg( 30370 );	return false;
	case Language::STRING_SIZE_MIN:
	case Language::STRING_SIZE_MAX:		return false;
	case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10029 );	return false;
	}

	if( nsCsFileTable::g_pCuidMng->CheckID( wsStoreName.c_str() ) == false )
	{
		cPrintMsg::PrintMsg( 10029 );
		return false;
	}

	return true;
}

bool PersonStoreContents::CheckSlotCount()
{
	PERSONSTORE* pPersonStore = nsCsFileTable::g_pBaseMng->GetPersonStore();
	SAFE_POINTER_RETVAL( pPersonStore, false );

	std::map< int, PERSONSTORE::sOBJECT_INFO* >::iterator it = pPersonStore->s_mapObject.find( iUsedItemID );
	PERSONSTORE::sOBJECT_INFO* pObject = NULL;
	if( it != pPersonStore->s_mapObject.end() )
		pObject = it->second;
	SAFE_POINTER_RETVAL( pObject, false );

	// 슬롯 개수 체크 - 슬롯 자리가 없으면
	if( pObject->s_nSlotCount <= m_vpSellInfo.size() )
	{
		cPrintMsg::PrintMsg( 30367 );
		return false;
	}

	return true;
}

void PersonStoreContents::LockInventory()
{
	SAFE_POINTER_RET( g_pDataMng );
	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	int m_nInvenSlotCount = pInven->GetInvenSlotCount();

	for( int i = 0; i < m_nInvenSlotCount; ++i )
	{
		bool bIsLimited = false;

		cItemInfo* pItemInfo = pInven->GetData( i );
		SAFE_POINTER_CON( pItemInfo );

		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() );
		SAFE_POINTER_CON( pFTItem );

		// 퀘스트 아이템인지
		CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
		SAFE_POINTER_CON( pFTInfo );

		if( ( pFTInfo->s_nType_L == nItem::Quest ) || 
			( pFTInfo->s_nType_L == nItem::UseQuest ) )
			bIsLimited = true;

		// 귀속된 아이템인지	
		if( pItemInfo->m_nLimited != 0 )
			bIsLimited = true;

		if( cData_Inven::IsTradeLimitedItem( pItemInfo ) )
			bIsLimited = true;
		
		if( bIsLimited )
			pInven->Itemlock_InvenSlot( TO_INVEN_SID( i ) );
	}
}

void PersonStoreContents::UnlockInventory()
{
	SAFE_POINTER_RET( g_pDataMng );

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	pInven->ItemUnlock_AllItem();
}

bool PersonStoreContents::Resist_Item_Money(cItemData* pItem, u8 uPrice)
{
	SAFE_POINTER_RETVAL( pItem, false );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, false );

	CsBaseMng::sPERSON_STORE* pFTPersonStore = nsCsFileTable::g_pBaseMng->GetPersonStore();
	SAFE_POINTER_RETVAL( pFTPersonStore, false );


	std::map< int, CsBaseMng::sPERSON_STORE::sOBJECT_INFO* >::iterator it = pFTPersonStore->s_mapObject.find( iUsedItemID );
	CsBaseMng::sPERSON_STORE::sOBJECT_INFO* pObject = it->second;
	SAFE_POINTER_RETVAL( pObject, false );

	SAFE_POINTER_RETVAL( g_pDataMng, false );

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );
	
	// 슬롯 갯수 체크
	if( pObject->s_nSlotCount <= m_vpSellInfo.size() )
	{
		cPrintMsg::PrintMsg( 30367 );
		return false;
	}

	// 기존의 아이템에 같은 타입이 있는지 체크
	sSellInfo* pSellInfo = GetSellItemInfo( pItem, uPrice );

	if( pSellInfo == NULL || pItem->m_nEndTime > 0)//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
	{
		pSellInfo = new sSellInfo;

		pSellInfo->ItemData = *pItem;
		pSellInfo->uPrice = uPrice;

		m_vpSellInfo.push_back( pSellInfo );

		//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
		if( pItem->m_nEndTime > 0 ) //기간제 아이템일때
		{
			pInven->ItemLock_ItemID( pItem->GetType() );
		}
	}
	else
	{
		cItemData* pItemData = &pSellInfo->ItemData;
		SAFE_POINTER_RETVAL( pItemData, false );
		int nCurCount = pItemData->GetCount();
		pItemData->SetCount( nCurCount + pItem->GetCount() );
	}

	Notify(eBtnDynamicOK);

	if( m_eValueType == VT_OPEN_PERSON || m_eValueType == VT_OPEN_EMPLOYMENT )
		pInven->ItemUnlock_ItemID( pItem->GetType() );

	return true;
}

void PersonStoreContents::Item_Buy( cItemData* pItem )
{
	SAFE_POINTER_RET( pItem );
	SAFE_POINTER_RET( g_pDataMng );

	sSellInfo* pSellInfo = m_vpSellInfo.at( iSelectIndex );
	SAFE_POINTER_RET( pSellInfo );

	cItemData* pItemData = &pSellInfo->ItemData;
	SAFE_POINTER_RET( pItemData );

	if( pItemData->GetType() != pItem->GetType() )
	{
		assert_cs( false );
		return;
	}

	if( pItemData->m_nRate != pItem->m_nRate )
	{
		assert_cs( false );
		return;
	}

	if( pItemData->GetCount() < pItem->GetCount() )
	{
		assert_cs( false );
		return;
	}

	// 인벤토리 여유공간체크
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	pInven->Backup_Save();
	if( pInven->ItemCrop( pItem ) != 0 )
	{
		cPrintMsg::PrintMsg( 11015 );
		pInven->Backup_Load();
		return;
	}
	pInven->Backup_Load();

	// 금액 체크
	u8 nNeedMoney = pSellInfo->uPrice * pItem->GetCount();
	if( nNeedMoney > pInven->GetMoney().GetInt() )
	{
		cPrintMsg::PrintMsg( 11003 );
		return;
	}

	switch( m_eValueType )
	{
	case PersonStoreContents::VT_OPEN_PERSON:
		net::game->SendTamerShopBuy( iTargetUniqID, iSelectIndex, pItem, static_cast< n8 >( pSellInfo->uPrice ) );
		break;
	case PersonStoreContents::VT_OPEN_EMPLOYMENT:
		net::game->SendCommissionShopBuy( iTargetUniqID, iSelectIndex, pItem, static_cast< n8 >( pSellInfo->uPrice ) );
		break;
	default:
		assert_cs( false );
	}

	Notify(eEnableWindow);
}

void PersonStoreContents::ItemTrade_BuySuccess(int nSlotIndex, int nCount)
{
	sSellInfo* pSellInfo = m_vpSellInfo.at( nSlotIndex );
	SAFE_POINTER_RET( pSellInfo );

	cItemData* pItemData = &pSellInfo->ItemData;
	SAFE_POINTER_RET( pItemData );

	if( 0 == nCount )
		return;

	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	SAFE_POINTER_RET( g_pDataMng );

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	
	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( pItemData->GetType() );
	SAFE_POINTER_RET( pItem );
	
	CsItem::sINFO* pFTItem = pItem->GetInfo();
	SAFE_POINTER_RET( pFTItem );

	bool bUpdateItem = false;
	
	switch( m_eValueType )
	{
	case VT_RES_PERSON_START:// 개인 상점에서 다른 유저가 아이템을 구매 했을 경우
		{
			SellResult( pSellInfo, nCount );

			bUpdateItem = true;

			cPrintMsg::PrintMsg( 30355, pFTItem->s_szName );
		}		
		break;
	case VT_OPEN_PERSON:
	case VT_OPEN_EMPLOYMENT:
		{
			BuyResult( pSellInfo, nCount );	

			bUpdateItem = true;

			// 윈도우 다시 동작 가능
			Notify(eEnableWindow);

			cPrintMsg::PrintMsg( 30356, pFTItem->s_szName );
		}
		break;

	default:
		assert_cs( false );
	}

	// 아이템 갱신
	if( bUpdateItem )
	{
		uint nCurCount = pItemData->GetCount();
		pItemData->SetCount( nCurCount - nCount );

		// 상점에 올린 품목중에 한품목이 모두 팔렸다면 지워주자
		if( pItemData->GetCount() == 0 )
		{
			SAFE_DELETE( m_vpSellInfo.at( nSlotIndex ) );
			m_vpSellInfo.erase( m_vpSellInfo.begin() + nSlotIndex );
			pInven->ItemUnlock_ItemID( pItemData->GetType() );
		}
	}

	Notify( eUpdateItem );
}

void PersonStoreContents::ItemTrade_BuyFail( int nResult )
{
	Notify( eEnableWindow );

	switch( m_eValueType )
	{
	case PersonStoreContents::VT_OPEN_PERSON:
	case PersonStoreContents::VT_OPEN_EMPLOYMENT:
		{
			switch( nResult )
			{
			case 1:
				cPrintMsg::PrintMsg( 30369 );
				break;
			default: assert_cs( false );
			}
		}
		break;
	default:
		assert_cs( false );
	}
}

PersonStoreContents::sSellInfo* PersonStoreContents::GetSellItemInfo( cItemData* pCmpItem, u8 uPrice)
{
	SAFE_POINTER_RETVAL( pCmpItem, NULL );

	// 같은 아이템 있는지 체크
	int nCnt = m_vpSellInfo.size();
	for( int i=0; i<nCnt; ++i )
	{
		sSellInfo* pInfo = m_vpSellInfo.at( i );
		SAFE_POINTER_RETVAL( pInfo, NULL );
		cItemData pItemData = pInfo->ItemData;

		if( ( pItemData.m_nType == pCmpItem->GetType() )&&
			( pItemData.m_nRate == pCmpItem->m_nRate )&&
			( pInfo->uPrice == uPrice ) )
		{
			return pInfo;
		}
	}
	return NULL;
}

u8 PersonStoreContents::_CalTexMoney( u8 nNeedMoney )
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, 0 );

	PERSONSTORE* pPersonStore = nsCsFileTable::g_pBaseMng->GetPersonStore();
	SAFE_POINTER_RETVAL( pPersonStore, 0 );

	switch( m_eValueType )
	{
	case VT_RES_PERSON_START:
		{
			float nTex = pPersonStore->s_BaseInfo.s_fPerson_Charge;
			n8 nCommission = (n8)ceilf( nNeedMoney * nTex );
			return nNeedMoney - nCommission;
		}
	
	case VT_OPEN_PERSON:
		return nNeedMoney;
	
	case VT_OPEN_EMPLOYMENT:
		return nNeedMoney;
	}

	assert_cs( false );
	return 0;
}

void PersonStoreContents::SellResult( sSellInfo* pSellInfo, int nCount )
{
	SAFE_POINTER_RET( pSellInfo );
	SAFE_POINTER_RET( g_pDataMng );

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	u8 nNeedMoney = pSellInfo->uPrice * nCount;

	int nRemoveCount = nCount;
	
	// 돈 증가
	pInven->IncreaseMoney( _CalTexMoney( nNeedMoney ), true );

	// 인벤 아이템 감소
	int m_nInvenSlotCount = pInven->GetInvenSlotCount();

	for( int k = 0; k < m_nInvenSlotCount; ++k )
	{
		cItemInfo* pItem = pInven->GetData( k );
		SAFE_POINTER_CON( pItem );

		cItemData* pItemData = &pSellInfo->ItemData;
		SAFE_POINTER_CON( pItemData );

		if( pItemData->GetType() != pItem->m_nType )
			continue;

		if( pItemData->GetEndTime() != pItem->m_nEndTime )
			continue;

		if( pItemData->m_nRate != pItem->m_nRate )
			continue;

		if( cData_Inven::IsTradeLimitedItem( pItem ) )
			continue;

		if( pItem->GetCount() < nRemoveCount )
		{
			nRemoveCount -= pItem->GetCount();
			pItem->Reset();
		}
		else
		{
			pItem->DecreaseCount( nRemoveCount );
			nRemoveCount = 0;
		}

		if( !pItem->IsEnable() )
			g_pDataMng->ItemUnlock( TO_INVEN_SID( k ) );

		if( nRemoveCount == 0 )
			break;
	}
}

void PersonStoreContents::BuyResult( sSellInfo* pSellInfo, int nCount )
{
	SAFE_POINTER_RET( pSellInfo );
	SAFE_POINTER_RET( g_pDataMng );

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	cItemData* pItemData = &pSellInfo->ItemData;
	SAFE_POINTER_RET( pItemData );

	u8 nNeedMoney = pSellInfo->uPrice * nCount;

	// 돈 감소
	pInven->DecreaseMoney( _CalTexMoney( nNeedMoney ) );
	
	// 인벤 아이템 증가
	pInven->ItemCrop( pItemData, nCount );
}

PersonStoreContents::eVALUE_TYPE PersonStoreContents::GetValueType()
{ 
	return m_eValueType; 
}

int PersonStoreContents::GetOpenSlotCount() const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, 0 );

	PERSONSTORE* pPersonStore = nsCsFileTable::g_pBaseMng->GetPersonStore();
	SAFE_POINTER_RETVAL( pPersonStore, 0 );

	std::map< int, PERSONSTORE::sOBJECT_INFO* >::iterator it = pPersonStore->s_mapObject.find( iUsedItemID );	

	PERSONSTORE::sOBJECT_INFO* pObject = it->second;
	SAFE_POINTER_RETVAL( pObject, 0 );

	return pObject->s_nSlotCount;
}

void PersonStoreContents::SetStoreName(std::wstring wsName)
{
	wsStoreName = wsName.substr( 0, Language::pCountry::eMaxLen::MAX_LenShopName );
	if( wsStoreName.empty() )
		wsStoreName = L"???";
}

int PersonStoreContents::GetSellInfoSize() const
{ 
	return m_vpSellInfo.size();
}

void PersonStoreContents::DelResistItem()
{
	int nSize = m_vpSellInfo.size();
	for( int i = 0; i < nSize; ++i )
		SAFE_DELETE( m_vpSellInfo.at( i ) );
	m_vpSellInfo.clear();
}

void PersonStoreContents::PressDynamicButton()
{
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
		{
			if( !CheckStoreName() )
				return;

			if( !SendOpenPersonStore( wsStoreName ) )
				return;
		}
		break;
	case VT_RES_PERSON_START:
		{
			SendRequestReady( 2 );
		}
		break;
	case VT_RES_EMPLOYMENT:
		{
			if( !CheckStoreName() )
				return;

			if( !SendOpenCommissionStore( wsStoreName ) )
				return;
		}
		break;
	case VT_RES_EMPLOYMENT_START:		// 위탁상점 회수 버튼
		{
			if( net::game )
				net::game->SendCommissionShopClose();
			Notify( eDisableWindow );
		}
		break;
	default:
		assert_cs( false );
	}
}

void PersonStoreContents::CloseWindowUI()
{
	// 상점 상태 체크해서 취소 분기하기
	switch( m_eValueType )
	{
	case VT_OPEN_PERSON:
	case VT_OPEN_EMPLOYMENT:
	case VT_RES_EMPLOYMENT_START:
		UnlockInventory();
		if( cItemSeparate::IsInstance( cBaseWindow::WT_PERSONSTORE, 0 ) )
			cItemSeparate::DeleteInstance( cBaseWindow::WT_PERSONSTORE, 0 );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PERSONSTORE );
		m_eValueType = VT_CLOSE_STORE;
		break;
	case VT_RES_PERSON:
	case VT_RES_PERSON_START:
		ClosePersonStore();
		break;
	case VT_RES_EMPLOYMENT:
		CloseCommissionStore();
		break;
	}
}

void PersonStoreContents::PressItemButton( int nIndex )
{
	SAFE_POINTER_RET( g_pDataMng );

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	switch( m_eValueType )
	{
	case VT_RES_PERSON:
	case VT_RES_EMPLOYMENT:
		{
			////디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
			sSellInfo* pSellInfo = m_vpSellInfo.at( nIndex ); //개인상점 등록 정보
			SAFE_POINTER_RET( pSellInfo );

			cItemData* pItemData = &pSellInfo->ItemData;
			SAFE_POINTER_RET( pItemData );
			
			if( pItemData->GetEndTime() > 0 )//기간제 템이면
			{
				int m_nInvenSlotCount = pInven->GetInvenSlotCount();//인벤 총 슬롯 수

				cItemInfo* pItemInfo;
				for( int i = 0; i < m_nInvenSlotCount; ++i )
				{
					pItemInfo = pInven->GetData( i ); //해당 슬롯의 아이템
					SAFE_POINTER_BEK( pItemInfo );

					if( !pItemInfo->IsEnable() )
						continue;

					if( pItemInfo->m_nType == pItemData->GetType() && 
						pItemInfo->m_nEndTime == pItemData->GetEndTime() ) //등록한 아이템일때
						g_pDataMng->ItemUnlock( TO_INVEN_SID( i ) );
				}
			}

			// 물건 빼주자
			SAFE_DELETE( m_vpSellInfo.at( nIndex ) );
			m_vpSellInfo.erase( m_vpSellInfo.begin() + nIndex );
		}
		break;
	case VT_OPEN_PERSON:
	case VT_OPEN_EMPLOYMENT:
		{
			// 아이템 구매창 띄우기
			iSelectIndex = nIndex;
			
			sSellInfo* pSellInfo = m_vpSellInfo.at( nIndex );
			SAFE_POINTER_RET( pSellInfo );

			cItemData* pItemData = &pSellInfo->ItemData;
			SAFE_POINTER_RET( pItemData );

			cItemInfo itemInfo;
			memcpy( &itemInfo, pItemData, sizeof( cItemInfo ) );
			itemInfo.m_nCount = 1;
			cItemSeparate::NewInstance( cBaseWindow::WT_PERSONSTORE, 0, cItemSeparate::BUY, 
				&itemInfo, pItemData->GetCount(), pSellInfo->uPrice, cMoneyIF::DISP_FULL );
		}
		break;
	default:
		break;
	}

	Notify( eUpdateItem );
}

PersonStoreContents::sSellInfo* PersonStoreContents::GetSellInfo(int nIndex) const
{
	if( m_vpSellInfo.empty() ) return NULL;

	sSellInfo* pSellInfo = m_vpSellInfo.at( nIndex );
	SAFE_POINTER_RETVAL( pSellInfo, NULL );

	return pSellInfo;
}

bool PersonStoreContents::IsRegisterItem() const
{
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
	case VT_RES_PERSON_START:
	case VT_RES_EMPLOYMENT:
		return true;
	default:
		return false;
	}
}

void PersonStoreContents::ClosePersonStoreMsgBox(void* pData)
{
	if( net::game )
	{
		UnlockInventory();
		net::game->SendTamerShopClose();
		Notify( eDisableWindow );
	}
}

void PersonStoreContents::RegisterSeparateItem(int const& nInvenIndex)
{
	// 닫혀 있으면 안 돼
	if( VT_CLOSE_STORE == m_eValueType )
		return;

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	cItemInfo* pItem = pInven->GetData( nInvenIndex );
	SAFE_POINTER_RET( pItem );

	if( m_eValueType == VT_RES_PERSON || m_eValueType == VT_RES_EMPLOYMENT )
	{
		if( g_pDataMng->IsTradeItem( pItem ) )
		{
			int nTotalCnt = pInven->GetCount_Item_ID_Rate( pItem->GetType(), pItem->m_nRate );
			if( nTotalCnt > 0 )
			{
				if( CheckSlotCount() )
					cItemSeparate::NewInstance( cBaseWindow::WT_PERSONSTORE, 0, cItemSeparate::SELL_RES, pItem, nTotalCnt, (u8)0, cMoneyIF::DISP_FULL );
			}
			else
				cPrintMsg::PrintMsg( 30366 );
		}
	}
}

void PersonStoreContents::ClosePersonStore()
{
	switch( m_eValueType )
	{
	case VT_RES_PERSON:
		{
			UnlockInventory();
			SendRequestReady( 3 );
		}
		break;
	
	case VT_RES_PERSON_START:
		cPrintMsg::PrintMsg( 30375 );	// 상점 창 닫을거니? 메시지 박스 오픈
		break;
	
	default:
		assert_csm( false, _T( "잘못된 호출임" ) );
		break;
	}
}

void PersonStoreContents::CloseCommissionStore()
{
	switch( m_eValueType )
	{
	case VT_RES_EMPLOYMENT:
		{
			UnlockInventory();
			SendRequestReady( 5 );
		}
		break;

	default:
		assert_csm( false, _T( "잘못된 호출임" ) );
		break;
	}
}