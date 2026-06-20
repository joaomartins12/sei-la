
#pragma once

#include "nScope.h"
#include <vector>

namespace pEvent
{
	enum
	{
		Begin = nScope::Event,

		DailyCheck,			// 출석 체크

		ChangeInfo,			// core->game, 이벤트 관련 정보 갱신
		CropItem,			// 게임 내에서 아이템 생성됨

		ChangeExpRate,		// 경험치 이벤트 비율 변경
		ChangeItemCount,	// 아이템 이벤트 갯수 변경

		DailyEventInfo,		// 매일 이벤트 정보

		Attendance,			// 출석 처리됨

		AddFriendly,		// 친구 초대 요청
		AddFriendlyAnswer,	// 친구 초대 결과
		FriendlyMark,		// 친구목표 달성 동기화
		FriendlyMarkItem,	// 목표 아이템 요청
		FriendlyStateChange,// 친구 초대 상태 변경

		ThanksgivingEventCnt,//2013추석 드랍율2배 상승 이벤트(월드버프)
		ThanksgivingEvent,	 //2013추석 드랍율 2배 상승 이벤트(월드버프)
		ThanksgivingEventState,//2013추석 드랍율 2배 상승 이벤트(월드버프)

		ExamParticipationPeriod,	// 시험 이벤트 참가 기간
		ExamParticipationAnswer,	// 시험 이벤트 참가 응답
		ExamQuestion,				// 시험 이벤트 문제 출제
		ExamQuestionAnswer,			// 시험 이벤트 문제 정답 여부	// 클라->서버 : 해당 답번호를 전달해주면 // 서버->클라 : 서버에서 클라에게 해당 인덱스 번호의 정답여부 알려줌
		ExamRanking,				// 시험 이벤트 랭킹 정보
		ExamMessage,				// 시험 이벤트 관련 정보
		ExamServerChange,			// 유저 초기 데이터 전달시 이벤트 상태 전달 여부
		ExamRankIndividual,			// 시험 관련 유저들의 개별 이벤트 정보

		MastersMatchingOpen,	//마스터즈 매칭 오픈
		MastersMatching,		//마스터즈 매칭 

		PinochiGetInfo,			// 피노키몬 카드게임 정보 불러오기
		PinochiVote,			// 피노키몬 카드게임 투표
		PinochiItemDelete,		// 피노키몬 카드게임 아이템 삭제

		FriendlyInfo,			// 친구 초대 정보
		FriendlyNameChange,		// 친구 테이머 이름 변경

		SuccessiveConnectDayNotice, // 유저 접속일 통보(Core서버 >> Game서버)

		CeckExpEventTime,			// 연속접속 경험치이벤트(버닝이벤트), 코어서버에서 모든유저를 체크하는데 부하문제로 클라에서 특정 시간마다 패킷전송-체크처리

		MonthlyAttendEventInfo,	// 월간 출석 이벤트 정보 

		HotTimeEvent,
		HotTimeItemRequest,

		End
	};
};



namespace nEvent
{
	enum
	{
		Active,				// 사용중 여부

		ItemTerm,			// 기간(minute)
		ItemType,			// 생산될 아이템 type

		ItemRate,			// 아이템 생산 확률
		ExpRate = ItemRate,	// 경험치 비율

		ItemNumber,			// 생산된 아이템 수
		ItemMaxNumber,		// ItemTerm 기간 동안 허용된 아이템 수량

		StartDay,			// 이벤트 시작
		EndDay,				// 이벤트 끝

		CardScan,

		MaxParameter
	};

/////////////////////////////////////////////////////
// 아이템 - 이벤트 광고 처리 관련
	enum
	{
		CropItem		= 1,	// field -> 아이템
		CapsuleToItem	= 2,	// 캡슐 -> 아이템
		DigitamaToItem	= 4,	// 디지타마 -> 아이템
	};

/////////////////////////////////////////////////////
// 친구 추천 이벤트 관련
	enum
	{
		Friendly_NONE	= 0,	// 없음
		Friendle_Type1	= 1,	// 내가 추천
		Friendle_Type2	= 2,	// 나를 추천
		Friendly_Save	= 3,	// 대상을 추천할 수 있는 상태 임
		Friendly_First	= 4,	// 최초 생성된 캐릭터
		Friendly_Recommand = 5,	// 다른 대상이 나를 친구 초대 진행중
		Friendly_NAME_CHANGE = 6,

		MARK_NONE		= 0,	// 미달성
		MARK_ARCHIVE	= 1,	// 달성
		MARK_GET		= 2,	// 달성 보상 획득
		MARK_MAX		= 3,	// 목표 달성 수치 최대 값( 지정된 수치 이상은 에러 )

		ANSWER_REJECT	= 0,
		ANSWER_OK		= 1,

		RECOMMAND_MY	= 0,	// 추천인 최대 레벨
		RECOMMAND_YOU	= 1,	// 추천 대상 최하 레벨

		MARK_TYPE_LV	= 1,	// 레벨업

		MARKITEM_GIFT	= 1,	// 기프트 인벤
		MARKITEM_CASH	= 2,	// 캐쉬샵		
	};

	enum
	{
		FRIENDLY_NOT_FOUND = 0,		// 테이머를 찾을 수 없습니다.
		FRIENDLY_REJECT,			// 친구 추천을 거부 하였습니다.
		FRIENDLY_FULL,				// 친구 추천 대상이 더 이상 친구 추천을 받을 수 없습니다.
		FRIENDLY_ALREADY,			// 이미 친구 추천을 하였기에 더이상 친구 추천을 할 수 없습니다.
		FRIENDLY_DUPLICATE,			// 친구를 중복 추천할 수 없습니다.
		FRIENDLY_OFFILE,			// 친구가 접속 상태가 아님으로 초대할 수 없습니다.
		FRIENDLY_TARGET_LEVEL_LOW,	// 친구 초대 대상의 레벨이 필요 레벨보다 낮습니다.				
		FRIENDLY_ANOTHER_ACTION,	// 친구 초대 대상이 다른 행동 중 입니다.

		FRIENDLY_NOT_GET,			// 해당 목표 아이템을 획득할 수 없는 상태 입니다.

		FRIENDLY_NOT_EVENT_DATE,	// 친구 초대 이벤트 기간이 아닙니다.
		FRIENDLY_NAME_DUPLICATE,	// 해당 이름의 친구가 이미 초대 되어 있거나 초대 받은 상태 입니다.
		FRIENDLY_NAME_CHANGE,		// 이름 변경에 따른 친구초대 이벤트를 이용할 수 없는 상태 입니다.
		FRIENDLY_NOT_USE = 99,		// 이벤트를 사용할 수 없는 상태 입니다.
		FRIENDLY_SUCCESS = 100,		// 등록 성공
		FRIENDLY_GET_SUCCESS,		// 목표 보상 획득 성공
	};

	enum
	{
		// 시험 이벤트 관련
		EXAM_NONE					= 0,	// 초기상태
		EXAM_PERIOD_END				= 1,	// 현재 진행중이지 않음
		EXAM_PARTICIPATION_PERIOD	= 2,	// 참가 기간
		EXAM_QUESTIONS_BEING		= 3,	// 문제 출제 중
		EXAM_RANKING_AGGREGATE		= 4,	// 랭킹 집계중
		EXAM_RANKING_END			= 5,	// 랭킹 집계 완료
		EXAM_FAIL_NOT_GAME			= 6,	// 가능 지역 아님
		EXAM_ANSWER_CHECK			= 7,	// 답 체크 중

		EXAM_ANSWER_NOTCOR			= 0,	// 오답
		EXAM_ANSWER_CORRECT			= 1,	// 정답
		EXAM_ANSWER_TIMEOVER		= 2,	// 시간초과 오답처리
	};
};

struct stFriendleData
{
	u8 nDBIDX;							// DB IDX
	u8 nTargetIDX;						// 테이머 IDX
	char szTamerName[Language::pLength::name+1];	// 테이머명
	u1 nType;							// 추천 타입
	u1 nMark[10];						// 달성 정보
};

namespace nMastersMatching
{
	enum
	{
		ATeam = 1,		//1팀
		BTeam = 2,		//2팀
		TeamCnt = 2,	//팀갯수
	};

};

struct stMatchingGroup
{
	u1 nGroupIDX;							//그룹번호
	u1 nLevel;								//그룹의레벨범위
	n1 RankTime[32];						//갱신되는 시간
	u4 nPersons[nMastersMatching::TeamCnt];	//인원 
};

struct stMastersMatching
{
	n8 nTamerIDX;			//테이머idx
	char szTamerName[Language::pLength::name+1];	//테이머명
	u4 nDonatedCnt;			//기부한갯수
	u2 nRank;				//테이머의 등수
	u1 nTeam;				//테이머의 팀
	u1 nGroup;				//테이머의 그룹번호
};

struct stMastersMatchingTime
{
	n4 index;
	u4 nEventStartDate;
	u4 nEventEndDate;
	u4 nRewardStartDate;
	u4 nRewardEndDate;
};

struct stMastersMatchingRewardData
{
	n4 index;
	n8 nTamerIDX;			//테이머idx
	u4 nRewardTime;
};

struct stMastersMatchingRewardItem
{		
	int m_iItemType;	
	int m_iItemCount;
};

struct stMastersMatchingRewardRank
{
	stMastersMatchingRewardRank()
		: m_iRankType(0), m_iRankStart(0), m_iRankEnd(0)
	{

	}
	int m_iRankType;
	double m_iRankStart;
	double m_iRankEnd;

	std::vector<stMastersMatchingRewardItem> m_vMastersMatchingRankItems;
};