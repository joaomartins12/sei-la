#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define WIN32_LEAN_AND_MEAN

#ifndef SAFE_DELETE
	#define SAFE_DELETE( p )		{ if( p ) { delete (p);     (p) = NULL; } }
#endif /*SAFE_DELETE*/
#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY( p )	{ if( p ) { delete[] (p);   (p) = NULL; } }
#endif /*SAFE_DELETE_ARRAY*/
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE( p )		{ if( p ) { (p)->Release(); (p) = NULL; } }
#endif /*SAFE_RELEASE*/

#ifndef NISAFE_DELETE
#define NISAFE_DELETE( p )		{ if( p ) { NiDelete(p);     (p) = NULL; } }
#endif /*NISAFE_DELETE*/

#define SAFE_POINTER_RET(x)				if( !x ) { return; }
#define SAFE_POINTER_CON(x)				if( !x ) { continue; }
#define SAFE_POINTER_BEK(x)				if( !x ) { break; }
#define SAFE_POINTER_RETVAL( x ,ret )	if( !x ) { return ret; }

#pragma warning(disable:4099)
#pragma warning(disable:4819)

#pragma warning(disable:4996)
#pragma warning(disable:4129)

#include <windows.h>
#include <stdio.h>


extern bool		g_bUseFilePack;
extern int		g_nScreenWidth;
extern int		g_nScreenHeight;

extern volatile bool g_bActive;

extern float g_fDeltaTime;
extern bool	g_bFirstLoding;			// ChangeServer 보내기 위해서
extern bool g_bInstallStart;

extern bool	g_bFirstConnetChar;	

#ifdef IME_WINDOW_POSITION
extern int g_nIMEWidth;
extern int g_nIMEHeight;
extern bool g_bIMEPosition;
#endif

#define DEFAULT_SCREEN_WIDTH		1024
#define DEFAULT_SCREEN_HEIGHT		768

namespace IFREGION_X
{
	enum eTYPE{
		LEFT, CENTER, RIGHT, JUST_CENTER,
	};
}

namespace IFREGION_Y
{
	enum eTYPE{
		TOP, CENTER, BOTTOM, JUST_CENTER,
	};
}

extern NiRenderer*	g_pRenderer;

extern CsLog		g_CriticalLog;

// enum eLOOP_PLAY_STATE
// {
// 	LPS_NONE, LPS_LOADING, LPS_PLAY, LPS_POST_LOAD, LPS_FADE_IN
// };
//extern eLOOP_PLAY_STATE	g_eLoopPlayState;

#endif