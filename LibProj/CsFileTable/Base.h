
#pragma once 

class CsBase
{
public:
	CsBase() {};
	~CsBase() {};

	struct sINFO
	{
		DWORD		s_dwID;

		USHORT		s_nLevel;
		__int64		s_dwExp;
		int			s_nHP;
		int			s_nDS;
		USHORT		s_nMoveSpeed;
		USHORT		s_nDefence;
		USHORT		s_nEvasion;
		USHORT		s_nCritical;
		USHORT		s_nAttack;
		USHORT		s_nHitRate;
	};

protected:
	sINFO			m_Info;
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsBaseMapInfo
{
public:
	struct sINFO
	{
		DWORD		s_nMapID;
		DWORD		s_nShoutSec;
		bool		s_bEnableCheckMacro;
	};
public:
	CsBaseMapInfo() {};
	~CsBaseMapInfo() {};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsBaseJumpBuster
{
protected:
	std::map< DWORD, DWORD >		m_mapJumpMapID;

public:
	CsBaseJumpBuster() {};
	~CsBaseJumpBuster() {};

	void			Delete();
	void			Insert( DWORD nDestMapID );

	bool			IsMap( DWORD nDestMapID ){ return ( m_mapJumpMapID.find( nDestMapID ) != m_mapJumpMapID.end() ); }

	std::map< DWORD, DWORD >*	GetMap(){ return &m_mapJumpMapID; }
};

struct sBaseEvolutionApply
{
public:
	sBaseEvolutionApply():m_nApplyValue(0) {};
	~sBaseEvolutionApply() {};
	
	std::wstring		m_EvolutionTypeName;
	int					m_nApplyValue;	// 능력치 적용 백분률
};

#ifdef GUILD_RENEWAL

class CsGuild_Skill
{
public:
	CsGuild_Skill(){}
	~CsGuild_Skill(){}

	enum eSkillType{ BASE = 0, TIME = 1, NONE = 2, };
	struct sINFO
	{
		USHORT		s_nSkillType;		// 0 : 기본스킬, 1 : 기간제스킬
		USHORT		s_nGroup;			// 0 : 기본스킬, 1 ~ : 같은 번호끼리 같은 그룹
		USHORT		s_nSkillLevel;		// 스킬 단계 ( 레벨 )
		UINT		s_nConsGsp;			// 소모 Gsp( guild skill point )
		USHORT		s_nReqLevel;		// 필요 길드 레벨
		UINT		s_nSkillCode;		// 스킬 코드
		TCHAR		s_szSkillName[ MAX_FILENAME ];	// 스킬 이름
		TCHAR		s_szSkillComment[ 128 ];		// 스킬 설명
	};

protected:
	std::list< sINFO* >	m_pListInfo;

public:
	void			Delete(){}
	void			InsertList( sINFO* pSrcInfo ){ m_pListInfo.push_back( pSrcInfo ); }

	std::list< sINFO* >*	GetList() { return &m_pListInfo; }

};
#endif