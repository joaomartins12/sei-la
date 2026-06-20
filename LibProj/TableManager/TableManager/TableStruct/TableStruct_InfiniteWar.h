#pragma once

BEGIN_NAME_TABLE
//////////////////////////////////////////////////////////////////////////
// 소환 몬스터 정보
//////////////////////////////////////////////////////////////////////////
struct sSummonMonsterInfo
{
	sSummonMonsterInfo():s_dwMonsterIDX(0),s_nCount(0)
	{}
	~sSummonMonsterInfo() {};

	DWORD	s_dwMonsterIDX;
	int		s_nCount;
};

//////////////////////////////////////////////////////////////////////////
// 무한 대전 관련 정보
//////////////////////////////////////////////////////////////////////////
struct sStageInfos
{
	sStageInfos() : s_nSummon_Type(0), s_dwRewardItemIdx(0),s_nWinPoint(0),s_nLosePoint(0) {}
	~sStageInfos() {};

	int				s_nSummon_Type;			// 소환 타입	 0 : 모두 소확, 1 : 한놈만 소환, 2 : 두놈 ...
	int				s_nMonsterInfoIndex;	// 소환할 몬스터의 정보
	DWORD			s_dwRewardItemIdx;		// 승리/패배시 획드 포인트 만큼 지급 되는 아이템 코드
	int				s_nWinPoint;			// 승리시 지급 포인트
	int				s_nLosePoint;			// 패배시 지급 포인트
};

//////////////////////////////////////////////////////////////////////////
// 보상 아이템 정보
//////////////////////////////////////////////////////////////////////////
struct sRewardItemInfo
{
	sRewardItemInfo() : s_dwItemCode(0), s_nCount(0)
	{}
	~sRewardItemInfo() {};

	DWORD	s_dwItemCode;
	int		s_nCount;
};


//////////////////////////////////////////////////////////////////////////
// 일일 보상 아이템 정보
//////////////////////////////////////////////////////////////////////////
struct sDaliyRewardInfo
{
	enum {
		eSunDay = 0,
		eMonDay,
		eTuesDay,
		eWednesDay,
		eThursDay,
		eFriDay,
		eSaturDay
	};
	sDaliyRewardInfo() : s_nNeedPoint(0),s_dwNeedItemCode(0)
	{}
	virtual ~sDaliyRewardInfo() {};

	DWORD	s_dwNeedItemCode;		// 필요 아이템 코드
	int		s_nNeedPoint;			// 필요 포인트
	std::list<sRewardItemInfo>	s_listRewardItems;
};

enum eInfiniteWarRewardType
{
	eDay = 0,	// 일일 보상
	eWeek,		// 주간 랭킹 보상
	eMonth,		// 월간 랭킹 보상
	eSeason,	// 시즌 랭킹 보상
};

//////////////////////////////////////////////////////////////////////////
// 랭크 보상 아이템 정보
//////////////////////////////////////////////////////////////////////////
struct sRankRewardInfo
{
	sRankRewardInfo():s_nRankType(0), s_nRankMin(0), s_nRankMax(0)
	{}
	virtual ~sRankRewardInfo() {};

	int		s_nRankType;	// 0 : 절대 순위, 1 : 퍼센테이지 순위
	int		s_nRankMin;		// 최소 순위
	int		s_nRankMax;		// 최대 순위
	std::list<sRewardItemInfo>	s_listRewardItems;
};

struct sInfiniteAwardNpcInfo
{
	sInfiniteAwardNpcInfo():s_dwModelID(0)
	{};

	std::wstring	s_wTitleName;
	std::wstring	s_wTamerName;
	std::wstring	s_wGuildName;
	DWORD			s_dwModelID;
	std::wstring	s_wTalkMsg;
};
END_NAME_TABLE