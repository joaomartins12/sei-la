
#pragma once


#include "nScope.h"


namespace pMgr
{
	enum
	{
		Begin = nScope::Manager,

		Notice,
		BanUser,
		ServerStatus,
		ServiceActive,
		CheatLevel,
		UpdateEvent,
		DisconnectAllUser,

		ReloadTamer,		// 해당 테이머 데이타가 있는 경우 다시 로딩한다.
		
		InsertItemUser,		// 캐시샵으로 해당 아이템 추가
		InsertItemTamer,	// 테이머 선물 창고로 아이템 추가
		ServerError,		// 서버 장애 알림
		ServerLive,			// 서버 복귀 알림
		ServerClose,		// 서버 프로그램 종료
		RefreshEvent,		// 이벤트 갱신
		PlayTimeSet,		// 플레이 시간 설정

		OpenLevel,			// ( GSP )Core <-> Manager 서버 Open Level
		UpdateOpenLevel,	// ( GSP )Manager <-> DM Manager
		OpenLevelInfo,		// ( GSP )현제 OpenLevel값

		UpdateCashItemServer, // ( GSP )캐쉬 아이템 시간 업뎃( 0 = 전부, 1 = 1선, 2 = 2섭, 3 = 3섭, 4 = Test )( ItemIdx = 0 이면 시간이 남은 모든 테이머의 캐쉬 아이템 업뎃 )
		UpdateCashItemTamer, // ( GSP )지정된 테이머의 캐쉬 아이템 시간 업뎃( ItemIdx = 0 지정된 테이머의 모든 캐쉬 아이템은 업뎃 )

		ScannerInfo,		// ( 국내 ) 스캐너 정보를 준다.
		UpdateScannerUsable, // ( 국내 ) 스캐너를 등록 시킨다.
		UpdateScannerClear,	// ( 국내 ) 스캐너를 해제 한다.

		DelTamerRestore,//	// ( 공통 ) 지운 테이머를 복원 한다.
		
		DigimonInchantInfo,	//	( 국내 ) 디지몬 강화 정보를 준다.

		PersonShopLog,		// ( 공통 ) 개인상점 로그를 보여준다.
		PersonShopLogEnd,

		CommissionShopLog,	// ( 공통 ) 위탁상점 로그를 보여준다.
		CommissionShopLogEnd,

		CommissionShopOpenLog,		// ( 공통 ) 위탁 상점 오픈 로그를 보여준다.
		CommissionShopOpenLogEnd,

		PayItemAllLog,		// 유료아이템 로그 검색( 국내용 ) ( All = 유저로 검색, Select = 선택한 테이먼만 검색 )
		PayItemSelectLog,
		PayItemLogEnd,

		CashItemBuyUserLog,		// 유로 아이템 구매 로그 ( GSP용 )
		CashItemBuyTamerLog,
		CashItemBuyLogEnd,

		CashItemTrackTamerLog,	// 유로 아이템 추적( GSP 용 )
		CashItemTrackUserLog,
		CashItemTrackLogEnd,

		GuildInfo,				// 길드 관련 정보만 보낸다.
		GuildMember,			// 길드 맴버들 정보를 보낸다.
		GuildInfoEnd,			// 길드 조회 검색 버튼을 막힌걸 풀어준다.
		GuildTamerInfo,			// 테이머에 길드관련 정보를 준다.

		ServerNotifyEnd,		// SvrMgr::Server_MoniterLog 패킷의 완료를 알려 준다.

		CashItemBuyLog_Global,	//유료아이템 전체 검색

		MultipleItems,			//다수의 아이템 지급
		GuildRuleInfo,			//검색하고 싶은거 선택하여 검색
		MoneySearch,			//서버별 가지고 있는 돈 검색
		UserMemoSave,			//운영자들이 유저들 저장해 놓는 메모 저장
		MapNotice,				//특정맵에 공지
		TamerNameChange,		//테이머이름변경 로그
		TamerNameChangeEnd,
		CommissionShopBuy,		//위탁상점에서 구매한 로그 
		CommissionShopBuyEnd,
		QuestReset,				//퀘스트 강제 포기
		MapTamerList,			//맵/채널별 테이머 정보 가져옴 

		SearchScannerOwner,	// ( 국내 ) 스캐너 소유자 찾음

		ChatLogOnManager,		// (국내) 운영툴에 확성기/길드확성/외치기/귓속말 로그띄움

		QuestOn,				//퀘스트 강제 완료
		QuestOff,				//퀘스트 강제 삭제

		CashItemImmediatelyDelete,			//캐시창고에서 해당 아이템 즉시 삭제
		DigimonDataExchange,
		DigimonDataExchangeEnd,
		ReservedNotifyList,					// 예약 공지 리스트
		ReservedNotifyUpdate,				// 예약 공지 수정
		ReservedNotifyDelete,				// 예약 공지 삭제

		QuestCompleteLog,
		QuestCompleteLogEnd,

		ItemOptionUpdate,					// 아이템 옵션 갱신
		ItemOptionUpdateLog,				// 아이템 옵션 로그
		ItemOptionUpdateLogEnd,

		MoneyLog,							// 돈 로그
		MoneyLogEnd,

		EvolutorLog,						// 디지몬 진화슬롯 개방 로그
		EvolutorLogEnd,

		OpenRideModeLog,					// 디지몬 라이딩 슬롯 개방 여부 확인가능한 로그
		OpenRideModeLogEnd,

		ChangePosition,						// 테이머 위치 조정, 강제조정으로 Off시(밴처리 후에) 해야함

		DigimonInchantLog,
		DigimonInchantLogEnd, 

		CommissionShopInfo,
		ShopForceClose,

		UserBanlogInfo,							// 유저 밴 기록 조회
		DigimonEvolutionInfo,
		DigimonEvolutionInfoEnd,

		QuestExpandInfo,

		// 테이머 서버 이전
		PassThroughCheckTamerName,
		PassThroughRelocateTamer,

		Result,	// 테이머 서버 이전 관련해서 공통으로 사용

		DigimonDieLog,
		DigimonDieLogEnd,
		
		IpDenySearch,
		IpDenyAdd,
		IpDenyDelete,

		MacDenySearch,
		MacDenyAdd,
		MacDenyDelete, 

		MoneyDailyLog,// 9799    패킷끝 9800 조심할것
		////////////////////////////////////////////////////////
		/////////////////프로토콜추가금지///////////////////////
		////////////////////////////////////////////////////////
		End
	};

	
};

namespace pMgrTwo
{
	enum
	{
		Begin = nScope::ManagerTwo, //25000
		MacroCheck,

		End	//~30000
	};
}

namespace ManagerSearchType
{
	enum
	{
		UserID = 0,
		TamerName,
		UserIDX,
		TamerIDX,
		DelteTamer,
	};
}

namespace nsMacroCheckResult
{
	enum
	{
		TamerNotFound = 0,
		NotInGame,
		InShop,
		InAway,
		AlreadyCheck,
		UseSuccess,
		MacroResponse,
		MacroChecked,
	};
}