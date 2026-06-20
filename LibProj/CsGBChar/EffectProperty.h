
#pragma once 

class CsC_EffectObject;
class CsEffectSound;
class CsC_EffectProp : public CsC_Property
{
	THREAD_MEMPOOL_H( CsC_EffectProp )

public:
	static void				GlobalEffectProp( sCREATEINFO* pProp );

public:
	enum eLOOP_EFFECT{ LE_DLEVEL, LE_ENCHANT,
		 LE_SIZE, LE_PCBANG, LE_BOSS, LE_BATTLE_TEAM,
		LE_AURA, //오라 추가 chu8820
		LE_COSEFFECT,	//이펙트 코스튬 추가
#ifdef SDM_TAMER_XGUAGE_20180628
		LE_XJOGRESS,
#endif
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
		LE_DIGIVICE,
#endif
#ifdef SDM_TAMER_XGUAGE_20180628
		LE_XAI,
#endif
		LE_MAX_COUNT, };
public:
	CsC_EffectProp();
	~CsC_EffectProp();

	virtual void			Delete();
	virtual void			DeleteEffect( int nEffectIndex );
	virtual void			DeleteEffect( CsC_AvObject* pEffect );
	void					DeleteLoopEffect( eLOOP_EFFECT le ){ DeleteEffect( m_LoopEffectArray[ le ] ); }	
	void					DeleteEventEffect();

	bool					IsDelete();

	void					Init( CsC_AvObject* pParent );
	void					Reset();

	void					PreUpdate( float fDeltaTime );
	void					Render();
	void					RenderAbsolute();

	bool					IsLoopEffect( eLOOP_EFFECT le ){ return m_LoopEffectArray[ le ] != NULL; }
	CsC_AvObject*			AddLoopEffect( eLOOP_EFFECT le, char const* cEffectPath );	

	CsC_EffectObject*		AddEffect( char* cEffectPath, CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption, float fScale );
	//CsC_EffectObject*		AddEffect( const NiPoint3& kStartPos, char* cEffectPath, CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption, float fScale );
	CsC_EffectObject*		AddEffect( const NiPoint3& kStartPos, char* cEffectPath);
	CsC_EffectObject*		AddFireWorkEffect( char* cEffectPath, float fScale, DWORD dwPlag, bool bPreLoad = false );

#ifdef LEVELUP_EFFECT_LOOP
	CsC_EffectObject*		AddEffect( char* cEffectPath, float fScale, DWORD dwPlag, int eState, bool bPreLoad = false );
#endif
	CsC_EffectObject*		AddEffect( char* cEffectPath, float fScale, DWORD dwPlag, bool bPreLoad = false );
	CsC_EffectObject*		AddEffect( char* cEffectPath, NiPoint3 vPos, float fScale );
	CsC_EffectObject*		AddEffect_FT( char* cEffectPath ); //이펙트 사운드 툴에서 설정한 이펙트 붙일때만 사용
	CsC_EffectObject*		AddEffect_FT( char* cEffectPath, float fEffecScale );
	CsC_EffectObject*		AddEffect_FT_WithoutScale( char* cEffectPath, float fScale );
	int						GetEffectCount(){ return m_vpEffect.Size(); }
	CsC_AvObject*			GetEffect( int nIndex ){ return m_vpEffect[ nIndex ]; }


	//==========================================================================
	//	Effect
	//==========================================================================
protected:
	CsC_AvObject*					m_LoopEffectArray[ LE_MAX_COUNT ];
	CsVectorPB< CsC_AvObject* >		m_vpEffect; 
	
public:
	CsC_AvObject*					GetLoopEffect( eLOOP_EFFECT le );

protected:
	typedef	std::map< DWORD, CsC_AvObject* >				BUFF_EFF_MAP;
	typedef	std::map< DWORD, CsC_AvObject* >::iterator		BUFF_EFF_MAP_IT;

	BUFF_EFF_MAP		m_mapLoopBuffEffect;	// 버프ID / 객체

	CsC_AvObject* _GetLoopBuffEffect(DWORD dwBuffCode);

public:
	void AddBuffLoopEffect( char* cEffectPath, DWORD dwBuffCode, 
		DWORD dwFlag = nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER, 
		bool bPreLoad = false );
	bool DeleteBuffLoopEffect(DWORD dwBuffCode);

	// 몬스터 스킬 대상(징표), 바닥 이펙트 전용
public:
	typedef	std::map< DWORD, CsC_AvObject* >				SKILL_EFF_MAP;
	typedef	std::map< DWORD, CsC_AvObject* >::iterator		SKILL_EFF_MAP_IT;
protected:
	SKILL_EFF_MAP	m_mapLoopSkillEffect;

public:
	SKILL_EFF_MAP*	_GetLoopSkillEffectMap()	{ return &m_mapLoopSkillEffect; }
	CsC_AvObject*	_GetLoopSkillEffect(DWORD dwSkillIdx);
	void			AddSkillLoopEffect( char* cEffectPath, DWORD dwSkillIdx, 
										DWORD dwFlag = nsEFFECT::LIVE_LOOP | nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER, 
										bool bPreLoad = false, float fScale = 1.0f, float fLifeTime = 0.0f );

	bool			DeleteSkillLoopEffect(DWORD dwSkillIdx);

	//==========================================================================
	//	EffectSoundData
	//==========================================================================	
protected:
	struct ES_PLAY : public NiMemObject
	{
		ES_PLAY():s_pEffect(NULL),s_pEffectSoundData(NULL),s_nStep(0),s_fAccumTime(0.0f)
		{}

		CsC_AvObject*					s_pEffect;
		CsEffectSound*					s_pEffectSoundData;
		int								s_nStep;		
		float							s_fAccumTime;
	};

	CsVectorPB< ES_PLAY* >				m_vpEffectSound;	
	
};