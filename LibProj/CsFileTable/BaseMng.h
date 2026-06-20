
#pragma once 

#include "DigimonSkillOpenTable.h"

#define BASE_CHARTYPE_CONSTANT		1000

class CsBaseMng
{
public:
	typedef	std::map< DWORD, CsBase* >				MAP;
	typedef	std::map< DWORD, CsBase* >::iterator	MAP_IT;

	typedef	std::map< DWORD, CsBaseMapInfo* >				MAP_MAPINFO;
	typedef	std::map< DWORD, CsBaseMapInfo* >::iterator		MAP_MAPINFO_IT;

	typedef	std::map< DWORD, CsBaseJumpBuster* >			MAP_JUMPBUSTER;
	typedef	std::map< DWORD, CsBaseJumpBuster* >::iterator	MAP_JUMPBUSTER_IT;	

public:
	enum eBASE_TAMER_TYPE
	{
		BT_TAMER_EXP	=	1,
		BT_MASARU		=	1,
		BT_TOHMA		=	2, 
		BT_YOSHINO		=	3,
		BT_HANJIHO		=	4,
		BT_TAICHI		=	5,
		BT_MIMI			=	6,
		BT_MATT			=	7,
		BT_TAKERU		=	8,
		BT_HIKARI		=	9,
		BT_SORA			=	10,
		BT_TAKATO		=	11,
		BT_RIKA			=	12,
		BT_HENRY		=	13,
 		BT_JANG			=	14,
 		BT_JUNG			=	15,
		BT_JERI			=	16,
		BT_RYO			=	17,
		BT_HIRO			=	18,
		BT_RULI			=	19,
		BT_KIYOSHIROU	=	20,
	};

	enum eBASE_DIGIMON_TYPE
	{
		BT_DIGIMON_EXP	=	1,
		BT_SHORT_TYPE	=	1,
		BT_LONG_TYPE	=	2,
		BT_DEX_TYPE		=	3,
		BT_DEF_TYPE		=	4,
	};

public:
	void			Delete();
	bool			Init( char* cPath );

protected:
	MAP				m_mapTamer;
	MAP				m_mapDigimon;
	MAP_MAPINFO		m_mapMapInfo;
	MAP_JUMPBUSTER	m_mapJumpBuster;

	//========================================================================================================
	//
	//		파티
	//
	//========================================================================================================

public:
	struct sPARTY
	{
		float		s_fPartyDist;
	};
protected:
	sPARTY			m_Party;
public:
	sPARTY*			GetParty(){ return &m_Party; }

	//========================================================================================================
	//
	//		길드
	//
	//========================================================================================================
public:
	struct sGuild
	{
		int			s_nLevel;
		UINT		s_nFame;

		int			s_nItemNo1;
		int			s_nItemCount1;
		int			s_nItemNo2;
		int			s_nItemCount2;		

		int			s_nMasterLevel;
		int			s_nNeedPerson;

		int			s_nMaxGuildPerson;
		int			s_nIncMember;
		int			s_nMaxGuild2Master;
	};	

	typedef	std::map< DWORD, sGuild* >				MAP_GUILD;
	typedef	std::map< DWORD, sGuild* >::iterator	MAP_GUILD_IT;
protected:
	MAP_GUILD			m_Guild;
	//Guild
public:
	sGuild*			GetGuildInfo( DWORD dwLevel );

	//========================================================================================================
	//
	//		제한
	//
	//========================================================================================================
public:
	struct sLIMIT
	{
		sLIMIT():s_nMaxTacticsHouse(0),s_nMaxWareHouse(0),s_nUnionStore(0),s_nConsume_XG(0),s_nCharge_XG(0)			
		{};

		SHORT		s_nMaxTacticsHouse;
		SHORT		s_nMaxWareHouse;
		SHORT		s_nUnionStore;
		
#ifdef NEW_SHARESTASH
		SHORT		s_nMaxShareStash; // 국내용 계정 공유 창고	엑셀 DM_Limit 수정 2012/8/16 vf00
#endif
#ifdef CROSSWARS_SYSTEM
		SHORT		s_nMaxCrossInven;
		SHORT		s_nMaxCrossTactics;
#endif
		unsigned int s_nConsume_XG;
		unsigned int s_nCharge_XG;
	};
protected:
	sLIMIT			m_Limit;
public:
	sLIMIT*			GetLimit(){ return &m_Limit; }

	//========================================================================================================
	//
	//		플레이 시간에 따라 페널티
	//
	//========================================================================================================
public:
	struct sPLAY_PANELTY
	{
		int		s_nPaneltyLevel;
		int		s_nExp;
		int		s_nDrop;
	};
protected:
	std::map< int, sPLAY_PANELTY* >	m_mapPlayPanelty;

public:
	sPLAY_PANELTY*	GetPlayPanelty( int nLevel ){ return m_mapPlayPanelty[ nLevel ]; }


	//========================================================================================================
	//
	//		개인상점
	//
	//========================================================================================================
public:
	struct sPERSON_STORE
	{
		void	Delete();

		struct sBASE_INFO
		{
			float		s_fPerson_Charge;
			float		s_fEmployment_Charge;
			float		s_fStoreDist;
#ifdef CONSIGNMENT_CREATE_RESTRICT
			float		s_fNpcDist;
#endif
		};
		struct sOBJECT_INFO
		{
			int		s_nItemID;
			int		s_nDigimonID;
			float	s_fScale;
			int		s_nSlotCount;
#ifdef CONSIGNMENT_BALLOON_CASH
			TCHAR	s_szFileName[MAX_FILENAME];
#endif
		};

		sBASE_INFO						s_BaseInfo;
		std::map< int, sOBJECT_INFO* >	s_mapObject;
	};

protected:
	sPERSON_STORE	m_PersonStore;
public:
	sPERSON_STORE*	GetPersonStore(){ return &m_PersonStore; }

#ifdef GUILD_RENEWAL
	//========================================================================================================
	//
	//		길드 Base
	//
	//========================================================================================================
public:
	struct sGUILDBASE{
		// 1: 레이드몬스터 1마리, 2: 퀘스트 완료 1회, 3: 알 부화 1회, 4: 몬스터 100마리(레이드몬스터 포함)
		enum ePointType{ RAIDMONSTER = 1, QUEST = 2, HATCH = 3, KILL_100 = 4, NONE, };

		struct sLEVELINFO
		{
			USHORT		s_nLevel;	// 레벨
			UINT		s_nReqExp;	// 필요경험치
		};

		struct sGUILDPOINT
		{
			USHORT		s_nType;	// 획득 조건
			USHORT		s_nPoint;	// 조건 만족 시 획득 포인트
		};

		USHORT		s_nLimitLv;		// 길드 생성 레벨 제한(테이머)
		UINT		s_nChangeRate;	// GSP 전환 비율

		std::map< int, sLEVELINFO* >	s_mapLevelInfo;
		std::map< int, sGUILDPOINT* >	s_mapPointInfo;
		void	Delete();

	};
protected:
	sGUILDBASE	m_sGuildBase;
public:
	sGUILDBASE*	GetGuildBase(){ return &m_sGuildBase; }


	//========================================================================================================
	//
	//		길드 스킬
	//
	//========================================================================================================
	typedef	std::map< DWORD, CsGuild_Skill* >				MAP_GUILDSKILL;
	typedef	std::map< DWORD, CsGuild_Skill* >::iterator		MAP_GUILDSKILL_IT;

	MAP_GUILDSKILL	m_mapGuildSkill;
	CsGuild_Skill*	GetGuildSkill( int nSkillGroup ) { return m_mapGuildSkill[nSkillGroup]; }
	bool			_IsGuildBaseSkill( int nSkillCode );
#endif

protected:
	void			_LoadExcel();

	void			_LoadExcel_TamerDigimon();
	void			_LoadExcel_MapInfo();
	void			_LoadExcel_Party();
	void			_LoadExcel_JumpBuster();
	void			_LoadExcel_Guild();
	void			_LoadExcel_Limit();
	void			_LoadExcel_PersonStore();
	void			_LoadExcel_PlayPanelty();
#ifdef GUILD_RENEWAL
	void			_LoadExcel_GuildSkill();
	void			_LoadExcel_GuildBase();
#endif
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

	void			_LoadExcel_EvolutionBase();

	void			_LoadExcel_DSkillMaxLevel();
	void			_LoadExcel_DskillOpenExpansion();

public:
	void			SaveBin( char* cPath );	


	// Base
public:
	bool			IsTamerBase( DWORD dwID ){ return ( m_mapTamer.find( dwID ) != m_mapTamer.end() ); }
	CsBase*			GetTamerBase( int nLevel, int nTamerType );

	bool			IsDigimonBase( DWORD dwID ){ return ( m_mapDigimon.find( dwID ) != m_mapDigimon.end() ); }
	CsBase*			GetDigimonBase( int nLevel, int nDigimonType );	


	// MapInfo
public:
	DWORD			GetShoutSec( DWORD nMapID ){ assert_cs( m_mapMapInfo.find( nMapID ) != m_mapMapInfo.end() ); return m_mapMapInfo[ nMapID ]->GetInfo()->s_nShoutSec; }
	bool			IsEnableCheckMacro( DWORD nMapID ){ assert_cs( m_mapMapInfo.find( nMapID ) != m_mapMapInfo.end() ); return m_mapMapInfo[ nMapID ]->GetInfo()->s_bEnableCheckMacro; }
	
	// JumpBuster
public:
	bool			IsUseJumpBuster( int nJumpBusterItemID, int nDestMapID );
	void			GetJumpBusterItemID( std::list< DWORD >* pList, int nDestMapID );	

	// 위탁 상점
public:
#ifdef CONSIGNMENT_BALLOON_CASH
	TCHAR*			IsBallonNewType( int nTargetID );
#endif

public:
	typedef std::map<int,sBaseEvolutionApply>					MAP_EVOLUTION_BASE_INFOS;
	typedef std::map<int,sBaseEvolutionApply>::iterator			MAP_EVOLUTION_BASE_INFOS_IT;
	typedef std::map<int,sBaseEvolutionApply>::const_iterator	MAP_EVOLUTION_BASE_INFOS_CIT;


	int				GetEvoApplyValue(int const& nEvoType) const;
	std::wstring	GetEvoTypeName( int const& nEvoType ) const;

	MAP_EVOLUTION_BASE_INFOS const& GetBaseEvoInfos() const;

private:
	MAP_EVOLUTION_BASE_INFOS		m_mapBaseEvolutionInfos;

public:
	// 사용하려는 아이템이 진화 타입과 같은지 체크
	bool	IsEnableSkillMaxLvUpFromEvoType( int const& nEvoType, int const& nUseItemSubType ) const;
	// 아이템으로 오픈 할 수 있는 스킬 랭크 얻어오는 함수
	int		GetOpenSkillRankFromItemSubType( int const& nUseItemSubType ) const;
	int		GetBeforOpenSkillRankeFromItemSubType( int const& nUseItemSubType ) const;

	// 최대 스킬 레벨 값으로 현재 스킬의 랭크 얻어오는 함수
	int		GetSkillRankFromSkillMaxLv( int const& nEvoType, int const& nSkillMaxLv ) const;

	int		GetSkillMaxLvFromItemSubType( int const& nEvoType, int const& nUseItemSubType ) const;
	__int64	GetDigimonSkillMaxExp( int const& nEvoType, int const& nSkillExpLv );

private:
	MAP_DSKILL_OPEN_EXPANSION_DATA	m_mapDskillOpenExpansionData;	// 아이템으로 디지몬 최대 스킬 레벨 확장 관련 데이터
	MAP_DIGIMON_EVOMAXLV			m_mapDigimonEvoMaxLevel;//디지몬의 진화 단계별 최대 스킬 레벨
};