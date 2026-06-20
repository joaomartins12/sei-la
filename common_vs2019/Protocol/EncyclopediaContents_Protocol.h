#pragma once

#include "EncyclopediaContents_Define.h"
#include <list>
#include <minwindef.h>
#include "../cEvoUnit.h"

class GS2C_RECV_ENCYCLOPEDIA_OPEN
{
public:
	GS2C_RECV_ENCYCLOPEDIA_OPEN(){};
	~GS2C_RECV_ENCYCLOPEDIA_OPEN() {};

public:	
	std::list<sEncyclopediaOpendedData>	m_listOpendedData;
};

class GS2C_RECV_ENCYCLOPEDIA_EVOLINFO
{
public:
	GS2C_RECV_ENCYCLOPEDIA_EVOLINFO():iBaseDigimonID(0),iEvolSlot(0)
	{}
	~GS2C_RECV_ENCYCLOPEDIA_EVOLINFO() {};

public:	
	DWORD iBaseDigimonID;
	int	  iEvolSlot;
};

class GS2C_RECV_ENCYCLOPEDIA_USEDECK
{
public:
	GS2C_RECV_ENCYCLOPEDIA_USEDECK():iDeckIdx(0),bUse(false)
	{}
	~GS2C_RECV_ENCYCLOPEDIA_USEDECK() {};

public:	
	int  iDeckIdx;
	bool bUse;
};

class GS2C_RECV_ENCYCLOPEDIA_GETDECKIDX
{
public:
	GS2C_RECV_ENCYCLOPEDIA_GETDECKIDX():iDeckIdx(0)
	{}
	~GS2C_RECV_ENCYCLOPEDIA_GETDECKIDX() {};

public:	
	int  iDeckIdx;
};

class GS2C_RECV_ENCYCLOPEDIA_INCHANTRESULT
{
public:
	GS2C_RECV_ENCYCLOPEDIA_INCHANTRESULT():nDigimonID(0),nAPLevel(0),nDPLevel(0),nCALevel(0),nEVLevel(0),nHPLevel(0)
	{}
	~GS2C_RECV_ENCYCLOPEDIA_INCHANTRESULT() {};

public:	
	int nDigimonID;
	u2  nAPLevel;
	u2  nDPLevel;
	u2  nCALevel;
	u2  nEVLevel;
	u2	nHPLevel;
};

class GS2C_RECV_ENCYCLOPEDIA_OPTIDX
{
public:
	GS2C_RECV_ENCYCLOPEDIA_OPTIDX():nOptIdx(0)
	{}
	~GS2C_RECV_ENCYCLOPEDIA_OPTIDX() {};

public:	
	int nOptIdx;
};

class GS2C_RECV_ENCYCLOPEDIA_TACTICSHOUSE
{
public:
	GS2C_RECV_ENCYCLOPEDIA_TACTICSHOUSE():nDigimonID(0),nLevel(0)
	{}
	~GS2C_RECV_ENCYCLOPEDIA_TACTICSHOUSE() {};

public:	
	DWORD nDigimonID;
	u1	  nLevel;
};

class GS2C_RECV_ENCYCLOPEDIA_EVOLUTION
{
public:
	GS2C_RECV_ENCYCLOPEDIA_EVOLUTION():nDigimonID(0),nScale(0),nType(0)
	{
		memset(nEvoUnit, 0, sizeof( cEvoUnit ) * nLimit::EvoUnit);
	}

	~GS2C_RECV_ENCYCLOPEDIA_EVOLUTION() {};

public:	
	DWORD		nDigimonID;
	u2			nScale;
	cEvoUnit	nEvoUnit[nLimit::EvoUnit];
	int			nType;
};

class GS2C_RECV_ENCYCLOPEDIA_BISREWARD
{
public:
	GS2C_RECV_ENCYCLOPEDIA_BISREWARD():nDigimonID(0)
	{}
	~GS2C_RECV_ENCYCLOPEDIA_BISREWARD() {};

public:	
	DWORD nDigimonID;
};

class GS2C_RECV_ENCYCLOPEDIA_SCALE
{
public:
	GS2C_RECV_ENCYCLOPEDIA_SCALE():nDigimonID(0),nScale(0)
	{}
	~GS2C_RECV_ENCYCLOPEDIA_SCALE() {};

public:	
	DWORD		nDigimonID;
	u2			nScale;
};