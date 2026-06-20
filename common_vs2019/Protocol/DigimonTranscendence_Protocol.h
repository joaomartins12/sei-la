#pragma once

#include "common_vs2019/Protocol/DigimonTranscendence_Result.h"
#include "common_vs2019/Protocol/ItemDel_Define.h"
#include "common_vs2019/Protocol/DigimonTranscendence_Define.h"
#include "common_vs2019/pMaster.h"
#include "common_vs2019/pGame.h"
#include <list>
#include <minwindef.h>   //Fix WORD


class C2GS_SEND_DigimonTranscendenceChargeEXP
{
public:
	C2GS_SEND_DigimonTranscendenceChargeEXP()
		:nProtocol(pGameTwo::DigimonTranscendenceChargeEXP), m_u1VIPMemberMode(false), m_uNpcIDX(0), m_u1Flag(eDigimonTranscendenceFlag::eGeneralChargeEXP),
		m_u1DigimonTranscendencePos(0), m_u2DigimonCount(0)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD nProtocol;
	u1 m_u1VIPMemberMode;           // VIP Mode
	u4 m_uInvenActiveItemSlot;
	u4 m_uNpcIDX;                   // NPC
	u1 m_u1Flag;                    // 선택 (일반 충전, 고급 충전)
	u1 m_u1DigimonTranscendencePos; // 초월하는 디지몬 슬롯 번호 0, 1, 2, 3
	u2 m_u2DigimonCount;            // 초월 재료 디지몬 수
	std::list<u2> m_listDigimonPos; // 초월 재료 디지몬 위치
	u2 m_u2ItemCount;               // 초월 재료 아이템 수
	std::list<stTranscendenceItem> m_lItemList;
};

class GS2C_RECV_DigimonTranscendenceChargeEXP
{
public:
	GS2C_RECV_DigimonTranscendenceChargeEXP()
		: nProtocol(pGameTwo::DigimonTranscendenceChargeEXP), m_nResult(nsDigimonTranscendenceResult::Success), m_u1Flag(eDigimonTranscendenceFlag::eGeneralChargeEXP),
		m_u1DigimonTranscendencePos(0), m_u2DigimonCount(0), m_u2ItemCount(0), m_u2SuccessRate(0), m_u8ChargeEXP(0), m_u8TotalEXP(0)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD nProtocol;
	n4	 m_nResult;									// 결과 값
	u1	 m_u1Flag;									// 선택 (일반 충전, 고급 충전)
	u1 m_u1DigimonTranscendencePos;					// 초월하는 디지몬 슬롯 번호 0, 1, 2, 3
	u2   m_u2DigimonCount;							// 재료 디지몬 수
	std::list<u2> m_listDigimonPos;					// 재료 디지몬 위치
	u2 m_u2ItemCount;							    // 재료 아이템 수
	std::list< nsItemDel::sItemInfo > m_lItemList;  // 재료 아이템 정보
	u2 m_u2SuccessRate;                             // 초월 (성공, 대성공) 배율
	u8 m_u8ChargeEXP;								// 획득 경험치
	u8 m_u8TotalEXP;								// 총 경험치
};

class C2GS_SEND_DigimonTranscendence
{
public:
	C2GS_SEND_DigimonTranscendence()
		:nProtocol(pGameTwo::DigimonTranscendence)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD nProtocol;
	u1 m_u1VIPMemberMode;           // VIP Mode
	u4 m_uInvenActiveItemSlot;
	u4 m_uNpcIDX;                   // NPC
	u1 m_u1DigimonTranscendencePos; // 초월하는 디지몬 슬롯 번호 0, 1, 2, 3
	n8 m_n8Money;                   // 초월 비용
};

class GS2C_RECV_DigimonTranscendence
{
public:
	GS2C_RECV_DigimonTranscendence()
		: nProtocol(pGameTwo::DigimonTranscendence), m_nResult(nsDigimonTranscendenceResult::Success), m_u1DigimonTranscendencePos(0), m_u1HatchLevel(0), m_n8DigimonTranscendenceMoney(0), m_n8Money(0), m_u8Exp(0)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD nProtocol;
	n4	 m_nResult;
	u1	 m_u1DigimonTranscendencePos;   // 초월하는 디지몬 슬롯 번호 0, 1, 2, 3
	u1   m_u1HatchLevel;
	n8	 m_n8DigimonTranscendenceMoney; // 초월 요청한 비용
	n8	 m_n8Money;					    // 현재 소유 돈
	u8   m_u8Exp;						// 현재 경험치
};

// 치트키
class C2GS_SEND_MasterDigimonTranscendenceChargeEXP
{
public:
	C2GS_SEND_MasterDigimonTranscendenceChargeEXP()
		:nProtocol(pMaster::TranscendenceChangeChargeEXP), m_u8ChargeEXP(0)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD nProtocol;
	u8 m_u8ChargeEXP;
};

class GS2C_RECV_MasterDigimonTranscendenceChargeEXP
{
public:
	GS2C_RECV_MasterDigimonTranscendenceChargeEXP()
		: nProtocol(pMaster::TranscendenceChangeChargeEXP), m_nResult(nsDigimonTranscendenceResult::Success), m_u8ChargeEXP(0)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD nProtocol;
	n4	 m_nResult;
	u8	 m_u8ChargeEXP;
};