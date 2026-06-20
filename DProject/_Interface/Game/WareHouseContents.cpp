#include "StdAfx.h"
#include "WareHouseContents.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

namespace
{
void CopyItemInfoFields( cItemInfo& dest, const cItemInfo& src )
{
	dest.m_nAll = src.m_nAll;
	dest.m_nRate = src.m_nRate;
	dest.m_nLevel = src.m_nLevel;
	dest.m_nLimited = src.m_nLimited;
	for( int i = 0; i < nLimit::SocketSlot; ++i )
	{
		dest.m_nSockItemType[i] = src.m_nSockItemType[i];
		dest.m_nSockAppRate[i] = src.m_nSockAppRate[i];
	}
	for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION; ++i )
	{
		dest.m_nAccOption[i] = src.m_nAccOption[i];
		dest.m_nAccValues[i] = src.m_nAccValues[i];
	}
	dest.m_nEndTime = src.m_nEndTime;
	dest.m_nRemainTradeLimitTime = src.m_nRemainTradeLimitTime;
#ifdef COMPAT_487
	dest.ExtraBytes = src.ExtraBytes;
#endif
}

void SwapItemInfoFields( cItemInfo& left, cItemInfo& right )
{
	cItemInfo backup;
	CopyItemInfoFields( backup, left );
	CopyItemInfoFields( left, right );
	CopyItemInfoFields( right, backup );
}
}

WareHouseContents::WareHouseContents(void):m_nSlotCount(0),m_nShareSlotCount(0),m_bFirstConnect(false),m_nSort(0)
{
	m_mapData.clear();
	m_mapShareData.clear();

	m_WareHouseListItem.clear();
	m_ShareStashListItem.clear();

	m_WareHouseSortSeq.SetEndTime( 20.f );
	m_ShareStashSortSeq.SetEndTime( 20.f);

	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_TOGGLE, this, &WareHouseContents::WareHouse_ToggleCheck); 

	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_SET_NORMAL, this, &WareHouseContents::WareHouse_Set_NormalData); 
	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_SET_SHARE, this, &WareHouseContents::WareHouse_Set_ShareData);

	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_ITEMREMOVE_NOSERVER, this, &WareHouseContents::WareHouse_ItemRemoveNoserver);

	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_NORMAL_SLOTCOUNT, this, &WareHouseContents::WareHouse_SetNormalSlotCount); 	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_SHARE_SLOTCOUNT, this, &WareHouseContents::WareHouse_SetShareSlotCount); 	

	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_NORMAL_DATA_DECREASE, this, &WareHouseContents::WareHouse_NormalDecreaseCount);
	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_SHARE_DATA_DECREASE, this, &WareHouseContents::WareHouse_ShareDecreaseCount);

	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_RESET, this, &WareHouseContents::WareHouse_Reset );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_ENDTIME, this, &WareHouseContents::WareHouse_EndTime );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_GET_SLOTCOUNT, this, &WareHouseContents::WareHouse_GetCount);
	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_GET_EVENTTIME, this, &WareHouseContents::WareHouse_GetEventTime);

	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_TRADELIMITED_ITEM_TIMEOUT, this, &WareHouseContents::WareHouse_TradeLimitedItem_Timeout);	
#ifdef WAREHOUSE_SERVER_SORT
	GAME_EVENT_ST.AddEvent( EVENT_CODE::WAREHOUSE_RECV_ITEMSORT_END, this, &WareHouseContents::WareHouse_RecvItemSortEnd);
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_WAREHOUSE_REQ_RESULT,	 this, &WareHouseContents::WareHouse_RecvReqResult );
#endif

	if( EventRouter() )
	{
		EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_Inventory_UseItem, this );
	}
}

WareHouseContents::~WareHouseContents(void)
{
	m_mapData.clear();
	m_WareHouseListItem.clear();

	GAME_EVENT_ST.DeleteEventAll( this );
	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
}

void WareHouseContents::Reset()
{
#ifdef NEW_SHARESTASH
#endif

}

int const WareHouseContents::IsContentsIdentity(void)
{
	return E_CT_WAREHOUSE;
};

int const WareHouseContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool WareHouseContents::Initialize(void)
{
	return true;
}

void WareHouseContents::UnInitialize(void)
{

}

bool WareHouseContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void WareHouseContents::Update(float const& fElapsedTime)
{
	//////////////////////////////////////////////////////////////////////////
	/// 임시 코드
	//////////////////////////////////////////////////////////////////////////
#ifdef NEW_WAREHOUSESORT	
	if( IsWareHouseSort() == true)	// 창고 정렬 기능.
	{
		if( m_NewWareSort.Update() )	// 0.1초마다 1회 정렬 실행.
			_SendWareHouseSort();
	}
#endif

#ifdef NEW_SHARESTASH
	if( IsShareStashSort() == true)	// 공유 창고 정렬 기능.
	{
		if( m_NewShareStashSort.Update() )	// 0.1초마다 1회 정렬 실행.
			_SendShareStashSort();
	}
#endif
}

void WareHouseContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void WareHouseContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_Inventory_UseItem:
		{
			int nInvenIndex = 0;
			kStream >> nInvenIndex;
			InvenToWarehouse( nInvenIndex, -1, 0 );
		}
		break;
	}
}

void WareHouseContents::MakeMainActorData()
{
#ifdef NEW_WAREHOUSESORT
	m_WareHouseSortSeq.Start();
#endif
	m_ShareStashSortSeq.Start();
}

void WareHouseContents::ClearMainActorData()
{
	m_WareHouseListItem.clear();
	SAFE_POINTER_RET(g_pGameIF);

	cBaseWindow* pWin =	g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE );
	if( pWin )
		pWin->SetEnableWindow( true ); // 창고창을 비활성화...
}

void WareHouseContents::MakeWorldData()
{  

}

void WareHouseContents::ClearWorldData()
{ 
	m_WareHouseListItem.clear();
}
 
cItemInfo* WareHouseContents::GetData(int nIdx)
{
	if(nIdx < 0)
		return NULL;

	map<int, cItemInfo>::iterator it = m_mapData.find( nIdx );
	if(it == m_mapData.end())
		return NULL;

	return &(it->second);
} 

cItemInfo* WareHouseContents::GetShareData(int nIdx) 
{
	if(nIdx < 0)
		return NULL;

	map<int, cItemInfo>::iterator it = m_mapShareData.find( nIdx );
	if(it == m_mapShareData.end())
		return NULL;

	return &(it->second);
} 

int WareHouseContents::GetSlotCount() const
{
	return m_nSlotCount;
}

int WareHouseContents::FindEmptySlot()
{
	map<int, cItemInfo>::iterator it ;
	for( int i=0; i<m_nSlotCount; ++i )
	{
		it = m_mapData.find(i);
		if( it->second.IsEnable() == false)
		{
			return i;
		}
	}
	return -1;
}

int WareHouseContents::GetShareSlotCount() const
{
	return m_nShareSlotCount;
}

int WareHouseContents::FindEmptyShareSlot()
{
	map<int, cItemInfo>::iterator it ;
	for( int i=0; i<m_nShareSlotCount; ++i )
	{
		it = m_mapShareData.find(i);
		if( it->second.IsEnable() == false)
		{
			return i;
		}
	}
	return -1;
}

void WareHouseContents::WareHouse_ToggleCheck(void* pData)
{ 
	if(m_bFirstConnect == false)
	{
//		m_WareHouseSortSeq.Start();
		m_bFirstConnect = true;
	}
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_REWARD, 0 ) )					
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_REWARD, 0 );
	else
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_REWARD, 0 );	
}

void WareHouseContents::WareHouse_ItemRemoveNoserver(void* pData)
{
#ifdef TRICK_ORTREAT
	SAFE_POINTER_RET(pData);

	int nItemIDX = *static_cast<int*>(pData);

	map<int, cItemInfo>::iterator it ;

	for( uint i=0; i<m_nSlotCount; ++i )
	{
		it = m_mapData.find(i);

		if( it->second.IsEnable() == false )
			continue;
		if(it->second.m_nType == nItemIDX)
		{
			it->second.Reset();
		}
	}
	// 퀘스트 체크
	g_pDataMng->GetQuest()->CalProcess();
	g_pDataMng->GetQuest()->RevQuestCheck();

	cData_QuickSlot::CheckItemCount_AllUser();
#endif
}

void WareHouseContents::WareHouse_Set_NormalData(void* pData)
{
	SAFE_POINTER_RET(pData);
	if(!m_mapData.empty())
		m_mapData.clear();

	map<int,cItemInfo>* pMapData = (map<int,cItemInfo>*)pData;

	map<int,cItemInfo>::iterator it = pMapData->begin();
	map<int,cItemInfo>::iterator itEnd = pMapData->end();
	
	for(; it != itEnd; ++it)
	{
		int nIdx = it->first;
		cItemInfo stItem = it->second;

		m_mapData.insert(make_pair(nIdx, stItem));
	}
}

void WareHouseContents::WareHouse_Set_ShareData(void* pData)
{
	SAFE_POINTER_RET(pData);
	if(!m_mapShareData.empty())
		m_mapShareData.clear();

	map<int,cItemInfo>* pMapData = (map<int,cItemInfo>*)pData;

	map<int,cItemInfo>::iterator it = pMapData->begin();
	map<int,cItemInfo>::iterator itEnd = pMapData->end();

	for(; it != itEnd; ++it)
	{
		int nIdx = it->first;
		cItemInfo stItem = it->second;

		m_mapShareData.insert(make_pair(nIdx, stItem));
	}
} 

bool WareHouseContents::_DeleteWareHouseItem( map<int,cItemInfo>& mapData, int const& nIdx, int const& nDeleteCount, DWORD const& dwItemCode )
{
	map<int,cItemInfo>::iterator it = mapData.find( nIdx );
	if( it == mapData.end() )
		return false;

	if( 0 != dwItemCode )
	{
		if( it->second.m_nType != dwItemCode )
			return false;
	}

	if( 0 == nDeleteCount )//
		it->second.Reset();
	else
		it->second.DecreaseCount(nDeleteCount);
	return true;
}

void WareHouseContents::WareHouse_GetCount(void* pData)
{
	SAFE_POINTER_RET(pData);
	int* pTemp = static_cast<int*>(pData);
	*pTemp = m_nSlotCount;
}

void WareHouseContents::WareHouse_GetEventTime(void* pData)
{
	SAFE_POINTER_RET(pData);
	int* pTemp = static_cast<int*>(pData);

	map<int, cItemInfo>::iterator it ;

	for( uint i=0; i<m_nSlotCount; ++i )
	{		
		it = m_mapData.find(i);
		if( it->second.IsEnable() )
		{
			CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( it->second.m_nType )->GetInfo();			
			if( 3 == pFTInfo->s_btUseTimeType && (nItem::TimeSet == pFTInfo->s_nType_L) )
			{
				float ftime = (float) it->second.m_nEndTime - _TIME_TS;

				*pTemp = ftime;
//				return ftime;	// 아이템의 남은시간
				//return (float)pFTInfo->s_nUseTime_Min*60;	// 테이블값의 리턴값
			}

		}			
	}
}

void WareHouseContents::WareHouse_SetNormalSlotCount(void* pData) 
{
	SAFE_POINTER_RET(pData);

	u2 nNormalCount = *static_cast<u2*>(pData);
	m_nSlotCount = (int)nNormalCount;
}

void WareHouseContents::WareHouse_SetShareSlotCount(void* pData) 
{
#ifdef NEW_SHARESTASH
	SAFE_POINTER_RET(pData);


	u2 nShareCount = *static_cast<u2*>(pData);
	m_nShareSlotCount = (int)nShareCount;
#endif
}


void WareHouseContents::WareHouse_NormalDecreaseCount(void* pData)
{
	SAFE_POINTER_RET(pData);
	if( m_mapData.empty() )
		return;

	list<sWAREHOUSE>* pListTemp = static_cast<list<sWAREHOUSE>*>(pData);
	list<sWAREHOUSE>::iterator it = pListTemp->begin();
	for( ; it != pListTemp->end(); ++it )
		_DeleteWareHouseItem( m_mapData, (*it).nIdx, (*it).nDecreaseCount, (*it).nItemCode );
}

void WareHouseContents::WareHouse_ShareDecreaseCount(void* pData)
{
	SAFE_POINTER_RET(pData);
	if( m_mapShareData.empty() )
		return;

	list<sWAREHOUSE>* pListTemp = static_cast<list<sWAREHOUSE>*>(pData);
	list<sWAREHOUSE>::iterator it = pListTemp->begin();
	for( ; it != pListTemp->end(); ++it )
		_DeleteWareHouseItem( m_mapShareData, (*it).nIdx, (*it).nDecreaseCount, (*it).nItemCode );
}

void WareHouseContents::WareHouse_Reset(void* pData)
{
	SAFE_POINTER_RET(pData);

	std::pair<int, int>* pTemp = (std::pair<int, int>*)pData;
	SAFE_POINTER_RET(pTemp);

	switch(pTemp->first)
	{
	case WAREHOUSE_NORMAL: // NORMAL,
		SAFE_POINTER_RET(GetData( pTemp->second ) );
		GetData( pTemp->second )->Reset();
		break;
	case WAREHOUSE_SHARE: // SHARE
		GetShareData( pTemp->second )->Reset();
		break;
	}
}

void WareHouseContents::WareHouse_EndTime(void* pData)
{
	SAFE_POINTER_RET(pData);

	std::pair<int, int>* pTemp = (std::pair<int, int>*)pData;
	switch(pTemp->first)
	{
	case WAREHOUSE_NORMAL: // NORMAL,
		GetData( pTemp->second )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
		break;
	case WAREHOUSE_SHARE: // SHARE
		GetShareData( pTemp->second )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
		break;
	}
}

void WareHouseContents::WareHouse_TradeLimitedItem_Timeout(void* pData)
{
	SAFE_POINTER_RET(pData);

	std::pair<int, int>* pTemp = (std::pair<int, int>*)pData;

	cItemInfo* pEndTimeItem = NULL;
	switch(pTemp->first)
	{
	case WAREHOUSE_NORMAL:	pEndTimeItem = GetData( pTemp->second );		break;// NORMAL,
	case WAREHOUSE_SHARE: 	pEndTimeItem = GetShareData( pTemp->second );	break;// SHARE
	}

	if( pEndTimeItem )
		pEndTimeItem->SetTradeLimitedTimeEnd();
}

#ifdef WAREHOUSE_SERVER_SORT
void WareHouseContents::WareHouse_RecvItemSortEnd(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_ITEMSORT* pRecv = static_cast< GS2C_RECV_ITEMSORT* >( pData );

	// 실패 시 메시지 출력
	if( eSORTRESULT::SUCCESS != pRecv->m_nResult )
	{
		cPrintMsg::PrintMsg( pRecv->m_nResult );
	}
	else
	{
		if( nItemSort::eWAREHOUSE == pRecv->m_u1SortFlag )
			_UpdateWareHouse( pRecv->m_lItems, m_mapData.size() );
#ifdef NEW_SHARESTASH
		else
			_UpdateShareStash( pRecv->m_lItems, m_mapShareData.size() );
#endif //NEW_SHARESTASH
	}

	if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_WAREHOUSE ) )
	{
		cBaseWindow* pWin = g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE );
		if( pWin )
			pWin->SetEnableWindow( true );
	}
}

void WareHouseContents::WareHouse_RecvReqResult(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_INVENTORY* pRecv = static_cast< GS2C_RECV_INVENTORY* >( pData );

	if( eINVENTORYRESULT::SUCCESS != pRecv->m_nResult )
	{
		//실패했을때 처리~
	}

	if( nInventoryInfo::eWAREHOUSE == pRecv->m_u1InventoryFlag )
	{
		m_nSlotCount = pRecv->m_u2InventorySlot;
		
		int nSlotCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxWareHouse;
		_UpdateWareHouse( pRecv->m_lItems, nSlotCount );
	}
#ifdef NEW_SHARESTASH
	else
	{
		m_nShareSlotCount = pRecv->m_u2InventorySlot;

		int nSlotCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxShareStash;
		_UpdateShareStash( pRecv->m_lItems, nSlotCount );
	}
#endif //NEW_SHARESTASH
}

void WareHouseContents::_UpdateWareHouse(std::list< cItemData >& lItems, int nSlotCount)
{
	m_mapData.clear();

	std::list<cItemData>::iterator it = lItems.begin();
	std::list<cItemData>::iterator itEnd = lItems.end();
	for( int nIndex = 0; nIndex < nSlotCount; ++nIndex )
	{
		cItemInfo itemInfo;
		if( it != itEnd )
			itemInfo = *it++;
		m_mapData.insert( std::make_pair( nIndex, itemInfo ) );
	}
}

#ifdef NEW_SHARESTASH
void WareHouseContents::_UpdateShareStash(std::list< cItemData >& lItems, int nSlotCount)
{
	m_mapShareData.clear();

	std::list<cItemData>::iterator it = lItems.begin();
	std::list<cItemData>::iterator itEnd = lItems.end();
	for( int nIndex = 0; nIndex < nSlotCount; ++nIndex )
	{
		cItemInfo itemInfo;
		if( it != itEnd )
			itemInfo = *it++;
		m_mapShareData.insert( std::make_pair( nIndex, itemInfo ) );
	}
}
#endif //NEW_SHARESTASH
#endif

#ifdef NEW_SHARESTASH
int WareHouseContents::CalItemSplit( int nSrcSrvID, int nDestSrvID, int nSrcCount )
{
	cItemInfo* pSrc = NULL;
	cItemInfo* pDest = NULL;

	switch( TO_CONSTANT( nDestSrvID ) )
	{
	case SERVER_DATA_INVEN_CONSTANT:
		{			
			pDest = pDest = g_pDataMng->GetInven()->GetData( TO_ID(nDestSrvID) );	
			assert_cs( pDest );			
		}
		break;
	case SERVER_DATA_WAREHOUSE_CONSTANT:
		{			
			pDest = GetData( TO_ID(nDestSrvID) );			
			assert_cs( pDest );			
		}
		break;
	case SERVER_DATA_SHARESTASH_CONSTANT:
		{			
			pDest = GetShareData( TO_ID(nDestSrvID) );			
			assert_cs( pDest );			
		}
		break;
	default:
		assert_cs( false );
	}

	pSrc = GetData( TO_ID(nSrcSrvID) );
	assert_cs( pSrc );

	assert_cs( pSrc->IsEnable() == true );
	// 대상쪽에 아이템이 존재 한다면
	if( pDest->IsEnable() )
	{
		// 다른타입이라면 나누지 않겠다
		if( pSrc->GetType() != pDest->GetType() )
			return 0;

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		// 캐쉬 아이템 거래 제한 시간이 남아있으면 분해 금지
		if( cData_Inven::IsTradeLimitedItem( pSrc ) || cData_Inven::IsTradeLimitedItem( pDest ) )
			return 0;
#endif

		uint nMaxCount = (uint)nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo()->s_nOverlap;

		// 이미 맥스 카운트 라면 실패
		if( pDest->GetCount() == nMaxCount )
			return 0;

		// 합쳐서 맥스치를 넘어 선다면 맥스치까지만 올려 주고 남은거 자리 복귀
		if( nSrcCount + pDest->GetCount() > nMaxCount )
		{
			assert_cs( (uint)nSrcCount > ( nMaxCount - pDest->GetCount() ) );
			return nMaxCount - pDest->GetCount();
		}
		// 맥스치를 안넘는다면 온전히 합체
		return nSrcCount;
	}
	// 대상쪽이 비여 있다면
	return nSrcCount;	
}

int WareHouseContents::CalItemSplit_ShareStash( int nSrcSrvID, int nDestSrvID, int nSrcCount )
{
	cItemInfo* pSrc = NULL;
	cItemInfo* pDest = NULL;

	switch( TO_CONSTANT( nDestSrvID ) )
	{
	case SERVER_DATA_INVEN_CONSTANT:
		{			
			pDest = g_pDataMng->GetInven()->GetData( TO_ID(nDestSrvID) );	
			assert_cs( pDest );			
		}
		break;
	case SERVER_DATA_WAREHOUSE_CONSTANT:
		{			
			pDest = GetData( TO_ID(nDestSrvID) );			
			assert_cs( pDest );			
		}
		break;
	case SERVER_DATA_SHARESTASH_CONSTANT:
		{			
			pDest = GetShareData( TO_ID(nDestSrvID) );			
			assert_cs( pDest );			
		}
		break;
	default:
		assert_cs( false );
	}

	pSrc = GetShareData( TO_ID(nSrcSrvID) );	// cData_Warehouse::CalItemSplit 와 다른점...
	assert_cs( pSrc );

	assert_cs( pSrc->IsEnable() == true );
	// 대상쪽에 아이템이 존재 한다면
	if( pDest->IsEnable() )
	{
		// 다른타입이라면 나누지 않겠다
		if( pSrc->GetType() != pDest->GetType() )
			return 0;

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		// 캐쉬 아이템 거래 제한 시간이 남아있으면 분해 금지
		if( cData_Inven::IsTradeLimitedItem( pSrc ) || cData_Inven::IsTradeLimitedItem( pDest ) )
			return 0;
#endif

		uint nMaxCount = (uint)nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo()->s_nOverlap;

		// 이미 맥스 카운트 라면 실패
		if( pDest->GetCount() == nMaxCount )
			return 0;

		// 합쳐서 맥스치를 넘어 선다면 맥스치까지만 올려 주고 남은거 자리 복귀
		if( nSrcCount + pDest->GetCount() > nMaxCount )
		{
			assert_cs( (uint)nSrcCount > ( nMaxCount - pDest->GetCount() ) );
			return nMaxCount - pDest->GetCount();
		}
		// 맥스치를 안넘는다면 온전히 합체
		return nSrcCount;
	}
	// 대상쪽이 비여 있다면
	return nSrcCount;	
}

#else

int WareHouseContents::CalItemSplit( int nSrcIndex, int nDestIndex, int nSrcCount )
{
	assert_cs( GetData( nSrcIndex )->IsEnable() == true );
	// 대상쪽에 아이템이 존재 한다면
	if( GetData( nDestIndex )->IsEnable() )
	{
		cItemInfo* pSrc = GetData( nSrcIndex );
		cItemInfo* pDest = GetData( nDestIndex );

		// 다른타입이라면 나누지 않겠다
		if( pSrc->GetType() != pDest->GetType() )
			return 0;			

		uint nMaxCount = (uint)nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo()->s_nOverlap;

		// 이미 맥스 카운트 라면 실패
		if( pDest->GetCount() == nMaxCount )
			return 0;

		// 합쳐서 맥스치를 넘어 선다면 맥스치까지만 올려 주고 남은거 자리 복귀
		if( nSrcCount + pDest->GetCount() > nMaxCount )
		{
			assert_cs( (uint)nSrcCount > ( nMaxCount - pDest->GetCount() ) );
			return nMaxCount - pDest->GetCount();
		}
		// 맥스치를 안넘는다면 온전히 합체
		return nSrcCount;
	}
	// 대상쪽이 비여 있다면
	return nSrcCount;
}

#endif


#ifdef NEW_SHARESTASH
//==============================================================================================
//		공유 창고 정리
//==============================================================================================
int WareHouseContents::GetCount_Item_ID2( int nItemID ) const
{
	int nReturn = 0;
	map<int, cItemInfo>::const_iterator it ;

	for( uint i=0; i<m_nShareSlotCount; ++i )
	{
		it = m_mapShareData.find(i);
		if( it->second.IsEnable() == false )
			continue;
		if( it->second.m_nType == nItemID )
			nReturn += it->second.GetCount();
	}
	return nReturn;
}
int WareHouseContents::GetSlot_Item_ID_ReverseToSlot2( int nSlot, int nItemID ) const
{
	map<int, cItemInfo>::const_iterator it ;

	for( int i=(int)m_nShareSlotCount - 1; i>nSlot; --i ) // 현재 모든 창고의 슬롯을 뒤에서부터 돌린다..
	{
		it = m_mapShareData.find(i);
		if( it->second.IsEnable() == false ) // 슬롯에 아이템이 활당 되있나?
			continue;
		if( it->second.m_nType == nItemID ) // 있으면 슬롯의 아이템 인덱스를 리턴한다.
			return i;
	}
	return INVALIDE_WAREHOUSE_INDEX;	// 모두 돌렸는데 없다. 
}
#endif

//==============================================================================================
//		창고 정리
//==============================================================================================
int WareHouseContents::GetCount_Item_ID( int nItemID ) const
{
	int nReturn = 0;
	map<int, cItemInfo>::const_iterator it ;

	for( uint i=0; i<m_nSlotCount; ++i )
	{
		it = m_mapData.find(i);
		if( it->second.IsEnable() == false )
			continue;
		if( it->second.m_nType == nItemID )
			nReturn += it->second.GetCount();
	}
	return nReturn;
}
#ifdef NEW_WAREHOUSESORT
int WareHouseContents::GetSlot_Item_ID_ReverseToSlot( int nSlot, int nItemID ) const
{
	map<int, cItemInfo>::const_iterator it ;

	for( int i=(int)m_nSlotCount - 1; i>nSlot; --i ) // 현제 모든 창고의 슬롯을 뒤에서부터 돌린다..
	{
		it = m_mapData.find(i);
		if( it->second.IsEnable() == false ) // 슬롯에 아이템이 활당 되있나?
			continue;
		if( it->second.m_nType == nItemID ) // 있으면 슬롯의 아이템 인덱스를 리턴한다.
			return i;
	}
	return INVALIDE_WAREHOUSE_INDEX;	// 모두 돌렸는데 없다.
}
#endif

#ifdef TRICK_ORTREAT
void WareHouseContents::ItemRemove_NoServer( int nItemID,  bool bCalQuest )
{
	map<int, cItemInfo>::iterator it ;

	for( uint i=0; i<m_nSlotCount; ++i )
	{
		it = m_mapData.find(i);
		if( it->second.IsEnable() == false )
			continue;
		if(it->second.m_nType == nItemID)
		{
			it->second.Reset();
		}
	}
	// 퀘스트 체크
	if( bCalQuest == true )
	{
		g_pDataMng->GetQuest()->CalProcess();
		g_pDataMng->GetQuest()->RevQuestCheck();
	}	

	cData_QuickSlot::CheckItemCount_AllUser();
}

float WareHouseContents::GetEventTime()
{
	map<int, cItemInfo>::iterator it ;

	for( uint i=0; i<m_nSlotCount; ++i )
	{		
		it = m_mapData.find(i);
		if( it->second.IsEnable() )
		{
			CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( it->second.m_nType )->GetInfo();			
			if( 3 == pFTInfo->s_btUseTimeType && (nItem::TimeSet == pFTInfo->s_nType_L) )
			{
				float ftime = (float) it->second.m_nEndTime - _TIME_TS;

				return ftime;	// 아이템의 남은시간
				//return (float)pFTInfo->s_nUseTime_Min*60;	// 테이블값의 리턴값
			}

		}			
	}
	return -1.0f;
}
#endif


void WareHouseContents::InvenToWarehouse( int nInvenIndex, int nWarehouseIndex, int nCount )
{
	if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_WAREHOUSE ) )
		return;

	// 빈 공간에 옮긴다
	if( nWarehouseIndex == -1 )
	{
		int nIndex = FindEmptySlot();
		// 창고가 가득 찼다
		if( nIndex == -1 )
		{
#ifdef NEW_SHARESTASH			
			int nEmptySlot = FindEmptyShareSlot();
			// 공유 창고가 가득 찼다
			if( nEmptySlot == -1 )
			{
				cPrintMsg::PrintMsg( 11037 );
				return;
			}
			g_pDataMng->SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_SHARESTASH_SID( nEmptySlot ), nCount );
			return;
#else
			cPrintMsg::PrintMsg( 11037 );
			return;
#endif
		}
		g_pDataMng->SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_WAREHOUSE_SID( nIndex ), nCount );
		return;
	}

	assert_cs( nCount > 0 );	
	cItemInfo* pInven = g_pDataMng->GetInven()->GetData( nInvenIndex );
	assert_cs( pInven->IsEnable() == true );
	cItemInfo* pWarehouse = GetData( nWarehouseIndex );

	//// 해당 인벤토리가 비여있을 경우
	//if( pInven->IsEnable() == false )
	//{
	//	assert_cs( pWarehouse->IsEnable() );

	//	// 갯수 체크 - 전부 옴기는 거라면
	//	if( pWarehouse->GetCount() == nCount )
	//	{
	//		// 인벤토리에 넣어주고
	//		*pInven = *pWarehouse;
	//		// 비워준다
	//		pWarehouse->Clear();
	//	}
	//	// 부분 옴기는 거라면
	//	else
	//	{
	//		assert_cs( false );
	//	}
	//}	
	/*else*/

	// 창고가 차있는가
	if( pWarehouse->IsEnable() == true )
	{
		// 다른 ID 면 교체
		if( (pInven->GetType() != pWarehouse->GetType())
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pWarehouse ) ||	cData_Inven::IsTradeLimitedItem( pInven ) )
#endif
		)
		{
			// 임시보관
			SwapItemInfoFields( *pInven, *pWarehouse );
		}
		// 같은 ID면 합체
		else
		{
			assert_cs( pWarehouse->GetCount() + nCount <= nsCsFileTable::g_pItemMng->GetItem( pInven->GetType() )->GetInfo()->s_nOverlap );
			pWarehouse->m_nCount += nCount;
			assert_cs( pInven->m_nCount >= (uint)nCount );
			pInven->m_nCount -= nCount;
			if( pInven->m_nCount == 0 )
				pInven->Clear();
		}
	}
	// 창고가 비여 있을때
	else
	{
		assert_cs( pWarehouse->IsEnable() == false );
		CopyItemInfoFields( *pWarehouse, *pInven );
		pWarehouse->m_nCount = nCount;
		pInven->DecreaseCount( nCount );
	}	
}

#ifdef NEW_SHARESTASH
void WareHouseContents::InvenToShareStash( int nInvenIndex, int nShareIndex, int nCount )
{
	assert_cs( nCount > 0 );	
	cItemInfo* pInven = g_pDataMng->GetInven()->GetData( nInvenIndex );
	assert_cs( pInven->IsEnable() == true );
	cItemInfo* pShare = GetShareData( nShareIndex );	

	// 창고가 차있는가
	if( pShare->IsEnable() == true )
	{
		// 다른 ID 면 교체
		if( pInven->GetType() != pShare->GetType() 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pShare ) ||	cData_Inven::IsTradeLimitedItem( pInven ) )
#endif
			)
		{
			// 임시보관
			SwapItemInfoFields( *pInven, *pShare );
		}
		// 같은 ID면 합체
		else
		{
			assert_cs( pShare->GetCount() + nCount <= nsCsFileTable::g_pItemMng->GetItem( pInven->GetType() )->GetInfo()->s_nOverlap );
			pShare->m_nCount += nCount;
			assert_cs( pInven->m_nCount >= (uint)nCount );
			pInven->m_nCount -= nCount;
			if( pInven->m_nCount == 0 )
				pInven->Clear();
		}
	}
	// 창고가 비여 있을때
	else
	{
		assert_cs( pShare->IsEnable() == false );
		CopyItemInfoFields( *pShare, *pInven );
		pShare->m_nCount = nCount;
		pInven->DecreaseCount( nCount );
	}	
}
#endif

void WareHouseContents::WarehouseToWarehouse( int nSrcIndex, int nDestIndex, int nCount )
{
	assert_cs( nCount > 0 );	
	cItemInfo* pSrcInfo = GetData( nSrcIndex );
	assert_cs( pSrcInfo->IsEnable() == true );
	cItemInfo* pDestInfo = GetData( nDestIndex );

	// 목적점이 차 있는가
	if( pDestInfo->IsEnable() == true )
	{
		int nMaxCnt = nsCsFileTable::g_pItemMng->GetItem( pDestInfo->GetType() )->GetInfo()->s_nOverlap;
		// 다른 ID거나 데스트 카운트가 맥스일때 교체
		if( ( pSrcInfo->GetType() != pDestInfo->GetType() )||( pDestInfo->GetCount() == nMaxCnt ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pDestInfo ) ||	cData_Inven::IsTradeLimitedItem( pSrcInfo ) )
#endif
			)
		{
			// 임시보관
			SwapItemInfoFields( *pSrcInfo, *pDestInfo );
		}
		// 같은 ID면 합체
		else
		{
			// 맥스 카운트를 넘는다면 맥스 카운트 까지만 교체
			if( pDestInfo->GetCount() + nCount > (uint)nMaxCnt )
			{
				assert_cs( pSrcInfo->m_nCount > ( nMaxCnt - pDestInfo->m_nCount ) );
				pSrcInfo->m_nCount -= ( nMaxCnt - pDestInfo->m_nCount );
				pDestInfo->m_nCount = nMaxCnt;
			}
			else
			{
				pDestInfo->m_nCount += nCount;
				pSrcInfo->DecreaseCount( nCount );
			}
		}
	}
	// 창고가 비여 있을때
	else
	{
		CopyItemInfoFields( *pDestInfo, *pSrcInfo );
		pDestInfo->m_nCount = nCount;
		pSrcInfo->DecreaseCount( nCount );
	}
}

void WareHouseContents::WarehouseToInven( int nWarehouseIndex, int nInvenIndex, int nCount )
{
	assert_cs( nCount > 0 );		
	cItemInfo* pWarehouse = GetData( nWarehouseIndex );
	assert_cs( pWarehouse->IsEnable() == true );
	cItemInfo* pInven = g_pDataMng->GetInven()->GetData( nInvenIndex );

	// 해당 인벤토리가 비여있을 경우
	if( pInven->IsEnable() == false )
	{
		CopyItemInfoFields( *pInven, *pWarehouse );
		pInven->m_nCount = nCount;
		pWarehouse->DecreaseCount( nCount );
	}
	// 해당 인벤이 차잇을 경우
	else
	{
		int nMaxCnt = nsCsFileTable::g_pItemMng->GetItem( pInven->GetType() )->GetInfo()->s_nOverlap;
		// 다른 ID거나 맥스 카운트면 교체
		if( ( pInven->GetType() != pWarehouse->GetType() )||( pInven->GetCount() == nMaxCnt ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pWarehouse ) ||	cData_Inven::IsTradeLimitedItem( pInven ) )
#endif
		)
		{
			// 임시보관
			SwapItemInfoFields( *pInven, *pWarehouse );
		}
		// 같은 ID면 합체
		else
		{
			// 맥스 카운트를 넘는다면 맥스 카운트 까지만 교체
			if( pInven->GetCount() + nCount > (uint)nMaxCnt )
			{
				assert_cs( pWarehouse->m_nCount > ( nMaxCnt - pInven->m_nCount ) );
				pWarehouse->m_nCount -= ( nMaxCnt - pInven->m_nCount );
				pInven->m_nCount = nMaxCnt;
			}
			else
			{
				pInven->m_nCount += nCount;
				pWarehouse->DecreaseCount( nCount );
			}
		}
	}	
}

#ifdef NEW_SHARESTASH
void WareHouseContents::WarehouseToShareStash( int nWarehouseIndex, int nShareIndex, int nCount )
{
	assert_cs( nCount > 0 );	
	cItemInfo* pSrcInfo = GetData( nWarehouseIndex );
	assert_cs( pSrcInfo->IsEnable() == true );
	cItemInfo* pDestInfo = GetShareData( nShareIndex );

	// 목적점이 차 있는가
	if( pDestInfo->IsEnable() == true )
	{
		int nMaxCnt = nsCsFileTable::g_pItemMng->GetItem( pDestInfo->GetType() )->GetInfo()->s_nOverlap;
		// 다른 ID거나 데스트 카운트가 맥스일때 교체
		if( ( pSrcInfo->GetType() != pDestInfo->GetType() )||( pDestInfo->GetCount() == nMaxCnt ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pSrcInfo ) ||	cData_Inven::IsTradeLimitedItem( pDestInfo ) )
#endif
		)
		{
			// 임시보관
			SwapItemInfoFields( *pSrcInfo, *pDestInfo );
		}
		// 같은 ID면 합체
		else
		{
			// 맥스 카운트를 넘는다면 맥스 카운트 까지만 교체
			if( pDestInfo->GetCount() + nCount > (uint)nMaxCnt )
			{
				assert_cs( pSrcInfo->m_nCount > ( nMaxCnt - pDestInfo->m_nCount ) );
				pSrcInfo->m_nCount -= ( nMaxCnt - pDestInfo->m_nCount );
				pDestInfo->m_nCount = nMaxCnt;
			}
			else
			{
				pDestInfo->m_nCount += nCount;
				pSrcInfo->DecreaseCount( nCount );
			}
		}
	}
	// 창고가 비여 있을때
	else
	{
		CopyItemInfoFields( *pDestInfo, *pSrcInfo );
		pDestInfo->m_nCount = nCount;
		pSrcInfo->DecreaseCount( nCount );
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////

void WareHouseContents::ShareStashToShareStash( int nSrcIndex, int nDestIndex, int nCount )
{
	assert_cs( nCount > 0 );	
	cItemInfo* pSrcInfo = GetShareData( nSrcIndex );
	assert_cs( pSrcInfo->IsEnable() == true );
	cItemInfo* pDestInfo = GetShareData( nDestIndex );

	// 목적점이 차 있는가
	if( pDestInfo->IsEnable() == true )
	{
		int nMaxCnt = nsCsFileTable::g_pItemMng->GetItem( pDestInfo->GetType() )->GetInfo()->s_nOverlap;
		// 다른 ID거나 데스트 카운트가 맥스일때 교체
		if( ( pSrcInfo->GetType() != pDestInfo->GetType() )||( pDestInfo->GetCount() == nMaxCnt ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pSrcInfo ) ||	cData_Inven::IsTradeLimitedItem( pDestInfo ) )
#endif
		)
		{
			// 임시보관
			SwapItemInfoFields( *pSrcInfo, *pDestInfo );
		}
		// 같은 ID면 합체
		else
		{
			// 맥스 카운트를 넘는다면 맥스 카운트 까지만 교체
			if( pDestInfo->GetCount() + nCount > (uint)nMaxCnt )
			{
				assert_cs( pSrcInfo->m_nCount > ( nMaxCnt - pDestInfo->m_nCount ) );
				pSrcInfo->m_nCount -= ( nMaxCnt - pDestInfo->m_nCount );
				pDestInfo->m_nCount = nMaxCnt;
			}
			else
			{
				pDestInfo->m_nCount += nCount;
				pSrcInfo->DecreaseCount( nCount );
			}
		}
	}
	// 창고가 비여 있을때
	else
	{
		CopyItemInfoFields( *pDestInfo, *pSrcInfo );
		pDestInfo->m_nCount = nCount;
		pSrcInfo->DecreaseCount( nCount );
	}
}

void WareHouseContents::ShareStashToInven( int nShareIndex, int nInvenIndex, int nCount )
{
	assert_cs( nCount > 0 );		
	cItemInfo* pShare = GetShareData( nShareIndex );
	assert_cs( pShare->IsEnable() == true );
	cItemInfo* pInven = g_pDataMng->GetInven()->GetData( nInvenIndex );

	// 해당 인벤토리가 비여있을 경우
	if( pInven->IsEnable() == false )
	{
		CopyItemInfoFields( *pInven, *pShare );
		pInven->m_nCount = nCount;
		pShare->DecreaseCount( nCount );
	}
	// 해당 인벤이 차잇을 경우
	else
	{
		int nMaxCnt = nsCsFileTable::g_pItemMng->GetItem( pInven->GetType() )->GetInfo()->s_nOverlap;
		// 다른 ID거나 맥스 카운트면 교체
		if( ( pInven->GetType() != pShare->GetType() )||( pInven->GetCount() == nMaxCnt ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pShare ) ||	cData_Inven::IsTradeLimitedItem( pInven ) )
#endif
		)
		{
			// 임시보관
			SwapItemInfoFields( *pInven, *pShare );
		}
		// 같은 ID면 합체
		else
		{
			// 맥스 카운트를 넘는다면 맥스 카운트 까지만 교체
			if( pInven->GetCount() + nCount > (uint)nMaxCnt )
			{
				assert_cs( pShare->m_nCount > ( nMaxCnt - pInven->m_nCount ) );
				pShare->m_nCount -= ( nMaxCnt - pInven->m_nCount );
				pInven->m_nCount = nMaxCnt;
			}
			else
			{
				pInven->m_nCount += nCount;
				pShare->DecreaseCount( nCount );
			}
		}
	}	
}


void WareHouseContents::ShareStashToWarehouse( int nShareIndex, int nWarehouseIndex, int nCount )
{
	assert_cs( nCount > 0 );	
	cItemInfo* pSrcInfo = GetShareData( nShareIndex );
	assert_cs( pSrcInfo->IsEnable() == true );
	cItemInfo* pDestInfo = GetData( nWarehouseIndex );

	// 목적점이 차 있는가
	if( pDestInfo->IsEnable() == true )
	{
		int nMaxCnt = nsCsFileTable::g_pItemMng->GetItem( pDestInfo->GetType() )->GetInfo()->s_nOverlap;
		// 다른 ID거나 데스트 카운트가 맥스일때 교체
		if( ( pSrcInfo->GetType() != pDestInfo->GetType() )||( pDestInfo->GetCount() == nMaxCnt ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pSrcInfo ) ||	cData_Inven::IsTradeLimitedItem( pDestInfo ) )
#endif
		)
		{
			// 임시보관
			SwapItemInfoFields( *pSrcInfo, *pDestInfo );
		}
		// 같은 ID면 합체
		else
		{
			// 맥스 카운트를 넘는다면 맥스 카운트 까지만 교체
			if( pDestInfo->GetCount() + nCount > (uint)nMaxCnt )
			{
				assert_cs( pSrcInfo->m_nCount > ( nMaxCnt - pDestInfo->m_nCount ) );
				pSrcInfo->m_nCount -= ( nMaxCnt - pDestInfo->m_nCount );
				pDestInfo->m_nCount = nMaxCnt;
			}
			else
			{
				pDestInfo->m_nCount += nCount;
				pSrcInfo->DecreaseCount( nCount );
			}
		}
	}
	// 창고가 비여 있을때
	else
	{
		CopyItemInfoFields( *pDestInfo, *pSrcInfo );
		pDestInfo->m_nCount = nCount;
		pSrcInfo->DecreaseCount( nCount );
	}
}
#endif

#ifdef NEW_WAREHOUSESORT

void WareHouseContents::WareHouseSort()
{
	if(g_pGameIF->IsActiveWindow( cBaseWindow::WT_WAREHOUSE, 0 ) == false )
		return;

#ifdef WAREHOUSE_SERVER_SORT
	CURSOR_ST.ReleaseIcon();
	cBaseWindow* pWin = g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE );
	if( pWin )
		pWin->SetEnableWindow( false ); // 창고 비활성화
	if( net::game )
		net::game->SendItemSort( nItemSort::eWAREHOUSE );
#else
	if( IsWareHouseSort() == true ) // 소트 리스트에 데이터가 있는가?(소트중이냐?)
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}
#ifndef NEW_SHARESTASH
#ifndef VERSION_HK
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE )->SetEnableWindow( false ); // 창고창을 비활성화...
#endif
#endif
	CURSOR_ST.ReleaseIcon();

	// 총 창고 수만큼
	for( int i = 0; i < m_nSlotCount; i++ )
	{
		cItemInfo* pItemInfo = GetData( i );
		SAFE_POINTER_CON( pItemInfo );

		int nItemIdx = pItemInfo->GetType();	// 아이템이 슬롯에 있으면 GO
		if( nItemIdx == 0 ) 
			continue;

		int nCnt = 0;
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		if( cData_Inven::IsTradeLimitedItem( pItemInfo ) )
		{
			nCnt = 1;
		}
		else
#endif
		{
			bool bFind = false;
			std::list< DWORD >::iterator it = m_WareHouseListItem.begin();	// 아이템 리스트를 돌려본다.
			std::list< DWORD >::iterator itEnd = m_WareHouseListItem.end();
			for( ; it != itEnd; it++ )
			{
				if( *it == nItemIdx )
				{
					bFind = true;
					break;
				}
			}
			if( bFind == true )
				continue;

			int nMaxCount = nsCsFileTable::g_pItemMng->GetItem( nItemIdx )->GetInfo()->s_nOverlap; // 그 아이템의 최대 수량
			nCnt = GetCount_Item_ID( nItemIdx ); // 그아이템의 가지고 있는수량.
			nCnt = nCnt / nMaxCount;	// 나눗셈을 연산
			if( GetCount_Item_ID( nItemIdx ) % nMaxCount > 0 )
				nCnt++;
		}

		for( int n = 0; n < nCnt; n++ )
			m_WareHouseListItem.push_back( GetData( i )->GetType() );							
	}
	m_WareHouseListItem.sort();	// 리스트 소트 인덱스 순서로 정렬
	m_nSort = 0;
#ifdef NEW_SHARESTASH	
	if(m_WareHouseListItem.empty() != true)
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE )->SetEnableWindow( false ); // 창고창을 비활성화...
	}
#endif

	//_SendWareHouseSort();
	m_NewWareSort.Start();
#endif //WAREHOUSE_SERVER_SORT
}

void WareHouseContents::_SendWareHouseSort()
{
	std::list< DWORD >::iterator it = m_WareHouseListItem.begin();	
	// 인벤에 있는 아이템리스트들 돌려서.
	if( m_WareHouseListItem.empty() != true )
	{
		// 겹치기판단....
		int nSrc = _GetMoveWareHouseSlot( *it ); 
		if( nSrc != WareHouseContents::INVALIDE_WAREHOUSE_INDEX )
		{
			// 실제적인 서버 인터페이스 호출..
			int nCode = g_pDataMng->SendItemMove( TO_WAREHOUSE_SID(nSrc), TO_WAREHOUSE_SID(m_nSort) ); // (이동할 슬롯 인덱스 , 검사할 슬롯 인덱스) 
			if( nCode != 0 )
			{
				//_ReleaseWareHouseSort();
				g_CriticalLog.Log( _T( "창고정렬실패 : Src = %d, Dest = %d, Max = %d, ErrCode = %d" ), nSrc, m_nSort, GetSlotCount(), nCode );
			}		
		}
		else
		{
			m_WareHouseListItem.erase( it );			
			_NextWareHouseSort();
		}		
	}
}


int WareHouseContents::_GetMoveWareHouseSlot( DWORD dwItemID )
{
	// 아이템인덱스를 검사해서 슬롯인덱스와 일치하는 인덱스값.
	int nSrc = GetSlot_Item_ID_ReverseToSlot( m_nSort, dwItemID );	// 검사할 슬롯 번호 , 아이템 인덱스 검사해서 몇번 슬롯인덱스인가?		

	// 현재 위치에 겹치기 가능한가?
	cItemInfo* pData = GetData( m_nSort );// 검사할 슬롯 번호의 아이템 정보를 가져온다.
	if( pData->GetType() == dwItemID ) // 아이템 인덱스가 일치하면
	{
		int nMaxCount = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() )->GetInfo()->s_nOverlap; // 현제 아이템의 슬롯에 주어지는 최대수량
		if( pData->GetCount() == nMaxCount 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			|| cData_Inven::IsTradeLimitedItem( pData )
#endif
		)// 한슬롯에 아이템이 만땅있으면..INVALIDE_INVEN_INDEX
			return WareHouseContents::INVALIDE_WAREHOUSE_INDEX;		
	}

	return nSrc;

} 


void WareHouseContents::_NextWareHouseSort()
{	
	if( m_WareHouseListItem.empty() == true ) // 
	{
		_ReleaseWareHouseSort();		
	}
	else
	{   		
		m_nSort++;
		_SendWareHouseSort();
	}
}

void WareHouseContents::_ReleaseWareHouseSort(bool	Check)
{
	m_WareHouseListItem.clear();
	m_nSort = 0;

	if(g_pGameIF->IsActiveWindow( cBaseWindow::WT_WAREHOUSE, 0 ) == true )
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE )->SetEnableWindow( true );
	}

	if( net::game && Check)
		net::game->SendQueryMoneyInven();	// 인밴에 돈을 갱신하는것 같은데...
}

bool WareHouseContents::IsWareHouseSort()
{
	return (m_WareHouseListItem.empty() == false );
}

CsCoolTimeSeq* WareHouseContents::GetWareHouseSortCoolTime()
{
	return &m_WareHouseSortSeq; 
}
#endif


#ifdef NEW_SHARESTASH
void WareHouseContents::ShareStashSort()
{
	if(g_pGameIF->IsActiveWindow( cBaseWindow::WT_WAREHOUSE, 0 ) == false )
		return;

#ifdef WAREHOUSE_SERVER_SORT
	CURSOR_ST.ReleaseIcon();
	cBaseWindow* pWin = g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE );
	if( pWin )
		pWin->SetEnableWindow( false ); // 창고 비활성화
	if( net::game )
		net::game->SendItemSort( nItemSort::eShareStash );
#else
	if( IsShareStashSort() == true ) // 소트 리스트에 데이터가 있는가?(소트중이냐?)
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	CURSOR_ST.ReleaseIcon();

	// 총 창고 수만큼
	for( int i = 0; i < m_nShareSlotCount; i++ )
	{
		cItemInfo* pItemInfo = GetShareData( i );
		SAFE_POINTER_CON(pItemInfo);

		int nItemIdx = pItemInfo->GetType();	// 아이템이 슬롯에 있으면 GO
		if( nItemIdx == 0 ) 
			continue;

		int nCnt = 0;
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		if( cData_Inven::IsTradeLimitedItem( pItemInfo ) )
		{
			nCnt = 1;
		}
		else
#endif
		{
			bool bFind = false;
			std::list< DWORD >::iterator it = m_ShareStashListItem.begin();	// 아이템 리스트를 돌려본다.
			std::list< DWORD >::iterator itEnd = m_ShareStashListItem.end();
			for( ; it != itEnd; it++ )
			{
				if( *it == nItemIdx )
				{
					bFind = true;
					break;
				}
			}
			if( bFind == true )
				continue;

			int nMaxCount = nsCsFileTable::g_pItemMng->GetItem( nItemIdx )->GetInfo()->s_nOverlap; // 그 아이템의 최대 수량
			nCnt = GetCount_Item_ID2( nItemIdx ); // 그아이템의 가지고 있는수량.
			nCnt = nCnt / nMaxCount;	// 나눗셈을 연산
			if( GetCount_Item_ID2( nItemIdx ) % nMaxCount > 0 )
				nCnt++;
		}

		for( int n = 0; n < nCnt; n++ )
			m_ShareStashListItem.push_back( GetShareData( i )->GetType() );							
	}
	m_ShareStashListItem.sort();	// 리스트 소트 인덱스 순서로 정렬
	m_nSort = 0;

	//_SendShareStashSort();
	m_NewShareStashSort.Start();
#endif
}

void WareHouseContents::_SendShareStashSort()
{
	if( !m_ShareStashListItem.empty() )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE )->SetEnableWindow( false ); // 창고창을 비활성화...

	std::list< DWORD >::iterator it = m_ShareStashListItem.begin();	
	// 인벤에 있는 아이템리스트들 돌려서.
	if( m_ShareStashListItem.empty() != true )
	{
		// 겹치기판단....
		int nSrc = _GetMoveShareStashSlot( *it ); 
		if( nSrc != WareHouseContents::INVALIDE_WAREHOUSE_INDEX )	// 쓰래기 값이면..
		{
			// 실제적인 서버 인터페이스 호출..
			int nCode = g_pDataMng->SendItemMove( TO_SHARESTASH_SID(nSrc), TO_SHARESTASH_SID(m_nSort) ); // (이동할 슬롯 인덱스 , 검사할 슬롯 인덱스) 
			if( nCode != 0 )
			{
				//_ReleaseShareStashSort();
				g_CriticalLog.Log( _T( "창고정렬실패 : Src = %d, Dest = %d, Max = %d, ErrCode = %d" ), nSrc, m_nSort, GetShareSlotCount(), nCode );
			}		
		}
		else
		{
			m_ShareStashListItem.erase( it );			
			_NextShareStashSort();
		}		
	}
}

int	 WareHouseContents::_GetMoveShareStashSlot( DWORD dwItemID )
{
	// 아이템인덱스를 검사해서 슬롯인덱스와 일치하는 인덱스값.
	int nSrc = GetSlot_Item_ID_ReverseToSlot2( m_nSort, dwItemID );	// 검사할 슬롯 번호 , 아이템 인덱스 검사해서 몇번 슬롯인덱스인가?		

	// 현재 위치에 겹치기 가능한가?
	cItemInfo* pData = GetShareData( m_nSort );// 검사할 슬롯 번호의 아이템 정보를 가져온다.
	if( pData->GetType() == dwItemID ) // 아이템 인덱스가 일치하면
	{
		int nMaxCount = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() )->GetInfo()->s_nOverlap; // 현제 아이템의 슬롯에 주어지는 최대수량
		if( pData->GetCount() == nMaxCount 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			|| cData_Inven::IsTradeLimitedItem( pData )
#endif
			)// 한슬롯에 아이템이 만땅있으면..INVALIDE_INVEN_INDEX
			return WareHouseContents::INVALIDE_WAREHOUSE_INDEX;		
	}

	return nSrc;
}

void WareHouseContents::_NextShareStashSort()
{
	if( m_ShareStashListItem.empty() == true ) // 
	{
		_ReleaseShareStashSort();		
	}
	else
	{   		
		m_nSort++;
		_SendShareStashSort();
	}
}

void WareHouseContents::_ReleaseShareStashSort(bool Check)
{
	m_ShareStashListItem.clear();
	m_nSort = 0;

	if(g_pGameIF->IsActiveWindow( cBaseWindow::WT_WAREHOUSE, 0 ) == true )
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE )->SetEnableWindow( true );
	}

	if( net::game && Check)
		net::game->SendQueryMoneyInven();	// 인밴에 돈을 갱신하는것 같은데...
}

bool WareHouseContents::IsShareStashSort()
{
	return (m_ShareStashListItem.empty() == false ); 
}


CsCoolTimeSeq* WareHouseContents::GetShareStashSortCoolTime()
{
	return &m_ShareStashSortSeq; 
}
#endif
