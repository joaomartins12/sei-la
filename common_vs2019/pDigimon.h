

#pragma once


#include "nlib/base.h"

#include "nScope.h"
#include "pType.h"
#include "pLength.h"
#include "pParameter.h"


namespace nNature
{
	enum eNatureAttribute
	{
		Fire,
		Thunder,
		Steel,
		Wind,
		Wood,
		Water,
		Darkness,
		Earth,
		Ice,
		Light,
		None,		// 무속성
		Unknown,	// 알수없는 속성 ==> 제일 강함
		MaxAttribute
	};
};


namespace pDigimon
{
	enum
	{
		Begin = nScope::Digimon,

		MoveTo,				//   0~ 99 기본 용병 슬롯
							// 100~199 보관소 용병 슬롯

		OpenCareSlot,
		OpenBaseSlot,

		CareSlotList,		// 용병 보관소 디지몬 목록
		CareSlotSize,		// 디지몬 보관소 크기

		ChangeName,			// 디지몬 이름 변경

		ChangeScale,		// 신비의 열매 등으로 디지몬 크기 변경

		AdvertBurstMode,	// 버스트모드 달성시 광고

		BattleTagSuccess,	// 전투시 디지몬 변경 성공
		BattleTagFailure,	// 전투시 디지몬 변경 실패

		BuffSave,			// 버프 신규 저장
		BuffUpdate,			// 기존 버프 슬롯 수치 변경

		InitSkillPoint,		// 스킬 포인트 초기화

		DigimonInchantInfo,	// 디지몬 강화 정보

		CrossWarsSlotSize,	// 크로스 인벤토리(디지몬 칩셋?) 슬롯 사이즈
		CrossWarsNotReg,	// 크로스 워즈 디지몬 미등록 상태
		CrossWarsHatch,		// 크로스 워즈 디지몬 부화 패킷
		CrossWarsEnergize,	// 크로스 워즈 디지몬 활성화
		CrossWarsEnergizeFail,	// 활성화 실패 패킷
		CrossWarsDigimonInfo,	// 크로스 워즈 디지몬 초기 정보 전송
		CrossWarsChangeOne,	// 단일 개체 파트너체인지
		CrossWarsCombine,	// 크로스워즈 합체 디지몬
		CrossWarsRecover,	// 크로스워즈 해제
		CrossWarsLoader,	// 크로스 로더 오픈 및 변경사항
		CrossWarsCrownHeal,	// 코드 크라운 게이지 회복량 변동

		IncubaterList,		// 디지몬 보관소 인큐베이터 리스트
		Facilitation,		// 인큐베이터 촉진
		IncubaterError,		// 인큐베이터 관련 예외사항 발생
		Extraction,			// 인큐베이터 추출
		IncubaterSaveExp,	// 인큐베이터에 있는 디지몬 경험치 증가
		IncubaterSlotChange,// 인큐베이터에 있는 디지몬과 디보에 있는 디지몬 교체 

		SetMasterCardLeader,	// 마스터 카드 리더 등록
		UnSetMasterCardLeader,	// 마스터 카드 리더 해제
		DigimonBookInfo,		//디지몬북 오픈시 정보 받아오는 프로토콜
		DigimonBookItem,		//디지몬 달성량에 따른 아이템 지급
		DigimonBookDeckUse,		//디지몬 북 덱 효과 사용
		DigimonBookDeckEff,		//디지몬 북 덱 효과 사용
		CapsuleEvoSlotResult,	//캡슐진화 슬롯 오픈 결과

		SpiritToDigimon,		// 스피릿아이템 -> 하이브리드디지몬
		DigimonToSpirit,		// 하이브리드디지몬 -> 스피릿아이템
		AncientSpiritEvolution,	// 카이젤그레이몬 + 매그너가루루몬 -> 스사노오몬

		DigimonDataChange,		// 디지몬 데이타 교환

		CareDigimonMove,		//보관소 내부 이동
		IncubaterEnd,			//인큐베이터 지속시간 종료

		DigimonSkillLimitOpen,	//디지몬 스킬 레벨 한계 확장 요청
		UpdateDigimonSkillCoolTime, //디지몬 스킬 쿨타임
		End	//MAX 3300
	};
};

namespace nDigimon
{
	enum eType
	{
		Agu,
		Rara,
		Gao,

		MaxType
	};

	enum eDigitalAttribute
	{
		No,	// 1 None
		Da,	// 2 Data
		Va,	// 3 Vaccine
		Vi,	// 4 Virus
		Un, // 5 Unidentified
		MaxDigitalAttribute,
	};

	// 아래는 순수 참조용
	// 디지몬 스킬 개편 이전
	//enum eBaseAttribute // attribute
	//{
	//	BeginBA = MaxExtStat-1,

	//	BaseLevel,	// 기본 레벨, 0값은 사용하지 아니함

	//	RangeType,	// 근거리 1, 원거리 2
	//	Radius,		// object 반지름
	//	EvoStep,	// 성장 단계

	//	Digital,	// va, da, vi, no, un, Digimon Base attribute
	//	Nature,		// 
	//	Family1,	//
	//	Family2,	//
	//	Family3,	//

	//	Skill0,		// linked skill == 기본 공격
	//	Skill1,		//
	//	Skill2,		//
	//	Skill3,		//
	//	Skill4,		//
	//	Skill5,		//

	//	ModelID,

	//	MaxBaseAttribute
	//}; // enum eBaseAttribute

	enum eBaseAttribute // attribute
	{
		BeginBA = MaxExtStat-1,

		BaseLevel,	// 기본 레벨, 0값은 사용하지 아니함

		RangeType,	// 근거리 1, 원거리 2
		Radius,		// object 반지름
		EvoStep,	// 성장 단계

		Digital,	// va, da, vi, no, un, Digimon Base attribute
		Nature,		// 
		Family1,	//
		Family2,	//
		Family3,	//

		Skill0,		// linked skill == 기본 공격
		Skill1,		//
		Skill2,		//		
		Skill3,		//		
		Skill4,		//		
		Skill5,		//		

		ModelID,

		Skill1_Condition,
		Skill2_Condition,
		Skill3_Condition,
		Skill4_Condition,
		Skill5_Condition,

		ClassSkill,
		PassiveSkill,

		AutoHealInterval,
		AutoHealHP,
		AutoHealDS,
		AutoHealXG,
		

		MaxBaseAttribute
	}; // enum eBaseAttribute

	enum
	{
		SkillOpen	= 0,			// 스킬 오픈 상태 
		SkillLv		= 1,			// 스킬 레벨 상태 

		ClassSkillFirst = 0,		// 클래스 스킬 시작 인덱스
		PassiveSkillFirst = 15,		// 패시브 스킬 시작 인덱스
	};

	enum
	{
		CLASS_SKILL_INDEX = 0,
		PASSIVE_SKILL_INDEX,

		MAX_NATIVE_SINGLE_SKILL_COUNT = 16,
	};

#ifdef HSS_NEW_CROSS_WARS_20130611
	enum
	{
		CROSSWARS_NONE		= 0,	// 크로스 상태 아님
		CROSSWARS_ONE		= 1,	// 단일 개체 크로스 상태
		CROSSWARS_COMBINE	= 2,	// 여러마리 합체 상태
		CROSSWARS_ERROR		= 99,	// 이상 상태임 확인 할 것

		CROSSWARS_LV		= 0,	// 레벨 스텟
		CROSSWARS_AP		= 1,	// 공격력 스텟
		CROSSWARS_DP		= 2,	// 방어율 스텟
		CROSSWARS_AV		= 3,	// 회피율 스텟
		CROSSWARS_CT		= 4,	// 크리티컬율 스텟
	};
#endif

//#ifdef KSJ_ADD_MEMORY_SKILL_20140805
	enum
	{
		CHIP_ICE		= 16,
	};
//#endif

	enum eDataChangeType
	{
		eDataChangeType_None = -1,
		eDataChangeType_Begin = 0,
		eDataChangeType_Size = eDataChangeType_Begin,
		eDataChangeType_Inchant,
		eDataChangeType_EvoSlot,
		eDataChangeType_End = eDataChangeType_EvoSlot,
		eDataChangeType_Count, 
	};
};

#ifdef KSJ_ADD_SPIRIT_EVOLUTION_20141001
struct stSpiritDigimonInfo
{
	n4 nNpcIdx;					// 해당 작업을 할수있는 NPCIDX
	u1 exchangeType;			// 결과물이 디지몬인지 아이템인지 판별. 1 = 디지몬, 2 = 아이템
	u1 requireType;				// 필요한 재료의 종류. 1 = 디지몬, 2 = 아이템
	u1 requireLevel;			// 필요한 재료가 디지몬 일때 결과물로 출력되기 위한 필요 레벨
	u4 resultIDX;				// 결과물의 IDX
	u4 requireMaterial[10];		// 필요한 재료의 IDX
	u4 reqMoney;				// 필요한 돈
	u4 additionItem[5];			// 추가로 필요한 아이템 IDX
	u4 additionItemQuantity[5];	// 추가로 필요한 아이템 개수
	u1 wayType;					// 0:사용안함, 1:requireMaterial,additionItem 의 재료를 모두 만족해야함, 2:requireMaterial,additionItem 의 재료 중 하나만 만족하면 됨. 이 조건은 아이템을 디지몬으로 바꿀때만 적용.
};
#endif













