#pragma once

#include "DigitamaContents_Define.h"

class GS2C_RECV_DIGITAMA_SCANITEM
{
public:
	GS2C_RECV_DIGITAMA_SCANITEM():m_nResult(0)
	{

	}

public:	
	WORD m_nResult;
	std::list<cItemInfo>	m_ItemList;
};

class GS2C_RECV_DIGITAMA_TO_MONEY_SUCCESS
{
public:
	GS2C_RECV_DIGITAMA_TO_MONEY_SUCCESS():nTotalPrice(0), nTamerMoney(0)
	{

	}

public:	
	u4 nTotalPrice;
	u8 nTamerMoney;
};