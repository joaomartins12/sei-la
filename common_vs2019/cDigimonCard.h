#pragma once
#include "nlib/base.h"
#include "common/plength.h"

enum eCardType
{
	Normal,
	Hero
};

#pragma pack(push,4)

class cCard
{
public:
	cCard(n1 nCardType) { m_nCardType = nCardType; Clear();};
	virtual ~cCard() {};
	virtual void Clear() {
		m_nType = 0;
		m_nDBIDX = 0;
		m_nLevel = 0;
		m_nExp = 0;
	};
public:
	void SetType(n4 nType) { m_nType = nType;};
	n4 GetType() { return m_nType; };

	void SetDBIDX(n8 nDBIDX){ m_nDBIDX = nDBIDX;};
	n8 GetDBIDX() { return m_nDBIDX; };

	void SetLevel(n2 nLevel){ m_nLevel = nLevel;};
	n2 GetLevel() { return m_nLevel; };

	void SetExp(n4 nExp){ m_nExp = nExp;};
	n4 GetExp() { return m_nExp; };

public:	//DB는 signed다
	n4 m_nType;
	n8 m_nDBIDX;
	n2 m_nLevel;
	n4 m_nExp;
	n1 m_nCardType;
};

class cDigimonCard :public cCard
{
public:
	cDigimonCard() :cCard(eCardType::Normal)
	{};
	virtual ~cDigimonCard(){};
	void Clear()
	{
		cCard::Clear();
	}
};

class cHeroCard :public cCard
{
public:
	cHeroCard():cCard(eCardType::Hero){		
		Clear();
	};
	virtual ~cHeroCard(){};
	void Clear(){
		m_nAT = 0;
		m_nDF = 0;
		m_nCR = 0;
		m_nAS = 0;
		m_nEV = 0;
		m_nAC = 0;
		m_nScale = 0;
		memset(m_szName ,0, Language::pLength::name+1);
		cCard::Clear();
	};

public:
	n2 m_nAT;		//공격력
	n2 m_nDF;		//방어력
	n2 m_nCR;		//크리티컬확률
	n2 m_nAS;		//공격속도(강화아직없음)
	n2 m_nEV;		//회피율
	n2 m_nAC;		//명중률(강화아직없음)

	n2 m_nScale;

	n4 m_nEvoState;	//진화 단계 , BitFlag

	char m_szName[Language::pLength::name+1];
};

#pragma pack(pop)