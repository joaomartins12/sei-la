
#include "stdafx.h"

#include "cCliGame.h"

#include "../Global.h"
#include "../Engine.h"


void cCliGame::RecvTradeLimitedTimeOut(void)
{
	u1 nSlotType;
	u1 nSlotNo;
//	u1 nTimeoutType;
//	u4 nItemType;

	pop(nSlotType);		// pItem.h ==> nItem::eTab 참조
	pop(nSlotNo);		//

	cItemInfo* pFindItem = NULL;
	switch( nSlotType )
	{
	case nItem::TabInven:
		{
			cData_Inven* pData = g_pDataMng->GetInven();
			if( pData )
				pFindItem = pData->GetData( nSlotNo );
		}break;
	case nItem::TabEquip:
		{
			cData_TEquip* pData = g_pDataMng->GetTEquip();
			if( pData )
				pFindItem = pData->GetData( nSlotNo );

		}break;
	case nItem::TabWarehouse:
		{
			std::pair<int, int> value(0,0);
			value.first = WareHouseContents::WAREHOUSE_NORMAL;
			value.second = nSlotNo;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_TRADELIMITED_ITEM_TIMEOUT, &value);
		}
		break;
	case nItem::TabShareStash:
		{
			std::pair<int, int> value(0,0);
			value.first = WareHouseContents::WAREHOUSE_SHARE;
			value.second = nSlotNo;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_TRADELIMITED_ITEM_TIMEOUT, &value);
		}
		break;
	case nItem::TabCashShop:
		{
			cData_Cash* pData = g_pDataMng->GetCash();
			if( pData )
				pFindItem = pData->GetCashData( nSlotNo );
		}break;
	case nItem::TabChipset:
		{
			cData_Digivice* pData = g_pDataMng->GetDigivice();
			if( pData )
				pFindItem = pData->GetChipset( nSlotNo );

		}break;
	case nItem::TabDigivice:
		{
			cData_TEquip* pData = g_pDataMng->GetTEquip();
			if( pData )
				pFindItem = pData->GetDigiviceItem();
		}break;
// 	case nItem::TabSkill:
// 		break;
// 	case nItem::TabGiftShop:
// 		break;
// 	case nItem::TabPCBang:
// 		break;
// 	case nItem::TabCrossWars:
// 		break;
	}

	if( pFindItem && pFindItem->IsEnable() )
		pFindItem->SetTradeLimitedTimeEnd();
}

void cCliGame::RecvItemTimeout(void)
{
	u1 nSlotType;
	u1 nSlotNo;
	u1 nTimeoutType;
	u4 nItemType;

	pop(nSlotType);		// pItem.h ==> nItem::eTab 참조
	pop(nSlotNo);		//
	pop(nItemType);		// 아이템 테이블 번호
	pop(nTimeoutType);
	
#ifdef TRICK_ORTREAT
	// 시간 다되면...퀘스트 포기
	// 그 아이템 타입이 nItem::TimeSet 이면? 진행중 퀘스트 선행 지급에 해당 아이템이 있는지?  퀘스트 아이디를 얻는다.

	CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( nItemType )->GetInfo();	
	if( 3 == pFTInfo->s_btUseTimeType && (nItem::TimeSet == pFTInfo->s_nType_L) ){
		// 아이템값으로 퀘스트중 수행지급 값과 비교해 퀘스트 아이디를 얻는다.
		//g_pDataMng->GetQuest()->GetProcessIDbyApplyRequiteItem();
		// 퀘스트 포기 1.지급폭탄도 제거 된다. 2.이펙트 폭탄이 터진다. (Tamer 폭탄맞은 상태 애니메이션) 3.아이템 지워준다. 인밴에 있는 사탕. 4. 카운트 지워준다.
		if(g_pDataMng->GetQuest()->IsProcess( pFTInfo->s_nQuest1 ))
		{
			g_pDataMng->GetQuest()->DropProcQuest( pFTInfo->s_nQuest1 );
		}		
		return;	// nItem::TimeSet 은 다이얼로그를 띄우지 않는다.
	}
#endif	
	if( nsCsFileTable::g_pItemMng->IsItem( nItemType ) )
	{
		cPrintMsg::PrintMsg( 20029, nsCsFileTable::g_pItemMng->GetItem( nItemType )->GetInfo()->s_szName );

		cMessageBox::GetFirstMessageBox()->SetValue1( nTimeoutType );	
		cMessageBox::GetFirstMessageBox()->SetValue2( nSlotType );
		cMessageBox::GetFirstMessageBox()->SetValue3( nSlotNo );
	}
}



void cCliGame::RecvItemAdd(void)		// 아이템 추가
{

}


void cCliGame::RecvItemUpdate(void)		// 아이템 정보 수정
{


}


void cCliGame::RecvItemDelete(void)		// 아이템 제거
{


}


void cCliGame::RecvItemSplit(void)		// 아이템 분리
{
	u2 nSrcPos;
	u2 nDstPos;

	pop(nSrcPos);
	pop(nDstPos);	

	g_pDataMng->ServerItemMoveSuccess( nSrcPos, nDstPos );
}


void cCliGame::RecvItemMerge(void)		// 아이템 합하기
{
}


void cCliGame::RecvItemMove(void)		// 아이템 이동
{
	u2 nSrcPos;
	u2 nDstPos;

	pop(nSrcPos);
	pop(nDstPos);

	g_pDataMng->ServerItemMoveSuccess( nSrcPos, nDstPos );
}


void cCliGame::RecvItemExchange(void)	// 아이템 교체
{


}

void cCliGame::RecvRaidItemReward(void)
{
	u1 nItemCount;
	u4 nType;
	u4 nNum;

	pop(nItemCount);	

	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	if( pTamerUser == NULL )
		return;	
	//pTamerUser->ItemCropSound( nItemType );


	//if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_RAID_REWARD ) )
	//	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_RAID_REWARD, true );

	//int	 nitems[] = {97504, 97001,97011,97021,97031,97041};
	// TEST 용 아이템 주울때.
	cBGSprite* pBGSprite = (cBGSprite*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_RAID_REWARD, cBGSprite::RAID_REWARD );
	for(int i =0; i < nItemCount; i++){

		pop(nType);
		pop(nNum);

		cItemData item;
		item.m_nType = nType;
		item.m_nCount = nNum;
		item.m_nRate = 0;
		item.m_nLevel = 0;
		item.m_nLimited = 0;	

		pTamerUser->RaidItemCropSuccess( &item );

		pBGSprite->PushValue1(nType); // 아이템 개수 설정
	}
}

void cCliGame::RecvItemActive()
{
	u2 invenPos;
	u4 itemType;
	u4 endTime;

	pop(invenPos);
	pop(itemType);
	pop(endTime);

	g_pDataMng->GetInven()->SetInvenItemEndTime(invenPos, endTime);

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( itemType );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	short nDstItemPos = TO_TEQUIP_SID( pFTInfo->s_nType_L - nItem::Head ); 

	switch(pFTItem->GetInfo()->s_nType_L)
	{
	case nItem::Head:
	case nItem::Coat:
	case nItem::Glove:
	case nItem::Pants:
	case nItem::Shoes:
	case nItem::Costume:
	case nItem::Glass:
	case nItem::EquipAura:
		{			
			g_pDataMng->SendItemUse( TO_INVEN_SID( invenPos ) );
		}
		break;

	case nItem::PersonStore:
		{
			if(nsCsGBTerrain::g_nSvrLibType ==  nLIB::SVR_DUNGEON)
			{
				cPrintMsg::PrintMsg( 30336 );
			}
			else
			{
				if( pFTInfo->s_nType_S == 1 || 
					pFTInfo->s_nType_S == 3 )
				{
					if( pFTInfo->s_nUseTimeGroup != 0 )
					{
						pFTInfo->GetCoolTimeSeq()->Start();		
					}

					if(pFTInfo->s_nType_S == 1)
					{
						if( net::game )
						{
							net::game->SendTamerShopReady( 1, invenPos );
						}		
					}
					else
					{
						if( net::game )
						{
							net::game->SendTamerShopReady( 4, invenPos );
						}
					}

					g_pDataMng->GetInven()->ItemLock_ItemType( nItem::PersonStore, 1 );
					g_pDataMng->GetInven()->ItemLock_ItemType( nItem::PersonStore, 3 );
				}				
			}
		}
		break;
	case nItem::Portable_Item:
		{
			switch( pFTInfo->s_nType_S )
			{
			case eTATICS_HOUSE:	// 휴대용 디지몬(용병) 보관소
				{
					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TACTICSHOUSE ) == false )
					{
						std::pair<int, NiPoint2> pairData;
						pairData.first = invenPos;
						pairData.second = g_pCharMng->GetTamerUser()->GetPos2D();

						GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_SHOWVIEWER_BT_ITEM, &pairData);
					}
				}
				break;

			case eSCANNER:	// 휴대용 스캐너
				{
					SAFE_POINTER_RET( g_pCharMng );
					CTamerUser* pTamer = g_pCharMng->GetTamerUser();
					SAFE_POINTER_RET( pTamer );
					UINT nWindowType = cBaseWindow::WT_DIGITAMA_SCAN;
					ContentsStream kTmp;
					kTmp << nWindowType << invenPos << pTamer->GetPos2D();
					GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_OPEN_PORTABLE, &kTmp );
				}
				break;

			case ePAY_BACK:	// 휴대용 회수기
				{
					SAFE_POINTER_RET( g_pCharMng );
					CTamerUser* pTamer = g_pCharMng->GetTamerUser();
					SAFE_POINTER_RET( pTamer );
					UINT nWindowType = cBaseWindow::WT_DIGITAMA_SELL;
					ContentsStream kTmp;
					kTmp << nWindowType << invenPos << pTamer->GetPos2D();
					GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_OPEN_PORTABLE, &kTmp );
				}
				break;

			case eHATCH:	// 휴대용 부화기
				if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) == false )
				{
					g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAKE_TACTICS, 0, invenPos);
					g_pGameIF->GetMakeTactics()->SetTarget( NULL, true );
				}
				break;

#ifdef LJW_EQUIPSYSTEM_190724
			case eANALYSOR: // 휴대용 분석기
			case eFUSION:	// 휴대용 합성기
				{
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_EQUIPSYSTEM_PORTABLE, &invenPos );
				}
				break;
#else
			case eANALYSOR:	// 휴대용 분석기
				{
					std::pair< int, int > pairSend( cBaseWindow::WT_ELEMENTITEM_CHECK, invenPos );
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_ELEMENT_ITEM_PORTABLE, &pairSend );
				}
				break;

			case eFUSION:	// 휴대용 합성기
				{
					std::pair< int, int > pairSend( cBaseWindow::WT_ELEMENTITEM_ATTACH, invenPos );
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_ELEMENT_ITEM_PORTABLE, &pairSend );
				}
				break;
#endif
			}
		}
		break;

	case nItem::Chipset:
		{
			int nEmptyChipset = g_pDataMng->GetDigivice()->GetEmptyChipsetSlot();
			if( nEmptyChipset != -1 )
			{
				g_pDataMng->SendItemUse( TO_INVEN_SID( invenPos ) );
			}
			else
			{
				cPrintMsg::PrintMsg( cPrintMsg::CHIPSET_FULL );
			}
		}		
		break;	

	case nItem::Cash_Shouter_N:
	case nItem::Cash_Shouter_S:
	case nItem::Cash_Shouter_NT:
	case nItem::Cash_Shouter_ST:
	case nItem::GM_Shouter_T:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SHOUT_CASH ) == false )
				g_pGameIF->GetDynamicIF( cBaseWindow::WT_SHOUT_CASH, 0, invenPos );
		}
		break;	
	}	
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void cCliGame::Recv_ServerRelocate_CheckTamerName()
{
	GS2C_RECV_CHECK_RELOCATE_NEW_TAMER_NAME recv;
	pop( recv.m_nResult );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_SERVER_RELOCATE_CHECK_TAMERNAME_RESULT, &recv );
}

void cCliGame::Recv_ServerRelocate_ItemUse()
{
	GS2C_RECV_REQUEST_RELOCATE_ITEM_USE recv;
	pop( recv.m_nResult );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_SERVER_RELOCATE_ITEMUSE_RESULT, &recv );
}

void cCliGame::Send_ServerRelocate_CheckTamerName( int const& nServerIDX, std::wstring const& wsTamerName )
{
	C2GS_SEND_CHECK_RELOCATE_NEW_TAMER_NAME packet;
	packet.m_nNewServerIDX = nServerIDX;
	packet.m_nTamerName = wsTamerName;

	newp(packet.GetPacketID());
	push(packet.m_nNewServerIDX);
	push(packet.m_nTamerName);
	endp(packet.GetPacketID());
	send();
}

void cCliGame::Send_ServerRelocate_ItemUse( int const& nInvenPos, int const& nServerIDX, std::wstring const& wsTamerName )
{
	C2GS_SEND_REQUEST_RELOCATE_ITEM_USE packet;

	packet.m_nItemInvenPos = nInvenPos;
	packet.m_nNewServerIDX = nServerIDX;
	packet.m_nTamerName = wsTamerName;

	newp(packet.GetPacketID());
	push(packet.m_nItemInvenPos);
	push(packet.m_nNewServerIDX);
	push(packet.m_nTamerName);
	endp(packet.GetPacketID());
	send();
}

//////////////////////////////////////////////////////////////////////////
// 충전 목록 요청 / 결과
void cCliGame::SendRequestChargeList()
{
#if COMMON_LIB_FIXED
	C2GS_SEND_TimeCharge_ItemList packet;
	newp( packet.GetProtocol() );
	endp( packet.GetProtocol() );
	send();
#endif
}

void cCliGame::RecvChargeListResult()
{
#if COMMON_LIB_FIXED
	GS2C_RECV_TimeCharge_ItemList_Result recv;
	pop( recv.nResult );
	pop( recv.nSlotCnt );
	pop( recv.nItemCnt );
	for( int i = 0; i < recv.nItemCnt; ++i )
	{
		nsTimeCharge::sChargingInfo sCharging;
		pop( sCharging.nSlot );
		pop( sCharging.nTimeLeftTS );
		pop( sCharging.cItem );
		recv.lCharingList.push_back( sCharging );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CHARGE_ITEM_LIST, &recv );
#endif
}

// 아이템 충전 요청 / 결과
// 일반 충전
void cCliGame::SendNormalChargeItem( n2 nSlotPos, n2 nItemPos, n4 nItemType )
{
#if COMMON_LIB_FIXED
	C2GS_SEND_TimeCharge_Normal packet;
	packet.nSlot = nSlotPos;
	packet.nItemPos = nItemPos;
	packet.nItemType = nItemType;

	newp( packet.GetProtocol() );

	push( packet.nSlot );
	push( packet.nItemPos );
	push( packet.nItemType );

	endp( packet.GetProtocol() );
	send();
#endif
}

void cCliGame::RecvNormalChargeItemResult()
{
#if COMMON_LIB_FIXED
	GS2C_RECV_TimeCharge_Normal_Result recv;
	pop( recv.nResult );
	pop( recv.nChargeSlot );
	pop( recv.nDeletedItemPos );
	pop( recv.nDeletedItemType );
	pop( recv.nMoneyLeft );
	pop( recv.cItem );
	pop( recv.nTimeLeft );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CHARGE_ITEM_NORMAL_RESULT, &recv );
#endif
}

// 즉시 충전
#if COMMON_LIB_FIXED
void cCliGame::SendQuickChargeItem( n2 nItemPos, n4 nItemType, n4 nChargeType, n1 nChargeTimeIndex,
								   std::list< nsTimeCharge::sChargeItemInfo >& lChargeItems )
{
	C2GS_SEND_TimeCharge_Immediate packet;
	packet.nItemPos = nItemPos;
	packet.nItemType = nItemType;
	packet.nChargeType = nChargeType;
	packet.nChargeTime = nChargeTimeIndex;
	packet.nItemAllCnt = lChargeItems.size();

	newp( packet.GetProtocol() );

	push( packet.nItemPos );
	push( packet.nItemType );
	push( packet.nChargeType );
	push( packet.nChargeTime );
	push( packet.nItemAllCnt );

	std::list< nsTimeCharge::sChargeItemInfo >::iterator it = lChargeItems.begin();
	std::list< nsTimeCharge::sChargeItemInfo >::iterator itEnd = lChargeItems.end();
	for( ; it != itEnd; ++it )
	{
		push( it->nItemPos );
		push( it->nItemCnt );
	}

	endp( packet.GetProtocol() );
	send();
}
#endif

void cCliGame::RecvQuickChargeItemResult()
{
#if COMMON_LIB_FIXED
	GS2C_RECV_TimeCharge_Immediate_Result recv;
	pop( recv.nResult );
	pop( recv.nChargedItemPos );
	pop( recv.nChargedItemType );
	pop( recv.cItem );
	pop( recv.nMoneyLeft );
	pop( recv.nItemAllCnt );
	for( int i = 0; i < recv.nItemAllCnt; ++i )
	{
		nsTimeCharge::sChargeItemInfo sCItem;
		pop( sCItem.nItemPos );
		pop( sCItem.nItemCnt );
		recv.lChargeItmes.push_back( sCItem );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CHARGE_ITEM_QUICK_RESULT, &recv );
#endif
}

// 아이템 충전 취소 / 결과
void cCliGame::SendChargeCancel( n2 nSlotIndex )
{
#if COMMON_LIB_FIXED
	C2GS_SEND_TimeCharge_Cancel packet;
	packet .nChargeSlot = nSlotIndex;

	newp( packet.GetProtocol() );
	push( packet.nChargeSlot );
	endp( packet.GetProtocol() );
	send();
#endif
}

void cCliGame::RecvChargeCancelResult()
{
#if COMMON_LIB_FIXED
	GS2C_RECV_TimeCharge_Cancel_Result recv;
	pop( recv.nResult );
	pop( recv.nChargeSlot );
	pop( recv.nItemPos );
	pop( recv.cItem );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CHARGE_ITEM_CANCEL, &recv );
#endif
}

// 충전 완료 아이템 회수 / 결과
void cCliGame::SendChargeItemWithdraw( n2 nSlotIndex )
{
#if COMMON_LIB_FIXED
	C2GS_SEND_TimeCharge_Withdraw packet;
	packet.nChargeSlot = nSlotIndex;
	newp( packet.GetProtocol() );
	push( packet.nChargeSlot );
	endp( packet.GetProtocol() );
	send();
#endif
}

void cCliGame::RecvChargeItemWithdrawResult()
{
#if COMMON_LIB_FIXED
	GS2C_RECV_TimeCharge_Withdraw_Result recv;
	pop( recv.nResult );
	pop( recv.nChargeSlot );
	pop( recv.nItemPos );
	pop( recv.cItem );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CHARGE_ITEM_WITHDRAW, &recv );
#endif
}

// 충전 목록 잠금 해제 / 결과
void cCliGame::SendUnlockChargeList( n2 nSlotIndex )
{
#if COMMON_LIB_FIXED
	C2GS_SEND_TimeCharge_SlotOpen packet;
	packet.nOpenSlotPos = nSlotIndex;

	newp( packet.GetProtocol() );
	push( packet.nOpenSlotPos );
	endp( packet.GetProtocol() );
	send();
#endif
}

void cCliGame::RecvUnlockChargeListResult()
{
#if COMMON_LIB_FIXED
	GS2C_RECV_TimeCharge_SlotOpen_Result recv;
	pop( recv.nResult );
	pop( recv.nMoneyLeft );
	pop( recv.nOpenedSlotCount );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CHARGE_ITEM_UNLOCK, &recv );
#endif
}