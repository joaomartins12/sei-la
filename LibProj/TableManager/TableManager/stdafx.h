// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifndef _WIN32_WINNT		// Windows XP 이상에서만 기능을 사용할 수 있습니다.                   
#define _WIN32_WINNT 0x0501	// 다른 버전의 Windows에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

#define	SAFE_POINTER_RET(x)				if( !x ) { return; }
#define	SAFE_POINTER_CON(x)				if( !x ) { continue; }
#define	SAFE_POINTER_BEK(x)				if( !x ) { break; }
#define	SAFE_POINTER_RETVAL( x ,ret )	if( !x ) { return ret; }

#define _CRT_NO_VA_START_VALIDATION
#include <atlstr.h>

#include "Dm-PCH.h"
#include "TableInclude.h"