// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

// 아래 지정된 플랫폼에 우선하는 플랫폼을 대상으로 하는 경우 다음 정의를 수정하십시오.
// 다른 플랫폼에 사용되는 해당 값의 최신 정보는 MSDN을 참조하십시오.
#ifndef WINVER				// Windows XP 이상에서만 기능을 사용할 수 있습니다.
#define WINVER 0x0501		// 다른 버전의 Windows에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_WINNT		// Windows XP 이상에서만 기능을 사용할 수 있습니다.                   
#define _WIN32_WINNT 0x0501	// 다른 버전의 Windows에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 이상에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINDOWS 0x0410 // Windows Me 이상에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_IE			// IE 6.0 이상에서만 기능을 사용할 수 있습니다.
#define _WIN32_IE 0x0600	// 다른 버전의 IE에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

#ifdef _SECURE_SCL
#undef _SECURE_SCL
#endif /*_SECURE_SCL*/

#define _SECURE_SCL	0

#ifdef _HAS_ITERATOR_DEBUGGING
#undef _HAS_ITERATOR_DEBUGGING
#endif /*_HAS_ITERATOR_DEBUGGING*/

#define _HAS_ITERATOR_DEBUGGING 0



// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//*
#ifdef _DEBUG
// "stdafx.h" file의 제일 하단에 카피해 넣으세요...
#	ifdef _AFXDLL
#		define	xnew new
#	else
#		define _CRTDBG_MAP_ALLOC // 메모리 누수를 탐지하기 위해 선언 해주어야 한다.
#		include <crtdbg.h>
#		define	xnew		new(_CLIENT_BLOCK, __FILE__, __LINE__)
//#		define	xnew		new(_NORMAL_BLOCK, __FILE__, __LINE__)
#	endif // _AFXDLL
#else
#	define	xnew new
#endif _DEBUG
//*/

#define xdelete(x)			if(x) { delete (x); x = null; }
#define xdelete_array(x)	if(x) { delete []x; x = null; }


#define xcast(type,val)		(*(type*)&val) // 해당 type으로 강제 캐스팅한다.

typedef __int64				int64;	// 가능하면 __int64로 사용해주세요.
typedef unsigned __int64	uint64;
typedef unsigned long		ulong;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned char		uchar;
typedef wchar_t				wchar;


#define n0					void
typedef char				n1;
typedef short				n2;
typedef int					n4;
typedef __int64				n8;

#define u0					void
typedef unsigned char		u1;
typedef unsigned short		u2;
typedef unsigned int		u4;
typedef unsigned __int64	u8;


//프로젝트 국가별 설정 chu8820
#include "_TestDefine.h"

#include "resource.h"
#include "TCHAR.H"
#include "Process.h"

#include "..\..\LibProj\CsThread\stdafx.h"
#include "..\..\LibProj\CsFunc\stdafx.h"
#include "..\..\LibProj\CsMFC\stdafx.h"
#include "..\..\LibProj\CsFileTable\stdafx.h"
#include "..\..\LibProj\CsGamebryo2.3\stdafx.h"
#include "..\..\LibProj\CsGBChar\stdafx.h"

//==================================================================================================
// GameBryo
#pragma warning( disable : 4267 )
#pragma warning( disable : 4995 )

#include <NiMain.h>
#include <NiAnimation.h>
#include <NiDx9Renderer.h>
#include <NiCollisionData.h>
#include <NiBoundingVolume.h>
#include <NiCollisionSDM.h>
#include <NiParticle.h>
#include <NiDebug.h>
#include <NiMemTracker.h>
#include <NiStandardAllocator.h>

#pragma warning( default : 4267 )
#pragma warning( default : 4995 )
//==================================================================================================

#include "_DlgController.h"
#include "_DlgMapList.h"
#include "_DlgEventlist.h"
#include "_DlgNpcList.h"
#include "_DlgEventTime.h"
#include "_DlgEventTalkList.h"
#include "_DlgEventSpeed.h"
#include "_DlgEventLoadList.h"
#include "_DlgSceneLoadList.h"
#include "_DlgTimeChange.h"
#include "_DlgSoundEvent.h"
#include "_DlgEffectList.h"

#include "EventTool.h"
#include "NiViewer.h"
#include "MainFrm.h"

#include "Window.h"
#include "Font.h"
#include "FontCheck.h"
#include "Sprite.h"
#include "Text.h"
#include "String.h"
#include "StringAnalysis.h"
#include "TalkBalloon.h"

#include "EventCall.h"
#include "Npc.h"
#include "NpcMng.h"
#include "Terrain.h"
#include "EventScene.h"


// Sound
#include <NiMilesAudio.h>
#include <NiMilesAudioSystem.h>
#include <NiAudioSystem.h>
#include <NiAudioSource.h>
#include "CSound.h"
#include "CSoundMgr.h"


