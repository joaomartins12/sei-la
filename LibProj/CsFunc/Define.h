
#pragma once

#ifdef _DEBUG
#	ifdef _AFXDLL
#		define _CRTDBG_MAP_ALLOC
#		include <crtdbg.h>
#		include <afxtempl.h>
#		include <afx.h>
#		define	csnew		DEBUG_NEW
#	else
#		define _CRTDBG_MAP_ALLOC // 메모리 누수를 탐지하기 위해 선언 해주어야 한다.
#		include <crtdbg.h>
#		define	csnew		new(_NORMAL_BLOCK, __FILE__, __LINE__)
#	endif // _AFXDLL
#else
#	define	csnew new
#endif _DEBUG


// FIleName
#ifndef MAX_FILENAME
	#define MAX_FILENAME			64
#endif	MAX_FILENAME

// SAFE_DELETE
#ifndef SAFE_DELETE
	#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
	#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
	#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif	//SAFE_DELETE

#define		SAFE_POINTER_RET(x)				if( !x ) { return; }
#define		SAFE_POINTER_CON(x)				if( !x ) { continue; }
#define		SAFE_POINTER_BEK(x)				if( !x ) { break; }
#define		SAFE_POINTER_RETVAL( x ,ret )	if( !x ) { return ret; }

#define		CP_KOREAN					949 
#define		CP_JAPANESE					932 
#define		CP_THAILAND					874
#define		CP_CHINESE_SIMPLIFIED		936 
#define		CP_CHINESE_TRADITIONAL		950

#ifdef VERSION_TH
#define		PO_CP_CODE					CP_THAILAND

#elif VERSION_TW
	#define		PO_CP_CODE				CP_UTF8//CP_CHINESE_TRADITIONAL

#elif VERSION_HK
	#define		PO_CP_CODE				CP_UTF8//CP_CHINESE_TRADITIONAL//CP_UTF8

#else
#define		PO_CP_CODE					CP_KOREAN
#endif

#define		W2M(dest,src,size)			WideCharToMultiByte( PO_CP_CODE, 0, src, -1, dest, size, NULL, NULL )
#define		M2W(dest,src,size)			MultiByteToWideChar( PO_CP_CODE, 0, src, -1, dest, size )

#define		VERSION_STRING( dwVersion, str, nSize )\
	_stprintf_s( str, nSize, _T( "Version %d.%d.%d" ), (dwVersion&0x00ff0000)>>16, (dwVersion&0x0000ff00)>>8, dwVersion&0x000000ff )


// Color
#define CS_ARGB(a,r,g,b) \
	((DWORD)((a<<24)|(r<<16)|(g<<8)|b))
	//((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define CS_XRGB(r,g,b) \
	((DWORD)((0xff<<24)|(r<<16)|(g<<8)|b))
	//((DWORD)(((0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define CS_GetAValue( argb )	(BYTE)(((argb)&0xff000000)>>24)
#define CS_GetRValue( argb )	(BYTE)(((argb)&0x00ff0000)>>16)
#define CS_GetGValue( argb )	(BYTE)(((argb)&0x0000ff00)>>8)
#define CS_GetBValue( argb )	(BYTE)((argb)&0x000000ff)

#define CsD2R( degree )			( (degree)*0.0174532925199433f )
#define CsR2D( radian )			( (radian)*57.295779513082321f )
#define CsFloat2Int( v )		( (int)( v + 0.0001f ) )
#define CsFloat2Int64( v )		( (__int64)( v + 0.0001f ) )
#define CsFloat2Uint( v )		( (unsigned int)( v + 0.0001f ) )
#define CsMax(a, b)				(((a) > (b)) ? (a) : (b))
#define CsMin(a, b)				(((a) > (b)) ? (b) : (a))
#define CsFloatRounding(x, dig)	( floor((x) * pow(float(10), dig) + 0.5f) / pow(float(10), dig) )// 반올림 메크로 

#define CsF2B( floatSrc )		( (BYTE)((floatSrc)*255.0f) )
#define CsB2F( byteSrc )		( (float)((byteSrc)*0.003921568627451f ) )

#define CsInFi( radian )		{\
	while( radian > 3.141592f ){\
		radian -= 6.283184f;\
	}\
	while( radian < -3.141592f ){\
		radian += 6.283184f;\
	}\
}