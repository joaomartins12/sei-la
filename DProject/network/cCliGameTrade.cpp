

#include "stdafx.h"

#include "cCliGame.h"

#include "common_vs2019/pTrade.h"

#include "common_vs2019/pPass2.h"

//////////////////////////////////////////////////////////////////////////
// Recebeu um pedido de transação (apenas da outra parte)
// Received transaction request (only from the other party)
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeRequest(void)	
{
	GS2C_RECV_TRADE_REQUEST recv;
	pop(recv.m_TargetTamerUID);

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_RECEIVE_REQUEST, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 거래 불가한 상태(상대방한테만 감)
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeImpossible(void)// 거래 불가한 상태
{
	u4 nFlag;
	pop(nFlag);

	GS2C_RECV_TRADE_REQUEST_RESULT recv;
	switch( nFlag )
	{
	case 0:		recv.m_dwResult = 30023;	break;		//다른 용무 중입니다. 거래를 신청할 수 없습니다
	case 1:		recv.m_dwResult = 11015;	break;		// 내 가방에 빈공간 없음
	case 2:		recv.m_dwResult = 30031;	break;		// 상대방 가방에 빈공간 없음
	default:	recv.m_dwResult	= nFlag;	break;
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_REQUEST_RESULT, &recv );	
}


//////////////////////////////////////////////////////////////////////////
// 거래 거부 당함(둘다 한테 감)
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeReject(void)
{
	GS2C_RECV_TRADE_REQUEST_RESULT recv;
	recv.m_dwResult = 30021;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_REQUEST_RESULT, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 거래 요청에 대한 승락/거부 (둘다 한테 감)
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeApproval(void)
{
	GS2C_RECV_TRADE_REQUEST_RESULT recv;
	pop(recv.m_TargetTamerUID);

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_REQUEST_RESULT, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 거래 아이템 등록 (둘다 한테 감)
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeAddItem(void)
{
	GS2C_RECV_TRADE_ITEM_REGIST recv;
	pop(recv.m_TargetTamerUID);//아이템을 추가한 유저 아이디
	pop(recv.m_ItemData);
	pop(recv.m_TradeInvenSlotNum);// 거래창 슬롯 번호
	pop(recv.m_InventorySlotNum);// 인벤토리 슬롯 번호
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_ITEM_REGIST, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 거래 아이템 취소 (둘다 한테 감)
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeCancelItem(void)
{
	GS2C_RECV_TRADE_ITEM_UNREGIST recv;
	pop(recv.m_TargetTamerUID);//아이템을 제거한 유저 아이디
	pop(recv.m_nItemPosition);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_ITEM_UNREGIST, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 거래 돈 변경 (둘다 한테 감)
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeAddMoney(void)	// 겜머니 추가
{
	GS2C_RECV_TRADE_MONEY_CHANGE recv;
	pop(recv.m_TargetTamerUID);
	pop(recv.m_nMoney);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_MONEY_CHANGE, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 거래 인벤토리 잠금.
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeInvenLock(void)
{
	GS2C_RECV_TRADE_INVEN_LOCK recv;
	pop(recv.m_TargetTamerUID);// 거래 Ready 취소한 테이머 UID
	recv.m_bLock = true;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_INVEN_LOCK_UNLOCK, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 거래 인벤토리 해제
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeUnInvenLock(void)
{
	GS2C_RECV_TRADE_INVEN_LOCK recv;
	pop(recv.m_TargetTamerUID);// 거래 Ready 취소한 테이머 UID
	recv.m_bLock = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_INVEN_LOCK_UNLOCK, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 최종 거래 확인 패킷
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeComplete(void)
{
	GS2C_RECV_TRADE_STANDBY recv;
	pop(recv.m_TargetTamerUID);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_STANDBY_RESULT, &recv );	
}

//////////////////////////////////////////////////////////////////////////
// 거래 완료 - 최종
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvTradeCompleteFinal(void)
{
	GS2C_RECV_TRADE_FINAL_RESULT recv;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_TRADE_FINAL_RESULT, &recv );	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// 거래 요청
void cCliGame::SendTradeRequest(u4 nTargetTamerUID)
{
	cCreateName::DeleteInstance();

	newp(pTrade::Request);
		push(nTargetTamerUID);
	endp(pTrade::Request);
	send();
}

// 승인
void cCliGame::SendTradeApproval(u4 nTargetTamerUID)
{
	newp(pTrade::Approval);
		push(nTargetTamerUID); // 나에게 거래 요청한 사용자에게, 거래 승인
	endp(pTrade::Approval);
	send();
}

// void cCliGame::SendTradeApprovalConfirm()
// {
// 	newp(pTrade::ApprovalConfirm);	
// 	endp(pTrade::ApprovalConfirm);
// 	send();
// }

void cCliGame::SendTradeReject(u4 nTargetTamerUID)
{
	newp(pTrade::Reject);
		push(nTargetTamerUID);
	endp(pTrade::Reject);
	send();
}


void cCliGame::SendTradeImpossible(u4 nTargetTamerUID)
{
	newp(pTrade::Impossible);
		push(nTargetTamerUID); // 나에게 거래 요청한 사용자에게, 거래 불가 알림
	endp(pTrade::Impossible);
	send();
}


void cCliGame::SendTradeAddItem(u2 nInvenPos, u2 nCnt)
{
	newp(pTrade::AddItem);
		push(nInvenPos);
		push(nCnt);
	endp(pTrade::AddItem);
	send();
}

void cCliGame::SendTradeCancelItem(n1 nPos)
{
	newp(pTrade::CancelItem);		
		push(nPos);
		endp(pTrade::CancelItem);
	send();
}


void cCliGame::SendTradeAddMoney(u4 nMoney)
{
	newp(pTrade::AddMoney);
		push(nMoney);
	endp(pTrade::AddMoney);
	send();
}
//////////////////////////////////////////////////////////////////////////
// 거래 창 인벤트로 잠금/해제 
//////////////////////////////////////////////////////////////////////////
void cCliGame::SendTradeInvenLock( bool bLock )
{
	if( bLock )  
	{
		newp(pTrade::InvenLock);
		endp(pTrade::InvenLock);
	}
	else
	{
		newp(pTrade::CompleteCancel);
		endp(pTrade::CompleteCancel);
	}

	send();
}

//////////////////////////////////////////////////////////////////////////
// 최종 거래 확인 패킷
//////////////////////////////////////////////////////////////////////////
void cCliGame::SendTradeCompleteFinal()
{
	newp(pTrade::Complete);
	endp(pTrade::Complete);
	send();	
}


//////////////////////////////////////////////////////////////////
// 테이머 개인 상점 관련 처리

// 개인 상점 오픈 준비 요청 & 개인 상점 편집 가능상태로 변경 요청
void cCliGame::SendTamerShopReady(n4 nRequestType, u4 nSlot)
{
	newp( pTrade::TamerShopReady );
		push( nRequestType );

#ifdef ITEM_USE_TIME_PASS
		if( nRequestType == 1 || nRequestType == 4 )	//2017-04-04-nova 1일경우에도 슬롯번호 전송
		{
			push( nSlot );
		}
#else
		if( nRequestType == 4 )
		{
			push( nSlot );
		}
#endif
		
	endp( pTrade::TamerShopReady );

	send();
}

void cCliGame::RecvTamerShopReadyResult(void)
{
	GS2C_RECV_STORE_READY_RESULT recv;
	pop( recv.nResult );
	pop( recv.nItemType );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_STORE_READY_RESULT, &recv );
}

void cCliGame::SendTamerShopOpen(wchar const* szTitle, n4 nItemNumber, nTrade::Item *pSellItemList)	// 개인 상점 개설
{
	std::wstring wtitle(szTitle);

	newp(pTrade::TamerShopOpen);
		push( wtitle );		// 상점 타이틀 
		push(nItemNumber);	// 판매할 아이템 개수
		push(pSellItemList, sizeof(nTrade::Item)*nItemNumber);	// 판매할 아이템 정보
	endp(pTrade::TamerShopOpen);

	send();
}

// 개인 상점 오픈 결과
void cCliGame::RecvTamerShopOpenResult(void)
{
	GS2C_RECV_STORE_OPEN_CLOSE_RESULT recv;
	pop( recv.nResult );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_OPEN_PERSONSTORE, &recv );
}

void cCliGame::SendTamerShopClose(void)	// 개인 상점 폐쇄
{
	newp(pTrade::TamerShopClose);
	endp(pTrade::TamerShopClose);
	send();
}

void cCliGame::RecvTamerShopCloseResult(void)
{
	GS2C_RECV_STORE_OPEN_CLOSE_RESULT recv;
	pop( recv.nResult );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CLOSE_PERSONSTORE, &recv );
}

// nType 기본 값은 0 입니다. 0: 개인 상점 1: 위탁 상점
void cCliGame::SendTamerShopItemList(u4 nTargetTamerUID, uint nType) // 상점에서 판매하는 아이템 목록 요청
{
	newp(pTrade::TamerShopList);
		push( nType );
		push( nTargetTamerUID );
	endp(pTrade::TamerShopList);
	send();
}

void cCliGame::RecvTamerShopItemList(void)	// 테이머 개인 상점 아이템 목록
{
	GS2C_RECV_PERSONSTORE_ITEMLIST recv;
	pop( recv.nResult );

	if( 100 == recv.nResult )
	{
		pop( recv.szTitle );
		pop( recv.nItemCount );		// 판매 아이템 개수
		for ( int i = 0; i < recv.nItemCount; ++i )
		{
			nsPersonStore::sSellItemInfo SellItem;
			pop( SellItem.ItemData );
			pop( SellItem.nItemPrice );
			recv.lItemInfo.push_back( SellItem );
		}
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_PERSONSTORE_ITEMLIST, &recv );
}


void cCliGame::SendTamerShopBuy(u4 nTargetTamerUID, uint nIndex, cItemData *item, n8 Price)	// 상점으로 부터 아이템 구매
{
	// 동기화 - 기반은 1부터
	nIndex += 1;

	newp(pTrade::TamerShopBuy);
		push(nTargetTamerUID);	// 대상 UID
		push(nIndex);			// 상점의 아이템 인덱스 0값은 사용하지 않습니다. 1부터~
		push(*item);			// 구매 아이템 정보
		push(Price);			// 개별 구매 금액
	endp(pTrade::TamerShopBuy);

	send();
}

// 상점으로 부터 아이템을 구매 결과
void cCliGame::RecvTamerShopBuy(void)
{
	GS2C_RECV_BUY_RESULT_PERSONITEM recv;
	pop( recv.nResult );
	if( 100 == recv.nResult )
	{
		pop( recv.nSlotIndex );
		pop( recv.nCount );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_BUY_ITEM_RESULT, &recv );

	net::game->SendQueryMoneyInven();
}

// 개인 상점 - 자기 상점 아이템 판매 결과
void cCliGame::RecvTamerShopSellResult(void)
{	
	// 자신이 올린 아이템 팔렸을 경우
	GS2C_RECV_BUY_RESULT_PERSONITEM recv;
	recv.nResult = 100;		// 판매 성공으로 가야한다
	pop( recv.nSlotIndex );
	pop( recv.nCount );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_BUY_ITEM_RESULT, &recv );

	net::game->SendQueryMoneyInven();
}

//////////////////////////////////////////////////////////////////
// 위탁 상점 관련 처리

// nRequestType 1: 상점 오픈 준비 요청 2: 상점 편집상태로 변경 요청 3: 상점 오픈 준비 취소
// pos 위치, nFloat 회전 값
// nSlot 라이센스 아이템이 있는 위치
	
// 위탁 상점 오픈 요청 ( 상점 이름 )

void cCliGame::SendCommissionShopOpen(nSync::Pos pos, float nFloat, wchar const * szTitle, n8 nMoney, uint nItemCount, nTrade::Item *item )
{
	std::wstring wtitle(szTitle);

	newp( pTrade::ShopOpen );
		push( pos );
		push( nFloat );
		push( wtitle );				// 상점 이름
		push( nMoney );					// 상점 구매 금액 등록
		push( nItemCount );
		push( item, sizeof( nTrade::Item) * nItemCount );
	endp( pTrade::ShopOpen );			
	send();

	ChannelContents::CHANNELINFO ChannelData;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::GET_CHANNELINFO, &ChannelData );

	GS2C_RECV_SHOP_DATA pRecv;
	pRecv.iCharMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	pRecv.iCharChannel = ChannelData.channel_idx;
	pRecv.iCharModelID = -1;
	pRecv.iCharPos = NiPoint2( (float)pos.m_nX, (float)pos.m_nY );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_SET_SHOPDATA, &pRecv );	
}

void cCliGame::RecvCommissionShopOpenResult(void)
{
	GS2C_RECV_STORE_OPEN_CLOSE_RESULT recv;
	pop( recv.nResult );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_OPEN_COMMISSIONSTORE, &recv );
}

void cCliGame::SendCommissionShopClose(void)															
{
	newp(pTrade::ShopClose);
	endp(pTrade::ShopClose);
	send();
}

void cCliGame::RecvCommissionShopCloseResult()
{
	GS2C_RECV_STORE_OPEN_CLOSE_RESULT recv;
	pop( recv.nResult );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CLOSE_COMMISSIONSTORE, &recv );
}

// 위탁 상점 아이템 구매 요청 ( UID, 상품 인덱스, 구매 개수 )
void cCliGame::SendCommissionShopBuy(u4 nShopUID, uint nIndex, cItemData *pItem, n8 nUnitPrice)
{
	// 동기화 - 기반은 1부터
	nIndex += 1;

	newp( pTrade::ShopBuy );
		push( nShopUID );
		push( nIndex );
		push( *pItem );
		push( nUnitPrice );
	endp( pTrade::ShopBuy );
	send();
}

void cCliGame::RecvCommissionShopBuyResult(void)
{
	GS2C_RECV_BUY_RESULT_PERSONITEM recv;
	pop( recv.nResult );
	pop( recv.nSlotIndex );
	pop( recv.nCount );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_BUY_ITEM_RESULT, &recv );

	net::game->SendQueryMoneyInven();
}

// 위탁 상점 아이템 판매 요청 ( UID, 상품 인덱스, 판매 아이템, 판매 개수, 아이템 타입, 개별 가격 )	
void cCliGame::SendCommissionShopSell(u4 nShopUID, uint nIndex, cItemData *pItem, n8 nUnitPrice)
{
	// 동기화 - 기반은 1부터
	nIndex += 1;

	newp( pTrade::ShopSell );
		push( nShopUID );
		push( nIndex );		
		push( *pItem );
		push( nUnitPrice );
	endp( pTrade::ShopSell );
	send();
}

void cCliGame::RecvCommissionShopSellResult(void)
{
	// 위탁 상점 판매 결과시 호출되지만 아무것도 안 함
	// 판매 결과 갱신도 안 함
	// 현재는 UI 다시 켤 때 갱신됨
}


void cCliGame::RecvCommissionShopItemListResult(void)
{
	GS2C_RECV_COMMISSIONSTORE_ITEMLIST recv;
	pop( recv.nResult );

	if( 100 == recv.nResult )
	{
		pop( recv.nUsedItemType );
		pop( recv.szShopName );

		if( recv.szShopName.empty() )
			recv.szShopName = L"???";

#ifdef EMPLOYMENT_TAMERNAME
		pop( recv.szTamerName );
#endif

		{	// 안 쓴다. 날린다.
			u4 nBuyCount = 0;
			pop( nBuyCount );						// 구매 개수

			cItemData ItemData;
			n8 nUnitPrice = 0;
			for( uint i = 0; i < nBuyCount; i++ )
			{		
				pop( ItemData );	// 아이템 정보
				pop( nUnitPrice );	// 아이템 개별 구매가
			}
		}

		pop( recv.nItemCount );
		for( int i = 0; i < recv.nItemCount; ++i )
		{
			nsPersonStore::sSellItemInfo SellItem;
			pop( SellItem.ItemData );
			pop( SellItem.nItemPrice );
			recv.lItemInfo.push_back( SellItem );
		}

		pop( recv.bMyShop );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_COMMISSIONSTORE_ITEMLIST, &recv );
}

void cCliGame::SendCommissionShopNpcItemList()
{
	newp(pTrade::ShopNPCItemList);
	endp(pTrade::ShopNPCItemList);
	send();	
}


void cCliGame::RecvCommissionShopNpcItemList(void)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_UNIONSTORE, 0 ) )
	{
		if( g_pGameIF->GetUnionStore()->IsActiveCollectBtn() == false )
			g_pGameIF->GetUnionStore()->SetEnableWindow( true );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_DELETE_DATA, NULL );

	uint nResult = 0;
	pop( nResult );

	if( 100 == nResult )
	{
		n8			nMoney = 0;
		uint		nCount = 0;
		cItemData	ItemData;

		pop( nMoney );	// 보관중인 금액
		GS2C_RECV_SET_UNIONMONEY recv1;
		recv1.money = nMoney;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_MONEY_SET_DATA, &recv1 );	

		pop( nCount );	// 보관중인 아이템 개수
		GS2C_RECV_ADD_UNIONITEM recv2;

		for( uint i = 0; i < nCount; i++ )
		{
			pop( ItemData );

			recv2.ItemData = ItemData;

			if( ItemData.m_nType == 0 )
				continue;

			GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_ITEM_ADD_DATA, &recv2 );
		}

		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_UNIONSTORE, 0 ) )
			g_pGameIF->GetUnionStore()->SetEnableCollectBtn( true );
	}
	else
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_UNIONSTORE, 0 ) )
			g_pGameIF->GetUnionStore()->SetEnableCollectBtn( false );
	}	
}

// 위탁 상점 아이템 보관함에서 아이템 회수 Type 1: 아이템 회수 2: 돈 회수
// 돈 회수의 경우 nIndex 값을 
void cCliGame::SendCommissionShopItemWithDraw()
{
	newp( pTrade::ShopWithdraw );
	endp( pTrade::ShopWithdraw );
	send();
}

void cCliGame::RecvCommissionShopItemWithDrawResult(void)
{
	int nResult = 0;
	pop( nResult );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_UNION_SHOP_WITHDRAW, &nResult );	
}
