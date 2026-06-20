
#pragma once 

namespace nsCsDigimonTable
{
	enum eEVOLUTION_TYPE{
		ET_DIGITAMA = 1,			// 디지타마
		ET_INTRAINING = 2,			// 유년기
		ET_ROOKIE = 3,				// 성장기
		ET_CHAMPION = 4,			// 성숙기
		ET_ULTIMATE = 5,			// 완전체
		ET_MEGA = 6,				// 궁극체
		ET_BUSTERMODE = 7,			// 초궁극체
		ET_JOINTPROGRESSMODE = 8,	// 합체진화체(조그레스)
		ET_CAPSULE = 9,				// 캡슐진화체
		ET_SPIRIT = 10,
#ifdef SDM_TAMER_XGUAGE_20180628
		ET_X_ROOKIE,
		ET_X_CHAMPION,
		ET_X_ULTIMATE,
		ET_X_MEGA,
		ET_X_BUSTERMODE,
		ET_X_JOGRESS,
		ET_MIX,						// 혼합체
		ET_MAX,
#endif
	};

	enum eATTRIBUTE_TYPE{
		AT_NONE = 1,
		AT_DATA = 2,
		AT_VACCINE = 3,
		AT_VIRUS = 4,
		AT_UNIDENTIFIED = 5
	};

	enum eFAMILY_TYPE{		
		FT_DR	= 6,
		FT_NSP,
		FT_ME,
		FT_WG,
		FT_UK,
		FT_DS,
		FT_DA,
		FT_JT,
		FT_NSO,
		FT_VB,
		FT_NONE,
	};

	enum eNATURE_TYPE{		
		NT_ICE = 16,
		NT_WATER = 17,
		NT_FIRE = 18,
		NT_EARTH = 19,
		NT_WIND = 20,
		NT_WOOD = 21,
		NT_LIGHT = 22,
		NT_DARKNESS = 23,
		NT_THUNDER = 24,
		NT_STEEL = 25,
		NT_NONE,
	};

	enum eGROW_TYPE
	{
		GT_SHORT_ATTACKER = 1,
		GT_NEAR_ATTACKER,
		GT_QUICK_ATTACKER,
		GT_DEFENDER,
	};

	enum ePASSIVE_TYPE
	{
		PA_NONE			= 0,		// 기능 없음		-> 사용하지 않음!!
		PA_START		= 1,
		// ---------- 기능 추가 란 ---------- //
		PA_NPC_DISCOUNT	= 1,		// NPC 할인
		
		
		
		
		// ---------- 기능 추가 란 ---------- //
		PA_MAX,						// map을 배열로 사용함
	};
}
