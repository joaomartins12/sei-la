
#pragma once 

BEGIN_NAME_TABLE
// 
// class CTacticsTable : public IDataTableEvent
// {
// 	typedef std::map< DWORD, sTranscendEvo >				MAP_TRANS_EVO;
// 	typedef	std::list< sEnchantStatInfo >					LIST_ENCHANT_STAT_INFO;
// 	/* 강화 하려는 스텟별 강화 수치 데이터*/
// 	typedef	std::map< int, sEnchantRankInfo >				MAP_ENCHANT_STAT_INFO;
// 	/*DWORD : 아이템 타입 (아이템 번호가 아님)*/
// 	typedef	std::map< DWORD, sEnchantItemInfo >				MAP_ENCHANT_ITEM_INFO;
// 
// 	
// 	typedef std::map<int/*현재 단수*/, sTranscendInfo>		MAP_TRANSEVO_INFO;
// 	typedef std::map<int/*진화 타입*/, MAP_TRANSEVO_INFO>	MAP_TRANS_INFO;
// 
// 	/*디지타마*/
// 	typedef	std::map< DWORD, sDigitamToDigimon >			MAP_TACTICS_CON;
// 	typedef	std::map< DWORD, MAP_TACTICS_CON >				MAP_DIGIMON_TACTICS;
// 	
// 	typedef	std::map< DWORD, sTacticsExplain >				MAP_TACTICS_EXPLAIN;
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	// 경험치용 디지몬의 데이터
// 	typedef std::map<int, DWORD/*Exp*/>						MAP_EXPDIGIMON_FIXED;
// 	typedef std::map<DWORD, MAP_EXPDIGIMON_FIXED>			MAP_EXPDIGIMON_GROUP;
// 
// public:
// 	CTacticsTable();
// 	~CTacticsTable();
// 
// 	virtual void	InitTable(int const& nServiceType);
// 	virtual void	ReleaseTable();
// 
// 
// private:
// 	MAP_DIGIMON_TACTICS		m_mapTactics;
// 	MAP_TACTICS_EXPLAIN		m_mapTacticsExplain;
// 
// 	MAP_TRANS_INFO			m_lTacticsTransInfo;// 초월시 대상 디지몬에대한 제한 값
// 	MAP_TRANS_EVO			m_lTacticsTransEvo;	// 초월시 대상 디지몬에 필요한 아이템 값
// 	sTranscendCorrect		m_Correct;			// 초월 보정 데이터 
// 
// 	MAP_ENCHANT_ITEM_INFO	m_mapEnchantItemInfo;	// 강화 아이템 정보
// 	MAP_ENCHANT_STAT_INFO	m_mapEnchantStatInfo;	// 강화 스텟 정보
// 	MAP_EXPDIGIMON_GROUP	m_mapExpDigimonGroup;	// 경험치 디지몬 데이터
// 	//////////////////////////////////////////////////////////////////////////
// };

END_NAME_TABLE