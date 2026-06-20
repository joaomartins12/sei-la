
#pragma once


namespace ANI
{
	enum eTYPE
	{
		INVALIDE_ID				= (DWORD)-1,
		IDLE_NORMAL				= 901101,
		IDLE_ATTACK				= 901102,
		IDLE_SHAKE				= 901103,
		IDLE_CHAR				= 901104,
		IDLE_ITEMCROP			= 901105,
		IDLE_FATIGUE			= 901106,
		IDLE_SIT				= 901107,
		IDLE_SIT_DOWN			= 901108,
		IDLE_SIT_UP				= 901109,

		TAKE_OBJECT				= 901114,

		MOVE_WALK				= 901201,
		MOVE_RUN				= 901202,
		MOVE_ATTACKRUN			= 901203,
		MOVE_FATIGUE			= 901204,

		IDLE_DIGIMON_RIDE		= 901211,
		MOVE_DIGIMON_RIDE		= 901212,

		IDLE_TAMER_RIDE_1		= 901213,
		MOVE_TAMER_RIDE_1		= 901214,
		IDLE_TAMER_RIDE_2		= 901215,
		MOVE_TAMER_RIDE_2		= 901216,
		IDLE_TAMER_RIDE_3		= 901217,
		MOVE_TAMER_RIDE_3		= 901218,
		IDLE_TAMER_RIDE_4		= 901219,
		MOVE_TAMER_RIDE_4		= 901220,
		IDLE_TAMER_RIDE_5		= 901221,
		MOVE_TAMER_RIDE_5		= 901222,
		IDLE_TAMER_RIDE_6		= 901223,
		MOVE_TAMER_RIDE_6		= 901224,		
		IDLE_TAMER_RIDE_7		= 901225,		// 2족보행 라이딩 chu8820
		MOVE_TAMER_RIDE_7		= 901226,		// 2족보행 라이딩 chu8820	
		IDLE_TAMER_RIDE_8		= 901227,		// 왼팔 라이딩 chu8820
		MOVE_TAMER_RIDE_8		= 901228,		// 왼팔 라이딩 chu8820	
		IDLE_TAMER_RIDE_9		= 901229,		// 왼손 라이딩 chu8820
		MOVE_TAMER_RIDE_9		= 901230,		// 왼손 라이딩 chu8820	

		ATTACK_ATTACK1			= 901301,
		ATTACK_ATTACK2			= 901302,
		ATTACK_MISS				= 901505,
		
		ATTACK_SKILL1			= 901311,
		ATTACK_SKILL2			= 901312,
		ATTACK_SKILL3			= 901313,
		ATTACK_SKILL4			= 901314,
		ATTACK_SKILL5			= 901315,

		ACT_BUFF				= 901321,
		// 보스몬스터 스킬 사용
		ATTACK_MON_SKILL1_USE	= 901401,		// 몬스터 스킬 시퀀스ID 수정 chu8820 14.08.18
		ATTACK_MON_SKILL2_USE	= 901402,		// 몬스터 스킬 시퀀스 추가 수정( 시작 / 시전 / 사용 으로 분할 - 현무몬 인던 추가시 수정 ) chu8820 14.11.05
		ATTACK_MON_SKILL3_USE	= 901403,		// 기존 몬스터 시퀀스 ID 수정 최소화를 위해 ID 순서 변경
		ATTACK_MON_SKILL4_USE	= 901404,
		ATTACK_MON_SKILL5_USE	= 901405,
		// 보스몬스터 스킬 시전 시작
		ATTACK_MON_SKILL1_START	= 901411,		
		ATTACK_MON_SKILL2_START	= 901412,		
		ATTACK_MON_SKILL3_START	= 901413,
		ATTACK_MON_SKILL4_START	= 901414,
		ATTACK_MON_SKILL5_START	= 901415,
		// 보스몬스터 스킬 시전 중
		ATTACK_MON_SKILL1_LOOP	= 901421,
		ATTACK_MON_SKILL2_LOOP	= 901422,
		ATTACK_MON_SKILL3_LOOP	= 901423,
		ATTACK_MON_SKILL4_LOOP	= 901424,
		ATTACK_MON_SKILL5_LOOP	= 901425,

		BERSERK_SKILL			= 901499,	// 광포화 애니메이션

		DAMAGE_DAMAGE1			= 901501,
		DAMAGE_DAMAGE2			= 901502,

		DAMAGE_BLOCK			= 901504,

		DAMAGE_EVADE1			= 901503,
		DAMAGE_DEAD				= 901509,

		DIGIVICE_CHAGE			= 901602,

		DIGIMON_EVOLUTION		= 901701,
		DIGIMON_EVOLUTION_END   = 901702,
		DIGIMON_EVOLUTION_DOWN   = 901703,

		BARCODE_CONNECT			= 901801,
		BARCODE_CARD_SUCCESS	= 901802,
		BARCODE_CARD_OVERCOUNT	= 901803,
		BARCODE_CARD_FAIL1		= 901804,
		BARCODE_CARD_FAIL2		= 901805,

		NPC_TALK				= 901111,
		NPC_YES					= 901112,
		NPC_NO					= 901113,		

		EMOTION_START			= 901901,
		EMOTION_END				= 901999,

		NPC_OPEN1				= 902902,
		NPC_OPEN_IDLE1			= 902903,
		NPC_OPEN2				= 902908,
		NPC_OPEN_IDLE2			= 902907,
		NPC_CLOSE				= 902906,
		NPC_TACTICS_SUCCESS		= 902904,
		NPC_TACTICS_FAIL1		= 902905,
		NPC_TACTICS_FAIL2		= 902909,
		NPC_TACTICS_GET			= 902910,
	};
	
	enum SKILLTYPE
	{
		NORMAL,
		RANDOMMULTY = 4,
		END,
	};
}

class CsC_AniProp_Sequence : public NiMemObject
{
#define CSC_ANIPROP_SEQUENCE_NONE_EVENT		-1
#define CSC_ANIPROP_SEQUENCE_INIT_EVENT		0

public:
	CsC_AniProp_Sequence();
	~CsC_AniProp_Sequence();

protected:
	CsC_AvObject*		m_pParent;
	CsC_LoadSequence*	m_pLoadSequence;

	DWORD				m_dwSrcID;
	DWORD				m_dwDestID;
	DWORD				m_dwLastDestID;
	float				m_fAniTime;

	bool				m_bLoop;
	int					m_nAniLoopCount;		// 루프 애니메이션이 아닐경우에 루프 카운트
	float				m_fOldExceedTime;		// 예전 초과 시간

	float				m_fStartTime;
	float				m_fEndTime;				// 끝나는 시간

	int					m_nActiveEventIndex;	// 발동 이벤트 인덱스

	int					m_nNAtt_Count;
	// 공격 관련 = 이벤트가 없다면 쓰레기 값이든 머든 상관 없다 - 외부에서는 참조하지 말자 ( 오직 콜백 이벤트에서만 참조 )	
	CsVectorPB< int >*	m_pNAtt_EffectIndexArray;	// 배열
	int					m_nLast_NAtt_SoundIndex;

	CsC_LoadSequence::sSEQUENCE_INFO*	m_pSrcSequence;	

	DWORD				m_dwLastID;
	CsVectorPB< DWORD >	m_pbLastID;

	bool				m_bChangedShader;

public:
	void		Delete();
	void		DeleteEventEffect();
	void		Init( CsC_AvObject* pParent, CsC_LoadSequence* pLoadSequence );	

	bool		Update( float fDeltaTime );

	void		SetTargetAnimation( DWORD dwAniID, bool bUseStopFrame );
	void		ResetAnimation();

	DWORD		GetSrcID(){ return m_dwSrcID; }
	float		GetCurTime(){ if( m_fStartTime != FLT_MAX ) return m_fAniTime - m_fStartTime; else return m_fAniTime; }
	float		GetEndTime(){ if( m_fStartTime != FLT_MAX ) return m_fEndTime - m_fStartTime; else return m_fEndTime; }
	void		SetCurTime( float fTime ){ m_fAniTime = fTime; }
	bool		IsEndAnimation(){ return ( m_fAniTime >= m_fEndTime ); }
	bool		IsLoop(){ return m_bLoop; }

	bool		IsSequence( DWORD dwAniID ){ return m_pLoadSequence->IsSequence( dwAniID ); }
	CsC_LoadSequence::sSEQUENCE_INFO*	GetSequence( DWORD dwAniID ){ return m_pLoadSequence->GetSequence( dwAniID ); }
	CsC_LoadSequence*					GetLoadSequence(){ return m_pLoadSequence; }

	// 원거리 공격 이벤트가 추가 되어 있는지 체크함
	bool		IsHaveRangeEvent(DWORD const& dwAniId);

protected:
	void		_FirstUpdate( float fDeltaTime );



	// 이벤트
public:
	struct sEVENT_OPTION : public NiMemObject
	{
		sEVENT_OPTION() { Reset(); }
		void Reset(){ s_fVelocity = s_fAccelVelocity = 0.0f; s_eHitType = CsC_AttackProp::HT_INVALIEDE; s_nAttackEffectIndex = 0; 
		s_bRangeAttack = true; s_fAniAccelRate = 1.0f; s_eSKillType = ANI::SKILLTYPE::NORMAL; 
		// 데미지 반사 버프 스킬 추가_14.05.14		chu8820
		s_nReflectType = 0;
		}
		float						s_fVelocity;
		float						s_fAccelVelocity;
		bool						s_bRangeAttack;
		CsC_AttackProp::eHIT_TYPE	s_eHitType;
		int							s_nAttackEffectIndex;
		float						s_fAniAccelRate;
		ANI::SKILLTYPE					s_eSKillType;
		// 데미지 반사 버프 스킬 추가_14.05.14		chu8820
		int							s_nReflectType;		// 데미지 반사 버프 대상 공격 시 버프 클래스
	};
protected:
	sEVENT_OPTION		m_EventOption;

public:
	sEVENT_OPTION*		GetEventOption(){ return &m_EventOption; }

public:
	CsC_LoadSequence::sTEXTKEY_EVENT*	NextEvent();
	void				SetEventOption( sEVENT_OPTION* pEventOption ){ memcpy( &m_EventOption, pEventOption, sizeof( sEVENT_OPTION ) ); }

	//============================================================================================================
	// 공격 관련

public:
	ANI::eTYPE	CalNormalAttackAni( CsC_AttackProp::eHIT_TYPE ht );				// 어떤애니메이션을 해야하는지

	// 크리티컬 이펙트와 싸운드 재생에 쓰인다
	bool		IsLastNAtt_Effect();
	bool		IsLastNAtt_Sound();

	// 공격 이벤트 전부 종료 되었는가 - 데미지 애니메이션 띄우는데 쓰인다
	bool		IsEmptyNAttEvent();
};