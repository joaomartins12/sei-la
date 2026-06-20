#pragma once

enum eBaseStats // 계속 해서 수정 되며, 저장되어야만 하는 값들
{
	MHP,		// 디지몬, 몬스터 max hp
	MSP = MHP,	// 테이머 max stamina point
	MDS,		// max ds

	DP,			// 방어
	AP,			// 공격

	HP,			// hp(디지몬/몬스터)
	SP = HP,	// StaminaPoint 테이머
	DS,

	FS,			// FriendShip, 친밀도, 테이머에게만 필요
	FP,			// Fatigue Point 피로도, 테이머에게만 필요

	MaxBaseStat,
};


enum eExtStats // 파생되는 스텟
{ 
	EV = MaxBaseStat,	// evasion rate
	CA, // critical attack power rate

	MS, // 이동 속도
	AS, // 공격 속도

	AR, // attack range
	HT, // hit rate
		// EXP 14
		// LEVEL 15

	FL = 16,	// 크리티컬방어,190429 추가 

	BLOCK = 17,			// 블럭율

	MaxExtStat = 18,

	AllParameter = 20
}; // eAttribute


enum eMetaStats
{
	EXP = 14,
	LEVEL,

	PowerApplyRate = EXP+1,	// 스킬 적용(DM_ItemList-Skill_Code) 시 아이템 능력치 적용 비율 상승 기능 처리를 위함

	MaxMetaStat,
};

// bkpark 카드 스킬 개발에 따른 기존 값 변경 ( 기존 데이터 )
//enum ePseudoStats
//{
//	EventScale = MaxMetaStat,
//	NA,	// 자연 속성의 모든 수치
//	PS, // 테이머 대표 스킬의 결과치 조정
//	DC,	// 디지몬 DS 소모량 - 테이머 보다 낮은 레벨의 파트너몬의 DS소모량 조정(진화, 진화유지, 스킬사용)
//	DE, // 디지몬 습득 경험치량 - 테이머 보다 낮은 레벨의 파트너몬의 경험치 습득량
//	CR, // 크리티컬 발생 확률
//	RP, // 사망 패널티
//	RR, // 선공몹 인식 거리
//	CT, // 쿨타임 - 테이머가 사용중인 디지몬의 스킬 쿨타임
//	DR, // DS 자연 치유력 - 시간당 회복력
//	HR, // HP 자연 치유력 - 시간당 회복력
//
//	MaxPresudoStat,
//};

// bkpark 카드 스킬 개발에 따른 기존 값 변경 ( 변경될 데이터 )
enum ePseudoStats
{
	BL = 17,			// 블럭율
	DA,					// 일반 공격 대미지
	ER,					// 일반 공격 회피 확률
	AllParam,			// 전체 파라미터 상승 ( 이 값은 AllParameter 로 사용되기 때문에 이곳에서 사용하지 않음 ) 
	SER,				// 스킬 회피확률
	SDR,				// 스킬 방어확률
	SRR,				// 스킬 적중확률
	SCD,				// 스킬 대미지 상승량
	SCR,				// 스킬 크리티컬 발생 확률			
	HRR,				// HP 회복량
	DRR,				// DS 회복량
	MDA,				// 일반 대미지 감소량
	HR,					// 일반 공격 적중 확률
	DSN,				// DS 자연 회복량 증가
	HPN,				// HP 자연 회복량 증가
	STA,				// 상태 이상 값

	UnBeatAble,			// HSS 무적
	AttributeBoost,		// 속성 부스트
	CC,					// 코드크라운 게이지
	CR,					// 크로스 로더

	DOT,				// 지속 데미지
	DOT2,				// 지연 데미지

	STUN,				//스턴

	DR,					// 데미지 반사 40번
	DamageShield,		// 데미지 흡수 41번

	HPPerDamage,		// 남은 HP량에 따라 데미지 증가, 42번
	AttriPerDamage,		// 속성에 따라 데미지 증가
	HPPerDefence,		// 남은 HP량에 따라 데미지 감소
	AttriPerDefence,	// 속성에 따라 데미지 감소
	PROVOKE,			// 도발
	HPInsurance,		// 피 일정미만시 피 회복

	// RENEWAL_TAMER_SKILL_20150923
	CAT,				// 크리티컬 공격력
	RDD,				// 받는 데미지 감소

	GShout = 51,		// 길드 광고 확성기
	GMemberUp,			// 길드 인원 확장 
	GSummon,			// 전체 길드원 소환
	GNameColor,			// 길드원 이름색 변경

	GNatureUp = 61,		// 속성 경험치 추가
	GScanReduce,		// 스캔 비용 감소
	GScanBitUp,			// 회수시 돈 획득 증가
	GHatchUp,			// 알 부화 확률 증가
	GHuntingBitUp,		// 사냥시 돈 획득 증가
	GMoveUp,			// 이동 속도 증가
	GInchaPayReduce,	// 강화 비용 감소
	GInchaRateUp,		// 강화 확률 증가

	Weakness = 71,		// 받는 피해량
	AttributeDamage,	// 주는 피해량	
	AttributeDR,		// 속성 반사 = 속성 피해량 반사
	AttributeDS,		// 속성 흡수 = 속성 피해량 흡수
	Silence,			// 침묵 = 스킬 사용 불가

	AllSkillDamageDecrease,		 //모든 스킬데미지 감소
	SpecificSkillDamageDecrease, //특정 스킬데미지 감소
	SpecificSkillDamageIncrease, //특정 스킬데미지 증가

	NatureShield,		//특정 속성 데미지 감소 79


	XGuage,				// X게이지 , 80
	XCrystal,			// 크리스탈 , 81

	RegionBuffDummy,			//지역버프 활성,비활성  82

	EvoTypeDamageIncrease,		//진화단계 데미지 증가 83
	NatureTypeDamageIncrease,	//자연속성 데미지 증가 84

	FixMoveSpeed,				//이동속도 강제보정(둘중에 더 높은값으로 비교보정) 85
	Skill_FL,					//탄력도 86

	MaxPresudoStat,
};

enum eServerStat
{
	ExpUpRate = MaxPresudoStat,
	ExpUpPlus,
	FSUpPlus,
	MaxServerStat,
	MaxServerStat_Cnt = MaxServerStat - MaxPresudoStat, 
};

enum nExtendState
{
	ET_AT = 0, // (AT) 공격 
	ET_BL = 1, // 블럭 
	ET_CR = 2, // 크리티컬 확률 
	ET_AS = 3, // 공격 스피드 
	ET_EV = 4, // 회피 
	ET_HT = 5, // 공격 성공률 
	ET_HP = 6, // 체력
	MaxExtendStat = 7,
	OpenETStat = 5, // AT, BL, CT, EV, HP
};



__forceinline
uint GetPowerPerStatPoint(uint nStat)
{
	switch(nStat)
	{
	case MHP : return 10;
	case MDS : return 10;
	case AP  : return  1;
	case DP  : return  1;
	}

	xstop1(false, "(stat:%d) check stat", nStat);
	return 0;
}


namespace nRange
{
	enum eType
	{
		Close,		// 근거리
		Long,		// 원거리
		Agility,	// 민첩형
		Guarder,	// 방어형

		MaxRangeType
	};
};

enum nTable_Parameter
{
	APPLY_HP = 1,				//HP
	APPLY_DS,					//DS
	APPLY_MAXHP,				// MAX HP 확장
	APPLY_MAXDS,				// MAX DS 확장
	APPLY_AP,					// 공격력 증가
	APPLY_CA,					// 크리티컬 대미지 증가
	APPLY_DP,					// 방어력 증가
	APPLY_EV,					// 회피력 증가
	APPLY_MS,					// 이동속도 증가
	APPLY_AS,					// 공격속도 증가
	APPLY_AR,					// 공격사거리 증가
	APPLY_HT,					// 적중도 증가
	APPLY_FP,					// 피로도 수치 증가
	APPLY_FS,					// 친밀도 증가
	APPLY_EXP,					// 경험치
	APPLY_POWERAPPLYRATE,		// 소켓 적용 능력치 향상

	APPLY_BL = 17,	// 블럭율
	APPLY_DA,		// 일반 공격 대미지
	APPLY_ER,		// 일반 공격 회피 확률
	APPLY_AllParam,	// 전체 파라미터 상승 ( 이 값은 AllParameter 로 사용되기 때문에 이곳에서 사용하지 않음 ) 
	APPLY_SER,		// 스킬 회피확률
	APPLY_SDR,		// 스킬 방어확률
	APPLY_SRR,		// 스킬 적중확률
	APPLY_SCD,		// 스킬 대미지 상승량
	APPLY_SCR,		// 스킬 크리티컬 발생 확률			
	APPLY_HRR,		// HP 회복량
	APPLY_DRR,		// DS 회복량
	APPLY_MDA,		// 일반 대미지 감소량
	APPLY_HR,		// 일반 공격 적중 확률
	APPLY_DSN,		// DS 자연 회복량 증가
	APPLY_HPN,		// HP 자연 회복량 증가
	APPLY_STA,		// 상태 이상
	APPLY_UB,		//무적 
	APPLY_ATTRIBUTTE,	// 속성 부스트
	APPLY_CC,			// 코드 크라운
	APPLY_CR,			// 크로스 로더
	APPLY_DOT,			// 지속 데미지
	APPLY_DOT2,			// 지연 데미지
	APPLY_STUN,			// 제어 불가
	APPLY_DR,			// 데미지 반사
	APPLY_AB,			// 데미지 흡수 // 41

	//#ifdef KSJ_ADD_MEMORY_SKILL_20140805
	APPLY_HPDMG,		// 남은 HP량에 따라 데미지 증가, 42번
	APPLY_ATDMG,		// 속성에 따라 데미지 증가
	APPLY_HPDEF,		// 남은 HP량에 따라 데미지 감소
	APPLY_ATDEF,		// 속성에 따라 데미지 감소
	APPLY_PROVOKE,		// 도발
	APPLY_INSURANCE,	// 피 일정미만시 피 회복
	
	// RENEWAL_TAMER_SKILL_20150923
	APPLY_CAT,			// 크리티컬 공격력
	APPLY_RDD,			// 받는 데미지 감소

	GUILD_SHOUT = 51,	// 길드 광고 확성기
	GUILD_MEMBERUP,		// 길드 인원 확장 
	GUILD_SUMMON,		// 전체 길드원 소환
	GUILD_NAMECOLOR,	// 길드원 이름색 변경

	GUILD_NATUREUP = 61,// 속성 경험치 추가
	GUILD_SCANREDUCE,	// 스캔 비용 감소
	GUILD_SCANBITUP,	// 회수시 돈 획득 증가
	GUILD_HATCHUP,		// 알 부화 확률 증가
	GUILD_CROPBITUP,	// 사냥시 돈 획득 증가
	GUILD_MOVEUP,		// 이동 속도 증가
	GUILD_INCHAPAYREDUCE,// 강화 비용 감소
	GUILD_INCHARATEUP,	// 강화 확률 증가

	APPLY_WEAKNESS = 71,	// 쇠약 = 받는 피해량
	APPLY_ATTRIBUTEDAMAGE,	// 속성 피해량	
	APPLY_ATTRIBUTEDR,		// 속성 반사 = 속성 피해량 반사
	APPLY_ATTRIBUTEDS,		// 속성 흡수 = 속성 피해량 흡수
	APPLY_SILENCE,			// 침묵 = 스킬 사용 불가

	APPLY_AllSkillDamageDecrease,		//모든 스킬데미지 감소
	APPLY_SpecificSkillDamageDecrease,	//특정 스킬데미지 감소
	APPLY_SpecificSkillDamageIncrease,	//특정 스킬데미지 증가

	APPLY_NatureShield,				//특정 속성 데미지 감소

	APPLY_XGuage,				// X게이지 , 80
	APPLY_XCrystal,			// 크리스탈 , 81
	APPLY_RegionBuffDummy,		// 지역버프 활,비활성
	APPLY_EvoTypeDamageIncrease,
	APPLY_NatureTypeDamageIncrease,
	APPLY_FixMoveSpeed,

	MAX_APPLY_TYPE,
};

namespace NewAttribute
{
	enum AttributeType
	{
		Digital = 0,
		Natual,
	};

	enum DigitalType
	{
		Data = 0,
		Vaccine,
		Virus,
		MaxDigitalType,
	};

	enum NatualType
	{
		Ice		= 0,
		Water,
		Fire,
		Earth,
		Wind,
		Wood,
		Light,
		Darkness,
		Thunder,
		Steel,
		MaxNatualType,
	};

	enum NatualMax
	{
		NatualExpCount = 3,
	};
}