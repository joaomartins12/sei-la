
#pragma once

#include "pGame.h"
#include "pSync.h"
#include "plength.h"


class cPacket;
struct NEW_DUNGEON_INFO
{
	uint nDungeonKey;
	uint nInsMapTeam;
};
/*
//클라이언트 => 맵서버
struct CG_GAME_INITDATA_REQ
{
	void push(cPacket &pack);
	void pop(cPacket &pack);
};

//클라이언트 => 맵서버
struct GC_GAME_INITDATA_RES
{
	void push(cPacket &pack);
	void pop(cPacket &pack);
};


//맵서버 => 코어
struct SVR_GC_GAME_INITDATA_REQ
{
	void push(cPacket &pack);
	void pop(cPacket &pack);
};
*/

//맵서버 <= 코어
struct SVR_CG_GAME_INITDATA_RES
{
	void push(cPacket& pack);
	void pop(cPacket& pack);

	uint nIDX;
	char szID[Language::pLength::id + 1];
	uint nClass;
	nSync::Pos nDungeonPos;

	int64 dbIDX;
	int nType;
	char szName[Language::pLength::name + 1];

	uint nExp;
	u2 m_nLevel;
	u2 nStatPoint;
};


////예시
//{
//	SVR_CG_GAME_INITDATA_REQ req = { 0, };
//	.. 구조체에 데이터 채우고
//
//	cPacket pack;
//	req.push(pack);	//패킷채움
//	send(pack);
//
//
//	SVR_CG_GAME_INITDATA_RES res = { 0, };
//
//	cPacket pack;
//	res.pop(pack);	//패켓에서 구조체로 데이터 가져옴	
//}