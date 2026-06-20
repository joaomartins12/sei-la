

#pragma once

#include "pNetwork.h"

namespace nLog
{
	namespace Tamer
	{
		enum
		{
			AccountIDX,			// 소유자 계정 IDX
			TotalPlayTime,		// 전체 게임 플레이 시간(초단위 처리)
			AcquireDigitama,	// 디지타마 획득
			KillCountMonster,	// 몬스터 제거 합계

			LastConnectedTick,	// 최근 접속 시간 - 접속 종료시 마지막 플레이 시간 산출을 위해 사용

			MaxData
		};
	};
};


namespace nLog
{
	namespace Guild
	{
		enum
		{
			Create			=  1,	// 길드 생성
			Close			=  2,	// 길드 삭제
			AddMember		=  3,	// 길드 멤버 추가
			DelMember		=  4,	// 길드 멤버 제거

			ToMaster		=  5,	// 마스터 변경
			ToSubMaster		=  6,	// 부마스터 임명
			ToDatsMember	=  7,	// 닷트 멤버로 변경

			LeaveMember		=  8,	// 멤버 자진 탈퇴

			ToSubMember		=  9,	// 준회원으로 변경
			ToMember		= 10,	// 일반회원으로 변경
		};
	};
};

namespace pLog
{
	enum
	{
		BEGIN = nScope::Log,

		In,
		Out,
		Portal,

		dDie,
		dExpUp,
		dLevelUp,
		dSkillUp,

		tExpUp,
		tLevelUp,
		tSkillUp,
		//10
		iMoney,
		iMove,
		iTrade,
		iDrop,
		iCrop,
		iBuy,
		iSell,

		Item,
		CropEventItem,

		Cheat,
		//20
		Hatch,
		Digimon,

		Tamer,

		ChangeName,

		MasterCmd,

		Guild,			// 길드 생성/삭제
		GuildMember,	// 길드 멤버 관련 연산 처리

		ExpItem,		// Exp연관된 아이템 사용시 처리
		PersonShop,		// 개인 상점 관련 로그
		ShopOpenClose,	// 위탁 상점 오픈 & 클로즈 로그
		//30
		ShopItemLog,	// 위탁상점 아이템 판매 & 구매 로그
		HatchOptionLog, // 부화보정 로그
		PCBangLog,		// PC방 접속 & 로그아웃 정보 (유저/플레이시간)
		DigimonInchant,	// 디지몬 강화 로그

		BattleLog,		// 배틀 추가후 인덱스값을 얻어옴
		BattleTamerResultLog,	// 배틀 테이머별 로그
		FriendlyEvent,

		AttributeExp,
		GotchaMachineLog,	//가차머신 로그 
		TamerSkill,		// 테이머 스킬 적용 확인
		//40
		AccessoryLog,	//악세사리 관련 수치 로그

		MoneyLog,
		GuildShout,		// 길드확성기

		DigimonDataChange,
		Quest,
		GuildExp,

		BattleTicketLog,	// 배틀 티켓 로그

		LogDie,		//죽음 로그
		LogMacroProtector,	//매크로 프로텍터 활성 로그
		DungeonCreate,
		DigimonTranscendenceChargeEXP,

		END
	};
};



