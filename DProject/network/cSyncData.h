

#pragma once

#include "common_vs2019/pType.h"
#include "common_vs2019/pSync.h"
#include "common_vs2019/pLength.h"

class cSyncDummy;
class cSyncOut;
class cSyncDelete;
class cSyncTamer;
class cSyncItem;
class cSyncNpc;
class cSyncMonster;
class cSyncDigimon;

class cSyncUnit
{
public:
	cSyncUnit(void) { }
   ~cSyncUnit(void) { }

public:
	uint m_nSyncType; // nSync::In, nSync::New, nSync::Out, nSync::Delete

	cType m_Type;
	nSync::Pos m_Pos;

public:
	uint GetUID(void) { return m_Type.m_nUID; }


	// 메모리 풀
protected:
	//static CsMemPool< cSyncDummy >		m_mpSyncDummy;
	//static CsMemPool< cSyncOut >		m_mpSyncOut;
	//static CsMemPool< cSyncDelete >		m_mpSyncDelete;
	//static CsMemPool< cSyncTamer >		m_mpSyncTamer;
	//static CsMemPool< cSyncItem >		m_mpSyncItem;
	//static CsMemPool< cSyncNpc >		m_mpSyncNpc;
	//static CsMemPool< cSyncMonster >	m_mpSyncMonster;
	//static CsMemPool< cSyncDigimon >	m_mpSyncDigimon;
	////static CsMemPool< cSyncTempChar >	m_mpSyncTempChar;

public:
	static void			DeleteUnit( cSyncUnit* pUnit );
	static cSyncUnit*	NewUnit( uint nSyncType, uint nClassType );
};


class cSyncDummy : public cSyncUnit // 쓰레드 큐의 더미 유닛
{
public:
	cSyncDummy(void) { }
	~cSyncDummy(void) { }
public:
};

class cSyncOut : public cSyncUnit //nSync::Out 처리시 사용
{
public:
	cSyncOut(void) { }
   ~cSyncOut(void) { }
public:
};

class cSyncDelete : public cSyncUnit //nSync::Delete 처리시 사용
{
public:
	cSyncDelete(void) { }
	~cSyncDelete(void) { }
public:
};

class cSyncTamer : public cSyncUnit
{
public:
	cSyncTamer(void) { }
   ~cSyncTamer(void) { }

public:
	uint m_nIDX;
	TCHAR m_szName[Language::pLength::name+1];
	float m_fDirect;

	ushort m_nMoveSpeed;
	ushort m_nCondition;

};

class cSyncItem : public cSyncUnit
{
public:
	cSyncItem(void) { }
   ~cSyncItem(void) { }

public:
};

class cSyncNpc : public cSyncUnit
{
public:
	cSyncNpc(void) { }
	~cSyncNpc(void) { }

public:
};

class cSyncMonster : public cSyncUnit
{
public:
	cSyncMonster(void) { }
   ~cSyncMonster(void) { }

public:
	ushort m_nHP;
	ushort m_nMaxHP;

	ushort m_nCondition;
	ushort m_nMoveSpeed;
};



class cSyncDigimon : public cSyncUnit
{
public:
	cSyncDigimon(void) { };
   ~cSyncDigimon(void) { };

public:
	uint m_nDBIDX;

	wchar m_szName[Language::pLength::name+1];
	short m_nTamerUID;
	float m_fDirect;

	ushort m_nHP;
	ushort m_nMaxHP;

	ushort m_nCondition;
	ushort m_nMoveSpeed;


};





