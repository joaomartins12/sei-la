#pragma once

//////////////////////////////////////////////////////////////////////////
// 진화시 필요한 조건
struct sTransNeedValue
{
public:
	sTransNeedValue():nNeedDigimonLv(0),nNeedEnchantLv(0),nNeedEvoMinType(0),nNeedEvoMaxType(0),nNeedScale(0),nNextDigimonGrowth(0),
		nNeedHatchLvMin(0),nNeedHatchLvMax(0)
	{}
	~sTransNeedValue() {}

	// 초월
	bool IsEnableDigimonLv( int const& nCurrentDigimonLv ) const
	{
		return nCurrentDigimonLv >= nNeedDigimonLv ? true : false;
	}
	bool IsEnableEnchantLv( int const& nCurrentEnchantLv ) const
	{
		return nCurrentEnchantLv >= nCurrentEnchantLv ? true : false;
	}
	bool IsEnableScale( int const& nCurrentScale ) const
	{
		return nCurrentScale >= nNeedScale ? true : false;
	}
	// 재료
	bool IsEnableEvoType( int const& nCurrentEvoType ) const
	{
		if( nCurrentEvoType < nNeedEvoMinType || nCurrentEvoType > nNeedEvoMaxType )
			return false;
		return true;
	}
	bool IsEnableHatchLv( int const& nCurrentHatchLv ) const
	{
		if( nCurrentHatchLv < nNeedHatchLvMin || nCurrentHatchLv > nNeedHatchLvMax )
			return false;
		return true;
	}

	int nNeedDigimonLv;		// 디지몬 레벨
	int	nNeedEnchantLv;		// 강화 레벨
	int nNeedEvoMinType;	// 디지몬 진화 타입
	int nNeedEvoMaxType;	// 디지몬 진화 타입
	int nNeedScale;			// 디지몬 크기
	int nNextDigimonGrowth;	// 다음 디지몬 단수
	int nNeedHatchLvMin;	// 디지몬 필요 부화 단계
	int nNeedHatchLvMax;
};

struct sTranscendInfo
{
	sTranscendInfo() : dwTransCost( 0 ), dwMaxExp(0) {}
	~sTranscendInfo() {}

	DWORD			dwTransCost;		// 초월 비용
	DWORD			dwMaxExp;			// 초월 경험치
	sTransNeedValue	sNeedValue;	// 초월시 필요한 기본 수치
};

typedef std::map<int/*현재 단수*/, sTranscendInfo>					MAP_TRANSEVO_INFO;
typedef std::map<int/*현재 단수*/, sTranscendInfo>::iterator		MAP_TRANSEVO_INFO_IT;
typedef std::map<int/*현재 단수*/, sTranscendInfo>::const_iterator	MAP_TRANSEVO_INFO_CIT;

typedef std::map< int/*진화 타입*/, MAP_TRANSEVO_INFO >					MAP_TRANS_INFO;
typedef std::map< int/*진화 타입*/, MAP_TRANSEVO_INFO >::iterator		MAP_TRANS_INFO_IT;
typedef std::map< int/*진화 타입*/, MAP_TRANSEVO_INFO >::const_iterator	MAP_TRANS_INFO_CIT;

//////////////////////////////////////////////////////////////////////////

struct sTranscendMaterial
{
	sTranscendMaterial():dwItemType(0), nNeedCount(0), dwMinExp(0){};
	~sTranscendMaterial() {};
	DWORD	dwItemType;	// 아이템의 대분류,소분류 합친 값
	int		nNeedCount;
	DWORD	dwMinExp;		// 최소 적용 경험치
};

typedef std::list<sTranscendMaterial>					LIST_MATERIALS;
typedef std::list<sTranscendMaterial>::iterator			LIST_MATERIALS_IT;
typedef std::list<sTranscendMaterial>::const_iterator	LIST_MATERIALS_CIT;

typedef std::map<int, LIST_MATERIALS>					MAP_TRANS_USE_TYPES;
typedef std::map<int, LIST_MATERIALS>::iterator			MAP_TRANS_USE_TYPES_IT;
typedef std::map<int, LIST_MATERIALS>::const_iterator	MAP_TRANS_USE_TYPES_CIT;

struct sTranscendEvo
{
	sTranscendEvo() : dwDigimonIndex( 0 ){}
	~sTranscendEvo() {}

	DWORD				dwDigimonIndex;		// 초월 가능한 디지몬 인덱스 - 동일한 재료 디지몬을 구분할 때도 사용
	MAP_TRANS_USE_TYPES	mapMaterialInfo;
};

typedef std::map< DWORD, sTranscendEvo >					MAP_TRANS_EVO;
typedef std::map< DWORD, sTranscendEvo >::iterator			MAP_TRANS_EVO_IT;
typedef std::map< DWORD, sTranscendEvo >::const_iterator	MAP_TRANS_EVO_CIT;
//////////////////////////////////////////////////////////////////////////
// 초월 관련 보정 데이터

typedef std::map<int, float>					MAP_EVO_CORRECT;
typedef std::map<int, float>::iterator			MAP_EVO_CORRECT_IT;
typedef std::map<int, float>::const_iterator	MAP_EVO_CORRECT_CIT;

typedef std::map<int, MAP_EVO_CORRECT>					MAP_EVO_CORRECT_GROUP;
typedef std::map<int, MAP_EVO_CORRECT>::iterator		MAP_EVO_CORRECT_GROUP_IT;
typedef std::map<int, MAP_EVO_CORRECT>::const_iterator	MAP_EVO_CORRECT_GROUP_CIT;
struct sTranscendCorrect
{
	sTranscendCorrect():nEnchentDefaultCorrect(0) {}
	~sTranscendCorrect() {}

	/*0 : 불일치, 1 : 일치*/
	MAP_EVO_CORRECT_GROUP	mapSameTypeCorrect;
	int						nEnchentDefaultCorrect;//강화 보정 수치
};

//////////////////////////////////////////////////////////////////////////
// 경험치용 디지몬의 데이터
typedef std::map<int, DWORD/*Exp*/>								MAP_EXPDIGIMON_FIXED;
typedef std::map<int, DWORD/*Exp*/>::iterator					MAP_EXPDIGIMON_FIXED_IT;
typedef std::map<int, DWORD/*Exp*/>::const_iterator				MAP_EXPDIGIMON_FIXED_CIT;

typedef std::map<DWORD, MAP_EXPDIGIMON_FIXED>					MAP_EXPDIGIMON_GROUP;
typedef std::map<DWORD, MAP_EXPDIGIMON_FIXED>::iterator			MAP_EXPDIGIMON_GROUP_IT;
typedef std::map<DWORD, MAP_EXPDIGIMON_FIXED>::const_iterator	MAP_EXPDIGIMON_GROUP_CIT;