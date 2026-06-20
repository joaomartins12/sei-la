#pragma once
#include "nScope.h"
namespace pBuff
{
	enum
	{
		CreateBuff = nScope::Buff,		
		BuffChange,
		BuffClear,
		BuffCheck,
		DieClear,
		ManualRemove,
		BuffSyncInit,
		UpdateLimitTime,
		AllClear,
		ReturnFail,
		DotDamageInfo,
		AbnormalDamage,
		BuffSkill,        // 버프스킬 보낼때
		Stun,			//몬스터가 스턴걸렸을때
		VIPAutoPaymentBuffUpdate,
	};
};

namespace nBuff
{	
	enum BUFF_FAIL_TYPE
	{	
		ITEM_BUFF_FAIL = 1,
		SKILL_BUFF_FAIL,
		MANUL_REMOVE_FAIL,	// 버프 수동 제거 실패
	};

	enum BUFF_TIME_TYPE
	{
		Flat_Time	= 1,		// 정액제
		Elapse_Time,			// 종량제
		LogIn_Time,				// 로그인 시간 동안만 유지
		NotCoreSave_Time,		// 데이터베이스 저장안하는 인스턴트 버프
		VIPAutoPayment_Time,    // 멤버십 자동 결제 버프		
	};

	enum	// CORE UPDATE 처리 타입
	{
		BUFF_EXPIRE = 0,		// 만료
		BUFF_UPDATE,			// 시간 업데이트
		BUFF_DIE_CLEAR,			// 캐릭터 죽음 처리
	};

	enum BUFF_DURATION_TYPE	// 버프 지속 타입
	{
		DIE_CLEAR = 1,
		DIE_NONE,
	};

	enum
	{
		BUFF_MAXHP = 3,			// MAX HP 확장
		BUFF_MAXDS,				// MAX DS 확장
		BUFF_AP,				// 공격력 증가
		BUFF_CA,				// 크리티컬 대미지 증가
		BUFF_DP,				// 방어력 증가
		BUFF_EV,				// 회피력 증가
		BUFF_MS,				// 이동속도 증가
		BUFF_AS,				// 공격속도 증가
		BUFF_AR,				// 공격사거리 증가
		BUFF_HT,				// 적중도 증가
		BUFF_FP,				// 피로도 수치 증가
		BUFF_FS,				// 친밀도 증가
		BUFF_EXP,				// 경험치
		BUFF_POWERAPPLYRATE,	// 소켓 적용 능력치 향상
		BUFF_EVENTSCALE,		// 디지몬 스케일 변화
		BUFF_NA,				// 자연속성 (모두)
		BUFF_PS,				// 테이머 대표 스킬결과값
		BUFF_DC,				// 디지몬 DS 소모량
		BUFF_DE,				// 디지몬 습득 경험치량		
		BUFF_CR,				// 크리티컬 확률 증가
		BUFF_RP,				// 사망 패널티
		BUFF_RR,				// 선공몹 인식 거리		
		BUFF_SKILL_COOLDOWN,	// 스킬 쿨타임 감소
		BUFF_DS_RECOVER_TIME,	// DS 자연 회복력 상승
		BUFF_HP_RECOVER_TIME,	// HP 자연 회복력 상승
		MAX_BUFF_APPLY_TYPE = 28,
	};

	enum BUFF_APPLY_RETURN_VALUE
	{
		BUFF_APPLY_LEVEL_LOW	= -6,	// 적용 대상 레벨이 낮습니다.
		BUFF_APLLY_MAX_DEBUFF	= -5,	// 이미 디버프 개수가 최대
		BUFF_APPLY_MAX_BUFF		= -4,	// 이미 버프 개수가 최대 
		BUFF_APPLY_HIGHLEVEL	= -3,	// 상위 레벨 버프가 적용 되어 있음
		BUFF_APPLY_ALREADY		= -2,	// 이미 적용되어 있음
		BUFF_APPLY_FAIL			= -1,	// 버프 적용 실패
		BUFF_APPLY_CHANGE,
		BUFF_APPLY_SUCCESS = 1,

		BUFF_APPLY_NOT_REMOVE	= 100,	// 수동 제거 할 수 없는 버프
		BUFF_APPLY_NOT_FOUND,				// 버프가 없음, 이미 제거된 버프일 수도 있음
	};

	enum BUFF_TYPE	// BUFF & DEBUFF 적용 대상
	{
		APPLY_NONE = 0,
		APPLY_BUFF,
		APPLY_DEBUFF,
		APPLY_CLIENT_SYSTEMBUFF,	// 클라이언트에 시스템 버프로 표시 되는 경우
	};

	enum	// 시스템 버프 타입
	{
		PCBANG_BUFF,
		PLAYNET_BUFF,
		MACRO_BUFF,
		MACRO_DEFENCE_BUFF,
		GSP_MOVE_UP_BUFF,
		MAX_SYSTEM_BUFF = 6,
	};

	enum BUFF_APPLY_TARGET
	{
		PROCESS_TAMER = 0,
		PROCESS_DIGIMON,
		PROCESS_SHARE,
		PROCESS_DONE,	// 없음
	};
	// 현재 gsp 사용중이기 때문에 제거하면 안됨 ( 추후 제거 요망 )
	enum FAIL_TYPE
	{
		APPLY_FAIL_CODE_ALREADY_HIGH_LEVEL = 1,		// 이미 상위 레벨이 적용 되어 있음
		APPLY_FAIL_CODE_TARGET_LEVEL_ISLOW,			// 대상의 레벨이 낮아 버프 적용할수 없음
		APPLY_FAIL_CODE_NOT_MANUAL_REMOVE_BUFF,		// 수동제거 할수 없는 버프를 제거 요청한 경우
		APPLY_FAIL_CODE_NOT_MANUAL_NOT_FOUND,		// 수동제거 요청한 버프가 존재하지 않음 이미 만료되었을수도 있음
		APPLY_FAIL_CODE_TARGET_NOT_FOUND,			// 사용 대상이 잘못 되었습니다.
		APPLY_FAIL_CODE_ERROR,						// 내부 체크 에러
	};

	enum DotAttribute
	{
		DOT_PM = 0,
		DOT_PA,
		DOT_PB,
		DOT_APPLY,
		DOT_CLASS,
		DOT_CLASSLEVEL,

		MAX_DOT_ATTRIBUTE,
	};
	enum Achieve
	{
		ACHIEVE_BUFF_CLASS = 999,
	};
};
// 버프 상태 Sync 값 ( 서버에서 사용 )
namespace nStateSync
{
	enum
	{
		LV1 = 0x00000001,
		LV2 = 0x00000002,
		LV3 = 0x00000004,
		LV4 = 0x00000008,
	};
};