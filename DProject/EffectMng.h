
#pragma once 

//#define EFFECT_NEW				"Data/Effect/New.nif"
//#define EFFECT_ATTACK			"Data/Effect/Attack.nif"
//#define EFFECT_CRITICALATTACK	"Data/Effect/CriticalAttack.nif"
//#define EFFECT_DIGIMON			"Data/Effect/Digimon.nif"
//#define EFFECT_CRITICAL			"Data/Effect/Critical_K.nif"
//#define EFFECT_MISS				"Data/Effect/Miss_K.nif"

#define EFFECT_GATE				"Data/Effect/System/Digimon_Gate.nif"

#define EFFECT_ITEM_SACN_D		"System/Digitama_Position.nif"
#define EFFECT_ITEM_TACTICS_D	"System/BIGEGG_effect.nif"
#define EFFECT_T_EVOL			"System/Digisoul_Charge.nif"
#define EFFECT_SKILLUP			"System/skillup.nif"
#define EFFECT_CHARGE			"System/Digisoul_Charge.nif"
#define EFFECT_DG				"System/DG.nif"
#define EFFECT_FOOD				"System/Food.nif"
#define EFFECT_DEGENERATION		"System/Digimon_degeneration.nif"		// 퇴화
#define EFFECT_EVOL				"System/Digimon_Evo.nif"				// 진화
#define EFFECT_EVOL_JOINTPROGRESS	"System\\joint_progress\\joint_progress.nif"				// 진화
#define EFFECT_EVOL_SUSANOOMON		"System\\joint_progress\\joint_progress_s.nif"				// 진화
#define EFFECT_EVOL_EXAMON			"System\\joint_progress_01\\joint_progress_01.nif"				// 진화
#define EFFECT_TACTICS			"System/Digimon_Tactics.nif"			// 용병 교체
#define EFFECT_DIGIMON_LEVELUP	"System/levelup.nif"
#define EFFECT_TAMER_LEVELUP	"System/Levelup.nif"
#define EFFECT_DEAD				"System/Digimon_Dead.nif"
#define EFFECT_DEAD_CONSTANT	2.0f
#define EFFECT_CREATE_SCENE		"system\\CreateScene.nif"
#define EFFECT_CLOSED_SCENE		"system\\ClosedScene.nif"
#define EFFECT_BOOM				"Shinegrey\\fireattack.nif"				// 폭파

//오라이펙트
#define EFFECT_AURA_DIGI			"system\\T_Aura\\T_Aura_Digi_Set.nif"// 디지오라 시전 이펙트 chu8820

#define EFFECT_CROSSCOMBINE					"system\\Cross\\DigiCross_E_Red.nif"				// 폭파
#define EFFECT_CROSSCOMBINE_COMPLATE		"system\\Cross\\DigiCross_C_Red.nif"				// 폭파


class CEffectMng : public CsC_AvObject
{
public:
	CEffectMng();
	~CEffectMng();

	static void		GlobalInit();
	static void		GlobalShotDown();

public:
	void			Reset();		// 로딩시 호출

	void			Update( float fDeltaTime );
	void			Render();
	
	CsC_EffectObject*	AddEffect( char* cEffectPath, NiPoint3 vPos, float fScale );
	void				DeleteEffect( CsC_AvObject* pDelEffect );


	//================================================================================================================
	//
	//		포인트 구슬
	//
	//================================================================================================================
public:
	enum eMARBLE_TYPE
	{
		MT_EVOLUTION,
		MT_MAX_CNT,
	};
	struct sMARBLE_INFO
	{
		enum eSTEP{ CREATE, ON_TR, FALLOW, DIE };

		eMARBLE_TYPE	s_eType;
		eSTEP			s_eStep;
		CsC_AvObject*	s_pOwner;
		CsNodeObj		s_NodeObj;
		float			s_fAniTime;
		float			s_fOnTrTime;

		NiPoint3		s_vInitPos;
		NiPoint3		s_vTrPos;
		NiPoint3		s_vSpline1;
		NiPoint3		s_vSpline2;
		NiPoint3		s_vCurPos;
	};

protected:
	CsNodeObj					m_MarbleObj[ MT_MAX_CNT ];

	std::list< sMARBLE_INFO* >	m_listMarble;
	std::queue< sMARBLE_INFO* >	m_mpMarble[ MT_MAX_CNT ];

protected:
	void			_Update_Marble( float fDeltaTime );
	void			_Render_Marble();


	sMARBLE_INFO*	_NewMarble( eMARBLE_TYPE mt, NiPoint3 vSrc, CsC_AvObject* pOwner );
	void			_ResetMarble();
	void			_DelMarble( sMARBLE_INFO* pInfo );

public:	
	void			Init_Marble();
	void			Destroy_Marble();
	void			AddEffect_Marble( eMARBLE_TYPE mt, NiPoint3 vSrc, CsC_AvObject* pOwner, int nCnt );
};

extern CEffectMng*	g_pEffectMng;