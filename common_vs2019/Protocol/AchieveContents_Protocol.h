#pragma once
#include "AchieveContents_Define.h"
#include <minwindef.h>

class GS2C_RECV_DISPTITLE
{
public:
	GS2C_RECV_DISPTITLE()
	{
		dispTitle = 0;
	}

public:	
	DWORD dispTitle;
};

class GS2C_RECV_RANKACHIEVE
{
public:
	GS2C_RECV_RANKACHIEVE()
	{
		rankAchieve = 0;
	}

public:	
	u4 rankAchieve;
};

class GS2C_RECV_CHECKTYPE
{
public:
	GS2C_RECV_CHECKTYPE()
	{
		nType   = 0;
		nValue1 = 0;
		nValue2 = 0;
	}

public:	
	int nType;
	int nValue1;
	int nValue2;
};