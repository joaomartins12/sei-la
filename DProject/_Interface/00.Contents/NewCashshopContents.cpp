#include "StdAfx.h"
#include "NewCashshopContents.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptBattleSystem.h"
#include "../Adapt/AdaptTutorialQuest.h"
#include "../Adapt/AdaptFriendShoutOut.h"
#include <shellapi.h>


extern void spawn_browser(	const char *uri,	// URL
						  int nWidth,				// Window Width
						  int nHeight,			// Window Height
						  HICON hIcon);			// Handle to the icon

//////////////////////////////////////////////////////////////////////////
// 보유 캐쉬 정보 관련 코드
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::sCashBalance::ClearData()
{
	m_cRecvedState = eNone;
	m_RealCash = 0;
	m_BonusCash = 0;
}

DWORD NewCashshopContents::sCashBalance::GetTotalCash() const
{
	return m_RealCash + m_BonusCash;
}

DWORD NewCashshopContents::sCashBalance::GetRealCash() const
{
	return m_RealCash;
}

DWORD NewCashshopContents::sCashBalance::GetBonusCash() const
{
	return m_BonusCash;
}

void NewCashshopContents::sCashBalance::SetCashBalance( int const& nReal, int const& nBonus )
{
	if( m_RealCash != nReal )
		m_RealCash = nReal;
	if( m_BonusCash != nBonus )
		m_BonusCash = nBonus;
}

void NewCashshopContents::sCashBalance::Recve_CashBalance( int const& nReal, int const& nBonus )
{
	m_cRecvedState = eRecved;
	SetCashBalance( nReal, nBonus );

	m_RefreshTime.SetDeltaTime( 11000 );
	m_RefreshTime.SetDisable();
}


bool NewCashshopContents::sCashBalance::Request_CashBalance()
{
	if( m_cRecvedState == eRecvWait )// 서버에 데이터를 요청한 상태
		return false;

	if( !m_RefreshTime.IsEnable() )	// 서버에 요청할 수 있는 시간이 체크
		return false;
	
	SAFE_POINTER_RETVAL( net::game, false );
	net::game->SendCashShopBalance();

	m_cRecvedState = eRecvWait;
	return true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 아이템 구매 컨테이너 관련 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::PurchaseItemInfo::ClearData()
{
	m_PurchaseItems.clear();
	m_cRecvedState = eNone;
}

bool NewCashshopContents::PurchaseItemInfo::IsRecvWait() const
{
	if( eRecvWait == m_cRecvedState )
		return false;

	return false;
}

bool NewCashshopContents::PurchaseItemInfo::AddPurchaseItem( DWORD const& dwProductIDX )
{
	if( eRecvWait == m_cRecvedState )
		return false;

	m_PurchaseItems.push_back( dwProductIDX );
	return true;
}


bool NewCashshopContents::PurchaseItemInfo::SendPurchaseItem(DWORD const& dwTotalPrice, uint64 ui64OrderID)
{
	if( eRecvWait == m_cRecvedState )
		return false;

	SAFE_POINTER_RETVAL( net::game, false );

	if( m_PurchaseItems.empty() )
	{// 구매할 아이템 목록이 없다.
		return false;
	}	

#ifdef VERSION_STEAM
	if( 0 == ui64OrderID )
	{
		if( STEAM_ST.IsOverlayEnable() )
		{
			net::game->SendCashShopBuyRequest_Steam( dwTotalPrice, m_PurchaseItems );	
			cPrintMsg::PrintMsg( 14010 );
			return true;
		}
		else
		{
			cPrintMsg::PrintMsg(cPrintMsg::CASHSHOP_STEAM_OVERLAY_SYSTEM_DIABLE);
			return false;
		}
	}
#endif

	net::game->SendBuyCashItem( dwTotalPrice, m_PurchaseItems, ui64OrderID );	
	return true;
}

bool NewCashshopContents::PurchaseItemInfo::SendPresentItem(DWORD const& dwTotalPrice, std::wstring const& wsTamerName, uint64 ui64OrderID)
{	
	if( eRecvWait == m_cRecvedState )
		return false;

	SAFE_POINTER_RETVAL( net::game, false );

	if( m_PurchaseItems.empty() )
	{// 구매할 아이템 목록이 없다.
		return false;
	}

	if( 1 != m_PurchaseItems.size() )
	{// 선물하기는 한번에 한개만 가능
		return false;
	}

#ifdef VERSION_STEAM
	if( 0 == ui64OrderID )
	{
		if( STEAM_ST.IsOverlayEnable() )
		{
			net::game->SendCashShopBuyRequest_Steam( dwTotalPrice, m_PurchaseItems );	
			cPrintMsg::PrintMsg( 14010 );
			return true;
		}
		else
		{
			cPrintMsg::PrintMsg(cPrintMsg::CASHSHOP_STEAM_OVERLAY_SYSTEM_DIABLE);
			return false;
		}
	}
#endif

	net::game->SendGiftCashItem( dwTotalPrice, m_PurchaseItems, wsTamerName, ui64OrderID );	
	return true;	
}

NewCashshopContents::PurchaseItems const& NewCashshopContents::PurchaseItemInfo::GetList() const
{
	return m_PurchaseItems;
}

size_t NewCashshopContents::PurchaseItemInfo::GetPurchaseItemCount() const
{
	return m_PurchaseItems.size();
}

DWORD NewCashshopContents::PurchaseItemInfo::GetFirstProductIDX() const
{
	if( m_PurchaseItems.empty() )
		return 0;
	return m_PurchaseItems.front();
}

bool NewCashshopContents::PurchaseItemInfo::IsEmpty() const
{
	return m_PurchaseItems.empty();
}

bool NewCashshopContents::PurchaseItemInfo::ChangeProductIDX(int const& nIdx, DWORD const& dwProductIDX)
{
	PurchaseItemIter it = m_PurchaseItems.begin();
	for( int n = 0; it != m_PurchaseItems.end(); ++it, ++n )
	{
		if( nIdx == n )
		{
			*it = dwProductIDX;
			return true;
		}
	}

	return false;
}

bool NewCashshopContents::PurchaseItemInfo::IsSameProductIDX( DWORD const& dwCheckProductIDX ) const
{
	if( 1 != m_PurchaseItems.size() )
		return false;
	if( m_PurchaseItems.front() != dwCheckProductIDX )
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 구매 내역 관련 함수들
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::sBuyHistory::ClearData()
{
// 	m_RefreshTime.SetDeltaTime( 11000 );
// 	m_RefreshTime.SetEnable();
	m_cRecvedState = eNone;
	m_HistoryList.clear();
}

bool NewCashshopContents::sBuyHistory::AddHistoryData( DWORD const& dwProductIDX )
{
	if( m_cRecvedState == eRecvWait )
		return false;

	if( 0 == dwProductIDX )
		return false;

	m_HistoryList.push_front( dwProductIDX );
	return true;
}

bool NewCashshopContents::sBuyHistory::Request_HistoryData()
{
	if( m_cRecvedState != eNone )
		return false;

// 	if( !m_RefreshTime.IsEnable() )
// 		return false;

	SAFE_POINTER_RETVAL( net::game, false );
	net::game->SendCashShopBuyHistory();
	m_cRecvedState = eRecvWait;
	return true;
}

void NewCashshopContents::sBuyHistory::RecvedHistoryData( std::list<n4> const& dataList )
{
	m_cRecvedState = eRecved;
	m_HistoryList.clear();
	std::list<n4>::const_iterator it = dataList.begin();
	for( ; it != dataList.end(); ++it )
		m_HistoryList.push_back( *it );
}

NewCashshopContents::BuyItemHistory NewCashshopContents::sBuyHistory::GetList() const
{
	return m_HistoryList;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 캐쉬 창고 컨테이너 관련 코드
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::sCashWarehouseInfo::ClearData()
{
	m_cRecvedState = eNone;

	CashStorageIter kIter = m_ItemList.begin();
	for(;kIter != m_ItemList.end();)
	{
		SAFE_DELETE((*kIter));
		kIter = m_ItemList.erase(kIter);
	}

	m_ItemList.clear();
}

bool NewCashshopContents::sCashWarehouseInfo::Request_CashWarehouseInfo()
{
	if( eRecvWait == m_cRecvedState )
		return false;

	if( !m_RefreshTime.IsEnable() )
		return false;

	SAFE_POINTER_RETVAL( net::game, false );
	net::game->SendReqCashShop();
	m_cRecvedState = eRecvWait;
	return true;
}

void NewCashshopContents::sCashWarehouseInfo::Recv_CashWarehouseInfo( std::vector<cItemData*> & vecData )
{
	ClearData();
	m_ItemList.reserve( vecData.size() );	
	
	std::vector<cItemData*>::iterator it = vecData.begin();
 	for( ; it != vecData.end(); ++it )
	{
		SAFE_POINTER_CON( *it );
		cItemInfo* pkItemInfo = new cItemInfo();
		*pkItemInfo = *(*it);
		m_ItemList.push_back( pkItemInfo );
	}

	m_RefreshTime.SetDeltaTime(11000);
	m_RefreshTime.SetDisable();
	m_cRecvedState = eRecved;
}

NewCashshopContents::CashStorageItems const& NewCashshopContents::sCashWarehouseInfo::GetCashWarehouseInfo() const
{
	return m_ItemList;
}


bool NewCashshopContents::sCashWarehouseInfo::RemoveCashWarehouse(int const& nCashInvenSlotNum)
{
	m_cRecvedState = eRecved;

	if( m_ItemList.size() <= nCashInvenSlotNum || nCashInvenSlotNum < 0 )
		return false;

	cItemInfo* pkItemInfo = m_ItemList.at(nCashInvenSlotNum);
	SAFE_POINTER_RETVAL( pkItemInfo, false );

	pkItemInfo->Clear();

	return true;
}

cItemInfo* NewCashshopContents::sCashWarehouseInfo::GetItemInfo( int const& nSlotIdx )
{
	if( m_ItemList.size() <= nSlotIdx || nSlotIdx < 0 )
		return NULL;

	return m_ItemList.at(nSlotIdx);
}

bool NewCashshopContents::sCashWarehouseInfo::Request_CashItemCrop(int const& nCashInvenSlotNum)
{
	if( eRecvWait == m_cRecvedState )
		return false;// 서버에서 데이터를 받을 때까지 기다림.
	
	cItemInfo* pkItemInfo = GetItemInfo(nCashInvenSlotNum);
	SAFE_POINTER_RETVAL( pkItemInfo, false );
	
	net::game->SendCropCashItem( nCashInvenSlotNum );
	m_cRecvedState = eRecvWait;
	return true;
}
//////////////////////////////////////////////////////////////////////////


bool NewCashshopContents::sProductItemInfo::IsPurchaseEnable() const
{
	SAFE_POINTER_RETVAL( net::game, false );
	uint nCurTime = net::game->GetTimeTS();
	if( nCurTime < nStartTime || nCurTime > nEndTime )
		return false;
	return true;
}

bool NewCashshopContents::sProductItemInfo::IsIncludeKeyword( std::wstring const& wsFileringKeyword )
{
	if( wsFileringKeyword.empty() )
	{
		bIsFileringShow = true;
	}
	else
	{
		std::size_t findpos = wsName.find( wsFileringKeyword );
		if( findpos != std::wstring::npos )
			bIsFileringShow = true;
		else
			bIsFileringShow = false;
	}

	return bIsFileringShow;		
}

void  NewCashshopContents::sProductItemInfo::ChangeFirstProductIDX( DWORD const& dwProductIDX )
{
	CASHITEM_TABLE_CIT it = pTableInfo.begin();
	for( ; it != pTableInfo.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		if( (*it)->dwProductID != dwProductIDX )
			continue;

		dwFirstProductIDX = dwProductIDX;
		nShowPrice = (*it)->nRealSellingPrice;
		nStandardPrice = (*it)->nStandardSellingPrice;
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void NewCashshopContents::sSubCategoryInfo::SetFileringKeyword( std::wstring const& wsFileringKeyword )
{
	LIST_PRODUCTGROUP_INFO_IT it = itemgroup.begin();
	for( ; it != itemgroup.end(); ++it )
		(*it).IsIncludeKeyword( wsFileringKeyword );
}

size_t NewCashshopContents::sSubCategoryInfo::GetShowItemCount() const
{
	int nCount = 0;
	LIST_PRODUCTGROUP_INFO_CIT it = itemgroup.begin();
	for( ; it != itemgroup.end(); ++it )
	{
		if( (*it).bIsFileringShow )
			nCount++;
	}

	return nCount;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void NewCashshopContents::sMainCategoryInfo::SetFileringKeyword( std::wstring const& wsFileringKeyword )
{
	MAP_SUBCATEGORY_INFO_IT it = subCategoryinfo.begin();
	for( ; it != subCategoryinfo.end(); ++it )
		it->second.SetFileringKeyword( wsFileringKeyword );
}
//////////////////////////////////////////////////////////////////////////

int const NewCashshopContents::IsContentsIdentity(void)
{
	return E_CT_NEW_CASHSHOP;
}

NewCashshopContents::NewCashshopContents(void)
:m_pFriendContents(0),
m_AdaptTutorialSystem(0),
m_AdaptBattleSystem(0),
m_nHistoryIdx(-1)
{
}

NewCashshopContents::~NewCashshopContents(void)
{
	m_pFriendContents = NULL;
	m_AdaptTutorialSystem = NULL;
	m_AdaptBattleSystem = NULL;
}

int const NewCashshopContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool NewCashshopContents::Initialize(void)
{
	return true;
}

void NewCashshopContents::UnInitialize(void)
{
}

bool NewCashshopContents::IntraConnection(ContentsSystem& System)
{
	m_pFriendContents = new AdaptFriendShoutOut;
	if( m_pFriendContents && !m_pFriendContents->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_FRIENDLIST ) ) )
		return false;

	m_AdaptTutorialSystem = new AdaptTutorialQuest;
	if( m_AdaptTutorialSystem && !m_AdaptTutorialSystem->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TUTORIAL_CONTENTS ) ) )
		return false;

	m_AdaptBattleSystem = new AdaptBattleSystem;
	if( m_AdaptBattleSystem && !m_AdaptBattleSystem->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_BATTLE_SYSTEM ) ) )
		return false;

	return true;
}

void NewCashshopContents::Update(float const& fElapsedTime)
{
}

void NewCashshopContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void NewCashshopContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_Cashshop_ShowProductItem:
		{			
			DWORD dwSelectProductID = 0;
			kStream >> dwSelectProductID;
			_ShowCashshopItemSelect( dwSelectProductID );
		}
		break;
	}
}

void NewCashshopContents::MakeMainActorData(void)
{	
}

void NewCashshopContents::ClearMainActorData(void)
{
}

void NewCashshopContents::MakeWorldData(void)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_SHOW_NEWCASHSHOP, this, &NewCashshopContents::Req_ShowCashshop);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQ_HIDE_NEWCASHSHOP, this, &NewCashshopContents::Req_HideCashshop);
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CASHSHOP_TOGGLE, this, &NewCashshopContents::Req_ToggleCashshop);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CASH_TOGGLE, this, &NewCashshopContents::ToggleCashWarehouse );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::CLOSE_CASH_ITEMBUY_WINDOW, this, &NewCashshopContents::CloseBuyWindow);	// 캐쉬 구매 창 닫는 이벤트

	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_CASHSHOP_ITEM_PURCHASE, this, &NewCashshopContents::Send_CashShopItemPurchase);// 캐쉬 아이템 구매에대한 요청
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_CASHSHOP_ITEM_PRESENT, this, &NewCashshopContents::Send_CashShopItemPresent);// 캐쉬 아이템 구매에대한 요청
	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_CASHSHOP_ITEM_CROP, this, &NewCashshopContents::Send_CashItemCrop);	// 캐쉬창고에서 아이템 획득에 대한 요청

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CASHSHOP_STORAGE, this, &NewCashshopContents::Recv_CashStorage);		// 캐쉬 창고 아이템 목록 요청에대한 결과
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_RESULT_HAVE_CASH_REFRESH, this, &NewCashshopContents::Recv_CashBalance);	// 보유 캐쉬 갱신에대한 결과
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CASHSHOP_ITEM_CROP, this, &NewCashshopContents::Recv_CashItemCrop);	// 캐쉬창고에서 아이템 획득에대한 결과
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CASHSHOP_ITEM_PRESENT, this, &NewCashshopContents::Recv_PresentItem);	// 캐쉬 아이템 선물에대한 결과
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_RESULT_CASHITEM_PURCHASE, this, &NewCashshopContents::Recv_PurchaseItem);// 캐쉬 아이템 구매에 대한 결과

	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQUEST_STEAM_CASHITEM_PURCHASE, this, &NewCashshopContents::Callback_CashItemPurchase_Steam);
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_REQUEST_STEAM_CASHITEM_PURCHASE, this, &NewCashshopContents::Recv_RequestCashItemPurchase_Steam);

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CASHSHOP_BUY_HISTORY, this, &NewCashshopContents::Recv_Cashshop_Buy_History );

	if( EventRouter() )
		EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_Cashshop_ShowProductItem, this );

	_LoadCashShopTable();
	mBaskets.clear();
	for( int n = 0; n < pCashShop::MAX_CASHITEM_COUNT; ++n )
		mBaskets.insert( std::make_pair( n, 0 ) );

	m_OnOffTimeSeq.SetDeltaTime(2000);
	m_OnOffTimeSeq.SetEnable();
}

void NewCashshopContents::ClearWorldData(void)
{
	_ClearAllData();
	GAME_EVENT_ST.DeleteEventAll( this );
	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
}

void NewCashshopContents::_ClearAllData(void)
{	
	m_mapInactiveProductItems.clear();
	mBaskets.clear();
	OriginEquipments.clear();
	ChangedEquip.clear();
	SerchingKeyword.clear();
	mMainCategoryInfos.clear();

	mCashBalance.ClearData();
	mCashStorageItems.ClearData();
	mBuyHistory.ClearData();
	mPurchaseItems.ClearData();
}

//////////////////////////////////////////////////////////////////////////
// 캐쉬샵 테이블을 읽어 들여 데이터 세팅
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::_LoadCashShopTable()
{
	SAFE_POINTER_RET(nsCsFileTable::g_pCashShopMng);
	m_mapInactiveProductItems.clear();

	MAP_CASHSHOP_CATEGORY const* mapCashShop = NULL;
	if( GLOBALDATA_ST.IsCountry( GData::eCountry_Steam ) )
		mapCashShop = nsCsFileTable::g_pCashShopMng->GetCashShopInfo(CsCashShopMng::eSteam);
	else
		mapCashShop = nsCsFileTable::g_pCashShopMng->GetCashShopInfo();
	SAFE_POINTER_RET( mapCashShop );
	
	MAP_CASHSHOP_CATEGORY_CIT it = mapCashShop->begin();
	for( ; it != mapCashShop->end(); ++it )
	{
		MAP_MAINCATEGORY_INFO_IT mainIt = mMainCategoryInfos.find( it->first );
		if( mainIt == mMainCategoryInfos.end() )// 대분류 키값
		{
			auto rib = mMainCategoryInfos.insert( std::make_pair( it->first, sMainCategoryInfo() ) );
			mainIt = rib.first;
			mainIt->second.name = it->second.wsCategoryName;//대분류 이름
		}
		
		MAP_CASHSHOP_SUBCATEGORY_CIT secIt = it->second.mapSubCategoryInfo.begin();
		for( ; secIt != it->second.mapSubCategoryInfo.end(); ++secIt )
		{
			MAP_SUBCATEGORY_INFO_IT subIt = mainIt->second.subCategoryinfo.find( secIt->first );
			if( subIt == mainIt->second.subCategoryinfo.end() )//소분류 키값
			{
				auto rib = mainIt->second.subCategoryinfo.insert( std::make_pair( secIt->first, sSubCategoryInfo() ) );
				subIt = rib.first;
				subIt->second.name = secIt->second.wsCategoryName;// 소분류 이름
			}
			
// 			LIST_CASHSHOP_PRODUCT_GROUP_CIT treIT = secIt->second.listProductData.begin();
// 			for( ; treIT != secIt->second.listProductData.end(); ++treIT )
			// 서브 카테고리의 품목 출력을 역순으로 출력되게 기획에서 요청함.
			LIST_CASHSHOP_PRODUCT_GROUP::const_reverse_iterator treIT = secIt->second.listProductData.rbegin();
			for( ; treIT != secIt->second.listProductData.rend(); ++treIT )
			{
				sProductItemInfo addNewItem;

				addNewItem.dwGroupID = (*treIT).dwItemGroupID;

				LIST_CASHSHOP_PRODUCT_CIT forIt = (*treIT).ProductGroupList.begin();
				for( ; forIt != (*treIT).ProductGroupList.end(); ++forIt )
				{
					if( !(*forIt).bActive )
					{
						m_mapInactiveProductItems.insert( std::make_pair( (*forIt).dwProductID, &(*forIt) ) );
						continue;
					}
					else if( addNewItem.dwFirstProductIDX == 0 )
					{
						addNewItem.wsName = (*forIt).wsName;				// 판매 물품 이름
						addNewItem.wsComment = (*forIt).wsComment;			// 판매 물품 설명
						addNewItem.nStartTime = nBase::GetTime( const_cast<char*>((*forIt).szStartTime) );
						addNewItem.nEndTime = nBase::GetTime( const_cast<char*>((*forIt).szEndTime) );
						addNewItem.nIconID = (*forIt).nIconID;			// 아이콘 ID
						addNewItem.nShowPrice = (*forIt).nRealSellingPrice;
						addNewItem.nStandardPrice = (*forIt).nStandardSellingPrice;
						addNewItem.nSaleIconIdx = ((*forIt).nSalePersent / 5) + 4;// 할인률 표시 이미지의 인덱스 번호 계산
						addNewItem.nMaskType = (*forIt).nMaskType;			// 1 : New, 2 : Hot, 3 : Event
						addNewItem.dwFirstProductIDX = (*forIt).dwProductID;
					}

					addNewItem.pTableInfo.push_back( &(*forIt) );
				}

				if( it->first == eMain || 0 != addNewItem.dwFirstProductIDX )
					subIt->second.itemgroup.push_back( addNewItem );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 테이블 데이터 넣은 후 서브 카테고리의 모두 탭에 데이터를 넣는다.
	//////////////////////////////////////////////////////////////////////////
	MAP_MAINCATEGORY_INFO_IT mainIt = mMainCategoryInfos.begin();
	for( int mainCount = 0; mainIt != mMainCategoryInfos.end(); ++mainIt, ++mainCount )
	{
		MAP_SUBCATEGORY_INFO_IT subIt = mainIt->second.subCategoryinfo.begin();
		for( ; subIt != mainIt->second.subCategoryinfo.end(); ++subIt )
		{
			if( eMain == mainIt->first  )
			{
				LIST_PRODUCTGROUP_INFO_IT groupIt = subIt->second.itemgroup.begin();
				for( ; groupIt != subIt->second.itemgroup.end(); ++groupIt )
				{
					sProductItemInfo const* findItem = GetCashshop_ItemGropInfo( (*groupIt).dwGroupID );
					if( findItem )
						(*groupIt) = *findItem;
				}
			}
			else
			{
				if( !subIt->second.itemgroup.empty() )
					continue;

				MAP_SUBCATEGORY_INFO_IT insertIt = subIt;
				insertIt++;
				for( ; insertIt != mainIt->second.subCategoryinfo.end(); ++insertIt )
				{
					LIST_PRODUCTGROUP_INFO_IT iIt = insertIt->second.itemgroup.begin();
					for( ; iIt != insertIt->second.itemgroup.end(); ++iIt )
						subIt->second.itemgroup.push_back( *iIt );
				}
			}
		}
	}	
 
	//////////////////////////////////////////////////////////////////////////
	// 최근 구매 탭을 테이블의 마지막 서브 카테고르 다음에 넣는다.
	//////////////////////////////////////////////////////////////////////////
	MAP_MAINCATEGORY_INFO::reverse_iterator lastIt = mMainCategoryInfos.rbegin();
	if( lastIt != mMainCategoryInfos.rend() )
		m_nHistoryIdx = lastIt->first + 1;

	auto rib = mMainCategoryInfos.insert( std::make_pair( m_nHistoryIdx, sMainCategoryInfo() ) );
	if( rib.second )
	{
		rib.first->second.name = UISTRING_TEXT("CASHSHOP_WINDOW_MAINTAB_RECENT_PURCHASES");//L"최근 구매";//대분류 이름

		auto secrib = rib.first->second.subCategoryinfo.insert( std::make_pair( 1, sSubCategoryInfo() ) );
		if( secrib.second )
			secrib.first->second.name = UISTRING_TEXT("CASHSHOP_WINDOW_SUBTAB_PURCHASES_HISTORY");//L"구매 목록";// 소분류 이름
	}
	
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 메인 카테고리 이름을 얻는 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::GetMainCategoryName( std::map<int,std::wstring>& categoryInfo )
{
	categoryInfo.clear();
	MAP_MAINCATEGORY_INFO_CIT it = mMainCategoryInfos.find( eMain );
	if( it == mMainCategoryInfos.end() )
		return;

	for( ; it != mMainCategoryInfos.end(); ++it )
		categoryInfo.insert( std::make_pair( it->first, it->second.name ) );	
}

//////////////////////////////////////////////////////////////////////////
// 서브 카테고리 이름을 얻는 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::GetSubCategoryName( int const& nMainType, std::map<int,std::wstring>& categoryInfo )
{
	categoryInfo.clear();

	if( eMain == nMainType )// 메인 카테고리 탭의 서브 카테고리는 표시 하지 않는다.
		return;

	MAP_MAINCATEGORY_INFO_CIT it = mMainCategoryInfos.find( nMainType );
	if( it == mMainCategoryInfos.end() )
		return;

	MAP_SUBCATEGORY_INFO_CIT subIt = it->second.subCategoryinfo.begin();
	for( ; subIt != it->second.subCategoryinfo.end(); ++subIt )
		categoryInfo.insert( std::make_pair( subIt->first, subIt->second.name ) );
}

//////////////////////////////////////////////////////////////////////////
// 카테고리로 해당 카테고리에 있는 모든 목록을 얻는 함수
//////////////////////////////////////////////////////////////////////////
NewCashshopContents::LIST_PRODUCTGROUP_INFO const * NewCashshopContents::GetCategoryItems( int const& nMainType, int const& nSubType )
{
	MAP_MAINCATEGORY_INFO_CIT it = mMainCategoryInfos.find( nMainType );
	if( it == mMainCategoryInfos.end() )
		return NULL;

	MAP_SUBCATEGORY_INFO_CIT subIT = it->second.subCategoryinfo.find(nSubType);
	if( subIT == it->second.subCategoryinfo.end() )
		return NULL;

	return &subIT->second.itemgroup;
}

//////////////////////////////////////////////////////////////////////////
// 카테고리로 해당 카테고리에 있는 모든 목록을 얻는 함수
//////////////////////////////////////////////////////////////////////////
NewCashshopContents::LIST_PRODUCTGROUP_INFO * NewCashshopContents::_GetCategoryItems( int const& nMainType, int const& nSubType )
{
	MAP_MAINCATEGORY_INFO_IT it = mMainCategoryInfos.find( nMainType );
	if( it == mMainCategoryInfos.end() )
		return NULL;

	MAP_SUBCATEGORY_INFO_IT subIT = it->second.subCategoryinfo.find(nSubType);
	if( subIT == it->second.subCategoryinfo.end() )
		return NULL;

	return &subIT->second.itemgroup;
}

void NewCashshopContents::GetRandomCategoryItemGroup( int const& nMainType, int const& nSubType, std::list<DWORD>& result )
{
	result.clear();

	MAP_MAINCATEGORY_INFO_CIT it = mMainCategoryInfos.find( nMainType );
	if( it == mMainCategoryInfos.end() )
		return;

	MAP_SUBCATEGORY_INFO_CIT subIT = it->second.subCategoryinfo.find(nSubType);
	if( subIT == it->second.subCategoryinfo.end() )
		return;

	std::vector<DWORD> vec;
	vec.resize( subIT->second.itemgroup.size() );
	LIST_PRODUCTGROUP_INFO_CIT forIt = subIT->second.itemgroup.begin();
	for( ; forIt != subIT->second.itemgroup.end(); ++forIt )
	{
		if( !(*forIt).IsPurchaseEnable() )// 판매가 종료된 상품 제거
			continue;
		vec.push_back( (*forIt).dwGroupID );
	}

	std::random_shuffle( vec.begin(), vec.end() );

	std::vector<DWORD>::const_iterator reIt = vec.begin();
	for( ; reIt != vec.end(); ++reIt )
		result.push_back( *reIt );

	//std::copy( vec.begin(), vec.end(), std::inserter( result, result.begin() ) );
}

//////////////////////////////////////////////////////////////////////////
// 카테고리로 해당 카테고리에 있는 모든 목록을 얻는 함수
//////////////////////////////////////////////////////////////////////////
NewCashshopContents::sSubCategoryInfo const * NewCashshopContents::GetSubCategoryInfos( int const& nMainType, int const& nSubType )
{
	MAP_MAINCATEGORY_INFO_CIT it = mMainCategoryInfos.find( nMainType );
	if( it == mMainCategoryInfos.end() )
		return NULL;

	MAP_SUBCATEGORY_INFO_CIT subIT = it->second.subCategoryinfo.find(nSubType);
	if( subIT == it->second.subCategoryinfo.end() )
		return NULL;

	return &subIT->second;
}
//////////////////////////////////////////////////////////////////////////
// 아이템 그룸 번호와 아이템 판매 번호로 아이템 정보를 얻는 함수
//////////////////////////////////////////////////////////////////////////
sCashshop_Item const* NewCashshopContents::GetProductItemInfo( DWORD const& dwGroup, DWORD const& dwProductItem ) const
{
	int nMain = CsCashShopMng::GetLargeType( dwProductItem );
	MAP_MAINCATEGORY_INFO_CIT it = mMainCategoryInfos.find( nMain );
	if( it == mMainCategoryInfos.end() )
		return NULL;

	int nSub = CsCashShopMng::GetSmallType( dwProductItem );
	MAP_SUBCATEGORY_INFO_CIT subIT = it->second.subCategoryinfo.find(nSub);
	if( subIT == it->second.subCategoryinfo.end() )
		return NULL;

	LIST_PRODUCTGROUP_INFO_CIT secIt = subIT->second.itemgroup.begin();
	for( ; secIt != subIT->second.itemgroup.end(); ++secIt )
	{
		if( dwGroup == (*secIt).dwGroupID )
			break;
	}

	if( secIt == subIT->second.itemgroup.end() )
		return NULL;

	CASHITEM_TABLE_CIT forIt = (*secIt).pTableInfo.begin();
	for( ; forIt != (*secIt).pTableInfo.end(); ++forIt )
	{
		SAFE_POINTER_CON( (*forIt) );
		if( (*forIt)->dwProductID == dwProductItem )
			return *forIt;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 아이템 판매 번호로 아이템 정보를 얻는 함수
//////////////////////////////////////////////////////////////////////////
sCashshop_Item const* NewCashshopContents::GetProductItemInfo( DWORD const& dwProductItem ) const
{
	int nMain = CsCashShopMng::GetLargeType( dwProductItem );
	MAP_MAINCATEGORY_INFO_CIT it = mMainCategoryInfos.find( nMain );
	if( it == mMainCategoryInfos.end() )
		return NULL;

	int nSub = CsCashShopMng::GetSmallType( dwProductItem );
	MAP_SUBCATEGORY_INFO_CIT subIT = it->second.subCategoryinfo.find(nSub);
	if( subIT == it->second.subCategoryinfo.end() )
		return NULL;

	LIST_PRODUCTGROUP_INFO_CIT secIt = subIT->second.itemgroup.begin();
	for( ; secIt != subIT->second.itemgroup.end(); ++secIt )
	{
		CASHITEM_TABLE_CIT forIt = (*secIt).pTableInfo.begin();
		for( ; forIt != (*secIt).pTableInfo.end(); ++forIt )
		{
			SAFE_POINTER_CON( (*forIt) );
			if( (*forIt)->dwProductID == dwProductItem )
				return *forIt;
		}
	}	

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 아이템 판매 번호로 아이템이 속한 그룹의 모든 데이터를 얻는 함수
//////////////////////////////////////////////////////////////////////////
NewCashshopContents::sProductItemInfo const* NewCashshopContents::GetCashshop_ItemGropInfo( DWORD const& dwProductItemID ) const
{
	int nMain = CsCashShopMng::GetLargeType( dwProductItemID );
	MAP_MAINCATEGORY_INFO_CIT it = mMainCategoryInfos.find( nMain );
	if( it == mMainCategoryInfos.end() )
		return NULL;

	int nSub = CsCashShopMng::GetSmallType( dwProductItemID );
	MAP_SUBCATEGORY_INFO_CIT subIT = it->second.subCategoryinfo.find(nSub);
	if( subIT == it->second.subCategoryinfo.end() )
		return NULL;

	LIST_PRODUCTGROUP_INFO_CIT secIt = subIT->second.itemgroup.begin();
	for( ; secIt != subIT->second.itemgroup.end(); ++secIt )
	{
		CASHITEM_TABLE_CIT forIt = (*secIt).pTableInfo.begin();
		for( ; forIt != (*secIt).pTableInfo.end(); ++forIt )
		{
			SAFE_POINTER_CON( (*forIt) );
			if( (*forIt)->dwProductID == dwProductItemID )
				return &*secIt;
		}
	}	

	return NULL;
}

NewCashshopContents::sProductItemInfo const* NewCashshopContents::GetCashshop_ItemGropInfo( int const& nMainType, int const& nSubType, DWORD const& dwItemGroupID ) const
{
	MAP_MAINCATEGORY_INFO_CIT it = mMainCategoryInfos.find( nMainType );
	if( it == mMainCategoryInfos.end() )
		return NULL;

	MAP_SUBCATEGORY_INFO_CIT subIT = it->second.subCategoryinfo.find(nSubType);
	if( subIT == it->second.subCategoryinfo.end() )
		return NULL;

	LIST_PRODUCTGROUP_INFO_CIT forIt = subIT->second.itemgroup.begin();
	for( ; forIt != subIT->second.itemgroup.end(); ++forIt )
	{
		if( dwItemGroupID == (*forIt).dwGroupID )
			break;
	}
	
	if( forIt == subIT->second.itemgroup.end() )
		return NULL;

	return &(*forIt);
}
//////////////////////////////////////////////////////////////////////////
// 판매 번호가 테이블에 있는지 검사하는 함수
//////////////////////////////////////////////////////////////////////////
bool NewCashshopContents::CheckVailedItem( DWORD const& dwProductItemID ) const
{
	int nMain = CsCashShopMng::GetLargeType( dwProductItemID );
	MAP_MAINCATEGORY_INFO_CIT it = mMainCategoryInfos.find( nMain );
	if( it == mMainCategoryInfos.end() )
		return false;

	int nSub = CsCashShopMng::GetSmallType( dwProductItemID );
	MAP_SUBCATEGORY_INFO_CIT subIT = it->second.subCategoryinfo.find(nSub);
	if( subIT == it->second.subCategoryinfo.end() )
		return false;

	LIST_PRODUCTGROUP_INFO_CIT secIt = subIT->second.itemgroup.begin();
	for( ; secIt != subIT->second.itemgroup.end(); ++secIt )
	{
		CASHITEM_TABLE_CIT forIt = (*secIt).pTableInfo.begin();
		for( ; forIt != (*secIt).pTableInfo.end(); ++forIt )
		{
			SAFE_POINTER_CON( (*forIt) );
			if( (*forIt)->dwProductID == dwProductItemID )
				return true;
		}
	}	
	
	return false;
}

void NewCashshopContents::GetFriendList(std::list<std::wstring>& friendList)
{
	friendList.clear();
	SAFE_POINTER_RET( m_pFriendContents );

	m_pFriendContents->GetFriendList( friendList );
}

bool NewCashshopContents::IsMyFriend( std::wstring const& wsTamerName )
{
	SAFE_POINTER_RETVAL( m_pFriendContents, false );
	return m_pFriendContents->IsMyFriend(wsTamerName);
}
//////////////////////////////////////////////////////////////////////////
// 캐쉬샵을 이용할 수 있는지 검사
//////////////////////////////////////////////////////////////////////////
bool NewCashshopContents::_CheckEnableCashshop()
{
	SAFE_POINTER_RETVAL( m_AdaptTutorialSystem, false );
	SAFE_POINTER_RETVAL( m_AdaptBattleSystem, false );

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RETVAL( pDigimonUser, false );
	if( pDigimonUser->IsDie() )// 디지몬이 죽었을 때 캐쉬샵 안열리도록 추가
		return false;

	if( m_AdaptTutorialSystem->IsTutorialPlaying() )
	{
		cPrintMsg::PrintMsg( 30412 );
		return false;
	}

	if( GLOBALDATA_ST.IsChannelingService() ) // 채널링 계정으로 접속시 캐시샵 단축키 오픈 제어
	{
		cMessageBox::DelMsg( 20089, false );
		cPrintMsg::PrintMsg( 20089 );
		return false;
	}

// 	if( m_AdaptBattleSystem->IsBattleMatchingRegisted() )
// 		return false;

	// 배틀 중에는 캐쉬샵 창을 열지 못하게함.
	if( m_AdaptBattleSystem->IsBattlePlaying() )
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
//캐시샵을 창을 띄운다
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::Req_ShowCashshop(void* pkData)
{
	_OpenCashshopWindow();
}

bool NewCashshopContents::_OpenCashshopWindow()
{
	if( !_CheckEnableCashshop() )
	{
		_CloseCashshopWindow();
		return false;
	}

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pTamerUser, false );
	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pDigimonUser, false );

	pTamerUser->ReleaseArriveTarget();
	pTamerUser->DeletePath();
	pDigimonUser->DeletePath();

	if( !m_OnOffTimeSeq.IsEnable() )
		return false;

	m_OnOffTimeSeq.SetDisable();


	SAFE_POINTER_RETVAL( g_pGameIF, false );

	if(g_pServerMoveOwner)
		g_pServerMoveOwner->KeyReset();

	_InitCharacterEquipment();

	mCashBalance.Request_CashBalance();
	mBuyHistory.Request_HistoryData();

	g_pGameIF->GetDynamicIF( cBaseWindow::WT_NEW_CASHSHOP);
	return true;
}
//////////////////////////////////////////////////////////////////////////
//캐시샵 창 토글
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::Req_ToggleCashshop(void* pkData)
{
	SAFE_POINTER_RET( g_pGameIF );
	bool bShow = g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_CASHSHOP );
	if( bShow )
		_CloseCashshopWindow();
	else
		_OpenCashshopWindow();
}

//////////////////////////////////////////////////////////////////////////
// 캐쉬 창고 토글
void NewCashshopContents::ToggleCashWarehouse(void* pData)
{
	SAFE_POINTER_RET( g_pGameIF );
	bool bShow = g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_CASH_WAREHOUSE );
	if( bShow )
	{
		g_pGameIF->CloseDynamicIF(cBaseWindow::WT_NEW_CASH_WAREHOUSE);
		return;
	}

	mCashStorageItems.Request_CashWarehouseInfo();
	g_pGameIF->GetDynamicIF(cBaseWindow::WT_NEW_CASH_WAREHOUSE);
}

//////////////////////////////////////////////////////////////////////////
// 캐쉬샵 창 닫기
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::Req_HideCashshop(void* pkData)
{
	_CloseCashshopWindow();
}

bool NewCashshopContents::_CloseCashshopWindow()
{
	if( !m_OnOffTimeSeq.IsEnable() )
		return false;

	/*m_OnOffTimeSeq.SetDisable();*/

	OriginEquipments.clear();	// 원래의 장비 정보
	ChangedEquip.clear();		// 변경된 장비 정보
	Clear_SetFilteringKeyword();
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_NEW_CASHSHOP);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 창을 열때 테이머가 착용중인 장비 데이터 저장.
//////////////////////////////////////////////////////////////////////////
bool NewCashshopContents::_InitCharacterEquipment()
{
	if( g_pCharMng )
	{
		CTamerUser* pkTamerUser = g_pCharMng->GetTamerUser();
		if( pkTamerUser )
		{
			pkTamerUser->ReleaseArriveTarget();
			pkTamerUser->DeletePath();
		}
	}

	ChangedEquip.clear();
	OriginEquipments.clear();

	cItemInfo* pItemInfo = NULL;
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
	for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
#else
	for( int i=0; i<nsPART::MAX_TOTAL_COUNT + 1; ++i )
#endif
	{
		CsC_PartObject::sCHANGE_PART_INFO cp;
		if( i == nTamer::MaxParts )
			pItemInfo = g_pDataMng->GetTEquip()->GetDigiviceItem();
		else
			pItemInfo = g_pDataMng->GetTEquip()->GetData( i );
		SAFE_POINTER_CON( pItemInfo );

		cp.s_nFileTableID = pItemInfo->GetType();
		cp.s_nPartIndex = i;
		cp.s_nRemainTime = pItemInfo->m_nEndTime;

		OriginEquipments.insert(Equipments::value_type(i, cp));
		ChangedEquip.insert(Equipments::value_type(i, cp));
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
// 메인 텝의 아이템을 눌렀을 때 해당 아이템이 있는 페이지로 이동하는 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::Move_Item(DWORD const& ProductID)
{
	int nMain = CsCashShopMng::GetLargeType( ProductID );
	int nSub = CsCashShopMng::GetSmallType( ProductID );
	LIST_PRODUCTGROUP_INFO const * pProductInfo = GetCategoryItems( nMain, nSub );
	SAFE_POINTER_RET( pProductInfo );
	
	int nFindSlot = -1;
	DWORD dwSelectGroupID = 0;
	LIST_PRODUCTGROUP_INFO_CIT it = pProductInfo->begin();
	for( int nCount = 0; it != pProductInfo->end(); ++it, ++nCount )
	{
		CASHITEM_TABLE_CIT subIT = (*it).pTableInfo.begin();
		for( ; subIT != (*it).pTableInfo.end(); ++subIT )
		{
			if( ProductID == (*subIT)->dwProductID )
			{
				nFindSlot = nCount;
				dwSelectGroupID = (*it).dwGroupID;
				break;
			}
		}
	}

	if( -1 == nFindSlot )
		return;

	int nPage = (nFindSlot / 9) + 1;
	
	ContentsStream kTmp;
	kTmp << nMain << nSub << nPage << dwSelectGroupID;
	Notify(Move_Page, kTmp);
}

//////////////////////////////////////////////////////////////////////////
// 구매 창을 띄우는 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::OnClick_ShowPurchaseItem(NewCashshopContents::CASHSHOP_FUNTION const& nType, DWORD const& dwProductIDX)
{
	sProductItemInfo const* pCashshopItemInfo = GetCashshop_ItemGropInfo( dwProductIDX );
	SAFE_POINTER_RET( pCashshopItemInfo );

	if( !pCashshopItemInfo->IsPurchaseEnable() )
	{
		cPrintMsg::PrintMsg( 20093 );
		Notify(Refresh_CurrentPage);
		_RefreshBasketEndofSale();// 장바구니에 들어 있는 아이템의 판매 종료 체크
		return; // 판매 종료 상품
	}

	if( !mPurchaseItems.AddPurchaseItem( dwProductIDX ) )
	{	// 서버에서 구매 결과를 받지 못했기 때문에 추가로 데이터를 넣을 수 없다
		return;
	}

	ContentsStream kTmp;
	kTmp << nType;
 	Notify(Show_PurchaseViewer, kTmp);
}

//////////////////////////////////////////////////////////////////////////
// 구매 창 닫는 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::OnClick_ClosePurchaceItem(void)
{
	if( !mPurchaseItems.IsRecvWait() )
		mPurchaseItems.ClearData();
	Notify(Hide_PurchaseViewer);
}

// 캐쉬 창고 창을 연다.
void NewCashshopContents::OnClick_ShowCashStorage()
{
	SAFE_POINTER_RET( g_pGameIF );
// 	bool bShow = g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_CASH_WAREHOUSE );
// 	if( bShow )
// 		return;

	mCashStorageItems.Request_CashWarehouseInfo();
	g_pGameIF->GetDynamicIF(cBaseWindow::WT_NEW_CASH_WAREHOUSE);
}

//////////////////////////////////////////////////////////////////////////
// 구매 창에서 아이템 구매 버튼을 눌렀을 때 패킷을 보내기전 다시 한번 확인 창을 띄우는 함수
//////////////////////////////////////////////////////////////////////////

void NewCashshopContents::OnClick_PurchaseItem(void)
{
	//하나 사는 경우만 처리
	if( mPurchaseItems.GetPurchaseItemCount() != 1 )
		return;

	DWORD PurchaseItemID = mPurchaseItems.GetFirstProductIDX();
	cPrintMsg::PrintMsg( 30312 );
	cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
	pMBox->SetUintValue1( PurchaseItemID );	
}

//////////////////////////////////////////////////////////////////////////
// 장바구니 구매 창에서 아이템 구매 버튼을 눌렀을 때 패킷을 보내기전 다시 한번 확인 창을 띄우는 함수
//////////////////////////////////////////////////////////////////////////

void NewCashshopContents::OnClick_PurchaseAllItem(void)
{
	if( mPurchaseItems.IsEmpty() )
		return;

	if( mPurchaseItems.IsRecvWait() )
	{// 서버에서 구매 결과를 받지 못하였다.
		return;
	}

	DWORD PurchaseItemID = mPurchaseItems.GetFirstProductIDX();
	cPrintMsg::PrintMsg( 30312 );
	cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
	pMBox->SetUintValue1( PurchaseItemID );	
}

//////////////////////////////////////////////////////////////////////////
// 캐쉬 카드에 아이템을 넣을 때 구매창 띄우는 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::OnClick_AddBasket()
{
	if( mPurchaseItems.IsEmpty() )
		return;

	if( mPurchaseItems.IsRecvWait() )
	{// 서버에서 구매 결과를 받지 못하였다.
		return;
	}

	DWORD dwInsertItemIDX = mPurchaseItems.GetFirstProductIDX();
	int nInsertSlotNum = _InsertBasketsItem( dwInsertItemIDX );
	if( nInsertSlotNum < 0 )
		return;

	ContentsStream kTmp;
	kTmp << nInsertSlotNum << dwInsertItemIDX;
	Notify( CashShop_Cart_InsertItem, kTmp );
	
	OnClick_ClosePurchaceItem();
}

//////////////////////////////////////////////////////////////////////////
// 캐쉬 카트에서 아이템 제거 함수
//////////////////////////////////////////////////////////////////////////

bool NewCashshopContents::_RemoveBasketItems(std::list<n4> const& removeItems )
{
	bool bResult = true;
	std::list<n4>::const_iterator it = removeItems.begin();
	for( ; it != removeItems.end(); ++it )
	{
		if( !_RemoveBasketItem(*it) )
			bResult = false;
	}

	return bResult; 
}

bool NewCashshopContents::_IsBasketItemEndOfSale() const
{
	BasketCIter it = mBaskets.begin();
	for( ; it != mBaskets.end(); ++it )
	{
		sProductItemInfo const* pCashshopItemInfo = GetCashshop_ItemGropInfo( it->second );
		SAFE_POINTER_CON( pCashshopItemInfo );
		if( !pCashshopItemInfo->IsPurchaseEnable() )
			return true;
	}
	return false;
}

void NewCashshopContents::_RefreshBasketEndofSale()
{
	BasketCIter it = mBaskets.begin();
	for( ; it != mBaskets.end(); ++it )
	{
		sProductItemInfo const* pCashshopItemInfo = GetCashshop_ItemGropInfo( it->second );
		SAFE_POINTER_CON( pCashshopItemInfo );
		if( !pCashshopItemInfo->IsPurchaseEnable() )
			OnClick_RemoveItemInBasket( it->first, it->second );
	}
}

bool NewCashshopContents::_RemoveBasketItem( DWORD const& dwProductIDX )
{
	int nSlotNum = -1;
	BasketIter it = mBaskets.begin();
	for( ; it != mBaskets.end(); ++it )
	{
		if( it->second == dwProductIDX )
		{
			it->second = 0;
			nSlotNum = it->first;
			break;
		}
	}

	if( -1 == nSlotNum  )
		return false;

	ContentsStream kTmp;
	kTmp << nSlotNum;
	Notify( CashShop_Cart_DeleteItem, kTmp );
	return true;
}

void NewCashshopContents::OnClick_RemoveItemInBasket(int const& nSlotNum,DWORD const& dwProductIDX)
{
	BasketIter it = mBaskets.find( nSlotNum );
	if( it == mBaskets.end() )  
		return;

	if( it->second != dwProductIDX )
		return;

	it->second = 0;
	
	ContentsStream kTmp;
	kTmp << nSlotNum;
	Notify( CashShop_Cart_DeleteItem, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 실제 캐쉬 카트에 아이템을 저장하는 함수
//////////////////////////////////////////////////////////////////////////
int NewCashshopContents::_InsertBasketsItem( DWORD const& dwProductIDX )
{
	BasketIter it = mBaskets.begin();
	for( ; it != mBaskets.end(); ++it )
	{
		if( it->second != 0 )
			continue;

		it->second = dwProductIDX;
		return it->first;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////
// 캐쉬 아이템 선물 할 때 다시 한번 확인 창을 띄우는 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::OnClick_PurchasePresent(std::wstring const& wsPresentTargetName)
{
	//하나 사는 경우만 처리
	if( mPurchaseItems.GetPurchaseItemCount() != 1 )
		return;

	// 선물하려는 친구의 이름이 없다
	if( wsPresentTargetName.empty() )
	{
		cPrintMsg::PrintMsg( 31014 );
		return;
	}

	DWORD dwInsertItemIDX = mPurchaseItems.GetFirstProductIDX();
	cPrintMsg::PrintMsg( 30314,(TCHAR*)wsPresentTargetName.c_str() );
	cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
	if( pMBox )
	{
		pMBox->SetCharValue1( wsPresentTargetName.c_str() );
		pMBox->SetUintValue1( dwInsertItemIDX );	
	}
}

//////////////////////////////////////////////////////////////////////////
// 캐쉬 아이템 구매 창에서 구매하려는 아이템 목록 선택이 변경 됬을 때 변경된 아이템을 알려주는 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::OnChange_PurchaseItem(int IndexInGroup)
{
	//일단 하나 사는 경우만 처리
	if( mPurchaseItems.IsEmpty() )
		return;

	DWORD dwItemProductIDX = mPurchaseItems.GetFirstProductIDX();
	sProductItemInfo const* pGorupInfo = GetCashshop_ItemGropInfo( dwItemProductIDX );
	SAFE_POINTER_RET( pGorupInfo );
	CASHITEM_TABLE_CIT subit = pGorupInfo->pTableInfo.begin();
	for( int nIdx = 0; subit != pGorupInfo->pTableInfo.end(); ++subit, ++nIdx )
	{
		if( nIdx != IndexInGroup )
			continue;

		if( mPurchaseItems.ChangeProductIDX( 0, (*subit)->dwProductID ) )
			Notify(PurchaseViewer_ChangeSelect);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// 카트에 담겨있는 모든 아이템을 구매창에 띄우는 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::OnClick_PurchaseAll_Cart(void)
{
	if( mPurchaseItems.IsRecvWait() )
	{// 서버에서 구매 결과를 받지 못하였다.
		return;
	}

	mPurchaseItems.ClearData();

	if( _IsBasketItemEndOfSale() )
	{
		cPrintMsg::PrintMsg( 20093 );
		Notify(Refresh_CurrentPage);
		_RefreshBasketEndofSale();// 장바구니에 들어 있는 아이템의 판매 종료 체크
		return;
	}

	BasketCIter it = mBaskets.begin();
	for( ; it != mBaskets.end(); ++it )
	{
		if( 0 == it->second )
			continue;

		mPurchaseItems.AddPurchaseItem( it->second );
	}

	if( mPurchaseItems.IsEmpty() )
		return;

	ContentsStream kTmp;
	kTmp << CARTALLPURCHASE;
	Notify(Show_PurchaseViewer, kTmp);
}

/////////////////////////////////////////////////////////////////////////
void NewCashshopContents::OnClick_ChargingCash(void)
{
	std::string cCashChargeUrl = GLOBALDATA_ST.GetCashChargeUrl();
	if(GLOBALDATA_ST.IsCountry( GData::eCountry_Steam ))
	{
#ifdef VERSION_STEAM
		STEAM_ST.SteamCashChageWebOpen();
#endif
	}
	else if( GLOBALDATA_ST.IsCountry( GData::eCountry_GSP ) ||
			 GLOBALDATA_ST.IsCountry( GData::eCountry_Hongkong ) ||
			 GLOBALDATA_ST.IsCountry( GData::eCountry_Taiwan ) )
	{
		//char szEncryption[1024] = {0,};
		//sprintf_s( szEncryption, 256, cCashChargeUrl.c_str(), GLOBALDATA_ST.GetAccountID().c_str(), net::MailAddress, net::cTicketingtime, net::cAccessToken);
		//spawn_browser( szEncryption, 700, 800, NULL );
		spawn_browser( cCashChargeUrl.c_str(), 700, 800, NULL );
	}
	else
	{
		if( !cCashChargeUrl.empty() )
			ShellExecuteA(NULL, "open","iexplore",cCashChargeUrl.c_str(), NULL, SW_SHOWDEFAULT );
	}
}

void NewCashshopContents::OnClick_OpenBanner(std::string const& sOpenUrl)
{
	if( !sOpenUrl.empty() )
		ShellExecuteA(NULL, "open","iexplore",sOpenUrl.c_str(), NULL, SW_SHOWDEFAULT );
}

/////////////////////////////////////////////////////////////////////////
// 캐쉬샵 아이템 구매 패킷을 보냄
/////////////////////////////////////////////////////////////////////////
void NewCashshopContents::Send_CashShopItemPurchase(void* pData)
{
	//SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET(net::game);

 	if( !_SendPurchaseItemAll() )//
 		OnClick_ClosePurchaceItem();
	else
		cPrintMsg::PrintMsg( 14010 );
}

/////////////////////////////////////////////////////////////////////////
// 캐쉬샵 아이템 선물 하기 패킷 보냄
/////////////////////////////////////////////////////////////////////////
void NewCashshopContents::Send_CashShopItemPresent(void* pData)
{
	SAFE_POINTER_RET( pData );
	std::pair<std::wstring, unsigned int> pairData = *static_cast<std::pair<std::wstring, unsigned int>*>(pData);

	if( !_SendPurchaseItem_Present(pairData.first, pairData.second) )//
		OnClick_ClosePurchaceItem();
	else
		cPrintMsg::PrintMsg( 14010 );
}

/////////////////////////////////////////////////////////////////////////
// 아이템 구입에 대한 패킷
/////////////////////////////////////////////////////////////////////////
void NewCashshopContents::Recv_PurchaseItem(void* pkData)
{
	cMessageBox::DelMsg( 14010, false );
	SAFE_POINTER_RET( pkData );
	GS2C_RECV_CASHSHOP_MULTIBUY * pRecv = static_cast<GS2C_RECV_CASHSHOP_MULTIBUY*>(pkData);
	if( nsCashShopResult::Success != pRecv->nResult )
	{
		cPrintMsg::PrintMsg( pRecv->nResult );
		if( pRecv->nResult != nsCashShopResult::PartBuyError )
			return;
	}

	int nTotalCash = pRecv->nRealCash + pRecv->nBonusCash;
	if( GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ) )
		cPrintMsg::PrintMsg( 30318, &nTotalCash );
	else
	{
#ifdef VERSION_STEAM
		cPrintMsg::PrintMsg( 30321 );
#else
		cPrintMsg::PrintMsg( 30313, &nTotalCash );
#endif
	}

	mCashBalance.Recve_CashBalance( pRecv->nRealCash, pRecv->nBonusCash );
	Notify(Update_Cash_Balance);

	OnClick_ClosePurchaceItem();

	_AddCashHistory( pRecv->nSuccessCashItemNo );
	_RemoveBasketItems( pRecv->nSuccessCashItemNo );
	_RePurchaseItems( pRecv->nFailedCashItemNo );
}


void NewCashshopContents::_RePurchaseItems( std::list<n4> const& reItems )
{
	if( reItems.empty() )
		return;

	std::list<n4>::const_iterator it = reItems.begin();
	for( ; it != reItems.end(); ++it )
		mPurchaseItems.AddPurchaseItem( *it );

	ContentsStream kTmp;
	kTmp << REPURCHASE_WINDOW;
	Notify(Show_PurchaseViewer, kTmp);
}

/////////////////////////////////////////////////////////////////////////
// 아이템 선물하기에 대한 패킷
/////////////////////////////////////////////////////////////////////////
void NewCashshopContents::Recv_PresentItem(void* pkData)
{
	SAFE_POINTER_RET( pkData );
	cMessageBox::DelMsg( 14010, false );
	GS2C_RECV_CASHSHOP_GIFT * pRecv = static_cast<GS2C_RECV_CASHSHOP_GIFT*>(pkData);

	if( nsCashShopResult::Success != pRecv->nResult )
	{
		OnClick_ClosePurchaceItem();
		cPrintMsg::PrintMsg( pRecv->nResult );
		return;
	}

	mCashBalance.Recve_CashBalance( pRecv->nRealCash, pRecv->nBonusCash );
	Notify(Update_Cash_Balance);

	_AddCashHistory( pRecv->nGiftItemProductIDX );
	OnClick_ClosePurchaceItem();

	std::wstring wsProductItemName = GetProductItemName(pRecv->nGiftItemProductIDX);
#ifdef VERSION_STEAM
	cPrintMsg::PrintMsg( 31018, (TCHAR*)pRecv->szPeerTamerName.c_str(), (TCHAR*)wsProductItemName.c_str() );	
#else
	int nTotalCash = pRecv->nRealCash + pRecv->nBonusCash;
	cPrintMsg::PrintMsg( 31019, (TCHAR*)pRecv->szPeerTamerName.c_str(), (TCHAR*)wsProductItemName.c_str(), &nTotalCash );	
#endif
}

/////////////////////////////////////////////////////////////////////////
// 아이템 구매 창 닫기
/////////////////////////////////////////////////////////////////////////
void NewCashshopContents::CloseBuyWindow(void* pData)
{
	OnClick_ClosePurchaceItem();
}

void NewCashshopContents::Req_RefreshCashStorage()
{
	mCashStorageItems.Request_CashWarehouseInfo();
}

void NewCashshopContents::Recv_CashStorage(void* pkData)
{
	SAFE_POINTER_RET( pkData );
	GS2C_RECV_CASHITEM_STORAGE* pkRecv = static_cast<GS2C_RECV_CASHITEM_STORAGE*>(pkData);
	
	mCashStorageItems.Recv_CashWarehouseInfo( pkRecv->CashItems );
	Notify(Update_Cash_Storage);// 캐쉬 창고 아이템 목록 갱신
}

void NewCashshopContents::Req_RefrashBalance()
{
	mCashBalance.Request_CashBalance();
}

void NewCashshopContents::Recv_CashBalance(void* pkData)
{
	GS2C_RECV_CASHSHOP_BALANCE* pkRecv = static_cast<GS2C_RECV_CASHSHOP_BALANCE*>(pkData);
	SAFE_POINTER_RET(pkRecv);

	switch( pkRecv->m_nResult )
	{
	case pCashShop::SUCCESS:	break;
	case pCashShop::BILLING_FAIL_BUY_CASHITEM:		pkRecv->m_nResult = nsCashShopResult::BILLING_RESULT_CASHITEM_NOT_PURCHASE;	break;// 캐시 부족 캐시창 안닫힘.
	case pCashShop::BILLING_FAIL_CLOSE_CASHSHOP:	
		{
			pkRecv->m_nResult = nsCashShopResult::BILLING_RESULT_CASHITEM_NOT_PURCHASE;	
			pkRecv->m_bCloseCashShop = true;
		}
		break;// 처리 완료 실패 캐시창 닫힘.
	case pCashShop::BILLING_FAIL_NOT_EXIST_TAMER:	// 테이머를 찾을수 없다.
	case pCashShop::BILLING_FAIL_GIFT_CASHITEM:		//아이템 선물 실패 메시지
	case pCashShop::BILLING_FAIL_BAN_USER:			//유저블록
	default:
		{
			pkRecv->m_nResult = nsCashShopResult::BILLING_RESULT_CASHITEM_NOT_PURCHASE;	
			pkRecv->m_bCloseCashShop = true;
		}
		break;// 처리 완료 실패 캐시창 닫힘.
	}

	if( pCashShop::SUCCESS != pkRecv->m_nResult )// 실패
	{
		cPrintMsg::PrintMsg( pkRecv->m_nResult );	// 처리 완료 실패 캐시창 닫힘.
		if( pkRecv->m_bCloseCashShop )
			_CloseCashshopWindow();
		return;
	}

	mCashBalance.Recve_CashBalance( pkRecv->m_nHaveCash, pkRecv->m_nBonusCash );
	Notify(Update_Cash_Balance);
}

void NewCashshopContents::Clear_SetFilteringKeyword()
{
	SerchingKeyword.clear();
	MAP_MAINCATEGORY_INFO_IT it = mMainCategoryInfos.begin();
	for( ; it != mMainCategoryInfos.end(); ++it )
	{
		if( eMain == it->first )
			continue;
		it->second.SetFileringKeyword( SerchingKeyword );
	}
}

void NewCashshopContents::OnClick_SetFilteringKeyword(std::wstring const& pData)
{
	SerchingKeyword = pData;

	MAP_MAINCATEGORY_INFO_IT it = mMainCategoryInfos.begin();
	for( ; it != mMainCategoryInfos.end(); ++it )
	{
		if( eMain == it->first )
			continue;
		it->second.SetFileringKeyword( SerchingKeyword );
	}

	Notify(Update_FilteringKeyword);
}

void NewCashshopContents::OnClick_TakeOffEquipmentItem(int PartID, int ItemID)
{
	EquipmentIter kIter = ChangedEquip.find(PartID);
	if(kIter == ChangedEquip.end())
		return;

	kIter->second.s_nFileTableID = 0;
	kIter->second.s_nRemainTime = _TIME_TS + 300;
	ContentsStream kStream;
	kStream << kIter->second.s_nPartIndex << kIter->second.s_nFileTableID;
	Notify(Upadte_Equipment, kStream);
}

void NewCashshopContents::OnClick_TakeOffAll(void)
{
	EquipmentIter kIter = ChangedEquip.begin();
	for(; kIter != ChangedEquip.end(); ++kIter)
	{
		kIter->second.s_nFileTableID = 0;
		kIter->second.s_nRemainTime = _TIME_TS + 300;
		ContentsStream kStream;
		kStream << kIter->second.s_nPartIndex << kIter->second.s_nFileTableID;
		Notify(Upadte_Equipment, kStream);
	}
}

void NewCashshopContents::OnClick_ReturnOriginally(void)
{
	EquipmentIter kIter = ChangedEquip.begin();
	for(; kIter != ChangedEquip.end(); ++kIter)
	{
		EquipmentIter kOriginIter = OriginEquipments.find(kIter->second.s_nPartIndex);
		if(kOriginIter != OriginEquipments.end())
		{
			kIter->second.s_nFileTableID = kOriginIter->second.s_nFileTableID;
			kIter->second.s_nRemainTime = kOriginIter->second.s_nRemainTime;
			ContentsStream kStream;
			kStream << kIter->second.s_nPartIndex << kIter->second.s_nFileTableID;
			Notify(Upadte_Equipment, kStream);
		}
	}
}

void NewCashshopContents::OnClick_SellingItem(DWORD const& dwGroupID, DWORD const& dwProductID)
{
	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );

	sCashshop_Item const* pItemInfo = GetProductItemInfo(dwGroupID, dwProductID);
	SAFE_POINTER_RET( pItemInfo );

	LIST_PACKAGE_ITEMS_CIT it = pItemInfo->packageItems.begin();
	for( ; it != pItemInfo->packageItems.end(); ++it )
	{		
		CsItem* pRealItemInfo = nsCsFileTable::g_pItemMng->GetItem( (*it).s_nItemID );
		SAFE_POINTER_BEK( pRealItemInfo );

		CsItem::sINFO* pItemInfo = pRealItemInfo->GetInfo();
		SAFE_POINTER_BEK( pItemInfo );

		switch( pItemInfo->s_nType_L )
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
#ifdef SDM_TAMER_XGUAGE_20180628
		case nItem::XAI:
#endif
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
		case nItem::Bracelet:
#endif
			break;
		default:
			continue;
		}

		int nItemType = pItemInfo->s_nType_L;
		ContentsStream kStream;
		kStream << (nItemType - nItem::Head) << (*it).s_nItemID;
		Notify(Upadte_Equipment, kStream);	
	}
}
//////////////////////////////////////////////////////////////////////////
void NewCashshopContents::CheckCropCashItem( int const& nCashInvenSlotNum, DWORD const& dwItemCode )
{
	cItemInfo* pkItemInfo = mCashStorageItems.GetItemInfo( nCashInvenSlotNum );
	SAFE_POINTER_RET( pkItemInfo );

	if( pkItemInfo->GetType() != dwItemCode )
		return;

	bool bIsInsertSlot = false;
	// 7일 거래 제한이 걸려 있는 아이템인 경우
	if( cData_Inven::IsTradeLimitedItem( pkItemInfo ) )
		bIsInsertSlot = g_pDataMng->GetInven()->IsSlot();
	else if( g_pDataMng->GetInven()->IsSlot( pkItemInfo->GetType(), pkItemInfo->GetCount() ) )// 인벤이 비여있는가 체크
		bIsInsertSlot = true;

	if( !bIsInsertSlot )// 인벤 부족
	{
		cWindow::PlaySound( cWindow::SD_Wa1 );					
		cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
	}
	else
	{
		// 아이템 뺼수 있는가 체크					
		if( nsCsFileTable::g_pItemMng->GetItem( pkItemInfo->GetType() )->GetInfo()->s_nUseTime_Min )
			cPrintMsg::PrintMsg( 20032 );							
		else
			cPrintMsg::PrintMsg( 20031 );
		cMessageBox::GetFirstMessageBox()->SetValue1( nCashInvenSlotNum );		
	}				
}

// 서버에 캐쉬창고에 있는 아이템을 회수 한다고 패킷을 보냄
void NewCashshopContents::Send_CashItemCrop(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nCashInvenSlot = *static_cast<int*>(pData);

	if( mCashStorageItems.Request_CashItemCrop( nCashInvenSlot ) )
		cPrintMsg::PrintMsg( 14010 );		
}

void NewCashshopContents::Recv_CashItemCrop(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_CASHITEM_CROP* pRecv = static_cast<GS2C_RECV_CASHITEM_CROP*>(pData);	
	cMessageBox::DelMsg( 14010, false );

	switch( pRecv->m_nResult )
	{
	case nsCashShopResult::BILLING_RESULT_CROPCASHITEM_SUCCESS: // 아이템 정상 획득
		break;
		//case BILLING_RESULT_CASH_NOT_ENABLE_SLOT:	// 이용 가능한 슬롯이 아닙니다.			
	case nsCashShopResult::BILLING_RESULT_CASH_NOT_SERVER_DATA:	// 서버에 요청한 슬롯 아이템 정보와 실제 아이템 정보가 다릅니다. 갱신 정보 수신
		{
			// 기존 캐쉬샵 목록 갱신과 같이 처리 됩니다.
			cPrintMsg::PrintMsg( pRecv->m_nResult );
			//g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CASH );
			return;
		}
		break;
	case nsCashShopResult::BILLING_RESULT_CASHITEM_NOT_INVENTORY_LIMITE:
		{
			cPrintMsg::PrintMsg( nsCashShopResult::BILLING_RESULT_CASHITEM_NOT_INVENTORY_LIMITE, &pRecv->m_nRTime );
		}
		return;
	}

	// 캐쉬 창고에서 꺼낸 슬롯 제거
	int nRemoveSlotIdx = pRecv->m_nCashWarehouseSlotNum;
	if( mCashStorageItems.RemoveCashWarehouse( nRemoveSlotIdx ) )
	{
		ContentsStream kTmp;
		kTmp << nRemoveSlotIdx;
		Notify(Update_Cash_Storage_Item, kTmp);
	}

	cItemInfo data;
	data.m_nType = pRecv->m_nItemType;
	data.m_nCount = pRecv->m_nItemCount;
	data.m_nRate = pRecv->m_nItemRate;
	data.m_nEndTime = pRecv->m_nItemEndTime;
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	data.m_nRemainTradeLimitTime = pRecv->m_nRemainTradeLimitTime;
#endif

	// 아이템 남아있는 시간 = nItemEndTime - GetTimeTS()
	switch( pRecv->m_nSlotType )
	{
	case nItem::TabInven:
		{
			if( g_pDataMng && g_pDataMng->GetInven() )
				g_pDataMng->GetInven()->ItemCrop( &data );
		}
		break;
	case nItem::TabEquip:
		{
#pragma todo("29가 들어와서 임시 셋팅")
			assert_csm1( pRecv->m_nSlotNo < nTamer::MaxParts, _T( "type > MaxParts, type = %d" ), pRecv->m_nSlotNo );
			int nSlotNo = pRecv->m_nSlotNo > nTamer::MaxParts ? pRecv->m_nSlotNo - 21 : pRecv->m_nSlotNo;
			g_pDataMng->GetTEquip()->SetData( nSlotNo, &data, true );
		}
		break;
	}

	if( g_pDataMng && g_pDataMng->GetQuest() )
		g_pDataMng->GetQuest()->CalProcess();
}

cItemInfo* NewCashshopContents::GetCashStorageItem( int const& nSlotNum )
{
	return mCashStorageItems.GetItemInfo( nSlotNum );
}

DWORD NewCashshopContents::GetTotalPurchasePrice() const
{
	DWORD totlaPrice = 0;
	PurchaseItems const& pList = mPurchaseItems.GetList();
	PurchaseItemConstIter it = pList.begin();
	for( ; it != pList.end(); ++it )
	{
		sCashshop_Item const* pItemInfo = GetProductItemInfo( (*it) );
		SAFE_POINTER_CON( pItemInfo );
		totlaPrice += pItemInfo->nRealSellingPrice;
	}

	return totlaPrice;
}


bool NewCashshopContents::IsHaveEndofSaleItem()
{
	SAFE_POINTER_RETVAL( net::game, false );
	uint nCurTime = net::game->GetTimeTS();

	PurchaseItems const& pList = mPurchaseItems.GetList();
	PurchaseItemConstIter it = pList.begin();
	for( ; it != pList.end(); ++it )
	{
		sProductItemInfo const* pCashshopItemInfo = GetCashshop_ItemGropInfo( (*it) );
		SAFE_POINTER_CON( pCashshopItemInfo );
		if( !pCashshopItemInfo->IsPurchaseEnable() )
			return true;
	}

	return false;
}

bool NewCashshopContents::_SendPurchaseItemAll()
{
	if( IsHaveEndofSaleItem() )
	{
		cPrintMsg::PrintMsg( 20093 );// 판매가 종료된 아이템이 포함되어 있다
		Notify(Refresh_CurrentPage);
		_RefreshBasketEndofSale();// 장바구니에 들어 있는 아이템의 판매 종료 체크		
		return false;
	}

	DWORD dwTotalPurchasePrice = GetTotalPurchasePrice();

#ifndef VERSION_STEAM
	if( GetTotalCash() < dwTotalPurchasePrice )
	{
		cPrintMsg::PrintMsg( 30310 );
		return false;
	}
#endif

	return mPurchaseItems.SendPurchaseItem( dwTotalPurchasePrice );
}

bool NewCashshopContents::_SendPurchaseItem_Present( std::wstring const& wsTamerName, DWORD const& dwPresentItemIDX )
{
	if( !IsMyFriend( wsTamerName )  )
	{
		// 테이머가 친구 목록에 없다.
		return false;
	}

	if( !mPurchaseItems.IsSameProductIDX( dwPresentItemIDX ) )
	{// 보낼려는 아이템 IDX가 다르다.
		return false;
	}

	if( IsHaveEndofSaleItem() )
	{
		cPrintMsg::PrintMsg( 20093 );// 판매가 종료된 아이템이 포함되어 있다
		Notify(Refresh_CurrentPage);
		_RefreshBasketEndofSale();// 장바구니에 들어 있는 아이템의 판매 종료 체크		
		return false;
	}

	DWORD dwTotalPurchasePrice = GetTotalPurchasePrice();

#ifndef VERSION_STEAM
	if( GetTotalCash() < dwTotalPurchasePrice )
	{
		cPrintMsg::PrintMsg( 30310 );
		return false;
	}
#endif

	return mPurchaseItems.SendPresentItem( dwTotalPurchasePrice, wsTamerName );
}

void NewCashshopContents::Recv_RequestCashItemPurchase_Steam(void*pData)
{
	SAFE_POINTER_RET( pData );
	pair<int,int> result = *static_cast<pair<int,int>*>(pData);

	switch( result.first )
	{
	case 0: 
		break;
	case -2:
		{
			cMessageBox::DelMsg( 14010, false );
			cPrintMsg::PrintMsg( 30310 );
			OnClick_ClosePurchaceItem();
		}
		break;
	default:
		{
			cMessageBox::DelMsg( 14010, false );
			cPrintMsg::PrintMsg( 30311 );	// 처리 완료 실패 캐시창 닫힘.
			OnClick_ClosePurchaceItem();
		}break;
	}
}


void NewCashshopContents::Callback_CashItemPurchase_Steam(void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( net::game );

	pair<unsigned char,uint64>	pairResult = *static_cast<pair<unsigned char,uint64>*>(pData);

	if( pairResult.first )
	{
		DWORD dwTotalPurchasePrice = GetTotalPurchasePrice();
		mPurchaseItems.SendPurchaseItem( dwTotalPurchasePrice, pairResult.second );
	}
	else
	{
		cMessageBox::DelMsg( 14010, false );
		OnClick_ClosePurchaceItem();
	}
}

void NewCashshopContents::Recv_Cashshop_Buy_History(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_CASHITEM_BUYHISTORY * recv = static_cast<GS2C_RECV_CASHITEM_BUYHISTORY*>(pData);
	if( recv->wResult == 0 )// 실패
	{
		//cPrintMsg::PrintMsg( recv->wResult );
		mBuyHistory.ClearData();
		Notify(Update_CashItem_Buy_History);
		return;
	}

	_RecvCashHistory( recv->listBuyProductID );
}

void NewCashshopContents::_ShowCashshopItemSelect(DWORD const& dwProductID)
{
	if( 0 == dwProductID )
		return;

	if( _OpenCashshopWindow() )
		Move_Item(dwProductID);
}

void NewCashshopContents::_AddCashHistory( n4 const& nAddProductIDX, bool bNotify )
{
	if( !mBuyHistory.AddHistoryData( nAddProductIDX ) )
		return;

	LIST_PRODUCTGROUP_INFO * pContaner = _GetCategoryItems( m_nHistoryIdx, 1 );
	SAFE_POINTER_RET( pContaner );

	sProductItemInfo const* pCashshopItemInfo = GetCashshop_ItemGropInfo( nAddProductIDX );
	SAFE_POINTER_RET( pCashshopItemInfo );

	sProductItemInfo newItem = *pCashshopItemInfo;
	newItem.ChangeFirstProductIDX( nAddProductIDX );
	pContaner->push_front( newItem );
	if( bNotify )
		Notify(Update_CashItem_Buy_History);
}

void NewCashshopContents::_AddCashHistory( std::list<n4> const& nList )
{
	std::list<n4>::const_iterator itemIT = nList.begin();
	for( ; itemIT != nList.end(); ++itemIT )
		_AddCashHistory( *itemIT, false );

	Notify(Update_CashItem_Buy_History);
}

void NewCashshopContents::_RecvCashHistory( std::list<n4> const& nList )
{
	mBuyHistory.RecvedHistoryData( nList );

	LIST_PRODUCTGROUP_INFO * pContaner = _GetCategoryItems( m_nHistoryIdx, 1 );
	SAFE_POINTER_RET( pContaner );
	pContaner->clear();

	BuyItemHistory const& pList = mBuyHistory.GetList();
	BuyItemHistoryConstIter it = pList.begin();
	for( ; it != pList.end(); ++it )
	{
		sProductItemInfo const* pCashshopItemInfo = GetCashshop_ItemGropInfo( (*it) );
		SAFE_POINTER_CON( pCashshopItemInfo );

		sProductItemInfo newItem = *pCashshopItemInfo;
		newItem.ChangeFirstProductIDX( (*it) );
		pContaner->push_back( newItem );
	}

	Notify(Update_CashItem_Buy_History);
}

void NewCashshopContents::GetAllIncludePackageItemsFromProductIDX( DWORD const& dwProductID, std::wstring & wsProductName, LIST_PACKAGE_ITEMS & listData )
{
	listData.clear();

	sCashshop_Item const* pCashItemInfo = GetProductItemInfo( dwProductID );
	if( pCashItemInfo )
	{
		wsProductName = pCashItemInfo->wsName;
		listData = pCashItemInfo->packageItems;
		return;
	}

	std::map<DWORD,sCashshop_Item const*>::const_iterator it = m_mapInactiveProductItems.find( dwProductID );
	if( it == m_mapInactiveProductItems.end() )
		return;

	SAFE_POINTER_RET( it->second );
	wsProductName = it->second->wsName;
	listData = it->second->packageItems;
}

std::wstring NewCashshopContents::GetProductItemName( DWORD const& dwProductID ) const
{
	sProductItemInfo const* pCashshopItemInfo = GetCashshop_ItemGropInfo( dwProductID );
	SAFE_POINTER_RETVAL( pCashshopItemInfo, L"" );

	return pCashshopItemInfo->wsName;
}

std::wstring NewCashshopContents::GetAllIncludeProductItemName( DWORD const& dwProductID ) const
{
	sProductItemInfo const* pCashshopItemInfo = GetCashshop_ItemGropInfo( dwProductID );
	if( pCashshopItemInfo )
		return pCashshopItemInfo->wsName;

	std::map<DWORD,sCashshop_Item const*>::const_iterator it = m_mapInactiveProductItems.find( dwProductID );
	if( it == m_mapInactiveProductItems.end() )
		return L"";

	SAFE_POINTER_RETVAL( it->second, L"" );
	return it->second->wsName;
}
//////////////////////////////////////////////////////////////////////////
LIST_CASHSHOP_WEBDATA const* NewCashshopContents::GetCashshopWebData()
{
	LIST_CASHSHOP_WEBDATA const* webData = NULL;
	if( GLOBALDATA_ST.IsCountry( GData::eCountry_Steam ) )
		return nsCsFileTable::g_pCashShopMng->GetCashshopWebData(CsCashShopMng::eSteam);
	else
		return nsCsFileTable::g_pCashShopMng->GetCashshopWebData();
}
