
#pragma once

//===========================================================================================================
//
//	Assert // 인자는 절대 계산 값 또는 함수가 들어가서는 안된다. ( 매크로 이기문에 )
//
//===========================================================================================================
namespace nsCSDEBUG
{
	void	_log( char* file, int line, char* Exp );
	void	_log( char* file, int line, char* Exp, LPTSTR strApp , ... );
	void	_log( char* file, int line, char* Exp, char* cApp , ... );

	struct sDEBUG_FUNC
	{
		virtual void		AssertRelease(){}
	};
	extern sDEBUG_FUNC*		g_pDebugFunc;
}

	#ifdef _GIVE
		#define assert_cs( p )										{}
		#define assert_csm( p, str )								{}
		#define assert_csm1( p, str, app )							{}
		#define assert_csm2( p, str, app1, app2 )					{}
		#define assert_csm3( p, str, app1, app2, app3 )				{}
		#define assert_csm4( p, str, app1, app2, app3, app4 )		{}
		#define assert_csm5( p, str, app1, app2, app3, app4, app5 )	{}
	#elif defined _DEBUG
		#define assert_cs( p )										if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p );										__asm { int 3} }
		#define assert_csm( p, str )								if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str );								__asm { int 3} }
		#define assert_csm1( p, str, app1 )							if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str, app1 );							__asm { int 3} }
		#define assert_csm2( p, str, app1, app2 )					if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str, app1, app2 );					__asm { int 3} }
		#define assert_csm3( p, str, app1, app2, app3 )				if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str, app1, app2, app3 );				__asm { int 3} }
		#define assert_csm4( p, str, app1, app2, app3, app4 )		if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str, app1, app2, app3, app4 );		__asm { int 3} }
		#define assert_csm5( p, str, app1, app2, app3, app4, app5 )	if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str, app1, app2, app3, app4, app5 );	__asm { int 3} }
	#else
		#define assert_cs( p )										if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p ); }
		#define assert_csm( p, str )								if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str ); }
		#define assert_csm1( p, str, app1 )							if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str, app1 ); }
		#define assert_csm2( p, str, app1, app2 )					if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str, app1, app2 ); }
		#define assert_csm3( p, str, app1, app2, app3 )				if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str, app1, app2, app3 ); }
		#define assert_csm4( p, str, app1, app2, app3, app4 )		if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str, app1, app2, app3, app4 ); }
		#define assert_csm5( p, str, app1, app2, app3, app4, app5 )	if( !(p) ) { nsCSDEBUG::_log( __FILE__, __LINE__, #p, str, app1, app2, app3, app4, app5 ); }
	#endif
