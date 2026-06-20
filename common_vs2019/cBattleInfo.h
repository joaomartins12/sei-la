#pragma once

namespace eBattleMatchScore
{
	enum
	{
		MatchWin = 0,
		MatchLose,
		MatchDraw,
		MatchTotal,
		eEnd,
	};
};

namespace eBattleMatchState
{
	enum
	{
		BattleVoid			= 0,	// 기본 상태
		BattleNotMap,				// 맵이 없는 상태
		BattleMatchReady,			// 매칭 응답 요청
		BattleMatchStopResponse,	// 응답 끝남
		BattleEnd,					// 완전 종료
		BattleStart,				// 처음 경기 시작( 1라운드 시간만 기록됨 )
		BattleMatchStart,			// 경기 시작 시간
	};
};

namespace eBattleStats
{
	enum
	{
		Void		= 0,
		Wait		= 0,
		Start,
		OutCheck,	// 다른팀 한명이 들어오지 않고 있다 5초에 여유후 결과 처리
		End,
		Out,
		Diconnect,
	};
};

namespace eBattleMatchListType
{
	enum
	{
		SingleBattle		= 0,	// 싱글 배틀( 1 VS 1 )
		DualSingleBattle,			// 대결
		ObserverMakeBattle,			// 경기용 옵저버
		ObserverFastBattle,			// 바로 경기 시작용 옵저버
		RandomTeamBattle,			// 랜덤 팀 배틀( 4 VS 4 )
		PartyBattle,				// 파티 배틀
		GuildBattle,				// 길드 배틀
		ManyBattle,					// 특정 다수 배틀
		eEnd,
	};
};

// 배틀 포인트에 따른 등급
namespace eBattleMatchGrade
{
	enum
	{
		Childhood			= 1,	// 유년기
		Growth,						// 성장기
		Adulthood,					// 성숙기
		Whole,						// 완전체
		Finality,					// 궁극체
		Extremity,					// 초궁극체
		Burst,						// 버스트
		Unlimited,					// 언리밋
		eEnd,
	};
};

namespace eBattleMatchFail
{
	enum
	{
		LastRound,					// 마지막 라운드가 시작된 상태
		NotFitUserCount,			// 유저 수가 맞지 않다
		Matching,					// 매칭 중이다.
		LateResponse,				// 응담이 여부가 늦었다.
		NotInsMapMove,				// 맵을 이동할수 없는 상태
		Waiting,					// 대기중이다.
		Not_Available_Time,			// 참여 가능 시간이 아니다.

		Not_Table_Data,
		Battle_Availabe_Time = 2000, // 배틀 가능 시간
		WaitingRegSucces = 3000		// 대기자 등록 성공
	};
};

namespace eDualBattleFail
{
	enum
	{
		NotTamer = 0,				// 테이머가 없다.
		NotUser,					// 유저가 없다.
		NotMap,						// 맵이 존재 하지 않다.
		InsMapMakeFail,				// 인스맵 생성 실패
		NotInsMap,					// 인스맵이 존재하지 않음
		NotDual,					// 대결이 없어졌다.
		
		BattleMatching,				// 배틀 매칭중이다.
		MoveMap,					// 맵 이동중
		NotParty,					// 상대가 파티중이 아니다.;
		SameParty,					// 파티가 같다.
		RejectDual,					// 대결을 거절함.

		ObserverFail,				// 옵저버 등록 실패
		ObserverJoinFail,			// 옵저버 경기 집입 실패

		NPCTalk,					// NPC와 용무중 실패
		Party,						// 파티중 실패
		Battle,						// 전투중 실패
	};
};

// 듀얼배틀 응답 값
namespace eDualBattleRequest
{
	enum
	{
		battle_OK = 1,		// 수락
		NPCTalk_NO,			// npc대화중 거절
		Party_NO,			// 파티중 거절
		Battle_NO,			// 전투중 거절
	};
};
namespace eBattleMatchEndStats
{
	enum
	{
		Shut_Down = 0,
		BattleTimeOver,				// 배틀 종료 지남
		MatchTimeOver,				// 경기 종료 시간
		BattleResult,				// 승부 여부가 나옴
		BattleTamerOut,				// 배틀중 테이머가 나감
		//BattleGiveUp,
	};
};



#ifdef NEW_BATTLE_SYSTEM
namespace eBattleMainMode
{
	enum
	{
		TEAM_BATTLE = 1,		// 팀 대전
	};
};

namespace eBattleSubMode
{
	enum
	{
		NONE		= 0		,		// 없음
		ONE_ON_ONE	= 1		,		// 1 vs 1
		TWO_ON_TWO			,		// 2 vs 2
		THREE_ON_THREE		,		// 3 vs 3
		FOUR_ON_FOUR		,		// 4 vs 4
		EIGHT_ON_EIGHT		,		// 8 vs 8
		SIXTEEN_ON_SIXTEEN	,		// 16 vs 16

		MAX_BATTLE_SUB_TYPE = SIXTEEN_ON_SIXTEEN,	// 배열 인덱스로 사용
	};
};

// 타입별 인원 최대수(팀별)
namespace eBattleTeamMemberMax
{
	enum
	{
		VS_1_MAX	= 1,
		VS_2_MAX	= 2,
		VS_3_MAX	= 3,
		VS_4_MAX	= 4,
		VS_8_MAX	= 8,
		VS_16_MAX	= 16,
	};
};

namespace eBattleEntry
{
	enum
	{
		NONE = 0,
		RANDOM,				// default는 random 참여
	};
};

namespace eBattleTeamType
{
	enum
	{
		NONE = 0,			// 소속된 팀 없음
		RED,
		BLUE,
	};
};

namespace eBattleMatchStats_new
{
	enum
	{
		NONE			= 0	,	// 기본 상태
		AWAITING_RESPONSE	,	// Play 응답 대기중
		END_RESPONSE		,	// 응답 완료
		BATTLE_PLAY			,	// 플레이 중
		BATTLE_END			,	// 배틀 종료
		TIME_OVER			,	// 종료 시간 지남

	};
};

namespace eBattleTamerStats
{
	enum
	{
//		NONE			= 0	,
		MATCH_WAITING		,	// 매칭 대기중
//		MATCH_COMPLETE		,	// 매칭 완료
//		BATTLE_PLAY_READY	,	// 플레이 대기 중
//		BATTLE_PLAY			,	// 플레이 중
//		BATTLE_END			,	// 배틀 종료
//		DIE					,	// 죽음
//		OUT					,
//		DISCONNECT			,
	};
};

namespace eBattleTimeOpen
{
	enum
	{
		OFF  = 0,
		ON
	};
};


namespace eErrorBattleMatch
{
	enum
	{
		BATTLE_ENTRY_REQUEST_SUCCESS	= 0	,	// 배틀 참여 요청 성공중
		BATTLE_ALREADY_REQUEST				,	// 이미 참여 요청 함
		BATTLE_PLAYING						,	// 배틀 플레이중
		BATTLE_READY_WAITING				,	// 배틀 시작 대기중
		BATTLE_DIGIMON_LOW_LEVEL			,	// 참여 가능한 digimon lv이 낮음
		BATTLE_MAIN_MODE_NO_DATA			,	// 배틀 메인 모드의 데이타가 없다.
		BATTLE_SUB_MODE_NO_DATA				,	// 배틀 하위 모드의 데이타가 없다.

		NOT_AVAILABLE_TIME					,	// 참여 가능 시간이 아니다.

		NOT_TABLE_DATA						,
		BATTLE_AVAILABE_TIME		= 2000	,	// 배틀 가능 시간
		WAITINGREGSUCCES			= 3000	,	// 대기자 등록 성공
	};
};
#endif // NEW_BATTLE_SYSTEM


#define BATTLE_MAP_COUNT			1

#define TIME_RESPONSE				5	// 응답시간(초)
#define TIME_BATTLE_BEFORE			30	// 배틀 시작전 시간(초)
#define TIME_BATTLE_AFTER			10	// 배틀 후 시간(초)
#define TIME_MATCH_BEFORE			10	// 경기 전 주어진 시간(10초) 버프 쓸시간
#define TIME_TAMER_OUT_CHECK		5	// 5초내에 들어오지 못하면 게임끝

#define BATTLE_ENTRY_TAMER_MIN_LEVEL	30	// 배틀 참여 가능한 최소 Tamer 레벨
#define BATTLE_ENTRY_DIGIMON_MIN_LEVEL	1	// 배틀 참여 가능한 최소 디지몬 레벨


struct stCompensation
{
	// 0 : 승, 1 : 패, 2 : 무
	u4				nCoinCount[ eBattleMatchScore::MatchTotal ];
	u4				nBattlePoint[ eBattleMatchScore::MatchTotal ];
	n8				nBattleMoney[ eBattleMatchScore::MatchTotal ];
	u4				nBattleExp[ eBattleMatchScore::MatchTotal ];
	stCompensation()
	{
		/*ZeroMemory( nCoinCount, sizeof( nCoinCount ) );
		ZeroMemory( nBattlePoint, sizeof( nBattlePoint ) );
		ZeroMemory( nBattleMoney, sizeof( nBattleMoney ) );
		ZeroMemory( nBattleExp, sizeof( nBattleExp ) );*/
	}
};