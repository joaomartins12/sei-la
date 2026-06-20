
#pragma once


/**
 * \ingroup     :	CsGBChar
 * \date        :	2009-11-06
 * \author      :	song
 *
 * \remarks 케릭터 관련 거의 모든 기능을 담당하고 있는 클래스
 *
 */
class CsC_AvObject : public CsC_Object
{
	CsCRTTI_H
	THREAD_MEMPOOL_H( CsC_AvObject )

public:
	CsC_AvObject();

public:
	static  bool		g_bEnableVoice;
	/**< 보이스 사운드 플레이가 가능한가? 전역변수 */

	//====================================================================================
	//		Base
	//====================================================================================
public:
	virtual void		Delete();
	virtual void		PreDelete();

	virtual bool		IsDelete();
	virtual void		DeleteUpdate();

	virtual void		Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp );
	virtual void		Update( float fDeltaTime, bool bAnimation = true );	
	virtual void		Render();
	virtual void		Render_PostChar(){}
	void				RenderEtc();

	// 타겟 위치, 범위 체크, 거리 반환
	bool				IsNearObject( NiPoint2 const& targetPos, float const& fLen, float& fDist );

public:
	void				UpdateVisible( float fAniTime ){ CsGBVisible::OnVisible( &m_Node, m_Node.m_pNiNode, m_dwVisiblePlag, fAniTime ); }
	/**< 실제 보이게 되면 보이는 것만 부가 처리 ( 빌보드, 파티클팩토리 등등 ) */

	//====================================================================================
	//		제거 대기 상태
	//====================================================================================
public:
	enum eDELETE_STATE{ DS_FADEOUT, DS_IMMEDEATE, DS_COSTOM_01, DS_COSTOM_02 };
	virtual void		SetDeleteState( eDELETE_STATE ds );
	virtual bool		UpdateDelete( float fDeltaTime );

protected:
	eDELETE_STATE		m_eDeleteState;

	//====================================================================================
	//		Load
	//====================================================================================
protected:
	enum{
		LOAD_INIT				=	0x00000000,		///< 로드전
		LOAD_MODEL_FILE			=	0x0000000f,		///< 로드 모델 파일
		LOAD_FIRST_UPDATE		=	0x00000010,		///< 로드 후 주 스레드에서의 초기화
		LOAD_INITITAL_LOAD		=	0x00000020,
		LOAD_COMPLEATE			=	LOAD_MODEL_FILE | LOAD_FIRST_UPDATE | LOAD_INITITAL_LOAD,		
		LOAD_DELETE				=	0x00000100,
	};

	sRES_LOAD*					m_pThreadResLoad;
	DWORD						m_dwLoadID;
	CThreadSafeDWORD			m_dwLoadPlag;			///< 로드 플래그~ 완료시 LOAD_COMPLEATE

public:
	enum eTHREAD_READY_TYPE{ TRT_ATTACH, };
	struct sTHREAD_READY_BASE
	{
		virtual void Delete(){}
		eTHREAD_READY_TYPE	s_eType;
	};

	// 포인터 연결만 해줄뿐~ 제거 루틴을 만들지 말자
	struct sTHREAD_READY_ATTACH : public sTHREAD_READY_BASE
	{
		sTHREAD_READY_ATTACH()
		{ s_pNiNode = NULL; s_pRefLoad = NULL; s_eType = TRT_ATTACH; s_nAttachType = 0; s_pFTValue = NULL; }

		virtual ~sTHREAD_READY_ATTACH(){ s_pNiNode = 0; }
		virtual void	Delete(){ s_pNiNode = 0; s_pRefLoad = NULL; s_nAttachType = 0; s_pFTValue = NULL; }

		int					s_nAttachType;
		sLOAD_THREAD_REF*	s_pRefLoad;
		NiNodePtr			s_pNiNode;
		void*				s_pFTValue;
	};

public:
	// 외부 안전 한데서 호출하자 - 업데이트전 - 유아이에서~ 또는 서버한테 정보 받았을 경우에
	// 파츠오브젝트는 변경 금지
	virtual void		ChangeKFM( UINT nFTID );
	virtual void		ChangeModel( UINT nFTID, LPCTSTR szName );

	virtual void		ThreadCallBack_LoadKFM( sLOAD_KFM* pLoad );
	virtual void		ThreadCallBack_LoadNIF( sLOAD_NIF* pLoad );

	virtual void		ThreadCallBack_LoadCompleate( int unitType, int nAddRefCount );

	virtual void		PostLinkObject(){}

	bool				IsLoad(){ return ( m_dwLoadPlag.Lock_Get() == LOAD_COMPLEATE ); }	

	/**< 로드가 완료 되었는가 */
	/**< 모델파일이 쓰레드로 부터 셋팅 되었는가 */
	enum eCHECK_TYPE{ CHECK_NOT_LOAD, CHECK_LOAD_COMP, CHECK_LOADED, };
	virtual eCHECK_TYPE	CheckModel();

protected:
	virtual bool		_IsInitialLoad(){ return true; }
	virtual void		_LoadedFile( int nCallRTTI );
	
	/**< 선행로드 인가? */
	virtual bool		_IsFastPartLoad() { return false; }

	//====================================================================================
	//		Trnasform
	//===================================================================================
	// Lock
public:
	enum ePAUSE_TYPE{	PAUSE_PATH,			/**< 패쓰 정지 - 움직이지 않겠다 */
						PAUSE_ANIMATION,	/**< 애니 정지 - 애니메이션 셋팅 되지 않겠다 */
						PAUSE_ROTATION,		/**< 회전 정지 - 회전애니 업데이트 하지 않겠다  */
						PAUSE_COUNT, };		/**< 정지 종류 카운트 */

	enum ePAUSE_TIME_END
	{
		PTE_NONE,
		PTE_PATH,
		PTE_ANIMATION,		
	};

	enum ePTE_PATH
	{
		PTE_P_NONE,
		PTE_P_KEYCHECKTIME_RESET,		
	};

protected:
	struct sPAUSE
	{
		void	Reset(){ s_bPause = false; s_bEnableTimeSeq = false; s_ePTEType = PTE_NONE, s_nValue = 0; }

		bool				s_bPause;
		bool				s_bEnableTimeSeq;
		CsTimeSeq			s_PauseTimeSeq;
		ePAUSE_TIME_END		s_ePTEType;
		int					s_nValue;
	};
	sPAUSE				m_PauseArray[ PAUSE_COUNT ];

	/**< 정지 정보 담은 배열 */
public:
	bool				IsPause( ePAUSE_TYPE type ){ return m_PauseArray[ type ].s_bPause; }
	void				SetPauseTime( ePAUSE_TYPE type, float fTime, ePAUSE_TIME_END pte = PTE_NONE, int nValue = 0 );
	
	void				SetPauseAll( bool bPause );
	void				SetPause( ePAUSE_TYPE type, bool bPause );

	virtual void		PauseEndTime( ePAUSE_TIME_END ePTEType, int nValue );

protected:
	void				_UpdatePause();
	virtual void		_Update_EffectPos(){}


	// Position
protected:
	NiPoint3			m_vPos;				/**< 현재 위치 정보 */
	float				m_fHeightConstant;	/**< 현재 위치에서 z값에 더하는 변위 (ex.몬스터 출현등에서 쓰임) */
	
public:
	void				SetPos( NiPoint2 vPos );
	void				SetPos( NiPoint3 vPos );
	void				SetHeightConstant( float fHeightConstant ){ m_fHeightConstant = fHeightConstant; }
	/**< z값 변위 셋팅 */

	NiPoint3			GetPos() const{ return m_vPos; }								/**< 현재 위치 얻어냄 */
	NiPoint2			GetPos2D() const { return NiPoint2( m_vPos.x, m_vPos.y ); }	/**< 현재 위치에서 z값을 제외한 값 얻어냄 */
	float				GetHeightConstant() const{ return m_fHeightConstant; }		/**< z값 변위 얻어냄 */

	// 스케일
protected:
	float				m_fScaleResult;		/**< 스케일 결과값 = 게임스케일*파일테이블스케일*툴스케일 */
	float				m_fScaleResultSqrt;	/**< 스케일 결과값의 루트 - 연산문에 미리 구해놓음 */
	float				m_fScaleGame;		/**< 게임스케일 - 게임내에서 동적 셋팅하는 스케일 */
	float				m_fScaleFT;			/**< 파일테이블스케일 - 현재 몬스터 테이블에서 셋팅되는값밖에 없다 */
	float				m_fScaleTool;		/**< 툴 스케일 - 툴에서 셋팅되는 스케일 */
	float				m_fScaleOrgGame;

public:
	virtual void		SetScale( float fScale, bool bOrgScale = true );	
	float				GetScale(){ return m_fScaleResult; }			/**< 최종 스케일 값 반환 */
	float				GetScaleSqrt(){ return m_fScaleResultSqrt; }	/**< 스케일 루트값 반환 */
	float				GetGameScale(){ return m_fScaleGame; }			/**< 게임 스케일 반환 */
	float				GetOrgGameScale(){ return m_fScaleOrgGame; }	/**< 게임 오리지널 스케일 반환 */
	DWORD				GetLoadID() { return m_dwLoadID; }				//2017-05-30-nova

protected:
	float				_GetScaleFT(){ return m_fScaleFT; }				/**< 파일테이블 스케일 값 반환 */
	float				_GetScaleTool(){ return m_fScaleTool; }			/**< 툴 스케일 값 반환  */

protected:
	float				m_fCurRot;			/**< 현재 회전값 NiPoint3( 0, -1, 0 ) 기준 */
	float				m_fXRot;			/**< x축으로 회전한 기울기 */
	float				m_fDestRot;			/**< 회전 목표값 */
	NiPoint3			m_vDir;				/**< 현재 케릭터가 바라보는 방향 */
public:
	void				SetRotation( CsC_AvObject* pObject, bool bIncludeDestRot = false );
	void				SetRotation( float fRot, bool bIncludeDestRot = false );
	void				SetRotation( NiPoint3 vDest, bool bIncludeDestRot = false );	

	void				SetRotation_AllAxis( NiPoint3 vDest );

	void				SetRotation_Ani( CsC_AvObject* pObject );
	void				SetRotation_Ani( NiPoint3 vDestPos );
	void				SetRotation_Ani( NiPoint2 vDestPos ){ SetRotation_Ani( NiPoint3( vDestPos.x, vDestPos.y, 0.0f ) ); }	
	/**< SetRotation_Ani( NiPoint3 vDestPos ) 와 같은 기능 */
	void				SetRotation_Ani( float fRot );
	void				SetRotation_AniDelta( float fRotDelta );
	virtual void		SetRotation_AniDir( NiPoint3 vDestDir );		
	
	void				SetRotX( float fx ) { m_fXRot = fx; }
	float				GetCurRotX() { return m_fXRot; }

	float				GetDestRot(){ return m_fDestRot; }
	float				GetCurRot(){ return m_fCurRot; }
	/**< 현재 회전값 반환 */
	NiPoint3			GetDir(){ return m_vDir; }
	/**< 현재 바라보는 방향 반환 */

protected:
	void				_SetRotationNode();
	void				_UpdateRotation( float fDeltaTime );

	bool				m_bAniPause;

public:
	void				SetAniPause( bool bAni ) { m_bAniPause = bAni; }
	bool				GetAniPause() { return m_bAniPause; }

	//====================================================================================
	//		UseClient
	//====================================================================================
public:
	enum eCLIENT_PLAG
	{
		CP_DMG_ALL		=	0x00000003,
		CP_DMG_NORMAL	=	0x00000001,
		CP_DMG_SKILL	=	0x00000002,
		CP_DMG_MASK		=	0x0000000f,
	};
protected:
	DWORD				m_nClientPlag;
public:
	DWORD				GetClientPlag(){ return m_nClientPlag; }
	void				SetClientPlag( DWORD plag ){ m_nClientPlag = plag; }

	//====================================================================================
	//		Model
	//====================================================================================
protected:
	bool				m_bViewSight;		/**< 현재 시야에 보이는가 */
	CsNodeObj			m_Node;				/**< NIF 노드 정보 */
	NiNode*				m_pABVWire;			/**< 충돌박스 */

	DWORD				m_dwChangeApplyModelID;

public:
	bool				GetViewSight(){ return m_bViewSight; }							/**< 시야에 보이는지 반환 */
	void				SetViewSight( bool bView ){ m_bViewSight = bView; }				/**< 시야에 보이는지 강제 셋팅 */
	CsNodeObj*			GetCsNode(){ assert_cs( ( IsLoad() == false ) || ( GetRefLinkObject() == 0 ) ); return IsLoad() ? &m_Node : NULL; }		/**< NIF노드 정보 반환 - 현재 쓰레드에 요청한게 있다면 NULL 반환 */
	NiPoint3			GetCenter();
	

	//====================================================================================
	//		Path
	//====================================================================================
public:
	virtual void		DeletePath();
	virtual void		ArriveTarget(){}
	virtual void		SetServerMoveReady(){}
	virtual NiPoint2	KnockBack( NiPoint2 vHitterPos );

	/**< 구현 내용 없음 */

	//====================================================================================
	//		Sound
	//====================================================================================
public:
	virtual char*		GetSoundDir()						{ return NULL; }
	virtual void*		PlaySound( char* cSoundPath )		{ return NULL; }	// 효과음이 기본입니다.
	virtual void*		PlayStaticSound( char* cSoundPath )	{ return NULL; }

	/**< 구현 내용 없음 */
	virtual void*		PlayAniSound( DWORD dwAni );

	//====================================================================================
	//		Skill
	//====================================================================================
public:
	virtual bool		SendSkill( int nSkillIndex, CsC_AvObject* pTarget ){ return false; }
	/**< 구현 내용 없음 */
	virtual bool		RecvSkill( bool bSuccess, int nSkillIndex ){ return false; }
	/**< 구현 내용 없음 */

	//====================================================================================
	//		Property
	//====================================================================================
protected:
	CsC_Property*		m_pProperty[ CsC_Property::MAX_COUNT ];
	/**< 프로퍼티 배열 */

public:
	bool				IsProperty( CsC_Property::eTYPE type ){ return ( m_pProperty[ type ] != NULL ); }
	/**< 프로퍼티 포인터가 존재 하는가 */

	// Animation
	float				GetAniTime(){ return GetProp_Animation()->GetAniTime(); }
	/**< 애니메이션 타임값 반환 */

	CsC_AniProp*		GetProp_Animation(){ assert_cs( m_pProperty[ CsC_Property::ANIMATION ] != NULL ); return (CsC_AniProp*)m_pProperty[ CsC_Property::ANIMATION ]; }
	/**< 애니 프로퍼티포인터 반환 */

	/**
	 * \author      :	      song
	 * \remarks	: 애니메이션 셋팅
	 * \param dwAniID 애니메이션 ID
	 * \param fAniAccel 애니메이션스피드
	 */
	virtual void		SetAnimation( DWORD dwAniID, float fAniAccel = 1.0f ){ GetProp_Animation()->SetAnimation( dwAniID, false, fAniAccel ); }

	virtual void		SetDefault_IdleAnimation();

protected:
	virtual void		_SetDefault_IdleAnimation_Stand();
	virtual void		_SetDefault_IdleAnimation_Move();

public:
	void				AttachProp( CsC_Property::eTYPE eType );
	/**< 초기에 생성안된 프로퍼티 만들기 */

	CsC_PathProp*		GetProp_Path(){ assert_cs( m_pProperty[ CsC_Property::PATH ] != NULL ); return (CsC_PathProp*)m_pProperty[ CsC_Property::PATH ]; }
	/**< 패스 프로퍼티포인터 반환 */

	CsC_AlphaProp*		GetProp_Alpha(){ assert_cs( m_pProperty[ CsC_Property::ALPHA ] != NULL ); return (CsC_AlphaProp*)m_pProperty[ CsC_Property::ALPHA ]; }
	/**< 알파 프로퍼티포인터 반환 */

	CsC_AttackProp*		GetProp_Attack(){ assert_cs( m_pProperty[ CsC_Property::ATTACK ] != NULL ); return (CsC_AttackProp*)m_pProperty[ CsC_Property::ATTACK ]; }
	/**< 어택 프로퍼티포인터 반환 */

	CsC_EffectProp*		GetProp_Effect(){ assert_cs( m_pProperty[ CsC_Property::EFFECT ] != NULL ); return (CsC_EffectProp*)m_pProperty[ CsC_Property::EFFECT ]; }
	/**< 이펙트 프로퍼티포인터 반환 */

	CsC_MoveControlProp* GetProp_MoveControl(){ assert_cs( m_pProperty[ CsC_Property::MOVE_CONTROL ] != NULL ); return (CsC_MoveControlProp*)m_pProperty[ CsC_Property::MOVE_CONTROL ]; }
	/**< 무브컨트롤러 프로퍼티포인터 반환 */

	virtual bool		AttachEffect( char* cEffectPath, float fScale, DWORD dwPlag, bool bPreLoad = false );

	//====================================================================================
	//		CallBack
	//====================================================================================
public:
	virtual void		CallBack_StartSequence( DWORD dwAniID );
	virtual bool		CallBack_EndofSequence( DWORD dwAniID );
	virtual void		CallBack_EventSequence( DWORD dwAniID, CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption );
protected:
			void		Event_RangeAttact_Start( CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption );

public:
			void		Event_Sound( CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption, CsC_AvObject* pTarget );
			void		Event_NormalAttack_Clash( CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption, CsC_AvObject* pTarget );

	//====================================================================================
	//		상태
	//====================================================================================
protected:
	bool							m_bServerDie;
	nsCsDigimonTable::eNATURE_TYPE	m_eNatureType;
	nsCsDigimonTable::eNATURE_TYPE	m_eNatureTypes[3];
	bool							m_bEnableObject;

	/**< 서버상태에서 죽은 상태인가 */

public:
	virtual void		SetBattle(){ assert_cs( false ); }
	/**< 구현 내용 없음 */
	virtual void		SetBattleEnd(){ assert_cs( false ); }
	/**< 구현 내용 없음 */
	virtual void		SetDie(){ m_pBaseStat->SetHP( 0 ); }
	/**< 죽었을때 호출 */

	bool				IsServerDie(){ return m_bServerDie; }
	/**< 서버에서 죽은 상태인지 반환 */
	virtual void		SetServerDie( bool bDie ){ m_bServerDie = bDie; }
	/**< 서버에서 죽은 상태인지 셋팅 */
	nsCsDigimonTable::eNATURE_TYPE	GetNatureType(){ return m_eNatureType; }
	nsCsDigimonTable::eNATURE_TYPE	GetNatureType2();
	bool							IsStrongNature2( CsC_AvObject* pTarget );

	bool							IsStrongNature( CsC_AvObject* pTarget );

	void				SetEnableObject( bool bEnable ){ m_bEnableObject = bEnable; }
	bool				IsEnableObject() const { return m_bEnableObject; }

	virtual void		SetMarble( DWORD mt, CsC_AvObject* pFollow, int nCnt ){}

	bool				IsHaveRangeEvent(DWORD const& dwAniId);
	//====================================================================================
	//		씬
	//====================================================================================
public:
	virtual void		SceneAttackPropEvent( CsC_AttackProp::sEVENT_INFO* pEvent );
	virtual void		SceneLevelUp( int nLevel );
	virtual void		SceneSkillUp( int nSkillLevel, int nSkillPoint, UINT nSkillExp, UINT nEvoIndex ){ assert_cs( false ); }
	/**< 구현 내용 없음 */

	//====================================================================================
	//		스탯
	//====================================================================================
public:

	enum eSTAT{ INVALIDE_STAT = INT_MAX,	/**< 스탯값 셋팅 되지 않았을 경우의 초기값 */
				BASE_STAT = 0,				/**< 기본 적으로 가지고 있는 셋팅들 */
				USER_STAT = 1,				/**< 유저 ( 내 케릭터 )일 경우에 가지고 잇는 셋팅값 */
	};	

	/**
	* \ingroup     :	CsGBChar
	* \date        :	2009-11-06
	* \author      :	song
	*
	* \remarks 기본 슷탯 정보를 담고 있는 구조체
	*/
	struct sBASE_STAT : public NiMemObject
	{
	public:
		sBASE_STAT( CsC_AvObject* pParent );
		virtual ~sBASE_STAT();

		virtual eSTAT	GetType() const;

	public:
		int				GetLevel() const;

		virtual int		GetMaxHP() const;
		virtual int		GetMaxDS() const;
		virtual int		GetHP() const;
		virtual int		GetBackupHP() const;
		virtual float	GetHPRate() const;
		virtual int		GetDS() const;
		virtual float	GetDSRate() const;
		virtual	int		GetFP() const;
		virtual float	GetFPRate() const;
		virtual int		GetFriendShip() const;
		virtual float	GetBattleEP() const;
		virtual int		GetEvade() const;
		virtual int		GetAtt() const;
		virtual int		GetDef() const;
		virtual int		GetCritical() const;

		virtual void	SetAttackSpeed( int nSpeed );
		virtual void	SetMaxHP( int nMaxHP );
		virtual void	SetMaxDS( int nMaxDS );
		virtual void	SetAtt(int nValue);
		virtual void	SetDef( int nValue );
		virtual void	SetEvade( int nValue );
		virtual void	SetHitRate(int nValue );
		virtual void	SetCritical( int nValue );
		virtual void	SetDeltaMaxHP( int nMaxHP );
		virtual void	SetDeltaMaxDS( int nMaxDS );
		virtual void	SetHP( int nHP );
		virtual void	SetBackupHP( int nHP );
		virtual void	SetDS( int nDS );
		virtual void	SetFP( int nFP );
		virtual void	SetDeltaFP( int nDelta );
		virtual void	SetDeltaHP( int nDelta );
		virtual void	SetDeltaBackupHP( int nDelta );
		virtual void	SetDeltaDS( int nDelta );
		virtual void	SetDeltaBackupDS( int nDelta );
		virtual void	SetFriendShip( int nFS );
		virtual void	SetBattleEP( float fEP );

		float			GetMoveSpeed() const;
		float			GetAttackSpeed() const;
		bool			IsFatigue() const;
		float*			GetPointer_MoveSpeed();

		void			SetLevel( int nLevel );
		void			SetMoveSpeed( float fMoveSpeed );
		void			EnableFatigue( bool bFatigue );

		virtual __int64	GetExp() const;
		virtual void	SetExp( __int64 dwExp );

	protected:
		CsC_AvObject*	s_pParent;
		int				s_nLevel;			/**< 레벨 */
		int				s_nHPPercent;		/**< 255분율 퍼센트 게이지 */
		int				s_nHPBackup;		/**< 실제 값 저장하는 백업값 ( 255분율 ) */
		float			s_fMoveSpeed;		/**< 이동 속도 */
		bool			s_bFatigue;			/**< 피로한가 */		
		int				s_nAttackSpeed;		/**< 공격 속도 */			
	};

protected:
	sBASE_STAT*				m_pBaseStat;
	/**< 기본 스탯 */
	
public:
	sBASE_STAT*				GetBaseStat(){ return m_pBaseStat; }
	/**< 기본 스탯 포인터 반환 */
	virtual float			GetAttackRange(){ assert_cs( false ); return 0.0f; }
	/**< 구현내용없음 */
	virtual float			GetAttackCharSize(){ assert_cs( false ); return 0.0f; }	
	/**< 구현내용없음 */
	virtual void			CalAniConstant( float fMoveSpeed ){}

	void					SetDoNotMoveObject( bool bValue );
	//====================================================================================
	//		부가 정보
	//====================================================================================
protected:
	sCREATEINFO			m_CharProp;			/**< 케릭터 생성 정보 */
	DWORD				m_dwVisiblePlag;	/**< 시야에 있을경우 처리해야하는 플래그 (ex.빌보드, 파티클팩토리) */
	float				m_fToolWidth;		/**< 툴에서 셋팅된 넓이값 */
	float				m_fToolHeight;		/**< 툴에서 셋팅된 높이값 */
	bool				m_bDoNotObject;		/* 고정형 인자? 이동 및 회전 제한*/

protected:
	virtual UINT		_GetTimeTS(){ assert_cs( false ); return (UINT)-1; }

public:
	virtual bool		IsUser(){ return false; }	/**< 유저(내케릭터) 인가 */

public:
	enum eATTACK_RANGE_TYPE{
		ART_CLOSE_COMBAT = 1,	/**< 근원 어택타입 : 근거리 */
		ART_RANGER = 2,			/**< 근원 어택타입 : 원거리 */
	};

public:
	virtual UINT		GetClass(){ assert_cs( false ); return 0; }
	/**< 구현내용없음 */
	virtual UINT		GetUniqID(){ assert_cs( false ); return 0; }
	/**< 구현내용없음 */
	virtual NiColor		GetNameColor(){ assert_cs( false ); return NiColor::BLACK; }
	/**< 구현내용없음 */

	virtual UINT		GetApplyModelID() const;
	/**< 애니메이션 시퀀스 모델적용 할때 사용하는 파일테이블 값 - 오토바이 라이딩때문에 생김 */
	virtual UINT		GetModelID() const;
	/**< 기본 모델ID는 파일테이블ID - 파일테이블 ID 리턴 */
	void				SetToolWidth( float fWidth ){ m_fToolWidth = fWidth; }
	/**< 툴 넓이값 셋팅 */
	void				SetToolHeight( float fHeight ){ m_fToolHeight = fHeight; }
	/**< 툴 높이값 셋팅 */
			float		GetToolWidth(){ return m_fToolWidth*GetScale(); }
			/**< 툴 넓이값 * 최종스케일 리턴 */
	virtual	float		GetToolHeight(){ return m_fToolHeight*GetScale(); }
			/**< 툴 높이값 * 최종스케일 리턴 */
	virtual	float		GetToolHeight_Int(){ return CsFloat2Int( m_fToolHeight )*GetScale(); }

	virtual float		CalIdleAniTime(){ return 8.0f + (float)( rand()%10 ); }
	/**< 기본 아이들 애니메이션 (ANI::IDLE_NORMAL ) 플레이 타임 반환 */
	virtual ANI::eTYPE	GetRandomIdle(){ return ( rand()%3 == 0 ) ? ANI::IDLE_CHAR : ANI::IDLE_SHAKE; }
	/**< 기본 아이들 애니메이션 후 나오는 애니메이션 반환 */
	virtual int			GetAttackRangeType(){ assert_cs( false ); return 0; }
	/**< 구현내용없음 */
	virtual bool		IsAttackRangeType(){ return ( GetAttackRangeType() == ART_RANGER ); }
	/**< 평타가 원거리 공격 타입인가 */
	virtual float		GetAlphaHideDist(){ assert_cs( false ); return 0.0f; }
	/**< 알파블랜딩으로 서서리 사라지는 거리값 리턴 */
	sCREATEINFO*		GetCharProp(){ return &m_CharProp; }
	/**< 케릭터 생성 정보 포인터 리턴 */
	virtual TCHAR*		GetName(){ assert_cs( false ); return NULL; }
	/**< 구현내용없음 */
	virtual bool		IsRenderName(){ assert_cs( false ); return false; }
	/**< 구현내용없음 */
	virtual float		GetAttackSpeed();

	// 
#define ATMD_INVALIDE  -1.0f
	virtual float		_GetAniTimeMoveDist_Walk(){ return ATMD_INVALIDE; }
	/**< 밀리는 효과를 없애기 위한 애니메이션 스피드 셋팅값 ( 애니메이션 한 루프당 이동 가능한 거리 )*/
	virtual float		_GetAniTimeMoveDist_Run(){ return ATMD_INVALIDE; }
	/**< 밀리는 효과를 없애기 위한 애니메이션 스피드 셋팅값 ( 애니메이션 한 루프당 이동 가능한 거리 )*/
	virtual float		_GetAniTimeMoveDist_ARun(){ return ATMD_INVALIDE; }
	/**< 밀리는 효과를 없애기 위한 애니메이션 스피드 셋팅값 ( 애니메이션 한 루프당 이동 가능한 거리 )*/
	std::string			GetModelPath() const;
	/**< kfm파일 경로 */

	//====================================================================================
	//		쉐이더
	//====================================================================================

	void				SetShaderEffect_Highlight();
	void				RemoveShaderEffect_Highlight();
	//====================================================================================
	//		픽킹
	//====================================================================================
public:
	virtual bool		IsPickEnable(){ return m_bViewSight && m_pABVWire; }
	/**< 픽킹 가능한지 리턴 */
	bool				PickObject( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, float& fLength );

	// 카메라
public:
	virtual void		AttachCamera_Critical( CsC_AvObject* pTarget ){}
	/**< 구현내용없음 */

protected:
	bool	m_bEnableColorEffect;
public:
	void SetEnableColorEffect(bool bFlag){m_bEnableColorEffect = bFlag;}

	void				DeleteLoopEffectSkill( DWORD nSkillArrIDX );
};

#include "02_AvObject.inl"