
#pragma once 

#define CO_NAME_LEN		64

class CCharOption : public NiMemObject
{
public:
	CCharOption();
	~CCharOption();

protected:
	static float	g_fNameScaleConstant;

public:
	static void		CalNameScaleConstant();
	static float	GetNameScaleConstant();


public:
	enum ePLAG{
		NAME		=	0x00000001,
		PEN_NAME	=	0x00000002,
		GUILD_NAME	=	0x00000004,
		GUILD_MARK	=	0x00000008,
		ACHIEVE		=	0x00000010,
		AWAY		=	0x00000020,
		MONSTER		=	0x00000040,
	};

	enum eMONSTERICON
	{
		MON_ICON1 = 0, MON_ICON2, MON_ICON3, MON_ICON4, MON_ICON_END,
	};

	//====================================================================================================
	//	이름
	//====================================================================================================
protected:
	CsC_AvObject*		m_pParent;
	cText3D*			m_pTextName;
	TCHAR				m_szName[ CO_NAME_LEN ];
	DWORD				m_dwPlag;

public:
	void				Delete();
	void				Init( CsC_AvObject* pParent, LPCTSTR szName );
	void				ResetName( LPCTSTR szName );
	void				ResetNameColor();
	void				SetAlpha( float fAlpha );

	void				RenderName();

	TCHAR*				GetName(){ return m_szName; }
	void				SetName( TCHAR const* szText );
	
	bool				SetTitleMark();	

	bool				IsPlag( ePLAG plag );
#ifdef GUILD_RENEWAL
	void				SetNameColor( NiColor pColor );
#endif
	//====================================================================================================
	//	호
	//====================================================================================================
protected:
	cText3D*			m_pTextPenName;
	TCHAR				m_szPenName[ CO_NAME_LEN ];

public:
	void				SetPenName( LPCTSTR szPenName );
	TCHAR*				GetPenName(){ return m_szPenName; }

	//====================================================================================================
	//	업적
	//====================================================================================================
protected:
	cText3D*			m_pAchieve;
	DWORD				m_nAchieveID;
	NiColor				m_AchiveColor;

public:
	void				SetAchieve( DWORD nID );
	void				SetAway( bool bAway );
	void				_GetAchieveIcon( int& nIcon, std::string& strIconFileName );
	//====================================================================================================
	//	몬스터아이콘
	//====================================================================================================
protected:
	cText3D*			m_pIcon;
	DWORD				m_nIconID;

public:
	bool				SetIconMark( DWORD nID );

	//====================================================================================================
	//	길드명
	//====================================================================================================
protected:
	int					m_nGuildUID;
	cText3D*			m_pGuildName;
	TCHAR				m_szGuildName[ Language::pLength::GuildName ];
	char				m_szGuildMarkName[ MAX_PATH ];

public:
	void				DeleteGuildName();
	void				SetGuildName( LPCTSTR szGuildName );
	TCHAR*				GetGuildName() { return m_szGuildName; }
	void				SetGuildUID( int nGuildUID );
	int					GetGuildUID() { return m_nGuildUID; }
	bool				SetGuildMark( bool bUse, char* szFileName );

//MASTERS_MATCHING -테이머 이름 옆 아이콘 뿌리기, 다른곳에서 쓸 수 있게 디파인은 안걸어둠 chu8820
	//====================================================================================================
	//	테이머아이콘
	//====================================================================================================
protected:
	cText3D*			m_pMatchIcon;		// 마스터즈매칭 아이콘 (빌보드용)
	DWORD				m_nMatchIconID;		// 마스터즈매칭 팀 정보
	cText3D*			m_pTamerIcon;
	DWORD				m_nTamerIconID;

public:
	bool				SetMatchIcon( DWORD nTeam ); //0 = 팀없음, 1 = A팀, 2 = B팀
	DWORD				GetMatchTeam() { return m_nMatchIconID; }
	bool				SetTamerIcon( DWORD nID );

protected:
	int					m_nSkillCode;

public:
	void				SetSkillCode(int iSkillCode); 
};