
#pragma once 

BEGIN_NAME_TABLE

struct sItemBase
{
	virtual ~sItemBase() {};

	std::wstring	s_wsName;
	int				s_nType;	// 반지 귀걸이 칩셋 등...

	DWORD			s_dwItemCode;
	DWORD			s_dwIcon;
	int				s_nClass;	// 일반, 매직, 레어 등...	
	std::wstring	s_nTypeName;
	short			s_shUseType; // 모두 사용, 비전투시만 사용 등...
	int				s_nCoolTimeGroup;
	DWORD			s_dwOverlapCount;	// 최대 중첩 갯수
	short			s_sDropType;
	short			s_sBelongType;// 귀속 타입
	short			s_sScanType;	// 스캔 타입
};

// 포션류 아이템
struct sPotionItem : public sItemBase
{

};

// 칩셋류 아이템
struct sChipsetItem: public sItemBase
{

};

// 장비류 아이템
struct sEquipItem : public sItemBase
{

};

// 스킬 습득류
struct sSkillItem : public sItemBase
{

};

// 강화류 아이템
struct sEnchantItem : public sItemBase
{

};

// 버프류 아이템
struct sBuffItem : public sItemBase
{
};

// 소환류 아이템
struct sSummonItem : public sItemBase
{

};

// 퀘스트용 아이템
struct sQuestItem : public sItemBase
{

};

// 재료 아이템
struct sMaterialItem : public sItemBase
{

};

// 이벤트 아이템
struct sEventItem : public sItemBase
{

};

// 크기 변경 아이템 
struct sScaleItem : public sItemBase
{

};

// 속성 아이템
struct sAttributeItem : public sItemBase
{

};

// 진화 재료 아이템
struct sEvolutionItem : public sItemBase
{

};

// 이름 변경 아이템
struct sChangeNameItem : public sItemBase
{

};

// 충전용 아이템
struct sChargeItem : public sItemBase
{

};

struct sItemData
{
	enum eEnableType
	{
		eEnableUse,		// 사용할 수 있다
		eNeedMinLv,		// 최소레벨 부족
		eOverMaxLv		// 최대레벨 이상
	};

	sItemData():s_dwItemID(0),s_nIcon(0),s_nClass(0),s_btCodeTag(0),s_nType_L(0),s_nType_S(0)
		,s_nTypeValue(0),s_nSection(0),s_nSellType(0),s_nUseMode(0),s_nUseTimeGroup(0)
		,s_nOverlap(0),s_nTamerReqMinLevel(0),s_nTamerReqMaxLevel(0),s_nDigimonReqMinLevel(0),s_nDigimonReqMaxLevel(0),
	s_nPossess(0),s_nEquipSeries(0),s_nUseCharacter(0),s_bDummy(false),s_nDrop(0),s_nEventItemType(0),s_dwEventItemPrice(0),
	s_dwDigiCorePrice(0),s_dwScanPrice(0),s_dwSale(0),s_bModel_Loop(false),	s_bModel_Shader(false),	s_dwSkill(0),s_btApplyRateMax(0),
	s_btApplyRateMin(0),s_btApplyElement(0),s_nSocketCount(0),s_dwSoundID(0),s_nBelonging(0),s_nQuest1(0),s_nQuest2(0),	s_nQuest3(0),
	s_nDigiviceSkillSlot(0),s_nDigiviceChipsetSlot(0),s_nQuestRequire(0),s_btUseTimeType(0),s_nUseTime_Min(0),				// 사용시간 (분)
	s_nUseBattle(0),				//	배틀시 사용가능한지
	s_nDoNotUseType(0),			// 아이템 사용 불가능한 타입 확인용
	s_bUseTimeType(0)	//2017-03-23-nova 시간제최초사용
	{}

	DWORD			s_dwItemID;
	int				s_nIcon;
	USHORT			s_nClass;
	BYTE			s_btCodeTag;
	USHORT			s_nType_L;
	USHORT			s_nType_S;
	int				s_nTypeValue;
	int				s_nSection;		// 아이템 타입 소분류를 그대로 저장
	USHORT			s_nSellType;
	BYTE			s_nUseMode;
	USHORT			s_nUseTimeGroup;
	USHORT			s_nOverlap;
	USHORT			s_nTamerReqMinLevel;
	USHORT			s_nTamerReqMaxLevel;
	USHORT			s_nDigimonReqMinLevel;
	USHORT			s_nDigimonReqMaxLevel;
	USHORT			s_nPossess;
	USHORT			s_nEquipSeries;
	USHORT			s_nUseCharacter;
	bool			s_bDummy;
	USHORT			s_nDrop;
	DWORD			s_nEventItemType;
	USHORT			s_dwEventItemPrice;
	USHORT			s_dwDigiCorePrice;	
	DWORD			s_dwScanPrice;		
	DWORD			s_dwSale;
	bool			s_bModel_Loop;
	bool			s_bModel_Shader;
	DWORD			s_dwSkill;
	BYTE			s_btApplyRateMax;
	BYTE			s_btApplyRateMin;
	BYTE			s_btApplyElement;
	USHORT			s_nSocketCount;
	USHORT			s_dwSoundID;
	BYTE			s_nBelonging;
	int				s_nQuest1;
	int				s_nQuest2;
	int				s_nQuest3;
	BYTE			s_nDigiviceSkillSlot;
	BYTE			s_nDigiviceChipsetSlot;
	DWORD			s_nQuestRequire;
	BYTE			s_btUseTimeType;
	DWORD			s_nUseTime_Min;				// 사용시간 (분)
	BYTE			s_nUseBattle;				//	배틀시 사용가능한지
	USHORT			s_nDoNotUseType;			// 아이템 사용 불가능한 타입 확인용
	bool			s_bUseTimeType;	//2017-03-23-nova 시간제최초사용

	std::wstring	s_szName;
	std::wstring	s_szComment;
	std::wstring	s_szTypeComment;

	std::string		s_cModel_Nif;
	std::string		s_cModel_Effect;
	std::string		s_cNif;
};


END_NAME_TABLE