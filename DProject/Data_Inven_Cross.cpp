
#include "stdafx.h"
#include "Data_Inven_Cross.h"

#include "_Interface/Adapt/AdaptWareHouse.h"


#ifdef CROSSWARS_SYSTEM
cData_Inven_Cross::cData_Inven_Cross()
{
	memset( m_pInven, 0, sizeof(cItemInfo)*IF_CROSSINVENSLOT_COUNT );	
	m_nInvenSlotCount = IF_CROSSINVENSLOT_COUNT;

	m_pWarehouseAdapt = new AdaptWareHouse;

	if( m_pWarehouseAdapt )
	{
		m_pWarehouseAdapt->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_WAREHOUSE ) );
	}
}
void cData_Inven_Cross::InitNetOff()
{	
	cProfile x("./dmo.ini");

	char key[ 32 ];
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		sprintf_s( key, 32, "%d_CR_IDX", i );
		m_pInven[ i ].m_nType = x.GetInt( "ITEM", key );
		sprintf_s( key, 32, "%d_CR_CNT", i );
		m_pInven[ i ].m_nCount = x.GetInt( "ITEM", key );

		if( m_pInven[ i ].m_nType != 0 )
		{
			m_pInven[ i ].m_nRate = 50;
			m_pInven[ i ].m_nEndTime = 600 + _TIME_TS;
		}		
	}	
	m_bCrossLoader = true;

	g_pGameIF->GetDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTTER_CROSSBUTTON, cBGSprite::CROSSBUTTON );
}

void cData_Inven_Cross::Reset()
{	
	memset( m_pInven, 0, sizeof(cItemInfo)*IF_CROSSINVENSLOT_COUNT );	
	
	m_listLock_ItemID.clear();
	m_listLock_ItemType.clear();
}

void cData_Inven_Cross::LoadApply()
{
	assert_cs( m_nInvenSlotCount > 0 );
}

int cData_Inven_Cross::FindEmptySlot(  )
{
	for( uint i=0; i<m_nInvenSlotCount; ++i )
	{
		if( m_pInven[ i ].IsEnable() == false )
			return i;
	}
	return INVALIDE_INVENCROSS_INDEX;
}

void cData_Inven_Cross::_InsertLock_ItemID( int nItemID )
{
	std::list< int >::iterator it = m_listLock_ItemID.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemID.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nItemID )
			return;
	}
	m_listLock_ItemID.push_back( nItemID );
}

void cData_Inven_Cross::_ReleaseLock_ItemID( int nItemID )
{
	std::list< int >::iterator it = m_listLock_ItemID.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemID.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nItemID )
		{
			m_listLock_ItemID.erase( it );
			return;
		}
	}
	assert_cs( false );
}
bool cData_Inven_Cross::_IsLock_ItemID( int nItemID )
{
	std::list< int >::iterator it = m_listLock_ItemID.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemID.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nItemID )
			return true;
	}
	return false;
}



void cData_Inven_Cross::_InsertLock_ItemType( int nType_L, int nType_S )
{
	int nTotalType = nType_L*100 + nType_S;
	std::list< int >::iterator it = m_listLock_ItemType.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemType.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nTotalType )
			return;
	}
	m_listLock_ItemType.push_back( nTotalType );
}

void cData_Inven_Cross::_ReleaseLock_ItemType( int nType_L, int nType_S )
{
	int nTotalType = nType_L*100 + nType_S;
	std::list< int >::iterator it = m_listLock_ItemType.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemType.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nTotalType )
		{
			m_listLock_ItemType.erase( it );
			return;
		}
	}
}


bool cData_Inven_Cross::_IsLock_ItemType( int nType_L, int nType_S )
{
	int nTotalType = nType_L*100 + nType_S;
	std::list< int >::iterator it = m_listLock_ItemType.begin();
	std::list< int >::iterator itEnd = m_listLock_ItemType.end();
	for( ;it!=itEnd; ++it )
	{
		if( *it == nTotalType )
			return true;
	}
	return false;
}

void cData_Inven_Cross::CrossToCross( int nSrcIndex, int nDestIndex, int nCount )
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
		if( (( pSrcInfo->GetType() != pDestInfo->GetType() )||( pDestInfo->GetCount() == nMaxCnt ) )
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pSrcInfo ) ||	cData_Inven::IsTradeLimitedItem( pDestInfo ) )
#endif
			)
		{
			// 임시보관
			cItemInfo Temp = *pSrcInfo;

			// 인벤토리에 넣어주고
			*pSrcInfo = *pDestInfo;
			// 창고 창에 넣어주자
			*pDestInfo = Temp;
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
		*pDestInfo = *pSrcInfo;
		pDestInfo->m_nCount = nCount;
		pSrcInfo->DecreaseCount( nCount );
	}
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}

void cData_Inven_Cross::CrossToInven( int nSrcIndex, int nDestIndex, int nCount )
{
	assert_cs( nCount > 0 );	
	cItemInfo* pSrcInfo = GetData( nSrcIndex );
	assert_cs( pSrcInfo->IsEnable() == true );
	cItemInfo* pDestInfo = g_pDataMng->GetInven()->GetData( nDestIndex );

	// 목적점이 차 있는가
	if( pDestInfo->IsEnable() == true )
	{
		int nMaxCnt = nsCsFileTable::g_pItemMng->GetItem( pDestInfo->GetType() )->GetInfo()->s_nOverlap;
		// 다른 ID거나 데스트 카운트가 맥스일때 교체
		if( (( pSrcInfo->GetType() != pDestInfo->GetType() )||( pDestInfo->GetCount() == nMaxCnt )  )
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pSrcInfo ) ||	cData_Inven::IsTradeLimitedItem( pDestInfo ) )
#endif
		)
		{
			// 임시보관
			cItemInfo Temp = *pSrcInfo;

			// 인벤토리에 넣어주고
			*pSrcInfo = *pDestInfo;
			// 창고 창에 넣어주자
			*pDestInfo = Temp;
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
		*pDestInfo = *pSrcInfo;
		pDestInfo->m_nCount = nCount;
		pSrcInfo->DecreaseCount( nCount );
	}

#ifdef UI_INVENTORY_RENEWAL
	std::pair< int, bool > pairInfo( nDestIndex, false );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ADD_NEWITEM_INVENTORY, &pairInfo );
#endif
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}

void cData_Inven_Cross::InvenToCross( int nSrcIndex, int nDestIndex, int nCount )
{
	assert_cs( nCount > 0 );	
	cItemInfo* pSrcInfo = g_pDataMng->GetInven()->GetData( nSrcIndex );
	assert_cs( pSrcInfo->IsEnable() == true );
	cItemInfo* pDestInfo = GetData( nDestIndex );

	// 목적점이 차 있는가
	if( pDestInfo->IsEnable() == true )
	{
		int nMaxCnt = nsCsFileTable::g_pItemMng->GetItem( pDestInfo->GetType() )->GetInfo()->s_nOverlap;
		// 다른 ID거나 데스트 카운트가 맥스일때 교체
		if( (( pSrcInfo->GetType() != pDestInfo->GetType() )||( pDestInfo->GetCount() == nMaxCnt ) )
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
			||	(cData_Inven::IsTradeLimitedItem( pSrcInfo ) ||	cData_Inven::IsTradeLimitedItem( pDestInfo ) )
#endif
			)
		{
			// 임시보관
			cItemInfo Temp = *pSrcInfo;

			// 인벤토리에 넣어주고
			*pSrcInfo = *pDestInfo;
			// 창고 창에 넣어주자
			*pDestInfo = Temp;
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
		*pDestInfo = *pSrcInfo;
		pDestInfo->m_nCount = nCount;
		pSrcInfo->DecreaseCount( nCount );
	}
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}
int cData_Inven_Cross::CalItemSplit( int nSrcSrvID, int nDestSrvID, int nSrcCount )
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
			pDest = m_pWarehouseAdapt->GetData( TO_ID(nDestSrvID) );
			assert_cs( pDest );			
		}
		break;
	case SERVER_DATA_SHARESTASH_CONSTANT:
		{
			pDest = m_pWarehouseAdapt->GetShareData(TO_ID(nDestSrvID));
			assert_cs( pDest );			
		}
		break;
	case SERVER_DATA_INVENCROSS_CONSTANT:
		{
			pDest = g_pDataMng->GetInvenCross()->GetData( TO_ID(nDestSrvID) );			
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

void cData_Inven_Cross::SetInvenSlotCount(int nCount)
{
	m_nInvenSlotCount = nCount;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_CROSSINVEN_SIZE, &nCount );
}

void cData_Inven_Cross::SetCrossLoader(int nEnable)
{
	0 != nEnable ? m_bCrossLoader = true : m_bCrossLoader = false;
}

//===============================================================================================
//
//	아이템 습득
//
//===============================================================================================
int cData_Inven_Cross::ItemCrop( int nItemID, int nCount )
{
	cItemData item;
	item.Set( nItemID, nCount );

	CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( nItemID )->GetInfo();
	if( pFTInfo->s_btApplyRateMin == pFTInfo->s_btApplyRateMax )
		item.m_nRate = pFTInfo->s_btApplyRateMax;

	if( ( ( pFTInfo->s_nType_L ) >= nItem::EItem_All )&&
		( ( pFTInfo->s_nType_L ) <= nItem::EItem_Glass ) )
	{
		item.m_nRate = 0;
	}
	if( pFTInfo->IsAccessory() )
		item.m_nRate = 0;

	return ItemCrop( &item );
}

int cData_Inven_Cross::ItemCrop( cItemData* pItemData )
{
	assert_cs( pItemData->GetType() != 0 );
	assert_cs( pItemData->GetCount() != 0 );

	CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pItemData->GetType() )->GetInfo();

	int nRemain = pItemData->GetCount();

	if( pFTInfo->s_nOverlap > 1 )	// 수량처리
	{
		// 기존 수량에 추가
		for( uint i=0; i<m_nInvenSlotCount; ++i )
		{
			assert_cs( nRemain >= 0 );

			// 수량이 0이 되면 중지
			if( nRemain == 0 )
				break;

			// 같은 타입인가
			if( m_pInven[ i ].m_nType != pItemData->GetType() )
				continue;

			// 갯수가 맥스인가		
			if( m_pInven[ i ].m_nCount == pFTInfo->s_nOverlap )
				continue;

			assert_cs( m_pInven[ i ].m_nCount < pFTInfo->s_nOverlap );
			// 채우고 넘치는가
			if( m_pInven[ i ].m_nCount + nRemain > pFTInfo->s_nOverlap )
			{
				nRemain -= ( pFTInfo->s_nOverlap - m_pInven[ i ].m_nCount );
				m_pInven[ i ].m_nCount = pFTInfo->s_nOverlap;
			}
			// 넘치지 않거나 딱 맞으면
			else
			{
				m_pInven[ i ].m_nCount += nRemain;
				nRemain = 0;
			}
		}
	}

	assert_cs( nRemain >= 0 );
	// 합치고 나서 남아 있는 아이템이 존재 하는가
	while( nRemain > 0 )
	{
		int nEmptySlot = FindEmptySlot( );
		if( nEmptySlot == cData_Inven_Cross::INVALIDE_INVENCROSS_INDEX )
			break;

		m_pInven[ nEmptySlot ] = *pItemData;

		if( nRemain <= pFTInfo->s_nOverlap )
		{
			m_pInven[ nEmptySlot ].m_nCount = nRemain;
			nRemain = 0;
		}
		else
		{
			m_pInven[ nEmptySlot ].m_nCount = pFTInfo->s_nOverlap;
			nRemain -= pFTInfo->s_nOverlap;
		}


		// 획득시 귀속이라면
		assert_cs( m_pInven[ nEmptySlot ].m_nLimited == 0 );
		if( pFTInfo->s_nBelonging == 2 )
			m_pInven[ nEmptySlot ].m_nLimited = 1;			

		// 이미 아이템 ID 闡려像繭窄?闡?
		if( _IsLock_ItemID( m_pInven[ nEmptySlot ].m_nType ) == true )
		{
			g_pDataMng->ItemLock( TO_INVEN_SID( nEmptySlot ) );
		}

		// 이미 아이템 Type 闡려像繭窄?闡?
		if( _IsLock_ItemType( pFTInfo->s_nType_L, pFTInfo->s_nType_S ) == true )
		{
			g_pDataMng->ItemLock( TO_INVEN_SID( nEmptySlot ) );
		}
	}

	/*
	// 퀘스트 체크
	// 진행 체크
	g_pDataMng->GetQuest()->CalProcess();
	// 받을수 있는지 체크
	g_pDataMng->GetQuest()->RevQuestCheck();
	// 퀵슬롯 체크
	cData_QuickSlot::CheckItemCount_AllUser();
	*/
	return nRemain;
}
#endif
