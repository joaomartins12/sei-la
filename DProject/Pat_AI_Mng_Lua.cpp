
#include "stdafx.h"
#include "Pat_AI_Mng.h" 

/*
#define L_INT( n )			lua_tointeger( L, n )		
#define L_FLOAT( n )		(float)lua_tonumber( L, n ) 
#define LP_BASE( n )		( (CPat_AI_Mng::sAI_Base*)lua_tointeger( L, n ) )
#define LuaGlue				extern "C" int

LuaGlue C_Start_Setting( lua_State* L )
{	 
	int nID = lua_tointeger( L, 1 ); // 루아 stack 1 을 읽어온다.

	g_pPatAIMng->SetContainerStartID( nID );
	return 0;
}

LuaGlue C_CreateCon( lua_State* L )
{
	int nCon = lua_tointeger( L, 1 );

	g_pPatAIMng->CreateContatiner( nCon );
	return 0;
}

LuaGlue C_GetCon( lua_State* L )
{
	assert_cs( lua_isnumber( L, 1 ) );
	int nCon = lua_tointeger( L, 1 );

	lua_pushinteger( L, (int)g_pPatAIMng->GetContainer( nCon ) );
	return 1;
}

LuaGlue C_Link( lua_State* L )
{
	assert_cs( lua_isnumber( L, 1 ) );
	assert_cs( lua_isnumber( L, 2 ) );
	assert_cs( lua_isnumber( L, 4 ) );

	int nConID						= lua_tointeger( L, 1 );
	int nParentID					= lua_tointeger( L, 2 );
	CPat_AI_Mng::sAI_Base* pBase	= (CPat_AI_Mng::sAI_Base*)lua_tointeger( L, 3 );
	int nRate						= lua_tointeger( L, 4 );

	if( nParentID != 0 )
	{
		CPat_AI_Mng::sAI_Base* pElement = g_pPatAIMng->GetContainer( nConID )->FindElement( nParentID );
		assert_cs( pElement != NULL );
		assert_cs( g_pPatAIMng->GetContainer( nConID )->FindElement( pBase->s_nID ) == NULL );	
		pElement->Add( pBase, nRate );
	}
	else
	{
		g_pPatAIMng->GetContainer( nConID )->Add( pBase, nRate );
	}
	return 0;
}

LuaGlue C_NewTime( lua_State* L )
{
	assert_cs( lua_isnumber( L, 1 ) );
	assert_cs( lua_isnumber( L, 2 ) );
	assert_cs( lua_isnumber( L, 3 ) );

	
	lua_pushinteger( L, (int)CPat_AI_Mng::sAI_Element::NewTime(	lua_tointeger( L, 1 ), lua_tointeger( L, 2 ), (float)lua_tonumber( L, 3 ) ) );
	return 1;
}

LuaGlue C_NewAni( lua_State* L )
{
	assert_cs( lua_isnumber( L, 1 ) );
	assert_cs( lua_isnumber( L, 2 ) );
	assert_cs( lua_isnumber( L, 3 ) );

	lua_pushinteger( L, (int)CPat_AI_Mng::sAI_Element::NewAnimation(	lua_tointeger( L, 1 ), lua_tointeger( L, 2 ), lua_tointeger( L, 3 )) );
	return 1;
}

LuaGlue C_NewMove( lua_State* L )
{
	assert_cs( lua_isnumber( L, 1 ) );
	assert_cs( lua_isnumber( L, 2 ) );
	assert_cs( lua_isnumber( L, 3 ) );
	
	lua_pushinteger( L, (int)CPat_AI_Mng::sAI_Element::NewMove( lua_tointeger( L, 1 ), lua_tointeger( L, 2 ), lua_tointeger( L, 3 )	));
	return 1;
}


void CPat_AI_Mng::_InitLua()
{
	cLua lua;
	lua.Init();
	
	lua.AddFunction( "C_Start_Setting", C_Start_Setting );
	lua.AddFunction( "C_CreateCon", C_CreateCon );

	lua.AddFunction( "C_GetCon", C_GetCon );	


	lua.AddFunction( "C_NewTime", C_NewTime );
	lua.AddFunction( "C_NewAni", C_NewAni );
	lua.AddFunction( "C_NewMove", C_NewMove );

	lua.AddFunction( "C_Link", C_Link );


	lua.RunScript( "Common.lua" );
}
*/
#pragma warning( default : 4311 )