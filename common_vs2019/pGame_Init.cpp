#include "pGame_Init.h"
#include "nlib/cPacket.h"
/*
void CG_GAME_INITDATA_REQ::push(cPacket &pack)
{

}

void CG_GAME_INITDATA_REQ::pop(cPacket &pack)
{


}

void GC_GAME_INITDATA_RES::push(cPacket &pack)
{

}
	
void GC_GAME_INITDATA_RES::pop(cPacket &pack)
{

}


void SVR_GC_GAME_INITDATA_REQ::push(cPacket &pack)
{

}
	
void SVR_GC_GAME_INITDATA_REQ::pop(cPacket &pack)
{

}
*/
void SVR_CG_GAME_INITDATA_RES::push(cPacket &pack)
{
	pack.newp(pGame::InitData);
	{
		pack.push(*this);
	}
	pack.endp(pGame::InitData);
}
	
void SVR_CG_GAME_INITDATA_RES::pop(cPacket &pack)
{
	pack.pop(*this);
}
