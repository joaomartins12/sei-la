
#pragma once 

#include "TableStruct_InfiniteWar.h"

class CsInfiniteWarMng
{
public:
	// 스테이지별 데이터 모음
	typedef std::map<int,TB::sStageInfos>						StageInfos;
	typedef std::map<int,TB::sStageInfos>::const_iterator		StageInfos_CIT;

	// NPC에 따른 데이터 모음
	typedef std::map<DWORD, StageInfos>							InfinityNPCContainer;
	typedef std::map<DWORD, StageInfos>::const_iterator			InfinityNPCContainer_CIT;

	// 소환 될 몬스터 키값에 따른 데이터 모음
	typedef std::list<TB::sSummonMonsterInfo>				SummonMonsters;
	typedef SummonMonsters::const_iterator					SummonMonsters_CIT;
	
	// 소환 몬스터 그룹에 따른 데이터 모음.
	typedef std::map<int, SummonMonsters>					SummonMonsterInfoContainer;
	typedef SummonMonsterInfoContainer::const_iterator		SummonMonsterInfoContainer_CIT;

	// 일일 보상 테이블 - 포인트별 데이터 모음.
	typedef std::list<TB::sDaliyRewardInfo>					DailyRewardItems;
	typedef DailyRewardItems::const_iterator				DailyRewardItems_CIT;

	// 일일 보상 테이블 - 요일별 데이터 모음.
	typedef std::map<int, DailyRewardItems>					DailyRewardContiner;
	typedef DailyRewardContiner::const_iterator				DailyRewardContiner_CIT;

	// 랭크 보상 테이블 - 랭크 순위에 따른 데이터 모음
	typedef std::list<TB::sRankRewardInfo>					RankRewardItems;
	typedef RankRewardItems::const_iterator					RankRewardItems_CIT;

	// 랭크 보상 테이블 - 랭크 타입에 따른 데이터 모음
	typedef std::map<int, RankRewardItems>					RankRewardItemsContanier;
	typedef RankRewardItemsContanier::const_iterator		RankRewardItemsContanier_CIT;

	typedef std::map<DWORD, TB::sInfiniteAwardNpcInfo>		AwardNpcInfoMap;
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
	CsInfiniteWarMng() {};
	~CsInfiniteWarMng() {};

	bool					Init( char* cPath );
	void					Delete();

public:
	bool					LoadFilePack( char* cPath );
	void					SaveBin( char* cPath );

	InfinityNPCContainer const&		GetInfiniteWarInfos() const;
	StageInfos const*				GetStageInfo( DWORD const& dwNpcIDX ) const;
	DailyRewardContiner const&		GetRewardData_Daily() const;
	RankRewardItemsContanier const& GetRewardData_Rank() const;

	TB::sInfiniteAwardNpcInfo const* GetAwardNpcInfo(int const& nServerIDX, DWORD const& dwTableID) const;

private:
	void					_Delete();
	bool					_LoadExcel();
	bool					_LoadExcel_Main(char* pExcelFile, char* pExcelSheetName);
	bool					_LoadExcel_Monster(char* pExcelFile, char* pExcelSheetName);

	bool					_LoadExcel_Daily_Reward(char* pExcelFile, char* pExcelSheetName);
	bool					_LoadExcel_Rank_Reward(char* pExcelFile, char* pExcelSheetName);
	bool					_LoadExcel_AwardNpcInfo(char* pExcelFile, char* pExcelSheetName);

	bool					_LoadBin( char* cPath );
	bool					_LoadFilePack( char* cPath );

	void					_SaveBin( FILE* fp );

private:
	InfinityNPCContainer				m_mapInfiniteNpcInfos;
	SummonMonsterInfoContainer			m_mapSummonMonsterInfo;	// 소환할 몬스터들의 정보

	DailyRewardContiner					m_mapDailyRewardInfos;
	RankRewardItemsContanier			m_mapRankRewardInfos;

	AwardNpcInfoContanier				m_mapAwardNpcInfos;
};
