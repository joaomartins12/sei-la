#pragma once

class SEffectMgr : public NiMemObject
{
public:
	SEffectMgr(void);
	virtual ~SEffectMgr(void);

	enum eEFFECT_TYPE	// 이펙트 타입 , 클래스 ID, map의 배열 인덱스
	{
		ET_COLOR,	// 단순 색상 변경
		ET_OUTLINE,	// 외곽선
		ET_MAX,
	};

	enum eUPDATE_TYPE
	{ 
		UT_NONE,	// 아무것도 없음.
		UT_TIME,	// 시간 단위
		UT_FPS,		// 프레임 단위
		UT_INFINITY_TIME,	// 시간 단위 무한 시간
	};

	enum eDEFINE_COLOR
	{
		DC_NONE,
		DC_RED,
		DC_BLUE,
		DC_YELLOW,
		DC_WHITE,	// WHITE는 단순히 밝아지는 효과 일뿐
		DC_GREEN,
		DC_TEST,	// 테스트용
		DC_MAX,
	};

	enum eFORM_TYPE
	{
		// Color Type
		FT_LOOPDEEPSOFT,	// deep - soft 반복
		FT_ONECOLOR,		// 원 컬러로 변경			
		FT_BEGINDEEP,		// 점점 deep
		FT_BEGINSOFT,		// 점점 soft
		FT_LIGHTBEGINSOFT,	// 라이팅 값 사용 + deep시작-soft끝
		FT_FLICKER,			// 깜박거림
		// OutLine Type
		//FT_OUTLINE_ARMOR,	// 무적 전용 플래그
	};

	enum ePRIORITY_TYPE
	{
		PRIORITY_NONE		= 0x00,
		PRIORITY_CHANGE		= 0x01,	// 설정하지 않으면 무조건 변경됩니다.
		PRIORITY_RANKMODE	= 0x10,	// 테이블에서 읽은 우선 순위에 따라 변경됩니다.
		PRIORITY_UPDATE		= 0x20,	// 동일한 우선 순위일 때 변경합니다. 그렇지 않으면 갱신되지 않음.
	};

	struct st_SHADING_FLAG
	{
		DWORD	s_dwBuffID;			// 어떤 버프/디버프인지 번호를 찾아서 삭제시키기 위해 추가
		int		s_nUpdateType;		// 컬러 업데이트 방식
		int		s_nFormType;		// 지속 형태
		int		s_nColorType;		// 색상 선택
		int		s_nShaderType;		// eSTATE_TYPE - 셰이더 어미가 달라짐
		int		s_nEffectType;		// 이펙트 타입(외곽선 or 컬러)
		int		s_nPriorityType;	// 우선순위		

		int		s_nPriority;		// 우선순위 값

		st_SHADING_FLAG(){}
		st_SHADING_FLAG(int nFormType, int nColorType, int nShaderType, DWORD dwBuffID,
			int nUpdateType = UT_INFINITY_TIME,	int nEffectType = ET_COLOR,
			int nProrityType = PRIORITY_CHANGE, int nPriority = 9) : 
		s_nFormType(nFormType), s_nColorType(nColorType), s_nShaderType(nShaderType), 
			s_dwBuffID(dwBuffID), s_nUpdateType(nUpdateType),
			s_nEffectType(nEffectType), s_nPriorityType(nProrityType), s_nPriority(nPriority){}
	};

public:
	class CShaderEffect_Base
	{
	public:
		CShaderEffect_Base():m_pTarget(NULL),m_bShow(true){};
		virtual ~CShaderEffect_Base(){};

		st_SHADING_FLAG	m_stOption;			// 초기값
		CsC_AvObject*	m_pTarget;
		D3DXVECTOR3		m_vOriginColor;		// 초기 색상

		// UpdateTime
		float			m_fCurrent_Time;	// 경과 시간
		float			m_fEnd_Time;		// 종료 시기 FPS or TIME 

		float			m_fCycle_CurrentTime;// 사이클의 경과 시간
		float			m_fCycle_Time;		// 사이클 
		bool			m_bShow;

	public:
		virtual void Init(const st_SHADING_FLAG& stFlag, CsC_AvObject* pTarget,
			float fExit_Time, float fCycle);
		virtual void Update(float fDeltaTime) = 0;
		virtual void ConstantTable() = 0;	// 셰이더 세팅 값
		virtual void FormUpdate(float fDeltaTime) = 0;	// 특징 별 업데이트 방식
		virtual void Clear() = 0;	// 셰이더 값 원복
		virtual void _SetVariableColor(float fCycleTime) = 0;		// 변화 주기
			
		void Delete();

		// UpdateTime
		void _Update_Time(float fDeltaTime);
		void _Update_FPS();

		void _SetSwitchColor();							// 변경 색상 값
		void _SetGeomType();
	};


	class CShaderEffect_Color : public CShaderEffect_Base
	{
	public:
		CShaderEffect_Color():m_fFlag(0.0f) {};
		virtual ~CShaderEffect_Color() {};

		// 여기서부터는 상속으로
		// 기본 <-> 색상 변경
		D3DXVECTOR3		m_vVariableColor;		// 변화값
		D3DXVECTOR3		m_vCurrentColor;		// 현재 값

		D3DXVECTOR3		m_vAmbLight;
		float			m_fFlag;

	public:
		virtual void Init(const st_SHADING_FLAG& stFlag, CsC_AvObject* pTarget,
			float fExit_Time, float fCycle);
		virtual void Update(float fDeltaTime);
		virtual void ConstantTable();
		virtual void FormUpdate(float fDeltaTime);
		virtual void Clear();

		virtual void _SetVariableColor(float fCycleTime);		// 변화 주기
	};

	class CShaderEffect_OutLine : public CShaderEffect_Base
	{
	public:
		CShaderEffect_OutLine():m_fFlag(0.0f){};
		virtual ~CShaderEffect_OutLine() {};

		D3DXVECTOR3		m_vVariableColor;		// 변화값
		D3DXVECTOR3		m_vCurrentColor;		// 현재 값
		float			m_fFlag;

		virtual void Init(const st_SHADING_FLAG& stFlag, CsC_AvObject* pTarget,
			float fExit_Time, float fCycle);
		virtual void Update(float fDeltaTime);
		virtual void ConstantTable();
		virtual void FormUpdate(float fDeltaTime);
		virtual void Clear();
		virtual void _SetVariableColor(float fCycleTime);		// 변화 주기
	};

private:
	map<CsC_AvObject*, CShaderEffect_Base*>		m_ShaderEffectMap[ET_MAX];	// 포인터 구분
	bool _Confirm(CsC_AvObject* ptr, const st_SHADING_FLAG& stFlag);

	typedef map<CsC_AvObject*, CShaderEffect_Base*>	MAP;
	typedef map<CsC_AvObject*, CShaderEffect_Base*>::iterator		MAP_IT;
	typedef map<CsC_AvObject*, CShaderEffect_Base*>::const_iterator	MAP_CIT;
	

public:
	void AddSEffect(float fEnd,	// 지속 시간
		CsC_AvObject*	pObj,			// 타겟
		const st_SHADING_FLAG&	stFlag,	// 플래그 정보
		float fCycle = 1.f);

public:
	void _PreUpdate();					// 시간 만료된 것 제거
	void Update(float fDeltaTime);
	void Render(CsC_AvObject* pObj);	// 없으면 기본 렌더로 처리함
	void OutLineRender();				// 리스트들의 외곽선 렌더(CCharResMng::Render_PostChar())
	bool DeleteSEffect( CsC_AvObject * pObj);	// 이펙트 삭제
	bool DeleteSEffect(CsC_AvObject* pObj, DWORD dwBuffID);		// cBuffData::ReleaseBuff(BuffID)

private:
	bool _DeleteSDuplicateEffect(CsC_AvObject* pObj, int nEffectType);	// AddSEffect->Confirm - 중복 추가 할때 호출

};

extern SEffectMgr*	g_pSEffectMgr;
