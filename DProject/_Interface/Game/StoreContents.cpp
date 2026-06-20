#include "StdAfx.h"
#include "StoreContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

int const CStoreContents::IsContentsIdentity(void)
{
	return E_CT_STORE_SYSTEM;
}

CStoreContents::CStoreContents(void):
	m_nNPCType(0),
	m_pInteractNPC(NULL),
	m_pInven(NULL),
	m_fDiscountRate(1.f)
{	
	m_StoreItemDataList.clear();	// 일반 상점 아이템 목록
	m_RepurchaseItemList.clear();	// 재구매 아이템 정보

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::OPEN_STORE_WINDOW,		this, &CStoreContents::OpenStoreWindow );

	if( EventRouter() )
	{
		EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_Inventory_UseItem, this );
	}
}

CStoreContents::~CStoreContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
	InitContentsData();
}

int const CStoreContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CStoreContents::Initialize(void)
{
	return true;
}

void CStoreContents::UnInitialize(void)
{
}

bool CStoreContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void CStoreContents::Update(float const& fElapsedTime)
{
}

void CStoreContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CStoreContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_Inventory_UseItem:
		{
			int nInvenIndex = 0;
			kStream >> nInvenIndex;
			CheckEnableSellItem( nInvenIndex );
		}
		break;
	}
}

void CStoreContents::MakeMainActorData(void)
{
}

void CStoreContents::ClearMainActorData(void)
{
}

void CStoreContents::ClearWorldData(void)
{
	InitContentsData();
}

void CStoreContents::MakeWorldData(void)
{
	InitContentsData();
}

void CStoreContents::InitContentsData(void)
{
	m_nNPCType = 0;				// npc 타입. 상점 타입
	m_pInteractNPC = NULL;		// 상호작용 npc
	m_pInven = NULL;			// 인벤토리
	m_fDiscountRate = 1.f;		// 할인률
	m_nBuyButtonIndex = 0;		// 구매를 위해 누른 버튼의 인덱스

	for(std::list< BaseStoreItemData* >::iterator iter = m_StoreItemDataList.begin(); iter != m_StoreItemDataList.end(); ++iter )
		SAFE_DELETE( *iter );
	for(std::list< BaseStoreItemData* >::iterator iter = m_RepurchaseItemList.begin(); iter != m_RepurchaseItemList.end(); ++iter )
		SAFE_DELETE( *iter );
	
	m_StoreItemDataList.clear();	// 상점 아이템 목록
	m_RepurchaseItemList.clear();	// 재구매 아이템 정보
}


void CStoreContents::SetDiscountRate( int nValue )
{ 
	if( nValue > 100 || nValue < 0 )
	{
		m_fDiscountRate = 1.f;
		assert_csm1(false, _T("NPC Discount Percentage Error : %d"), nValue);	// 할인율이 잘못되었을 때
		return;
	}

	m_fDiscountRate -= (float)nValue / 100.f;		
}

void CStoreContents::SetPassiveDiscount(CDigimonUser* pkMyPartnerMon, DWORD dwDigimonID)
{
	// 1. 현재 파트너몬이 패시브에 등록되었는지 확인합니다.
	CsPassive_Ability_Digimon* pkAbility = nsCsFileTable::g_pDigimonMng->GetPassiveAbility(dwDigimonID, nsCsDigimonTable::PA_NPC_DISCOUNT);
	SAFE_POINTER_RET( pkAbility );
	
	// 2. 진화 가능한 리스트를 순회하면서 슬롯이 오픈되어 있고(레벨도 달성되어야 함) 진화 조건에 부합하면 할인율을 적용합니다.
	CDigimonEvolveInfo* pFTEvolInfo = pkMyPartnerMon->GetFTEvol();	// 파트너몬의 진화체 정보
	CDigimonEvolveObj::MAP mapEvol = pFTEvolInfo->m_Map;

	CDigimonEvolveObj::MAP_IT iter = mapEvol.begin();	// 성장기부터 진화 정보를 모두 검사
	CDigimonEvolveObj::MAP_IT iterEnd = mapEvol.end();

	for( ; iter != iterEnd ; ++iter)
	{
		const CDigimonEvolveObj* pkEvolObj = (iter->second);

		// 순회 중인 슬롯 번호 >= 조건 슬롯
		if( pkEvolObj->m_nEvoSlot >= pkAbility->GetInfo()->s_shEvoNum )
		{
			// 오픈 == 레벨, 에보류터 모두 일치 시
			if( pkMyPartnerMon->GetEvoValue( pkEvolObj->m_nEvoSlot, CDigimonUser::eActive ) )
			{
				SetDiscountRate( pkAbility->GetInfo()->s_nValue1 );
				return;
			}
		}
	}
}

int CStoreContents::GetEnableBuyCnt( int nItemID )		
{
	int nMoneyCnt = 0;
	int nTempMoneyCnt = 0;

	CsItem::sINFO* pFTBuy = nsCsFileTable::g_pItemMng->GetItem( nItemID )->GetInfo();
	SAFE_POINTER_RETVAL(pFTBuy, 0);

	switch( m_nNPCType )
	{
	case nsCsNpcTable::NT_TRADE:
	case nsCsNpcTable::NT_GUILD:
		{
			u8 tempPrice = static_cast<u8>(pFTBuy->s_dwScanPrice);
			tempPrice = static_cast<u8>(tempPrice * GetDiscountRate());
			nMoneyCnt =	(int)( m_pInven->GetMoney().GetInt() / tempPrice );
		}break;
	case nsCsNpcTable::NT_DIGICORE:
		{
			nMoneyCnt = m_pInven->GetCount_Item_TypeLS( DIGICORE_ITEM_TYPE_L, 0 ) / (int)pFTBuy->s_dwDigiCorePrice;
		}break;
	case nsCsNpcTable::NT_EVENT_STORE:
		{
			nMoneyCnt = m_pInven->GetCount_Item_ID( pFTBuy->s_nEventItemType ) / (int)pFTBuy->s_dwEventItemPrice;
		}break;
	case nsCsNpcTable::NT_PROPERTY_STORE:
		{
			CsItemExchange::sINFO* pFTExchangeBuy = nsCsFileTable::g_pItemMng->GetExchangeItem( m_pInteractNPC->GetFTID(), nItemID )->GetInfo();
			SAFE_POINTER_RETVAL(pFTExchangeBuy, 0);

			nMoneyCnt = 32000;
			for( int i=0; i<4; ++i )
			{
				switch(i)
				{
				case 0:
					if( pFTExchangeBuy->s_dwPropertyA_Price == 0)
						break;

					nTempMoneyCnt = m_pInven->GetCount_Item_ID( pFTExchangeBuy->s_dwExchange_Code_A ) / (int)pFTExchangeBuy->s_dwPropertyA_Price;
					nMoneyCnt = CsMin(nMoneyCnt, nTempMoneyCnt);
					break;
				case 1:
					if( pFTExchangeBuy->s_dwPropertyB_Price == 0)
						break;

					nTempMoneyCnt = m_pInven->GetCount_Item_ID( pFTExchangeBuy->s_dwExchange_Code_B ) / (int)pFTExchangeBuy->s_dwPropertyB_Price;
					nMoneyCnt = CsMin(nMoneyCnt, nTempMoneyCnt);
					break;
				case 2:
					if( pFTExchangeBuy->s_dwPropertyC_Price == 0)
						break;

					nTempMoneyCnt = m_pInven->GetCount_Item_ID( pFTExchangeBuy->s_dwExchange_Code_C ) / (int)pFTExchangeBuy->s_dwPropertyC_Price;
					nMoneyCnt = CsMin(nMoneyCnt, nTempMoneyCnt);
					break;
				case 3:
					if( pFTExchangeBuy->s_dwPropertyD_Price == 0)
						break;

					nTempMoneyCnt = m_pInven->GetCount_Item_ID( pFTExchangeBuy->s_dwExchange_Code_D ) / (int)pFTExchangeBuy->s_dwPropertyD_Price;
					nMoneyCnt = CsMin(nMoneyCnt, nTempMoneyCnt);
					break;
				}
			}
		}break;

	default:
		assert_cs( false );
		break;
	}
	return nMoneyCnt;
}

void CStoreContents::SendStoreItemBuy( void* pData) // 서버 보내기 전
{
	cItemData* pItemData = static_cast<cItemData*>(pData);
	SAFE_POINTER_RET( pItemData );

	int nBuyCount = pItemData->GetCount();
	int nItemID = pItemData->GetType();
	DWORD dwNpcID = m_pInteractNPC->GetFTID();

	if( m_nNPCType != nsCsNpcTable::NT_PROPERTY_STORE )// 일반 상점일때
	{
		CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem(nItemID)->GetInfo();
		SAFE_POINTER_RET( pFTItem );
		if( GetEnableBuyCnt(pFTItem->s_dwItemID) < nBuyCount ) // 구매 불가능 할때
		{
			std::list< CStoreContents::BaseStoreItemData* >::iterator iter;
			std::list< CStoreContents::BaseStoreItemData* >::iterator enditer = m_StoreItemDataList.end();
			for(iter = m_StoreItemDataList.begin(); iter != enditer; iter++)
			{
				BaseStoreItemData* pitem = *iter;
				if(pitem->m_nItemID == nItemID)
				{
					TCHAR sz[ 32 ] = {_T("")};
					switch(m_nNPCType)
					{
					case nsCsNpcTable::NT_DIGICORE:
						{
							DigicoreStoreItemData* pDigicoreData = static_cast<DigicoreStoreItemData*>(pitem);
							SAFE_POINTER_RET(pDigicoreData);
							_stprintf_s( sz, 32, pDigicoreData->m_strCurrencyName.c_str() );
							break;
						}
					case nsCsNpcTable::NT_EVENT_STORE:
						{
							EventStoreItemData* pEventItemData = static_cast<EventStoreItemData*>(pitem);
							SAFE_POINTER_RET(pEventItemData);
							_stprintf_s( sz, 32, pEventItemData->m_strEventItemName.c_str() );
							break;
						}
					}
					cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_SHORTAGE, sz);
					break;
				}
			}
			Notify( eEnable_Window );
			return;
		}

		if( net::game )
			net::game->SendBuy( dwNpcID, m_nBuyButtonIndex, nBuyCount );
	}
	else if( m_nNPCType == nsCsNpcTable::NT_PROPERTY_STORE )// 속성 상점일때
	{
		// 물물교환 상점 판매분류가 두개 이상일 때 첫번째 tab에 있는 메뉴의 아이템만(툴팁, 구매등..) 나타나는 현상이 있어 수정	15.01.13  lks007
		CsItemExchange::sINFO* pFTExItem = nsCsFileTable::g_pItemMng->GetExchangeItem( dwNpcID, nItemID )->GetInfo();
		SAFE_POINTER_RET( pFTExItem );

		if( GetEnableBuyCnt( pFTExItem->s_dwItemID ) < nBuyCount ) // 구매 불가능 할때
		{
			if(nsCsFileTable::g_pItemMng->GetItem(pFTExItem->s_dwExchange_Code_A)->GetInfo())
				cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_SHORTAGE, nsCsFileTable::g_pItemMng->GetItem(pFTExItem->s_dwExchange_Code_A)->GetInfo()->s_szName );
			else
				cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );

			Notify( eEnable_Window );
			return;
		}

		if( net::game )
			net::game->SendPropertyBuy( dwNpcID, m_nBuyButtonIndex, nBuyCount, pFTExItem->s_dwItemIndex );
	}
}

bool CStoreContents::IsEnableSellItem( int nInvenIndex )
{
	cItemData* pItem = m_pInven->GetData( nInvenIndex );
	SAFE_POINTER_RETVAL(pItem,false);

	DWORD dwItemFTID = pItem->GetType();
	CsItem::sINFO* pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( dwItemFTID )->GetInfo();
	SAFE_POINTER_RETVAL(pFTItemInfo, false);

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	if( cData_Inven::IsTradeLimitedItem( pItem ) )
	{
		cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_LIMIT_SALE );
		return false;
	}
#endif
	//================================================================================================
	// 팔수 있는 아이템인지 체크
	//================================================================================================
	// 가격이 0원짜리는 팔수 없다
	if( pFTItemInfo->s_dwSale == 0 )
	{
		cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_LIMIT_SALE );
		return false;
	}	
	switch( pFTItemInfo->s_nType_L )
	{
		// 디지타마류는 팔수 없다.
	case nItem::Digitama1:
	case nItem::Digitama2:
	case nItem::Digitama3:
	case nItem::Digitama4:
	case nItem::Digitama5:
	case nItem::Digitama6:
		cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_LIMIT_SALE );
		return false;
	}
	return true;
}


void CStoreContents::SetRepurchaseItemInfo( void* pData )
{
	SAFE_POINTER_RET( pData );
	ST_RECV_ItemRepurchaseOpenInfo* RepurchaseInfo = static_cast<ST_RECV_ItemRepurchaseOpenInfo*>(pData);

	u4	nItemCount   = RepurchaseInfo->m_nItemCount;
	std::list< cItemData >	ItemDataList = RepurchaseInfo->m_ItemDataList;
	std::list< cItemData >::iterator iter	  = ItemDataList.begin();
	std::list< cItemData >::iterator enditer  = ItemDataList.end();

	for( ; iter != enditer; ++iter)
	{
		// 재구매 아이템 목록에 삽입
		m_RepurchaseItemList.push_back( MakeRepurchaseItemdata( *iter ) );
	}
	//데이터 수신중 메세지 
	cMessageBox::DelMsg( cPrintMsg::LOADING_SERVER_INFO, false );
}

void CStoreContents::CompleteItemRepurchase( void* pData )
{
	SAFE_POINTER_RET(pData);
	ST_RECV_ItemRepurchaseSuccessInfo RepurchaseInfo = *static_cast<ST_RECV_ItemRepurchaseSuccessInfo*>(pData);

	std::list<ST_RECV_ItemRepurchaseSuccessInfo::sInvenInfo>::iterator iter		= RepurchaseInfo.m_InvenInfoList.begin();
	std::list<ST_RECV_ItemRepurchaseSuccessInfo::sInvenInfo>::iterator enditer  = RepurchaseInfo.m_InvenInfoList.end();

	std::map< int, int > mapAcquireItem;
	std::map< int, int > mapConsumeItem;
	for( ; iter != enditer; ++iter )
	{
		cItemData OldItemData = *m_pInven->GetData( iter->m_nInvenSlotNumber );
		cItemData newItemData;
		newItemData.Set( iter->m_nItemTypeID, iter->m_nInvenItemCount );
		newItemData.m_nRate	   = iter->m_nRate;
		newItemData.m_nLimited = iter->m_nLimited;
		newItemData.m_nRemainTradeLimitTime = OldItemData.m_nRemainTradeLimitTime;	

		newItemData.m_nLevel = iter->m_nLevel;
		newItemData.m_nEndTime  = iter->m_nEndTime;

		for(int i=0; i<nLimit::MAX_ACCESSORY_OPTION; ++i)
		{
			newItemData.m_nAccOption[i] = iter->m_nAccOption[i];
			newItemData.m_nAccValues[i] = iter->m_nAccValues[i];
		}

		CalcChangeItemInfo( mapAcquireItem, mapConsumeItem, OldItemData, newItemData );

		// 인벤 데이터 세팅
		m_pInven->SetInvenItemData( iter->m_nInvenSlotNumber, newItemData ); 
	}

	int nBuyItemTypeID(0);
	int nBuyCount(0);
	std::map< int, int >::iterator AcquireIter = mapAcquireItem.begin(); // 현재 구매를 통해서는 한번에 한종류의 아이템만 얻을 수 있다
	if( AcquireIter != mapAcquireItem.end() )
	{
		nBuyItemTypeID	= AcquireIter->first;
		nBuyCount		= AcquireIter->second;
	}

	SAFE_POINTER_RET(m_pInven)
	m_pInven->SetMoney( RepurchaseInfo.m_CurrentMoney, false );

	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( nBuyItemTypeID );
	SAFE_POINTER_RET(pItem);
	CsItem::sINFO* pFTItem = pItem->GetInfo();
	SAFE_POINTER_RET( pFTItem );
	// 구매 메세지
 	sMONEY money( (u8)( pFTItem->s_dwSale * nBuyCount ) );
 	cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_BUY, pFTItem->s_szName, &money );

	// 재구매 데이터에서 정보 삭제
	DeleteRepurchaseItemData( nBuyItemTypeID, nBuyCount );

	//목록 UI 갱신
	ContentsStream kStream;
	kStream << nBuyItemTypeID << nBuyCount;
	Notify( eDelete_Repurchase, kStream );

	//데이터 수신중 메세지 닫기
	cMessageBox::DelMsg( cPrintMsg::LOADING_SERVER_INFO, false );
}

void CStoreContents::CompleteItemBuy( ST_RECV_ItemBuySuccessInfo& BuyInfo )
{
	std::list<ST_RECV_ItemBuySuccessInfo::sInvenInfo>::iterator iter = BuyInfo.m_InvenInfoList.begin();
	std::list<ST_RECV_ItemBuySuccessInfo::sInvenInfo>::iterator enditer = BuyInfo.m_InvenInfoList.end();

	std::map< int, int > mapAcquireItem;
	std::map< int, int > mapConsumeItem;
	for( ; iter != enditer; ++iter )
	{
		cItemData OldItemData = *m_pInven->GetData( iter->m_nInvenSlotNumber );
		cItemData newItemData;
		newItemData.Set( iter->m_nItemTypeID, iter->m_nInvenItemCount );
		newItemData.m_nRate	   = iter->m_nRate;
		newItemData.m_nLimited = iter->m_nLimited;
		newItemData.m_nRemainTradeLimitTime = OldItemData.m_nRemainTradeLimitTime;		

		//2017-06-07-nova
		CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( iter->m_nItemTypeID )->GetInfo();

		if(pSrcFT != NULL)
		{
			if(pSrcFT->s_nBelonging == 1)// 귀속템인지 여부
				newItemData.m_nEndTime = 0xFFFFFFFF;
			else if( cItemInfo::IsUseItemTime( pSrcFT ) )// 시잔제 아이템인 경우
				newItemData.m_nEndTime = _TIME_TS + pSrcFT->s_nUseTime_Min*60;
			else // 기간제 아이템이 아닌 경우
				newItemData.m_nEndTime = 0xFFFFFFFF;
		}

		CalcChangeItemInfo( mapAcquireItem, mapConsumeItem, OldItemData, newItemData );
		
		// 인벤 데이터 세팅
		m_pInven->SetInvenItemData( iter->m_nInvenSlotNumber, newItemData ); 
	}
		
	int nBuyItemTypeID(0);
	int nBuyCount(0);
	std::map< int, int >::iterator AcquireIter = mapAcquireItem.begin(); // 현재 구매를 통해서는 한번에 한종류의 아이템만 얻을 수 있다
	if( AcquireIter != mapAcquireItem.end() )
	{
		nBuyItemTypeID	= AcquireIter->first;
		nBuyCount		= AcquireIter->second;
	}

	// 머니 설정
	SAFE_POINTER_RET(m_pInven)
	m_pInven->SetMoney( BuyInfo.m_CurrentMoney,false );

	switch( m_nNPCType )
	{
	case nsCsNpcTable::NT_TRADE:
	case nsCsNpcTable::NT_GUILD:
		{
			BaseStoreItemData* pBuyItemData = GetStoreItemDataByID( nBuyItemTypeID );
			if( pBuyItemData )
			{
				NormalStoreItemData* pNormal = static_cast<NormalStoreItemData*>(pBuyItemData);
				u8 dc_Price = static_cast<u8>( pNormal->m_nScanPrice * GetDiscountRate() );
				// 돈감소
				sMONEY money( (u8)( dc_Price * nBuyCount ) );
				cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_BUY, const_cast<wchar_t*>(pBuyItemData->m_strName.c_str()), &money );
			}
		}		
		break;
	case nsCsNpcTable::NT_DIGICORE:
	case nsCsNpcTable::NT_EVENT_STORE:
	case nsCsNpcTable::NT_PROPERTY_STORE:
		{
			BaseStoreItemData* pBuyItemData = GetStoreItemDataByID( nBuyItemTypeID );
			if( pBuyItemData )
			{
				for( std::map< int, int >::iterator iter = mapConsumeItem.begin(); iter != mapConsumeItem.end(); ++iter )
				{
					int nConsumeItemID	  = iter->first;
					int nConsumeItemCount = iter->second;

					CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( nConsumeItemID );
					SAFE_POINTER_RET(pItem);
					CsItem::sINFO* pItemInfo = pItem->GetInfo();
					SAFE_POINTER_RET(pItemInfo);
					cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_BUY_BYITEM, pItemInfo->s_szName, &nConsumeItemCount, const_cast<wchar_t*>(pBuyItemData->m_strName.c_str()) );
					
				}
			}
		}break;
	}	

	cWindow::PlaySound( cWindow::SD_Np2 );

	Notify( eEnable_Window );
}

void CStoreContents::CompleteItemSell( ST_RECV_ItemSellSuccessInfo& SellInfo )
{
	SAFE_POINTER_RET(m_pInven);

	std::list<ST_RECV_ItemSellSuccessInfo::sInvenInfo>::iterator iter = SellInfo.m_InvenInfoList.begin();
	std::list<ST_RECV_ItemSellSuccessInfo::sInvenInfo>::iterator enditer = SellInfo.m_InvenInfoList.end();

	std::map< int, int > mapAcquireItem;
	std::map< int, int > mapConsumeItem;

	int nSellInvenSlotIndex( 0 );
	for( ; iter != enditer; ++iter )
	{
		nSellInvenSlotIndex = iter->m_nInvenSlotNumber;

		cItemData OldItemData = *m_pInven->GetData( iter->m_nInvenSlotNumber );
		cItemData newItemData;
		newItemData.Set( iter->m_nItemTypeID, iter->m_nInvenItemCount );
		newItemData.m_nRemainTradeLimitTime = OldItemData.m_nRemainTradeLimitTime;

		CalcChangeItemInfo( mapAcquireItem, mapConsumeItem, OldItemData, newItemData );

		// 인벤 데이터 세팅
		m_pInven->SetInvenItemData( iter->m_nInvenSlotNumber, newItemData ); 
	}

	int nConsumeItemID(0);
	int nConsumeItemCount(0);
	for( std::map< int, int >::iterator miter = mapConsumeItem.begin(); miter != mapConsumeItem.end(); ++miter)
	{
		nConsumeItemID	  = miter->first;
		nConsumeItemCount = miter->second;
	}

	g_pDataMng->ItemUnlock( TO_INVEN_SID( nSellInvenSlotIndex ) );

	m_pInven->SetMoney( SellInfo.m_CurrentMoney, false );

#ifdef REPURCHASE
	AddRepurchaseItemdata( nConsumeItemID, abs(nConsumeItemCount) );
#endif

	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( nConsumeItemID );
	SAFE_POINTER_RET(pItem);
	CsItem::sINFO* pFTItemInfo = pItem->GetInfo();
	SAFE_POINTER_RET(pFTItemInfo);

	sMONEY money( pFTItemInfo->s_dwSale * abs(nConsumeItemCount) );	
	cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_SELL, pFTItemInfo->s_szName, &money ); // 판매 메시지

	cWindow::PlaySound( cWindow::SD_Np4 );

	// 퀘스트 체크
	g_pDataMng->GetQuest()->CalProcess();
	// 퀵슬롯 체크
	cData_QuickSlot::CheckItemCount_AllUser();

	Notify( eEnable_Window );
}

void CStoreContents::CloseStoreWindow()
{
	SAFE_POINTER_RET( m_pInteractNPC );
	// 연결된 NPC 닫힘애니실행

	( (CNpc*)m_pInteractNPC )->SetCloseAnimation();

	InitContentsData();
}

void CStoreContents::OpenStoreWindow( void* pData )
{
	SAFE_POINTER_RET( pData );

	SetInventory();
	// 인벤토리를 연다
	OpenInventory();

	std::pair<CsC_AvObject*, int> pairStoreOpenInfo = *( static_cast<std::pair<CsC_AvObject*, int>*>(pData) );
	SetInteractNPC( pairStoreOpenInfo.first );
	SetInteractNPCType( pairStoreOpenInfo.second );
	if( SetStoreInfo( pairStoreOpenInfo.first ) == false )
		return;

	if( !CreateStoreWindow() )
	{
		InitContentsData(); // 초기화
		return;
	}

	ApplyMyDigimonDiscount(); // 할인률 적용

	RequestRepurchaseOpen();  // 재구매 정보 요청

	Notify(eSet_StoreItemList);

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SEND_STORE_ITEMBUY,		this, &CStoreContents::SendStoreItemBuy );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_STORE_BUY_SUCCESS, this, &CStoreContents::RecvStoreItemBuyInfo );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SEND_STORE_ITEMSELL,	 this, &CStoreContents::SendStoreItemSell );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_STORE_SELL_SUCCESS, this, &CStoreContents::RecvStoreItemSellInfo );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::STORE_APPLY_DISCOUNT, this, &CStoreContents::ApplyDiscountRate ); // 할인률 적용

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::CHANGE_HAVE_MONEY, this, &CStoreContents::ChangeMyMoney ); // 소지금 변경
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::CHANGE_ITEM_COUNT, this, &CStoreContents::ChangeMyItem ); // 소지 아이템 변경
}

void CStoreContents::SetInteractNPC( CsC_AvObject* pNPC )
{
	m_pInteractNPC = pNPC;
}

CsC_AvObject* CStoreContents::GetInteractNPC() const
{
	return m_pInteractNPC;
}

void CStoreContents::SetInteractNPCType( const int nType )
{
	m_nNPCType = nType;
}

int CStoreContents::GetInteractNPCType() const
{
	return m_nNPCType;
}

bool CStoreContents::SetStoreInfo( CsC_AvObject* pInteractNPC )
{
	SAFE_POINTER_RETVAL(pInteractNPC, false );
	CNpc* pNpc = dynamic_cast<CNpc*>( pInteractNPC );
	SAFE_POINTER_RETVAL( pNpc, false );
	CsNpc* pFTNpc = pNpc->GetFTNpc();
	SAFE_POINTER_RETVAL( pFTNpc, false );
	sNPC_TYPE_BASE* pBase = pFTNpc->GetType();
	SAFE_POINTER_RETVAL( pBase, false )

	std::list< DWORD > tempItemIndexList;
	if( m_nNPCType == nsCsNpcTable::NT_PROPERTY_STORE) // 속성 상점일때
		tempItemIndexList = dynamic_cast<sNPC_TYPE_EXCHANGE*>(pBase)->s_list_Item;
	
	else // 일반 상점일때
		tempItemIndexList = dynamic_cast<sNPC_TYPE_TRADE*>(pBase)->s_list_Item;

	std::list< DWORD >::const_iterator iter = tempItemIndexList.begin();
	std::list< DWORD >::const_iterator eiter = tempItemIndexList.end();
	std::list< BaseStoreItemData* > tempItemDdataList;
	for( ; iter!=eiter; ++iter )
	{
		const CsItem::sINFO* pInfo = nsCsFileTable::g_pItemMng->GetItem( *iter )->GetInfo();
		SAFE_POINTER_RETVAL(pInfo, false);

		BaseStoreItemData* pData(NULL);
		switch( m_nNPCType )
		{
			case nsCsNpcTable::NT_TRADE:
			case nsCsNpcTable::NT_GUILD:
				{
					pData = new NormalStoreItemData;
					NormalStoreItemData* pNormalData = static_cast<NormalStoreItemData*>(pData);
					SAFE_POINTER_RETVAL(pNormalData, false);

					pNormalData->m_nScanPrice = pInfo->s_dwScanPrice;
				}break;
			case nsCsNpcTable::NT_DIGICORE:
				{
					pData = new DigicoreStoreItemData;
					DigicoreStoreItemData* pDigicoreData = static_cast<DigicoreStoreItemData*>(pData);
					SAFE_POINTER_RETVAL(pDigicoreData, false);
					
					pDigicoreData->m_nDigiCorePrice = pInfo->s_dwDigiCorePrice;
					pDigicoreData->m_nDispID		= nsCsFileTable::g_pItemMng->TypeLS_to_Disp( DIGICORE_ITEM_TYPE_L, 0 );
					
					const CsItem::sINFO* pCurrencyItem = nsCsFileTable::g_pItemMng->GetItem( pDigicoreData->m_nDispID )->GetInfo();
					SAFE_POINTER_RETVAL(pCurrencyItem, false);
					pDigicoreData->m_strCurrencyName.assign(pCurrencyItem->s_szName);
				}break;
			case nsCsNpcTable::NT_EVENT_STORE:
				{
					pData = new EventStoreItemData;
					EventStoreItemData* pEventData = static_cast<EventStoreItemData*>(pData);
					SAFE_POINTER_RETVAL(pEventData, false);

					pEventData->m_nEventItemType	= pInfo->s_nEventItemType;
					pEventData->m_nEventItemPrice	= pInfo->s_dwEventItemPrice;

					const CsItem::sINFO* pEventItem = nsCsFileTable::g_pItemMng->GetItem( pEventData->m_nEventItemType )->GetInfo();
					SAFE_POINTER_RETVAL(pEventItem, false);
					pEventData->m_strEventItemName.assign(pEventItem->s_szName);
				}break;
			case nsCsNpcTable::NT_PROPERTY_STORE:
				{
					pData = new PropertyStoreItemData;
					PropertyStoreItemData* pPropertyData = static_cast<PropertyStoreItemData*>(pData);
					SAFE_POINTER_RETVAL(pPropertyData, false);

					CsItemExchange::sINFO* pExchangeInfo = nsCsFileTable::g_pItemMng->GetExchangeItem( pInteractNPC->GetFTID(), *iter )->GetInfo();
					SAFE_POINTER_RETVAL(pExchangeInfo, false);

					pPropertyData->SetNeedItemInfo( pExchangeInfo->s_dwExchange_Code_A, pExchangeInfo->s_dwPropertyA_Price );
					pPropertyData->SetNeedItemInfo( pExchangeInfo->s_dwExchange_Code_B, pExchangeInfo->s_dwPropertyB_Price );
					pPropertyData->SetNeedItemInfo( pExchangeInfo->s_dwExchange_Code_C, pExchangeInfo->s_dwPropertyC_Price );
					pPropertyData->SetNeedItemInfo( pExchangeInfo->s_dwExchange_Code_D, pExchangeInfo->s_dwPropertyD_Price );
				}break;
		}

		pData->m_nItemID		= *iter;
		pData->m_nItemShowCount = 1;
		pData->m_strName.assign(pInfo->s_szName);
		pData->m_nSellType		= pInfo->s_nSellType;
		pData->m_nSale			= pInfo->s_dwSale;
		pData->m_nClass			= pInfo->s_nClass;

		tempItemDdataList.push_back( pData );
	}

	// 데이터 세팅
	SetStoreItemDataList( tempItemDdataList );
	return true;
}

void CStoreContents::AddRepurchaseItemdata( int nID, int nCount )
{
	if( m_RepurchaseItemList.size() >= nLimit::MAX_PURCHASE )		//최대 사이즈 넘어갔으면
	{
		CStoreContents::BaseStoreItemData* pOldRepurchaseData = *m_RepurchaseItemList.begin();
		SAFE_POINTER_RET(pOldRepurchaseData);

		int nOldItemID = pOldRepurchaseData->m_nItemID;
		int nOldItemCount = pOldRepurchaseData->m_nItemShowCount;
		DeleteRepurchaseItemData( pOldRepurchaseData->m_nItemID, pOldRepurchaseData->m_nItemShowCount );

		//목록 UI 갱신
		ContentsStream kStream;
		kStream << nOldItemID << nOldItemCount;
		Notify( eDelete_Repurchase, kStream );
	}
	
	//아이템 넣어주자
	cItemData RepurchaseData;
	RepurchaseData.Set(nID, nCount);
	m_RepurchaseItemList.push_back( MakeRepurchaseItemdata( RepurchaseData ) );

	ContentsStream kStream;
	kStream << nID << nCount;
	Notify( eAdd_Repurchase, kStream );
}

int CStoreContents::GetRepurchaseItemCount() const
{
	return m_RepurchaseItemList.size();
}

CStoreContents::BaseStoreItemData* CStoreContents::GetRepurchaseItemData(int nItemID, int nItemCount)
{
	std::list< BaseStoreItemData* >::const_iterator iter	= m_RepurchaseItemList.begin();
	std::list< BaseStoreItemData* >::const_iterator endIter = m_RepurchaseItemList.end();

	for( ; iter != endIter; ++iter )
	{
		if( (*iter)->m_nItemID == nItemID && (*iter)->m_nItemShowCount == nItemCount )
			return (*iter);
	}
	return NULL;
}

int CStoreContents::GetRepurchaseItemIndex(int nItemID, int nItemCount)
{
	std::list< BaseStoreItemData* >::const_iterator iter	= m_RepurchaseItemList.begin();
	std::list< BaseStoreItemData* >::const_iterator endIter = m_RepurchaseItemList.end();

	for( int nIndex = 0; iter != endIter; ++iter, ++nIndex )
	{
		if( (*iter)->m_nItemID == nItemID && (*iter)->m_nItemShowCount == nItemCount )
			return nIndex;
	}
	return -1;
}

float CStoreContents::GetDiscountRate() const
{
	return m_fDiscountRate;
}

void CStoreContents::RecvStoreItemBuyInfo( void* pData )
{
	SAFE_POINTER_RET( pData );
	ST_RECV_ItemBuySuccessInfo& BuyInfo = *static_cast<ST_RECV_ItemBuySuccessInfo*>(pData);
	
	SAFE_POINTER_RET( g_pGameIF );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_STORE ) )
		CompleteItemBuy( BuyInfo );

	if(net::game)
		net::game->SendQueryMoneyInven();

	Notify( eUpdate_Currency );
}

// 서버에 판매패킷 보내기전
void CStoreContents::SendStoreItemSell( void* pData ) 
{
	SAFE_POINTER_RET( pData );
	std::pair< int, int >* pairSellInfo = static_cast< std::pair< int, int >* >( pData );
	int nInvenIndex = pairSellInfo->first;
	int nSellCount  = pairSellInfo->second;

	cBaseWindow* pStore = g_pGameIF->GetDynamicIF( cBaseWindow::WT_STORE, 0, false, false );
	SAFE_POINTER_RET(pStore);
	if( pStore->IsEnableWindow() == false )
		return;

	assert_cs( m_pInteractNPC->GetLeafRTTI() == RTTI_NPC );
	g_pDataMng->ItemLock( TO_INVEN_SID( nInvenIndex ) );

	if( net::game )
		net::game->SendSell( m_pInteractNPC->GetFTID(), nInvenIndex, nSellCount );

	Notify( eEnable_Window );
}

// 서버 응답 받은 후 호출
void CStoreContents::RecvStoreItemSellInfo( void* pData )
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( g_pGameIF );

	ST_RECV_ItemSellSuccessInfo SellInfo = *static_cast<ST_RECV_ItemSellSuccessInfo*>(pData);

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_STORE ) )
		CompleteItemSell( SellInfo );

	if( net::game )
		net::game->SendQueryMoneyInven(); // 확인용
	
	Notify( eUpdate_Currency );
}

void CStoreContents::ApplyDiscountRate( void* pData )
{
	SAFE_POINTER_RET(pData);
	CDigimonUser* pDigimonUser = static_cast<CDigimonUser*>(pData);

	SAFE_POINTER_RET( g_pGameIF );
	if( !g_pGameIF->IsActiveWindow(cBaseWindow::WT_STORE) )
		return;

	SetDiscountRate( 0 ); // 할인율 초기화

	// 열려있는 상점이 비트 상점이면 true
	if( m_nNPCType == nsCsNpcTable::NT_TRADE ||	m_nNPCType == nsCsNpcTable::NT_GUILD )
	{
		SetPassiveDiscount(pDigimonUser, pDigimonUser->GetBaseDigimonFTID() );	// 할인율 적용
		Notify( eSet_StoreItemList );

		// 구매 창이 오픈되어 있을 때 구매 창 UI에 가격과 개수를 1로 초기화합니다.
		if( g_pItemSeparate->IsInstance(cBaseWindow::WT_STORE, 0) )
		{
			const CsItem::sINFO* pItemInfo = nsCsFileTable::g_pItemMng->GetItem( g_pItemSeparate->GetRegistItemTypeID() )->GetInfo();

			sMONEY tempMoney( static_cast<u8>(pItemInfo->s_dwScanPrice * GetDiscountRate()));

			// 할인 가격으로 구매창 열고 파트너몬 교체 시 변경된 가격 기준으로 최대 구매 개수를 구해서 갱신합니다.
			int nMaxCount = GetEnableBuyCnt(pItemInfo->s_dwItemID);

			g_pItemSeparate->Passive_UpdateDiscount(tempMoney, nMaxCount);

			CsDigimon* pkBaseDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon(pDigimonUser->GetBaseDigimonFTID());
			SAFE_POINTER_RET( pkBaseDigimon );
			// 현재 디지몬의 진화단계와 관계 없이 진화 트리의 베이스 디지몬의 이름으로 설정합니다.
			g_pItemSeparate->SetDiscountText(pkBaseDigimon->GetInfo()->s_szName, static_cast<int>(m_fDiscountRate * 100.f) );
		}
	}
}

bool CStoreContents::OpenInventory()
{
	bool bSound = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );
	
	return true;
}

void CStoreContents::SetStoreItemDataList( const std::list< BaseStoreItemData* >& rItemIndexList )
{
	m_StoreItemDataList = rItemIndexList;
}

std::list< CStoreContents::BaseStoreItemData* > CStoreContents::GetStoreItemDataList() const
{
	return m_StoreItemDataList;
}

CStoreContents::BaseStoreItemData* CStoreContents::GetStoreItemData( int nStoreButtonIndex )
{
	std::list< CStoreContents::BaseStoreItemData* >::iterator iter = m_StoreItemDataList.begin();
	std::list< CStoreContents::BaseStoreItemData* >::iterator enditer = m_StoreItemDataList.end();

	for( int i = 0; iter != enditer; ++iter, ++i )
	{
		if( nStoreButtonIndex == i )
			return (*iter);
	}
	return NULL;
}

CStoreContents::BaseStoreItemData* CStoreContents::GetStoreItemDataByID( int nItemID )
{
	std::list< CStoreContents::BaseStoreItemData* >::iterator iter = m_StoreItemDataList.begin();
	std::list< CStoreContents::BaseStoreItemData* >::iterator enditer = m_StoreItemDataList.end();

	for( ;iter != enditer; ++iter)
	{
		if( (*iter)->m_nItemID == nItemID )
		{
			return (*iter);
		}
	}
	return NULL;
}

std::list< CStoreContents::BaseStoreItemData* > CStoreContents::GetStoreItemDataList( int nTabType )
{
	std::list< CStoreContents::BaseStoreItemData* >::iterator iter = m_StoreItemDataList.begin();
	std::list< CStoreContents::BaseStoreItemData* >::iterator enditer = m_StoreItemDataList.end();

	std::list< BaseStoreItemData* > tempItemDataList;
	for( ; iter != enditer; ++iter )
	{
		if( (*iter)->m_nSellType == nTabType )
			tempItemDataList.push_back( *iter );
	}
	return tempItemDataList;
}

void CStoreContents::CheckEnableSellItem(int const& nInvenIndex)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_STORE ) == false )
		return;

	if( IsEnableSellItem( nInvenIndex ) )
	{
		cItemInfo* pItemInfo = g_pDataMng->GetInven()->GetData( nInvenIndex );
		SAFE_POINTER_RET(pItemInfo);

		CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() )->GetInfo();
		SAFE_POINTER_RET(pFTItem);

		sMONEY money( pFTItem->s_dwSale * pItemInfo->GetCount() );
		cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_STORE_SELL, pFTItem->s_szName, &money );
		cMessageBox::GetFirstMessageBox()->SetValue1( nInvenIndex );
		cMessageBox::GetFirstMessageBox()->SetValue2( pItemInfo->GetCount() );
	}
}

void CStoreContents::RequestStoreItemRepurchase( int nIndex, int nItemID, int nItemCount )
{
#ifdef REPURCHASE
	const CStoreContents::BaseStoreItemData* pItemData = GetRepurchaseItemData( nItemID, nItemCount ); // 재구매한 아이템 데이터
	SAFE_POINTER_RET( pItemData );

	if( IsEnableRepurchase( *pItemData ) ) // 재구매가 가능하면
	{
		cPrintMsg::PrintMsg( cPrintMsg::LOADING_SERVER_INFO ); // 서버 정보를 불러오고 있습니다.

		if( net::game )
			net::game->SendItemRepurchase( m_pInteractNPC->GetFTID(), GetRepurchaseItemIndex( nItemID, nItemCount ) );
	}
#endif
}

void CStoreContents::RequestNormalItemBuy( int nItemID )
{
	int nEnableBuyCnt = 0;
	if( IsEnableBuy( nEnableBuyCnt, nItemID ) )
		OpenItemSeparateWindow( nEnableBuyCnt, nItemID ); // 아이템 구매설정창 열기
}

bool CStoreContents::IsEnableBuy( int& nEnableCount, int nItemID )
{
	DWORD dwNpcID = m_pInteractNPC->GetFTID();

	CsItem::sINFO* pInfo = nsCsFileTable::g_pItemMng->GetItem( nItemID )->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, false );

	nEnableCount = GetEnableBuyCnt( nItemID );

	BaseStoreItemData* pBaseData = GetStoreItemDataByID(nItemID);
	SAFE_POINTER_RETVAL(pBaseData, false);
	// 화폐 부족
	if( nEnableCount <= 0)
	{
		switch( m_nNPCType )
		{
		case nsCsNpcTable::NT_TRADE:
		case nsCsNpcTable::NT_GUILD:
			{
				cPrintMsg::PrintMsg( cPrintMsg::SHOP_MONTY_SHORT );
			}break;
		case nsCsNpcTable::NT_DIGICORE:
			{
				DigicoreStoreItemData* pDigicoreData = static_cast<DigicoreStoreItemData*>(pBaseData);
				cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_SHORTAGE_DIGICORE, const_cast<wchar_t*>(pDigicoreData->m_strCurrencyName.c_str()) );
			}break;
		case nsCsNpcTable::NT_EVENT_STORE:
			{
				EventStoreItemData* pEventData = static_cast<EventStoreItemData*>(pBaseData);
				cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_SHORTAGE, const_cast<wchar_t*>(pEventData->m_strEventItemName.c_str()) );
			}break;
		case nsCsNpcTable::NT_PROPERTY_STORE:
			{
				PropertyStoreItemData* pPropertyData = static_cast<PropertyStoreItemData*>(pBaseData);
				std::list<PropertyStoreItemData::NeedItemInfo>::iterator iter = pPropertyData->m_NeedItemsList.begin();
				std::list<PropertyStoreItemData::NeedItemInfo>::iterator enditer = pPropertyData->m_NeedItemsList.end();

				for( ; iter != enditer; ++iter)
				{
					if(iter->m_nNeedItemID != 0)
					{
						if( 0 == m_pInven->GetCount_Item_ID( iter->m_nNeedItemID ) / iter->m_nNeedItemCount )
							cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_SHORTAGE, const_cast<wchar_t*>(iter->m_strNeedItemName.c_str()) );
					}
				}
			}break;
		}
		return false;
	}
	// 인벤 부족
	else if( m_pInven->GetCount_Item_EnableCrop( nItemID ) == 0 )
	{
		cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
		return false;
	}
	return	true;
}

void CStoreContents::OpenItemSeparateWindow( const int nEnableCount, const int nItemID )
{
	DWORD dwNpcID = m_pInteractNPC->GetFTID();

	BaseStoreItemData* pBaseData = GetStoreItemDataByID( nItemID );
	SAFE_POINTER_RET( pBaseData );
	cItemInfo info;
	info.Set( nItemID, 1, 0, 0 );

	switch( m_nNPCType )
	{
	case nsCsNpcTable::NT_TRADE:
	case nsCsNpcTable::NT_GUILD:
		{
			NormalStoreItemData* pData = static_cast<NormalStoreItemData*>(pBaseData);
			u8 tempPrice = static_cast<u8>( pData->m_nScanPrice * GetDiscountRate());
			cItemSeparate::NewInstance( cBaseWindow::WT_STORE, 0, cItemSeparate::BUY, &info, nEnableCount, tempPrice, cMoneyIF::DISP_FULL );				
		}break;
	case nsCsNpcTable::NT_DIGICORE:
		{
			DigicoreStoreItemData* pData = static_cast<DigicoreStoreItemData*>(pBaseData);
			cItemSeparate::NewInstance( cBaseWindow::WT_STORE, 0, cItemSeparate::BUY, &info, nEnableCount, pData->m_nDigiCorePrice, cMoneyIF::DISP_DIGICORE );
		}break;
	case nsCsNpcTable::NT_EVENT_STORE:
		{			
			EventStoreItemData* pData = static_cast<EventStoreItemData*>(pBaseData);
			cItemSeparate::NewInstance( cBaseWindow::WT_STORE, 0, cItemSeparate::BUY, &info, nEnableCount, pData->m_nEventItemPrice, cMoneyIF::DISP_EVENT, pData->m_nEventItemType );
		}break;
	case nsCsNpcTable::NT_PROPERTY_STORE:
		{
			PropertyStoreItemData* pData = static_cast<PropertyStoreItemData*>(pBaseData);
			std::list<PropertyStoreItemData::NeedItemInfo>::iterator iter	 = pData->m_NeedItemsList.begin();
			std::list<PropertyStoreItemData::NeedItemInfo>::iterator enditer = pData->m_NeedItemsList.end();

			sMONEY tempMoney;
			for( int i = 0;iter != enditer; ++iter, ++i )
			{
				switch(i)
				{
				case 0:	tempMoney.SetProperty_A( iter->m_nNeedItemCount );	break;
				case 1:	tempMoney.SetProperty_B( iter->m_nNeedItemCount );	break;
				case 2:	tempMoney.SetProperty_C( iter->m_nNeedItemCount );	break;
				case 3:	tempMoney.SetProperty_D( iter->m_nNeedItemCount );	break;
				}
			}
			cItemSeparate::NewInstance( cBaseWindow::WT_STORE, dwNpcID, 0, cItemSeparate::BUY, &info, nEnableCount, tempMoney, cMoneyIF::DISP_PROPERTY );
		}break;
	default:
		assert_cs( false );
		break;
	}
}

bool CStoreContents::IsEnableRepurchase( const BaseStoreItemData& rItemData )
{
	SAFE_POINTER_RETVAL( m_pInven, false );
	float nMoneyCnt = (float)m_pInven->GetMoney().GetInt() / (float)( rItemData.m_nSale * ( rItemData.m_nItemShowCount ) );
	if( nMoneyCnt < 1.0f )	// 돈 부족
	{
		cPrintMsg::PrintMsg( cPrintMsg::SHOP_MONTY_SHORT );
		return false;
	}
	else if( (uint)( m_pInven->GetCount_Item_EnableCrop( rItemData.m_nItemID ) ) < rItemData.m_nItemShowCount ) // 인벤 부족
	{
		cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
		return false;
	}
	return true;
}

void CStoreContents::RequestRepurchaseOpen()
{
#ifdef REPURCHASE
	if( net::game )
		net::game->SendItemRepurchaseOpen( m_pInteractNPC->GetFTID() );
	//데이터 수신중 메세지
	cPrintMsg::PrintMsg( cPrintMsg::LOADING_SERVER_INFO );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_STORE_REPURCHASE_SET,	   this, &CStoreContents::SetRepurchaseItemInfo );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_STORE_REPURCHASE_SUCCESS, this, &CStoreContents::CompleteItemRepurchase  ); // 재구매 요청 확인
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_STORE_REPURCHASE_FAILED,  this, &CStoreContents::FailedItemRepurchase  ); // 재구매 요청 확인
#endif
}

void CStoreContents::DeleteRepurchaseItemData( const int nItemTypeID, const int nBuyCount  )
{
	std::list< CStoreContents::BaseStoreItemData* >::iterator iter	 = m_RepurchaseItemList.begin();
	std::list< CStoreContents::BaseStoreItemData* >::iterator itEnd	 = m_RepurchaseItemList.end();
	
	for(;iter != itEnd; ++iter)
	{
		if( nItemTypeID == (*iter)->m_nItemID && nBuyCount == (*iter)->m_nItemShowCount )
		{
			SAFE_DELETE(*iter);
			iter = m_RepurchaseItemList.erase( iter );
			break;
		}
	}
}

const std::list< CStoreContents::BaseStoreItemData* >& CStoreContents::GetRepurchaseItemList() const
{
	return m_RepurchaseItemList;
}

void CStoreContents::SetBuyButtonIndex( int nIndex )
{
	m_nBuyButtonIndex = nIndex;
}

void CStoreContents::ApplyMyDigimonDiscount()
{
	// 특정 디지몬 패시브 기능 (할인)
	CDigimonUser* pkMyPartnerMon = g_pCharMng->GetDigimonUser(0);		// 0번 디지몬[현재 꺼낸 녀석] 가져옴
	DWORD dwDigimonID = pkMyPartnerMon->GetBaseDigimonFTID();
	SetPassiveDiscount(pkMyPartnerMon, dwDigimonID);
}

void CStoreContents::SetInventory()
{
	m_pInven = g_pDataMng->GetInven();
}

void CStoreContents::ChangeMyMoney( void* pData )
{
	Notify( eUpdate_Currency );
}

void CStoreContents::ChangeMyItem( void* pData )
{
	Notify( eUpdate_Currency );
}

CStoreContents::RepurchaseItemData* CStoreContents::MakeRepurchaseItemdata( const cItemData& rItemData )
{
	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( rItemData.m_nType );
	SAFE_POINTER_RETVAL(pItem,NULL);
	const CsItem::sINFO* pInfo = pItem->GetInfo();
	SAFE_POINTER_RETVAL(pInfo,NULL);

	CStoreContents::RepurchaseItemData* pData = new CStoreContents::RepurchaseItemData;

	pData->m_nItemID		= rItemData.m_nType;
	pData->m_nItemShowCount = rItemData.GetCount();
	pData->m_strName.assign(pInfo->s_szName);
	pData->m_nSellType		= REPURCHASE_ITEM_SELLCLASS;
	pData->m_nSale			= pInfo->s_dwSale;
	pData->m_nClass			= pInfo->s_nClass;

	return pData;
}

bool CStoreContents::CreateStoreWindow()
{
	// 윈도우 생성
	cStore* pStore = (cStore*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_STORE, 0, m_nNPCType );
	SAFE_POINTER_RETVAL( pStore, false );
	return true;
}

void CStoreContents::CalcChangeItemInfo( std::map< int, int >& mapAcquire, std::map< int, int >& mapConsume, cItemData OldInvenInfo, cItemData NewInvenInfo )
{
	std::map< int, int >::iterator mapIter;
	// 획득이나 감소 카운트를 계산
	int nDiffCount = NewInvenInfo.GetCount() - OldInvenInfo.GetCount();
	if( nDiffCount < 0 ) // 소모
	{
		int nConsumeItemID = OldInvenInfo.m_nType;
		
		mapIter = mapConsume.find( nConsumeItemID );
		if( mapIter != mapConsume.end() )
			mapIter->second += nDiffCount;

		else
			mapConsume.insert( make_pair(nConsumeItemID, nDiffCount) );

	}
	else if( nDiffCount > 0 ) // 획득
	{
		int nAcquireItemID = NewInvenInfo.m_nType;
		
		mapIter = mapAcquire.find( nAcquireItemID );
		if( mapIter != mapAcquire.end() )
			mapIter->second += nDiffCount;

		else
			mapAcquire.insert( make_pair(nAcquireItemID, nDiffCount) );
	}
}

void CStoreContents::FailedItemRepurchase( void* pData )
{
	n4 nErrorCode = *static_cast<n4*>(pData);

	//데이터 수신중 메세지 삭제
	cMessageBox::DelMsg( cPrintMsg::LOADING_SERVER_INFO, false );	

	switch(nErrorCode)
	{
	case nErrorMessage::eErrorMessage_Repurchase_NotEnoughInvenSlot: //인벤 부족
		{
			cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
		}break;
	}
}

cData_Inven* CStoreContents::GetInventory() const
{
	SAFE_POINTER_RETVAL( m_pInven, NULL );
	return m_pInven;
}

void CStoreContents::PropertyStoreItemData::SetNeedItemInfo( int nID, int nCount )
{
	if( nID == 0 || nCount == 0)
	{
		m_NeedItemsList.push_back(PropertyStoreItemData::NeedItemInfo());
		return;
	}

	m_NeedItemsList.push_back(PropertyStoreItemData::NeedItemInfo(nID,nCount,this->m_strName));
}
