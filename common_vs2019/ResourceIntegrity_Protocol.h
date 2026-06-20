#pragma once
#include "pGame.h"
#include <minwindef.h>




/*--------------------------------------------
pGameTwo::ResourceIntegrity
C -> S
해당스테이지가 완료되면 전송된다
--------------------------------------------*/

struct C2GS_SEND_RESOURCE_INTEGRITY{
	C2GS_SEND_RESOURCE_INTEGRITY() : protocol(pGameTwo::ResourceIntegrity) , nResult(0), nDataSize(0)
	{
	}
	u1 nResult;
	u2 nDataSize; //MAX 65520
	//char data[nDataSize]


	WORD GetProtocol(){return protocol;}
private:
	WORD protocol;
};


/*--------------------------------------------
pGameTwo::ResourceIntegrity
S -> C
해당스테이지가 완료되면 전송된다
--------------------------------------------*/
struct GS2C_RECV_RESOURCE_INTEGRITY{
public:
	GS2C_RECV_RESOURCE_INTEGRITY() : protocol(pGameTwo::ResourceIntegrity) , nResult(0)
	{
	}

	u1 nResult;
	
	WORD GetProtocol(){return protocol;}
private:
	WORD protocol;
};