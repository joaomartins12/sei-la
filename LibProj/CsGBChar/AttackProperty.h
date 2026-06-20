
#pragma once 

typedef void (*CALLBACK_DamageNumber)( int nEffect, int nType, int nDamage, NiPoint3 vPos, float fDamageRot );

class CsC_Object;
class CsC_PathProp;
class CsC_AttackProp : public CsC_Property
{
	THREAD_MEMPOOL_H( CsC_AttackProp )

	// 콜백
protected:
	static CALLBACK_DamageNumber		g_CallBackDamageNumber;
public:
	static void			ResistCallBack_DamageNumber( void* func ){ g_CallBackDamageNumber = (CALLBACK_DamageNumber)func; }
	static NiPoint2		CalKnockBackPos( NiPoint2 vHitter, NiPoint2 vTarget, CsC_PathProp* pTargetPathProp, float fDist );


public:
	enum ePROCESS_STATE{
		SUCCESS_ATTACK_START, SUCCESS_ATTACK_ING, SUCCESS_MOVING, SUCCESS_CHANGE_TARGET,
		FAILE_BLOCKING, FAILE_POS, FAILE_HOLDBACK, FAILE_ASSERT,
	};

public:
	CsC_AttackProp();
	~CsC_AttackProp();
	virtual void			Delete();
	virtual void			PreDelete();
	void					DeleteHDList();
	void					Init( CsC_AvObject* pParent );

	void					PreUpdate( float fDeltaTime );
	//void					Update( float fDeltaTime );	

	CsC_AvObject*			GetHitTarget();
	CsC_AvObject*			GetFollowTarget(){ return m_pFollowTarget; }


public:
	enum eFOLLOW_ITERATOR{	FI_SKILL1 = 0, FI_SKILL2, FI_SKILL3, FI_SKILL4, FI_SKILL5, FI_ATTACK, };
	typedef std::map<int, CsC_AvObject*> MultyTargets;
	typedef MultyTargets::iterator MultyTargetIter;
protected:
	CsC_AvObject*			m_pHitTarget;						// 때려야 하는타겟
	CsC_AvObject*			m_pFollowTarget;					// 내가 움직여야 하는 타겟
	float					m_fFollowRange;						// AttRange
	eFOLLOW_ITERATOR		m_eFollowIterator;					// 따라가는 이유 - 식별자
	bool					m_bFollowIteratorRange;

	CsTimeSeq				m_TargetMoveTimeSeq;				// 움직이는거 매번 안하기 위한 체크 타임
	CsTimeSeq				m_TargetChangeTimeSeq;				// 타겟 바뀐거 매번 체크 안하기 위한 체크
	bool						m_bAttackingTarget;					// 타겟을 때리는 중인가
	MultyTargets				mMultiTargets;
public:
	MultyTargets* GetMultyTarget(void){return &mMultiTargets;}
	void					AddMultyTarget(int MultyTargetID, CsC_AvObject* pkObject);
	void					SetHitTarget( CsC_Object* pTarget );
	void					SetFollowTarget( CsC_Object* pTarget, float fFollowRange, eFOLLOW_ITERATOR eFollowIterator, bool bIteratorRange );
	void					ReleaseFollowTarget();
	ePROCESS_STATE			UpdateAttack( float fDeltaTime, eFOLLOW_ITERATOR& eFollowIterator );
	void					ReleaseAttackingTarget();
	bool					IsAttackingTarget() const;
	eFOLLOW_ITERATOR		GetFollowIterator() const;
	bool					IsFollowIteratorRange() const;
protected:
	bool					_IsAttackBlocking();

public:
	float					GetEnableFollowDist();


	//==========================================================================
	//	Hit, Damage
	//==========================================================================
public:
	enum eHD_TYPE{ HD_HIT = 0, HD_DAMAGE, HD_EVENT, };
	enum eHIT_TYPE{	HT_Normal = 10, HT_Critical, HT_Miss, HT_Skill, HT_Absorb,	// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
		HT_Skill_NoAni,	// 애니 없이 데미지만 들어가는 스킬( 장판으로 입는 데미지 ) chu8820
		HT_INVALIEDE = (DWORD)-1 };
	enum eDAMAGE_TYPE{ DT_Normal = 100, DT_Miss, DT_Block, DT_KnockBack, DT_Damage_Dot, DT_Recover, DT_Condition, DT_Dead,
						DT_Absorb, };	// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820		
public:
	struct sHD_BASE : public NiMemObject
	{
		eHD_TYPE s_nBaseType;
	};

	enum eACTIVE_TYPE{ AT_NONE_ACTIVE, AT_ACTIVE, AT_JUST_NOW, };
	enum eMARBLE_TYPE{	MB_NONE	= 0x00000000,
						MB_EVOL = 0x00000001,
						MB_HP	= 0x00000002,
						MB_DS	= 0x00000004, };
	struct sDAMAGE_INFO : public sHD_BASE
	{
		THREAD_MEMPOOL_H( CsC_AttackProp::sDAMAGE_INFO )
		sDAMAGE_INFO():s_eMarbleType(MB_NONE),s_eDamageType(DT_Normal),
		s_nNumEffect(0),s_nNumType(0),s_fHitEventTime(0.0f),s_fDamageRot(0.0f),s_eActive(AT_NONE_ACTIVE),
		s_dwResistTime(0),s_pChildDamageInfo(NULL),s_vHitterPos(NiPoint2::ZERO),s_pHitter(NULL)
		{ 
			s_nBaseType = HD_DAMAGE;
			for( int n = 0; n < 5; ++n)
				s_nValue[n] = 0;
		}

		eDAMAGE_TYPE	s_eDamageType;
		int				s_nNumEffect;
		int				s_nNumType;
		
		float			s_fHitEventTime;
		float			s_fDamageRot;

		int				s_nValue[ 5 ];					// 1:데미지, 2:HP, 3:DS, 4:MHP, 5:MDS
		eACTIVE_TYPE	s_eActive;						// 히트가 있을 경우 히트 쪽에서 활성화 시켜준다 ( 히트 없으면 바로 활성화로 집어 넣는다 )
		DWORD			s_dwResistTime;					// 등록 된지 이벤트 타임*2 초가 지나면 무조건 활성화 시켜준다. ( 히트쪽에서 데이터를 가지고 사라질경우 )
		sDAMAGE_INFO*	s_pChildDamageInfo;
		NiPoint2		s_vHitterPos;


		DWORD			s_eMarbleType;
		CsC_AvObject*	s_pHitter;
	};

	struct sHIT_INFO : public sHD_BASE
	{
		THREAD_MEMPOOL_H( CsC_AttackProp::sHIT_INFO )
		sHIT_INFO():s_MultyTargetID(-1),s_eHitType(HT_Normal),s_dwAniID(0),s_fAniAccelRate(1.0f)
		,s_pPartnerDamageInfo(NULL),s_bRotation(false),s_vDamagePos(NiPoint3::ZERO),s_pHitTarget(NULL),s_fRangeVelocity(0.0f)
		,s_fRangeAccelVelocity(0.0f),s_bRangeAttack(false),s_nAttackEffectIndex(0)
		// 데미지 반사 버프 스킬 추가_14.05.14		chu8820
		,s_nReflectType(0)
		{
			s_nBaseType = HD_HIT;
		}

		eHIT_TYPE		s_eHitType;
		DWORD			s_dwAniID;
		float			s_fAniAccelRate;
		sDAMAGE_INFO*	s_pPartnerDamageInfo;
		bool			s_bRotation;
		NiPoint3		s_vDamagePos;		
		CsC_AvObject*	s_pHitTarget;

		float			s_fRangeVelocity;
		float			s_fRangeAccelVelocity;
		bool			s_bRangeAttack;
		int				s_nAttackEffectIndex;
		int				s_MultyTargetID;
		// 데미지 반사 버프 스킬 추가_14.05.14		chu8820
		int				s_nReflectType;		// 데미지 반사 버프 대상 공격 시 버프 클래스
	};
	/*struct sENDBATTLE_INFO : public sHD_BASE
	{
		THREAD_MEMPOOL_H( CsC_AttackProp::sENDBATTLE_INFO )
		sENDBATTLE_INFO(){ s_nBaseType = HD_ENDBATTLE; }
	};
	struct sSKILL_UP_INFO : public sHD_BASE
	{
		THREAD_MEMPOOL_H( CsC_AttackProp::sSKILL_UP_INFO )
		sSKILL_UP_INFO(){ s_nBaseType = HD_SKILL_UP; }

		int				s_nLevel;
		UINT			s_nExp;
		int				s_nSkillPoint;
	};
	struct sLEVEL_UP_INFO : public sHD_BASE
	{
		THREAD_MEMPOOL_H( CsC_AttackProp::sLEVEL_UP_INFO )
		sLEVEL_UP_INFO(){ s_nBaseType = HD_LEVEL_UP; }

		int				s_nLevel;
	};*/

	enum eEVENT_TYPE{ EVENT_END_BATTLE, EVENT_SKILL_UP, EVENT_LEVEL_UP, EVENT_QUEST, };
	struct sEVENT_INFO : public sHD_BASE
	{
		THREAD_MEMPOOL_H( CsC_AttackProp::sEVENT_INFO )
		sEVENT_INFO(){ s_nBaseType = HD_EVENT; }
		
		eEVENT_TYPE		s_eEventType;

		union
		{
			int				s_nValue1;			// 레벨
			int				s_nLevel;
			int				s_nQuestIDX;
		};
		
		union
		{
			int				s_nValue2;
			int				s_nPoint;			// 포인트
			int				s_nQuestCondition;
		};		

		union
		{
			UINT			s_nUnSignValue1;	// 경험치
			UINT			s_nExp;
			UINT			s_nQuestValue;
		};

		union
		{
			UINT			s_nUnSignValue2;
			UINT			s_nEvoIndex;		// 해당 진화체
		};
	};

protected:
	std::list< sHD_BASE* >		m_listHDProcess;
	sDAMAGE_INFO*				m_pLastPartnerDamageInfo;

#ifdef LEVELUP_EFFECT_LOOP
	DWORD				m_nTickTime;
#endif

public:	
	void			InsertHit( sHIT_INFO* pSrcHitInfo );
	void			InsertMultiHit(sHIT_INFO* pSrcHitInfo);
	sDAMAGE_INFO*	InsertDamage( sDAMAGE_INFO* pSrcDamageInfo );
	void			InsertDamageEnd();

	void			InsertEvent( eEVENT_TYPE eType, int nValue1, int nValue2, UINT nUnsignValue1, UINT nUnsignValue2 );
	void			DeleteHitEvent();
	void			DeleteDemageEvent();
	//void			InsertEndBattle();
	//void			InsertSkillUp( int nLevel, UINT nExp, int nSkillPoint );
	//void			InsertLevelUp( int nLevel );

	bool			IsSkillProcess();
	bool			IsAttack_SkillProcess();
	bool			IsConditionProcess();

	void			AbsoluteSkillProcess();

	void			ClearDamageValue();

#ifdef LEVELUP_EFFECT_LOOP
	std::list< sHD_BASE* >		GetEventList() { return m_listHDProcess; }
#endif

public:
	static float	GetNormalRangeVelocity(){ return 700.0f; }
	static float	GetNormalRangeAccelVelocity(){ return 1200.0f; }
	static float	CalRangeHitTime( float fStartVelocity, float fAccelVelocity, float fDist ){ return ( sqrt( fStartVelocity*fStartVelocity + fAccelVelocity*fDist*2.0f ) - fStartVelocity )/fAccelVelocity; }
};


struct sRECV_HITTER_INFO
{
	sRECV_HITTER_INFO(){}

	UINT							s_nUID;
	CsC_AttackProp::eHIT_TYPE		s_HitType;

	int								s_nSkillIndex;
	bool							s_bRangeAttack;
	DWORD							s_AniID;
	float							s_fAniAccelRate;	
	// 데미지 반사 버프 스킬 추가_14.05.14		chu8820
	int								s_nReflectType;	// 데미지 반사 버프 대상 공격 시 버프 클래스

	UINT							s_nSkillCode;	// 디지몬 메모리 스킬 idx가 -1일때에만 스킬 코드를 저장.
	void		CalInfo( CsC_AvObject* pHitter );
};

struct sRECV_TARGET_INFO
{
	sRECV_TARGET_INFO(){ memset( s_nValue, 0, sizeof( int )*5 ); }

	UINT							s_nUID;	
	CsC_AttackProp::eDAMAGE_TYPE	s_DamageType;
	int								s_nNumEffectType;

	int								s_nValue[ 5 ];				// 데미지, HP, DS, MHP, MDS
};

