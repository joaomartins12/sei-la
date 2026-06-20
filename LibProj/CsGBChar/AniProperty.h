
#pragma once 

class CsC_AniProp : public CsC_Property
{
	THREAD_MEMPOOL_H( CsC_AniProp )

public:
	static bool				g_bUpdateActor;


protected:
	NiActorManagerPtr		m_pActorMng;

	float					m_fAniTime;
	float					m_fAniAccelRate;
	DWORD					m_dwCurAniID;	
	int						m_nBoneLODLevel;

	bool					m_bOldViewSight;

	CsC_AniProp_Sequence*	m_pSequence;

	CsCriticalSection		s_CS;


	// 애니메이션 가중치 상수값
	float					m_fAniConstantTime_Walk;
	float					m_fAniConstantTime_Run;
	float					m_fAniConstantTime_ARun;

public:
	CsC_AniProp();
	~CsC_AniProp();

	virtual void			Delete();
	virtual void			ChangeKFM();
	void					Init( CsC_AvObject* pParent );
	virtual void			CheckModelComp();

	void					Update( float fDeltaTime );

	void					SetAnimation( DWORD dwAniID, bool bUseStopFrame = false, float fAccelRate = 1.0f );
	void					SetAniAccelRate( float fRate ){ m_fAniAccelRate = fRate; }
	float					GetAniAccelRate(){ return m_fAniAccelRate; }

	void					ResetAnimation();
	float					GetAniTime(){ return m_fAniTime; }
	DWORD					GetAnimationID(){ return m_dwCurAniID; }	
	NiActorManager*			GetActorMng(){ return m_pActorMng; }
	CsC_AniProp_Sequence*	GetSequenceMng(){ return m_pSequence; }
	bool					IsBarcodeCardAni();
	bool					IsMoveObjectAni( DWORD dwAni );

	float					GetAniConstantTime();
	float					GetAniConstantTime( float fMoveSpeed );

protected:
	void					_UpdateAnimation( float fDeltaTime );	
	
	//==========================================================================
	//	CsC_AniProp_Sequence
	//==========================================================================
public:
	float		CalAniAccelRate( DWORD dwAniID, float fAniEndTime );	
	bool		GetNAttEventIndex( DWORD dwAniID, CsVectorPB< int >** ppOutArray, int& nArrayCount );

	float		CalEventTime( DWORD dwAniID, int nEventIndex, float fAniAccelRate );	

	bool		IsSequence( DWORD dwAniID ){ return m_pSequence != NULL ? m_pSequence->IsSequence( dwAniID ) : false; }

	//==========================================================================
	//	Set ActorMng
	//==========================================================================
public:
	void		SetActorMng( NiActorManagerPtr pActorMng, CsC_LoadSequence* pLoadSequence );
	void		CalAniConstant( CsC_LoadSequence* pLoadSequence );
	bool		IsKFM(){ return ( m_pSequence != NULL ); }

	//==========================================================================
	//	Idle Animation
	//==========================================================================
protected:
	float		m_fIdleAniTime;
	bool		m_bUseIdleAni;
protected:
	void		_UpdateIdleAni( float fDeltaTime );		
public:
	void		SetUseIdleAni( bool bUse ){ m_bUseIdleAni = bUse; }

	//==========================================================================
	//	StopFrame
	//==========================================================================
protected:
	bool		m_bUseStopFrame;
public:
	void		SetStopFrame( bool bUseStopFrame ){ m_bUseStopFrame = bUseStopFrame; }	
	float		GetActorMngStartFrame(){ assert_cs( m_pActorMng->GetSequence( m_dwCurAniID ) ); return m_pActorMng->GetSequence( m_dwCurAniID )->GetStartTime(); }

	//==========================================================================
	//	Ani Enable State
	//==========================================================================
public:
	bool		IsEnableNormalAttack();
	bool		IsEnableDieAni();
	bool		IsEnableSkill();
	ANI::eTYPE	IsEnableDamage();
	ANI::eTYPE	IsEnableEvade();
	ANI::eTYPE	IsEnableBlock();
	ANI::eTYPE	IsEnableKnockBack();
	bool		IsEnableAttackMove();
	bool		IsEnableEvol();
	bool		IsEnableRide();

	bool		IsDamage();
};