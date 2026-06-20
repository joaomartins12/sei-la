#pragma once
#include "DigimonSkill_Define.h"
#include "DigimonSkill_Result.h"
#include "../cEvoUnit.h"

class C2GS_SEND_DIGIMON_SKILL_LIMIT_OPEN
{
public:
	C2GS_SEND_DIGIMON_SKILL_LIMIT_OPEN(): nProtocol(pDigimon::DigimonSkillLimitOpen), nItemPos(0), nDigimonEvoArray(0),nItemType(0)
	{}
	~C2GS_SEND_DIGIMON_SKILL_LIMIT_OPEN(){}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;
	u4 nItemPos;			//확장 아이템 위치
	u4 nItemType;			//아이템테이블 고유번호
	u4 nDigimonEvoArray;	//디지몬 진화 단계
};


class GS2C_RECV_DIGIMON_SKILL_LIMIT_OPEN
{
public:
	GS2C_RECV_DIGIMON_SKILL_LIMIT_OPEN(): nProtocol(pDigimon::DigimonSkillLimitOpen), nResult(0), nDigimonEvoArray(0),nItemPos(0),nItemType(0)
	{
	}
	~GS2C_RECV_DIGIMON_SKILL_LIMIT_OPEN(){}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	u4 nResult;	//DigimonSkill_Rusult  ->   nsSkillLimitOpenResult::
	u4 nDigimonEvoArray;	//확장성공한 진화단계(보내준것과 동일)
	cEvoUnit cEvo;
	u4 nItemPos;			//확장 아이템 위치
	u4 nItemType;			//아이템테이블 고유번호
};

class GS2C_SEND_DIGIMON_SKILL_COOL_TIME
{
public:
	GS2C_SEND_DIGIMON_SKILL_COOL_TIME(): nProtocol(pDigimon::UpdateDigimonSkillCoolTime),nDigimonSkillCount(0),u2IdxDigimon(0),uSkillType(0),uSkillCoolTimeTS(0)
	{}
	~GS2C_SEND_DIGIMON_SKILL_COOL_TIME(){}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	u2 u2IdxDigimon; //디지몬 서버 유니크 인덱스
	u4 uEvoType;
	n4 nDigimonSkillCount; //밑에 2개 변수가 한묶음, 한묶음의 Count
	u4 uSkillType;
	u4 uSkillCoolTimeTS;
};