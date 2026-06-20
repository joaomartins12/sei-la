#pragma once

#include "pTamer.h"
#include "pCountry.h"

namespace nLimit	
{
	enum
	{
		Level			=	Language::pCountry::TamerLevelMax,
		TamerLevelMax	=	Language::pCountry::TamerLevelMax,
		DigimonLevelMax	=	Language::pCountry::DigimonLevelMax,
		GuildLevel		=   10,	// 길드의 최고 랩 제한
		GuildGrade		=    5, // 길드				, 부마스터, 닷트대원, 일반대원, 신입대원

		OpenedTamerSlot	=   10,	// Opened Tamer Slot
		MaxTamerSlot	=	12,	// Max Tamer Slot

		DigimonBaseSlot	=    Language::pCountry::SizeDigimonBaseSlot,  // Digimon Slot
//		DigimonCareSlot =   20,	// 디지몬 보관소 크기

		ItemRank		=  255,	// item rank 최대값

		SkillSlot		=    5,	// digivice 내에 장착 가능한 스킬아이템 슬롯 최대 갯수
		Chipset			=   12, // digivice chipset slots
		Digivice		=    1, // digivice 내의 디지바이스 아이템 슬롯

		//Capsule		=    3,	// 클라이언트에서 사용
		Memory			=    6, // 클라이언트에서 사용, 용병 슬롯 갯수

		Equip			=   nTamer::MaxParts, // Equipaments Slots

		Inven			=   Language::pCountry::SizeInvenMax,	// 인벤토리 아이템 슬롯 최대 갯수
		PcbangInven		=	7,	// PC방 최대 인벤토리 개수
//		Warehouse		=   70,	// 창고의 아이템 슬롯 최대 갯수

		ShareStash      =   14, // 계정공유 창고 슬롯 기본 수

		CashShop		=   49,	// 캐쉬로 구매한 아이템 창고
		GiftShop		=	 7,	// 선물 또는 아이템 복구로 인해 테이머에게 제공되는 아이템 창고

		EvoStep			=    Language::pCountry::EvoStep,	// 최대 5단계 진화(0~4), 5번째는 퇴화
		EvoUnit			=   17,	// 최대 진화 개체수 20 가지를 넘지 않을듯, 0번째 배열 사용하지 아니함 // 테이블에 15개까지 존재 할수 있음

		Skill			=    5,	// maximum skill number, 디지몬,테이머가 소유할 수 있는 스킬의 최대 갯수
		Quest			= 3072,	// maximum quest number, 128bytes == 1024bits
//#ifdef QUEST_EXPAND
		MaxQuestSetCount=	2,
//#else
//		MaxQuestSetCount=	1,	// 퀘스트셋 최대 카운트(기존 1 + 확장 1) #define QUEST_EXPAND
//#endif
		DigimonQuest	=	64,

		QuickSlot		=	 8,
		CenterBar		=	 12,
		Channel			=   32,	// 최대 생성 가능한 채널 수는 16개로 제한

		Map				=  200,	// Quando o jogo terminar, o número de mapas não pode exceder 192
		Region			=   10,	// As informações de área em um mapa não podem exceder 7.

		SocketSlot		=    3, // 1개 아이템의 최대 소켓 갯수

		SkillLevel		=   Language::pCountry::MAX_DIGIMONSKILLLevel, // 최대 스킬 레벨
		TamerBuff		=	10,	// 테이머 버프 최대 개수

		TradeItem		=    5,	// 최대 거래 가능 아이템 갯수
		BuyItem			=	12,	// 게임내 상점에서 한번에 구매할 수 있는 아이템의 최대 갯수

		CardDropItem	=    5,	// 카드로 부터 드랍되는 아이템의 종류
		DropItem		=   Language::pCountry::DropItem,	// 몬스터 드랍 일반 아이템 종류
		DropQuestItem	=   12,	// 몬스터 드랍 퀘스트 아이템 종류
		DropEventItem	=	12,	// 몬스터 드랍 이벤트 아이템 종류
		DropCardItem	=	4,	// 몬스터 드랍 카드 아이템 종류

		SoldierType		=   18,

		PartyMember		=    6,	// 한 파티의 최대 인원수

		CoolDown		=   32,	// 쿨다운타임의 종류가 32개를 넘을 수 없다.

		MaxAttrInchantLevel = Language::pCountry::MaxAttrInchantLevel,	//d++ 속성 당 최대 강화단계
		MaxInchantLevel =	Language::pCountry::MaxInchantLevel,			//d++ 최대 디지몬 강화 단계 12 -> 24로 수정
		MaxInchantType	=	nExtendState::MaxExtendStat, // 최대 강화 종류
		MaxShopType		=	 9,
		MaxLieKill		=	10000,	// 가상으로 최대 죽일수 있는 몬스터 수
		HackShildLimit	=	15,		
		MaxBuff			=	20,		// 게임내 스킬로 생성되는 버프 최대 개수	// MONSTER_SKILL_RENEWAL_20150715_DEBUFF 10 -> 20 확장
		SystemBuff		=	6,


		BattleEvo_TypeCnt	=	5,	// 배틀 에볼루션 타입 갯수
		BattleEvo_Element	=	6,	// 배틀 에볼루션 요소 갯수

		ClassSkill		=	32,
		Native_Skill	=	2,		// 네이티브 스킬은 스킬 오픈 상태와 스킬 레벨 상태 2가지로 나뉜다.
		MAX_SKILL_APPLY =	3,
		MAX_CHANNELING	=	Language::pCountry::MAX_CHANNELING,		// 채널링 개수 정보
		MAX_EVENT_EXP_TYPE	=	2,	// 이벤트 EXP 종류 수
		MAX_FRIENDLE_TYPE1 = 1,		// 내가 추천할수 있는 친구는 1명
		MAX_FRIENDLE_TYPE2 = 3,		// 나를 추천할수 있는 친구는 3명
		MAX_FRIENDLE = 4,
		MAX_FRIENDLY_MARK = 10,	// 친구 초대 최대 목표 수

		MAX_COMBINE_ID = 3,

		MAX_LevelUp_Event = 10,

		MAX_NATUALEXP = 10000,
		MAX_DIGITALEXP = MAX_NATUALEXP,

		Max_BarterCount = 4,

		MAX_ItemSkill	= 2,
		DigimonCrossWarsSlot	= 5,	// 서버 사용중, 크로스워즈 디지몬 최대치 크로스 로더 디지몬 칸수와 다름
		MAX_CROSS_STAT	= 5,	// 크로스로 분배되는 스텟 갯수
		MAX_CROSS_RANK_COUNT = 11,	// 크로스 랭크될수 있는 갯수
		MAX_EXAM	=	30,		// 총 수능 출제 문제 개수
		MAX_ACCESSORY_OPTION = 8,		//악세사리(반지)옵션 최대갯수
		MAX_ACCESSORY_EXCELOPTION = 16,	//악세사리 엑셀시트에 있는최대갯수
		MAX_PURCHASE		= 6,
		MAX_ItemSkillDigimon = Language::pCountry::MAX_ItemSkillDigimon,		// 사용가능한 메모리스킬 개수
		MAX_Battle_Ticket_Point = 99999999,
		FirstInven			=   Language::pCountry::SizeFirstInven,	// 초기 인벤토리 사이즈
		FirstWarehouse		=   Language::pCountry::SizeFirstWarehouse,	// 초기 창고 사이즈
		FirstShareStash		=   Language::pCountry::SizeFirstShareStash,	// 초기 공유창고 사이즈
	};
};





