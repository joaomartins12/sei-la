
#pragma once 

#define DROP_KIND				12

class CsMonsterDrop
{
public:
	typedef	std::map< DWORD, CsMonsterDrop* >				MAP;
	typedef	std::map< DWORD, CsMonsterDrop* >::iterator		MAP_IT;

public:
	struct sINFO
	{
		DWORD				s_dwMonsterID;
		DWORD				s_nNormalDrop[ DROP_KIND ];
		DWORD				s_nExtraDrop[ DROP_KIND ];
	};

public:
	sINFO			m_Info;

public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

#define MONSTER_NAME_LEN		64
#define MONSTER_PEN_NAME_LEN	34

#define MONSTER_ICON_NAME_LEN	64

class CsMonster
{
public:
	typedef	std::map< DWORD, CsMonster* >				MAP;
	typedef	std::map< DWORD, CsMonster* >::iterator		MAP_IT;

public:
	struct sINFO
	{
		DWORD				s_dwMonsterID;
		DWORD				s_dwModelID;
		TCHAR				s_szName[ MONSTER_NAME_LEN ];
		TCHAR				s_szPenName[ MONSTER_PEN_NAME_LEN ];
		TCHAR				s_szTitle[ MONSTER_ICON_NAME_LEN ];
		USHORT				s_nLevel;
		USHORT				s_nEXP;

		USHORT				s_nBattle;

		int					s_nHP;
		int					s_nDS;

		USHORT				s_nDefPower;
		USHORT				s_nEvasion;
		USHORT				s_nMoveSpeed;
		USHORT				s_nWalkSpeed;
		USHORT				s_nCritcalRate;
		USHORT				s_nAttPower;
		USHORT				s_nAttSpeed;
		USHORT				s_nAttRange;
		USHORT				s_nHitRate;

		USHORT				s_nSight;
		USHORT				s_nHuntRange;	
		float				s_fScale;

		USHORT				s_nClass;
		USHORT				s_nIcon1;
		USHORT				s_nIcon2;
		USHORT				s_nIcon3;
		USHORT				s_nIcon4;
		USHORT				s_nIcon5;
		USHORT				s_nIcon6;
		USHORT				s_Exp_Min;				
		USHORT				s_Exp_Max;
	};
public:
	sINFO			m_Info;
	int				m_nCharSize;

public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );

	sINFO*			GetInfo(){ return &m_Info; }

	bool			IsBossType(){ return ( m_Info.s_nClass == 3 )||( m_Info.s_nClass == 4 )||( m_Info.s_nClass == 6 ); }	//	3: 보스 4: 정예보스 6: 레이드 보스
};


#define MAX_FACTOR_COUNT		3
class CsMonsterSkill
{
public:
	typedef	std::map< DWORD, CsMonsterSkill* >					MAP;
	typedef	std::map< DWORD, CsMonsterSkill* >::iterator		MAP_IT;

public:
	enum eTERM_TYPE{	//	스킬 사용 조건
		TERM_TYPE_NONE		= 0,
		TARGET_HP_PER_DOWN	= 1,	// 타겟 HP x% 이하
		TARGET_HP_PER_UP	= 2,	// 타겟 HP x% 이상
		TARGET_HP_VAL_DOWN	= 3,	// 타겟 HP x 이하
		TARGET_HP_VAL_UP	= 4,	// 타겟 HP x 이상

		OWN_HP_PER_DOWN		= 5,	// 자신 HP x% 이하
		OWN_HP_PER_UP		= 6,	// 자신 HP x% 이상
		OWN_HP_VAL_DOWN		= 7,	// 자신 HP x 이하
		OWN_HP_VAL_UP		= 8,	// 자신 HP x 이상

		TARGET_DS_PER_DOWN	= 9,	// 타겟 DS x% 이하
		TARGET_DS_PER_UP	= 10,	// 타겟 DS x% 이상
		TARGET_DS_VAL_DOWN	= 11,	// 타겟 DS x 이하
		TARGET_DS_VAL_UP	= 12,	// 타겟 DS x 이상

		OWN_DS_PER_DOWN		= 13,	// 자신 DS x% 이하
		OWN_DS_PER_UP		= 14,	// 자신 DS x% 이상
		OWN_DS_VAL_DOWN		= 15,	// 자신 DS x 이하
		OWN_DS_VAL_UP		= 16,	// 자신 DS x 이하
	};

	enum eEFFECT_TYPE{	//	스킬 효과
		EFFECT_TYPE_NONE	= 0,
		HP_PER_INCREASE		= 1,	// 현재 HP x% 증가
		HP_PER_DECREASE		= 2,	// 현재 HP x% 감소
		HP_VAL_INCREASE		= 3,	// HP x 증가
		HP_VAL_DECREASE		= 4,	// HP x 감소
		HP_MAX_INCREASE		= 5,	// HP 최대치 x% 증가
		HP_MAX_DECREASE		= 6,	// HP 최대치 x% 감소

		DS_PER_INCREASE		= 7,	// 현재 DS x% 증가
		DS_PER_DECREASE		= 8,	// 현재 DS x% 감소
		DS_VAL_INCREASE		= 9,	// DS x 증가
		DS_VAL_DECREASE		= 10,	// DS x 감소
		DS_MAX_INCREASE		= 11,	// DS 최대치 x% 증가
		DS_MAX_DECREASE		= 12,	// DS 최대치 x% 감소

		SUMMON_MONSTER		= 13,	// 몬스터 소환
		GROWTH				= 14,	// 성장( 데미지, 스케일 증가 )
		CALL_UP				= 15,	// 현재 맵의 생존몬스터 호출

		ASSEMBLE			= 16,	// 모아맞기		( 대상 수 만큼 데미지 분산 )
		DISPERSE			= 17,	// 흩어져맞기	( 대상 수 만큼 데미지 중첩 )
		ATTACH_SEED			= 18,	// 씨앗깔기		( 대상 디버프, 시간 경과 후 대상 위치 바닥 생성 )
		BERSERK				= 19,	// 광폭화		( 공략 시간 초과, 몬스터 능력치 증가 ) + 반사 추가

		CONTINUE_WIDE_ATTACK= 20,	// 연속 광역공격
		BUFF_OCCURE			= 21,	// 버프 발생

		Single_StackDeBuff_Attack = 22, //단일 타켓을 타격시 디버프 쌓임
		Region_Buff_Nesting	= 23,// 일정지역에 버프 쌓임
		Range_Buff_Nesting	= 24, // 일정범위에 버프 쌓임
		GatheringExt		= 25,		// 모아맞기(근/원딜 발사체 나가게 수정)
		DisperseExt			= 26,		// 흩어져맞기(근/원딜 발사체 나가게 수정)
	};

	enum eFACTOR_TYPE{	//	성장 효과 팩터
		FACTOR_TYPE_NONE	= 0,
		DP_INCREASE			= 21,	// 방어력 증가
		AP_INCREASE			= 29,	// 공격력 증가
		SCALE_INCREASE		= 41,	// 스케일 증가
	};

	struct sINFO
	{
		USHORT				s_nSkill_IDX;		// 인덱스 ( 1~ )
		DWORD				s_dwMonsterID;		// 몬스터 ID
		DWORD				s_dwCoolTime;		// 쿨타임
		USHORT				s_nCastTime;		// 캐스팅 타임

		USHORT				s_nCastCheck;		// 캐스팅 체크( 0 - 사용안함, 1 - 스킬 시전 중 이동 X )

		USHORT				s_nTarget_Cnt;		// 스킬 대상 수
		USHORT				s_nTarget_MinCnt;	// 스킬 대상 수 (최소)
		USHORT				s_nTarget_MaxCnt;	// 스킬 대상 수 (최대)

		USHORT				s_nUse_Terms;		// 스킬 사용 조건
		// 		USHORT				s_nStart_Terms;		// 스킬 시작 조건
		// 		USHORT				s_nEnd_Terms;		// 스킬 종료 조건

		USHORT				s_nSkillType;		// 스킬 타입 

		DWORD				s_dwEff_Val_Min;	// 효과 최소 수치
		DWORD				s_dwEff_Val_Max;	// 효과 최대 수치

		USHORT				s_nRangeIdx;		// 스킬범위 참조 인덱스( SkillTerms 테이블 인덱스 )

		DWORD				s_dwSequenceID;		// 스킬 애니 시퀀스 ID
		USHORT				s_nAni_Delay;		// 애니 딜레이
		USHORT				s_nValocity;		// 스킬 속도
		USHORT				s_nAccel;			// 스킬 가속도

		USHORT				s_nEff_Factor[ MAX_FACTOR_COUNT ];		// 효과 팩터
		DWORD				s_dwEff_Fact_Val[ MAX_FACTOR_COUNT ];	// 효과 팩터값*-
		UINT				s_nTalkId;			// 토크 ID

		UINT				s_nActiveType; //스킬 생성 타입 0=자신, 1=타켓, 2=좌표
		FLOAT				s_fNoticeTime; //예꼬 이펙트 시간
		char				s_szNoticeEff_Name[ MAX_FILENAME ];	// 예고 이펙트 파일 명.(폴더 포함)
	};
public:
	sINFO			m_Info;

public:
	void			Delete() {}
	void			Init( sINFO* pSrcInfo ) { memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsMonsterSkillTerms
{
public:
	typedef	std::map< DWORD, CsMonsterSkillTerms* >					MAP;
	typedef	std::map< DWORD, CsMonsterSkillTerms* >::iterator		MAP_IT;

public:
	struct sINFO
	{
		USHORT				s_nIDX;				// 인덱스 ( 1~ )
		USHORT				s_nDirection;		// 방향성
		UINT				s_nRange;			// 범위, 거리
		USHORT				s_nTargetingType;	// 타겟팅 타입
		USHORT				s_nRefCode;			// 참조 코드
	};
public:
	sINFO			m_Info;

public:
	void			Delete() {}
	void			Init( sINFO* pSrcInfo ) { memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};
