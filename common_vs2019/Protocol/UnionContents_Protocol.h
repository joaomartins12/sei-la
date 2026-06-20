#pragma once

#include "UnionContents_Define.h"

class GS2C_RECV_ADD_UNIONITEM
{
public:
	GS2C_RECV_ADD_UNIONITEM()
	{
		ItemData.Clear();
	}

public:	
	cItemInfo ItemData;
};

class GS2C_RECV_SET_UNIONMONEY
{
public:
	GS2C_RECV_SET_UNIONMONEY()
	{
		money = 0;
	}

public:	
	u8 money;
};

class GS2C_RECV_SHOP_DATA
{
public:
	GS2C_RECV_SHOP_DATA()
	{
		iCharMapID = 0;
		iCharPos = NiPoint2::ZERO;
		iCharChannel = 0;
		iCharModelID = 0;
	}

public:	
	int		 iCharMapID;
	NiPoint2 iCharPos;
	int		 iCharChannel;
	int		 iCharModelID;
};

class GS2C_RECV_MODEL_ID
{
public:
	GS2C_RECV_MODEL_ID()
	{
		iCharModelID = 0;
	}

public:	
	int		 iCharModelID;
};