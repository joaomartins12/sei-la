#pragma once

#include "nScope.h"

namespace pSession
{
	enum
	{
		Begin = nSession::User,

		Message,
		MessageConfirm,

		End
	};
};

namespace nSession
{
	enum
	{
		NONE = 0,
		DISCONNECT_CLIENT_CLOSE,	// 클라이언트 종료
		DISCONNECT_ALREADY_CLOSE,		// 이미 접속 종료됨
		DISCONNECT_PORTALFAIL_ALREADY_CLOSE,	// 포탈 이동 실패이나 이미 유저는 종료 되었음
		DISCONNECT_PORTAL_ALREADY_CLOSE,		// 포탈 이동. 이미 유저는 종료 되었음
		DISCONNECT_CONNECT_CHECK_FAIL,	// 접속 단계가 잘못 되었음 
		DISCONNECT_SERVER_CLOSE,		// 서버 종료		
		DISCONNECT_MAP_CHAGE,			// 맵 이동
		DISCONNECT_MAP_CHANGE_TIME_FAIL,// 정해진 시간내에 맵 이동 실패
		DISCONNECT_SERVER_CHECK_FAIL,	// 서버에서 데이터 체크 에러로 종료
	};

	enum
	{
		CORE_D_ACESS_USER_NOT_FOUND = 100,
		CORE_D_ACESS_SESSION_NOT_MATCH,

		CORE_D_CHANGESERVER_USER_NOT_FOUND,
		CORE_D_CHANGESERVER_SESSION_NOT_MATCH,
		CORE_D_CHANGESERVER_ENTER_ERROR,

		CORE_D_PORTAL_USER_NOT_FOUND,
		CORE_D_PORTAL_ALREADY_PORTAL,
		CORE_D_PORTAL_TAMER_NULL,
		CORE_D_PORTAL_DIGIMON_NULL,
		CORE_D_PORTAL_MAP_NOT_FOUND,

		CORE_D_CHANNEL_USER_NOT_FOUND,
		CORE_D_CHANNEL_MAP_INFO_ERROR,
		CORE_D_CHANNEL_ALREADY_MOVE,
		CORE_D_CHANNEL_TAMER_NULL,
		CORE_D_CHANNEL_DIGIMON_NULL,

		CORE_D_NEXT_MAP_NOT_FOUND,			// 다음 이동할 맵 정보 없음
		CORE_D_MAP_NOT_FOUND,				// 없는 맵 요청
		CORE_D_MAP_NOT_ONLINE,				// 해당 맵은 OFF LINE 상태

		CORE_D_FIND_TAMER_NULL,				// 테이머 정보 없음
		CORE_D_FIND_USER_NULL,				// 유저 정보 없음

		CORE_D_USER_KILL = 10000,
	};
};

namespace CONNECT_TYPE
{
	enum
	{
		MAX			= 0,
		DIGITALIC	, // (디폴트) 우리 홈페이지에서 들어오면 디지탈릭으로 표시
		PLAYNET		, // 네이버
		TOONILAND	, // 투니버스
		GATE		, //케릭터 선택창
		IMBC, // 방송사
		MGAME, // 엠게임
		ZAM			,
		STEAM		,
		HANGAME		, //한게임

		MAX_TYPE	,
	};
};