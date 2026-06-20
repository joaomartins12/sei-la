#pragma once

#define dfCREATE_DIGIMONTRANSCENDENCE_IDXKEY(EVOIDX, TERMSGROWTH) ((EVOIDX) << 8) | (TERMSGROWTH)
#define dfDIGIMONTRANSCENDENCE_IDXKEY_TO_EVOIDX(IDXKEY) (IDXKEY) >> 8

enum eTranscendenceValue
{
	eNotFind3Magnification = 0,						// 불일치 디지몬 3단 배율
	eNotFind4Magnification = 1,						// 불일치 디지몬 4단 배율
	eNotFind5Magnification = 2,						// 불일치 디지몬 5단 배율
	eFind3Magnification = 3,						// 일치 디지몬 3단 배율
	eFind4Magnification = 4,						// 일치 디지몬 4단 배율
	eFind5Magnification = 5,						// 일치 디지몬 5단 배율
	eTranscendenceSuccessMagnification = 6,			// 초월 성공 배율
	eTranscendenceGreatSuccessMagnification = 7,	// 초월 대성공 배율
	eTranscendenceSuccessRate = 8,					// 초월 성공 확률
	eTranscendenceGreatSuccessRate = 9,				// 초월 대성공 확률
	eBaseExpReinforcementCorrectionValue = 10,		// 기본 경험치 강화 관련 보정값
};

class cDigimonTranscendenceInfo
{
public:
	u4 m_uEvoIdx;
	u1 m_u1TermsGrowth;
	u1 m_u1TargetGrowth;
	u2 m_u2TermsLevel;
	u2 m_u2TermsScale;
	u2 m_u2TermsDigiclone;
	u2 m_u2UseDigimonEvoStartLevel;
	u2 m_u2UseDigimonEvoEndLevel;
	u2 m_u2UseDigimonHatchLvMin;
	u2 m_u2UseDigimonHatchLvMax;
	n8 m_n8TransCost;
	u8 m_u8ExpMax;

public:
	cDigimonTranscendenceInfo() { memset(this, 0, sizeof(cDigimonTranscendenceInfo)); }
	~cDigimonTranscendenceInfo() {}

public:
	u8 GetIDX() { return dfCREATE_DIGIMONTRANSCENDENCE_IDXKEY(m_uEvoIdx, m_u1TermsGrowth); }
};



class cDigimonTranscendenceEvo
{
public:
	u4 m_uDigimonIdx;
	u1 m_u1UseItemType;
	u2 m_u2ItemTypeID;
	u2 m_u2ItemConsume;
	u2 m_u2ItemMin;
	u2 m_u2ItemMax;

public:
	cDigimonTranscendenceEvo() { memset(this, 0, sizeof(cDigimonTranscendenceEvo)); }
	~cDigimonTranscendenceEvo() {}

public:
	u8 GetIDX() { return dfCREATE_DIGIMONTRANSCENDENCE_IDXKEY(m_uDigimonIdx, m_u1UseItemType); }
};



class cDigimonTranscendenceValue
{
public:
	u2 m_u2Idx;
	float m_fApplyValue;
	float m_fMaxValue;

public:
	cDigimonTranscendenceValue() { memset(this, 0, sizeof(cDigimonTranscendenceValue)); }
	~cDigimonTranscendenceValue() {}

public:
	u2 GetIDX() { return m_u2Idx; }
};