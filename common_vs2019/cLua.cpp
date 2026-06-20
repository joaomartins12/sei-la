#if 0
#include "cLua.h"

cLua::cLua()
{
	Init();	

	m_LuaState = luaL_newstate();
	luaL_openlibs(m_LuaState);
}

cLua::~cLua()
{
	Destory();
}

//luaL_Reg nReg = { "StringToTS", StringToTS };
//lua_register(g_pLuastate,nReg.name,nReg.func);	

void cLua::SetLuaGlu()
{
	lua_register(m_LuaState,m_LuaReg.name,m_LuaReg.func);	
}

int cLua::OpenFile(char *szFile)
{	
	if( szFile == NULL || strlen(szFile) == 0 )
	{
		LOG("Open File Error.");
		return -1;
	}

	int nResult = luaL_dofile(m_LuaState,szFile);
	if( nResult != 0 )
	{
		m_szLastEr = lua_tostring(m_LuaState,1);
		LOG("%s",m_szLastEr);
	}

	return nResult;
}

int cLua::GetDataCount()
{
	return lua_gettop(m_LuaState);
}

void cLua::SetLuaFunction(char *FunctionName)
{
	if( FunctionName == NULL || strlen(FunctionName) == 0 )
	{
		LOG("FunctionName Error");
		return;
	}

	lua_getglobal(m_LuaState, FunctionName);
}

void cLua::PushNumber(int n)
{
	lua_pushinteger(m_LuaState, n);
}

void cLua::LuaRun(int nPush, int nResult)
{
	lua_call(m_LuaState, nPush, nResult);
//	int error = lua_pcall(m_LuaState, nPush, nResult, 0);
//	if (error != 0)
//		LOG("error : %d, %s", error,
//		lua_tostring(m_LuaState, -1));
}

int cLua::GetInt(int nIndex)
{
	int nResult = static_cast<int>(lua_tointeger(m_LuaState,nIndex));
	return nResult;
}

void cLua::Init()
{	
	m_LuaState = NULL;
	memset(	&m_LuaReg,0, sizeof(luaL_Reg) );
}

void cLua::Destory()
{
	lua_close(m_LuaState);
}

#endif
