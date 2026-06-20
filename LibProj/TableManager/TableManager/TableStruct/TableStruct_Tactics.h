 #pragma once
// 
// BEGIN_NAME_TABLE
// 
// struct sNeedItemInfo
// {
// 	DWORD		s_dwItemIdx;
// 	int			s_nCount;
// };
// 
// BYTE		s_nLimitLevel;		// 제한 단계
// BYTE		s_nViewWarning;		// 경고 단계
// 
// //////////////////////////////////////////////////////////////////////////
// 
// struct sTacticsExplain
// {
// 	sTacticsExplain():s_dwTacticsMonID(0) {};
// 
// 	DWORD			s_dwTacticsMonID;
// 	std::wstring	s_szTacticsName;
// 	std::wstring	s_szTacticsExplain;
// };
// 
// //////////////////////////////////////////////////////////////////////////
// 
// struct sEnchantItemInfo
// {
// 	sEnchantItemInfo():nLowLevel(0),nHighLevel(0),dwNeedMoney(0) {}
// 	~sEnchantItemInfo() {}
// 
// 	int		nLowLevel;		// 사용할수 있는 최소단계
// 	int		nHighLevel;		// 사용할수 있는 최대단계
// 	DWORD	dwNeedMoney;	// 강화 금액
// };
// 
// struct sEnchantStatInfo
// {
// 	sEnchantStatInfo():nLowEnchantLv(0),nHighEnchantLv(0),nDigimonGrowMinLv(0),nDigimonGrowMaxLv(0)
// 		,nNormalEnchantMinValue(0),nNormalEnchantMaxValue(0),nSpecialEnchantValue(0)
// 	{}
// 	~sEnchantStatInfo() {}
// 
// 	int nLowEnchantLv;		// 사용할수 있는 최소단계
// 	int nHighEnchantLv;		// 사용할수 있는 최대단계
// 	int nDigimonGrowMinLv;	// 강화 할 수 있는 디지몬의 최소 단수
// 	int nDigimonGrowMaxLv;	// 강화 할 수 있는 디지몬의 최대 단수
// 	int nNormalEnchantMinValue;	// 일반 강화시 렌덤 최소값
// 	int nNormalEnchantMaxValue;	// 일반 강화시 렌덤 최대값
// 	int nSpecialEnchantValue;	// 궁극 강화시 최대값
// };
// 
// struct sEnchantRankInfo
// {
// 	sEnchantRankInfo() {}
// 	~sEnchantRankInfo() {}
// 
// 	LIST_ENCHANT_STAT_INFO		listEnchantInfo;
// };
// 
// //////////////////////////////////////////////////////////////////////////
// // 진화시 필요한 조건
// struct sTransNeedValue
// {
// public:
// 	sTransNeedValue():nNeedDigimonLv(0),nNeedEnchantLv(0),nNeedEvoMinType(0),nNeedEvoMaxType(0),nNeedScale(0),nNextDigimonGrowth(0),
// 		nNeedHatchLvMin(0),nNeedHatchLvMax(0)
// 	{}
// 	~sTransNeedValue() {}
// 
// 	// 초월
// 	bool IsEnableDigimonLv( int const& nCurrentDigimonLv ) const
// 	{
// 		return nCurrentDigimonLv >= nNeedDigimonLv ? true : false;
// 	}
// 	bool IsEnableEnchantLv( int const& nCurrentEnchantLv ) const
// 	{
// 		return nCurrentEnchantLv >= nCurrentEnchantLv ? true : false;
// 	}
// 	bool IsEnableScale( int const& nCurrentScale ) const
// 	{
// 		return nCurrentScale >= nNeedScale ? true : false;
// 	}
// 	// 재료
// 	bool IsEnableEvoType( int const& nCurrentEvoType ) const
// 	{
// 		if( nCurrentEvoType < nNeedEvoMinType || nCurrentEvoType > nNeedEvoMaxType )
// 			return false;
// 		return true;
// 	}
// 	bool IsEnableHatchLv( int const& nCurrentHatchLv ) const
// 	{
// 		if( nCurrentHatchLv < nNeedHatchLvMin || nCurrentHatchLv > nNeedHatchLvMax )
// 			return false;
// 		return true;
// 	}
// 
// 	int nNeedDigimonLv;		// 디지몬 레벨
// 	int	nNeedEnchantLv;		// 강화 레벨
// 	int nNeedEvoMinType;	// 디지몬 진화 타입
// 	int nNeedEvoMaxType;	// 디지몬 진화 타입
// 	int nNeedScale;			// 디지몬 크기
// 	int nNextDigimonGrowth;	// 다음 디지몬 단수
// 	int nNeedHatchLvMin;	// 디지몬 필요 부화 단계
// 	int nNeedHatchLvMax;
// };
// 
// struct sTranscendInfo
// {
// 	sTranscendInfo() : dwTransCost( 0 ), dwMaxExp(0) {}
// 	~sTranscendInfo() {}
// 
// 	DWORD			dwTransCost;		// 초월 비용
// 	DWORD			dwMaxExp;			// 초월 경험치
// 	sTransNeedValue	sNeedValue;	// 초월시 필요한 기본 수치
// };
// 
// 
// //////////////////////////////////////////////////////////////////////////
// 
// struct sTranscendMaterial
// {
// 	sTranscendMaterial():dwItemType(0), nNeedCount(0), dwMinExp(0){};
// 	~sTranscendMaterial() {};
// 	DWORD	dwItemType;	// 아이템의 대분류,소분류 합친 값
// 	int		nNeedCount;
// 	DWORD	dwMinExp;		// 최소 적용 경험치
// };
// 
// 
// struct sTranscendEvo
// {
// 	sTranscendEvo() : dwDigimonIndex( 0 ){}
// 	~sTranscendEvo() {}
// 
// 	typedef std::list<sTranscendMaterial>						LIST_MATERIALS;
// 	typedef std::map<int, LIST_MATERIALS>						MAP_TRANS_USE_TYPES;
// 
// 	DWORD				dwDigimonIndex;		// 초월 가능한 디지몬 인덱스 - 동일한 재료 디지몬을 구분할 때도 사용
// 	MAP_TRANS_USE_TYPES	mapMaterialInfo;
// };
// 
// //////////////////////////////////////////////////////////////////////////
// // 초월 관련 보정 데이터
// 
// struct sTranscendCorrect
// {
// 	sTranscendCorrect():nEnchentDefaultCorrect(0) {}
// 	~sTranscendCorrect() {}
// 
// 	typedef std::map<int, float>					MAP_EVO_CORRECT;
// 	typedef std::map<int, MAP_EVO_CORRECT>			MAP_EVO_CORRECT_GROUP;
// 
// 	/*0 : 불일치, 1 : 일치*/
// 	MAP_EVO_CORRECT_GROUP	mapSameTypeCorrect;
// 	int						nEnchentDefaultCorrect;//강화 보정 수치
// };
// 
// END_NAME_TABLE
