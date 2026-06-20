#pragma once
#include "../plength.h"
#include "../pGame.h"

namespace nChallengeSt{
	struct st_ItemInfo{
		st_ItemInfo(u2 _nItemPos , u2 _nCount , u4 _nItemCode) : nItemPos (_nItemPos) , nCount (_nCount) , nItemCode(_nItemCode) {};
		st_ItemInfo() : nItemPos (0) , nCount (0) , nItemCode(0) {};

		u2 nItemPos;	//아이템 인벤토리 위치
		u2 nCount;		//아이템 갯수
		u4 nItemCode;	//아이템 코드
	};	


	struct st_RewardItemInfo{
		st_RewardItemInfo( u2 _nCount , u4 _nItemCode) :  nCount (_nCount) , nItemCode(_nItemCode) {};
		st_RewardItemInfo() :  nCount (0) , nItemCode(0) {};

		u2 nCount;		//아이템 갯수
		u4 nItemCode;	//아이템 코드
	};	
}

namespace nArenaRank{
	struct st_TopRankInfo{
		//st_TopRankInfo(u2 _nItemPos , u2 _nCount , u4 _nItemCode) : nItemPos (_nItemPos) , nCount (_nCount) , nItemCode(_nItemCode) {};
		st_TopRankInfo() : nRank (0) , nPoint (0) , nIsNew(0) , nModifier(0){
		};
		
		u1 nRank;										//등수
		std::string szName;			//이름
		std::string szGuild;		//길드
		u4 nPoint;										//포인트
		u1 nIsNew;										//신규진입인가
		n1 nModifier;									//등수 변동 수치
	};
	
	struct st_TopRankOldInfo{
		st_TopRankOldInfo() : nRank (0) , nPoint (0){
		};
		u1 nRank;										//등수
		std::string szName;			//이름
		std::string szGuild;		//길드
		u4 nPoint;										//포인트
	};

	struct st_TopRankFameInfo{
		st_TopRankFameInfo() : nRank (0) , nPoint (0){
		};
		u1 nRank;										//등수
		std::string szName;			//이름
		std::string szGuild;		//길드
		u4 nPoint;										//포인트
	};
}


enum eRankType{
	RANK_WEEK = 1,
	RANK_MONTH,
	RANK_SEASON,
	RANK_END
};


enum eRankStatus{
	STATUS_RUNNING = 1,	//현재랭킹 진행중
	STATUS_WAITING,		//다음랭킹 대기중
	STATUS_END			//다음랭킹 없음
};

#define df_MONSTER_SPAWN_DELAY 5000				//아레나 라운드 시작후 몹 소환까지 딜레이
#define df_CHALLENGE_FINISH_KICK_DELAY 30000	//아레나 최종 라운드 종료후 ?아낼따까지 걸리는 ms