#pragma once

#include "MakeTacticsContents_Define.h"

class GS2C_RECV_MAKE_DIGITAMA
{
public:
	GS2C_RECV_MAKE_DIGITAMA():m_nEggType(0),m_nEggTradeLimitTime(0),m_nEggLevel(0),m_nBackupDisk(0),m_nBackupdiskTradeLimitTime(0)
	{}

	~GS2C_RECV_MAKE_DIGITAMA()
	{}

public:	
	u4 m_nEggType;
	n4 m_nEggTradeLimitTime;
	u4 m_nEggLevel;
	u4 m_nBackupDisk;
	n4 m_nBackupdiskTradeLimitTime;
};

class GS2C_RECV_MAKE_DIGITAMA_LV
{
public:
	GS2C_RECV_MAKE_DIGITAMA_LV()
	{
		m_nEggLevel = 0;
	}

public:
	u4 m_nEggLevel;
};

class GS2C_RECV_MAKE_DIGITAMA_SCLV
{
public:
	GS2C_RECV_MAKE_DIGITAMA_SCLV()
	{
		m_nSuccessLevel = 0;
	}

public:
	u4 m_nSuccessLevel;
};

class GS2C_RECV_MAKE_DIGITAMA_MINIGAME_START
{
public:
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_START()
		:nBarTime(0), 
		 nStage(0)
	{

	}
public:
	u2 nBarTime;	//바 채우는 시간
	u1 nStage;		//랜덤으로 가로 세로 버전 받기(아직 미구현)

};


class GS2C_RECV_MAKE_DIGITAMA_MINIGAME_CLICKRESULT
{
public:
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_CLICKRESULT()
		:nResult(false), 
		nBarIndex(0),
		nBarTime(0)
	{

	}
public:
	bool nResult;		//성공 실패 결과값
	u2 nBarIndex;		//바 인덱스
	u2 nBarTime;		//다음 바 채우는 시간

};

class GS2C_RECV_MAKE_DIGITAMA_MINIGAME_TIMEOUT
{
public:
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_TIMEOUT()
		:nBarTime(0)
	{

	}
public:
	u2 nBarTime;		//다음 바 채우는 시간

};

class GS2C_RECV_MAKE_DIGITAMA_MINIGAME_ERRORMESSAGE
{
public:
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_ERRORMESSAGE()
		:nErrorCode(0)
		,nSuccessCnt(0)
	{

	}
public:
	u1 nErrorCode;
	u2 nSuccessCnt;

};

class GS2C_RECV_MAKE_DIGITAMA_MINIGAME_ISPLAYABLE
{
public:
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_ISPLAYABLE()
		:nErrorCode(0)
	{

	}
public:
	u1 nErrorCode;
};

class GS2C_RECV_MAKE_DIGITAMA_MINIGAME_END
{
public:
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_END()
		:nSuccess(0)
	{

	}
public:
	u1 nSuccess;	//성공 횟수
};

class GS2C_RECV_MAKE_DIGITAMA_MINIGAME_INIT
{
public:
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_INIT()
		:nResult(false)
		, nSuccessCnt(0)
	{

	}
public:
	bool nResult;
	ushort nSuccessCnt;
};