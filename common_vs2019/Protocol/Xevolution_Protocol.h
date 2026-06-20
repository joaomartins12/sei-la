#pragma once
#include "Xevolution_Define.h"




/************************************************************************/
/* X시스템 게이지 전송													*/
/************************************************************************/

class GS2C_RECV_XGuage_Update
{
public:
	GS2C_RECV_XGuage_Update(): nProtocol(pGameTwo::XGuageUpdate)
	{}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	u4 nXGuage;
	u2 nCrystal;
};


/************************************************************************/
/* X시스템 게이지 최대량 전송											*/
/************************************************************************/

class GS2C_RECV_XGuage_Max_Update
{
public:
	GS2C_RECV_XGuage_Max_Update(): nProtocol(pGameTwo::XGuageMaxUpdate)
	{}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	u4 nXGuageMax;
	u2 nCrystalMax;
};



/************************************************************************/
/* X게이지->크리스탈 전환												*/
/************************************************************************/

class C2GS_SEND_XGuage_to_Crystal
{
public:
	C2GS_SEND_XGuage_to_Crystal(): nProtocol(pGameTwo::XGToCrystal)
	{}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;
};

class GS2C_RECV_XGuage_to_Crystal
{
public:
	GS2C_RECV_XGuage_to_Crystal(): nProtocol(pGameTwo::XGToCrystal)
	{}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	n4 nResult;
};


/************************************************************************/
/* 크리스탈->X게이지 전환												*/
/************************************************************************/

class C2GS_SEND_Crystal_to_XGuage
{
public:
	C2GS_SEND_Crystal_to_XGuage(): nProtocol(pGameTwo::CrystalToXG)
	{}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;
};

class GS2C_RECV_Crystal_to_XGuage
{
public:
	GS2C_RECV_Crystal_to_XGuage(): nProtocol(pGameTwo::CrystalToXG)
	{}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	n4 nResult;
};
