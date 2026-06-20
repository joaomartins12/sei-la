#pragma once

class cBuffSource
{
public:
	u1 m_nSystemType;			// 시스템 버프 유무
	u1 m_nBuffType;				// 버프 타입 1: 버프 2: 디버프
	u1 m_DurationType;			// 지속 타입으로 죽었을경우 사라지는가 여부
	u1 m_nTimeType;				// 시간 적용 타입 종량 / 정액 여부	
	u4 m_nSkillCode;			// 스킬 코드
	u4 m_nDigimonSkillCode;		// 디지몬 스킬 코드
	u2 m_nBuffIDX;				// 버프 IDX		
	u2 m_nBuffClass;			// 버프 클래스	
	u2 m_nBuffLowLevel;			// 버프 적용 최소 레벨
	u2 m_nRemove;				// 수동제거 가능한지 여부
	u2 m_nApplyBaseTime;		// 효과 적용 기준 시간
	u1 m_nTicDistance;			// 효과 적용 간격
	u1 m_nTargetDuplication;	// 단일 타겟 중복 적용 여부
	u1 m_nReleaseLv;			// 해제 레벨 ( 1 맞으면 풀린다 2: 스킬 & 아이템 풀린다 )
	u1 m_nStateLv;				// 상태 이상 레벨 ( 1 : 이동 불가 2: 이동, 아이템 사용 불가, 3: 이동, 아이템, 공격 불가 )
public:
	cBuffSource(void) {};
	~cBuffSource(void) {};
	u2 GetIDX() { return m_nBuffIDX; }					// 버프 코드
	u2 GetClass() { return m_nBuffClass; }				// 버프 클래스
	u1 GetDurationType() { return m_DurationType; }		// 지속 타입
	u1 GetTimeType() { return m_nTimeType; }			// 시간 적용 타입
	u2 GetApplyLowLevel() { return m_nBuffLowLevel; }	// 적용 최소 레벨
	u4 GetSkillCode() { return m_nSkillCode; }			// 연결 스킬 코드
	u1 GetSystemType() { return m_nSystemType; }		// 시스템 버프 유무
	u1 GetBuffType() { return m_nBuffType; }			// 버프 타입 1: 버프 2 : 디버프
	u4 GetDigimonSkillCode() { return m_nDigimonSkillCode; }	// 디지몬 스킬 코드
	bool IsRemove() { return m_nRemove == 1 ? true : false; }	// 삭제 가능한 버프 인지
	u1 GetDistanceTic() { return m_nTicDistance; }
	u2 GetApplyBaseTime() { return m_nApplyBaseTime; }
	bool GetTargetDuplication() { return m_nTargetDuplication == 1 ? true : false; }
	u1 GetReleaseLv() { return m_nReleaseLv; }
	u1 GetStateLv() { return m_nStateLv; }
};