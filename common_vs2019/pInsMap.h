

#pragma once


#include "nScope.h"

namespace nInsMap
{
	enum eTYPE
	{
		FreeBattle,
		League,
		Dungeon,
	};

	enum EnterCondition
	{
		NEED_ITEM		= 1,
		NEED_QUEST		= 2,
		HIGH_TAMERLEVEL	= 3,
		NEED_MONEY		= 4,
		LOW_TAMERLEVEL	= 5,
		HIGH_DIGIMONLV	= 6,
		LOW_DIGIMONLV	= 7,
	};
}

namespace pInsMap
{
	enum eInsMap
	{
		Begin = nScope::InsMap,

		Ready,				// 배틀 준비

		InsMapMove,			//	인스 맵으로 이동
		InsMapCome,			//	인스 맵에서 복귀

		Free_NotFound,

		InsMap_ID,


		GM_RemoveMap,

		BattleMemberInfo,		// 배틀 맴버 정보
		BattleInsReady,			// 배틀 준비상태( 들어갈 테이머 IDX, 경기시작끝시간 )
		BattleStart,			// 배틀 시작을 알려줌
		BattleEnd,				// 배틀 끝을 알려줌
		BattleJoin,				// 배틀 들어옴
		BattleReadyComplete,	// 배틀 준비 완료
		BattleGiveUp,			// 배틀 포기
		DualMove,				// 결투 이동
		BattleTeamSync,			// 배틀팀 동기화

		ObserverBattleJoin,		// 옵저버 경기 조인
		ObserverBattleJoinResult,// 옵저버 경기 조인 결과
		ObserverBattleInfoList,	// 옵저버 목록 리스트


		DungeonIN,
		DungeonCreate,
		DungeonFail,
		DungeonMove,
		DungeonPartyOut,
		ReturnMap,				// 끝나고 돌아간다.

//#ifdef INFINITY_CHALLENGE_TEMP
		//무한대전 추가 패킷
		ChallengeStageClear,
		ChallengeTryNextStage,
		ChallengeGiveup,
		ChallengePartyAlreadyPlay,
		ChallengeStageTimeSave,


		GetChallengeDailyPoint,
		PutChallengeDailyPoint,
		ChallengeDailyPointUpdate,
//#endif
		DungeonRoomIn,	// 특정 방 번호로 직접 들어갈수있다
		DungeonCreateCount,		//통계파악용


		End
	};
};