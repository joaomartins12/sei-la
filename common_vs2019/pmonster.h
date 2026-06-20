

#pragma once


#include "nScope.h"
#include "pParameter.h"

#include "pSync.h"
class cMonsterSource;


namespace pMonster
{
	enum
	{
		Begin = nScope::Monster,

		Die,
		NatualExp,
		GetNatualItem,
		MsgRaidRankingList,		// HSS 레이드 순위표시 20130108
		MsgRaidMonsterDie,		// HSS 레이드 몬스터 잡았을 시에 전광판
		RandomMonsterCreate,	// 랜덤 몬스터 리젠
		RandomMonsterSuccess,	// 랜덤 몬스터 리젠 성공
		RandomMonsterFail,		// 랜덤 몬스터 리젠 실패
		RandomMonsterEnd,		// 랜덤 몬스터 맵에서 제거 됨

		End
	};
};

namespace nMonster
{
	enum eBattle
	{
		Guarder,	// passive attacker
		Helper,		// party helper	, 190319 - 망부석용으로 사용하겠다고해서 변경
		Attacker,	// auto attacker

		MaxBattle
	};

	enum eClass
	{
		Common = 1,	// 일반 몬스터
		Boss   = 2,	// 보스 몬스터
		Event  = 3,	// 이벤트 몬스터
	};

	enum eAttribute
	{
		Begin = MaxExtStat,

		DigimonNo,	// 대상 디지몬 Source IDX
		//Radius,		// 몬스터 반경, 스케일 적용값

		Class,		// 일반, 보스, 이벤트
		Sight,		// 시야
		Chase,		// 추격 거리

		MoveRate,	// Monster's AI 이동 빈도
		WalkSpeed,	// 걷기시 속도

		Battle,		// battle type

		EvoStep,	// 진화 단계

		AttrBase,	// 기본 속성, No, Da, Va, Vi, Un
		AttrNature,	// 자연 속성
		AttrFamily1,//
		AttrFamily2,//
		AttrFamily3,//

		MonsterScale,

		MaxAttribute
	};

	enum
	{
		ClassNormal = 1,
		ClassElite,
		ClassNormalBoss,
		ClassEliteBoss,
		ClassSpecial,
		ClassRaid,
		ClassBlack,				// HSS 블랙톱니바퀴 일반
		ClassBlackRaid,			// HSS 블랙톱니바퀴 레이드
		ClassBuffRaid,			// HSS 버프 레이드
	};

	enum eReward
	{
		RewardNormal = 2,		// HSS 어그로 최대 순위 유저 (1 -> 2로 변경)
		RewardRanking = 4,		// HSS 어그로 순위 방식
		RewardDice = 8,			// HSS 주사위 방식
		RewardParty = 16,		// 딜량 1위의 파티
	};

	enum UnknowPercent
	{
		DIgitama		= 1500,		// 디지타마
		Training		= 2500,		// 유년기
		Rookie			= 3500,		// 성장기
		Champion		= 4500,		// 성숙기
		Ultimate		= 5500,		// 완전체
		Mega			= 6500,		// 궁극체
		BusterMode		= 7500,		// 초궁극체
		Joint			= 10000,	// 조그레스
		UPCapsuleEvo	= 0,		// 캡슐진화
		UPSpiritEvo		= 0,		// 스피릿진화
		UPRookieX		= 3500,		// 성장기X
		UPChampionX		= 4500,		// 성숙기X
		UPUltimateX		= 5500,		// 완전체X
		UPMegaX			= 6500,		// 궁극체X
		UPBurstModeX	= 7500,		// 초궁극체X
		UPCombineModeX	= 10000,	// 합체X
		UPExtraEvo		= 7500,
	};

	enum eSkillMotion
	{
		NoMotion	= 0,
		CastSkillAni,			// 캐스팅 애니중
		CastSkillDmg,			// 캐스팅 후 데미지
		MonsterSummon,			// 몬스터 소환
		MonsterBuffStack,		// 몬스터 버프 스택 
		MonsterGather,			// 지역몬스터 보스몬스터로 이동
		Gathering,				// 모여서 같이 맞아야 되는 스킬(범위안의 유저수 만큼 데미지 분산)
		Disperse,				// 흩어져서 따로맞아야 되는스킬(범위안의 유저수 만큼 데미지 증가)
		Range_Persist,			// 범위 지속형 스킬
		Berserk,				// 광폭화
		NoticeTime,				// 예고 이펙트 중 ( 대기시간 )
		SingleStackDeBuff,		// 단일 디버중첩 공격
		RegionBuffNesting,		// 일정지역에 버프 쌓임
		Rush,					// 돌진 진행중
		RandomAoE,				// 광역 랜덤장판
		ChainBounce,			// 튕기는 스킬
		Exterminate,			// 전멸기
		SummonPos ,				// 특정 위치에 소환
	};
	enum eSkillTargetType
	{
		None					= 0,		// 타겟타입이 없음 (현재 몬스터가 바라보고있는 대상)
		Aggro					= 1,		// 어그로 순으로 정렬하여 결정
		HP						= 2,		// 현재 HP순으로 정렬하여 결정
		Random					= 3,		// 무작위
		Near					= 4,		// 가까운 순으로 정렬하여 결정
		OtherMonsterAggro		= 5,		// 다른 몬스터 어그로 순위
		BuffTarget				= 6,		// 특정 버프 적용자
	};

	enum eReserveActions
	{
		MonsterRemove = 1,		//몹을 없애버립니다

	};
}; // namespace nMonster

struct sObjectMove		// 값이 더 있을수 있다.
{
	u1				nType;		// 0 = 시작, 2 = 이동, 3 = 끝( 다시 0으로 시작 )
	nSync::Pos		pos;		//이동해야할 포지션
};

class cObjectMove
{
public:
	cObjectMove()		{ memset( m_sObjectMove, 0, sizeof( m_sObjectMove ) ); m_nMaxMove = 0; }
	~cObjectMove() {};

public:
	u1				m_nMaxMove;
	sObjectMove		m_sObjectMove[ 30 ];

	nSync::Pos		GetNextPos( u1 nMove )		{ return m_sObjectMove[ nMove ].pos; }
	sObjectMove		GetObjectMove( u1 nMove )	{ return m_sObjectMove[ nMove ]; }
};

struct stMonsterInfo
{
	u4				nType;
	u4				nCX;
	u4				nCY;
	u4				nCR;
	u4				nLimit;
	u4				nTermTick;
	u1				nMove;

	u4				nBossIDX;
	u4				nBossSeedCount;
	u4				nBossCount;

	cMonsterSource* pMosterSource;
};



