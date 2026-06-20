
#pragma once 

namespace nsEFFECT
{
	enum eLIVE										/**< 루프여부 */
	{
		LIVE_ONCE		=	0x00000000,			/**< 한번 */
		LIVE_LOOP		=	0x00000001,			/**< 루프 */
		LIVE_MASK		=	0x0000000f,
	};
	// 동작 형태
	enum ePOS										/**< 위치 여부 */
	{
		POS_NORMAL		=	0x00000000,			/**< 그자리에서 나와서 계속 존재 */
		POS_CHARPOS		=	0x00000010,			/**< 케릭터위치를 따라감 */
		POS_SHOT		=	0x00000020,			/**< 원거리용 - 외부사용금지 */		
		POS_LEVEL		=	0x00000030,			/**< 레벨이펙트전용 별도로 위치 지정해주고있음 - 외부사용금지 */	
		POS_AURA		=	0x00000040,				/*	캐릭터 위치(AURA 전용) chu8820*/
		POS_LEVEL_MID		=	0x00000050,			/**< 강화수치에 따른 갯수 변화를 위해 사용 chu8820 */	
		POS_LEVEL_RIGHT		=	0x00000060,			/**< 강화수치에 따른 갯수 변화를 위해 사용 chu8820 */	
		POS_LEVEL_LEFT		=	0x00000070,			/**< 강화수치에 따른 갯수 변화를 위해 사용 chu8820 */	
		POS_MASK		=	0x000000f0,
	};
	enum eDIR
	{
		DIR_NONE			=	0x00000000,		/**< 방향성 업데이트 안됨 */	
		DIR_CHAR			=	0x00000100,		/**< 매프레임 부모 방향으로 업데이트 */
		DIR_CAMERA_AXIS_Z	=	0x00000200,		/**< 매프레임 Z축만 카메라방향으로 회전 */	
		DIR_CHAR_ALL		=	0x00000300,		/**< 매프레임 캐릭터의 모든방향으로 업데이트 */	
		DIR_MASK			=	0x00000f00,
	};
	enum eOFFSET
	{
		OFFSET_NONE		=	0x00000000,			/**< 오프셋 사용 안함 */
		OFFSET_USE		=	0x00001000,			/**< 오프셋 사용 */
		OFFSET_MASK		=	0x0000f000,
	};

	enum eSTART_POS									/**< 시작 위치 */
	{
		SPOS_CHARWORLD		=	0x00000000,		/**< 케릭터 월드위치 */
		SPOS_BOUND_CENTER	=	0x00010000,		/**< 겜브리오 중심점 */
		SPOS_HALF_THEIGHT	=	0x00020000,		/**< 툴에서 저장된 높이의 반 */
		SPOS_THEIGHT		=	0x00030000,		/**< 툴에서 저장된 높이 */
		SPOS_CHAIN			=	0x00040000,		/**< 연속되는 위치에서 시작 */
		SPOS_MASK			=	0x000f0000,
	};

	enum eEvnetMake
	{
		EVENT_NONE		=  0x00000000,		/**< 이벤트 계속사용 */
		EVENT_ONCE		=  0x00100000,		/**< 애니메이션 시작시 이벤트 한번만사용 */
		EVENT_ONLY_ONE	=  0x00200000,		/**< 애니메이션과 관계없이 무조선 이벤트 한번만사용 */
		EVENT_TYPE_MASK	=  0x00f00000,		
	};

	enum eEvnetDEL
	{
		EVENT_DEL_NONE		=  0x00000000,		/**< 이벤트 자체 종료후 삭제 */
		EVENT_DEL_ANI		=  0x01000000,		/**< 애니메이션 변경시 같이 삭제 */		
		EVENT_DEL_MASK		=  0x0f000000,		
	};
}

namespace nsSTATIC_EVENT
{
	enum eDY_EFFECT{
		DYE_NORMAL_ATTACK = 0,
		DYE_ATTACK_CLASH_FILE,
		DYE_NORMAL_ATTACK_FILE,

		DYE_RANGE_ATTACK_START = 100,
		DYE_RANGE_ATTACK_FILE,		
		DYE_RANGE_ATTACK_END,
	};	

	enum eDY_SOUND{
		// 순서 바뀌자 말고 POWER 뒤에 멀 추가 할거라면 ApplyModelData 참조해서 고치자
		DYS_DMG_ATT_L = 0,
		DYS_DMG_ATT_M,
		DYS_DMG_ATT_H,
		DYS_DMG_ATT_METAL,
		DYS_DMG_ATT_THRUST,
		DYS_DMG_ATT_WIP,
		DYS_DMG_ATT_STICK,
		DYS_DMG_ATT_POWER,

		DYS_T_Foot_N_01,
		DYS_T_Foot_N_02,

		DYS_LANGUAGE_SOUND,
		DYS_LANGUAGE_COMMON,

		DYS_USER_ONLY,
		DYS_USER_ONLY_LANGUAGE_SOUND,
		DYS_USER_ONLY_LANGUAGE_COMMON,

		DYS_RANGE_ATTACK_START = 100,
		DYS_RANGE_ATTACK_CLASH,		
	};

	enum eDY_ETC
	{
		DYETC_STOP_FRAME,		// 사용되지 않음
		DYETC_SHADER,
	};
}	

class CsC_LoadSequence : public NiMemObject
{
public:

	struct sTEXTKEY_SHADER : public NiMemObject
	{
		enum eSHADER_TYPE{ ST_NONE, ST_ALPHATEX, ST_BLUR };

		THREAD_MEMPOOL_H( CsC_LoadSequence::sTEXTKEY_SHADER );

		sTEXTKEY_SHADER()
		{
			Reset();
		}
		~sTEXTKEY_SHADER() {};

		void			Reset()
		{ 
			memset(s_cApplyObjectName,0,sizeof(char)*32);
			s_eShaderType = ST_NONE; 
			s_nValue1 = 0; 
			s_fValue1 = 0;
			s_fValue2 = 0;
			s_fValue3 = 0; 
		}

		char			s_cApplyObjectName[ 32 ];
		int				s_eShaderType;

		int				s_nValue1;
		float			s_fValue1;
		float			s_fValue2;
		float			s_fValue3;
	};

	struct sTEXTKEY_EVENT : public NiMemObject
	{
		enum eEVENT_TYPE{ ET_NONE, ET_E_FILE, ET_S_FILE, ET_DE_STATIC, ET_DS_STATIC, };

		// 재생 타입
		THREAD_MEMPOOL_H( CsC_LoadSequence::sTEXTKEY_EVENT );

		sTEXTKEY_EVENT():s_fEventTime(0.0f),s_eType(ET_NONE),s_nStaticIndex(0),s_dwPlag(0),s_vOffset(NiPoint3::ZERO)
			,s_fEffectScale(0.0f),s_bParentScale(false),s_fFadeoutTime(0.0f),s_vValue1(NiPoint3::ZERO),s_vValue2(NiPoint3::ZERO)
		{
			memset( s_cText, 0, sizeof(char)*MAX_TEXTKEY_LEN );
		}	
		~sTEXTKEY_EVENT() {};

		float			s_fEventTime;
		eEVENT_TYPE		s_eType;
		int				s_nStaticIndex;
		DWORD			s_dwPlag;
		NiPoint3		s_vOffset;
		char			s_cText[ MAX_TEXTKEY_LEN ];
		float			s_fEffectScale;
		bool			s_bParentScale;

		float			s_fFadeoutTime;
		NiPoint3		s_vValue1;
		NiPoint3		s_vValue2;		
	};

	struct sSEQUENCE_INFO : public NiMemObject
	{
		THREAD_MEMPOOL_H( CsC_LoadSequence::sSEQUENCE_INFO )

		sSEQUENCE_INFO()
		{ 
			s_dwSequenceID = 0; s_fEndTime = 0.0f; s_nUseStopFrameCount = 0;
			s_nNAtt_Count = 0; s_nLast_NAtt_SoundIndex = 0; s_nLoopCount = 0; 
		}
		~sSEQUENCE_INFO() {};

		void		Delete();		
		
		DWORD						s_dwSequenceID;
		float						s_fEndTime;
		NiTimeController::CycleType	s_eCyleType;

		int							s_nLoopCount;

		int							s_nUseStopFrameCount;

		// 공격 정보 관련
		int							s_nNAtt_Count;
		//int							s_nNAtt_EffectIndexArray[ MAX_NORAML_ATTACK_EVENT_COUNT ];
		int							s_nLast_NAtt_SoundIndex;		

		CsVectorPB< int >				s_vpNAtt_EffectIndexArray;
		CsVectorPB< sTEXTKEY_EVENT* >	s_vpTextKeyEvent;		
		CsVectorPB< sTEXTKEY_SHADER* >	s_vpTextKeyShader;
	};

public:
	CsC_LoadSequence();
	virtual ~CsC_LoadSequence();

protected:
	std::map< DWORD, sSEQUENCE_INFO* >	m_mapSequence;
	bool								m_bApplyModel;
	CsCriticalSection					m_CS;
	DWORD								m_dwModelID;

public:
	void		Delete();
	void		Init( NiActorManager* pActorMng, DWORD dwModelID );
	void		ResetApplyModelData( DWORD dwModelID );
	void		ApplyModelData( DWORD dwModel );							// 모델데이터 쪽에서 정보를 가져 온다.
	DWORD		GetModelID(){ return m_dwModelID; }

	bool				IsSequence( DWORD dwAniID ){ return  ( m_mapSequence.find( dwAniID ) != m_mapSequence.end() ); }
	sSEQUENCE_INFO*		GetSequence( DWORD dwAniID );
	std::map< DWORD, sSEQUENCE_INFO* >*	GetSequenceMap(){ return &m_mapSequence; }


protected:
	void		_ApplyModel_Event( CsC_LoadSequence* pParent, sSEQUENCE_INFO* pSeq, CsModelData::sSEQUENCE* pModelSeq );
	void		_ApplyModel_Shader( CsC_LoadSequence* pParent, sSEQUENCE_INFO* pSeq, CsModelData::sSEQUENCE* pModelSeq );
};
