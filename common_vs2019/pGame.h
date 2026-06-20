

#pragma once

#include "nScope.h"


namespace nGame
{
	enum
	{
		ExpEvent = 0x00000001,	// 경험치 이벤트 진행중

	};
};

namespace nLoginType
{
	enum
	{
		DigiTalic	= 0x00000001,
		PcBang		= 0x00000002,
		PlayNet		= 0x00000004,
		TooNiland	= 0x00000008,
		IMBC		= 0x00000010,
		MGAME		= 0x00000020,
		ZAM			= 0x00000040,
		STEAM		= 0x00000080,
		HANGAME		= 0x00000100,
	};

	enum EVENT_TARGET
	{
		EVENT_DIGITALIC	= 0,
		EVENT_PLAYNET	,
		EVENT_TOONILAND	,
		EVENT_IMBC,
		EVENT_MGAME,
		EVENT_STEAM,
		EVENT_HANGAME,
		EVENT_TARGET_MAX,
	};

	inline bool is_DigiTalic(int compareVal)	{	return ( DigiTalic == (compareVal & DigiTalic) ); }
	inline bool is_PcBang(int compareVal)		{	return ( PcBang == (compareVal & PcBang) ); }
	inline bool is_PlayNet(int compareVal)		{	return ( PlayNet == (compareVal & PlayNet) ); }
	inline bool is_TooNiland(int compareVal)	{	return ( TooNiland == (compareVal & TooNiland) ); }
	inline bool is_IMBC(int compareVal)			{	return ( IMBC == (compareVal & IMBC) ); }
	inline bool is_MGAME(int compareVal)		{	return ( MGAME == (compareVal & MGAME) ); }
	inline bool is_ZAM(int compareVal)			{	return ( ZAM == (compareVal & ZAM) ); }
	inline bool is_STEAM(int compareVal)		{	return ( STEAM == (compareVal & STEAM) ); }
	inline bool is_HANGAME(int compareVal)		{	return ( HANGAME == (compareVal & HANGAME) ); }
};

namespace nPlayTime
{
	enum
	{
		CASH_PLAYTIME = 0,
		NORMAL_PLAYTIME,
	};

	enum
	{
		NO_PANELTY	= 1,
		LEVEL1_PANELTY,
		LEVEL2_PANELTY,

		MAX_PENALTY = 3,
		USE_CASH	= 100,
	};
};

namespace GameServiceCode
{
	enum
	{
		SERVICECODE_COMMISSIONSHOP = 0,
		SERVICECODE_NEW_BUFF,

		MAX_SERVICE = 2,
	};
};


namespace pGame
{
	enum
	{
		Begin = nScope::Game,

		Ready,				// 게임 시작할 준비 완료
		SvrInfo,			// game server관련 정보 처리

		InitData,			// 처음 시작시 자신의 정보

		MoveTo,				// 목표 지점까지 이동후, 다음 목표 지점으로 이동
		NewPath,			// 이동중 목표 지점 변경		

		Sync,
		HpRate,				// 주변의 HpRate를 알 수 있도록 처리, 자신에게는 보내지 아니함

		Chat,
		Whisper,
		WhisperFailure,		// 귓말 실패시 송신됨

		MsgAll,				// 해당 서버 전체 송신, 아이템(type:159) 필요
		MsgSystem,			// system message, 해당 클러스터내의 전 인원 대상

		Attack,				// attack from A to B
//		AttackShape,		// attack 시, 외형만 처리
//		AttackDmgInfo,		// attack 시, 데미지 수치만 처리
		AttackMiss,			// attack miss A to B

		Skill,				// skill S from A to B 

		SetTarget,			// target 설정, target에 대한 정보를 습득 할 수 있도록 해준다.
		TargetInfo,			// target's hp, ds 정보를 동기화 시킴

		GetExp,
		LevelUp,

		Die,
		SkillDie,				// skill 에 의해 죽는 경우
		DieConfirm,				// client에서 죽음-확인 요청(이후에 마을로 전송한다.)

		HealUp,					// 차후 HP, DS 모두 처리한다.
		FsChange,				// 친밀도 증감

		EvoCard,				// 진화 카드 장착
		EvoCrest,				// 진화 문장 장착
		EvoOpen,				// 진화 슬롯 개방
		Evolution,				// 진화, 퇴화
		EvoFailure,				// 진화, 퇴화 실패

		StatUp,					// 스텟업
		ChangeStatValue,		// 스텟 결과값 변경시

		Stop,					// 이동을 3초간 멈춘다.
		StopAttack,				// 공격을 멈춘다.

		BattleOn,				// 전투모드 시작
		BattleOff,				// 전투모드 끝

		HatchIn,				// 부화 디지타마 등록
		HatchUp,				// 부화 디지몬 데이타 입력
		HatchOut,				// 부화 확인
		HatchCancel,			// 부화 취소
		HatchFailure,			// 부화 실패

		ChangePartner,			// 용병 디지몬 변경
		DeletePartner,			// 용병 제거

		AllStat,
		MoveBossMonster,		// Boss Monster 이동
		WalkBossMonster,

		RefreshScreen,
		StartMessage,			// 게임 시작시 첫 대화 메세지

		MsgAllHatchOut,			// 부화 성공시
		MsgAllCropEventItem,	// 문화 상품권등 습득

		ChangeChannel,			// 채널 변경 요청 및 응답
		OpenRegion,				// 새로운 지역에 들어간 경우

		DiedAlready,			// 이미 죽어있음

		Ting,					// 선공 몬스터가 자신을 발견

		ChangeState,			// 게임의 현재 상태값

		OpenEvoSlot,			// evolutor 또는 burst mode open 아이템 사용

		MsgShout,				// 외치기

		JumpGate,				// 점프게이트 사용

		Emoticon,				// 이모티콘 - 감정 동기화

		AdvertBossCount,		// 보스몬스터가 태어나기 전까지의 갯수
		AdvertBossCreate,		// 보스 생성됨을 해당맵 인원에게 알림
		AdvertBossDie,			// 보스몬스터 죽음을 알림

		Event,					// 현재 화면에만 뿌려줄 수 있는 이벤트 동기화

		OpenRideMode,			// 디지몬 타는 기능 오픈

		MacroQuestion,			// 게임 매크로 확인 요청, 매크로 아이템 1개를 제거한다.
		MacroAnswer,			// 게임 매크로 확인 결과 (매크로 질문에 대한 답 응답)
		MacroFailure,			// 매크로 처리 실패 관련 처리 (아이템 사용으로 대상에게 매크로 질문 날리기 실패)
		MacroSuccess,			// 매크로 확인 요청 성공 (아이템 사용으로 대상에게 매크로 질문 날리기 성공)
		MacroResult,			// 매크로 확인 처리 결과 (대상이 매크로이면 성공, 아니면 실패)

		AwayTime,				// 자리 비움 상태 요청
		SyncCondition,			// 테이머의 현재 상태

		MsgAllFailure,			// 해당 서버 외치기 - 오류

		HatchGage,				// 손실 보정 게이지 처리
		ShopInfo,				// 위탁상점 정보 전송 요청 (GAME<->CORE)

		MsgAllByItem,			// 외치기 
		DigimonInchant,			// 디지몬 강화시도 ( 이때 등록된 재료 정보를 클라이언트에서 넘겨 받습니다.)
		SaveClientOption,		// 클라이언트 옵션정보 저장

		reserve_1077,			// 예약 

		PlayTimeCheck,			// 플레이 시간 변경 및 체크
		PlayTimePaneltyChange,	// 플레이 시간 패널티 변경

		QueryMoneyInven,		// 인벤토리와 돈 정보 재 송신 요청 ( 테스트용으로만 사용 ) 항상 마지막 값에 두도록 
		ReloadData,				// 데이터 파일 갱신	
		GMHatch,				// 알 부화

		DecreaseDigimonInchant,	//d++ 디지몬 강화정보 깍기
		ChangeStat,				// 스탯 변경됨	
		KnockBack,				// 넉백 위치로
		BattleEvolPoint,		// 배틀 에볼류션 포인트

//#pragma todo("배틀 패킷 삭제 할것 시작")
		//BattleMatchInfo,		// 배틀 관련 정보
		//BattleMatchRequest,		// 등록
		//		
		//BattleMatchCancel,		// 배틀 참가 취소

		//BattleRoundResult,		// 결과
		//BattleUpdateRoundInfo,	// 라운드 업데이트

		//BattleMatchFail,		// 배틀 실패
		//BattleMatchReady,		// 배틀 여부
//#pragma todo("배틀 패킷 삭제 할것 끝")
		ClientEventSave,		// 클라이언트 이벤트 정보 저장 요청
		ViewEventSeen,			// 이벤트씬 구경중
		DotDie,					// 도트에 의해 죽었다고 전송

//#pragma todo("배틀 패킷 삭제 할것 시작")
		//DualBattleRequest,		// 대결 신청 
		//DualBattleResponse,		// 대결 응답
		//DualBattleFail,			// 대결 실패
//#pragma todo("배틀 패킷 삭제 할것 끝")

/************************************************************************/
/* 패킷 추가 생성 금지                                                  */
/************************************************************************/
/************************************************************************/
/* 패킷 추가 생성 금지                                                  */
/************************************************************************/
/************************************************************************/
/* 패킷 추가 생성 금지                                                  */
/************************************************************************/

		End
	};

}; // namespace pGame

namespace pGameTwo
{
	enum
	{
		Begin = nScope::GameTwo,

		RecompenseList,	// NPC 점검 보상 목록 요청
		RecompenseGain,	// NPC 점검 보상 획득
		RecompenseGainFail,	// 보상 획득 실패
		RecompenseChange,	// 이전 보상 받았던 목록 업데이트로 인한 갱신
		RecompenseRemove,	// 보상 리스트 제거
		AllMsg,				// 새로만든 공지
		DoorObjectOpen,		// 문열기
		HackShildCheck,		// 핵쉴드 체크
		GameGuardCheck,		// 게임가드 체크

		SkillAction,		// MONSTER_SKILL_RENEWAL_20150715 스킬 패킷 분리
		SkillHit,			// MONSTER_SKILL_RENEWAL_20150715 스킬 패킷 분리

		ErrorMessage,		// 실패 결과
		SvrInfoFailed,		// 서버 동작 실패
//#pragma todo("배틀 패킷 삭제 할것 시작")
		//DualBattleSucc,			// 대결 성공		
//#pragma todo("배틀 패킷 삭제 할것 끝")

		DigimonInchantData,	// 인탠트 차감(리셋) 후 정보 통보

		BurningEventAddExpRate,	// 버닝이벤트용 추가 경험치(비율) 통보

		MacroBanCountSaveNotice,// 매크로밴누적 횟수 저장 통보

		CheckTamerName,
		CheckTamerResult,

		Result,	// 테이머 서버 이전 관련해서 공통으로 사용

		//서버-서버용
		ArenaRankingReqTopCur,
		ArenaRankingReqTopOld,
		ArenaRankingReqFame,

		//서버-클라용
		ArenaRequestRank,
		ArenaRequestOldRank,
		ArenaRequestFame,

		ArenaRequestFameList,

		SkillHitEffect,

		DieLog,
		
		ResourceIntegrity,

		//X항체 관련
		XGToCrystal,	//X게이지 -> 크리스탈
		CrystalToXG,	//크리스탈-> X게이지
		XGuageUpdate,	//X게이지 양 갱신
		XGuageMaxUpdate,//X게이지,크리스탈 최대량 갱신

		NeonDisplay,

		XignCodeCheck,
		RegionBuff,		//맵-맵간 지역버프 , 임시
		MacroCheckOtherMap,
		InventoryInfo,
		DigimonTranscendenceChargeEXP,
		DigimonTranscendence,

		End // = 20000 // 20000까지 사용할 수 있기를
	};
};

namespace nErrorMessage
{
	enum eErrorMessage
	{
		eErrorMessage_Make_CountOver = 0,				// 제작 요청 개수 오류
		eErrorMessage_Make_ItemCountOver,				// 제작 아이템 요청 개수 오류
		eErrorMessage_Make_NoneResource,				// 제작 테이블 리소스 없음
		eErrorMessage_Item_NoneResource,				// 아이템 테이블 리소스 없음
		eErrorMessage_Make_NotEnoughInvenSlot,			// 제작 인벤 공간 부족
		eErrorMessage_Make_NotEnoughMoney,				// 제작 금액 부족
		eErrorMessage_Make_NotEnoughMaterialItem,		// 제작 재료 부족
		eErrorMessage_Repurchase_NotEnoughInvenSlot,	// 재구매 아이템 인벤 공간 부족
	};
};

namespace pMiniEvent
{
	enum
	{
		Begin = nScope::MiniEvent,
		PiemonGameStart,		// 피에몬 카드게임 시작메세지
		PiemonGameEnd,			// 피에몬 카드게임 종료
		//PiemonGameFail,		// 피에몬 카드게임 비정상 종료 차후 작업(2012 10 08 ~
		End
	};
};//(2012 10 09)

namespace pMiniGame
{
	enum
	{
		Begin = pMiniEvent::End+1,
		MiniGameStart,		// 미니게임 시작
		MiniGameClickBtn,	// 미니게임 진행중 stop버튼 클릭
		MiniGameTimeOut,	//버튼 클릭 안하고 시간 지났을
		MiniGameEnd,		// 미니게임 종료
		MiniGameLimit,		// 제한조건

		MiniGameIsStartReq,	// 미니게임 시작가능여부 요청
		MiniGameIsStartAcc, // 미니게임 시작가능여부 응답
		End
	};
};

#define d_nWhisperNotConnected	(u1)0	// 접속중이지 아니함
#define d_nWhisperMapChange		(u1)1	// 맵이동 중
#define d_nWhisperNotFound		(u1)2	// 존재하지 아니한 유저
#define d_nWhisperSuccess		(u1)3	// 송수신 성공








