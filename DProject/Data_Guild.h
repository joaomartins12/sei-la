
#pragma once 
#ifdef GUILD_RENEWAL
#define GUILD_MEMO_SIZE		32
#endif
class cData_Guild : public NiMemObject
{
public:
	cData_Guild();

public:
	enum eRANK{ GR_NONE = 0, GR_MASTER, GR_2NDMASTER, GR_DAT, GR_NORMAL, GR_NEW, GR_END };
	struct sGUILD_INFO 
	{
		sGUILD_INFO() :s_nExp(0), s_nGuildLevel(0), s_nPersonCount(0),s_nCurPersonCount(0),s_nMaxPersonCount(0)
			,s_nIncMember(0), s_n2AndMasterNum(0)
#ifdef GUILD_RENEWAL
			,s_nGuildSkillPoint(0)
#endif
		{ }

		int			s_nGuildLevel;
		int			s_nExp;
		int			s_nPersonCount;
		int			s_nCurPersonCount;
		int			s_nMaxPersonCount;
		int			s_nIncMember;
		int			s_n2AndMasterNum;
#ifdef GUILD_RENEWAL
		uint		s_nGuildSkillPoint;	// 길드스킬포인트
#endif

		std::wstring s_szGuildName;
		std::wstring s_szMasterName;
		std::wstring s_szNotice;
	};

	struct sGUILD_PERSON_INFO : public NiMemObject
	{
		sGUILD_PERSON_INFO() : s_bInit(false), s_bOnline(false), s_eRank(GR_NONE),s_nPersonLevel(0)
			,s_nChannel(0), s_nContribution(0), s_Tick(0.0f),s_dwMapID(0),s_dwTamerType(80001)
#ifdef GUILD_RENEWAL
			, s_nUserRank(0), s_nLastPlayTime(0)
#endif
		{ 
	
		}
		
		bool		s_bInit;	//게임 접속 알림변수
		float		s_Tick;
		bool		s_bOnline;
		DWORD		s_dwTamerType;
		int			s_nChannel;
		eRANK		s_eRank;
		int			s_nPersonLevel;
		int			s_nContribution;
		DWORD		s_dwMapID;		
		std::wstring s_szName;

#ifdef GUILD_RENEWAL
		USHORT			s_nUserRank;		// 공헌랭크
		uint			s_nLastPlayTime;	// 최종접속시간
		std::wstring	s_szGuildMemo;;	// 길드원 메모
#endif
	};

	struct sHISTORY_INFO : public NiMemObject
	{
		sHISTORY_INFO():s_nType(0), s_nMasterClass(0),s_nMemberClass(0)
		{}

		int		s_nType;		
		int		s_nMasterClass;
		int		s_nMemberClass;
		std::wstring	s_szMemberName;
		std::wstring	s_szMasterName;

		// 시간 정보
		uint	s_nTime;     		
	};

#ifdef GUILD_RENEWAL
	//사용중인 스킬 정보
	struct sGUILDSKILL_INFO
	{
		sGUILDSKILL_INFO():s_nSkillCode(0),s_nEndTime(0),s_nCount(1)
		{  }

		uint	s_nSkillCode;	// 스킬코드( 비교용 )
		uint	s_nEndTime;		// 남은 사용 시간( 사용중이 아닐 때 : 0, 사용중일때 : 종료 시간 ) - Base스킬일 땐 쿨타임 끝나는 시간
		u1		s_nCount;		// 스킬 중첩 수 ( 기본 1, 중첩 시 1씩 증가 )
		bool	_IsCoolTime();	// 쿨타임 중인가 ( true : 쿨타임 중 , false : 쿨타임 아님 )
	};
	std::map< uint, sGUILDSKILL_INFO* >	m_mapUsingSkill;					// 현재 적용 중인 스킬 (맵)
	sGUILDSKILL_INFO*					GetUsingSkill( u4 nSkillCode );		// 스킬 코드로 적용중인 스킬 얻기( 쿨타임/적용기간, 스킬 존재 유무 확인용 )
	void								SetUsingSkill( bool bUsing , u4 nSkillCode, u4 nEndTime = 0, u4 nUseTime = 0 );	// 적용 중인 스킬 추가
	void								SetBaseSkill( int nLevel );			// 레벨에 따라 Base길드스킬 -> 적용 스킬 리스트에 넣기
	NiColor								CheckPasiveSkill();					// Base스킬 중 패시브 스킬 능력치 적용 
	int									CheckActSkill( int nSkillCode );	// 사용 하려는 스킬보다 상위 스킬이 적용 중인지 확인 ( 0 : 같은그룹 아님, 1: 같은 그룹의 상위 존재, 스킬코드: 같은 그룹이지만 하위 스킬 존재 )
	bool								IsActSkillExist();					// 액티브스킬 존재 유무( 길드버프 아이콘 표시 확인을 위함 )
	int									GetUsingSkillValue( int nApply );	// 적용 중인 스킬의 수치 얻어오기
	void								ClearUsingSkill();					// 적용중인 스킬 초기화
	int									GetOnlineCount();
#endif

public:
	void	Delete();
	void	DeleteHistory();
	void	LogOut() { Delete(); m_bNoticeShow = false; } 
	void	Update();

	void	SetGuildName( TCHAR const* szGuildName );
	void	InitNetOff();		

protected:
	sGUILD_INFO							m_GuildInfo;
	sGUILD_PERSON_INFO*					m_MyInfo;
	CsVectorPB< sGUILD_PERSON_INFO* >	m_vpPersonInfo;	//안씀. m_ListPersonInfo 으로 대체됨.
	CsVectorPB< sHISTORY_INFO* >		m_vpHistoryInfo;
	bool								m_bNoticeShow;
	bool								m_bOnOff;

	TCHAR								m_szGuildRankName[ GR_END ][ Language::pLength::GuildClassName ];
	TCHAR								m_szGuildRankDesc[ GR_END ][ MAX_PATH ];

public:
	typedef std::list< sGUILD_PERSON_INFO* >				MAP_GUILD;
	typedef std::list< sGUILD_PERSON_INFO* >::iterator		MAP_GUILD_IT;

	MAP_GUILD				m_ListPersonInfo;
	MAP_GUILD*				GetMap() { return &m_ListPersonInfo; }
	int						GetSize() { return m_ListPersonInfo.size(); }
#ifdef GUILD_RENEWAL
	void					_PersonSort( int nButton, bool bSort );			// 길드인원 정렬
	void					SetGuildSkillPoint( uint nGSP ) { m_GuildInfo.s_nGuildSkillPoint = nGSP; }	// 길드 스킬 포인트 셋
	void					SetGuildMemo( TCHAR const * szName, TCHAR const * szMemo );	// 길드원 메모 셋
#else
	void					PersonSort( int nType, int nButton, bool bSort );
#endif
	void					ReName( TCHAR const * szFrom, TCHAR const* szTo );

public:
	bool					IsGuild() { return ( m_GuildInfo.s_szGuildName.length() > 0 ) ? true : false ; }
	bool					IsSendInvate( TCHAR const * szTamerName );
	sGUILD_INFO*			GetGuildInfo(){ return &m_GuildInfo; }
	sGUILD_PERSON_INFO*		GetMyInfo(){ return m_MyInfo; }
	sGUILD_PERSON_INFO*		GetInfo( int nDataIdx );
	eRANK					GetMyRank(){ return ( m_MyInfo != NULL ) ? m_MyInfo->s_eRank : GR_NONE; }
	eRANK					GetRank( TCHAR const * szName );
	sHISTORY_INFO*			GetHistoryInfo( int nIdx ) { return ( m_vpHistoryInfo.IsExistElement( nIdx ) ) ? m_vpHistoryInfo[ nIdx ] : NULL; }

	void					SetGuildInfoData();	// 길드 정보 변경
	void					SetGuildPersonInfo( sGUILD_PERSON_INFO* pPerson ); // 정보 갱신때 들어오는 사람들
	void					SetJoinPerson( sGUILD_PERSON_INFO* pPerson );		// 신규 길드 가입자일때
	void					SetGuildOnOff( bool bValue, TCHAR const * szTamerName, DWORD nMapID, int nChannel, int nLevel );
	void					SetTempOff( TCHAR const * szTamerName ); //임시 함수임!!
	void					SetLeavePerson( TCHAR const * szTamerName, int nType );	// 길드에서 나간 사람들
	void					SetLevelUp( TCHAR const* szTamerName, int nLevel );
	void					SetNotice( TCHAR const* szNotice, bool bChat = true );
	void					SetGuildLevel( int nLevel );
	void					SetHistory( sHISTORY_INFO* sInfo );
	void					SetClassRefresh( TCHAR const * szTamerName, cData_Guild::eRANK nClass );

	void					SetRankName( eRANK eRank, TCHAR const * szName );
	TCHAR*					GetRankName( eRANK eRank ) { return &m_szGuildRankName[ eRank ][ 0 ]; }

	void					SetRankDesc( eRANK eRank, TCHAR const * szDesc );
	TCHAR*					GetRankDesc( eRANK eRank ) { return &m_szGuildRankDesc[ eRank ][ 0 ]; }

	bool					IsOnOff() { return m_bOnOff; }
	void					SetOnOff( bool bOnOff ) { m_bOnOff = bOnOff; }

protected:
	sGUILD_PERSON_INFO*					_GetPersonInfo( TCHAR const * szName, int& nIdx );	
	void								_ChangeGuildMaster( TCHAR const * szName );
	void								_Change2ndMaster( TCHAR const * szName );
	void								_ChangeDatMember( TCHAR const * szName );
	void								_ChangeNormal( TCHAR const * szName );
	void								_ChangeNewbi( TCHAR const * szName );

protected:	
	struct sNAME : public NiMemObject
	{
		sNAME():s_fLifeTime(0)
		{}

		float		s_fLifeTime;
		std::wstring s_szName;
	};

	std::list< sNAME* >	m_listInviteName;

public:
	void					SendGuildInvite( TCHAR const * szTamerName );
	void					RecvInvite( TCHAR const * szName );
	bool					PreCheckSendGuildDestory();

};