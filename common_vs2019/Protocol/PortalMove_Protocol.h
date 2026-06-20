#pragma once
#include "PortalMove_Define.h"
#include <minwindef.h>
#include "common_vs2019/pServer.h"
#include "common_vs2019/pInsMap.h"


class GS2C_RECV_SELECT_PORTAL_FAILURE
{
public:
	GS2C_RECV_SELECT_PORTAL_FAILURE()
		:nProtocol(pSvr::SelectPortalFailure) , nResult(0)
	{
	}
	~GS2C_RECV_SELECT_PORTAL_FAILURE(){};

	WORD GetProtocol(){return nProtocol;}
public:
	WORD				nProtocol;

	n4 nResult;
};

class GS2C_RECV_DUNGEON_FAIL
{
public:
	GS2C_RECV_DUNGEON_FAIL()
		:nProtocol(pInsMap::DungeonFail) , nResult(0)
	{
	}
	~GS2C_RECV_DUNGEON_FAIL(){};

	WORD GetProtocol(){return nProtocol;}
public:
	WORD				nProtocol;

	n4 nResult;
};