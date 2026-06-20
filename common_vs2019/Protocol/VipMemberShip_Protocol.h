#pragma once

#include "VipMemberShip_Define.h"
#include <windows.h>

class C2GS_REQ_VIP_SYSTEM_DATA
{
public:
	C2GS_REQ_VIP_SYSTEM_DATA():nProtocol(pCashShop::VIPAutoPayment){};
	~C2GS_REQ_VIP_SYSTEM_DATA() {};

	WORD GetProtocol(){return nProtocol;}
public:
	WORD				nProtocol;
};

class GS2C_RECV_VIP_SYSTEM_DATA
{
public:
	GS2C_RECV_VIP_SYSTEM_DATA():nProtocol(pCashShop::VIPAutoPayment),bIsVip(false), unVipEndTime(0) {};
	~GS2C_RECV_VIP_SYSTEM_DATA() {};

	WORD GetProtocol(){return nProtocol;}

public:
	WORD				nProtocol;
	bool				bIsVip;
	u4					unVipEndTime;
};