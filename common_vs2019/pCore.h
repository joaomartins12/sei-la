
#pragma once

#include "nScope.h"


namespace pCore
{
	enum
	{
		Begin = nScope::Core,
		SvrInfo,
		ConnCount,	// 접속자 수 정보 core->account
		MaxConnection, // 최대 허용 접속수 변경

		LastSaveData,
		CloseGameServer, // 게임 서버 종료전, 접속자들 강제 종료 시작, 서버 종료는 아님

		Resume,		// 사용자 접속 허용
		Suspend,	// 사용자 접속을 더이상 불허한다.

		SaveExp,
		SaveQuest,

		SaveFirstEvoUnitData,

		FullState,	// 서버 포화 상태

		OpenEvoSlotByItem,	// 아이템으로 진화 슬롯을 오픈한 경우

		NewPort,	// 새로운 네트웍 포트 번호가 필요한 경우, 상위 서버에 요청

		OpenRideMode,		// 아이템으로 타는 기능 오픈

		ReloadData,			// 데이터 파일 갱신
		LimitPlayTime,		// 제한된 플레이 시간 정보 수신

		OpenLevel,		// Account -> Core ( 서버 Open Level )
		OpenLevelInfo,

		PartialServiceOn,		// 코어 서버 서비스 상태 추가( 내부, 외부 )

		LoadDataAccess,
		LoadData,

		LoadTamer,
		LoadDigimon,
		LoadItem,

		// 테이머 서버 이전
		CheckTamerName,
		RelocateTamer,
		CheckRelocateTamer,
		IsUserOnline,

		Result,
		RelocateLoadTamer,

		
		End
	};

	enum	// FullState
	{
		Light,	 // 쾌적
		Normal,  // 보통
		Crowded, // 혼잡
		Full	 // 풀
	};

	enum	// 접속 가능 여부
	{
		AccessDenied,	// 거부
		Possible,		// 가능
		
	};
};






