#pragma once

#include "common_vs2019/pGame.h"
#include "common_vs2019/Protocol/Inventory_Result.h"

namespace nInventoryInfo
{
	enum eInventoryFlag
	{
		eINVEN = 0,
		eWAREHOUSE,
		eSHARESTASH,
		eMAX,
	};
}

class C2GS_SEND_INVENTORY
{
public:
	C2GS_SEND_INVENTORY() :nProtocol(pGameTwo::InventoryInfo), m_u1InventoryFlag(nInventoryInfo::eINVEN)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD nProtocol;
	u1   m_u1InventoryFlag; //인벤 요청정보
};

class GS2C_RECV_INVENTORY
{
public:
	GS2C_RECV_INVENTORY() : nProtocol(pGameTwo::InventoryInfo), m_nResult(eINVENTORYRESULT::SUCCESS), m_n8Money(0), m_u1InventoryFlag(nInventoryInfo::eINVEN), m_u2InventorySlot(0)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD		nProtocol;
	n4          m_nResult;
	n8          m_n8Money;
	u1          m_u1InventoryFlag; //인벤 요청했던 정보
	u2			m_u2InventorySlot;
	std::list< cItemData > m_lItems;
};