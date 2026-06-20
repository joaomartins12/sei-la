#pragma once

#include "common_vs2019/Protocol/DigimonTranscendence_Result.h"

enum eDigimonTranscendenceFlag
{
	eGeneralChargeEXP = 1, // 일반 충전
	eHighChargeEXP = 2, // 고급 충전
};

struct stTranscendenceItem
{
	stTranscendenceItem():m_uItemType(0),m_u2ItemPos(0),m_u2ItemCount(0) {}
	~stTranscendenceItem() {}

	u4 m_uItemType;      // 초월 재료 아이템 타입
	u2 m_u2ItemPos;      // 초월 재료 아이템 위치
	u2 m_u2ItemCount;    // 초월 재료 아이템 개수
};

namespace nsTransExp
{
	static n4 ItemRate (const n4 nItemMin, const n4 nItemMax)
	{
		if (nItemMin == nItemMax) return nItemMin;
		else					  return nItemMin + rand() % (nItemMax - nItemMin);
	}

	static double MaterialAddExp( n4 const& nMaterialDigimonLv,		// 재료 디지몬의 레벨
						   n4 const& nEnchantLv,				// 재료 디지몬의 강화 레벨
						   n4 const& nEnchentDefaultCorrect,	// 테이블 상에 적용된 기본 경험치 강화 보정값
						   n4 const& nGrowthCorrect,			// 디지몬 단수 보정 값( Transcend_Value 시트 참조)
						   n4 const& nExpCorrect )				// 대상 디지몬에 따른 경험치 값 (Transcend_Evo 테이블의 경험치 적용 비율)
	{
		// 인첸트 총 레벨이 서버에서 1부터 시작하기 때문에 1을 뺌
		u8 DefaultExp = 250 + nMaterialDigimonLv + ( (nEnchantLv - 1) * nEnchentDefaultCorrect);
		float fAddExp = (DefaultExp * nGrowthCorrect) * (nExpCorrect * 0.01f);
		return ceil( fAddExp );// 소수점 올림
	}
}

namespace nsTransCost
{
	static n8 TranscendenceCost(const n8 n8BaseTransCost,	 // 기본 초월 비용
						 const u8 u8NowTransExp,			 // 현재 초월 경험치
						 const u8 u8TransExpMax)             // 초월 경험치 Max
	{
		float fAddExp = n8BaseTransCost * (2 - (u8NowTransExp / (float)u8TransExpMax));
		return ceil( fAddExp );// 소수점 올림
	}
}