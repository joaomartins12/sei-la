#pragma once

#include "pLimit.h"
#include "cItemData.h"

#pragma pack(push)
#pragma pack(1)


namespace nEvo
{
	enum
	{
		None			= 0, // 미사용
		Opened			= 1, // 아이템 사용해서 슬롯을 사용 가능하게 한 경우 1, 아니면 0
		Reserved1		= 2, // 아직 닫혀져 있는 상태, 에볼류터 또는 버스트모드 오픈 아이템 필요
		Reserved2		= 4, // 아직 닫혀져 있는 상태, 에볼류터 또는 버스트모드 오픈 아이템 필요
		RideMode		= 8  // 1: ride mode opened, 0: ride mode closed
	};

	enum
	{
		EvoNone,
		DigiTama,		
		InTraining,		
		Rookie,			// Rookie
		Champion,		// Champion
		Ultimate,		// Ultimate
		Mega,			// Mega
		BurstMode,		// BurstMode
		CombineMode,	// Jogress
		CapsuleEvo,		// Capsule
		SpiritEvo,		// Spirit
		RookieX = 11,	// Rookie X
		ChampionX,		// Champion X
		UltimateX,		// Ultimate X
		MegaX,			// Mega X
		BurstModeX,		// BurstMode X
		CombineModeX,	// Jogress X
		ExtraEvo = 17,

		MaxEvoStep		// Max Evolutions
	};
};



class cEvoUnit
{
public:
	cEvoUnit(void) : m_nMaxSkillLevelStep(0){
		ZeroMemory(m_nSkillLevel , sizeof(u1) * nLimit::Skill);
#ifdef SERVER_KSW_SKILL_LV_EXPEND_181205
		ZeroMemory(m_nSkillMaxLevel , sizeof(u1) * nLimit::Skill);
#endif
	};
   ~cEvoUnit(void) { };

   cEvoUnit(const cEvoUnit &kEvoUnit) { *this = kEvoUnit; };
   void operator=(const cEvoUnit &kEvoUnit)
   {
	   m_nSkillExp = kEvoUnit.m_nSkillExp;
	   m_nSkillExpLevel = kEvoUnit.m_nSkillExpLevel;
	   m_nSlotState = kEvoUnit.m_nSlotState;
	   m_nSkillPoint = kEvoUnit.m_nSkillPoint;
	   for ( n4 i = 0; i < nLimit::Skill; i++ )
	   {
		   m_nSkillLevel[i] = kEvoUnit.m_nSkillLevel[i];
#ifdef SERVER_KSW_SKILL_LV_EXPEND_181205
		   m_nSkillMaxLevel[i] = kEvoUnit.m_nSkillMaxLevel[i];
#endif
	   }
   };

public:
	//cItemData m_Card;	// open 카드
	//cItemData m_Crest;	// open 문장
	//struct
	//{
	//	u4 m_nSkillExp      : 24;	// 8,388,608 까지 가능
	//	u4 m_nSkillExpLevel :  4;	// 15랩 까지 가능
	//	u4 m_nSlotState     :  4;	// 1 1: open item(에볼류터, 버스트모드 오픈아이템) 사용		0: this slot is closed
	//	// 2 reserved
	//	// 4 reserved
	//	// 8 1: ride mode opened									0: ride mode closed
	//};

	struct
	{
		u4 m_nSkillExp      : 26;	// 33,554,431 까지 가능하지만 구조상(DB저장쪽 *100을하므로) 21억/100 까지 사용가능
		u4 m_nSkillExpLevel :  6;	// 63랩 까지 가능
		u4 m_nSlotState     :  4;	// 1 1: open item(에볼류터, 버스트모드 오픈아이템) 사용		0: this slot is closed
		u4 m_nMaxSkillLevelStep : 8;
		u4 m_nReserved		: 20;	// 20비트어치 사용안하는중
	};

//*	// 차후 수정해야할 사항
	// m_nEvoMask 는 없어져야함
	//struct
	//{
	//	u4 m_nSkillExp      : 23;	// 8,388,608 까지 가능
	//	u4 m_nSkillExpLevel :  5;	// 31랩 까지 가능
	//	u4 m_nSlotState     :  4;	// 1 1: open item(에볼류터, 버스트모드 오픈아이템) 사용		0: this slot is closed
	//								// 2 reserved
	//								// 4 reserved
	//								// 8 1: ride mode opened									0: ride mode closed
	//};
//*/

/*	// 아래는 기존 값
	struct
	{
		u4 m_nSkillExp      : 23;	// 8,388,608 까지 가능
		u4 m_nSkillExpLevel :  7;
		u4 m_nSlotState     :  2;	// 0 진화체 없음, 1 에볼류터사용 또는 원래 오픈된 상태, 2 아직 닫혀있는 상태 - 에볼류터 필요
	};
//*/


	u1 m_nSkillPoint;
	u1 m_nSkillLevel[nLimit::Skill];	// 스킬별 0 none, 1~ level
#ifdef SERVER_KSW_SKILL_LV_EXPEND_181205
	u1 m_nSkillMaxLevel[nLimit::Skill];	// 스킬별 현재 찍을수있는 MAX 레벨
#endif
};


#pragma pack(pop)
