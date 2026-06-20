

#include "stdafx.h"

#include "cNetwork.h"
#include "cCliGame.h"

#include "../TamerUser.h"

#include "common_vs2019/pItem.h"
#include "common_vs2019/pCashShop.h"

#include "common_vs2019/pPass2.h"
#include "common_vs2019/pParty.h"


void cCliGame::SendBuy( u4 NpcFTID, n4 ShopSlotIndex, u2 ItemCount )
{
#define d_nItemCount 312
	//uint nItemCnt = 0;

	newp(pItem::Buy);
	{
		push((u4)NpcFTID);

		uint8_t m = d_nItemCount & 0xFF;
		mark(m); // 1byte marking

		assert_cs( ItemCount > 0 );
		push((n4)ShopSlotIndex); // shop item slot index
		push((u2)ItemCount);

		push(m);
	}
	endp(pItem::Buy);
	send();
#undef d_nItemCount
}


void cCliGame::SendPropertyBuy( u4 NpcFTID, n4 ShopSlotIndex, u2 ItemCount, u2 ItemIndex )
{
#define d_nItemCount 312
	//uint nItemCnt = 0;

	newp(pItem::Buy);
	{
		push((u4)NpcFTID);

		uint8_t m = d_nItemCount & 0xFF;
		mark(m); // 1byte marking

		assert_cs( ItemCount > 0 );
		push((n4)ShopSlotIndex); // shop item slot index
		push((u2)ItemCount);

		push(m);
		push((u2)ItemIndex);
	}
	endp(pItem::Buy);
	send();
#undef d_nItemCount
}

void cCliGame::SendSell( u4 NpcFTID, u1 InvenSlotIndex, u2 ItemCount )
{
#define d_nItemCount 312
	newp(pItem::Sell);
	{
		uint8_t m = d_nItemCount & 0xFF;

		push((u4) NpcFTID );
		mark(m); // 1byte marking

		assert_cs( ItemCount != 0)
		push((u1) InvenSlotIndex ); // inventory slot index process
		push((u2) ItemCount );

		push(m);
	}
	endp(pItem::Sell);
	send();
#undef d_nItemCount
}


void cCliGame::RecvItemBuy(void)
{
	//uchar nBuyItemCnt;

	//ushort nSlotNo;
	//uint nItemType;
	//uchar nItemCount; // nItemType의 아이템 갯수

	//uint nMoney;

	//pop(nBuyItemCnt);
	//for(int i=0; i<nBuyItemCnt; i++)
	//{
	//	pop(nSlotNo);
	//	pop(nItemType);
	//	pop(nItemCount);
	//	net::chat("Buy(%d) slot:%d type:%d cnt:%d", i, nSlotNo, nItemType, nItemCount);
	//	g_PlayerUser.m_pInven[ nSlotNo ].m_nType = nItemType;
	//	g_PlayerUser.m_pInven[ nSlotNo ].m_nCount = nItemCount;
	//}

	//pop(nMoney);
	//g_PlayerUser.m_nMoney = nMoney;

	//net::chat("Remained Money : %d", nMoney);
}


void cCliGame::RecvItemSell(void)
{
	assert_csm( false, _T( "이제 이거 안써도 될거 같아욤~!!" ) );
	n8 money;
	pop( money );
}


void cCliGame::RecvItemCropSuccess(void) // crop success
{
	u4 nTamerIDX = 0;	// 아이템이 지급되는 테이머 인덱스
	u4 nItemType;
	u2 nCount;
	u1 nRate;
	u4 nPickUpTamerIDX = 0;// 아이템을 줍는 테이머 인덱스

	pop(nTamerIDX);
	pop(nItemType);
	pop(nCount);
	pop(nRate);
	pop(nPickUpTamerIDX);

	cItemData item;
	item.m_nType = nItemType;
	item.m_nCount = nCount;
	item.m_nRate = nRate;
	item.m_nLevel = 0;
	item.m_nLimited = 0;

	if( nPickUpTamerIDX == 0 )
		nPickUpTamerIDX = nTamerIDX;

	// 줍는 애니메이션 수행
	CTamer* pPickupTamer = g_pCharMng->GetTamer( nPickUpTamerIDX );
	if( pPickupTamer )
		pPickupTamer->ItemPickUpAnimation();

	CTamer* pItemGetTamer = g_pCharMng->GetTamer( nTamerIDX );
	if( pItemGetTamer )
	{
		pItemGetTamer->PickUpItemGotoInven(&item);
		pItemGetTamer->ItemCropSound( nItemType );
	}
}



void cCliGame::RecvCropGiftItemSuccess(void)
{
	u1 nSlotType;	// pItem.h ==> nItem::eTab 참조
	u1 nSlotNo;

	u4 nItemType;
	u2 nItemCount;
	u1 nItemRate;
	u4 nItemEndTime;

	u2 nDeleteidx;

	g_pDataMng->GetQuest()->CalProcess();

	pop(nSlotType);
	pop(nSlotNo);

	pop(nItemType);
	pop(nItemCount);
	pop(nItemRate);
	pop(nItemEndTime);

	// 아이템 남아있는 시간 = nItemEndTime - GetTimeTS()
	switch( nSlotType )
	{
	case nItem::TabInven:
		{
			g_pDataMng->GetQuest()->CalProcess();

			cItemData data;
			data.m_nType = nItemType;
			data.m_nCount = nItemCount;
			data.m_nRate = nItemRate;
			data.m_nEndTime = nItemEndTime;
			g_pDataMng->GetInven()->ItemCrop( &data );
			g_pDataMng->GetInven()->OutputCropMsg( &data ); // [3/24/2016 hyun] 메세지 출력
		}
		break;
	case nItem::TabEquip:
		{
			g_pDataMng->GetQuest()->CalProcess();

			cItemInfo info;
			info.m_nType = nItemType;
			info.m_nCount = nItemCount;
			info.m_nRate = nItemRate;
			info.m_nEndTime = nItemEndTime;
#pragma todo("29가 들어와서 임시 셋팅")
			assert_csm1( nSlotNo < nTamer::MaxParts, _T( "type > MaxParts, type = %d" ), nSlotNo );
			nSlotNo = nSlotNo > nTamer::MaxParts ? nSlotNo - 21 : nSlotNo;
			g_pDataMng->GetTEquip()->SetData( nSlotNo, &info, true );
		}
		break;
	default:
		assert_cs( false );
	}

	//보상 지급 UI 에서 받은 아이템 의 목록을 삭제

	pop(nDeleteidx);
	
	// 아이템 빼자
	//cItemInfo* pPost = g_pDataMng->GetCash()->GetPostData( nDeleteidx );	if(pPost)pPost->Reset();
//	g_pDataMng->GetCash()->DeletePostItem( nDeleteidx );

	GS2C_RECV_REWARD_SEND_TYPE recv;
	recv.iType = 2;
	recv.iIndex = nDeleteidx;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::REWARD_SEND_ACCPET_DATA, &recv );

	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::GET_REWARD, nDeleteidx );
	if(true == g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::GET_REWARD ) )
	{
		net::game->SetSendBlock(false);
	}

}

// 선물 지급
void cCliGame::RecvGiftShop(void)
{
	cItemData* pItemData = NULL;

	n2 nGiftShopSlotNumber = 0;
	pop(nGiftShopSlotNumber);

	pItemData = new cItemData[nGiftShopSlotNumber];
	pop(pItemData, nGiftShopSlotNumber * sizeof(cItemData));

	//DBG("type:%d count:%d", pItemData[0].m_nType, pItemData[0].m_nCount);

	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::POSTAL_REFRESH, 0, &nGiftShopSlotNumber, pItemData );

	SAFE_DELETE_ARRAY(pItemData);
}

void cCliGame::SendReqGiftShop(void)
{
	newp(pItem::GiftShop);
	endp(pItem::GiftShop);
	send();
}


// 캐쉬 창고에서 아이템을 꺼낼때 패킷
void cCliGame::RecvCropCashItemSuccess(void)
{
	GS2C_RECV_CASHITEM_CROP recv;
	pop( recv.m_nResult );	// 결과
	pop( recv.m_nRTime );//남은 시간(초)
	pop( recv.m_nCashWarehouseSlotNum);// 캐쉬 창고에서 제거될 슬롯 번호

	// 아이템이 들어갈 인벤토리 종류
	pop(recv.m_nSlotType);// pItem.h ==> nItem::eTab 참조
	pop(recv.m_nSlotNo);	// 인벤토리 슬롯 번호
	pop(recv.m_nItemType);
	pop(recv.m_nItemCount);
	pop(recv.m_nItemRate);
	pop(recv.m_nItemEndTime);

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	pop(recv.m_nRemainTradeLimitTime);
#endif

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CASHSHOP_ITEM_CROP, &recv );

	//캐시 아이템 환불과 관련하여 처리 방법이 변경 되었습니다.
// 	n4 nResult = 0;
// 	pop( nResult );	// 결과
// 
// 	n4 nRTime = 0;//남은 시간(초)
// 	pop( nRTime );
// 
// 	// 캐쉬 창고에서 제거될 슬롯 번호
// 	u2 nCashWarehouseSlotNum = 0;
// 	pop(nCashWarehouseSlotNum);
// 
// 	switch( nResult )
// 	{
// 	case BILLING_RESULT_CROPCASHITEM_SUCCESS: // 아이템 정상 획득
// 		break;
// 		//case BILLING_RESULT_CASH_NOT_ENABLE_SLOT:	// 이용 가능한 슬롯이 아닙니다.			
// 	case BILLING_RESULT_CASH_NOT_SERVER_DATA:	// 서버에 요청한 슬롯 아이템 정보와 실제 아이템 정보가 다릅니다. 갱신 정보 수신
// 		{
// 			// 기존 캐쉬샵 목록 갱신과 같이 처리 됩니다.
// 			cPrintMsg::PrintMsg( nResult );
// 			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CASH );
// 			return;
// 		}
// 		break;
// 	case BILLING_RESULT_CASHITEM_NOT_INVENTORY_LIMITE:
// 		{
// 			cPrintMsg::PrintMsg( BILLING_RESULT_CASHITEM_NOT_INVENTORY_LIMITE, &nRTime );
// 		}
// 		return;
// 	}
// 
// 	g_pDataMng->GetQuest()->CalProcess();
// 
// 	g_pDataMng->GetCash()->RemoveCashItem( nCashWarehouseSlotNum );
// 
// 	u1 nSlotType;	// pItem.h ==> nItem::eTab 참조
// 	pop(nSlotType);
// 	u1 nSlotNo;
// 	pop(nSlotNo);
// 
// 	u4 nItemType;
// 	pop(nItemType);
// 	u4 nItemCount;
// 	pop(nItemCount);
// 	u1 nItemRate;
// 	pop(nItemRate);
// 	u4 nItemEndTime;
// 	pop(nItemEndTime);
// 
// #ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
// 	u4 nRemainTradeLimitTime;
// 	pop(nRemainTradeLimitTime);
// #endif
// 	// 아이템 남아있는 시간 = nItemEndTime - GetTimeTS()
// 	switch( nSlotType )
// 	{
// 	case nItem::TabInven:
// 		{
// 			cItemData data;
// 			data.m_nType = nItemType;
// 			data.m_nCount = nItemCount;
// 			data.m_nRate = nItemRate;
// 			data.m_nEndTime = nItemEndTime;
// #ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
// 			data.m_nRemainTradeLimitTime = nRemainTradeLimitTime;
// #endif
// 			g_pDataMng->GetInven()->ItemCrop( &data );
// 
// 			g_pDataMng->GetQuest()->CalProcess();
// 		}
// 		break;
// 	case nItem::TabEquip:
// 		{
// 			cItemInfo info;
// 			info.m_nType = nItemType;
// 			info.m_nCount = nItemCount;
// 			info.m_nRate = nItemRate;
// 			info.m_nEndTime = nItemEndTime;
// #ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
// 			info.m_nRemainTradeLimitTime = nRemainTradeLimitTime;
// #endif
// #pragma todo("29가 들어와서 임시 셋팅")
// 			assert_csm1( nSlotNo < nTamer::MaxParts, _T( "type > MaxParts, type = %d" ), nSlotNo );
// 			nSlotNo = nSlotNo > nTamer::MaxParts ? nSlotNo - 21 : nSlotNo;
// 			g_pDataMng->GetTEquip()->SetData( nSlotNo, &info, true );
// 
// 			g_pDataMng->GetQuest()->CalProcess();
// 		}
// 		break;
// 	default:
// 		assert_cs( false );
// 	}
}


void cCliGame::SendCashShopCart(void)	// 장바구니 목록 요청
{
	assert_csm( false, L"SendCashShopCart : 장바 구니 사용 안됨" );

	newp(pCashShop::Cart);
	endp(pCashShop::Cart);
	DoSend();
}


void cCliGame::RecvCashShopCart(void)	// 장바구니 목록 수신
{
	assert_csm( false, L"RecvCashShopCart : 장바 구니 사용 안됨" );

	n4 nCart[12];
	pop(nCart, sizeof(nCart));
}


void cCliGame::SendCashShopCartSave(int *nItemTypes)	// 장바구니 목록 저장 요청
{
	assert_csm( false, L"SendCashShopCartSave : 장바 구니 사용 안됨" );

	newp(pCashShop::CartSave);
		push(nItemTypes, sizeof(int)*12);
	endp(pCashShop::CartSave);
	DoSend();
}


void cCliGame::RecvCashShop(void)
{
	n2 nCashShopSlotNumber = 0;
	cItemData pItemData[nLimit::CashShop];

	pop(nCashShopSlotNumber);
	if(nCashShopSlotNumber > 0)
	{
		pop(pItemData, nCashShopSlotNumber * sizeof(cItemData));
	}

	GS2C_RECV_CASHITEM_STORAGE kRecv(nCashShopSlotNumber);
	for(int i =0; i < nCashShopSlotNumber; ++i)
	{
		kRecv.AddCashItem( pItemData[i] );
	}
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CASHSHOP_STORAGE, &kRecv );

	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_REFRESH, 0, &nCashShopSlotNumber, pItemData );
}


void cCliGame::SendReqCashShop(void)	// 캐시 창고 갱신 요청
{
	newp(pItem::CashShop);
	endp(pItem::CashShop);
	send();
}



void cCliGame::SendCashShopBalance(void)
{
	cPacket p;
	p.newp(pCashShop::Balance);
// #ifdef VERSION_USA
// 	if(net::IsAeria)
// 	{
// 		char szAuthCode[512] = {0,};
// 		strcpy(szAuthCode,net::cAccessToken);
// 		p.push(szAuthCode);//캐쉬샵 열 때 코드한번 보내주자
// 	}
// #endif
	p.endp(pCashShop::Balance);
	DoSend(p);
}


void cCliGame::RecvCashShopBalance(void)
{
	int nResult = 0;

	GS2C_RECV_CASHSHOP_BALANCE recv;
	pop(recv.m_nResult);
	pop(recv.m_nBonusCash);
	pop(recv.m_nHaveCash);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_RESULT_HAVE_CASH_REFRESH, &recv );

// #ifdef VERSION_USA
// 	if(net::IsAeria)
// 	{
// 		char cAuth_Code[MAX_ANAL_LEN]= {0, };
// 		pop(cAuth_Code); //Aeria 인증코드 받기
// 		strcpy(net::cAccessToken, cAuth_Code);
// 	}
// #endif
}

void cCliGame::SendCashShopBuyHistory(void)
{
	C2GS_SEND_CASHITEM_BUYHISTORY packet;
	cPacket p;
	p.newp(packet.GetProtocol());
	p.endp(packet.GetProtocol());
	DoSend(p);
}

void cCliGame::RecvCashshopBuyhistory(void)
{
	GS2C_RECV_CASHITEM_BUYHISTORY recv;
	n1 nResult;
	pop( nResult );
	//pop( recv.wResult );
	recv.wResult = nResult;
	n2 nCount = 0;
	pop( nCount );
	for( int n = 0; n < nCount; ++n )
	{
		n4 nProductItemID = 0;
		pop( nProductItemID );
		recv.listBuyProductID.push_back( nProductItemID );
	}
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CASHSHOP_BUY_HISTORY, &recv );
}

void cCliGame::SendGiftCashItem(n4 const& nPrice, std::list<DWORD> const& nItemNo, std::wstring const& szPeerTamerName, uint64 ui64OrderID )
{
	n4 nProductIDX = nItemNo.front();
	C2GS_SEND_CASHSHOP_GIFT packet;
	packet.nProductIDX = nProductIDX;
	packet.nPrice = nPrice;
	packet.szPeerTamerName = szPeerTamerName;
	cPacket p;
	p.newp(packet.GetProtocol());
		p.push(packet.nPrice);
		p.push(packet.nProductIDX);
		p.push(packet.szPeerTamerName);
		p.push(packet.GetProtocol());
	p.endp(packet.GetProtocol());
	DoSend(p);
}

void cCliGame::RecvGiftCashItem(void)
{
	GS2C_RECV_CASHSHOP_GIFT recv;
	pop(recv.nResult);
	pop(recv.nRealCash);
	pop(recv.nBonusCash);
	pop(recv.szPeerTamerName);
	pop(recv.nGiftItemProductIDX);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CASHSHOP_ITEM_PRESENT, &recv );
}


void cCliGame::SendBuyCashItem(n4 const& nTotalPrice, std::list<DWORD> const& nItemNo, uint64 ui64OrderID )
{
	if( nItemNo.empty() )
		return;

	C2GS_SEND_CASHSHOP_MULTIBUY packet;
	packet.nItemCnt = (n1)nItemNo.size();
	packet.nTotalPrice = nTotalPrice;
	std::list<DWORD>::const_iterator it = nItemNo.begin();
	for( ; it != nItemNo.end(); ++it )
		packet.nCashItemNo.push_back( (n4)(*it) );

	cPacket p;
	p.newp(packet.GetProtocol());
	p.push(packet.nItemCnt);
	p.push(packet.nTotalPrice);
	p.push( ui64OrderID );

	std::list<n4>::const_iterator secit = packet.nCashItemNo.begin();
	for( ; secit != packet.nCashItemNo.end(); ++secit )
		p.push( (*secit) );

	p.endp(packet.GetProtocol());
	DoSend(p);
}

void cCliGame::SendCashShopBuyRequest_Steam(n4 nTotalPrice, std::list<DWORD> const& nItemNo)
{
	C2GS_SEND_CASHITEM_PURCHASE_STEAM_REQUEST packet;
	packet.nItemCount = (int)nItemNo.size();
	packet.nTotalPrice = nTotalPrice;
	packet.itemList = nItemNo;

	cPacket p;
	p.newp(packet.GetProtocol());
	p.push(packet.nItemCount);
	p.push(packet.nTotalPrice);
	p.push(2);// 리얼 + 보너스 캐쉬로 차감
	std::list<DWORD>::const_iterator it = packet.itemList.begin();
	for( ; it != packet.itemList.end(); ++it )
		p.push( (n4)(*it) );
	p.endp(packet.GetProtocol());
	DoSend(p);
}

// void cCliGame::SendBuyCashItem(int nCashType, n1 nItemCount, n4 nTotalPrice, n4 *nItemNo, uint64 ui64OrderID)
// {
// 	if(nItemCount > 12 || nItemCount < 1)	// 한번에 1~12개 이상 구매 불가능
// 	{
// 		LOG("아이템 구매 개수(%d) 오류", nItemCount);
// 		return;
// 	}
// 
// 	for(int i=0; i<nItemCount; i++)
// 	{
// 		if(nItemNo[i] == 0)
// 		{
// 			LOG("Err nItemNo == 0");
// 			return;
// 		}
// 	}
// 
// 	cPacket p;
// 	p.newp(pCashShop::Buy);
// 	p.push(nItemCount);
// 	p.push(nTotalPrice);
// 
// 	if( GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ) )
// 		p.push( 1 );	// 리얼 캐쉬로 차감
// 	else
// 		p.push( 2 );	// 리얼 + 보너스 캐쉬로 차감
// 
// 	p.push( ui64OrderID );// OrderID;
// 
// 	p.push(nItemNo, sizeof(int)*nItemCount);
// 	p.endp(pCashShop::Buy);
// 	DoSend(p);
// }
// 
// void cCliGame::SendCashShopBuyRequest_Steam(int nCashType, n1 nItemCount, n4 nTotalPrice, n4 *nItemNo)
// {
// 	cPacket p;
// 	p.newp(pCashShop::BuyRequest);
//  	p.push(nItemCount);
// 	p.push(nTotalPrice);	
// 	p.push(nCashType);
// 	p.push(nItemNo, sizeof(int)*nItemCount);
// 	p.endp(pCashShop::BuyRequest);
// 	DoSend(p);
// }

void cCliGame::RecvBuyCashItemRequest_Steam(void)
{
	int nError;
	int nCash;
	pop( nError );
	pop( nCash );

	pair<int,int> result;
	result.first = nError;
	result.second = nCash;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::RECV_REQUEST_STEAM_CASHITEM_PURCHASE, &result);
}

void cCliGame::RecvBuyCashItem(void)
{
	GS2C_RECV_CASHSHOP_MULTIBUY recv;
	pop(recv.nResult);	// 캐쉬 구입 결과
	pop(recv.nRealCash);	// 캐쉬 구입 결과
	pop(recv.nBonusCash);	// 캐쉬 구입 결과

	pop(recv.nSuccessItemCnt);

	for( int n = 0; n < recv.nSuccessItemCnt; ++n )
	{
		n4 productItemIDX = 0;
		pop(productItemIDX);
		recv.nSuccessCashItemNo.push_back( productItemIDX );
	}

	pop(recv.nFailedItemCnt);
	for( int n = 0; n < recv.nFailedItemCnt; ++n )
	{
		n4 productItemIDX = 0;
		pop(productItemIDX);
		recv.nFailedCashItemNo.push_back( productItemIDX );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_RESULT_CASHITEM_PURCHASE, &recv );
}

// 리스트 요청
void cCliGame::SendRequestRewardList(void)
{	
	cPacket p;
	p.newp(pGameTwo::RecompenseList);
	p.endp(pGameTwo::RecompenseList);
	DoSend(p);
}
// 수령 요청
void cCliGame::SendRequestRewards( u1 nReqCnt, u1 nReqIdx )
{
	GS2C_RECV_REWARD_SEND_TYPE recv;
	recv.iType = 3;
	recv.bSendAll = false;
	recv.iIndex = nReqIdx;
	recv.iCnt = nReqCnt;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::REWARD_SEND_ACCPET_DATA, &recv );	
}
void cCliGame::SendRequestRewardAll()
{
	GS2C_RECV_REWARD_SEND_TYPE recv;
	recv.iType = 3;
	recv.bSendAll = true;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::REWARD_SEND_ACCPET_DATA, &recv );
}

#ifdef REPURCHASE
void cCliGame::SendItemRepurchaseOpen( u4 nNpcID )
{
	newp( pItem::ItemRepurchaseOpen );
	push( nNpcID );
	endp( pItem::ItemRepurchaseOpen );
	send();
}

void cCliGame::RecvItemRepurchaseOpen()
{
	ST_RECV_ItemRepurchaseOpenInfo RepurchaseOpenInfo;

	pop( RepurchaseOpenInfo.m_nItemCount );	// 재구매 아이템 갯수
	for( uint i = 0 ; i < RepurchaseOpenInfo.m_nItemCount ; i++ )
	{
 		cItemData ItemData;
		pop( ItemData );
		
		RepurchaseOpenInfo.AddItemData( ItemData );
	}

	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::RECV_STORE_REPURCHASE_SET, &RepurchaseOpenInfo );
}

void cCliGame::SendItemRepurchase( u4 nNpcID, u2 nItemIdx )
{
	newp( pItem::ItemRepurchase );
	push( nNpcID );
	push( nItemIdx );
	endp( pItem::ItemRepurchase );
	send();
}

void cCliGame::RecvItemRepurchase()
{
	ST_RECV_ItemRepurchaseSuccessInfo RepurchaseSuccessInfo;

	pop(RepurchaseSuccessInfo.m_nResult);

	if( RepurchaseSuccessInfo.m_nResult == 1 ) // 성공
	{
		pop(RepurchaseSuccessInfo.m_CurrentMoney);

		u1 ChangedInvenCount(0);
		pop(ChangedInvenCount);
		for( int i = 0; i < ChangedInvenCount; ++i )
		{
			ST_RECV_ItemRepurchaseSuccessInfo::sInvenInfo InvenInfo;
			pop(InvenInfo.m_nInvenSlotNumber);
			pop(InvenInfo.m_nItemTypeID);
			pop(InvenInfo.m_nInvenItemCount);
			pop(InvenInfo.m_nRate);
			pop(InvenInfo.m_nLimited);
			pop(InvenInfo.m_nLevel);

			pop(InvenInfo.m_nAccOption);
			pop(InvenInfo.m_nAccValues);
			pop(InvenInfo.m_nEndTime);

			RepurchaseSuccessInfo.m_InvenInfoList.push_back( InvenInfo );
		}

		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::RECV_STORE_REPURCHASE_SUCCESS, &RepurchaseSuccessInfo );
	}
	else if( RepurchaseSuccessInfo.m_nResult == 0 ) // 실패
	{
		n4 nErrorCode(0);
		pop(nErrorCode);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::RECV_STORE_REPURCHASE_FAILED, &nErrorCode );
	}
}
#endif





