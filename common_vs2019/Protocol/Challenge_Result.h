#pragma once


namespace CHALLENGE_RESULT{
	enum{
		OK = 0,
		FAIL,
		NPCIDXERROR,

	};
}

namespace CHALLENGE_REWARD_INPUT_RESULT{
	enum{
		OK = 0,
		WRONG_ITEM,
		ITEM_EMPTY,
		BLOCKED_TIME
	};

}

namespace ARENA_RESULT{
	enum{
		OK = 0,			//성공
		WRONG_TYPE,		//타입정보 오류 (eRankType)
		WRONG_SEASON,	//시즌정보 오류 (없는시즌 요청)
	};

}
