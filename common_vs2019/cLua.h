#pragma once

#if 0

#include "nlib/cLock.h"

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

class cLua : public cLock
{
private:
	lua_State *m_LuaState;
	const char *m_szLastEr;
public:
	luaL_Reg m_LuaReg;
	
public:
	cLua();
	cLua(char *szFileName);
	virtual ~cLua();

public:
	int OpenFile(char *szFile);

	void SetLuaGlu();
	void SetLuaFunction(char *FunctionName);
	void PushNumber(int n);

	int GetDataCount();
	void LuaRun(int nPush, int nResult);	

public:
	int GetInt(int nIndex);
private:
	void Init();
	void Destory();
};

#endif
