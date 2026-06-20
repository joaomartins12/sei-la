#pragma once

#include "PlayerInfo_Define.h"

class GS2C_RECV_TAMER_ITEM_EQUIP
{
public:
	GS2C_RECV_TAMER_ITEM_EQUIP(): nProtocol(pTamer::EquipItem),m_nTamerIDX(0)
		,m_nEquipSlot(0),m_nItemType(0),m_nEndTime(0),m_nChangeCostumeNo(0)
	{}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	u2 m_nTamerIDX;
	u1 m_nEquipSlot;
	u4 m_nItemType;
	u4 m_nEndTime;
	u4 m_nChangeCostumeNo;
};
