

#pragma once


#include "nScope.h"

namespace pBattle
{
	enum eBATTLE_OPTION
	{
		BO_NONE			= 0x00000000,
		BO_CRITICAL		= 0x00000001,
		BO_BLOCK		= 0x00000002,
		BO_KNOCKBACK	= 0x00000004,
		BO_BUFF			= 0x00000008,
	};
};

namespace pSkill
{
	enum eSkill
	{
		Begin = nScope::Skill,

		Apply,			// 처음 사용시
		ApplyAround,	// 스킬 또는 약물 사용시 주변에 알림
		Repeat,			// 일정 간격으로 계속 적용하는 skill

		LevelUp,		// 스킬 레벨업, 소유 스킬에 포인트 할당
		PointUp,		// 2포인트 부여 받음

		Error,			// hp, ds 등의 수치 부족, 타겟이 사라진 경우

		ExpUp,			// 스킬 경험치 상승

		SaveElapseTS,	// 날짜가 지나... 스킬 관련 시간이 초기화 되어야하는 경우
		SlotOpen,		// 스킬 슬롯 오픈

		AroundSkill,	// 스킬 알리는 방식 추가

		AroundSkillAni,
		AroundSkillDmg,
		RickySkill,

		Summon,			//테이머 스킬 중 GMSummon과 비슷한 현상을 발생하는 스킬
		MonsterBuff,	//몬스터 버프스킬 사용
		RangeSkillDmg,	//광역스킬 데미지 표시

		ActiveSkillEff,	//특수효과 붙은 스킬 (액티브)

//#ifdef KSJ_ADD_MEMORY_SKILL_YYYYMMDD
		AddSkillChip,	// 특성 스킬 추가
		RemoveSkillChip,// 특성 스킬 삭제
		UseSkillChip,	// 특성 스킬 사용 1120
		ChipCoolTime,	// 특성 스킬 쿨타임
		ResultSkillChip,// 특성 스킬 사용 결과
//#endif
//#ifdef JHB_ADD_RAID_MONSTERSKILL_20140916
		MonsterSkillEffect,	//몬스터 스킬 사용
		Range_PersistEnd,	// 범위 지속형 스킬 사라짐
//#endif
		UpdateDigimonSkillLimit,
		End
	};
};

namespace nTarget
{
	enum
	{
		HomeTamer			= 11,	// 자신의 테이머 1명	/* 사용안함 */
		HomeDigimon			= 12,	// 자신의 디지몬 1명	/* 사용안함 */
		HomeDigimons		= 13,	// 자신의 디지몬 전체	/* 사용안함 */

		DigimonAroungTarget	= 17,	// 디지몬 주변 적 몬스터 전체

		AoEonTargetMonster	= 18,	// 타격한 몬스터 주변 전체

		AwayTamer			= 31,
		AwayDigimon			= 32,
		AwayDigimons		= 33,

		PartyTamer			= 21,
		PartyDigimon		= 22,
		PartyTamers			= 23,
		PartyDigimons		= 24,

		AwayPartyTamers		= 41,
		AwayPartyDigimons	= 42,

		Monster				= 51,
		PartyMonsters		= 52,	// 적 몬스터 파티 전체

		Field				= 61,

		AroundTamer			= 62,
		AroundDigimon		= 63,

		Rush = 80, //자신의 디지몬에서 적 타겟 위치까지 직선 범위에 몬스터 전부

		MaxTargetNumber
	};

	enum NEW_TARGET
	{
		MY_ALL	=	11,
		TAMER	=	12,
		DIGIMON =	13,

		MY_PALL	=	25,	// 내 파티원 전체

		MONSTER =	31,

		// 2017-08-31 GyeongSu. Seo.
		// Buff 아이템 관련.
		Enemy_D =	36, // 상대 테이머의 디지몬 1명

		ANOTHER_ALL = 55,
		ANOTHER_T = 56,
	};
};


namespace nSkill
{
		//M0 =   1, // damage 계산
		//M1 =   2, // skill damage 계산
		//M2 = 101, // A=A+B
		//M3 = 102, // A=A+(A*B/100)
		//M4 = 103, // A=A*B/100
		//M5 = 104, // A=B

		//M6 = 201, // B초 동안 A=A+C
		//M7 = 202, // B초 동안 A=A+(A*C/100)
		//M8 = 203, // B초 동안 A=A*C/100
		//M9 = 204, // B초 동안 A=C
	enum eMethod
	{
		Me_001 =   1, // damage 계산
		Me_002 =   2, // skill damage 계산
		Me_101 = 101, // A=A+B
		Me_102 = 102, // A=A+(A*B/100)
		Me_103 = 103, // A=A*B/100
		Me_104 = 104, // A=B
		Me_105 = 105, // A=A+(A*B/100)

		Me_201 = 201, // B초 동안 A=A+C
		Me_202 = 202, // B초 동안 A=A+(A*C/100)f
		Me_203 = 203, // B초 동안 A=A*C/100
		Me_204 = 204, // B초 동안 A=C

		Me_106 = 106, // A=A+(A*B/100)
		Me_107 = 107, // 상태 이상
#ifdef LJS_NEW_COMBINE_EVO_2012_09_07
		Me_108 = 108, // A = A
#endif
#ifdef HSS_NEW_BUFF_UNBEATABLE_20130114
		Me_200 = 200, // 공식적용 없음
#endif
		Me_205 = 205,	// 도트 데미지 공식 <- 2014.04.16 패치 버전에 추가 된 효과 타입
#ifdef KSJ_ADD_DIGIMON_BUFF_SKILL_20140408
		Me_206 = 206,	// 스킬 데미지 증가(수치) 버프 <- 2014.04.16 패치 버전에 추가 된 효과 타입
		Me_207 = 207,	// 스킬 데지미 증가(비율) 버프 <- 2014.04.16 패치 버전에 추가 된 효과 타입
#endif
#ifdef KSJ_ADD_DIGIMON_UNBEATABLE_20140428
		Me_208 = 208, // TIme = Time + (SkillLv * Apply)
#endif
		Me_301 = 301, // A= A-B
		Me_302 = 302, 
		Me_010 =  10, // Tamer Skill Damage 계산 (AT * B) + {AT * (1 + Random(0~200) / 100) * B} - [{MD / (TL / ML) * 100} * 100]
		Me_401 = 401, // 이벤트 동기화 // 폭죽등
		Me_402 = 402, // 디지몬 스케일 임시 변경 // 수수께끼 성장열매 같은거
		Me_403 = 403, 
		Me_404 = 404, 
	};

	enum eAttribute
	{
		PM1,			// 적용 방법
		PA1,			// 파라메타 A
		PB1,			// 파라메타 B
		PC1,			// 파라메타 C
		PT1,			// 파라메타 타겟

		PM2,			// 적용 방법
		PA2,			// 파라메타 A
		PB2,			// 파라메타 B
		PC2,			// 파라메타 C
		PT2,			// 파라메타 타겟

		PM3,			// 적용 방법
		PA3,			// 파라메타 A
		PB3,			// 파라메타 B
		PC3,			// 파라메타 C
		PT3,			// 파라메타 타겟

		LvUpPt,			// level up point
		MaxLevel,		// max level
		ReqLevel,		// require level, open 요구 레벨

		Apply1,			// 증가 수치
		Apply2,
		Apply3,

		UseHP,			// 소요 HP
		UseDS,			// 소요 DS

		aDigital,		// 디지탈속성
		aNature,		// 자연속성
		aFamily,		// 속성

		Target,			// 대상물
		RangeType,		// 공격 타입

		Range_MinDmg,	// 공격 최소 거리
		Range_NorDmg,	// 공격 적정 거리
		Range_MaxDmg,	// 공격 최적 거리
		Range,			// 공격 거리
		Radius,			// 적용 반경

		AttackTick,		// 사용아니함
		AppTimeTS,		// 적용 시간(초)
		tInterval,		// 적용 간격(초)
		tRecovery,		// 회복 시간

		Speed,			// 발사체 진행 속도
		Acceleration,	// 발사체 진행 가속도
		EndTime,		// 스킬이 끝나는 순간
		ReadyTime,		// 스킬 사용전 준비 시간

		Aggro,			// 적대 수치


		Price,			// 스킬 구입 가격


		Class,			// 스킬 계열(스킬 클레스)
		Skill_Type,		// 스킬 타입 ( 5: 버프, 6: 디버프 7: 패시브 )
		//Level,			// 스킬 레벨(동일 계열내에서의 스킬 우선 순위)
		//BuffBit,		// 경험치 가속 부스터인 경우 관련 버프 비트 존재
		//BuffBitByParty,	// 파티에 의한 경험치 가속 부스터인 경우 관련 버프 비트 존재

		//AppType,		// 적용 타입 : 0.무관, 1.최초것우선, 2.최후것우선, 3.레벨비교, 4.누적(시간,데미지)
		//AppIcon,		// buff,debuff,DOT 타입

		//AppTarget,		// 적용 대상 : 0.무관, 1.자신->자신, 2.상대->자신
		//AppTargetLevel,	// 적용 레벨 : 스킬 적용 대상의 제한 레벨, 해당 레벨 이상 되어야 적용 가능

		//ReqItemType,	// 스킬 사용시 소모되는 아이템 타입
		//ReqItemCount,	// 스킬 사용시 소모되는 아이템 갯수

		PRate1,
		PRate2,
		PRate3,
//#ifdef KSJ_ADD_MEMORY_SKILL_20140805
		SkillGroup,		// 스킬 그룹. 같은 계열을 표시함.
		SkillRank,		// 스킬 랭크. 스킬의 레벨.
		MemorySkill,	// 스킬 삭제 여부. 0 이면 삭제 불가능. 1~3 : 메모리스킬(공격,방어,보조형)이면 삭제 가능.
//#endif


		MaxAttribute
	};

	enum eSKILL_TYPE
	{
		SKILL_BUFF		= 5,
		SKILL_DEBUFF	= 6,
		SKILL_PASSIVE	= 7,
	};

	extern int GetReachedTime(int nDistance, int nSpeed, int nAcceleration);

	enum eMemory_Skill_Type
	{
		UNKNOWN_MEMORY_SKILL_TYPE = 0,	// 메모리 스킬이 아님
		MEMORY_SKILL_ATK = 1,			// 공격형 메모리스킬
		MEMORY_SKILL_DEF = 2,			// 방어형 메모리스킬
		MEMORY_SKILL_AST = 3,			// 보조형 메모리스킬
	};
};








