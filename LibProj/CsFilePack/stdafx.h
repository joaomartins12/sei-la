// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifndef _WIN32_WINNT		// Windows XP 이상에서만 기능을 사용할 수 있습니다.                   
#define _WIN32_WINNT 0x0501	// 다른 버전의 Windows에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

#include <Windows.h>
#include <map>
#include <list>
#include <vector>
#include <algorithm>

#include <io.h>
#include <fcntl.h>
#include <assert.h>

#define FILE_HASH_VERSION					0x00000010
#define FILE_PACK_VERSION					0x00000010
#define FILE_HASH_VERSION_SECURITY			0x00000011
#define FILE_HASH_VERSION_SECURITY_NEW		0x00000012

#define FP_INVALIDE_SIZE		((UINT64)-1)
#define FP_CHANGE_SIZE			((UINT64)-2)
#define FP_INVALIDE_HANDLE		( -1 )
#define FP_XOR_VALUE			0xd0


#define ERROR_OK				0x0000000
#define ERROR_HASH_FILE_LOAD	0x0000001
#define ERROR_HASH_FILE_OPEN	0x0000002
#define ERROR_HASH_FILE_VERSION	0x0000003
#define ERROR_HASH_FILE_SIZE	0x0000004
#define ERROR_HASH_FILE_NULL	0x0000005

#define ERROR_PACK_FILE_OPENED	0x0000010
#define ERROR_PACK_FILE_LOAD	0x0000011
#define ERROR_PACK_FILE_OPEN	0x0000012
#define ERROR_PACK_FILE_VERSION	0x0000013
#define ERROR_PACK_FILE_SIZE	0x0000014

#define ERROR_NOT_OPENED_PACKFILE	0x0000020


// SAFE
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif	//SAFE_DELETE

#ifndef SAFE_POINTER_RET
#define		SAFE_POINTER_RET(x)				if( !x ) { return; }
#define		SAFE_POINTER_CON(x)				if( !x ) { continue; }
#define		SAFE_POINTER_BEK(x)				if( !x ) { break; }
#define		SAFE_POINTER_RETVAL( x ,ret )	if( !x ) { return ret; }
#endif

#include "FPHeader.h"
#include "CsFileHash.h"
#include "CsFilePack.h"
#include "CsFilePackSystem.h"

