
#pragma once 

class CTamer	:	public	CsC_PartObject
{
	THREAD_MEMPOOL_H( CTamer )
	CsCRTTI_H

public:
	enum eTAMER_STATE{
		TAMER_NORMAL, TAMER_BATTLE, TAMER_DIE,
	};
	enum eTAMER_MOVE{
		TM_NONE = 1000, TM_RIDE,
	};

public:
	CTamer();

public:
	virtual void		Delete();
	virtual void		PreDelete();
	virtual void		Init( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwSyncOption );
	virtual void		Init( uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwSyncOption );

protected:
	void				_Init( LPCTSTR szName );	

public:
	virtual void		Update( float fDeltaTime, bool bAnimation = true );
	virtual void		Render();
	virtual void		Render_PostChar();


	virtual void		CallBack_StartSequence( DWORD dwAniID );
	virtual bool		CallBack_EndofSequence( DWORD dwAniID );

	virtual void		PostLinkObject();
	void				SetEvent( int nType );	

	//====================================================================================================
	//	펫
	//====================================================================================================
protected:
	CPat*				m_pPat;
public:
	void				AddPat( uint nFileTableID, float fScale );
	void				DeletePat();
public:
	CPat*				GetPat();

	//====================================================================================================
	//	파츠
	//====================================================================================================
protected:
	sCHANGE_PART_INFO	m_OrgCostumePartInfo;
	uint				m_nOrgModelID;	
	uint				m_nPcbangCostume;

public:
	uint				GetOrgModelID();
	
	void				SetPcbangCostume( uint dwID );
	uint				GetPcbangCostume();

public:
	virtual void		ThreadLoad_Part( sCHANGE_PART_INFO* pItemCodeArray, bool bIncludeBaseKFM = true );
	virtual void		ChangePart( sCHANGE_PART_INFO* pPartInfo, bool bCompel = false/*옵션에 상관없이 강제로 파티 변경*/ );
	virtual void		DeletePart( int nPartIndex );

protected:
	virtual void		_CheckPart( sCHANGE_PART_INFO* pPartInfo );
public:
			void		_CheckPcbangCostume();

	//====================================================================================================
	//	상태
	//====================================================================================================
protected:
	DWORD				m_dwSyncOption;
	DWORD				m_dwSyncOption_OnThreadBackup;
	eTAMER_STATE		m_eTamerState;
	eTAMER_MOVE			m_eTamerMove;
	CsTamer*			m_pFT_Tamer;
#ifdef GM_CLOCKING
	bool				m_bIsClocking;
#endif
public:	
	DWORD				GetSyncOption();
	bool				IsSyncOption( DWORD dwPlag );
	void				ApplySyncOption( DWORD dwSSyncOption );

	virtual void		PickUpItemGotoInven(cItemData* pItemData);
	virtual void		ItemPickUpAnimation();

	virtual void		ItemCropSound( int nType );

	virtual void		SetBattle();
	virtual void		SetBattleEnd();	
	bool				IsBattle();

#ifdef GM_CLOCKING
	bool				IsClocking();
	void				SetClocking( bool bClocking );
#endif


	//====================================================================================
	//	Digimon Link
	//====================================================================================
protected:
	int					m_nDigimonLinkID;
public:
	void				SetDigimonLink( int nDigimonIDX );
	int					GetDigimonLink();

protected:
	void				_UpdateDigimonLink();
	
	//====================================================================================================
	//	씬
	//====================================================================================================
protected:
	eTAMER_STATE		m_eBackupTamerState;
public:
	virtual	void		Scene_Evol( CsC_AvObject* pDigimon, int nDigimonFTID );
	virtual void		SceneLevelUp( int nLevel );
	virtual void		Scene_Barcode_Connect();
	virtual void		Scene_Barcode_CardScratch_Success();
	virtual void		Scene_Barcode_CardScratch_Faile();
	virtual void		Scene_Barcode_CardScratch_OverCount();
	void				Scene_Aura( const CsC_PartObject::sCHANGE_PART_INFO* pInfo );
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
	//void				SetExpCondition( uint nExpCondition );
	//void				AddExpCondition( uint nExpCondition );
	//void				ReleaseExpCondition( uint nExpCondition );

	//====================================================================================
	//	버프
	//====================================================================================
protected:
	cBuffData		m_BuffData;

public:
	cBuffData*		GetBuff();
	void			CheckBuff();

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
public:
	virtual UINT			GetClass();
	virtual UINT			GetUniqID();
	virtual NiColor			GetNameColor();
			CCharOption*	GetCharOption();
			sCHAR_IMAGE*	GetCharImage();

	virtual TCHAR*			GetName();
	virtual bool			IsRenderName();
	virtual	float			GetToolHeight();

protected:
	virtual UINT			_GetTimeTS();

protected:
	sCHAR_IMAGE*		m_pCharImage;
	CCharOption			m_Option;

	//MASTERS_MATCHING -테이머 이름 옆 아이콘 뿌리기, 다른곳에서 쓸 수 있게 디파인은 안걸어둠 chu8820
	//=============================================================================================================
	//테이머 아이콘
	//=============================================================================================================
	DWORD				m_dwMatchTeam;		// 마스터즈매칭 팀 ( 0 = 팀없음, 1 = A팀, 2 = B팀 )
	public:
	DWORD				GetMatchTeam();
	void				SetMatchTeam( u1 nTeam );

	DWORD				m_dwTamerIcon;

	//=============================================================================================================
	//	라이딩
	//=============================================================================================================
protected:

public:
	virtual bool		SetRide();
	virtual bool		SetRideEnd();
	bool				IsRide();
};