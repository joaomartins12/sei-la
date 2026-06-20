
#pragma once 

//#pragma comment(lib, "lua5.1.lib" )

// extern "C" {
// 	//#include "..\LibProj\Lua\clua.h"
// #include "..\Lua\Include\lua.h"
// #include "..\Lua\Include\lualib.h"
// #include "..\Lua\Include\lauxlib.h"
// }
#pragma warning( disable : 4311 )

class CPat_AI_Mng : public NiMemObject
{
// 	struct cLua
// 	{
// 		lua_State* g_pLuaSt;
// 
// 		cLua(){g_pLuaSt = NULL;}
// 		void	Delete(){ lua_close(g_pLuaSt);}
// 		void Init()
// 		{
// 			g_pLuaSt = lua_open();
// 			//luaopen_base(g_pLuaSt);
// 			//luaopen_io(g_pLuaSt);
// 			//luaopen_string(g_pLuaSt);
// 			//luaopen_math(g_pLuaSt);
// 			//luaopen_debug(g_pLuaSt);
// 			//luaopen_table(g_pLuaSt);
// 			luaL_openlibs(g_pLuaSt);
// 		}
// 		void RunScript( const char* sScr)
// 		{
// 
// 			int ret = luaL_dofile(g_pLuaSt, sScr);
// 			CsMessageBoxA( MB_OK, "return:%d", ret );
// 		}
// 		void AddFunction( const char* csFunc, int(*func)(lua_State* L) )
// 		{
// 			lua_register(g_pLuaSt, csFunc, func);
// 		}
// 	};


public:
	static void		GlobalInit();
	static void		GlobalShotDown();

public:
	CPat_AI_Mng(){ m_nInitContainerID = 0; }
	virtual ~CPat_AI_Mng(){ Delete(); }

public:
	void			Init();
	void			Delete();

protected:
	void			_InitLua();

public:
	enum eCHECK_TYPE
	{
		CHK_NONE, CHK_TIME, CHK_ANI, CHK_MOVE,
	};

	enum eMODUL
	{
		MODUL_Container, MODUL_Element,
	};

public:

	struct sNEXT
	{
		void*	s_pNext;
		int		s_nRate;
	};

	struct sAI_Base : public NiMemObject
	{
	public:
		sAI_Base(){ s_nNextTotalRate = 0; s_nID = 0; Delete();}

		virtual eMODUL	GetModul() = 0;
		virtual void	Delete();
		sAI_Base*		FindElement( int nID );		

		sAI_Base*		GetNext();
		sAI_Base*		Add( sAI_Base* pNext, int nRate );

		int						s_nID;
		int						s_nNextTotalRate;		
		CsVectorPB< sNEXT >		s_vpNext;
	};

	struct sAI_Element : public sAI_Base
	{
		static sAI_Element* NewTime( int nID, int ani, float fSec );
		static sAI_Element* NewAnimation( int nID, int ani, int nLoopCount );
		static sAI_Element* NewMove( int nID, int act, int ani );

		eMODUL			GetModul(){ return MODUL_Element; }

		// 시간 체크 타입
		struct sTIME
		{
			float		s_fTimeSec;
		};

		// 애니메이션 체크 타입
		struct sANIMATION
		{
			int			s_nLoopCount;
		};

		// 이동 체크 타입
		struct sMOVE
		{
			int			s_nAct;
		};		

		eCHECK_TYPE		s_eCheckType;
		ANI::eTYPE		s_eAnimation;

		sTIME			s_Time;
		sANIMATION		s_Animation;
		sMOVE			s_Move;		
	};

	struct sAI_Container : public sAI_Base
	{
		eMODUL			GetModul(){ return MODUL_Container; }		
	};

protected:
	int										m_nInitContainerID;
	std::map< DWORD, sAI_Container* >		m_mapContainer;

public:
	sAI_Container*	CreateContatiner( int nCon );
	sAI_Container*	GetContainer( int con );

	void			SetContainerStartID( int nID ){ m_nInitContainerID = nID; }
	int				GetContainerStartID(){ return m_nInitContainerID; }
};


extern CPat_AI_Mng*		g_pPatAIMng;
