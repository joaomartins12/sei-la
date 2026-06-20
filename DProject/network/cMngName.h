

#pragma once


#include "nlib/map.h"
#include "nlib/queue.h"
#include "common_vs2019/pLength.h"


class cName
{
public:
	 cName(void) { }
	~cName(void) { }

public:
	uint m_nIDX;
	char m_szName[Language::pLength::name+1];

};


class cMngName
{
public:
	 cMngName(void);
	~cMngName(void);

public:
	void Load(void);
	void Save(void);

public:
	char *GetName(uint idx);



///////////////////////////////////////////////////////////////////////////
// internal procedure
private:
	void SendReqNameList(void); // 서버에 이름 목록을 요청


///////////////////////////////////////////////////////////////////////////
// internal data
private:
	cMap<uint,cName*> m_mName;

private:
	cQueue<uint> m_qReqName; // 요청되었으나, 갖구 있지 않은 이름 목록

};
