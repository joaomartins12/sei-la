
#pragma once 

BEGIN_NAME_TABLE

class CInfiniteWarTable : public IDataTableEvent
{
public:
	// 스테이지별 데이터 모음
	typedef std::map<int,sStageInfos>					StageInfos;
	typedef std::map<int,sStageInfos>::const_iterator	StageInfos_CIT;

	// NPC에 따른 데이터 모음
	typedef std::map<DWORD, StageInfos>						InfinityNPCContainer;
	typedef std::map<DWORD, StageInfos>::const_iterator		InfinityNPCContainer_CIT;

	// 소환 될 몬스터 키값에 따른 데이터 모음
	typedef std::list<sSummonMonsterInfo>				SummonMonsters;
	typedef SummonMonsters::const_iterator					SummonMonsters_CIT;

	// 소환 몬스터 그룹에 따른 데이터 모음.
	typedef std::map<int, SummonMonsters>					SummonMonsterInfoContainer;
	typedef SummonMonsterInfoContainer::const_iterator		SummonMonsterInfoContainer_CIT;

	// 일일 보상 테이블 - 포인트별 데이터 모음.
	typedef std::list<sDaliyRewardInfo>					DailyRewardItems;
	typedef DailyRewardItems::const_iterator				DailyRewardItems_CIT;

	// 일일 보상 테이블 - 요일별 데이터 모음.
	typedef std::map<int, DailyRewardItems>					DailyRewardContiner;
	typedef DailyRewardContiner::const_iterator				DailyRewardContiner_CIT;

	// 랭크 보상 테이블 - 랭크 순위에 따른 데이터 모음
	typedef std::list<sRankRewardInfo>					RankRewardItems;
	typedef RankRewardItems::const_iterator					RankRewardItems_CIT;

	// 랭크 보상 테이블 - 랭크 타입에 따른 데이터 모음
	typedef std::map<int, RankRewardItems>					RankRewardItemsContanier;
	typedef RankRewardItemsContanier::const_iterator		RankRewardItemsContanier_CIT;

	typedef std::map<DWORD, sInfiniteAwardNpcInfo>		AwardNpcInfoMap;
	typedef AwardNpcInfoMap::const_iterator					AwardNpcInfoMap_CIT;

	typedef std::map<int, AwardNpcInfoMap>					AwardNpcInfoContanier;
	typedef AwardNpcInfoContanier::const_iterator			AwardNpcInfoContanier_CIT;

protected:
	typedef StageInfos::iterator							StageInfos_IT;
	typedef InfinityNPCContainer::iterator					InfinityNPCContainer_IT;
	typedef SummonMonsters::iterator						SummonMonsters_IT;
	typedef SummonMonsterInfoContainer::iterator			SummonMonsterInfoContainer_IT;
	typedef DailyRewardItems::iterator						DailyRewardItems_IT;
	typedef DailyRewardContiner::iterator					DailyRewardContiner_IT;
	typedef RankRewardItems::iterator						RankRewardItems_IT;
	typedef RankRewardItemsContanier::iterator				RankRewardItemsContanier_IT;
	typedef AwardNpcInfoContanier::iterator					AwardNpcInfoContanier_IT;

public:
	CInfiniteWarTable();
	~CInfiniteWarTable();

	virtual void						InitTable(int const& nServiceType);
	virtual void						ReleaseTable();


	InfinityNPCContainer const&			GetInfiniteWarInfos() const;
	StageInfos const*					GetStageInfo( DWORD const& dwNpcIDX ) const;
	DailyRewardContiner const&			GetRewardData_Daily() const;
	RankRewardItemsContanier const&		GetRewardData_Rank() const;

	sInfiniteAwardNpcInfo const*		GetAwardNpcInfo(int const& nServerIDX, DWORD const& dwTableID) const;

private:
	bool								Load_SummonsMain(DataStream& pReader);
	bool								Load_SummonsMonster(DataStream& pReader);
	bool								Load_DailyReward(DataStream& pReader);
	bool								Load_Rank_Reward(DataStream& pReader);
	bool								Load_Awards(DataStream& pReader);

private:
	InfinityNPCContainer				m_mapInfiniteNpcInfos;
	SummonMonsterInfoContainer			m_mapSummonMonsterInfo;	// 소환할 몬스터들의 정보
	DailyRewardContiner					m_mapDailyRewardInfos;
	RankRewardItemsContanier			m_mapRankRewardInfos;
	AwardNpcInfoContanier				m_mapAwardNpcInfos;
};



END_NAME_TABLE