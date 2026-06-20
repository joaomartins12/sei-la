

#pragma once


#include "nScope.h"

namespace pSvr
{
	enum
	{
		Begin = nScope::Svr,

		ClusterList,
		SelectCluster,

		Change,					// Game 서버 변경
		GoBackGate,				// game 서버에서 gate server로 
		GoBackAccount,			// gate -> account server

		AccessCode,				// account에 접속시 AccessCode 가 생성됨, 
								// 해당 값으로 core server에서 인증 처리함

		KillGate,
		KillSession,			// UserIDX

		SelectPortal,			// request and success
		SelectPortalFailure,	// failure

		LocalPortal,			// local portal success

		TryLogin,				// 이미 로그인 되어 있는데, 또 시도하는 경우, 코어 서버에 이미 접속중인 자의 연결을 끊어달라고 요청

		ChannelInfo,			// 채널 목록과 채널 상태를 송신

		Pause,					// 서버 일시 정지
		Resume,					// 서버 재개

		PrimiumStart,			// pc방 서비스 정보
		AlramExpire,			// pc방 서비스 종료전 5분전 알림
		TimeExpire,				// pc방 서비스 종료
		PrimiumChange,			// pc방 서비스 등급 변경
		SelectCharacter,		// 캐릭터를 선택하여 게임을 시작

		KoreaNumberCheck,	

		SMS,

		MoveInstGame,

		TEST_GameServerLog,
		ChangeVersionCheck,
		PCBangServerDown,		// PC방서포터 서버가 떨어지면 모든유저 피시방 효과 삭제하기 위해 사용

		SkinUser,
		KillSession2,			// TamerIDX

		EncryptionControl,

		TimeServerUserKick,		//시간제맵 종료로 유저를 다 ?아냄
		TimeMapClose,
		TimeMapOpen,

		ResourceHashReload,

		End
	};
};








