
#pragma once

#include "nScope.h"

namespace pBuddy
{
	enum eBuddy
	{
		Begin = nScope::Buddy,

		Add,
		Delete,

		Ban,			// 차단 시킴

		List,			// 친구 및 차단 목록 요청

		Memo,			// 친구, 차단에 대한 메모

		Removed,		// 테이머가 제거됨
		NotFound,		// 등록하려 하는데.. 상대를 찾을 수 없음

		On,				// 로그온됨
		Off,			// 로그오프됨

		ChangeName,		//

		End
	};



};






