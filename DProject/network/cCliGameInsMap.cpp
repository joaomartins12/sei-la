


#include "stdafx.h"
#include "cCliGame.h"
#include "common_vs2019/pInsMap.h"

void cCliGame::SetInsMap_DungeonRestoreFlag()
{
	g_pResist->SetEnablePortal( true ); // 포탈 사용 다시 활성화
	//CHAT_TEXT_DEGUG(_T("RestoreFlag-포탈 상태 %d"), (int)g_pResist->IsEnablePortal());
	m_bPortalRequesting = false;	// 포탈이동 요청 중단.	
	m_bPortalProcessing = false;	// 싱크를 사용
	net::nIsMoveMapByCommand = true;
}
void cCliGame::SetInsMap_DungeonMSG(uint nPortalNo)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	if( g_pResist->IsEnablePortal() == false )
	{
		return;
	}
	g_pResist->SetEnablePortal( false );	

	if(m_bPortalRequesting)
	{
		return;
	}
	m_bPortalRequesting = true;	

	

	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser(0);		

	pTamer->SetPauseAll(true);	
	pDigimon->SetPauseAll(true);
	
	pTamer->GetProp_Path()->ResetAgent();	
	pDigimon->GetProp_Path()->ResetAgent();

	pTamer->SetPauseAll(false);		
	pTamer->SetAnimation( ANI::IDLE_NORMAL );	

	pDigimon->SetPauseAll(false);		
	pDigimon->SetAnimation( ANI::IDLE_NORMAL );	

// 	cPrintMsg::PrintMsg( 30096);
// 	cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
// 	pMBox->SetValue1( nPortalNo );
	// 메시지 박스에서 처리
	//ok 시도
	if(net::game)
		net::game->SendInsMap_DungeonIN( nPortalNo );	

}
void cCliGame::SendInsMap_DungeonIN(uint nPortalNo)
{
	cPacket p;
	p.newp( pInsMap::DungeonIN );
	p.push(nPortalNo);
	p.endp( pInsMap::DungeonIN );
	net::game->DoSend( p );

	//cClient::SetSendBlock(true);
}

void cCliGame::RecvBattleEvoPoint()
{
	u2 nEP;
	u1 bMax;
	pop(nEP);	
	pop(bMax);

	g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->SetBattleEP( nEP );

	if( bMax )
	{
		g_pCharMng->GetDigimonUser( 0 )->SetBattleEPDecrDisable();
	}	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void cCliGame::Recv_InfiniteWar_ChallengeStageClear()
{
	GS2C_RECV_CHALLENGE_STAGE_CLEAR recv;
	pop(recv.nCurrentPlayedStage);	//클리어한 스테이지의 단계
	pop(recv.bIsLastStage);		//클리어한 스테이지가 마지막 스테이지였는지 0 = false , 1 = true	
	pop(recv.nMonsterIDX);			//스테이지에 소환된 몬스터중 표시할 몬스터IDX	
	pop(recv.nTotalPoint);
	pop(recv.nCurrentStagePoint);
	pop(recv.nNpcIDX);				// 현재 던전의 NPCIDX
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_INFINITEWAR_CHALLENGESTATECLEAR, &recv );
}

void cCliGame::Recv_InfiniteWar_ChallengeTryNextStage()
{
	GS2C_RECV_CHALLENGE_RES_NEXT_STAGE recv;
	pop( recv.eRes );
	pop( recv.nNpcIDX );//서버측의 NpcIDX를 전송
	pop( recv.nStartCountDown );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_INFINITEWAR_CHALLENGETRY_NEXTSTAGE, &recv );
}

void cCliGame::Recv_InfiniteWar_ChallengeGiveup()
{
	GS2C_RECV_CHALLENGE_RES_GIVE_UP recv;
	pop( recv.eRes );	//포기 요청 결과값
	pop( recv.nNpcIDX );//서버측의 NpcIDX를 전송
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_INFINITEWAR_CHALLENGE_GIVEUP, &recv );
}

void cCliGame::Recv_InfiniteWar_ChallengePartyAlreadyPlay()
{
	GS2C_RECV_CHALLENGE_RES_PARTY_ENTER_FAIL recv;

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_INFINITEWAR_MAPMOVE_FALSE, &recv );	
}

// Sender
void cCliGame::Send_InfiniteWar_ChallengeTryNextStage(n4 const& nNpcIdx)
{
	C2GS_SEND_CHALLENGE_REQ_NEXT_STAGE packet;
	packet.nNpcIDX = nNpcIdx;
	newp( pInsMap::ChallengeTryNextStage );
	push( packet.nNpcIDX );
	endp( pInsMap::ChallengeTryNextStage );
	send();
}

void cCliGame::Send_InfiniteWar_ChallengeGiveup(n4 const& nNpcIdx)
{
	C2GS_SEND_CHALLENGE_REQ_GIVE_UP packet;
	packet.nNpcIDX = nNpcIdx;
	newp( pInsMap::ChallengeGiveup );
	push( packet.nNpcIDX );
	endp( pInsMap::ChallengeGiveup );
	send();
}

/////////////////////////////여기서부터 보상투입관련/////////////////////////////
// 서버에 무한 대전 게시판 관련 데이터 요청
void cCliGame::Send_Request_ChallengeData()
{
	C2GS_SEND_CHALLENGE_DAILY_GET packet;
	newp(pInsMap::GetChallengeDailyPoint);
	endp(pInsMap::GetChallengeDailyPoint);
	send();
	BHPRT( "Send - C2GS_SEND_CHALLENGE_DAILY_GET" );
};

void cCliGame::Recv_ChallengeData()
{
	GS2C_RECV_CHALLENGE_DAILY_GET recv;
	pop( recv.nDay );				//요일 , 0 - 일 ~ 6 - 토
	pop( recv.nResetTimeLeftSec );	//초기화까지 남은 시간(초)
	pop( recv.nDailyPoint );		//현재 자신의 일일 포인트

	BHPRT( "Recv - GS2C_RECV_CHALLENGE_DAILY_GET( nDay : %d, nResetTimeLeftSec : %I64d, nDailyPoint: %u)", recv.nDay, recv.nResetTimeLeftSec, recv.nDailyPoint );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_INFINITEWAR_NOTICEBOARD_DATA, &recv );
};

// 포인트 투입
void cCliGame::Send_Challenge_PointItem_Put(std::list<nChallengeSt::st_ItemInfo> const& items)
{
	C2GS_SEND_CHALLENGE_DAILY_PUT packet;
	packet.lItemList = items;

	newp( pInsMap::PutChallengeDailyPoint );
	n2 nListSize = (n2)packet.lItemList.size();						//투입할 아이템 리스트 사이즈
	push( nListSize );

	BHPRT( "Send - C2GS_SEND_CHALLENGE_DAILY_PUT( Item Count : %d )", nListSize );
	std::list<nChallengeSt::st_ItemInfo>::const_iterator it = packet.lItemList.begin();
	for( ; it != packet.lItemList.end(); ++it )
	{	
		push( (*it).nItemPos );//아이템 인벤토리 위치
		push( (*it).nCount );//아이템 갯수
		push( (*it).nItemCode );//아이템 코드		
		BHPRT( "	Send - C2GS_SEND_CHALLENGE_DAILY_PUT( Pos : %d, Code : %d, Count : %d )", (*it).nItemPos, (*it).nItemCode, (*it).nCount );
	}

	endp( pInsMap::PutChallengeDailyPoint );
	send();
};

void cCliGame::Recv_ChallengeData_PointItem_Put()
{
	GS2C_RECV_CHALLENGE_DAILY_PUT recv;
	pop( recv.nResult );					//요청결과

	pop( recv.nDailyPoint );				//투입하고난뒤 자신의 포인트

	BHPRT( "Recv - GS2C_RECV_CHALLENGE_DAILY_PUT( Result : %d, Point : %u)", recv.nResult, recv.nDailyPoint );

	n2 nUsedListSize;						//실제 사용한 아이템 리스트 크기
	pop( nUsedListSize );					//요청결과
	for( int n = 0; n < nUsedListSize; ++n )
	{
		nChallengeSt::st_ItemInfo delItem;
		pop( delItem.nItemPos );		//아이템 인벤토리 위치
		pop( delItem.nCount );			//아이템 갯수
		pop( delItem.nItemCode );		//아이템 코드
		recv.lUsedItemList.push_back( delItem );	//실제 사용한 아이템들 목록		
		BHPRT( "	Recv - GS2C_RECV_CHALLENGE_DAILY_PUT( Delete ItemPos : %d, Code : %u, Count : %d )", delItem.nItemPos, delItem.nItemCode, delItem.nCount );			
	}

	n2 nRewardListSize;						//보상으로 나온 아이템 리스트
	pop( nRewardListSize );
	for( int n = 0; n < nRewardListSize; ++n )//보상 아이템들 목록
	{
		nChallengeSt::st_RewardItemInfo addItem;
		pop( addItem.nCount );			//아이템 갯수
		pop( addItem.nItemCode );		//아이템 코드
		recv.lRewardItemList.push_back( addItem );	//실제 사용한 아이템들 목록		
		BHPRT( "	Recv - GS2C_RECV_CHALLENGE_DAILY_PUT( Crop Code : %u, Count : %d )", addItem.nItemCode, addItem.nCount );					
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_INFINITEWAR_NOTICEBOARD_POINT_PUT_RESULT, &recv );
}

void cCliGame::Ntf_ChallengeDailyUpdate()
{
	GS2C_RECV_CHALLENGE_DAILY_UPDATE recv;
	pop( recv.nDay );				//요일 , 0 - 일 ~ 6 - 토
	pop( recv.nResetTimeLeftSec );	//초기화까지 남은 시간(초)
	pop( recv.nDailyPoint );		//현재 자신의 일일 포인트
	GAME_EVENT_ST.OnEvent( EVENT_CODE::NTF_INFINITEWAR_NOTICEBOARD_DATA_UPDATE, &recv );
	BHPRT( "Recv - Ntf_ChallengeDailyUpdate( nDay : %d, nResetTimeLeftSec : %u, nDailyPoint: %u)", recv.nDay, recv.nResetTimeLeftSec, recv.nDailyPoint );	
};