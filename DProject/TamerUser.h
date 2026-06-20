
#pragma once 

#include "UserServerMove.h"

#define TAMERUSER_ATTACK_DIST		1500.0f

class CTamerUser	:	public	CTamer
{
	CsCRTTI_H

public:
	enum eTAMERUSER_STATE{
		TU_NONE, TU_SKILL, TU_WAIT_REV_SKILL, TU_USE_TARGET_ITEM, TU_TARGET_RIDE_END,
#ifdef KEYBOARD_MOVE
		TU_KEYBOARD_MOVE , TU_FOLLOW_DIGIMON,
#endif
	};
	enum eFATIGUE_TALK_STATE{
		FTS_NONE, FTS_DECREASE, FTS_INCREASE_0, FTS_INCREASE_40, FTS_INCREASE_80, FTS_FATIGUE,
	};
	enum eRIDE_END_STATE{
		RES_NONE, RES_STOP, RES_ATTACK, RES_SKILL,
	};

public:
	CTamerUser();
public:
	virtual void		Delete();
	virtual void		PreDelete();
	virtual void		Init( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwServerOption );
	virtual void		Init( uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwServerOption );

	eCHECK_TYPE			CheckModel();

protected:
	void				_Init();
	void				_NetOffInit();

public:
	virtual void		Update( float fDeltaTime, bool bAnimation = true );
	virtual void		Render();

protected:
	void				_UpdateAttack( float fDeltaTime );
	virtual void		_UpdatePortal(const float& fDeltaTime);
	void				_UpdateRegion();	
	void				_UpdateFatiguTalk();
	void				_UpdateWeather();

protected:
	bool isEnablePortal(void);
	const DWORD GetNearPortalID(const float& fDeltaTime) const;

	DWORD			m_dwLastNearPortalID;

protected:	
	CSound*				m_pRainSound;

public:

	void				PlayBGM() { g_pSoundMgr->PlayMusic( m_cBGSound.c_str() ); }

	//====================================================================================================
	//	파트
	//====================================================================================================
public:
	virtual void		ThreadLoad_Part( sCHANGE_PART_INFO* pItemCodeArray, bool bIncludeBaseKFM = true );
	virtual void		ChangePart( sCHANGE_PART_INFO* pPartInfo );	

	/**< 선행로드 인가? */
	virtual bool		_IsFastPartLoad() { return true; }


	//====================================================================================================
	//	서버상 옵션
	//====================================================================================================
public:
	enum eSERVER_OPTION
	{
		SO_DO_TUTORIAL		=	0x00000001,
	};

protected:
	DWORD				m_dwServerOption;
public:
#ifdef THAI_TUTORIAL	// 신규 캐릭 재접속 시 튜토리얼 진행 안되는 것 관련 코드 수정 12.10.22 lks007
	DWORD				GetServerOption(){ return m_dwServerOption; }
	bool				IsServerOption() { return ( ( m_dwServerOption == 1 ) ? true : false); }
	void				SetServerOption( DWORD dwServerOption ){ m_dwServerOption = dwServerOption; }
	void				AddServerOption( DWORD dwPlag ){ m_dwServerOption = dwPlag; }
	void				RemoveServerOption( DWORD dwPlag ){ m_dwServerOption = dwPlag; }
#else
	DWORD				GetServerOption(){ return m_dwServerOption; }
	bool				IsServerOption( DWORD dwPlag ){ return ( ( m_dwServerOption & dwPlag ) != 0 ); }
	void				SetServerOption( DWORD dwServerOption ){ m_dwServerOption = dwServerOption; }
	void				AddServerOption( DWORD dwPlag ){ m_dwServerOption |= dwPlag; }
	void				RemoveServerOption( DWORD dwPlag ){ m_dwServerOption &= ~dwPlag; }
#endif

	//====================================================================================================
	//	상태
	//====================================================================================================
protected:
	CsOnceTimeSeq		m_SkillRevTimeSeq;						// 스킬 받아야 하는데 받지 못한상태로 3초 지나면 강제로 풀어주자
	eTAMERUSER_STATE	m_eTamerUserState;
	eFATIGUE_TALK_STATE	m_eFatigueTalkState;
	int					m_nOldFatiguePoint;
	CsTimeSeq			m_TimeSeqFatigueTalk;
	union
	{
		int		m_nStatValue_ItemFTID;
	};

public:
	virtual void		ItemPickUpAnimation();
	virtual void		PickUpItemGotoInven(cItemData* pItemData);

	virtual void		RaidItemCropSuccess( cItemData* pItemData, int nTalkID = 11023 );
	virtual void		ItemCropSound( int nType );
	void				ItemCropFailed();
	void				MoneyCrop( int nMoney, int TamerUID, int nGuildAddMoney = 0, int nBuffAddMoney = 0 );
	

	virtual void		SetBattle();
	virtual void		SetBattleEnd();

	void				SetTamerUserState( eTAMERUSER_STATE tuState, void* pValue = NULL );
	void				SetStateValue( eTAMERUSER_STATE tuState, int nValue );

	void				ResetOldFatiguePoint(){ m_nOldFatiguePoint = GetBaseStat()->GetFP(); }
	int					GetOldFatiguePoint(){ return m_nOldFatiguePoint; }
protected:
	bool				_IsApplyTUState();

	//====================================================================================================
	//	씬
	//====================================================================================================
public:
	virtual	void		Scene_Evol( CsC_AvObject* pDigimon, int nDigimonFTID );
	virtual void		SceneLevelUp( int nLevel );
	virtual void		Scene_Barcode_Connect();
	virtual void		Scene_Barcode_CardScratch_Success();
	virtual void		Scene_Barcode_CardScratch_Faile();
	virtual void		Scene_Barcode_CardScratch_OverCount();

	//====================================================================================
	//		Path
	//====================================================================================
public:
	virtual void		DeletePath();
	virtual void		ArriveTarget();

	//====================================================================================
	//		Rotation
	//====================================================================================
public:
	virtual void		SetRotation_AniDir( NiPoint3 vDestDir );

	//====================================================================================
	//		Skill
	//====================================================================================
protected:
	bool				m_bServerSendSkill;
	int					m_nActiveIDX;	// Tamer_Skill.xls 의 스킬 인덱스 번호

public:
	void				UseSkill( int nSkillIndex );
	virtual bool		SendSkill( int nSkillIndex, CsC_AvObject* pTarget );
	virtual bool		RecvSkill( bool bSuccess, int nSkillIndex );
	void				ReleaseSrvSendSkill(){ m_bServerSendSkill = false; m_SkillRevTimeSeq.OffEnable(); }
	void				SetActiveIDX( int nIDX ) { m_nActiveIDX = nIDX; }

	//====================================================================================
	//		CallBack
	//====================================================================================
public:
	virtual void		CallBack_StartSequence( DWORD dwAniID );
	virtual bool		CallBack_EndofSequence( DWORD dwAniID );

	//====================================================================================
	//		스탯
	//====================================================================================
public:
	struct sUSER_STAT : public CsC_AvObject::sBASE_STAT
	{
	public:
		sUSER_STAT( CsC_AvObject* pParent );
		~sUSER_STAT();

		virtual CsC_AvObject::eSTAT	GetType() const;

	protected:
		int				s_nDSBackup;

		int				s_nMaxHP;
		int				s_nMaxDS;
		int				s_nHP;
		int				s_nDS;
		int				s_nFP;
		__int64			s_dwExp;
		int				s_nAtt;
		int				s_nDef;
#ifdef CROSSWARS_SYSTEM
		int				s_nCR;
		int				s_nMaxCR;
#endif

	public:
		int		GetMaxHP() const;
		int		GetMaxDS() const;
		int		GetHP() const;
		int		GetDS() const;
		int		GetFP() const;

		float	GetHPRate() const;
		float	GetDSRate() const;
		float	GetFPRate() const;
		
		void	SetMaxHP( int nMaxHP );
		void	SetMaxDS( int nMaxDS );
		void	SetDeltaMaxHP( int nDelta );

		void	SetHP( int nHP );
		void	SetDS( int nDS );
		void	SetFP( int nFP );
		
		void	SetDeltaFP( int nDelta );

		void	SetDeltaHP( int nDelta );
		void	SetDeltaBackupHP( int nDelta );

		void	SetDeltaDS( int nDelta );
		void	SetDeltaBackupDS( int nDelta );
		__int64	GetExp() const;
		void	SetExp( __int64 dwExp );
		int				GetAtt() const;
		int				GetDef() const;
#ifdef CROSSWARS_SYSTEM
		int				GetCR() const;
		int				GetMaxCR() const;
		float			GetCRRate() const;
#endif

		void			SetAtt( int nValue );
		void			SetDef( int nValue );
#ifdef CROSSWARS_SYSTEM
		void			SetMaxCR( int nValue );
		void			SetCR( int nValue );
#endif
#ifdef SDM_TAMER_XGUAGE_20180628
public:
		void			SetMaxCrystalXGuage(unsigned short const& maxC, unsigned int const& maxGuage );

		void			SetMaxCrystal(unsigned short const& maxC );
		unsigned short  GetMaxCrystal() const;
		void			SetCurrentCrystal( unsigned short const& currentC);
		unsigned short  GetCurrentCrystal() const;
		
		void			SetMaxXGuage(unsigned int const& maxGuage);
		unsigned int	GetMaxXGuage() const;
		void			SetCurrentXGuage(unsigned int const& currentGuage);
		unsigned int	GetCurrentXGuage() const;
		//bool			IsXGToCrystal() const;
		//bool			IsCrystalToXG() const;
		
private:
		unsigned short	s_usMaxCrystal;
		unsigned short	s_usCurrentCrystal;
		unsigned int	s_uiMaxGuage;
		unsigned int	s_uiCurrentGuage;
#endif
	};	

	void				SetExp( __int64 dwExp );

	//====================================================================================================
	//	부가 정보
	//====================================================================================================
protected:	
	cResist::sCHAR_QUICKSLOT::sCHAR*	m_pQuickSlot;
	NiBoundingVolume*					m_pBoundingVolume;
public:
	virtual NiColor		GetNameColor(){ return DEFAULT_NAMECOLOR; }
	cResist::sCHAR_QUICKSLOT::sCHAR*	GetQuickSlot(){ return m_pQuickSlot; }

	virtual bool		IsRenderName(){ return g_pResist->m_Global.s_bMyRenderName && IsEnableObject(); }

	// 현재는 음성 플레이 밖에 쓰이는데가 없다
	virtual bool		IsUser(){ return g_pResist->m_Global.s_bEnableVoice; }

	//====================================================================================================
	//	지역 정보
	//====================================================================================================
protected:
	bool				m_bFirstRegion;
	std::wstring		m_szMapDiscript;
	std::string			m_cBGSound;
	std::wstring		m_szMainMapDiscript;
	int					m_nRegionIndex;
	int					m_nCurPosRegionIndex;

public:
	int					GetCurPosRegionIndex(){ return m_nCurPosRegionIndex; }

	std::wstring		GetMapDiscript() { return m_szMapDiscript; }

	//=============================================================================================================
	//	이동
	//=============================================================================================================
protected:
	CUserServerMove		m_UserServerMove;
	CsC_AvObject*		m_pArriveTarget;
	CsC_AvObject*		m_pArriveTargetItem;

#ifdef DEBUFF_ITEM
	int					m_nDebuffInvenIndex;
	float				m_fDebuffDist;
	NiPoint3			m_vDebuffPos;
	CsC_AvObject*		m_pDebuffTarget;
#endif

public:
	CUserServerMove*	GetUserServerMove(){ return &m_UserServerMove; }
	CsC_AvObject*		GetArriveTargetItem(){ return m_pArriveTargetItem; }
	void				SetArriveTarget( CsC_AvObject* pArriveTarget ){ m_pArriveTarget = pArriveTarget; }
	void				ReleaseArriveTarget(){ m_pArriveTarget = NULL; }
	virtual void		SetServerMoveReady(){ m_UserServerMove.SetServerMoveReady(); }

public:
	void		SetTargetPos( NiPoint3 vTargetPos, float fConnectLength, bool bRenderMoveModel, bool bTamerAttack );
	void		SetAttackPos( CsC_AvObject* pAttackSrc );

#ifdef DEBUFF_ITEM
	void		SetDebuffPos( CsC_AvObject* pDebuffSrc, float fDebuffDist, int nInvenIndex );
	void		_UpdateDebuff();
#endif

	void		DoAttackSkillPos();

#ifdef KEYBOARD_MOVE
protected:
	void				_MoveFollowDigimon();
	void				_MoveFollowDigimonPortal();
	void				_MoveStop_ByArriveDist();
	NiPoint2			_GetArrivePos_ByDigimon();
	void				DoPos_ByDigimon();
	bool				m_bFollowPortalMove;		// 디지몬 이동 시 디지몬이 포탈에 부딪히면 테이머가 디지몬 위치로 이동해야함

public:
	eTAMERUSER_STATE	GetTamerUserState()					{ return m_eTamerUserState; }
	bool				IsFollowPortalMove()				{ return m_bFollowPortalMove; }
	void				SetFollowPortalMove( bool bIsMove )	{ m_bFollowPortalMove = bIsMove; }
	bool				_IsDoMove();

#endif
	//=============================================================================================================
	//	라이딩
	//=============================================================================================================
public:
	struct sRIDE_TARGET_INFO
	{
		int		s_nValue1;
		float	s_fValue1;
		bool	s_bValue1;
	};

protected:
	eRIDE_END_STATE		m_eRideEndState;
	CsC_AvObject*		m_pRideTarget;

	sRIDE_TARGET_INFO	m_RideTargetInfo;

public:
	void				SetRideEndState( eRIDE_END_STATE res, CsC_AvObject* pTarget, sRIDE_TARGET_INFO* pTargetInfo );

	virtual bool		SetRide();
	virtual bool		SetRideEnd();

	void				SetServerRelocateCompletedTime( unsigned int const& nLimitTIme );
	unsigned int		GetServerRelocateCompletedTime() const;
	bool				IsEnableServerRelocate() const;	// 서버 이전을 할 수 있는 제한 시간이 지났는지 체크

private:
	unsigned int		m_nSereverRelocateCompletedTime;
//=============================================================================
// 확성기 ban
//=============================================================================
#ifdef MEGAPHONE_BAN
protected:
	bool				m_bMegaPhoneBan;	// 확성기 차단 기능
public:
	void				SetMegaPhoneBan( bool bBan ) { m_bMegaPhoneBan = bBan; }
	bool				GetMegaPhoneBan() { return m_bMegaPhoneBan; }
#endif

public:
	void				ResetSkillCool();

};