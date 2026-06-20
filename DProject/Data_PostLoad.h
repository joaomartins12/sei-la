
#pragma once 

class cData_PostLoad : public NiMemObject
{
public:
	cData_PostLoad();
	~cData_PostLoad();
#pragma pack(push)
#pragma pack(1)
	struct sDATA
	{
		sDATA();
		~sDATA();

		void	ResetData();

#ifdef CROSSWARS_SYSTEM
		virtual void		Delete(){ s_Type.Reset(); s_dwBaseDigimonID = 0; }
		virtual void		ToObject( CsC_AvObject* pAvObject, bool bInit );
	
#else
		void		Delete(){ s_Type.Reset(); s_dwBaseDigimonID = 0; }
		void		ToObject( CsC_AvObject* pAvObject, bool bInit );
		//void		FromObject( CsC_AvObject* pAvObject );
#endif
		u4		s_SlotIdx;
		cType	s_Type;
		u1		s_nBattleTeam;

		n4		s_Attribute[ MaxExtStat ];
		u2		s_ExtendAttribute[ MaxExtendStat ];
#ifdef COMPAT_487
		u2		s_ExtendAttributeLV[MaxExtendStat - 1];
#else
		u2		s_ExtendAttributeLV[MaxExtendStat];
#endif
	
		/*
#ifdef CROSSWARS_SYSTEM
		n4		s_MCR;
		n4		s_CR;
#endif
		*/
		u2		s_nPat;
		u8		s_nExp;
		u8		s_nTranscendenceExp;
		n2		s_nLevel;

#ifdef COMPAT_487
		n4	s_nEnchantLevel;
#else
		n2	s_nEnchantLevel;
#endif
	
		float	s_fScale;
		u1		s_HatchLevel;

		TCHAR	s_szName[ Language::pLength::name + 1 ];

		// 기본 진화체 정보
		DWORD		s_dwBaseDigimonID;

		// 진화 객체 정보
		char		s_nMaxEvoUnit;
		u4			s_nNewSkillPoint;
		cEvoUnit	s_EvoUnit[nLimit::EvoUnit];
		n2			s_AttributeExp[ NewAttribute::MaxDigitalType ];
		n2			s_NatureExp[ NewAttribute::MaxNatualType ];

		u4			s_nTamerSkillCode[nLimit::Skill];			// 사용된 테이머 스킬 코드
		u4			s_nTamerSkillCoolTime[nLimit::Skill];		// 사용된 테이머 스킬의 남은 쿨타임

		u4			s_nTamerCashSkill_Index[nLimit::Skill];		// 캐쉬 스킬 인덱스 (Tamer_Skill.xls 의 인덱스 값)
		u4			s_nTamerCashSkill_LifeTime[nLimit::Skill];	// 기간 만료까지 시간
		u4			s_nTamerCashSkill_CoolTime[nLimit::Skill];	// 쿨 타임

#ifdef MEGAPHONE_BAN
		bool		s_bMegaPhoneBan;
#endif

		struct sDCashSkill
		{
			u1		s_nDigimonEvoStatus;
			u4		s_nDigimonCashSkillCode[ nLimit::MAX_ItemSkillDigimon ];
			u4		s_nSkillCoolTime[ nLimit::MAX_ItemSkillDigimon ];
		};

		sDCashSkill s_DCashSkill[nLimit::EvoUnit];
		u4			s_nUID;		// 현재는 안씀.. 작업 종료까지 사용하지 않으면 삭제 예정.
	};

#ifdef CROSSWARS_SYSTEM
	struct sTAMERDATA:public sDATA
	{
		u1		s_nCrossLoader;  // 크로스 로더 활성화
#ifdef CROSSWARS_SYSTEM
		n4		s_MCR;			// 코드크라운 MAX
		n4		s_CR;			// 코드 크라운 현재
#endif
		sTAMERDATA()  { s_nCrossLoader = 0;}

		virtual void		Delete(){ s_nCrossLoader = 0; }
		virtual void		ToObject( CsC_AvObject* pAvObject, bool bInit );
	};
#endif

#pragma pack(pop)

	struct sPostBuff
	{
		sPostBuff():s_nBuffCode(0),s_nBuffClassLevel(0),s_nBuffEndTS(0)
			,s_dwSkillCode(0)
		{};
		~sPostBuff() {};


		u4		s_nBuffCode;
		u4		s_nBuffClassLevel;
		u4		s_nBuffEndTS;	
		u4		s_dwSkillCode;
	};

public:
	void			Delete();
	void			Init();
	void			InitNetOff();
	void			Reset();
	void			PostLoad();
#ifdef ADVENTURE_TAMER_CHANGE
	void			PostLoadT();
#endif
	void			PostLoad_Tutorial();

protected:
	void			_NetOff_InitPostLoad();

protected:
	bool			m_bNetOffInitPostLoad;
	nSync::Pos		m_sPos;
	nSync::Pos		m_sBackupPos;							// 다음맵 카메라를 위치시키기 위해서
	DWORD			m_dwSyncOption;
	DWORD			m_dwServerOption;
#ifdef CROSSWARS_SYSTEM
	sTAMERDATA			m_Tamer;
#else
	sDATA			m_Tamer;
#endif
	sDATA			m_Digimon;	

	unsigned int	m_nServerRelocateCompletedTime;

public:
	void			SetPos( nSync::Pos pos ){ m_sBackupPos = m_sPos; m_sPos = pos; }
	void			SetServerRelocateCompletedTime( unsigned int const& nTime );

	nSync::Pos		GetPos(){ return m_sPos; }
	nSync::Pos		GetBackupPos(){ return m_sBackupPos; }
#ifdef CROSSWARS_SYSTEM
	sTAMERDATA*			GetTamerData(){ return &m_Tamer; }
#else
	sDATA*			GetTamerData(){ return &m_Tamer; }
#endif
	sDATA*			GetDigimonData(){ return &m_Digimon; }

	void			SetTamerData( uint const& nItemID );
	void			SetDigimonData( uint const& nFTID, uint const& nBaseDigimonID, std::wstring const& wsName );

	void			SetSyncOption( DWORD dwSyncOption ){ m_dwSyncOption = dwSyncOption; }
	void			SetServerOption( DWORD dwServerOption ){ m_dwServerOption = dwServerOption; }

protected:
	std::list< sPostBuff > m_PostTBuff;
	std::list< sPostBuff > m_PostDBuff;

public:
	void						SetTBuff( sPostBuff pBuffData ) { m_PostTBuff.push_back( pBuffData ); } 
	std::list< sPostBuff >*		GetTBuffList() { return &m_PostTBuff; }

	void						SetDBuff( sPostBuff pBuffData ) { m_PostDBuff.push_back( pBuffData ); } 
	std::list< sPostBuff >*		GetDBuffList() { return &m_PostDBuff; }

private:
	struct sPartsData
	{
		sPartsData() : s_nPartsItemID(0) {}
		~sPartsData() {}

		UINT			s_nPartsItemID;
		std::string		s_strFile;
	};
	std::map< int, sPartsData >		m_mapDParts;

public:
	void				AddPartsInfo( UINT const& nDigimonFTID, int const& nPartsItemID );
	void				ClearPartsInfo();
};