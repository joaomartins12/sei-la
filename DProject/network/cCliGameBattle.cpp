
#include "stdafx.h"

#include "cNetwork.h"
#include "cCliGame.h"

#include "../Global.h"
#include "../Engine.h"

#include "common_vs2019/pGame.h"
#include "common_vs2019/pTamer.h"
#include "common_vs2019/pDigimon.h"
#include "common_vs2019/pSkill.h"

#include "common_vs2019/pLimit.h"

#include "common_vs2019/pItem.h"
#include "common_vs2019/pInven.h"

#include "common_vs2019/cItemData.h"


#include "common_vs2019/pInsMap.h"


#include "../Flow/Flow.h"
#include "../Flow/FlowMgr.h"

#include "common_vs2019/cBattleInfo.h"


//////////////////////////////////////////////////////////////////////////
// Sender
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 배틀 관련 내 정보를 서버에 요청 한다.
//////////////////////////////////////////////////////////////////////////
void cCliGame::SendMyBattleMatchInfo()
{
	C2GS_SEND_BATTLEMATCHING_MY_INFO packet;
	newp(packet.GetPacketID());
	endp(packet.GetPacketID());
	send();
}

//////////////////////////////////////////////////////////////////////////
// 서버에 배틀 등록 요청 패킷을 보냄
//////////////////////////////////////////////////////////////////////////
void cCliGame::SendBattleMatchRequest(n1 const& nMainMode, n1 const& nSubMode, u4 const& nNpcID, u4 const& nMapID)
{
	//배틀 참가신청
	C2GS_SEND_BATTLEMATCHING_REGISTER packet;
	packet.m_nBattleMainMode = nMainMode;
	packet.m_nBattleSubMode = nSubMode;
	packet.m_nNpcIdx = nNpcID;
	packet.m_nMapIdx = nMapID;

	newp(packet.GetPacketID());
	push(packet.m_nBattleMainMode);
	push(packet.m_nBattleSubMode);
	push(packet.m_nNpcIdx);
	push(packet.m_nMapIdx);
	endp(packet.GetPacketID());
	send();
}

//////////////////////////////////////////////////////////////////////////
// 서버에 배틀 등록이 되어 있으면 배틀 등록 취소 패킷을 보낸다.
//////////////////////////////////////////////////////////////////////////
void cCliGame::SendBattleMatchCancel()
{
	C2GS_SEND_BATTLE_REGISTER_REQUEST_CANCEL packet;
	//배틀 참가취소
	newp(packet.GetPacketID());
	endp(packet.GetPacketID());
	send();
}

//////////////////////////////////////////////////////////////////////////
// 서버에 나의 준비 상태를 보냄 , 0 : 취소, 1 : 수락
//////////////////////////////////////////////////////////////////////////
void cCliGame::SendBattleReady(n1 nIs, n4 const& nBattleIdx)
{
	//SetSendBlock( false );

	C2GS_SEND_BATTLE_REGISTER_READY_STATE packet;
	packet.m_nReadyState = nIs;
	packet.m_nBattleIdx = nBattleIdx;
	// nIs 0 : 안함, 1 : 함
	newp(packet.GetPacketID());
	push(packet.m_nReadyState);
	push(packet.m_nBattleIdx);
	endp(packet.GetPacketID());
	send();
}

//****************************************************************************/

//****************************************************************************/
//****************************************************************************/

//****************************************************************************/
//////////////////////////////////////////////////////////////////////////
// 서버에서 내 배틀 정보를 받는다.
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvMyBattleMatchInfo()
{
	GS2C_RECV_BATTLEMATCHING_MY_INFO recv;

	pop(recv.m_nBattleTicketPoint);	//	보유 배틀 티켓

	// 모드별 플레이 횟수
	n4 nCount;
	pop(nCount);			// 모드 갯수
	for (int n = 0; n < nCount; ++n)
	{
		n1 modeType;
		pop(modeType);		// 모드 타입

		sMainTypeData addInfo;
		pop(addInfo.m_nTotalPlayCount);	// 플레이 횟수
		pop(addInfo.m_nTotalRankPoint);	// 플레이 횟수
		auto rib = recv.m_nBattleTypeUserData.insert(std::make_pair(modeType, addInfo));
		if (!rib.second)
			assert_cs(false);
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_BATTLEMATCH_MY_INFO, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 배틀 등록 결과
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvBattleRegistRequest()
{
	GS2C_RECV_BATTLEMATCHING_REGISTER recv;
	pop(recv.m_nResult);
	pop(recv.m_nBattleMainMode);
	pop(recv.m_nBattleSubMode);
	pop(recv.m_nMapIdx);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_BATTLEMATCH_REGISTER_RESULT, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 배틀 등록 취소 결과
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvBattleRegistCancel()
{
	GS2C_RECV_BATTLE_REGISTER_REQUEST_CANCEL recv;
	pop(recv.m_nResult);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_BATTLEMATCH_REGISTER_CANCEL, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 배틀 준비 체크 데이터를 받는다. 배틀에 참여할 플레이어 정보 포함
//////////////////////////////////////////////////////////////////////////
void cCliGame::NtfBattleRegistReadyInfo()
{
	GS2C_NTF_BATTLE_READY_STATE_ASK recv;

	pop(recv.m_nBattleIdx);
	pop(recv.m_nMapIdx);
	pop(recv.m_cBattleType);
	pop(recv.m_cBattleMod);

	n4 TeamCount = 0;
	pop(TeamCount);
	for (n4 n = 0; n < TeamCount; ++n)
	{
		n1 teamCode;
		pop(teamCode);

		std::map<n1, std::list<sBattlePlayerInfo>>::iterator it = recv.m_mapTeamInfo.find(teamCode);
		if (it == recv.m_mapTeamInfo.end())
		{
			auto rib = recv.m_mapTeamInfo.insert(std::make_pair(teamCode, std::list<sBattlePlayerInfo>()));
			it = rib.first;
		}

		n4 nPlayerCount = 0;
		pop(nPlayerCount);
		for (n4 n = 0; n < nPlayerCount; ++n)
		{
			sBattlePlayerInfo addInfo;
			pop(addInfo.m_uTamerUIDX);
			pop(addInfo.m_nTamerName);
			pop(addInfo.m_uDigimonTableIdx);
			it->second.push_back(addInfo);
		}
	}


	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_BATTLE_MATCH_READY_REQUEST, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 내가 배틀 준비 상태 패킷을 보낸 것에 대한 결과값
//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvBattleRegistReadyResult()
{
	GS2C_RECV_BATTLE_REGISTER_READY_STATE recv;
	pop(recv.m_nReadyState);
	pop(recv.m_nResult);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_BATTLEMATCH_READY_STATE, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 나를 제외한 다른 플레이어들의 레디 상태 결과값
//////////////////////////////////////////////////////////////////////////
void cCliGame::NtfBattleRegistReadyResult()
{
	GS2C_NTF_BATTLE_REGISTER_READY_STATE recv;
	pop(recv.m_nReadyState);	//	0 : 취소, 1 : 수락
	pop(recv.m_nTamerIDX);	//	테이머 IDX	
	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_BATTLEMATCH_READY_STATE, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 배틀 매칭이 취소 시 취소 이유가 담긴 패킷
//////////////////////////////////////////////////////////////////////////
void cCliGame::NtfBattleMatchCancel()
{
	GS2C_NTF_BATTLE_MATCH_CANCEL recv;
	pop(recv.m_nResult);
	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_BATTLEMATCH_CANCEL, &recv);
}


//////////////////////////////////////////////////////////////////////////
// 배틀 매칭이 성공 했을 날라오는 패킷
//////////////////////////////////////////////////////////////////////////
void cCliGame::NtfBattleMatchComplete()
{
	GS2C_NTF_BATTLE_MATCH_COMPLETE recv;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_BATTLEMATCH_COMPLETE, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 배틀 시작 타이머 데이터 받음.
//////////////////////////////////////////////////////////////////////////
void cCliGame::NtfBattleStart()
{
	GS2C_NTF_BATTLE_START recv;
	pop(recv.m_nStart);	// 경기 시작 시간
	pop(recv.m_nEnd);		// 경기 종료 시간
	pop(recv.m_nRound);	// 경기 진행 시간

	pop(recv.m_nBattleTicketPoint);		// 최종 보유 배틀 티켓
	pop(recv.m_nBattleMainMode);	// 1 : 팀 대전, 2 ...
	pop(recv.m_nPlayCount);			// 최종 플레이 카운트

	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_BATTLE_START, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 배틀 종료
//////////////////////////////////////////////////////////////////////////
void cCliGame::NtfBattleEnd()
{
	GS2C_NTF_BATTLE_END recv;
	pop(recv.m_nWinTeamCode);		// 승리 팀(0:무승부, 1 : 승, 2 : 패)
	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_BATTLE_END, &recv);
}

//////////////////////////////////////////////////////////////////////////
// 배틀 종료 후 결과
//////////////////////////////////////////////////////////////////////////
void cCliGame::NtfBattleReward()
{
	GS2C_NTF_BATTLE_REWARDS recv;
	pop(recv.m_nBattleMainType);	// 획득한 모드
	pop(recv.m_nDuelPoint);			// 획득 포인트
	n4 nCount = 0;
	pop(nCount);
	for (n4 n = 0; n < nCount; ++n)
	{
		sRewardItemInfo add;
		pop(add.m_nItemCode);
		pop(add.m_nItemCount);
		recv.m_Rewardlist.push_back(add);	// 배틀 보상 아이템
	}

	GAME_EVENT_ST.OnEvent(EVENT_CODE::NTF_BATTLE_REWARDS, &recv);
}

void cCliGame::NtfBattleMemberSync()
{
}

void cCliGame::NtfBattleMatchSync()
{

}