#pragma once
#define XIGNCODE_PACKET_SIZE 1024

enum XIGNCODE_RESULT
{
	OK = 0,
	USER_NOT_FOUND,
};

struct stXigncodeWaitPacket
{
	n4 nUserIdx;
	char szData[XIGNCODE_PACKET_SIZE];
	u4 nSize;
	u1 nTryCnt;
};