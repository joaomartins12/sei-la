
#pragma once 

BEGIN_NAME_TABLE

struct sDigimonData
{
	enum eEVOLUTION_TYPE{
		ET_NONE = 0,
		ET_DIGITAMA,			// 디지타마
		ET_INTRAINING,			// 유년기
		ET_ROOKIE,				// 성장기
		ET_CHAMPION,			// 성숙기
		ET_ULTIMATE,			// 완전체
		ET_MEGA,				// 궁극체
		ET_BUSTERMODE,			// 초궁극체
		ET_JOINTPROGRESSMODE,	// 합체진화체(조그레스)
		ET_CAPSULE,				// 캡슐진화체
		ET_SPIRIT,				// 하이브리드체
		ET_X_ROOKIE,			// 성장기 X
		ET_X_CHAMPION,			// 성숙기 X
		ET_X_ULTIMATE,			// 완전체 X
		ET_X_MEGA,				// 궁극체 X
		ET_X_BUSTERMODE,		// 초궁극체 X
		ET_X_JOGRESS,			// 조그레스 X
		ET_MIX,					// 혼합체(특수진화체)
		ET_MAX,
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


	struct sBaseStat
	{
		sBaseStat():s_nBaseStat_HP(0),s_nBaseStat_DS(0),s_nBaseStat_DefPower(0)
			,s_nBaseStat_Evasion(0),s_nBaseStat_MoveSpeed(0),s_nBaseStat_CriticalRate(0),s_nBaseStat_AttPower(0),s_nBaseStat_AttSpeed(0)
			,s_nBaseStat_AttRange(0),s_nBaseStat_HitRate(0)
		{};

		// 스탯 분류
		USHORT				s_nBaseStat_HP;
		USHORT				s_nBaseStat_DS;
		USHORT				s_nBaseStat_DefPower;
		USHORT				s_nBaseStat_Evasion;
		USHORT				s_nBaseStat_MoveSpeed;
		USHORT				s_nBaseStat_CriticalRate;
		USHORT				s_nBaseStat_AttPower;
		USHORT				s_nBaseStat_AttSpeed;
		USHORT				s_nBaseStat_AttRange;
		USHORT				s_nBaseStat_HitRate;
	};

	struct sSKILL
	{
		sSKILL():s_dwID(0),s_nReqPrevSkillLevel(0)
		{};

		DWORD		s_dwID;
		int			s_nReqPrevSkillLevel;
	};
	typedef std::map<int, sSKILL>	SkillCont;

	sDigimonData():s_dwDigimonID(0),s_dwModelID(0),s_dwBaseLevel(0),s_nDigimonType(0)
				  ,s_dwCharSize(0),s_fWakkLen(0.0f),s_fRunLen(0.0f),s_fARunLen(0.0f),s_nDigimonRank(0)
	{};

	sBaseStat			s_sBaseStat;
	DWORD				s_dwDigimonID;
	DWORD				s_dwModelID;
	float				s_fSelectScale;
	DWORD				s_dwBaseLevel;
	//디지몬 형태
	std::wstring		s_szForm;
	// 케릭터 타입
	BYTE				s_nDigimonType;
	// 크기
	USHORT				s_dwCharSize;

	eEVOLUTION_TYPE		s_eEvolutionType;
	eATTRIBUTE_TYPE		s_eAttributeType;
	eFAMILY_TYPE		s_eFamilyType[ 3 ];
	eNATURE_TYPE		s_eBaseNatureType;
	eNATURE_TYPE		s_eBaseNatureTypes[ 3 ] ;

	std::wstring		s_szName;	
	std::string			s_cSoundDirName;
	std::string			s_szEvoEffectDir;

	// 스킬
	SkillCont			s_Skill;

	// 애니 이동 거리
	float				s_fWakkLen;
	float				s_fRunLen;
	float				s_fARunLen;
	int					s_nDigimonRank;
};

END_NAME_TABLE