#pragma once


#include "nScope.h"


namespace pItem
{
	enum
	{
		Begin = nScope::Item,

		Use,					// 아이템 사용
		UseFailure,				// 사용 실패
		MoneyChange,			// 아이템 구매,판매 대금, 구매시-, 판매시+

		Move,					// inven-inven, inven-digivice, inven-equip 사이의 이동, 요청에 대한 성공 응답으로도 사용됨
		MoveFailure,			// 이동 실패
		Merge,					// 두개의 아이템을 하나로 합함
		Split,					// 아이템을 인벤상태에서 둘로 나눔
		Exchange,				// 두개의 슬롯 아이템을 교체

		Drop,					// inven->field
		Crop,					// field->inven
		CropMoney,				// 잡으면 바로 먹어지는 아이템을 먹은 경우
		Destroy,				// inven->waste basket
		CropFailure,			// 아이템 습득 실패

		Trade,					// user->user

		Buy,					// shop->inven
		Sell,					// inven->shop

		BuyFailure,				// maybe insufficient money
		SellFailure,			//

		Add,					// 아이템 습득
		Update,					// 아이템 확인으로 인한 능력치 추가
		Delete,					// 아이템 사용
//		Item
//		ItemUpdate,

		DigitamaToItem,			// 디지타마 스캔
		DigitamaToMoney,		// 디지타마 회수

		InvenSlotSize,			// 인벤 슬롯 크기가 변경되는 경우
		WarehouseSlotSize,		// 창고 슬롯 크기 변경

		SocketIn,				// 소켓에 아이템을 박다.
		SocketOut,				// 소켓에서 아이템을 뽑아내다.
		SocketClear,			// 해당 소켓의 아이템 제거

		Analysis,				// 소켓 아이템 분석, 분석되지 않은 소켓 아이템의 능력치 비율 값은 0

		CashShop,				// 구매한 캐쉬 아이템 목록 요청 및 응답
		CropCashItem,			// 구매한 아이템을 인벤토리로 이동한 경우 사용

		ScanCount,				// 아이템 스캔횟수

		Timeout,				// 아이템의 유효 기간이 지남

		ChangeLimitedAttribute,	// 아이템의 Limited(소유 제한) 변경

		GiftShop,				// 선물 상자(복구 및 이벤트 제공) 목록 요청 및 응답
		CropGiftItem,			// 선물 상자로 부터 아이템을 가져오는 경우

		CapsuleToItem,			// 선물 상자를 아이템으로 교환, 루틴은 DigitamaToItem과 동일

		CoolDown,				// cool down 관련 정보 송수신

		PartyCrop,				// 파티원이 먹은 경우

		HatchOptionAdd,         // 부화 보정 아이템 등록
		PCbangInven,			// PC 방 인벤토리 정보 요청
		CropPCBangItem,			// PC 방 아이템을 가져 오는 경우
		PortableItemFail,		// 포터블 아이템 사용 실패
		CropGiftItemFail,		// 선물상자 아이템 획득 실패

		ShareStashSlotSize ,    // 계정 창고 크기 변경

		HatchBackupDiskAdd,	    // 부화 백업 디스크 등록
		HatchBackupDiskCancel,	// 부화 백업 디스크 등록 취소

		ScanItemUse,			// 스캔 아이템 사용시
		LimitItemUse,			// 아이템 사용 제한( 황금간때문에 만듬 )
		BackupDiskIn,           // 백업디스크 등록
		BackupDiskOut,          // 백업디스크 취소
		StoreSummonUse,			// 상점 소환 아이템 사용시

		RaidItemReward,			// 레이드 아이템 보상
		InsertGiftItem,			// GIFT 창고에 넣는다.

		DirectHatch,			// 알 클릭시 부화

		GotchaOpen,				// 뽑기머신 오픈
		GotchaExecute,				//뽑기머신 실행
		GotchaItem,				//일반아이템일 경우 
		GotchaError,				//가챠머신 에러
		MsgAllGotchaMachine,		//레어획득 전체메세지
		MoveClear,				// 아이템을 원본 탭에 있던 모든 아이템을 해당 하는 위치로 이동시키기(서버에서 클라로 보내줌)
		FireWork,				//불꽃놀이 아이템 추가 
		CrossWarsUse,			// 크로스 아이템 사용
		CrossWarsUseFailure,	// 크로스 아이템 사용 실패
		CrossInvenSlotSize,		// 크로스 인벤토리 증가 패킷
		MysteryMachine,			//수수께끼머신 추가
		ExpireGiftItem,			// 기프트 창고 아이템 기간 만료
		AccessoryCheck,			//악세사리 아이템 확인
		AccStoneUse,			//주문서 사용하여 능력치 갱신
		DigiSlotInc,			//기본디지몬슬롯 확장 아이템 사용 

		// 씰관련
		CardUnSeal,				// 카드 마스터 봉인 해제
		CardReSeal,				// 카드 마스터 밀봉
		CardDrop,				// 카드 아이템 드랍
		SealSlotSize,			// 실 마스터 슬롯 확장
		CardAdd,				// 카드 추가
		CardUpdate,				// 카드 수량 업데이트
		CardDelete,				// 카드 제거

		ItemRepurchase,			//아이템 재구매 기능
		ItemRepurchaseOpen,		//아이템 재구매창 오픈
		ItemRepurchaseList,		//아이템 판매시 리스트만듬

		ShouterOffer,			// 확성기 차단

		Make,					// 아이템 제작

		CashItemTradeTimeOut, // 캐시 아이템 거래 제한 해제

		UseActive,				// 아이템 활성화

		RelocateTamer,			// 테이머 서버 이전 아이템
		ItemSort,
		ItemScan,
		End
	};

	enum // CropFailure
	{
		CropF_Late  = 1,			// 남이 먼저 주워감
		CropF_Right = 2,			// 획득 권리 없음
		CropF_Far   = 3,			// 거리가 너무 멀다
		CropF_Full	= 4,			// 인벤토리 가득참
	};
};



namespace nItem
{
/////////////////////////////////////////////////////
// item source info
	enum eTab
	{
		TabInven = 0,	// nLimit::Inven
		TabEquip,		// nLimit::Equip
		TabWarehouse,	// nLimit::Warehouse

		// 아래는 digivice
		TabSkill,		// nLimit::Skill, 디지바이스에 장착 가능한 스킬 아이템
		TabChipset,		// nLimit::Chipset, 디지바이스에 장착 가능한 칩셋 아이템
		TabDigivice,	// nLimit::Digivice, 디지바이스를 장착할 수 있는 슬롯 ==> 공간은 하나임

		TabCashShop,	// nLimit::CashShop, 구매한 cash item 저장 창고
		TabGiftShop,	// nLimit::GiftShop, 선물 또는 복구 등에 의해 저장되는 아이템
		TabPCBang,		// PC방 전용 인벤토리

		TabShareStash,	// nLimit::ShareStash
		// 아래는 inven common item tab
		TabCrossWars,	// nLimit::DigimonCrossWars, 크로스 워즈 카드? 아이템 보관함
		
		MaxSlotType,

		MaxSlotSize	 = 1000, // 한개 탭내의 최대 슬롯 갯수
	};
	//	nTab  == nPos / MaxSlotSize;
	//	nSlot == nPos % MaxSlotSize;

	enum ItemClass
	{
		eNothingItem		  = 1,	// 쓸모없는 아이템 / 회색
		eNormalItem        = 2,	// 일반 아이템 / 흰색
		eMagicItem         = 3,	// 매직 아이템 / 초록색
		eRareItem          = 4,	// 레어 아이템 / 파랑색
		eEpicItem		  = 5,	// 에픽 아이템 / 보라색
		eUniqueItem		  = 6,	// 유니크 아이템 / 주황색
		eLegendItem		  = 7,	// 레전드 아이템 / 황금색
		eEventItem		  = 9,	// 이벤트 아이템
		eCashItem		  = 10,	// 캐시 아이템
		eAdministratorItem = 11, // 관리자 아이템
	};


	namespace nMaker
	{
		enum
		{
			Program			=  0, // 프로그램 테스트
			FirstSupply		=  1, // 캐릭 생성시 제공
			Design			=  2, // 기획 테스트
			Monster			=  3, // 몬스터 제공
			Shop			=  4, // 게임 상점 아이템
			Cash			=  5, // 유료아이템
			Mix				=  6, // 아이템 조합
			Scan			=  7, // 디지타마 스캔
			HatchCancel		=  8, // 부화 취소
			Crop			=  9,
			Split			= 10,
			Quest			= 11, // 퀘스트에 의한 획득
			SocketOut		= 12, // 아이템 소켓 슬롯에서 소켓아이템을 뽑아내는 경우
			Card			= 13,
			Gift			= 14, // 선물 또는 시스템 복구
			TamerShop		= 15,
			PCBangSupply	= 16,
			BDCancel        = 17,
			GotchaMachine	= 18,
			Recompense		= 19, // NPC 보상
			Repurchase		= 20, // 재구매 아이템
			DigimonBook		= 21, // 디지몬도감 아이템 지급
			Spirit			= 22, // 디지몬->스피릿 으로 인해 만들어짐
			ShopMake		= 23, // 게임 상점 아이템 제작
			Parcel			= 24, // 점핑이벤트 아이템 지급

			Master			= 30, // 30보다 크면 게임 마스터 제공
			
		};
	};

/////////////////////////////////////////////////////
// item source info
	enum eType	// item type
	{
		Head					= 21,
		Coat					= 22,
		Glove					= 23,
		Pants					= 24,
		Shoes					= 25,
		Costume					= 26,
		Glass					= 27,		
		Necklace				= 28,
		Ring					= 29,
		Earring					= 30,

		EquipAura				= 31,

#ifdef SERVER_KSW_XEVOLUTION_1_180615
		XAI						= 32,
#endif
#ifdef SERVER_KSW_ADD_BRACELET_181106
		Bracelet				= 33,
#endif

		

		

		Memory					= 50,	// 클라이언트에서 이미 작업됨
		Capsule					= 51,	// 클라이언트에서 이미 작업됨
		Chipset					= 52,	//
		Digivice				= 53,	// 디지바이스를 아이템화
		Skill					= 54,	// 스킬용 아이템

		TacticsEnchant			= 55,	// 디지몬 강화용 아이템
		TacticsEnchantSub		= 56,	// 디지몬 강화 보조 아이템
		TacticsEnchantReset		= 57,
		DigimonDataChange		= 58,	// 디지몬 데이터 교환

		// 아래는 중첩 가능할 수 있음
		LevelRequire			= 60,	// 레벨 제한 아이템			
		Potion					= 61,	// 물약, 음식류 포함
		NoQuick_Consume			= 62,	// 소모성 아이템 (퀵바 장착불가)
		Buff					= 63,	// 버프 적용 아이템
		SKILL_Card				= 64,	// 카드
		Card					= 65,	// 카드		
		Observer				= 66,	// 옵저버 대결아이템

		DigimonSkillChipATK		= 67,	// 공격형 특성 메모리
		DigimonSkillChipDEF		= 68,	// 방어형 특성 메모리
		DigimonSkillChipAST		= 69,	// 보조형 특성 메모리
		DigimonSkillMemoryChip  = 70,	// 특성메모리 사용시 소모 칩

		Consume					= 71,	// 일반 소모품 inven 즉시 사용
		Portal					= 72,	// 위치 이동 아이템
		Resurrection			= 73,	// 제자리 부활 아이템
		Guild					= 75,   // 길드생성 아이템
		PersonStore				= 76,	// 개인 상점
		Portable_Item			= 77,	// 휴대용 Npc이용권
		NoticeItem				= 78,	// 이타입의 아이템을 사용하면 전체공지

		Quest			    	= 80,	// 원료 아이템(일반재료, 퀘스트, 문장, 출입증)
		Matrial2				= 81,	// 디지타마 부화를 위한 디지몬 데이타
		EventItem				= 82,	// 이벤트 아이템(문화 상품권, MP3 등등)
		EventItem2				= 83,	// 이벤트 아이템 - 전광판에 출력하지 아니함
		Revision				= 84,   // 부화 보조 아이템

		UseQuest				= 85,	// 사용해야 하는퀘스트 아이템
		Matrial					= 86,	// 잡템
		DropMoney				= 87,	// 잡으면 바로 먹어지는 아이템^
		Money					= 88,	// 돈 ( 메가 )
		Rescale					= 89,	// 신비의 열매, 스케일 재 조정
// NEW_BATTLE_SYSTEM
		DogimonBattle			= 90,

		Digitama1				= 91,	// 전리품 판매용
		Digitama2				= 92,	// 전리품 아이템 획득용
		Digitama3				= 93,	// 전리품 디지몬 부화용

		Digitama4				= 95,	// 죽은 디지몬 소생용
		Digitama5				= 96,	// 죽은 디지몬 소환 & 환생용
		Digitama6				= 97,	// 죽은 디지몬 환생용

		Capsule_Coin			= 100,	// 캡슐 아이템
//#ifdef JHB_ADD_ACCESSORYSYSTEM_20131028
		AccStone				= 110,	//악세사리 옵션변경 스톤
//#endif
		EItem_All				= 120,	// 모든 곳 장착 가능한 속성 아이템
		EItem_Glass				= 127,	// 안경에 장착 가능한 속성 아이템
		EItem_Coat				= 122,	// 상의에 장착 가능한 속성 아이템
		EItem_Glove				= 123,	// 손에 장착 가능한 속성 아이템
		EItem_Pants				= 124,	// 하의에 장착 가능한 속성 아이템
		EItem_Shoes				= 125,	// 발에 장착 가능한 속성 아이템

		Cash_Evoluter			= 150,	// 에보루터 - 진화슬롯 뚫어주는 케쉬 아이템
		Cash_ChangeDName		= 151,	// 디지몬 이름 변경
		Cash_ChangeTName		= 152,	// 테이머 이름 변경
		Cash_ResetSkillPoint	= 153,	// 디지몬스킬초기화
		Cash_BackupDisk			= 154,	// 더미 용병알
		Cash_ExtraInven			= 155,	// 인벤 확장
		Cash_ExtraWarehouse		= 156,	// 창고 확장
		Cash_JumpBuster			= 157,	// 점프부스터
		Cash_Resurection		= 158,	// 부활약	
//#ifdef JHB_DIGIMONSLOT_INC_20131111
		DigimonSlotInc			= 159,	//디지몬 슬롯 확장 아이템
//#endif

		Cash_DigimonCareSlot	= 160,	// 용병 보관소 슬롯 확장

		Cash_DigiCore			= 161,	// 디지코어
		Excelator				= 162,	// 초궁극체 진화시 소모 아이템, type 값이 별도 기능을 하지는 않는다.
		ChangeTamer				= 163,	// 소유 테이머를 영구 변환 시킨다.

		BattleTag				= 164,	// 전투시 디지몬 변경 아이템
		Cash_Shouter_N			= 165,	// 확성기 일반형
		Cash_Shouter_S			= 166,	// 확성기 고급형
		Cash_Shouter_NT			= 167,	// 확성기 일반형 기간제
		Cash_Shouter_ST			= 168,	// 확성기 고급형 기간제
		GM_Shouter_T			= 169,  // GM 전용 확성기 

		ScanUse_Item			= 170, // 스캔을 사용하는 아이템
		BackupDisk              = 171, // 부화 백업 디스크 아이템
		
		//CombinenceChipset		= 172, // 합체 칩셋		
		GoldBar					= 172, // 이벤트 금괴 타입
		TimeSet    				= 173, // 호박폭탄 시간
		StoreSummon				= 174, // 상점 소환 아이템 // 쓰지 않음 174번 다른용도로 사용하면 됩니다.

		AttributeItem			= 176,

		RClick_Hatch			= 177,	// 우클릭 알 부화

		GotchaCoin				= 178,	//가챠머신 코인 

		Item_TamerSKillConsume	= 180,	// 테이머 스킬 캐시 아이템(소모형)
		Fire_Work				= 181,  // 폭죽 리뉴얼
		CrossWars				= 182,	// 크로스 워즈 아이템(부화 및 칩)
		CodeCrown				= 183,	// 코드 크라운
		CrossLoader				= 184,	// 크로스 로더
		Cash_CrossInvenSlot		= 185,	// 크로스 인벤토리 증가
		Debuff					= 186,	// 디버프 아이템
		BuffEvent				= 187,	// 23013추석이벤트 (기부하는 아이템)
		Incubater				= 188,	// 디지몬 보관소 인큐베이터		

		CardMaster				= 190,	// 씰 아이템
		UnSealCardMaster		= 191,	// 씰 봉인해제
		ReSealCardMaster		= 192,	// 씰 밀봉
		EvoCapsule				= 193,	// 캡슐진화 아이템

		Cash_Shouter_Ban		= 194,	// 확성기 차단 아이템 // 꾸준글의 승리 -by 림토

		Spirit					= 195,	// 스피릿 진화 아이템
		SpiritEXP				= 196,	// 진화 경험치 아이템
		FriendShip_Increase		= 197,	// 친화력 상승 아이템		

		RelocateTamer			= 198, // 테이머 서버 이전
		TamerSlotExpansion		= 199, // 테이머 슬롯 확장

		AntiX					= 201,	// X게이지,크리스탈 충전
		SkillLevelOpen			= 202,	// 디지몬 스킬레벨 확장 대분류
		AntiXEvoluter		= 400,


		SockAll					= EItem_All,
		SockGlass				= EItem_Glass,
		SockCoat				= EItem_Coat,
		SockGlove				= EItem_Glove,
		SockPants				= EItem_Pants,
		SockShoes				= EItem_Shoes,
	};

	enum eSubType
	{
		TacticsEnchantReset_One = 5701,
		TacticsEnchantReset_All = 5702,		
		IncGuildMaxMember		= 6201,	// 길드 수용 최대 인원 확장 아이템 
		InitSkillPoint			= 6202,	// 스킬 초기화 아이템
		Oberver_Watch			= 6601,	// 옵저버 아이템 관전 가능
		Oberver_Match			= 6602, // 옵저버 아이템 관전 불가
		PersonStoreLicence		= 7601, // 개인 상점 허가증		
		CommissionShopLicence	= 7603, // 고용 상점 허가증
		Portable_Hatch			= 7701, // 휴대용 부화기
		Portable_WareHouse		= 7702,	// 휴대용 창고 이용 아이템
		Portable_DigimonCareHouse = 7703, // 휴대용 디지몬 보관소 이용 아이템
		Portable_Scanner		= 7704,	// 휴대용 스캔너 이용 아이템
		Portable_DigitamaCollect= 7705,	// 휴대용 아이템 회수 이용 아이템
		Portable_Analysis		= 7706, // 휴대용 속성 분석기
		Portable_Socket			= 7707, // 휴대용 장비 합성기
		Portable_JumpGate       = 7708, // 휴대용 장비 점프게이트

		Evoluter				= 15001, //15001	에보류터
		EvolutionLimit			= 15002, //15002	에보루션 리미트
		ToyGun					= 15003, //15003	장난감총
		GdPo					= 15004, //15004	지드포
		DramonBreaker			= 15005, //15005	드라몬브레이커
		HolySpeer				= 15006, //15006	성스러운창
		CARD_BUFF				= 6451,		// 카드 버프 아이템
		CARD_ACTIVE_SKILL		= 6452,		// 카드 ACTIVE 아이템
		//CombinenceChipset_CASH	= 17201,
		//CombinenceChipset_USE	= 17202,
		BackupDisk_Event        = 17101,    // 이벤트 백업디스크
		BackupDisk_Cash         = 17102,    // 캐쉬 백업디스크

		TaiChangeItem			= 16301,
		MinaChangeItem			= 16302,	// 미나 체인지 아이템
		MattChangeItem			= 16303,	// 매튜 체인지 아이템
		TakeruChangeItem		= 16304,	// 리키 체인지 아이템
		SoraChangeItem			= 16305,	// 소라 체인지 아이템
		HikariChangeItem		= 16306,	// 미나 체인지 아이템

		IncubaterFacilitation	= 18801,	// 디지몬 보관소 인큐베이터 촉진 아이템
		IncubaterExtraction		= 18802,	// 디지몬 보관소 인큐베이터 추출 아이템

		HeadAll					= 2101,
		HeadSavers				= 2102,
		HeadAdverture			= 2103,

		CoatAll					= 2201,
		CoatSavers				= 2202,
		CoatAdverture			= 2203,

		GloveAll				= 2301,
		GloveSavers				= 2302,
		GloveAdverture			= 2303,

		PantsAll				= 2401,
		PantsSavers				= 2402,
		PantsAdverture			= 2403,

		ShoesAll				= 2501,
		ShoesSavers				= 2502,
		ShoesAdverture			= 2503,

		CostumeAll				= 2601,		// 공용의복
		CostumeSavers			= 2602,		// 세이버즈 전용 의복
		CostumeAdverture		= 2603,		// 어드벤처 전용 의복
		CostumeMan				= 2604,		// 남자 코스튬
		CostumeWoman			= 2605,		// 여자 코스튬

		AccessoryAll			= 2701,		// 장신구 공용
		AccessorySavers			= 2702,		// 장신구 세이버즈
		AccessoryAdverture		= 2703,		// 장신구 어드벤처

		AuraFireWall			= 3102,		// 디지오라슬롯 방화벽

		SubUnSealCardMaster		= 19101,	// 카드 봉인해제 아이템 소분류 타입
		SubReSealCardMaster		= 19201,	// 카드 밀봉 아이템 소분류 타입
		NormalDigiCloneS		= 5501,
		NormalDigiCloneA		= 5502,
		NormalDigiCloneB		= 5503,
		NormalDigiCloneC		= 5504,
		NormalDigiCloneD		= 5505,
		AdvancedDigiCloneS		= 5511,
		AdvancedDigiCloneA		= 5512,
		AdvancedDigiCloneB		= 5513,
		AdvancedDigiCloneC		= 5514,
		AdvancedDigiCloneD		= 5515,
		UltimateDigiCloneS		= 5521,
		UltimateDigiCloneA		= 5522,
		UltimateDigiCloneB		= 5523,
		UltimateDigiCloneC		= 5524,
		UltimateDigiCloneD		= 5525,
		TutorialDigiClone		= 5526,
		OldDigiCloneS			= 5531,
		OldDigiCloneA			= 5532,
		OldDigiCloneB			= 5533,
		OldDigiCloneC			= 5534,
		OldDigiCloneD			= 5535,

		DigiCloneMaxStart		= 5511,
		DigiCloneMaxEnd			= 5525,

		AdditionalPotionType_Va = 6101,
		AdditionalPotionType_Vi = 6102,
		AdditionalPotionType_Da = 6103,
		AdditionalPotionType_Un = 6104,
		AdditionalPotionType_No = 6105,
		
		DigimonScaleAllChangeStep1	= 8901,
		DigimonScaleAllChangeStep2	= 8902,
		DigimonScaleAllChangeStep3	= 8903,
		DigimonScaleAllFixStep1		= 8904,

		DigimonScale3LevelHybrid	= 8911,
		DigimonScale4LevelHybrid	= 8912,
		DigimonScale5LevelHybrid	= 8913,

		DigimonScale3LevelExtra		= 8914,
		DigimonScale4LevelExtra		= 8915,
		DigimonScale5LevelExtra		= 8916,

		DigimonScale6LevelTotal		= 8917,
		
		DigimonScaleParcelOut		= 8921,
		

		Tutorialnecklace			= 2802,
		TutorialRing				= 2902,		
		TutorialEaring				= 3002,

		ParcelJewelry				= 6092,

		// NEW_BATTLE_SYSTEM
		DigimonBattleTicket			= 9001,

		AntiBodyX					= 20101,
		XCrystal					= 20102,


//#end		
	};

	enum eDrop		// item drop
	{
		Reject,		// 거부
		Field,		// field drop 허용
		Delete,		// drop 시 제거
	};

	enum eUseCharacter
	{
		UC_NONE,	// 해당 사항 없음
		UC_COMMON,	// 테이머 & 디지몬 모두 사용 가능
		UC_DIGIMON,	// 디지몬 전용		
		UC_TAMER,	// 테이머 전용
		
	};


	enum eBaseAttributes	// attributes
	{
		Type,				// 아이템유형, nItem::eType 참조
		PowerAppType,		// 아이템 능력 적용치 확정을 습득시, 사용시, 확인시로 구분 처리
		Class,				// item class

		UseMode,			// 사용제한, 전투중 사용가부
		UseSeries,			// 사용계열
		UseCharacter,		// 사용대상제한 eUseCharacter 참조
		UseTimeGroup,		// 사용시간계열

		Overlap,			// 동시보관 수
		Possess,			// 테이머당 획득 가능 수
		ReqTamerLevel,		// 착용 및 사용시 레벨 제한
		ReqDigimonLevel,	// 착용 및 사용시 레벨 제한
		ReqQuest,			// 아이템 사용 또는 착용시 퀘스트 제한(수행완료한 자만이 사용 또는 착용 가능)

		EquipSeries,		// 장착계열 - 동시장착 제한
		Trade,				// 거래 가능 여부
		Drop,				// drop 시 처리 방법

		DigiCore,			// 구입시 디지코어 소모 갯수
		StanPrice,			// 구입 표준 가격
		Cash,				// 구입 캐시
		Sale,				// 처분 가격

		SubType,			// 용도
		Value,				// 아이템 값

		dcDigimonLevel,		// drop constraint
		dcTamerLevel,		// drop constraint
		dcQuestIDX1,		// drop constraint
		dcQuestIDX2,		// drop constraint
		dcQuestIDX3,		// drop constraint

		LinkedSkill,		// skill(or spell) 관련 테이블 참조

		RateMin,			// 아이템의 능력치 적용 비율
		RateMax,			// 아이템의 능력치 적용 비율

		SocketCount,		// 아이템에 밖을 수 있는 소켓의 갯수
		SocketPowerRate,	// 속성 아이템 적용 능력치 비율

		SkillCount,			// 디지바이스에만 존재, 담을 수 있는 스킬의 갯수
		ChipsetCount,		// 디지바이스에만 존재, 담을 수 있는 칩셋 갯수

		Limited,			// 아이템이 귀속형인지, 그리고 귀속되는 시점 정보, 0:미귀속, 1:착용시 귀속, 2:습득시 귀속

		TimeLimitedType,	// 0:무관, 1:시간제 아이템(모두 사라짐), 2:능력치만 사라짐 아이템 보존, 4:아이템 사용 시 시간 적용(효과 적용 후 아이템 삭제)
		TimeLimitedAmount,	// 초단위 처리 

		Family,				// 자연 속성값

		EventCoinItemType,	// 아이템 구매시 지불할 이벤트 아이템 타입
		EventCoinItemCount,	// 아이템 구매시 지불할 이벤트 아이템 개수
//#ifdef KSJ_ADD_MEMORY_SKILL_20140805
		TargetException,	// 사용 제한 디지몬 설정
//#endif
		TimePassActive,		// #define ITEM_TIME_PASS_ACTIVE
		SvrBattle,

		MaxBaseAttribute
	};

	enum // drop 방법
	{
		DropField,
		DropDelete,
		DropReject,
	};
/////////////////////////////////////////////////////
// 선물 인벤(GIFT) 획득 실패 코드
	enum
	{
		GiftF_ITEMTYPE			= 1,		// 아이템 타입 에러
		GiftF_INVEN_NOT_EMPTY	= 2,		// 인벤 공간 부족
		GiftF_CROP_FAIL			= 3,		// 획득 처리 실패 추후 다시 시도 ( 내부 처리 실패 )
		GiftF_COUNT_OVER		= 4,		// 최대 중첩 개수 에러

	};

/////////////////////////////////////////////////////
// 아이템 고유 번호
	enum
	{
		IdxDigiCore		= 5000,	// 디지코어 아이템 번호
	};

/////////////////////////////////////////////////////
// 아이템 사용 실패 코드
	enum
	{
		USE_FAIL_EXPIRE = 1,	// 아이템 만료
		USE_FAIL_BATTLE,		// 전투중
		USE_FAIL_COOLDOWN,		// 쿨타임
		USE_FAIL_NOT_USE_AREA,	// 사용 불가능 지역
		USE_FAIL_NON_TARGET,	// 대상 미선택
		USE_FAIL_NOT_MONSTER,	// 대상이 몬스터가 아님
		USE_FAIL_NOT_RANGE,		// 사용할수 없는 거리
		USE_FAIL_NOT_USE_TARGET,// 사용할수 없는 대상 입니다.
		USE_FAIL_NOT_USE_TERMS, // 사용할수 있는 조건이 아닙니다.
		USE_FAIL_LIMIT,			// 사용제한
		USE_FAIL_NOT_INVEN,		// 인벤토리 부족 속성칩 관련
		USE_FAIL_NOT_SAME,		// 동일한 아이템 적용 불가
		USE_FAIL_NOT_USE_SCAN,	// 스캔결과 아이템값이 없어서 사용실패
		USE_FAIL_SPRIT_EXP,		// 디지몬(타입 10번)을 소환한 상태가 아닐 경우 아이템 사용 실패
		USE_FAIL_MAX_LIMIT,		// 최대치에 도달하여, 아이템 사용 실패
		USE_FAIL_NO_SKILL,		// 스킬이 없다
		USE_FAIL_OTHERS,		// 실패 
		USE_FAIL_BATTLESERV,	// 배틀 서버 사용금지
	};



/////////////////////////////////////////////////////
// created item info
	enum 
	{
//		Count, // 중복 수량
// customizing data
// enchant info
// socket info
// name customizing
	};
//#ifdef JHB_ADD_ACCESSORYSYSTEM_20131028
	enum eAccessory			// AccItem Option
	{
		AP		= 1,		// 공격력
		DP		= 2,		//방어력
		MAXHP	= 3,		//최대HP
		MAXDS	= 4,		//최대DS
		SkillAP	= 5,		//스킬데미지
		AttrAP	= 6,		//속성데미지
		CA		= 7,		//크리티컬
		CD		= 8,		//치명타데미지증가
		AS		= 9,		//공속증가
		EV		= 10,
		BL		= 11,
		HT		= 12,
		MaxSize = HT,		//배열사이즈, 항상 위에 옵션값 갯수와 같아야한다

		DigiPower	= 1,	//디지터리파워
		Renewal		= 2,	//디지어블 파워 리뉴얼
		AccOption	= 3,	//악세사리 옵션 
		OptionValue = 4,	//해당 옵션 값
	};
		
//#endif
};




/*
// 각 속성별 체크 가능하도록 처리
	enum // grade
	{
		Normal = 1, // 1 - 일반 아이템 (Normal item)		  : 아무런 통제를 하지 않으며, 동시 장착이 가능.
		High,		// 2 - 고급 아이템 (High Grade item)	  : 아무런 통제를 하지 않으며, 동시 장착이 불가.
		Cash,		// 3 - 유료 아이템 (Cash item)			  : 생성일자별로 시리얼번호 자동발행.
		Rare,		// 4 - 레어급 아이템 (Rare item)		  : 생성일자별로 시리얼번호 자동발행.
		Govern,		// 5 - 통제 아이템 (Govern item)		  : 고유시리얼번호 입력하에 발행되며, 발행된 수량만큼만 생성된다.
		Helper,		// 6 - 관리자 아이템 (Administrator item) : 운영자 및 헬퍼등의 관리자메게만 지급되며,
	};
//*/

namespace nNatualItemCode
{
	enum
	{

	};
}






namespace nCardGrade
{
	enum
	{
		Normal,			// 노말 등급
		Bronze,			// 브론즈 등급
		Silver,			// 실버 등급
		Gold,			// 골드 등급
		Platinum,		// 플래티넘 등급
		Master,			// 마스터 등급

		MAX_GRADE		// 등급 최대치
	};

	enum
	{
		None			=	0,	// 효과 없음

		DefeRateInc		=	1,	// 방어력 현재 % 증가 효과
		DefeRateDec		=	2,	// 방어력 현재 % 감소 효과

		DefeValueInc	=	3,	// 방어력 정수 증가 효과
		DefeValueDec	=	4,	// 방어력 정수 감소 효과

		APRateInc		=	5,	// 공격력 현재 %증가 효과
		APRateDec		=	6,	// 공격력 현재 %감소 효과
	
		APValueInc		=	7,	// 공격력 정수 증가 효과
		APValueDec		=	8,	// 공격력 정수 감소 효과

		MAXEFFECT		// 효과 최대치
	};
	enum
	{
		HP = 1,		//HP
		HP_PER,		//HP %
		DS,			//DS
		DS_PER,		//DS %
		AT,			//AT
		AT_PER,		//AT %
		AS,			//AS
		AS_PER,		//AS %
		CT,			//CT
		CT_PER,		//CT %
		HT,			//HT
		HT_PER,		//HT %
		DE,			//DE
		DE_PER,		//DE %
		BL,			//BL
		BL_PER,		//BL %
		EV,			//EV
		EV_PER,		//EV %
	};
};

enum eCardApplyType
{
	eCardApplyType_None = 0,
	eCardApplyType_HP_Value,
	eCardApplyType_HP_Rate,
	eCardApplyType_DS_Value,
	eCardApplyType_DS_Rate,
	eCardApplyType_AT_Value,
	eCardApplyType_AT_Rate,
	eCardApplyType_AS_Value,
	eCardApplyType_AS_Rate,
	eCardApplyType_CT_Value,
	eCardApplyType_CT_Rate,
	eCardApplyType_HT_Value,
	eCardApplyType_HT_Rate,
	eCardApplyType_DE_Value,
	eCardApplyType_DE_Rate,	
	eCardApplyType_BL_Value,
	eCardApplyType_BL_Rate,	
	eCardApplyType_EV_Value,
	eCardApplyType_EV_Rate,
	eCardApplyType_End,
};