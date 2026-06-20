
#pragma once 

struct sEnchantItemInfo
{
	sEnchantItemInfo():nLowLevel(0),nHighLevel(0),dwNeedMoney(0) {}
	~sEnchantItemInfo() {}

	int		nLowLevel;		// 사용할수 있는 최소단계
	int		nHighLevel;		// 사용할수 있는 최대단계
	DWORD	dwNeedMoney;	// 강화 금액
};

/*DWORD : 아이템 타입 (아이템 번호가 아님)*/
typedef	std::map< DWORD, sEnchantItemInfo >					MAP_ENCHANT_ITEM_INFO;
typedef	std::map< DWORD, sEnchantItemInfo >::iterator		MAP_ENCHANT_ITEM_INFO_IT;
typedef	std::map< DWORD, sEnchantItemInfo >::const_iterator	MAP_ENCHANT_ITEM_INFO_CIT;


struct sEnchantStatInfo
{
	sEnchantStatInfo():nLowEnchantLv(0),nHighEnchantLv(0),nDigimonGrowMinLv(0),nDigimonGrowMaxLv(0)
					  ,nNormalEnchantMinValue(0),nNormalEnchantMaxValue(0),nSpecialEnchantValue(0)
	{}
	~sEnchantStatInfo() {}

	int nLowEnchantLv;		// 사용할수 있는 최소단계
	int nHighEnchantLv;		// 사용할수 있는 최대단계
	int nDigimonGrowMinLv;	// 강화 할 수 있는 디지몬의 최소 단수
	int nDigimonGrowMaxLv;	// 강화 할 수 있는 디지몬의 최대 단수
	int nNormalEnchantMinValue;	// 일반 강화시 렌덤 최소값
	int nNormalEnchantMaxValue;	// 일반 강화시 렌덤 최대값
	int nSpecialEnchantValue;	// 궁극 강화시 최대값
};

typedef	std::list< sEnchantStatInfo >					LIST_ENCHANT_STAT_INFO;
typedef	std::list< sEnchantStatInfo >::iterator			LIST_ENCHANT_STAT_INFO_IT;
typedef	std::list< sEnchantStatInfo >::const_iterator	LIST_ENCHANT_STAT_INFO_CIT;

struct sEnchantRankInfo
{
	sEnchantRankInfo() {}
	~sEnchantRankInfo() {}

	LIST_ENCHANT_STAT_INFO		listEnchantInfo;
};

/* 강화 하려는 스텟별 강화 수치 데이터*/
typedef	std::map< int, sEnchantRankInfo >					MAP_ENCHANT_STAT_INFO;
typedef	std::map< int, sEnchantRankInfo >::iterator			MAP_ENCHANT_STAT_INFO_IT;
typedef	std::map< int, sEnchantRankInfo >::const_iterator	MAP_ENCHANT_STAT_INFO_CIT;
