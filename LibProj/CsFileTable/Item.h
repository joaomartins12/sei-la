
#pragma once

class CsItemCoolTime
{
public:
	CsItemCoolTime() {};
	~CsItemCoolTime() {};

	typedef	std::map< DWORD, CsItemCoolTime* >				MAP;
	typedef	std::map< DWORD, CsItemCoolTime* >::iterator	MAP_IT;

public:
	struct sINFO
	{
		sINFO():s_nGroupID(0),s_bNetwork(false),s_fTimeSec(0)
		{};
		~sINFO() {};

		DWORD		s_nGroupID;
		bool		s_bNetwork;
		double		s_fTimeSec;
	};

protected:
	sINFO				m_Info;	
	CsCoolTimeSeq		m_CoolTimeSeq;

public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); m_CoolTimeSeq.SetEndTime( m_Info.s_fTimeSec ); }
	sINFO*			GetInfo(){ return &m_Info; }

	CsCoolTimeSeq*	GetSeq(){ return &m_CoolTimeSeq; }
};

class CsItem
{
public:
	CsItem() {};
	~CsItem() {};

	enum eEnableType
	{
		eEnableUse,		// 사용할 수 있다
		eNeedMinLv,		// 최소레벨 부족
		eOverMaxLv		// 최대레벨 이상
	};

	typedef	std::map< DWORD, CsItem* >				MAP;
	typedef	std::map< DWORD, CsItem* >::iterator	MAP_IT;
public:
	struct sINFO
	{
		sINFO():s_dwItemID(0),s_nIcon(0),s_nClass(0),s_btCodeTag(0),s_nType_L(0),s_nType_S(0),s_nTypeValue(0)
			,s_nSellType(0),s_nUseMode(0),s_nUseTimeGroup(0),s_nOverlap(0),s_nTamerReqMinLevel(0),s_nDigimonReqMinLevel(0)
			,s_nTamerReqMaxLevel(0),s_nDigimonReqMaxLevel(0),s_nPossess(0)
			,s_nEquipSeries(0),s_nUseCharacter(0),s_bDummy(false),s_nDrop(0),s_nEventItemType(0),s_dwEventItemPrice(0),s_dwDigiCorePrice(0)
			,s_dwScanPrice(0),s_dwSale(0),s_bModel_Loop(false),s_bModel_Shader(false),s_dwSkill(0),s_btApplyRateMax(0),s_btApplyRateMin(0)
			,s_btApplyElement(0),s_nSocketCount(0),s_dwSoundID(0),s_nBelonging(0),s_nQuest1(0),s_nQuest2(0),s_nQuest3(0)
			,s_nDigiviceSkillSlot(0),s_nDigiviceChipsetSlot(0),s_nQuestRequire(0),s_btUseTimeType(0),s_nUseTime_Min(0),s_nUseBattle(0)
			,s_nDoNotUseType(0)
#ifdef ITEM_USE_TIME_PASS_FILE
			,s_bUseTimeType(0)
#endif
#ifdef DEBUFF_ITEM
			,s_nSection(0)
#endif
		{
			memset( &s_szName, 0, sizeof( s_szName ) );
			memset( &s_szComment, 0, sizeof( s_szComment ) );
			memset( &s_cNif, 0, sizeof( s_cNif ) );
			memset( &s_szTypeComment, 0, sizeof( s_szTypeComment ) );
			memset( &s_cModel_Nif, 0, sizeof( s_cModel_Nif ) );
			memset( &s_cModel_Effect, 0, sizeof( s_cModel_Effect ) );
		}
		~sINFO() {};

		DWORD		s_dwItemID;
		TCHAR		s_szName[ ITEM_NAME_LEN ];
		
		int			s_nIcon;
		TCHAR		s_szComment[ ITEM_COMMENT_LEN ];
		char		s_cNif[ ITEM_FILE_LEN ];

		USHORT		s_nClass;
		TCHAR		s_szTypeComment[ ITEM_NAME_LEN ];
		BYTE		s_btCodeTag;
		USHORT		s_nType_L;
		USHORT		s_nType_S;
		int			s_nTypeValue;
#ifdef DEBUFF_ITEM
		int			s_nSection;		// 아이템 타입 소분류를 그대로 저장
#endif

		USHORT		s_nSellType;
		BYTE		s_nUseMode;

		USHORT		s_nUseTimeGroup;
		USHORT		s_nOverlap;
		USHORT		s_nTamerReqMinLevel;
		USHORT		s_nTamerReqMaxLevel;
		USHORT		s_nDigimonReqMinLevel;
		USHORT		s_nDigimonReqMaxLevel;

		USHORT		s_nPossess;
		USHORT		s_nEquipSeries;
		USHORT		s_nUseCharacter;

		bool		s_bDummy;
		USHORT		s_nDrop;

		DWORD		s_nEventItemType;
		USHORT		s_dwEventItemPrice;
		USHORT		s_dwDigiCorePrice;	

		DWORD		s_dwScanPrice;		
		DWORD		s_dwSale;

		char		s_cModel_Nif[ ITEM_FILE_LEN ];
		char		s_cModel_Effect[ ITEM_FILE_LEN ];
		bool		s_bModel_Loop;
		bool		s_bModel_Shader;

		USHORT		s_nSkillCodeType;
		DWORD		s_dwSkill;

		BYTE		s_btApplyRateMax;
		BYTE		s_btApplyRateMin;
		BYTE		s_btApplyElement;
		USHORT		s_nSocketCount;
		USHORT		s_dwSoundID;

		BYTE		s_nBelonging;
		int			s_nQuest1;
		int			s_nQuest2;
		int			s_nQuest3;

		BYTE		s_nDigiviceSkillSlot;
		BYTE		s_nDigiviceChipsetSlot;
		DWORD		s_nQuestRequire;

		BYTE		s_btUseTimeType;
		DWORD		s_nUseTime_Min;				// 사용시간 (분)

		BYTE		s_nUseBattle;				//	배틀시 사용가능한지

		USHORT		s_nDoNotUseType;			// 아이템 사용 불가능한 타입 확인용

#ifdef ITEM_USE_TIME_PASS_FILE
		bool		s_bUseTimeType;	//2017-03-23-nova 시간제최초사용
#endif
		CsItemCoolTime*	GetCoolTime();
		CsCoolTimeSeq*	GetCoolTimeSeq();

		bool			IsCashType() const;
		bool			IsPCBangType() const;
		bool			IsNoticeItem() const;
		bool			IsAccessory() const;
		bool			IsAccStone() const;

		bool			IsEnableTamerLv(int const& nTamerLv) const;
		bool			IsEnableDigimonLv(int const& nDigimonLv) const;
		eEnableType		IsEnableTamerLv(int const& nTamerLv, int& nNeedLv) const;
		eEnableType		IsEnableDigimonLv(int const& nDigimonLv, int& nOverLv) const;
		void			GetTamerReqLv( int& nMinLv, int& nMaxLv ) const;
		void			GetDigimonReqLv( int& nMinLv, int& nMaxLv ) const;
	};
protected:
	sINFO			m_Info;	

public:
	static void		TypeS_to_TypeLS( int nTypeS, int& nResTypeL, int& nResTypeS );

	void			Delete();
	void			Init( sINFO* pSrcInfo );
	sINFO*			GetInfo();
	std::wstring	GetName() const;
	bool			IsEnableTamerLv(int const& nTamerLv) const;
	bool			IsEnableDigimonLv(int const& nDigimonLv) const;
	eEnableType		IsEnableTamerLv(int const& nTamerLv, int& nNeedLv) const;
	eEnableType		IsEnableDigimonLv(int const& nDigimonLv, int& nOverLv) const;
	void			GetTamerReqLv( int& nMinLv, int& nMaxLv ) const;
	void			GetDigimonReqLv( int& nMinLv, int& nMaxLv ) const;
};


class CsElementItem
{
public:
	CsElementItem() {};
	~CsElementItem() {};

	typedef	std::map< DWORD, CsElementItem* >				MAP;
	typedef	std::map< DWORD, CsElementItem* >::iterator		MAP_IT;
public:
	struct sINFO
	{	
		sINFO():s_dwItemID(0) 
		{};
		~sINFO() {};

		DWORD		s_dwItemID;		
	};

protected:
	sINFO			m_Info;	

public:	
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
	sINFO*			GetInfo(){ return &m_Info; }	
};

class CsItemExchange	//물물교환 상점
{
public:
	CsItemExchange() {};
	~CsItemExchange() {};

	typedef	std::map< DWORD, CsItemExchange* >				MAP;
	typedef	std::map< DWORD, CsItemExchange* >::iterator	MAP_IT;
public:
	struct sINFO
	{		
		sINFO():s_dwNpcID(0),s_dwItemIndex(0),s_dwItemID(0),s_dwExchange_Code_A(0),s_dwExchange_Code_B(0),s_dwCount(0)
				,s_dwExchange_Code_C(0),s_dwExchange_Code_D(0),s_dwPropertyA_Price(0),s_dwPropertyB_Price(0),s_dwPropertyC_Price(0),s_dwPropertyD_Price(0)
		{}
		~sINFO() {};

		DWORD		s_dwNpcID;

		USHORT		s_dwItemIndex;
		DWORD		s_dwItemID;

		int			s_dwExchange_Code_A;
		int			s_dwExchange_Code_B;
		int			s_dwExchange_Code_C;
		int			s_dwExchange_Code_D;

		USHORT		s_dwPropertyA_Price;
		USHORT		s_dwPropertyB_Price;
		USHORT		s_dwPropertyC_Price;
		USHORT		s_dwPropertyD_Price;

		USHORT		s_dwCount;
	};
protected:
	sINFO			m_Info;	

public:	

	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
	sINFO*			GetInfo(){ return &m_Info; }	
};

#define MAX_OPT_COUNT	16
class CsAccessory_Option
{
public:
	CsAccessory_Option() {};
	~CsAccessory_Option() {};

	typedef	std::map< DWORD, CsAccessory_Option* >				MAP;
	typedef	std::map< DWORD, CsAccessory_Option* >::iterator	MAP_IT;
public:
	//옵션 정보
	struct sOPTION
	{
		sOPTION():s_nOptIdx(0),s_nMin(0),s_nMax(0)
		{};
		~sOPTION(){};

		SHORT		s_nOptIdx;		//옵션 종류
		UINT		s_nMin;			//해당 옵션 최소값
		UINT		s_nMax;			//해당 옵션 최대값
	};

	struct sINFO
	{		
		sINFO():s_nIdx(0),s_nGainOpt(0),s_nLimit(0)
		{};
		~sINFO(){};

		UINT		s_nIdx;			//Idx(스킬번호)
		SHORT		s_nGainOpt;	//추가되는 옵션 수
		SHORT		s_nLimit;			//강화 제한 횟수
		sOPTION		m_OptInfo[ MAX_OPT_COUNT ];	//해당 Idx 옵션 정보
	};
protected:
	sINFO			m_Info;	

public:	

	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
	sINFO*			GetInfo(){ return &m_Info; }	
};
class CsAccessory_Enchant
{
public:
	CsAccessory_Enchant() {};
	~CsAccessory_Enchant() {};

	typedef	std::map< DWORD, CsAccessory_Enchant* >				MAP;
	typedef	std::map< DWORD, CsAccessory_Enchant* >::iterator	MAP_IT;
public:
	struct sINFO
	{	
		sINFO():s_nIdx(0),s_nOpt(0),s_nVal(0)
		{};
		~sINFO(){};

		UINT		s_nIdx;			//Idx(스킬번호)
		SHORT		s_nOpt;			//인첸트 아이템 종류( 1 = 디지터리 파워, 2 = 업그레이드 횟수 )
		SHORT		s_nVal;			//강화 수치
	};
protected:
	sINFO			m_Info;	

public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
	sINFO*			GetInfo(){ return &m_Info; }	
};

//////////////////////////////////////////////////////////////////////////
// 디지몬 파츠 정보
//////////////////////////////////////////////////////////////////////////
struct sDEquipStruct
{
	enum eParts
	{
		eSkin = 0,
	};
	sDEquipStruct() :dwDigimonTBIdx(0), nPartIndex(0)
	{}

	DWORD		dwDigimonTBIdx;// 착용할 디지몬의 테이블 IDX
	std::string fileName;
	int			nPartIndex;
};
