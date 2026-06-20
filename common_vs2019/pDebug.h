

#pragma once


#include "nScope.h"

namespace pDebug
{
	enum
	{
		Begin = nScope::Debug,

		LoseTarget,			// target을 잃는 경우
		InsufficientRange,	// 사거리가 부족할때

		SyncInfo,

		JoinGame,	// id, cluster, tamer's name 값으로 로그인

		End
	}; // enum


}; // namespace pDebug




