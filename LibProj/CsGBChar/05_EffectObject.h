
#pragma once

class CsC_EffectObject : public CsC_AvObject
{
	CsCRTTI_H
	THREAD_MEMPOOL_H( CsC_EffectObject )

public:
	enum eSTATE{	LIVE, DIE_READY, DIE,
	};
#ifdef LEVELUP_EFFECT_LOOP
	enum eLevelEffect { eNormal, eLeveUp, };
#endif

	CsC_EffectObject();
	~CsC_EffectObject();

protected:
	CsC_AvObject*		m_pParent;

	NiPoint3			m_vOffset;
	DWORD				m_dwPlag;
	float				m_fStartTime;
	float				m_fEndAniTime;
	float				m_fLifeTime;
	CsCoolTimeSeq		m_DieTimeSeq;

	eSTATE				m_eState;
#ifdef LEVELUP_EFFECT_LOOP
	eLevelEffect		m_eLevelEffect;
#endif

public:
	virtual void		Delete();
	virtual void		DeleteUpdate();

	virtual void		Init( CsC_AvObject* pParent, sCREATEINFO* pProp, CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption );
	//virtual void		InitForChainLightning( CsC_AvObject* pParent, sCREATEINFO* pProp, const NiPoint3& StartPos, CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption );
	virtual void		InitForChainEffect( CsC_AvObject* pParent, sCREATEINFO* pProp, const NiPoint3& StartPos );
	virtual void		Init( CsC_AvObject* pParent, sCREATEINFO* pProp, DWORD dwPlag );
	virtual void		Init( CsC_AvObject* pParent, NiPoint3 vPos, sCREATEINFO* pProp );

	virtual void		Update( float fDeltaTime, bool bAnimation = true );
	virtual void		Render();

	bool				CheckEffectModel( float fDeltaTime );
	
protected:	
	bool				_CheckEffectLive( float fAniTime );
	bool				_CheckLifeTime( float fDeltaTime );

	bool				_PreUpdate_Effect( float fDeltaTime );
	void				_UpdateClash( float fDeltaTime );
	void				UpdateChainEffect(float fDeltaTime);
	void				UpdateBaseTransform(const float& fDeltaTime, const NiPoint3& vDir );

	float				_CalVelocity( float fDeltaTime );

public:
	bool				IsEndEffect();

	float				GetEndTime() const;
	DWORD				GetEffectPlag();
	void				SetEffectPlag( DWORD dwPlag);
	void				SetOffset( NiPoint3 vOffset );
	void				SetWorldPos( NiPoint3 vPos );
	void				SetLifeTime(float fLifeTime);
	
#ifdef LEVELUP_EFFECT_LOOP
	eLevelEffect		GetLevelEffect() { return m_eLevelEffect; }
	void				SetLevelEffect( eLevelEffect eType ) { m_eLevelEffect = eType; }
#endif

protected:
	CsC_AvObject*										m_pHitTarget;
	CsC_AvObject*										m_pChainStartTarget;
	CsC_AvObject*										m_pChainEndTarget;
	CsC_AvObject*										m_pDefaultTest;
	NiNode												m_BaseTransform;

	NiPoint3											m_vTargetMissOffset;
	NiPoint3											m_vStartPos;
	NiPoint3											m_vSpline0;
	NiPoint3											m_vSpline1;
	float												m_fMaxScale;
	float												m_fTotalDist;
protected:
	float						_CalCurDist(){ return m_EventOption.s_fVelocity*m_fRangeAccumeTime + 0.5f*m_EventOption.s_fAccelVelocity*m_fRangeAccumeTime*m_fRangeAccumeTime; }

protected:
	float												m_fRangeAccumeTime;	
	float												m_fClashAccumeTime;
	float												m_fClashConstantTime;
	float												m_effectDelayTime;
	std::queue< CsC_LoadSequence::sTEXTKEY_EVENT* >		m_queueClashEffect;
	std::queue< CsC_LoadSequence::sTEXTKEY_EVENT* >		m_queueClashSound;
	CsC_AniProp_Sequence::sEVENT_OPTION					m_EventOption;


public:
	void				SetHitTarget( CsC_AvObject* pHitTarget );
	void				SetChainTarget(CsC_AvObject* pStartTarget , CsC_AvObject* pEndTarget );
	void				SetClashConstantTime( float fTime );
	void				SetDelayTime(const float fTime);
	void				AddClashEvent( CsC_LoadSequence::sTEXTKEY_EVENT* pEvent );
	void				AddClashSound( CsC_LoadSequence::sTEXTKEY_EVENT* pClashSound );
	float				LookAt(NiAVObject* pNode, const NiPoint3& kNormal);
};
