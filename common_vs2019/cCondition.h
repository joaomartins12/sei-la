

#pragma once

#include "nlib/base.h"


#define d_nMaxConditionParameter 3
// d_nMaxConditionParameter 갯수 이내에서, 파라메타에 영향을 준다.

// buff, de-buff 처리
class cCondition
{
public:
	cCondition(void);
   ~cCondition(void);

public:
	enum eCondition
	{
		None,			//
		StopA,			// 공격을 받아도 움직이지 못함
		StopD,			// stop 상태 해제
		StunA,			// 공격을 받으면 풀림
		StunD,			// stun 상태 해제
//		Immune,			// 스킬 공격에 대한 내성 - 일반 공격만 가능해진다.
//		Reveal,			// 스킬 공격에 대한 내성 제거
		InvisibleA,		// 투명 캐릭
		InvisibleD,		// 투명 캐릭
		InvincibleA,	// 무적
		InvincibleD,	// 무적
	};

public:
	void OnUpdate(void);



private:
	uint m_nNextTick;	// 해당 상태를 한번더 적용하는 경우
	uint m_nEndTick;	// 해당 상태가 끝나는 경우

	uint m_nState; // eCondition 값이 들어감

	uint m_nP[d_nMaxConditionParameter];	// -1 이면 사용되지 아니함
	uint m_nV[d_nMaxConditionParameter];

};






