#pragma once

typedef enum eContentsType
{
	E_CT_LOGO,
	E_CT_LOGIN,
	E_CT_SECOND_PASSWORD,
	E_CT_SERVER_SELECT,
	E_CT_CHARACTER_SELECT,
	E_CT_CHARACTER_CREATE,
	E_CT_LOADING,
#ifdef USE_SHUTDOWN
	E_CT_SHUTDOWN_SYSTEM,
#endif
	E_CT_INVENTORY_CONTENTS,
	E_CT_WAREHOUSE,
	E_CT_MAIL_SYSTEM,
	E_CT_ITEMPRODUCTION,			// 아이템 제작
	E_CT_CHATTING_STANDARDIZATION,	// 채팅창(시스템,배틀로그) 표준화
	E_CT_CARD_EVENT,		// 카드 이벤트 시스템
	E_CT_MAP_INTERFACE,		// 맵 콘텐츠
	E_CT_DATA_TRADE_SYSTEM,	// 데이터 교환 시스템
	E_CT_ITEM_REWARD,		// 아이템 지급 창고.
	E_CT_FRIENDRECOMMEND_EVENT,	// [5/12/2016 hyun] 친구 추천 이벤트
	E_CT_STORE_SYSTEM,			// [8/23/2016 hyun] 상점 시스템

	E_CT_CHANELLSELECT,		// 채널셀렉트

	E_CT_DATS_CENTER,
	E_CT_DAMAGE_METER,

	E_CT_EVENT_CONTENTS,
	E_CT_TRADE,					// 아이템 거래 컨텐츠

	E_CT_FRIENDLIST,		//친구 목록 및 차단 목록

	E_CT_DIGITAMA_CONTENTS,		//2017-03-22-nova 아이템스캔및회수

	E_CT_MAKETACTICS_CONTENTS,	//2017-04-11-nova 용병 디지몬 부화 / 인큐베이터

	E_CT_PERSONSTORE_CONTENTS,	//2017-04-27-nova 개인상점
	E_CT_UNIONSTORE_CONTENTS,	//2017-04-19-nova 위탁상점 보관함
	E_CT_ENCYCLOPEDIA_CONTENTS,	//2017-05-10-nova 디지몬도감
	E_CT_ACHIEVE_CONTENTS,		//2017-06-08-nova 업적
	E_CT_BATTLE_SYSTEM,			// 배틀 시스템 컨텐츠
	E_CT_PARTYSYSTEM,	
	E_CT_RESOURCEINTEGRITYCHECKER,
	E_CT_SEALMASTER,		//씰 마스터	
	
	E_CT_COMMUNITY,			//길드

	E_CT_MACROPROTECT,				// 매크로 프로텍트
	E_CT_SERVERCHANGE_CONTENTS,		// 서버 변경 컨텐츠
	E_CT_TUTORIAL_CONTENTS,			// 튜토리얼 시스템
	E_CT_INFINITEWAR_CONTENTS,		// 무한대전
	E_CT_INFINITEWAR_RANKREWARD_CONTENTS,		// 무한대전 보상
	
	E_CT_DIGIMON_ARCHIVE_CONTENTS,
	E_CT_MAINFRAME_CONTENTS,
	E_CT_NEON_DISPLAY_CONTENTS,	// 전광판 

	E_CT_NEW_CASHSHOP,
	E_CT_EVOLUTION_STORE_CONTENTS,	// 진화 상점
#ifdef SDM_VIP_SYSTEM_20181105
	E_CT_VIPMEMBERSHIP_CONTENTS,
#endif
	E_CT_GOTCHAMACHINE_CONTENTS,	// 가챠머신
	E_CT_ELEMENTITEM_CONTENTS,		// 속성아이템
	E_CT_ENCHANT_OPTION_CONTENTS,	// 옵션 아이템 감정 / 강화
	E_CT_QUEST_PROGRESS_CONTENTS,	// 퀘스트 진행
	E_CT_EVENT_NOTICE,
	E_CT_SEASON_PASS,
	E_CT_END,
}E_CONTENTS_TYPE;

namespace CONTENTS_EVENT
{
	typedef enum eContentsEvent
	{
		EEvt_None = 0,
		
		EEvt_PrepareDestroy,
		EEvt_CharacterCreate_Success,
		EEvt_Party_New,				// 새로운 파티에 들어갔거나 파티를 만들었다
		EEvt_Party_Leave,			// 내가 파티를 떠났다.
		EEvt_Party_Banish,			// 내가 파티에서 강퇴 榮?
		EEvt_BattleLoading_Start,	
		EEvt_MacroProtect_Start,	// 매크로 프로텍터 질문이 왔다.
		EEvt_EndEvent,
	}E_CONTENTS_EVENT;

	typedef enum eContentsStreamEvent
	{
		EStreamEvt_SecondPassword_Type,
		EStreamEvt_PartyMember_Digimon_Change,	// 파티원의 디지몬 변경
		EStreamEvt_PartyMember_Map_Change,		// 파티원의 맵 이동.
		EStreamEvt_PartyMember_Info_Change,		// 파티원 정보 변경.
		EStreamEvt_PartyMember_Leave,			// 파티 맴버가 떠났다
		EStreamEvt_PartyMember_Banish,			// 파티 맴버 강퇴 榮?
		EStreamEvt_PartyMember_New,				// 새로운 파티 맴버가 들어왔다
		EStreamEvt_PartyMember_Faraway,			// 맴버가 멀리 가거나 오거나
		EStreamEvt_Guild_Channel,				// 길드 채널 정보 세팅
		//EStreamEvt_VipMemberShip_Change,		// Vip Membership 변경
		EStreamEvt_Inventory_NewItem,			// 인벤토리 새 아이템 정보
		EStreamEvt_Inventory_UseItem,			// 인벤토리에서 우클릭으로 아이템 사용
		EStreamEvt_Cashshop_ShowProductItem,	// 캐쉬샵의 제품 선택 이벤트
		EStreamEvt_NotMatch_MapNameResource,	// 리소스 맵 이름이 다르다.
		EstreamEvt_MainFrame_SetTarget,			// 타겟팅
		EstreamEvt_MainFrame_ReleaseTarget,		// 타겟팅 해제
		EStreamEvt_EventNotice,

	}E_CONTENTS_STREAM_EVENT;
}
