

#pragma once


#include "nlib/base.h"
#include "pLimit.h"


class cEvoStep
{
public:
	cEvoStep(void) { }
   ~cEvoStep(void) { }

public:
	uint m_nEvoUnitIDX;	// 0 < 진화체 배열 idx < nLimit::EvoUnit

	uint m_nMethod;	// 진화 시 방법
		// 0 = 100% charge, 테이머 DS 계산에 의한 소모
		// 1 = 기존에 비례
};


class cEvoInfo
{
public:
	cEvoInfo(void) { }
   ~cEvoInfo(void) { }

public:
	uint m_nIDX; // digimon type index
	uint m_nEvoStep;	// 2.유년기, 3.성장기, 4.성숙기, 5.완전체, 6.궁극체, 7.초궁극체

public: // 진화 가능 여부
	cEvoStep m_EvoStep[nLimit::EvoStep]; // 0~4 진화체, 5는 퇴화체

public:	// 오픈 제한
	uint m_nIsActive;			// 1=사용
	uint m_nOpenReqBase;		// 1=파트너몬만 진화, 2=로얄나이츠만 진화 가능
	uint m_nOpenReqLevel;
	uint m_nOpenReqQuest;
	uint m_nOpenReqItem;		// 주로 카드

	uint m_nOpenReqCrest;		// 문장

public:	// 진화 제한
	uint m_nEvoReqItem;			// 진화시 필요 아이템
	uint m_nEvoReqCard1;		// 진화시 필요 카드
	uint m_nEvoReqCard2;		// 진화시 필요 카드
	uint m_nEvoReqCapsule;		// 진화시 필요 캡슐

public: // 오픈 아이템
	uint m_nOpenItemType;		// 에볼류터, 버스트 모드 오프너
	uint m_nOpenItemCount;		// 에볼류터, 버스트 모드 오프너 필요 개수

public:	// 진화시 소모 아이템 
	uint m_nConsumeItemType;	// 진화시 소모되는 아이템
	uint m_nConsumeItemCount;	// 진화시 소모되는 아이템 갯수

	int m_nReqFS;				// 진화시 요구되는 친밀도

#ifdef LJS_NEW_COMBINE_EVO_2012_09_07
	u4 m_nTreeType;
	u4 m_nNeedQuest;
	u4 m_nSubType;
	u4 m_nSubTypeCount;
	u4 m_nSubTypeTime;
	u1 m_nCombineIDCount;
	u4 m_nCombineID[ nLimit::MAX_COMBINE_ID ];
#endif

};


class cEvoSource
{
public:
	cEvoSource(void);
   ~cEvoSource(void);

public:
	uint GetIDX(void) { return m_nIDX; }
	void SetIDX(uint idx) { m_nIDX = idx; }


//////////////////////////////////////////////////////
// internal data
public:
	uint	m_nIDX; // digimon type idx
	short	m_nBattleEvoType;
	u2		m_nNativeSkill[nLimit::Native_Skill];	// 디지몬의 고유 스킬 총 개수(패시브 1+클래스 1)

public:
	uint m_nMaxEvoUnit; // 진화할 수 있는 최대 가지수

	cEvoInfo m_EvoInfo[nLimit::EvoUnit];	// 배열 0번째 사용 하지 아니함
};

class cBattleEvo
{
public:
	struct sELEMENT
	{
		sELEMENT(){ s_bEnable = false; }
		bool	s_bEnable;
		u2		s_nEvolStep;
		u2		s_nReqEvoEP;
		u2		s_nDecEP;

		u2		s_nAttNormalRate;
		u2		s_nAttSkillRate;
		u2		s_nDefNormalRate;
		u2		s_nDefSkillRate;
		u2		s_nUpperRate;
		u2		s_nDownRate;

		u2		s_nBonusHP;
		u2		s_nHPRate;
		u2		s_nBonusDS;
		u2		s_nDSRate;
	};
	int				m_nMaxReqEP;
	sELEMENT		m_Element[ nLimit::BattleEvo_Element ];
};





