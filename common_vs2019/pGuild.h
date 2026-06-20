


#pragma once

#include "nScope.h"

namespace pGuild
{
	enum
	{
		Begin		= nScope::Guild,

		Create,			// 길드 생성
		Close,			// 길드 폐쇄

		Allow,			// 멤버 추가 허가
		AllowFailure,	//
		Reject,			// 멤버 추가 거부

		Delete,			// 멤버 삭제(강퇴)
		Leave,			// 탈퇴

		Add,			// 해당 테이머를 길드에 추가했음
		ReqAdd,			// 추가 요청
		ReqAddFailure,	// 상대가 이미 길드 가입한 상태입니다.
						// nCode == 1; ==> 상대가 이미 다른 길드에 소속되어 있습니다.
						// nCode == 2; ==> 상대가 접속중아님, 또는 존재하지 않은 상대 
						// nCode == 3; ==> 상대가 다른 업무중
						// nCode == 4; ==> 길드 가입인원 FULL

		On,				// 온라인 되었음을 알림, 어느 맵에서 온라인 되는지도 알려야 함
		Off,			// 오프라인 되었음을 알림

		Info,			// 길드 정보 및 멤버 목록 요청 및 응답

		Chat,			// 길드원간의 채팅

		ToSubMember,	// 준회원
		ToMember,		// 일반 맴버로 강등
		ToDatsMember,	// 닷트요원
		ToSubMaster,	// 부길마로 승급
		ToMaster,		// 마스터 자리를 다른님에게 이전

		ChangeMap,		// 맵이나 채널 변경시
		LevelUp,		// 길드 레벨업

		MemberLevelUp,	// 길드원 레벨업시 동기화 필요

		ChangeTName,	// 테이머 이름 변경됨
		DeletedTamer,	// 테이머가 캐릭터 선택창에서 제거됨

		AddExp,			// 길드 경험치 습득

		ChangeNotice,	// 길드 공지 변경

		RankChange,		// 랭크 변경
		History,		// 길드 히스토리, 

		ChangeClassName,	// 클래스 이름(마스터,부마스터...)을 변경한다.

		IncMember,		// 아이템에 의한 길드 최대 인원수 확장
		GSPLevelUp,		// 길드 스킬 포인트 레벨업
		GSPUse,			// 길드 스킬 포인트 사용
		Memo,			// 길드 메모
		Shout,			// 길드 확성기
		Summon,			// 길드원 단체 소환
		GSkill,			// 길드스킬사용
		GSkillRemove,	// 길드스킬해제
		SkillError,		// 길드스킬사용실패

		ChangeMemo,		// 길드 멤버의 메모 변경

		End
	};
};



namespace nGuild
{
	enum
	{
		Master		= 1,	// 마스터
		SubMaster	= 2,	// 부마스터

		DatsMember	= 3,	// 닷츠회원
		Member		= 4,	// 회원
		SubMember	= 5,	// 준회원
	};

	namespace History
	{
		enum
		{
			ToMaster		= 1,	// 길드 마스터
			ToSubMaster		= 2,	// 부길마
			ToDatsMember	= 3,	// 닷츠대원
			ToMember		= 4,	// 일반 대원
			ToSubMember		= 5,	// 준회원

			Create			= 100,	// 창설

			Add				= 101,	// 가입
			Leave			= 102,	// 탈퇴
			Delete			= 103,	// 강퇴

			Close			= 104,	// 길드 폐쇠
		};
	};	

	namespace GetPoint // 길드 공헌도 얻는 방법
	{
		const int MONSTER_HUNT_COUNT = 100;		// 4번 몬스터를 100마리 잡았을 경우 공헌도 1점 가산

		enum
		{
			RaidHunt		= 1,	// 레이드몬스터사냥
			CompQuest		= 2,	// 퀘스트완료
			HatchEgg		= 3,	// 알부화
			MonsterHunt		= 4,	// 일반 몬스터 N 마리 잡을 경우
		};
	};
	namespace SkillCode
	{
		enum{
			GuildShout		= 2600001,	// 길드 확성기
			NameColor		= 2600004	// 길드 색 변경 스킬코드
		};
	};

};


struct stGuildSkill
{
	u4 nSkillCode;					// 스킬코드
	u4 nGSP;						// 소모 GSP
	u2 nStep;						// 스킬 단계					
	u2 nLevel;						// 요구 레벨
	u2 nGroup;						// 스킬 그룹 ID
	u2 nValue;						// 스킬 증가값
	u2 nTimeS;						// 지속시간 [초]
	u2 nTimeM;						// 지속시간 [분]
	u2 nCoolS;						// 쿨타임 [초]
	u2 nCoolM;						// 쿨타임 [분]
	u1 nSection;					// 스킬 구분(0 이면 패시브, 1 이면 액티브)
};
struct stGuildSkillCore
{
	u4 nSkillCode;					// 발동중인 스킬코드
	u4 nStartTime;					// 스킬발동한 시간
};