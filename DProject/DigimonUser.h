
#pragma once 

#include "UserServerMove.h"

class CDigimonUser : public CDigimon
{
	CsCRTTI_H

public:
enum eDIGIMONUSER_STATE{
		DU_WAIT, DU_FOLLOW, DU_ATTACK, DU_SKILL, DU_WAIT_REV_SKILL, DU_NPCFIND, DU_MANUAL_CONTROL, 
#ifdef KEYBOARD_MOVE
		DU_KEYBOARD_MOVE,
#endif
	};

public:
	CDigimonUser();
	~CDigimonUser();

public:
	virtual void		Delete();
	virtual void		PreDelete();
	virtual void		ChangeKFM( UINT nFTID );
#ifdef CROSSWARS_SYSTEM
	virtual void		ChangeKFM( UINT nFTID, bool bUser );
	virtual void		ChangeCrossModel( UINT nFTID, LPCTSTR szName, bool bUser );
#endif
	virtual void		ChangeModel( UINT nFTID, LPCTSTR szName );
	virtual void		Init( uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, int nUserIndex, bool bInitPos );
	virtual void		Init( uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, int nUserIndex, bool bInitPos );

protected:
	void				_Init( bool bCreate );
	void				_NetOffInit();

	void				_InitStartPos();
	void				_ResetBattleEvo( bool bCreate );

public:
	virtual void		Update( float fDeltaTime, bool bAnimation = true );
	virtual void		Render();

protected:
	void				_Update_Battle( float fDeltaTime );

	void				_UpdateAttack_Game( float fDeltaTime );
	void				_UpdateAttack_Battle( float fDeltaTime );
	void				_UpdateAttack_ClientMode( float fDeltaTime );
	void				_UpdateMove();
	void				_UpdateNpcFind();
	void				_UpdateTamerDist();
#ifdef KEYBOARD_MOVE
	void				_UpdatePortal(const float& fDeltaTime);
#endif
	void				_Update_DigimonTalk();



private:
	bool				isEnablePortal(void);
	const DWORD	GetNearPortalID(const float& fDeltaTime) const;
	DWORD			m_dwLastNearPortalID;


	//====================================================================================================
	//	Path
	//====================================================================================================
public:
	virtual void		DeletePath();
	virtual void		ArriveTarget();
	void				SetTargetPos( NiPoint3 vTargetPos, float fConnectLength, bool bRenderMoveModel );

	//====================================================================================
	//		Skill
	//====================================================================================
public:
			void		UseSkill( int nSkillIndex );
	virtual bool		SendSkill( int nSkillIndex, CsC_AvObject* pTarget );
	virtual bool		RecvSkill( bool bSuccess, int nSkillIndex );
			void		ReleaseSrvSendSkill(){ m_SkillRevTimeSeq.OffEnable(); }

	//====================================================================================
	//		CallBack
	//====================================================================================
public:
	virtual void		CallBack_StartSequence( DWORD dwAniID );
	virtual bool		CallBack_EndofSequence( DWORD dwAniID );

	//====================================================================================================
	//		Property
	//====================================================================================================
protected:
	virtual void		_SetDefault_IdleAnimation_Move();


	//====================================================================================================
	//	상태
	//====================================================================================================
protected:
	enum eNPC_FIND_STATE{ NFS_DOING, NFS_WAIT, NFS_COMPLEATE, };
protected:
	CsOnceTimeSeq		m_SkillRevTimeSeq;						// 스킬 받아야 하는데 받지 못한상태로 3초 지나면 강제로 풀어주자
	int					m_nDigimonUserIndex;
	eDIGIMONUSER_STATE	m_eDigimonUserState;
	eDIGIMONUSER_STATE	m_eDigimonUserState_NoneSkill;			// 스킬 상태가 아닌 스테이트

	bool				m_bWarningHP;
	bool				m_bWarningDS;
	CsTimeSeq			m_IdleCheckTimeSeq;
	float				m_fClientAttackTime;

	CsC_AvObject*		m_pFindTarget;
	CsTimeSeq			m_FindCheckTimeSeq;
	int					m_eFindState;

public:
	int					GetDigimonUserIndex(){ return m_nDigimonUserIndex; }

	virtual void		SetBattle();
	virtual void		SetBattleEnd();
	virtual void		SetDie();
	virtual void		SetServerDie( bool bDie );
	void				ReleaseFollowTarget( CsC_AvObject* pDeleteTarget );
	void				SetDigimonUserState( eDIGIMONUSER_STATE duState, void* pValue = NULL );
	eDIGIMONUSER_STATE	GetDigimonUserState(){ return m_eDigimonUserState; }

	bool				IsOpenedEvoSlot( unsigned int const& nSlotNum );
protected:
	void				_SetDigimonUserState_Game( eDIGIMONUSER_STATE duState, void* pValue );
	void				_SetDigimonUserState_Battle( eDIGIMONUSER_STATE duState, void* pValue );
	bool				_IsApplyDUState( eDIGIMONUSER_STATE duState );

	//====================================================================================================
	//	씬
	//====================================================================================================
protected:
	int					m_nTacticsIndex;

protected:
	virtual bool		_UpdateEvol_JointProgressScene ( float fDeltaTime );
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

	virtual void		WithoutUpdate_CrossChange2Partner();
#endif

	virtual void		_CheckAttackDigimonTalk();
	void				_DecreaseChipset(USHORT const& nChipsetType, USHORT const& nChipsetTypeC, int const& nChipsetCount);

public:
	virtual void		SceneAttackPropEvent( CsC_AttackProp::sEVENT_INFO* pEvent );
	
	virtual bool		Scene_Evol( UINT nNextFTID, bool bAbsoluteEvolution );
	virtual bool		Scene_Degenerate( UINT nNextFTID );
	virtual bool		Scene_ChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );
#ifdef CROSSWARS_SYSTEM
	virtual bool		Scene_CrossRecoverPartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );

	virtual bool		Scene_CrossChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );
	virtual bool		Scene_CrossChangePartner2( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );
	virtual void		WithoutScene_CrossChangePartner2( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );

#endif
	virtual bool		WithoutScene_ChangePartner( UINT nNextFTID, int nTacticsIndex, TCHAR* szNextName, float fNextScale, int nNextLevel );
	virtual bool		_WithoutUpdate_ChangePartner();	
	virtual void		SceneLevelUp( int nLevel );
	virtual void		SceneSkillUp( int nSkillLevel, int nSkillPoint, UINT nSkillExp, UINT nEvoIndex );

	//====================================================================================================
	//	진화
	//====================================================================================================
public:
	enum eEvoState		
	{
		eNone			=	0x00000000, 
		eActive			=	0x00000001,	/*진화창 완전오픈상태*/ 				
		eInActive		=	0x00000002,	/*진화창 보여는 주는상태*/
		eClosed			=	0x00000004,

		eActive_Mask	=	0x0000000f,

		eRide			=	0x00000100,
	};	

	enum eEvoTree// CAPSULE_EVOLUTION - 진화 트리 타입 추가  chu8820
	{
		TREE_COMMON		= 0,	// 공용 트리 ( 분기 이전 )
		TREE_NORMAL		= 1,	// 분기 - 일반 트리 ( 조그레스, 캡슐진화 아닌 트리 )
		TREE_JOGRESS	= 2,	// 분기 - 조그레스
		TREE_CAPSULE	= 3,	// 분기 - 캡슐진화
		TREE_CROSS		= 4,	// 분기 - 크로스워즈
	};

	void				AddEvoPlag( int nSlot, eEvoState es );
	void				SetEvoPlag( bool bInit, int nStartSlot, int nCount );

	//XAI SYSTEM 장/탈착시 X 항체 디지몬들의 슬롯창의 Flag 를 갱신합니다
	void				UpdateXEvoFlag();

	bool				IsEvoPlag( eEvoState es ){ return ( m_EvoStateArray[ GetFTEvolCurObj()->m_nEvoSlot ] & es ) != 0; }
	bool				IsEvoPlag( int nSlot, eEvoState es ){ return ( m_EvoStateArray[ nSlot ] & es ) != 0; }

	DWORD				GetEvoValue( int nSlot, eEvoState mask );

protected:
	DWORD				m_EvoStateArray[ nLimit::EvoUnit ];	//	모든 슬롯들의 오픈 상태값 클라이언트에 저장 진화 이미지 표시할때 쓰인다.

protected:
	DWORD				m_dwPrevBaseDigimonID;
	DWORD				m_dwBaseDigimonID;
	int					m_nMaxEvoUnit;
	cEvoUnit			m_EvoUnit[ nLimit::EvoUnit ];	// 서버에서 받는 슬롯 정보

	// 진화 트리 합체진화 시스템과 공유 chu8820
	int					m_nEvoTree;					// 현재 진화 트리. 0: 일반 트리, 1: 분기 일반 트리, 2: 분기 조그레스 트리, 3: 분기 캡슐진화 트리
	DWORD				m_nJointProgressFlag;	// /* m_nJointProgressFlag 변수가 살아있는 시간 조그레스 진화 해서 성장기로 퇴화 할때 까지*/

public:
	// 진화 트리 합체진화 시스템과 공유 chu8820
	void				SetEvoTree( int nEvoTree ){	m_nEvoTree = nEvoTree; }
	int					GetEvoTree(){return m_nEvoTree;}
	bool				IsCapsuleTree( int nTree ){ if( nTree == TREE_CAPSULE ) return true; else return false; }

	void				CompQuestJointProgress( DWORD dwOpenQuestID );
	bool				IsJogressTree( int nTree ){ if((nTree == TREE_JOGRESS)||(nTree == TREE_NORMAL)) return true; else return false; }

	void				SetJointProgressFlag( DWORD dwSlotFlag){ m_nJointProgressFlag = dwSlotFlag;}	// 슬롯 값으로 조그레스 플래그 설정	
	DWORD				GetJointProgressFlag() { return m_nJointProgressFlag; }

	bool				IsJointProgressing() { if( 0 != m_nJointProgressFlag) {return true;} return false; }		// 조그레스 진화 과정중인가?
	bool				IsCurJointProgress();			// 현재 조그레스 모드인가?
	int					GetCurEvoUnit();

	virtual DWORD		GetBaseDigimonFTID() const{ return m_dwBaseDigimonID; }
	DWORD				GetPrevBaseDigimonFTID(){ return m_dwPrevBaseDigimonID; }
	int					GetMaxEvoUnit(){ return m_nMaxEvoUnit; /* 기반 인덱스가 1부터 시작하므로 부등호 <=, >= 로 사용해야댄다*/ }

	void				SetAttributeEvoUnit( int nMaxEvoUnit, cEvoUnit* pEvoUnit );
	int					GetAttributeEvoUnitSlotState( int nSlot );
	cEvoUnit*			GetAttributeEvoUnit();
	cEvoUnit*			GetAttributeEvoUnit( int nSlot );
	cEvoUnit*			GetCurAttributeEvoUnit();
	int					GetCurrentEvoMaxSkillLv();

	void				SetAttributeExp( n2* pUnit );	// 
	n2				GetAttributeExp( int nAttributeType ){ return m_AttributeExp[ nAttributeType ]; }
	n2				GetFTAttributeExp( int nFTAttributeType );

	void				SetNatureExp( n2* pUnit );	// 
	n2				GetNatureExp( int nNatureType ){ return m_NatureExp[ nNatureType ]; }
	n2				GetFTNatureExp( int nFTNatureType );

	bool			IsCurrentElement( int nElementID );
	n2				_GetUnknownExpforEvolRate( nsCsDigimonTable::eEVOLUTION_TYPE eEvol );
protected:
	n2					m_AttributeExp[NewAttribute::MaxDigitalType];
	n2					m_NatureExp[NewAttribute::MaxNatualType];

	//====================================================================================================
	//	테이블
	//====================================================================================================
protected:
	CDigimonEvolveInfo*								m_pFT_Evol;
	CDigimonEvolution::sBATTLE_EVO_INFO*			m_pFT_BattleEvo;
	CDigimonEvolution::sBATTLE_EVO_INFO::sELEMENT*	m_pFT_BattleEvoElement;
public:
	CDigimonEvolveInfo*	GetFTEvol();
	CDigimonEvolveObj*	GetFTEvolCurObj();
	CDigimonEvolution::sBATTLE_EVO_INFO*			GetFTBattleEvo();
	CDigimonEvolution::sBATTLE_EVO_INFO::sELEMENT*	GetFTBattleEvoCurElement();

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
		int				s_nMaxHP;
		int				s_nMaxDS;
		int				s_nHP;
		int				s_nDS;
		__int64			s_dwExp;
		int				s_nDSBackup;

		int				s_nFriendShip;

		int				s_nAtt;
		int				s_nDef;
		int				s_nCritical;
		//int				s_nAttSpeed;
		int				s_nEvade;
		int				s_nHitRate;
		int				s_nAttRange;
		float			s_fBattleEP;

		int				s_nBL;

	public:
		int		GetMaxHP() const;
		int		GetMaxDS() const;
		int		GetHP() const;
		int		GetDS() const;
		float	GetHPRate() const;
		float	GetDSRate() const;
		int		GetFriendShip() const;

		void	SetMaxHP( int nMaxHP );
		void	SetMaxDS( int nMaxDS );
		void	SetDeltaMaxHP( int nDelta );

		void	SetHP( int nHP );
		void	SetDS( int nDS );
		void	SetDeltaHP( int nDelta );
		void	SetDeltaBackupHP( int nDelta );
		void	SetDeltaDS( int nDelta );
		void	SetDeltaBackupDS( int nDelta );

		__int64	GetExp() const;
		void	SetExp( __int64 dwExp );
		void	SetFriendShip( int nFS );

		int				GetAtt() const;
		int				GetDef() const;
		int				GetCritical() const;
		//int				GetAttSpeed() const;
		int				GetEvade() const;
		int				GetHitRate() const;
		int				GetAttRange() const;

		void			SetAtt( int nValue );
		void			SetDef( int nValue );
		void			SetCritical( int nValue );
		//void			SetAttSpeed( int nValue );
		void			SetEvade( int nValue );
		void			SetHitRate( int nValue );
		void			SetAttRange( int nValue );
		void			SetBattleEP( float fEP );
		float			GetBattleEP() const;

		void			SetBL(int nValue);
		int				GetBL() const;
	};	

	//====================================================================================
	//	스탯	- 항상 동적으로 구하겠다 - 공식을 변경하고 싶으면 이부분을 바꾸라
	//====================================================================================
protected:
	CsEnableTimeSeq			m_BattleEPDecrTimeSeq;
	int						m_nNewSkillPoint;

public:
	void					SetBattleEPDecrDisable();
	void					SetNewSkillPoint( int nPoint );
	DWORD					GetNewSkillPoint() const;
	void					DecNewSkillPoint( int nDec );

	virtual float			GetAttackRange();
//	virtual float			GetAttackSpeed();

	//====================================================================================================
	//	부가 정보
	//====================================================================================================
protected:
	cResist::sCHAR_QUICKSLOT::sCHAR*	m_pQuickSlot;
public:
	virtual NiColor		GetNameColor(){ return NiColor( 71.0f/255.0f, 225.0f/255.0f, 152.0f/255.0f ); }
	cResist::sCHAR_QUICKSLOT::sCHAR*	GetQuickSlot(){ return m_pQuickSlot; }

	virtual bool		IsRenderName(){ return g_pResist->m_Global.s_bMyRenderName && IsEnableObject(); }

	virtual	float		GetToolHeight(){ return CsC_AvObject::GetToolHeight(); }
	float				CalTamerDist();

	// 현재는 음성 플레이 밖에 쓰이는데가 없다
	virtual bool		IsUser(){ return g_pResist->m_Global.s_bEnableVoice; }

protected:
	void				_StopAttack( bool bSkillCall );

	//=============================================================================================================
	//	이동
	//=============================================================================================================
public:	
	virtual void		PauseEndTime( ePAUSE_TIME_END ePTEType, int nValue );

	void				SetServerInitPos(){ m_UserServerMove.SetServerInitPos( m_vPos ); }
	void				DoPos_ByTamer();
	void				DoAttackTargetPos();
	void				DoPos_ByNpcFind( int nNpcID );

	bool				CheckTamerDist();
	bool				CheckTargetDist( CsC_AvObject* pTarget, float fAttRange );
	virtual void		SetServerMoveReady(){ m_UserServerMove.SetServerMoveReady(); }
	virtual NiPoint2	KnockBack( NiPoint2 vHitterPos );

protected:
	void				_SetTargetPos();
	bool				_SetAttack( CsC_AvObject* pAttackSrc );

	void				_MoveStop_ByArriveDist();
	NiPoint2			_GetArrivePos_ByTamer();
public:
	bool				_IsDoMove();

protected:
	CUserServerMove		m_UserServerMove;
	CsOnceTimeSeq		m_MoveOnceTimeSeq;
	CsCoolTimeSeq		m_MoveCoolTimeSeq;
public:
	CUserServerMove*	GetUserServerMove(){ return &m_UserServerMove; }	
	CsCoolTimeSeq*		GetMoveCoolTimeSeq(){ return &m_MoveCoolTimeSeq; }


	//=============================================================================================================
	//	이벤트
	//=============================================================================================================
protected:
	CsTimeSeq			m_EventTime;

	//=============================================================================================================
	//	라이딩
	//=============================================================================================================
public:
	virtual bool		SetRide();
	virtual bool		SetRideEnd();
	CsTimeSeq			m_ResetTime;			//스킬 쿨타임 발동 -> 반짝이 마스크 랜더 시간 체크, true일 때 반짝이 랜더

	void				ResetSkillCool();
	void				ReloadEffect();

	DWORD				GetCurrentDigimonType() const;

	void TamerDistCheck();	//2017-01-03-nova _UpdateTamerDist() 호출 / 테이머와의 거리를 계속 측정하여 일정간격 유지

	//////////////////////////////////////////////////////////////////////////
	// 서버에서 받은 디지몬의 스킬 남은 시간 임시 저장
	// 디지몬 스킬 정보 변경시 아래 정보가 있으면 아래의 정보로 셋팅 해 줘야 함.
	//////////////////////////////////////////////////////////////////////////
public:
	struct sReaminCoolTime
	{
		sReaminCoolTime():dwSkillCode(0),dwRemainTime(0){}
		DWORD dwSkillCode;
		DWORD dwRemainTime;		
	};

	struct sSkillCoolTimeReservation
	{
		sSkillCoolTimeReservation():dwDigimonTBID(0) {};
		void Clear()
		{
			dwDigimonTBID = 0;
			listReaminTime.clear();
		}

		DWORD dwDigimonTBID;
		std::list<sReaminCoolTime> listReaminTime;
	};

	void SetReservationSkillReaminCoolTime( sSkillCoolTimeReservation const& reservationData );

protected:
	virtual void	SetReservationSkillCoolTime(uint nFileTableID);

private:
	sSkillCoolTimeReservation	m_ReservationData;
};