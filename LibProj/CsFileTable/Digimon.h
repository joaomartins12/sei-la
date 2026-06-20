

#pragma once

#define DIGIMON_MAX_SKILL_COUNT		4
#define DIGIMON_MAX_FAMILY			3
#define DIGIMON_MAX_NATURE			3

class CsDigimon
{
public:
	typedef	std::map< DWORD, CsDigimon* >					MAP;
	typedef	std::map< DWORD, CsDigimon* >::iterator			MAP_IT;
	typedef	std::map< DWORD, CsDigimon* >::const_iterator	MAP_CIT;

public:
	CsDigimon();

	// ==== Info
public:
	struct sINFO
	{
		sINFO():s_dwDigimonID(0),s_dwModelID(0),s_dwBaseLevel(0),s_nBaseStat_HP(0),s_nBaseStat_DS(0),s_nBaseStat_DefPower(0)
			,s_nBaseStat_Evasion(0),s_nBaseStat_MoveSpeed(0),s_nBaseStat_CriticalRate(0),s_nBaseStat_AttPower(0),s_nBaseStat_AttSpeed(0)
			,s_nBaseStat_AttRange(0),s_nBaseStat_HitRate(0),s_nDigimonType(0),s_dwCharSize(0),s_fWakkLen(0.0f),s_fRunLen(0.0f),s_fARunLen(0.0f)
			,s_nDigimonRank(0)
		{
		}

		struct sSKILL
		{
			sSKILL():s_dwID(0),s_nReqPrevSkillLevel(0)
			{};

			DWORD		s_dwID;
			int			s_nReqPrevSkillLevel;
		};

		DWORD				s_dwDigimonID;
		DWORD				s_dwModelID;
		TCHAR				s_szName[ MAX_FILENAME ];
		char				s_cSoundDirName[ MAX_FILENAME ];
		float				s_fSelectScale;
		TCHAR				s_szEvoEffectDir[ MAX_FILENAME ];

		nsCsDigimonTable::eEVOLUTION_TYPE		s_eEvolutionType;
		nsCsDigimonTable::eATTRIBUTE_TYPE		s_eAttributeType;
		nsCsDigimonTable::eFAMILY_TYPE			s_eFamilyType[ 3 ];
		nsCsDigimonTable::eNATURE_TYPE			s_eBaseNatureType;
		nsCsDigimonTable::eNATURE_TYPE			s_eBaseNatureTypes[ 3 ] ;

		DWORD				s_dwBaseLevel;

		// 스탯 분류
		USHORT				s_nBaseStat_HP;
		USHORT				s_nBaseStat_DS;
		USHORT				s_nBaseStat_DefPower;
		USHORT				s_nBaseStat_Evasion;
		USHORT				s_nBaseStat_MoveSpeed;
		USHORT				s_nBaseStat_CriticalRate;
		USHORT				s_nBaseStat_AttPower;
		USHORT				s_nBaseStat_AttSpeed;
		USHORT				s_nBaseStat_AttRange;
		USHORT				s_nBaseStat_HitRate;

		// 케릭터 타입
		BYTE				s_nDigimonType;

		// 크기
		USHORT				s_dwCharSize;

		// 스킬
		sSKILL				s_Skill[ DIGIMON_MAX_SKILL_COUNT ];

		// 애니 이동 거리
		float				s_fWakkLen;
		float				s_fRunLen;
		float				s_fARunLen;

		//디지몬 형태
		TCHAR				s_szForm[ 64 ];

		int					s_nDigimonRank;
	};
protected:
	sINFO				m_Info;
public:
	sINFO*				GetInfo(){ return &m_Info; }
#ifdef SDM_TAMER_XGUAGE_20180628
	bool				isX_Anti(void);
	bool				isX_AntiJogress(void);
#endif


	// ==== Base
public:
	void				Delete();
	void				Init( sINFO* pSrcInfo );
};
#ifdef CROSSWARS_SYSTEM
#define CROSSDIGIMON_MAX			12

class  CsCrossDigimon
{
public:
	typedef	std::map< DWORD, CsCrossDigimon* >				MAP;
	typedef	std::map< DWORD, CsCrossDigimon* >::iterator		MAP_IT;

public:
	CsCrossDigimon();

	// ==== Info
public:
	struct sINFO
	{

		DWORD				s_dwDigimonID; // 고유번호

		TCHAR				s_szName[ MAX_FILENAME ]; //디지몬이름

		USHORT				s_nCodeCrownRequire; // 코드크라운 요구 수치
		USHORT				s_nCodeCrownTime; //  크로스 유지시간
		USHORT				s_nCrossType; // 크로스 Type
		USHORT				s_nCrossRank; // 디지몬 랭크
		DWORD				s_nCrossDigimon[CROSSDIGIMON_MAX]; // 디지몬 수

	};
protected:
	sINFO				m_Info;
public:
	sINFO*				GetInfo(){ return &m_Info; }


	// ==== Base
public:
	void				Delete();
	void				Init( sINFO* pSrcInfo );
};


#define CROSSSCENE_DIGIMON_MAX			8

class  CsCrossScene
{
public:
	typedef	std::map< DWORD, CsCrossScene* >				MAP;
	typedef	std::map< DWORD, CsCrossScene* >::iterator		MAP_IT;

public:
	CsCrossScene(){}

	// ==== Info
public:
	struct sINFO
	{
		struct sPoint
		{
			float		x;
			float		y;
		};
		struct sScene
		{
			DWORD		s_dwID;		// 등장할 디지몬
			char		s_szRegionPath[ MAX_FILENAME ]; // 영역 할당 이미지
			float		s_rx;		// 렌더텍스쳐의 좌표
			float		s_ry;
			float		s_width;		// 디지몬이 출력될 화면 사이즈
			float		s_height;
			float		s_dist;		// 화면과의 거리
			float		s_high;		// 화면에서 높이
			sPoint		s_v0;		// tx,ty 값을 기준으로 텍스쳐 uv 값
			sPoint		s_v1;
			sPoint		s_v2;
			sPoint		s_v3;
			DWORD		m_dwAni;
			float		m_fStartTime;
			float		m_fEndTime;
		};

		DWORD				s_dwDigimonID; // 고유번호

		TCHAR				s_szName[ MAX_FILENAME ]; //디지몬이름

		float				s_nCrossSceneTime; 
		WORD				s_nDigimonCount;
		char				s_szBGPath[ MAX_FILENAME ]; //씬 배경 이미지
		sScene				s_Scene[ CROSSSCENE_DIGIMON_MAX ];


	};
protected:
	sINFO				m_Info;
public:
	sINFO*				GetInfo(){ return &m_Info; }


	// ==== Base
public:
	void				Delete(){}
	void				Init( sINFO* pSrcInfo );
};
#endif

#define ENCY_MAX_OPTION		3
#define ENCY_NAME_LEN		64
#define ENCY_EXPLAIN_LEN	512

class  CsEncy_OptData	// 옵션의 이름, 아이콘, 설명 ( Data 시트 )
{
public:
	typedef	std::map< DWORD, CsEncy_OptData* >					MAP;
	typedef	std::map< DWORD, CsEncy_OptData* >::iterator		MAP_IT;

	CsEncy_OptData(){}

	struct sINFO	//예외 디지몬 정보
	{
		DWORD		s_dwOptID;							//옵션 번호
		TCHAR		s_szOptName[ ENCY_NAME_LEN ];		//옵션 이름
		USHORT		s_nIcon;							//옵션 아이콘
		TCHAR		s_szOptExplain[ ENCY_EXPLAIN_LEN ];	//옵션 설명
	};
protected:
	sINFO			m_Info;

public:
	sINFO*			GetInfo(){ return &m_Info; }

	// ==== Base
	void			Init( sINFO* pSrcInfo )	{ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
};

class  CsEncy_Exception_Digimon
{
public:
	typedef	std::map< DWORD, CsEncy_Exception_Digimon* >				MAP;
	typedef	std::map< DWORD, CsEncy_Exception_Digimon* >::iterator		MAP_IT;

	CsEncy_Exception_Digimon(){}

	struct sINFO	//예외 디지몬 정보
	{
		DWORD		s_dwDigimonID;				// 고유번호
		TCHAR		s_szName[ ENCY_NAME_LEN ];	// 디지몬이름
	};
protected:
	sINFO			m_Info;

public:
	sINFO*			GetInfo(){ return &m_Info; }

	// ==== Base
	void			Init( sINFO* pSrcInfo )	{ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
};

class  CsEncy_Deck	// 덱 정보( Book_Deck 시트 )
{
public:
	typedef	std::map< USHORT, CsEncy_Deck* >			MAP;
	typedef	std::map< USHORT, CsEncy_Deck* >::iterator	MAP_IT;

	CsEncy_Deck(){}

	struct sINFO	//덱 구성 디지몬 정보
	{
		sINFO(){ 
			s_nGroupIdx = 0; memset(s_szGroupName,0,ENCY_NAME_LEN);	memset(s_szExplain,0,ENCY_NAME_LEN);
			for( int i = 0; i < ENCY_MAX_OPTION ; i++ ) { s_nOption[i] = 0; s_nVal[i] = 0; }
		}
		USHORT		s_nGroupIdx;					//덱(그룹) 인덱스( 1001 ~ )
		TCHAR		s_szGroupName[ ENCY_NAME_LEN ];	//덱(그룹) 이름
		TCHAR		s_szExplain[ ENCY_EXPLAIN_LEN ];//덱 설명

		USHORT		s_nCondition[ ENCY_MAX_OPTION ];//옵션 발동 조건		( 1:패시브, 2:확률발동, 3:확률발동+지속시간 )
		USHORT		s_nAT_Type[ ENCY_MAX_OPTION ];	//발동 효과 체크 타입	( 0:사용안함-패시브 1:일반공격시 , 2:스킬공격시 )
		USHORT		s_nOption[ ENCY_MAX_OPTION ];	//옵션
		//옵션 : 수치 증가 ( 1:공격력	2:HP	3:스킬피해	4:크리티컬 )
		//옵션 : 확률발동  ( 5: 스킬 쿨타임 초기화 )
		//옵션 : 확률발동  ( 6: 공속증가 )		
		USHORT		s_nVal[ ENCY_MAX_OPTION ];		//옵션값
		UINT		s_nProb[ ENCY_MAX_OPTION ];		//확률
		UINT		s_nTime[ ENCY_MAX_OPTION ];		//적용시간
	};

protected:
	sINFO			m_Info;

public:
	sINFO*			GetInfo(){ return &m_Info; }

	// ==== Base
	void			Init( sINFO* pSrcInfo )	{ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
};

class  CsEncy_Deck_Composition	//덱 구성 디지몬 정보( Composition_Deck 시트 )
{
public:
	typedef	std::map< DWORD, CsEncy_Deck_Composition* >				MAP;
	typedef	std::map< DWORD, CsEncy_Deck_Composition* >::iterator	MAP_IT;

	CsEncy_Deck_Composition(){}

	struct sDATA	
	{
		sDATA(){ 
			s_dwBaseDigimonID=s_dwDestDigimonID=0;
			memset(s_szBaseDigimonName,0,ENCY_NAME_LEN); memset(s_szDestDigimonName,0,ENCY_NAME_LEN); 
		}
		DWORD		s_dwBaseDigimonID;						//계열체 번호
		TCHAR		s_szBaseDigimonName[ ENCY_NAME_LEN ];	//계열체 이름
		USHORT		s_nEvolslot;							//진화슬롯 번호
		DWORD		s_dwDestDigimonID;						//디지몬 번호
		TCHAR		s_szDestDigimonName[ ENCY_NAME_LEN ];	//디지몬 이름
	};

	struct sINFO
	{
		sINFO(){ s_nGroupIdx=0; memset(s_szGroupName,0,ENCY_NAME_LEN); s_pListData.clear(); }
		USHORT		s_nGroupIdx;					//덱(그룹) 인덱스( 1001 ~ )
		TCHAR		s_szGroupName[ ENCY_NAME_LEN ];	//덱(그룹) 이름

// 		TCHAR				s_szEffectPath[ MAX_FILENAME ];	//덱 효과 이펙트 경로
		std::list< sDATA* >	s_pListData;
		void				InsertListData( sDATA* pData ) { s_pListData.push_back( pData ); }	
	};

protected:
	sINFO			m_Info;

public:
	sINFO*			GetInfo(){ return &m_Info; }

	// ==== Base
	void			Init( sINFO* pSrcInfo )	{ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
};

//!< 디지몬의 패시브 기능을 보관합니다.
class	CsPassive_Ability_Digimon
{
public:	// DigimonID , Data
	typedef	std::map< DWORD, CsPassive_Ability_Digimon* >				MAP;
	typedef	std::map< DWORD, CsPassive_Ability_Digimon* >::iterator		MAP_IT;

	CsPassive_Ability_Digimon(){}
public:
	struct sINFO
	{
		DWORD		s_dwDigimonID;	// 디지몬 고유 ID
		short		s_shEvoNum;		// 디지몬 진화 단계
		int			s_nType;		// 효과 번호
		int			s_nValue1;		// 효과 수치1


		sINFO() : s_dwDigimonID(0) , s_shEvoNum(0), s_nType(0), s_nValue1(0){}

	};
public:
	void				Init( sINFO* pSrcInfo )	{ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
	sINFO*	GetInfo(){ return &m_Info; }

protected:
	sINFO			m_Info;

};

//	DATA_TRADE_SYSTEM
class CsData_Exchange
{

public:
	struct sINFO
	{
		int		Item_Index;					// 아이템 번호
		int		Item_Type;					// 아이템 타입
		int		Item_Code;					// 아이템 코드
		char	Item_Name[MAX_PATH];					// 아이템 이름
		USHORT	Use_Rank;					// 아이템 소모 우선 순위
		USHORT	Req_Num;					// 아이템 필요 개수
		sINFO() :	Item_Index(0), Item_Type(0), Item_Code(0), Use_Rank(0), Req_Num(0){}
		~sINFO(){}
	};

public:
	typedef std::list<sINFO>								LIST_INFO;
	typedef std::list<sINFO>::const_iterator				LIST_INFO_CITER;
	typedef std::map< DWORD, LIST_INFO >					MAP;
	typedef std::map< DWORD, LIST_INFO >::iterator			MAP_Iter;
	typedef std::map< DWORD, LIST_INFO >::const_iterator	MAP_cIter;

public:
	CsData_Exchange() {}
	virtual ~CsData_Exchange() {}

public:
	void		Init( sINFO* pSrcInfo );
	sINFO*		GetInfo();

protected:
	sINFO		m_Info;

};