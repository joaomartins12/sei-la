#pragma once

BEGIN_NAME_TABLE

enum eTamerType
{
	eMasaru,	//최건우
	eTohma,		// 토우마
	eYoshino,	// 요시노
	eHanjiho,	// 한지호
	eTaichi,	// 신태일
	eMina,		// 이미나
	eMatt,		// 매튜
	eTakeru,	// 타케루(리키)
	eHikari,	// 신나리
	eSora,		// 한소라
	eJangHansol,// 장한솔
	eJeongseog,	// 정석
	eMax,
};

enum eTBaseStats
{
	eTamer_Hp,	// 생명력
	eTamer_DS,	// 디지 소울
	eTamer_MS,	// 이동 속도
	eTamer_DE,	// 방어력
	eTamer_AT,	// 공격력
	eTamer_HT,	// 적중률
	eTamer_Max
};

struct sTamerBaseStats
{
	sTamerBaseStats():nHP(0), nDS(0), nMS(0), nDE(0), nAT(0), nHT(0)
	{}
	int nHP;
	int nDS;
	int nMS;
	int nDE;
	int nAT;
	int nHT;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

enum eDAttackType
{
	eATT_Short,		// 근거리형
	eATT_Near,		// 원거리형
	eATT_Quick,		// 민첩형
	eATT_Defeder,	// 방어형
	eATT_Max,
};

enum eDBaseStats
{
	eDigimon_Hp,	// 생명력
	eDigimon_DS,	// 디지 소울
	eDigimon_DE,	// 방어력
	eDigimon_EV,	// 회피력
	eDigimon_CR,	// 크리티컬
	eDigimon_AT,	// 공격력
	eDigimon_HT,	// 적중률
	eDigimon_Max
};

struct sDigimonBaseStats
{
	sDigimonBaseStats():nHP(0), nDS(0), nDE(0), nEV(0), nCR(0), nAT(0), nHT(0)
	{}
	int nHP;
	int nDS;
	int nDE;
	int nEV;
	int nCR;
	int nAT;
	int nHT;
};

END_NAME_TABLE
