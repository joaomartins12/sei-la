
#pragma once 


class CDigimon : public CsC_DPartObject
{
	THREAD_MEMPOOL_H( CDigimon )
	CsCRTTI_H

public:
	CDigimon();

	enum eDIGIMON_STATE{
		DIGIMON_NORMAL, DIGIMON_BATTLE, DIGIMON_EVOL,		
		DIGIMON_EVOL_JOINTPROGRESS,
		DIGIMON_DEGENERATE, 
#ifdef CROSSWARS_SYSTEM
		DIGIMON_CROSSCHANGE_PARTNER_START,
		DIGIMON_CROSSCHANGE_PARTNER_END,

		DIGIMON_CROSSCHANGE_PARTNER2_START,
		DIGIMON_CROSSCHANGE_PARTNER2_END,

		DIGIMON_CROSSRECOVER_START,
		DIGIMON_CROSSRECOVER_END,
#endif
		DIGIMON_CHANGE_PARTNER_START, DIGIMON_CHANGE_PARTNER_END, DIGIMON_DIE,
	};

	enum eDIGIMON_MOVE{
		DM_NONE = 1000, DM_RIDE,
	};

public:
	virtual void		Delete();
	virtual void		PreDelete();
	virtual void		ChangeKFM( UINT nFTID );
#ifdef CROSSWARS_SYSTEM
	virtual void		ChangeCrossKFM( UINT nFTID, bool bUser = true );
	virtual void		ChangeCrossModel( UINT nFTID, LPCTSTR szName, bool bUser = true );
#endif
	virtual void		ChangeModel( UINT nFTID, LPCTSTR szName );
			bool		ChangeRideKFM( UINT nRideID, bool bDoRide );

	virtual void		Init( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp );
	virtual void		Init( uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp );

	virtual eCHECK_TYPE	CheckModel();

	void				CheckLevelEffect();
	void				CheckEnchantEffect();
	void				CheckSizeEffect();
	void				CheckBattleEffect();
	void				CheckingXAntiEffect();
	void				SetEvent( int nType );
private:
	//X Anti Effect Anchor Name
	std::string			mToeName;


protected:
	void				_Init( LPCTSTR szName );	

public:
	virtual void		Update( float fDeltaTime, bool bAnimation = true );	
	virtual void		Render();
	virtual void		Render_PostChar();

protected:
    virtual void		_Update_EffectPos();
	void				_Update_Region();
	virtual void		SetReservationSkillCoolTime(uint nFileTableID) {};

	//====================================================================================================
	//	상태
	//====================================================================================================
protected:
	eDIGIMON_STATE		m_eDigimonState;
	eDIGIMON_MOVE		m_eDigimonMove;
	CsDigimon*			m_pFTDigimon;	
#ifdef GM_CLOCKING
	bool				m_bIsClocking;
#endif

public:
	void				SetDigimonState( eDIGIMON_STATE state );
	eDIGIMON_STATE		GetDigimonState();
	CsDigimon*			GetFTDigimon();
	virtual DWORD		GetBaseDigimonFTID() const;
	virtual void		SetBattle();
	virtual void		SetBattleEnd();
	virtual void		SetDie();	
	
	bool				IsBattle();
	bool				IsDie();
	bool				IsEvolDegener();
	virtual void		SetMarble( DWORD mt, CsC_AvObject* pFollow, int nCnt );
#ifdef GM_CLOCKING
	bool				IsClocking();
	void				SetClocking( bool bClocking );
#endif

protected:
	void				_ResetFTDigimon( uint nFileTableID );

	// HP/DS 회복스킬 (버프스킬 발동 체크)_
	bool				m_bSkillFlag;	// 버프 스킬 발동 여부 체크
public:
	bool				GetBuffSkillFlag();
	void				SetBuffSkillFlag( bool bSuccess );

	//====================================================================================
	//	Tamer Link
	//====================================================================================
protected:
	int					m_nTamerLinkID;
public:
	void				SetTamerLink( int nTamerIDX );
	int					GetTamerLink();

	//====================================================================================================
	//	씬
	//====================================================================================================
protected:
	UINT				m_nNextFTID;
	TCHAR				m_szNextName[ Language::pLength::name + 1 ];
	float				m_fEvolTime;
	float				m_fNextScale;
	int					m_nNextLevel;
	int					m_nEvolStep;	
	eDIGIMON_STATE		m_eBackupDigimonState;

	struct sRESERVATION : public NiMemObject
	{
		enum eTYPE{ RV_Evol, RV_Degenerate, RV_ChangePartner, RV_Ride };
		eTYPE	s_eType;
		int		s_nIDX;
		int		s_nFTID;
		float	s_fScale;
		int		s_nLevel;
		int		s_nTacticsIDX;
		int		s_nBattleTeam;
		TCHAR	s_szNextName[ Language::pLength::name + 1 ];
		DWORD	s_dwChangeApplyModelID;
	};	
	std::queue< sRESERVATION* >		m_ReservationStack;

public:
	int					GetEvoStep() { return m_nEvolStep + 1; }	// Evo 엑셀 시트와 진화 숫자를 맞추기 위해 +1을 해 줌

protected:
	virtual bool		_UpdateEvol_JointProgressScene( float fDeltaTime );
	virtual bool		_UpdateEvolScene( float fDeltaTime );
	virtual bool		_UpdateDegenerateScene( float fDeltaTime );
	virtual bool		_UpdateChangePartnerStartScene( float fDeltaTime );
	virtual bool		_UpdateChangePartnerEndScene( float fDeltaTime );
#ifdef CROSSWARS_SYSTEM
	virtual bool		_UpdateCrossChangePartnerStartScene( float fDeltaTime );
	virtual bool		_UpdateCrossChangePartnerEndScene( float fDeltaTime );

	virtual bool		_UpdateCrossChangePartnerStartScene2( float fDeltaTime );
	virtual bool		_UpdateCrossChangePartnerEndScene2( float fDeltaTime );

	virtual bool		_UpdateCrossRecoverStartScene( float fDeltaTime );
	virtual bool		_UpdateCrossRecoverEndScene( float fDeltaTime );

	virtual void		WithoutUpdate_CrossChange2Partner(){}
#endif
public:
	virtual void		SceneLevelUp( int nLevel );
	virtual bool		Scene_Evol( UINT nNextFTID, bool bAbsoluteEvolution );
	virtual bool		Scene_Degenerate( UINT nNextFTID );
	virtual bool		Scene_ChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );
#ifdef CROSSWARS_SYSTEM
	virtual bool		Scene_CrossRecoverPartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );

	virtual bool		Scene_CrossChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );
	virtual bool		Scene_CrossChangePartner2( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );
	virtual void		WithoutScene_CrossChangePartner2( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel ) {}
#endif
	virtual bool		WithoutScene_ChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );
	virtual bool		_WithoutUpdate_ChangePartner();	
	virtual UINT		GetNextFTID();
	virtual TCHAR*		GetNextName();

	//====================================================================================================
	//	Path
	//====================================================================================================
public:
	virtual void		DeletePath();	

	//====================================================================================================
	//	Property
	//====================================================================================================
protected:
	virtual void		_SetDefault_IdleAnimation_Stand();
	virtual void		_SetDefault_IdleAnimation_Move();			

	//====================================================================================
	//	스킬
	//====================================================================================
protected:
	cSkill				m_SkillMng;
public:
	cSkill*				GetSkillMng();

	//====================================================================================
	//	컨디션
	//====================================================================================
protected:
	cCondition			m_Condition;
public:
	cCondition*			GetCondition();
	void				SetCondition( uint nCondition );

	//====================================================================================
	//	버프
	//====================================================================================
protected:
	cBuffData		m_BuffData;

public:
	cBuffData*		GetBuff();

	//====================================================================================
	//	스탯	- 항상 동적으로 구하겠다 - 공식을 변경하고 싶으면 이부분을 바꾸라
	//====================================================================================
public:
	virtual float			GetAttackRange();
	virtual float			GetAttackCharSize();
	//virtual float			GetAttackSpeed();

	//====================================================================================
	//		Sound
	//====================================================================================
public:
	virtual char*		GetSoundDir();
	virtual void*		PlaySound( char* cSoundPath );
	virtual void*		PlayStaticSound( char* cSoundPath );

	//====================================================================================================
	//	부가 정보
	//====================================================================================================
protected:
	int					m_nBattleTeam;
public:
	void				SetBattleTeam( int nTeam );
	int					GetBattleTeam();

public:
	virtual UINT			GetClass();
	virtual UINT			GetUniqID();
	virtual NiColor			GetNameColor();
			CCharOption*	GetCharOption();
			sCHAR_IMAGE*	GetCharImage();
			NiPoint3		GetRidePos();

	virtual	float			GetToolHeight();

	virtual UINT		GetModelID() const;
	virtual int			GetAttackRangeType();

	virtual TCHAR*		GetName();
	virtual bool		IsRenderName();

	// 애니메이션 한 루프당 이동 가능한 거리
	virtual float		_GetAniTimeMoveDist_Walk();
	virtual float		_GetAniTimeMoveDist_Run();
	virtual float		_GetAniTimeMoveDist_ARun();

	int					GetEvoType();	// 디지몬 진화 타입

	bool				IsSameEvoultionType( nsCsDigimonTable::eEVOLUTION_TYPE eEvol ) const;
	//=============================================================================================================
	//	강화 수치
	//=============================================================================================================
public:
	struct sEnchantInfo
	{
		sEnchantInfo():s_nLv(0),s_nValue(0) {}
		~sEnchantInfo() {};

		int  s_nLv;
		int  s_nValue;  
	};

	struct sENCHENT_STAT: public NiMemObject
	{
	public:
		sENCHENT_STAT();
		~sENCHENT_STAT();

	public:
		void    GetEnchantLV_Info( int& nMaxLv, int& nMaxLvCnt ) const;
		void    GetEnchantLV_Info( int& nMaxLv, int& nOldMaxLvCnt, int& nCurMaxLvCnt ) const;
		void    SetEnchantLevel( int nLevel );
		/**< 강화 total레벨 반환 */
		int     GetEnchantLevel() const;

		void    SetEnchantInfo( int const& nType, int const& nLv, int const& nValue );

		int     GetEnchantLv( int const& nType ) const;
		int     GetEnchantValue( int const& nType ) const;
		sEnchantInfo const* GetEnchantInfo( int const& nType ) const;

	protected:
		int        s_nEnchantLevel;
		std::map<int,sEnchantInfo>  s_mapInfos;
	};
// 	struct sENCHENT_STAT: public NiMemObject
// 	{
// 	public:
// 		sENCHENT_STAT();
// 	protected:
// 		int				s_nEnchantLevel;
// 		/**< 강화 total레벨 */
// 		
// 		// 강화 레벨
// 		int				s_nEnchant_Att_LV;  // 공격력 강화레벨
// 		int				s_nEnchant_Def_LV;  // 블락률 강화레벨
// 		int				s_nEnchant_Critical_LV;  // 크리티컬 강화레벨
// 		int				s_nEnchant_Evade_LV;     // 회피 강화레벨
// 		int				s_nEnchant_Hp_Lv;		// HP 강화 레벨
// 
// 		// 강화 수치
// 		int				s_nEnchant_Att;  // 공격력 강화량
// 		int				s_nEnchant_Def;  // 블락률 강화량
// 		int				s_nEnchant_Critical;  // 크리티컬 강화량		
// 		int				s_nEnchant_Evade;     // 회피 강화량
// 		int				s_nEnchant_AttSpeed;  // 공속 강화량 (사용 안함)
// 		int				s_nEnchant_HitRate;   // 적중 강화량 (사용 안함)
// 		int				s_nEnchant_Hp;		// HP 강화량
// 
// 	public:
// 		void			GetEnchantLV_Info( int& nMaxLv, int& nMaxLvCnt );
// 		void			GetEnchantLV_Info( int& nMaxLv, int& nOldMaxLvCnt, int& nCurMaxLvCnt );
// 		void			SetEnchantLevel( int nLevel );
// 		/**< 강화 total레벨 반환 */
// 		int				GetEnchantLevel();
// 		/**< 강화 레벨 반환 */
// 		int				GetAtt_Enchant();
// 		int				GetBlock_Enchant();
// 		int				GetCritical_Enchant();
// 		int				GetAttSpeed_Enchant();
// 		int				GetEvade_Enchant();
// 		int				GetHitRate_Enchant();
// 		int				GetEnchantValue( ET_HP );
// 
// 		void			SetAtt_Enchant( int nValue );
// 		void			SetBlock_Enchant( int nValue );
// 		void			SetCritical_Enchant( int nValue );
// 		void			SetAttSpeed_Enchant( int nValue );
// 		void			SetEvade_Enchant( int nValue );
// 		void			SetHitRate_Enchant( int nValue );
// 		void			SetHP_Enchant(int nValue);
// 
// 		int				GetAtt_Enchant_LV();
// 		int				GetBlock_Enchant_LV();
// 		int				GetCritical_Enchant_LV();
// 		int				GetEvade_Enchant_LV();
// 		int				GetHP_Enchant_LV();
// 
// 		void			SetAtt_Enchant_LV( int nValue );
// 		void			SetBlock_Enchant_LV( int nValue );
// 		void			SetCritical_Enchant_LV( int nValue );
// 		void			SetEvade_Enchant_LV( int nValue );
// 		void			SetHP_Enchant_LV( int nValue );
// 	};

	sENCHENT_STAT*		GetEnchantStat();

protected:
	sENCHENT_STAT*		m_pEnchentStat;

protected:
	sCHAR_IMAGE*		m_pCharImage;
	CCharOption			m_Option;


	//=============================================================================================================
	//	라이딩
	//=============================================================================================================
protected:
	CsRide*				m_pFTRide;

protected:
	void				_SetRideRot();

public:
	virtual bool		SetRide();
	virtual bool		SetRideEnd();
	bool				IsRide();
	CsRide*				GetFTRide();

protected:
		CDigimon*			m_pCrossTempDigimon;

	//=============================================================================================================
	//	디지몬 파츠 추가
	//=============================================================================================================
private:
	std::list<nsDPART::sDPART_INFO>		m_PostParts;

public:
	void				SetPostDParts( UINT const& nPartsItemType );
	void				ClearAllPostDParts();
};