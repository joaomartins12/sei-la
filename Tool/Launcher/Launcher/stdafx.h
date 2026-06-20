// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include <SDKDDKVer.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.


#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.
#include <afxpriv.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "json.h"
#include "..\..\..\LibProj\CsFilePack\stdafx.h"
#include "..\..\..\LibProj\CsFunc\stdafx.h"
#include "..\..\..\LibProj\CsThread\stdafx.h"
#include "..\..\..\LibProj\CsDM\Dm-PCH.h"
#include "..\..\..\LibProj\CsFilePack\CsFilePackSystem.h"

#include <assert.h>
#include <afxinet.h>

#include "_Define.h"
#include "Configuration.h"
#include "DownloadManager.h"

#include "_String.h"
#include "_GameInfo.h"
#include "BmpRender.h"
#include "resource.h"


#include "StrMng.h"
#include "ThreadMng.h"
#include "Launcher.h"
#include "LauncherDlg.h"
#include "CDownloadInfo.h"

#ifdef USE_THEMIDA_SDK
#include "..\..\..\ThemidaSDK\Include\C\ThemidaSDK.h"
#endif
