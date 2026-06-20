#pragma once
#include "..\nScope.h"
#include <string>


// 네트워크 패킷 메세지
namespace pDuel
{
	enum
	{
		Begin		= nScope::Duel,

		Info,			// 대결 정보 요청	c->s : 
		//					s->c : n4 duelTicket, n4 modeCount, [n4 현재 참여 카운트]
		MatchRegister,	// 대결 참여 요청	c->s : n1 mainMode, n1 subMode, n4 Npc_idx
		//					s->c : n4 result, n1 mainMode, n1 subMode

		MatchRegiCancel, // 대결 참여 취소	c->s : 
		//					s->c : n4 result

		TeamReadyInfo,	// 매칭 정보	-> 배틀 모드, 테이머의 파트너몬 정보
		//					s->c : n4 DuelIndex, n4 MapIndex, n1 mainMode, n1 subMode, n4 teamCount, u1 teamCode, n4 memberCount, [n8 tamerIDX, string TamerName, n4 DigimonTableIndex]

		Ready,			// 대결 준비		c->s:  n1 0: 취소 1: 확인, n4 nDuelIDX
		//					s->c: n1 nReply, n4 nErrorCode

		ReadyResultNoti,	// 다른 유저들 정보 
		//				s->c: bool result, n8 nTAmerIDX

		RemoveDuel,			// 듀얼 삭제 ( )
		//				s->c: n4 nReason,

		ReadyComplete,		// 모든 멤버 대결 준비 완료.
		//				s-c: 

		Start,			// 대결 시작	-> 대기시간, 종료 시간, 플레이 시간, 시작시간,

		End,			// 대결 종료
						//			s-c: n1 배틀 상태, n1 배틀 결과, n1 승리 팀

		Reward,			// 대결 결과

		MemberSync,		// 멤버들 상태 동기화

		MatchSync,		// 배틀 동기화

		DuelZoneSet,	// duel 던전 설정 패킷. 서버에서만 사용

		TeamPartyInfo,	// 게임방에 입장한 유저에게 같은팀원의 파티 정보(1~4개의 파티)  ex) 3vs3 에서는 1개의파티 정보, 16vs16 에서는 4개의 파티가 생성?
		//byte count // 파티 수
		//for(;;)		
		//{
		//	u4 tamerIndex
		//	byte slot
		//}

		JoinTeamPartyMemberInfo, // 게임방에 유저가 입장했을 시 기존 멤버들에게 입장 유저의 파티Index, Slot정보 전달
		//u4 tamerindex
		//byte	slot
		
	};
}

// 듀얼에서 사용할 enum 
namespace eDuelCategory // 대분류
{
	enum
	{
		TEAM	 = 1,		//	Team 대전
		DEFENSE,		//  방어전	( 미구현 )

		MAX,		// 배열 Max
	};
}

namespace eDuelActivate
{
	enum
	{
		OFF		,		// 비활성화
		ON		,		// 활성화
	};
}

namespace eDuelSubCategory
{
	enum
	{
		NONE				,		// 없음
		SINGLE_DUEL			,		// 1 vs 1
		DOUBLE_DUEL			,		// 2 vs 2
		TRIPLE_DUEL			,		// 3 vs 3
		QUADRUPLE_DUEL		,		// 4 vs 4
		OCTUPLE_DUEL		,		// 8 vs 8
		SEXDECUPLE_DUEL		,		// 16 vs 16
	};
}

namespace eDuelTeamType
{
	const n1 RED	= 1;
	const n1 BLUE	= 2;
}

namespace eDuelRecord
{
	enum
	{
		TOTAL	,		// 전체 전적
		WIN		,		// 승
		DRAW	,		// 무승부
		LOSE	,		// 패
	};
}

namespace eDuelMemberStats
{
	enum
	{
		NONE						,	// 일반 상태
		DUEL_MATCH_WATING			,	// 매칭 대기중
		PLAY_DUEL					,	// 전투중
		DUEL_ENTER_REPLY_WAITING	,	// 전투 참여 응답 대기 중
		DUEL_ENTER_REPLY_OK			,	// 전투 참여 확인
		DUEL_ENTER_REPLY_CANCEL		,	// 전투 참여 취소
		// 전투 참여 미 응답,
		// 로그 아웃
		// 죽음(digimon의 죽음)
	};
}

namespace eDuelStats
{
	enum
	{
		AWAITING_RESPONSE	,	// 참여 인원 응답 대기중
		PLAY				, 	// 플레이 ( 게임 플레이 시간 )
		CANCEL				,	// 취소
		END					,	// 종료
	};
}


namespace eDuelSubCategoryMaxTeamCNT
{
	enum 
	{
		SINGLE			= 1 ,		// team 인원 1명
		DOUBLE			= 2 ,		// team 인원 2명
		TRIPLE			= 3 ,		// team 인원 3명
		QUADRUPLE		= 4 ,		// team 인원 4명
		OCTUPLE			= 8	,		// team 인원 8명
		SEXDECUPLE		= 16,		// team 인원 16명
	};
}


struct STRUCT_SCORE_RECORD
{
	int total;		// 전적
	int win;		// 승
	int draw;		// 무승부
	int lose;		// 패
	u8	point; // 포인트
public:
	void init()
	{
		total = 0; win = 0; draw = 0; lose = 0; point = 0;
	}

	void incrementSocre(n1 nScoreType, n8 addPoint)
	{
		total = total + 1;

		switch(nScoreType)
		{
		case eDuelRecord::WIN:
			win++;
			point += addPoint;
			break;
		case eDuelRecord::DRAW:
			draw++;
			point -= addPoint;
			if (point < 0) point = 0;
			break;
		case eDuelRecord::LOSE:
			lose++;
			point -= addPoint;
			if (point < 0) point = 0;
			break;
		};
	}
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// 메인 모드별 데이터
struct sMainTypeData
{
	n4		m_nTotalPlayCount;		// 메인 모드별 총 플레이 횟수
	u8		m_nTotalRankPoint;	// 메인 모드별 랭킹 포인트
};

// 배틀 등록한 플레이어 정보
struct sBattlePlayerInfo
{
public:
	sBattlePlayerInfo():m_uDigimonTableIdx(0),m_uTamerUIDX(0)
	{}
	~sBattlePlayerInfo(){}

	u4				m_uDigimonTableIdx;
	u4				m_uTamerUIDX;
	std::wstring	m_nTamerName;
};

// 배틀 결과 보상
struct sRewardItemInfo
{
public:
	sRewardItemInfo():m_nItemCode(0), m_nItemCount(0)
	{}

	~sRewardItemInfo(){}

	n4	m_nItemCode;			// 아이템 갯수
	n4	m_nItemCount;			// 아이템 갯수
};


struct sBattleTeamSync
{
public:
	sBattleTeamSync():m_nTamerUID(0),m_unTamerType(0),m_unTamerLv(0),m_unTamerMHP(0),m_unTamerMDS(0)
		,m_nDigimonUID(0),m_unCurrentEvoType(0)
	{
		ZeroMemory( &m_nStat, sizeof(m_nStat ) );
	}

	~sBattleTeamSync(){}

	u2	m_nTamerUID;
	u4	m_unTamerType;
	u4	m_unTamerLv;
	u4	m_unTamerMHP;
	u4	m_unTamerMDS;

	u2	m_nDigimonUID;
	u4	m_unCurrentEvoType;
	n2	m_nStat[eBaseStats::MaxBaseStat];
};
