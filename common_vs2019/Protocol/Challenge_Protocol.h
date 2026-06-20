#pragma once
#include "Challenge_Define.h"
#include "Challenge_Result.h"


/*--------------------------------------------
pInsMap::ChallengeStageClear
S -> C
해당스테이지가 완료되면 전송된다
--------------------------------------------*/

struct GS2C_RECV_CHALLENGE_STAGE_CLEAR{
	GS2C_RECV_CHALLENGE_STAGE_CLEAR() : nCurrentPlayedStage(0) , bIsLastStage(0) , nMonsterIDX(0) , nTotalPoint(0) , nCurrentStagePoint(0) , nNpcIDX(0)
	{
	}
	u4 nCurrentPlayedStage;	//클리어한 스테이지의 단계
	n1 bIsLastStage;		//클리어한 스테이지가 마지막 스테이지였는지 0 = false , 1 = true	

	u4 nMonsterIDX;			//스테이지에 소환된 몬스터중 표시할 몬스터IDX
	
	u2 nTotalPoint;			//현재까지 총 획득한 포인트
	u2 nCurrentStagePoint;		//클리어한 스테이지에서의 포인트
	n4 nNpcIDX;				// 현재 던전의 NPCIDX
};


/*--------------------------------------------
pInsMap::ChallengePartyAlreadyPlay
S -> C
파티가 이미 들어가서 게임이 시작되었을경우 전송
만약 던전에 입장가능할경우에는 기존패킷을 사용하여 입장
--------------------------------------------*/
struct GS2C_RECV_CHALLENGE_RES_PARTY_ENTER_FAIL{
	GS2C_RECV_CHALLENGE_RES_PARTY_ENTER_FAIL()
	{
	}
};


/*--------------------------------------------
pInsMap::ChallengeTryNextStage
C -> S
다음 스테이지 시작 요청
--------------------------------------------*/
struct C2GS_SEND_CHALLENGE_REQ_NEXT_STAGE{
	C2GS_SEND_CHALLENGE_REQ_NEXT_STAGE() : nNpcIDX(0)
	{
	}
	n4 nNpcIDX;	// 시작을 요청한 NPC 인덱스
};

/*--------------------------------------------
pInsMap::ChallengeTryNextStage
S -> C
다음 스테이지 시작 요청에대한 응답
RESULT : CHALLENGE_RESULT (ok = 0 , fail = 1 , npcIdxError = 2)
--------------------------------------------*/
struct GS2C_RECV_CHALLENGE_RES_NEXT_STAGE{
	GS2C_RECV_CHALLENGE_RES_NEXT_STAGE() : eRes(0) , nNpcIDX(0) , nStartCountDown(0)
	{
	}
	n1 eRes;	//시작 요청 결과값
	n4 nNpcIDX;	//서버측의 NpcIDX를 전송
	n4 nStartCountDown;		//해당 ms 이후에 서버에서 몹을 생성합니다	만약 Result가 0이아니면 0으로 넣습니다
};




/*--------------------------------------------
pInsMap::ChallengeGiveup
C -> S
파티장이 도전 포기버튼을 눌렀을때 전송된다
--------------------------------------------*/
struct C2GS_SEND_CHALLENGE_REQ_GIVE_UP{
	C2GS_SEND_CHALLENGE_REQ_GIVE_UP() : nNpcIDX(0)
	{
	}
	n4 nNpcIDX;	// 포기를 요청한 NPC 인덱스
};


/*--------------------------------------------
pInsMap::ChallengeGiveup
S -> C
도전포기에대한 응답 ( 임시 )
RESULT : CHALLENGE_RESULT (ok = 0 , fail = 1 , npcIdxError = 2)
--------------------------------------------*/
struct GS2C_RECV_CHALLENGE_RES_GIVE_UP{
	GS2C_RECV_CHALLENGE_RES_GIVE_UP() : eRes(0) , nNpcIDX(0)
	{
	}

	n1 eRes;	//포기 요청 결과값
	n4 nNpcIDX;	//서버측의 NpcIDX를 전송
};





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////////
/////////		디지몬 아레나 보상 투입
/////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


/*--------------------------------------------
pInsMap::GetChallengeDailyPoint
C -> S
게시판을 열었을때 정보갱신 요청(추후 랭킹갱신포함)
--------------------------------------------*/
struct C2GS_SEND_CHALLENGE_DAILY_GET
{
	C2GS_SEND_CHALLENGE_DAILY_GET()
	{
	}
	// X
};

/*--------------------------------------------
pInsMap::GetChallengeDailyPoint
S -> C
정보갱신에대한 응답
--------------------------------------------*/
struct GS2C_RECV_CHALLENGE_DAILY_GET
{
	GS2C_RECV_CHALLENGE_DAILY_GET() : nDay(0) , nResetTimeLeftSec(0) , nDailyPoint(0)
	{
	}
	u1 nDay;				//요일 , 0 - 일 ~ 6 - 토
	n8 nResetTimeLeftSec;	//초기화되는 시간(초)
	u4 nDailyPoint;			//현재 자신의 일일 포인트
};


/*--------------------------------------------
pInsMap::PutChallengeDailyPoint
C -> S
일일보상 포인트 투입 요청
--------------------------------------------*/

struct C2GS_SEND_CHALLENGE_DAILY_PUT
{
	C2GS_SEND_CHALLENGE_DAILY_PUT()	: lItemList()
	{

	}
	//n2 nListSize;						//투입할 아이템 리스트 사이즈
	std::list<nChallengeSt::st_ItemInfo> lItemList;	//투입할 아이템 목록
};

/*--------------------------------------------
pInsMap::PutChallengeDailyPoint
S -> C
일일보상 포인트 투입 요청에 대한 응답
RESULT : CHALLENGE_REWARD_INPUT_RESULT (ok = 0 , WRONG_ITEM = 1 , ITEM_EMPTY = 2)
--------------------------------------------*/

struct GS2C_RECV_CHALLENGE_DAILY_PUT
{
	GS2C_RECV_CHALLENGE_DAILY_PUT() : nResult(0) , nDailyPoint(0) , lUsedItemList() , lRewardItemList()
	{
	}

	u1 nResult;								//요청결과
	u4 nDailyPoint;							//투입하고난뒤 자신의 포인트
	//n2 nUsedListSize;						//실제 사용한 아이템 리스트 크기
	std::list<nChallengeSt::st_ItemInfo> lUsedItemList;	//실제 사용한 아이템들 목록	(내부의 count는 남은갯수이다 0이면 다씀)			
	//n2 nRewardListSize;						//보상으로 나온 아이템 리스트
	std::list<nChallengeSt::st_RewardItemInfo> lRewardItemList;	//보상 아이템들 목록 (순서대로 삽입)
};

/*--------------------------------------------
pInsMap::ChallengeDailyPointUpdate
S -> C
게시판 정보 갱신(날자변경으로인한)
--------------------------------------------*/

struct GS2C_RECV_CHALLENGE_DAILY_UPDATE
{
	GS2C_RECV_CHALLENGE_DAILY_UPDATE() : nDay(0) , nResetTimeLeftSec(0) , nDailyPoint(0)
	{
	}
	u1 nDay;				//요일 , 0 - 일 ~ 6 - 토
	n8 nResetTimeLeftSec;	//초기화되는 시간(초)
	u4 nDailyPoint;			//현재 자신의 일일 포인트
};






////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////////
/////////		디지몬 아레나 랭킹
/////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


/*--------------------------------------------
pGameTwo::ArenaRequestRank
C -> S
주,월,시즌 현재 랭킹 정보 요청
--------------------------------------------*/

struct C2GS_RECV_ARENA_REQUEST_RANK
{
	C2GS_RECV_ARENA_REQUEST_RANK() :protocol(pGameTwo::ArenaRequestRank) , nType(0)
	{
	}
	u1 nType;	//요청할 랭킹타입(eRankType)


	WORD GetProtocol(){return protocol;}
private:
	WORD protocol;
};

/*--------------------------------------------
pGameTwo::ArenaRequestRank
S -> C
주,월,시즌 현재 랭킹 정보 응답
--------------------------------------------*/

struct GS2C_RECV_ARENA_REQUEST_RANK
{
	GS2C_RECV_ARENA_REQUEST_RANK() :protocol(pGameTwo::ArenaRequestRank) , nType(0) , nResult(0), nPoint(0), nRank(0),
		nModifier(0), nRankType(0), nRankPercent(0), nRankRenewSec(0), nRankEndSec(0)
	{
	}
	u1 nType;				//요청한 랭킹타입(eRankType)
	u1 nResult;				//결과

	u1 nRankStatus;			//랭킹 상황(eRankStatus)

	u4 nPoint;				//자신의 포인트 - 넣은정보가없으면 0
	u4 nRank;				//자신의 절대 등수
	u4 nModifier;			//순위 변동수치
	u1 nRankType;			//자신의 랭크위치 분류(0 절대값 , 1상대값(%))
	u1 nRankPercent;		//자신의 랭크위치

	u4 nRankRenewSec;		//다음 랭킹 갱신 시간		다음랭킹 대기중일때 다음랭킹 시작시간으로 들어감 
	u4 nRankEndSec;			//현 랭킹 마감 시간			다음랭킹이 없으면 둘다 0

	//u1 nRankSize;			//Top100 랭킹 갯수(최대 100)
	std::list<nArenaRank::st_TopRankInfo> lRankList;


	WORD GetProtocol(){return protocol;}
private:
	WORD protocol;
};


/*--------------------------------------------
pGameTwo::ArenaRequestOldRank
C -> S
주,월,시즌 지난 랭킹 정보 요청
--------------------------------------------*/

struct C2GS_RECV_ARENA_REQUEST_OLD_RANK
{
	C2GS_RECV_ARENA_REQUEST_OLD_RANK() :protocol(pGameTwo::ArenaRequestOldRank) , nType(0)
	{
	}

	u1 nType;	//요청할 랭킹타입(eRankType)


	WORD GetProtocol(){return protocol;}
private:
	WORD protocol;
};

/*--------------------------------------------
pGameTwo::ArenaRequestOldRank
S -> C
주,월,시즌 지난 랭킹 정보 응답
--------------------------------------------*/

struct GS2C_RECV_ARENA_REQUEST_OLD_RANK
{
	GS2C_RECV_ARENA_REQUEST_OLD_RANK() :protocol(pGameTwo::ArenaRequestOldRank) , nType(0), nResult(0) , nRank(0)
	{
	}
	u1 nType;				//요청한 랭킹타입(eRankType)
	u1 nResult;				//결과

	u4 nRank;				//자신의 절대 등수 , 자신이 참여안해서 정보가없으면 0

	//u1 nRankSize;			//Top100 랭킹 갯수(최대 100)
	std::list<nArenaRank::st_TopRankOldInfo> lRankList;

	WORD GetProtocol(){return protocol;}
private:
	WORD protocol;
};



/*--------------------------------------------
pGameTwo::ArenaRequestFame
C -> S
명예의 전당 랭킹 정보 요청
--------------------------------------------*/

struct C2GS_RECV_ARENA_REQUEST_FAME
{
	C2GS_RECV_ARENA_REQUEST_FAME() :protocol(pGameTwo::ArenaRequestFame) , nSeason(0)
	{
	}

	u2 nSeason;	//요청할 시즌 번호 , 0을넣을경우 최신시즌으로 요청


	WORD GetProtocol(){return protocol;}
private:
	WORD protocol;
};

/*--------------------------------------------
pGameTwo::ArenaRequestFame
S -> C
명예의 전당 랭킹 정보 응답
--------------------------------------------*/

struct GS2C_RECV_ARENA_REQUEST_FAME
{
	GS2C_RECV_ARENA_REQUEST_FAME() :protocol(pGameTwo::ArenaRequestFame) , nSeason(0), nResult(0)
	{
	}

	u2 nSeason;					//요청한 시즌 번호 , 0을넣었을경우 최신시즌
	u1 nResult;					//결과


	//u1 nRankSize;				//Top50 랭킹 갯수(최대 50)
	std::list<nArenaRank::st_TopRankFameInfo> lRankList;

	WORD GetProtocol(){return protocol;}
private:
	WORD protocol;
};




/*--------------------------------------------
pGameTwo::ArenaRequestFameList
C -> S
명예의 전당 시즌 리스트 정보 요청
--------------------------------------------*/

struct C2GS_RECV_ARENA_REQUEST_FAME_LIST
{
	C2GS_RECV_ARENA_REQUEST_FAME_LIST() :protocol(pGameTwo::ArenaRequestFameList)
	{
	}
	WORD GetProtocol(){return protocol;}
private:
	WORD protocol;
};

/*--------------------------------------------
pGameTwo::ArenaRequestFameList
S -> C
명예의 전당 시즌 리스트 정보 응답
--------------------------------------------*/

struct GS2C_RECV_ARENA_REQUEST_FAME_LIST
{
	GS2C_RECV_ARENA_REQUEST_FAME_LIST() :protocol(pGameTwo::ArenaRequestFameList)
	{
	}
	//u2 nSeasonSize;				//시즌리스트(오름차순)
	std::list<u2> lSeasonList;
	WORD GetProtocol(){return protocol;}
private:
	WORD protocol;
};