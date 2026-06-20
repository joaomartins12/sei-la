

#include "stdafx.h"


#include "cCliGame.h"
#include "common_vs2019/pQuest.h"


void cCliGame::SendQuestAchievement(u4 nQuestIDX)
{	
	u4 nCTS = cClient::GetTimeTS();
	u4 nQNO = nCTS - nQuestIDX;

	newp(pQuest::Achievement);
		push(nQuestIDX);
		push(nCTS);
		push(nQNO);
	endp(pQuest::Achievement);
	send();
}


void cCliGame::RecvUpdateDailyQuest(void)
{
	if( g_pDataMng )
		g_pDataMng->GetQuest()->ResetDailyQuest();
}



void cCliGame::SendQuestChangeInfo(uint nQuestIDX, uint nCondIDX, uint nValue)
{
	newp(pQuest::ChangeInfo);
		push((u2)nQuestIDX);
		push((u1)nCondIDX);
		push((u1)nValue);
	endp(pQuest::ChangeInfo);
	send();	
}


void cCliGame::RecvQuestChangeInfo(void)
{
	u2 nQuestIDX;
	u1 nCondIDX;	//
	u1 nValue;		// 

	pop(nQuestIDX);
	pop(nCondIDX);
	pop(nValue);
	
	g_pCharMng->GetDigimonUser( 0 )->GetProp_Attack()->InsertEvent( CsC_AttackProp::EVENT_QUEST, nQuestIDX, nCondIDX, nValue, 0 );	
}


void cCliGame::SendQuestRequire(uint nQuestIDX)
{
	newp(pQuest::Require);
	push((u2)nQuestIDX);
	endp(pQuest::Require);
	send();
}


void cCliGame::SendQuestCancel(uint nQuestIDX)
{
	newp(pQuest::Cancel);
	push((u2)nQuestIDX);
	endp(pQuest::Cancel);
	send();
}


void cCliGame::SendQuestComplete(uint nQuestIDX)
{
	newp(pQuest::Complete);
	push((u2)nQuestIDX);
	endp(pQuest::Complete);
	send();
}


// 퀘스트 취소시 날라오는 패킷으로 지금은 
// 퀘스트 관련 아이템을 창고에서 제거하는 용도록 사용.
void cCliGame::RecvQuestCancel(void)
{
	u2 deleteItemTotalCount = 0;
	pop( deleteItemTotalCount );

	list<WareHouseContents::sWAREHOUSE> listNormalWareHouse;
	list<WareHouseContents::sWAREHOUSE> listShareWareHouse;

	for( int n = 0; n < deleteItemTotalCount; ++n )
	{
		n4 nWarehouseType;
		pop( nWarehouseType );
		u2 nWarehouseIdx;
		pop( nWarehouseIdx );
		u4 nDeleteCount;
		pop( nDeleteCount );
		u4 nDeleteItemCode;
		pop( nDeleteItemCode );

		WareHouseContents::sWAREHOUSE stTemp;
		stTemp.nIdx = nWarehouseIdx;
		stTemp.nDecreaseCount = nDeleteCount;
		stTemp.nItemCode = nDeleteItemCode;

		switch( nWarehouseType )
		{
		case nItem::TabWarehouse:	listNormalWareHouse.push_back( stTemp );	break;// 개인 창고
		case nItem::TabShareStash:	listShareWareHouse.push_back( stTemp );		break;// 공유 창고
		}		
	}

	if( !listNormalWareHouse.empty() )
		GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_NORMAL_DATA_DECREASE, &listNormalWareHouse );
	if( !listShareWareHouse.empty() )
		GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_SHARE_DATA_DECREASE, &listShareWareHouse );
}

void cCliGame::RecvQuestAllInfo()
{
	// 전체 퀘스트 정보 받아옴
	pop(*g_pDataMng->GetQuest()->GetInfo());
	g_pDataMng->GetQuest()->PostRevPostLoad(net::next_map_no);

	// 퀘스트 관련 UI 갱신
	if (g_pGameIF && g_pGameIF->GetQuestHelper())
		g_pGameIF->GetQuestHelper()->ResetHelperString();

	if (g_pGameIF && g_pGameIF->GetQuestList())
		g_pGameIF->GetQuestList()->ResetQuestList(false);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_QUEST_ALL_INFO);
}