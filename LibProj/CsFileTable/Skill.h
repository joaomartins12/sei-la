
#pragma once 

class CsSkill
{
public:
	typedef	std::map< DWORD, CsSkill* >				MAP;
	typedef	std::map< DWORD, CsSkill* >::iterator	MAP_IT;

public:
	enum eSKILL_TYPE
	{
		ST_DIGIMON, ST_ITEM, ST_TAMER,
	};

	enum eICON_TYPE // 아
	{ 
		IT_CHANGE1 = 50000, IT_CHANGE2,
		IT_CHANGE3,	// 디지몬 교체스킬 3번
	};

	struct sINFO
	{
		struct sAPPLY
		{
			int			s_nA;
			int			s_nInvoke_Rate;		// 발동 확률
			int			s_nB;
			int			s_nC;
			
			USHORT		s_nBuffCode;		// 아이템 스킬 일 때는 버프코드, 테이머/디지몬 스킬 에서는 스킬 효과의 타겟
			USHORT		s_nID;
			SHORT		s_nIncrease_B_Point;
		};

		DWORD		s_dwID;
		TCHAR		s_szName[ SKILL_NAME_LEN ];
		TCHAR		s_szComment[ SKILL_COMMENT_LEN ];
		sAPPLY		s_Apply[ SKILL_APPLY_MAX_COUNT ];

		USHORT		s_nLevelupPoint;
		USHORT		s_nMaxLevel;

		USHORT		s_nAttributeType;
		USHORT		s_nNatureType;
		USHORT		s_nFamilyType;

		USHORT		s_nUseHP;
		USHORT		s_nUseDS;

		USHORT		s_nIcon;
		USHORT		s_nTarget;
		USHORT		s_nAttType;
		float		s_fAttRange;
		float		s_fAttRange_MinDmg;
		float		s_fAttRange_NorDmg;
		float		s_fAttRange_MaxDmg;
		USHORT		s_nAttSphere;

		// 타임
		float		s_fCastingTime;
		float		s_fDamageTime;
		USHORT		s_nDamageDay;
		float		s_nDistanceTime;

		float		s_fCooldownTime;
		USHORT		s_nCooldownDay;

		float		s_fSkill_Velocity;
		float		s_fSkill_Accel;

		USHORT		s_nSkillType;
		USHORT		s_nLimitLevel;
		USHORT		s_nSkillGroup;		// 메모리 스킬(디지몬 캐시 스킬) 중첩 확인 ??
		USHORT		s_nSkillRank;		// 메모리 스킬 능력( 상(3), 중(2), 하(1) )
		USHORT		s_nMemorySkill;		// 1~3이면 메모리 스킬(, 0이면 일반 스킬... 메모리 스킬은 삭제 할 수 있다.
		USHORT		s_nReq_Item;		// 메모리 스킬 사용 시 필요 메모리칩 개수
	};

protected:
	sINFO			m_Info;
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );

	sINFO*			GetInfo(){ return &m_Info; }

	bool			IsActive(){ return ( m_Info.s_nAttType != 4 )&&( m_Info.s_nAttType != 0 ); }
	bool			IsPasive(){ return ( m_Info.s_nAttType == 4 ); }

	bool			IsTamerBaseSkill() { return ( m_Info.s_dwID % 1000000 < 10 ); }
	bool			IsEmotionSkill() { return ( (m_Info.s_dwID % 1000000 > 3000) && (m_Info.s_dwID % 1000000 < 3100) ); }
	bool			IsTamerSkill() { return ( m_Info.s_dwID / 1000000 == 8 ); }
	bool			IsTamerUnicSkill ( DWORD dwTamerID ) { return ( m_Info.s_dwID % dwTamerID < 100000 ); }
	//특수스킬 추가 시 버프 표현 위해 수정 chu8820
	bool			IsDigimonSkill() { 
		if( m_Info.s_dwID / 100 == 21134 || m_Info.s_dwID / 100 == 21137 )// 페티메라몬, 시드몬 예외처리
		{
			return true;
		}
		else { return ( m_Info.s_dwID / 1000000 >= 3 && m_Info.s_dwID / 1000000 <= 7 ); }
	}
};


class CsTamerSkill
{
public:
	typedef	std::map< DWORD, CsTamerSkill* >				TSKILL_MAP;
	typedef	std::map< DWORD, CsTamerSkill* >::iterator		TSKILL_MAP_IT;

public:
	struct sTSkill_INFO
	{
		USHORT		s_nIndex;				// 인덱스

		DWORD		s_dwSkillCode;			// 스킬 코드 번호

		USHORT		s_nType;				// 스킬 타입 ( 1: 공격, 2: 버프 / 디버프, 3: 스크립트 스킬 )

		DWORD		s_dwFactor1;			// 스킬 타입에 따른 입력
		DWORD		s_dwFactor2;				

		DWORD		s_dwTamer_SeqID;		// 테이머 시퀀스 아이디
		DWORD		s_dwDigimon_SeqID;		// 디지몬 시퀀스 아이디
		
		USHORT		s_nUseState;			// 사용 가능 상태 ( 0: 항상 사용, 1: 전투 중 사용 불가 )

		USHORT		s_nUse_Are_Check;		// 지역설정 사용여부 ( 1: 스킬을 사용할 수 있는 지역을 설정하겠다, 0: 아무곳에서나 사용가능 )
		USHORT		s_nAvailable;			// 가능/불가능( 1: 해당 지역에서 사용 불가능, 2: 해당 지역에서 사용 가능)
	};

protected:
	sTSkill_INFO		m_TSkill_Info;		// 테이머 스킬 정보

public:
	sTSkill_INFO*		Get_TSkill_Info() { return &m_TSkill_Info; }

	void				TSkill_Init( sTSkill_INFO* pInfo );
};

class CsAreaCheck
{
public:
	typedef	std::map< DWORD, CsAreaCheck* >				AREACHECK_MAP;
	typedef	std::map< DWORD, CsAreaCheck* >::iterator	AREACHECK_MAP_IT;

public:
	struct sArea_Check
	{
		DWORD		s_dwIndex;			// 인덱스 (스킬 코드 번호)
		USHORT		s_nArea[30];		// 지역
	};

protected:
	sArea_Check		m_Area_Info;

public:
	sArea_Check*	Get_Area_Info() { return &m_Area_Info; }
	void			AreaCheck_Init( sArea_Check* pInfo );
};