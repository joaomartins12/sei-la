

#pragma once


#include "pHatch.h"
#include "nlib/base.h"


class cHatchSource
{
public:
	cHatchSource(void) { }
   ~cHatchSource(void) { }

public:
	uint GetIDX(void) { return m_nIDX; }

public:
	uint m_nIDX;			// 디지타마 타입
	uint m_nDigimonType;	// 용병 디지몬 타입

	uint m_nEvoMask;		// 진화체 on/off 유무

	uint m_nItemType[5];	// 부화시 필요한 디지몬 데이타
	uint m_nItemCount[5];	// 부화시 필요한 디지몬 데이타 갯수

public:
#ifdef JHB_ADD_INTERMEDIATE_DATA_20140522
	uint m_nSuccessRate[10];	// 성공 확률
	uint m_nFailureRate[10];	// 용병 디지타마 보존, 입력데이타 삭제
	// 나머지는 용병 디지타마 제거, 입력데이타 제거
#else
	uint m_nSuccessRate[5];	// 성공 확률
	uint m_nFailureRate[5];	// 용병 디지타마 보존, 입력데이타 삭제
							// 나머지는 용병 디지타마 제거, 입력데이타 제거
#endif


};




