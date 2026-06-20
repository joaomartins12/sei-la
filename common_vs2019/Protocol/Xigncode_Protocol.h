#pragma once
#include "Xigncode_Define.h"
#include <minwindef.h> 



// C -> GS
class C2GS_SEND_XignCodePacket
{
public:
	C2GS_SEND_XignCodePacket(): nProtocol(pGameTwo::XignCodeCheck), nSize(0)
	{}
	~C2GS_SEND_XignCodePacket(){};

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;
	char szPacketData[XIGNCODE_PACKET_SIZE];
	u4 nSize;
};


// GS -> C
class GS2C_RECV_XignCodePacket
{
public:
	GS2C_RECV_XignCodePacket(): nProtocol(pGameTwo::XignCodeCheck) , nSize(0)
	{}
	~GS2C_RECV_XignCodePacket(){};

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;
	char szPacketData[XIGNCODE_PACKET_SIZE];
	u4 nSize;
};

