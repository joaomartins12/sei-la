#pragma once

#include "pSkill.h"
#include "pLength.h"
#include <vector>

class cSkillSource
{
public:
	cSkillSource(void);
   ~cSkillSource(void);

public:
	uint get(uint nAttr);
	void set(uint nAttr, uint nVal);

	int GetReachedTime(int nDistance); // return ms

public:
	uint GetIDX(void) { return m_nIDX; }
	char *GetName(void) { return m_szName; }

	int GetNatureAttribute(void) { return get(nSkill::aNature); }
	int GetDigitalAttribute(void) { return get(nSkill::aDigital); }

public:
	void SetIDX(uint idx) { m_nIDX = idx; }
	void SetName(char *name) { strcpy_s(m_szName, name); }

//public:
//	bool IsExpBuff(void);		// 개인 가속 부스터
//	bool IsExpBuffParty(void);	// 파티 가속 부스터
//	bool IsExpBuffExt(void);	// 기타 가속 부스터

//#ifdef SERVER_KOREANDNS_DIGIMONSKILLMODE_190117
public:
	n4	nEffectFactor[6];			// 효과 팩터				/* nEffect 값에 따라 사용처 변경 */
//#endif

//////////////////////////////////////////////////////
// internal data
private:
	uint m_nIDX;
	// 2011_04_01 스킬이름 오버 플로우발생에 따라 스킬이름 최대 16글자로 확대
	char m_szName[Language::pLength::SkillName+1]; // skill name
private:
	uint m_nAttribute[nSkill::MaxAttribute];
};

//class cSkillSource
//{
//public:
//	cSkillSource(void);
//   ~cSkillSource(void);
//
//public:
//	int get(uint nAttr);
//	void set(uint nAttr, uint nVal);
//
//	int GetReachedTime(int nDistance); // return ms
//
//public:
//	uint GetIDX(void) { return m_nIDX; }
//	char *GetName(void) { return m_szName; }
//
//	int GetNatureAttribute(void) { return get(nSkill::aNature); }
//	int GetDigitalAttribute(void) { return get(nSkill::aDigital); }
//
//public:
//	void SetIDX(uint idx) { m_nIDX = idx; }
//	void SetName(char *name) { strcpy_s(m_szName, name); }
//
////public:
////	bool IsExpBuff(void);		// 개인 가속 부스터
////	bool IsExpBuffParty(void);	// 파티 가속 부스터
////	bool IsExpBuffExt(void);	// 기타 가속 부스터
//
//
////////////////////////////////////////////////////////
//// internal data
//private:
//	uint m_nIDX;
//	// 2011_04_01 스킬이름 오버 플로우발생에 따라 스킬이름 최대 16글자로 확대
//	char m_szName[Language::pLength::SkillName+1]; // skill name
//
//private:
//	int m_nAttribute[nSkill::MaxAttribute];
//};

struct stMonsterSkill_Terms
{
	u4	nSkillCode;		// 스킬 코드
	u1	nTargetType;	// 타켓팅 방식
	u1	nDirectionType;	// 방향성 사용 여부
	u4	nDirectionPos;	// 공격 위치
	u4	nEffectRange;	// 공격 영역
};
#ifdef JHB_ADD_RAID_MONSTERSKILL_20140916
struct stMonsterSkill
{
	u4	nIndexNo;				// 인덱스 넘버
	u4	nDigimonCode;			// 디지몬코드
	u1	nGlobalCoolType;		// 글로벌 쿨 타입
	u4	nGlobalCoolTS;			// 글로벌 쿨 시간
	u4	nCoolTS;				// 쿨 시간
	u4	nCastTS;				// 캐스팅 시간
	u1	nMoveCastType;			// 무빙 캐스팅 타입
	u4	nBeginDistance;			// 시전 시작 사정거리
	u4	nMaximumUse;			// 최대사용 횟수
	u4	nConsumeDS;				// 감소DS
	u2	nTargetType;			// 타겟 타입
	u1	nTargetNumber;			// 대상 인원
	u2	nTargetMinvalue;		// 인원범위 최소
	u2	nTargetMaxvalue;		// 인원범위 최대	
	u1	nTerms;					// 실행 타입
	u4	nTermsValue;			// 실행 최대값			// 1000 분률	// 시작 조건
	u4  nTermsMin;				// 실행 최소값							// 종료 조건
	u1	nEffect;				// 효과 타입			// 스킬 타입
	u4	nMinEffectValue;		// 효과 최소 값
	u4	nMaxEffectValue;		// 효과 최대 값
	u4  nSkillNo;				// 디지몬리스트에 있는 스킬번호
	u4	nSkillDelay;			// 스킬 애니메이션 시간
	u4	nSkillVelocity;			// 스킬 속도
	u4	nSkillAccel;			// 스킬 가속도
	n4	nEffectFactor[6];		// 효과 팩터				/* nEffect 값에 따라 사용처 변경 */

	// MONSTER_SKILL_RENEWAL_20150715
	n1	nActiveType;			// 동작 타입			// 0 자신 주변, 1 타켓, 2 좌표
	u4  NoticeTime;				// 예고 시간 ms
	n4  nLinkType[5];			// DM_Buff-BuffCode 값 or MonsterSkill		/* MAX_MONSTER_BUFF_SKILL_COUNT */
	n4	nLinkCode[5];			// DM_Buff-BuffCode 값 or MonsterSkill		/* MAX_MONSTER_BUFF_SKILL_COUNT */
	n4  nLinkRate[5];			// nLinkCode의 동작 확율 1/10000

	n1  nAttackType;			// 공격 타입 0 즉시, 1 지연 ( 발사체 도달시간 )
	n4	nApplyTarget;			// 버프 혹은 디버프 대상

	//u4	nSkillCodeNo;		// 스킬코드 넘버
	stMonsterSkill_Terms*	pSkillTerms;
};
#else
struct stMonsterSkill
{
	u4	nIndexNo;				// 인덱스 넘버
	u4	nDigimonCode;			// 디지몬코드
	u1	nGlobalCoolType;		// 글로벌 쿨 타입
	u4	nGlobalCoolTS;			// 글로벌 쿨 시간
	u4	nCoolTS;				// 쿨 시간
	u4	nCastTS;				// 캐스팅 시간
	u1	nMoveCastType;			// 무빙 캐스팅 타입
	u4	nBeginDistance;			// 시전 시작 사정거리
	u4	nConsumeHP;				// 소모 HP -> 최대사용횟수로 변경
	u4	nConsumeDS;				// 소모 DS
	u2	nTargetType;			// 타겟 타입
	u1	nCastType;				// 시전 타입
	u1	nCastTypeFactor1;		// 시전 타입 1
	u1	nCastTypeFactor2;		// 시전 타입 2
	u1	nTerms;					// 실행 타입
	u4	nTermsValue;			// 실행 값			// 1000 분률
	u1	nEffect;				// 효과 타입		// 스킬 타입
	u4	nMinEffectValue;		// 효과 최소 값
	u4	nMaxEffectValue;		// 효과 최대 값
	u4  nSkillNo;				// 디지몬리스트에 있는 스킬번호
	u4	nSkillDelay;			// 스킬 애니메이션 시간
	u4	nSkillVelocity;			// 스킬 속도
	u4	nSkillAccel;			// 스킬 가속도
	n4	nEffectFactor[6];			// 효과 팩터				/* nEffect 값에 따라 사용처 변경 */	
	
	n1	nActiveType;			// 동작 타입			// 0 자신 주변, 1 타켓, 2 좌표
	u4  NoticeTime;				// 예고 시간 ms
	n4  nLinkType[5];			// DM_Buff-BuffCode 값 or MonsterSkill		/* MAX_MONSTER_BUFF_SKILL_COUNT */
	n4	nLinkCode[5];			// DM_Buff-BuffCode 값 or MonsterSkill		/* MAX_MONSTER_BUFF_SKILL_COUNT */
	n4  nLinkRate[5];			// nLinkCode의 동작 확율 1/10000

	n1  nAttackType;			// 공격 타입 0 즉시, 1 지연 ( 발사체 도달시간 )	
	n4	nApplyTarget;			// 버프 혹은 디버프 대상
	
	//u4	nSkillCodeNo;		// 스킬코드 넘버
	stMonsterSkill_Terms*	pSkillTerms;
};
#endif

namespace eMon_SkillTermsType
{
	enum
	{
		None	= 0,				// 없음

		MonsterHP_Below = 5,		// 자신의 HP 이하
		User_Die_Count = 10,		// 유저의 사망 횟수
		Active_Buff_Count = 11,		// 해당 버프 적용 갯수
	};
}

namespace eMon_SkillEffect
{
	enum
	{
		None	= 0,		// Monster_Skill->Skill_Type
		InsHP_Per,			// HP 현재 증가 효과	/* 사용 안함 */
		DecHP_Per,			// HP 현재 감소 효과	/* 사용 안함 */
		InsHP_Num,			// HP 정수 증가 효과	/* 사용 안함 */
		DecHP_Num,			// HP 정수 감소 효과	
		InsMAXHP_Per,		// HP 최대치 증가 효과	/* 사용 안함 */
		DecMaxHP_Per,		// HP 최대치 감소 효과	/* 사용 안함 */
		InsDS_Per,			// DS 현재 증가 효과	/* 사용 안함 */
		DecDS_Per,			// DS 현재 감소 효과	/* 사용 안함 */
		InsDS_Num,			// DS 정수 증가 효과	/* 사용 안함 */
		DecDS_Num,			// DS 정수 감소 효과	/* 사용 안함 */
		InsMAXDS_Per,		// DS 최대치 증가 효과	/* 사용 안함 */
		DecMaxDS_Per,		// DS 최대치 감소 효과	/* 사용 안함 */
		Summon,				// Monster 소환
		InsAtt_Stack,		// 몬스터 성장
		MonsterGather,		// 현재 맵의 생존몬스터 호출
		Gathering,			// 모아맞기
		Disperse,			// 흩어져맞기
		Range_Persist,		// 씨앗깔기
		Berserk,			// 광폭화		
		Continu_Range_Attack, // 연속 광역 공격		
		Buff_Attack,		// 버프 발생 - 특수한 경우, 죽을때 상대방 디버프 해제
		Single_StackDeBuff_Attack, //단일 타켓을 타격시 디버프 쌓임		
		Region_Buff_Nesting,// 일정지역에 버프 쌓임
		GatheringExt,		// 모아맞기(근/원딜 발사체 나가게 수정)
		DisperseExt,		// 흩어져맞기(근/원딜 발사체 나가게 수정)
		Rush,				// 특정방향으로 돌진(실제 돌진이아니라 투사체만 돌진함)
		RandomAoE,			// 랜덤 광역 범위공격
		ChainBounce,		// 주변 유저에게 튕기는 유형

		Exterminate = 30,	// 맵 전역전멸기
		SummonPos = 31,		// 특정 위치에 소환
	};
}
#ifdef JHB_ADD_RAID_MONSTERSKILL_20140916
struct  stRange_Persist
{
	u4		nRange;			
	bool	bSelfPersist;
 	u4		nX;				//pos x 위치
 	u4		nY;				//pos y 위치
	u4		nTx;			//Tile x 위치
	u4		nTy;			//Tile y 위치
	u4		nSkillIDX;	
	n4		nSkillCount;	// 동작 횟수
	u4		nSkillTick;		// 반복 시간
	u4		nLastActTick;	// 마지막 동작 시간
	u4		nEndTick;		// 종료 시간
};
#endif

struct stOpenFactor
{
	enum 
	{
		OPENTYPE_NONE	 = 0,
		OPENTYPE_MONSTER = 1,
		OPENTYPE_QUEST	 = 2,
		OPENTYPE_SERVER	 = 3,
	};

	u1 nOpenType;		// 1=몬스터, 2=퀘스트
	u4 nOpenFactor;
	u1 nFactorNumber;
};
struct stDoorObject
{
	stDoorObject() : nMapID(0), nObjectID(0), nOpenSequence(0)
	{}

	u4 nMapID;
	u4 nObjectID;
	u2 nOpenSequence;

	std::vector<stOpenFactor> vOpenCheck;
};



namespace SKILLVALUE{	//0~9
	enum{
		SKILLVALUE_MAX = 10,

		RUSH_STARTTIME	= 0,
		RUSH_RUSHCOUNT,
		RUSH_LASTRUSHTIME,
		RUSH_STARTX,
		RUSH_STARTY,
		RUSH_ENDX,
		RUSH_ENDY,

		RANDAOE_ = 0,

		CHAIN_ = 0,

	};
}