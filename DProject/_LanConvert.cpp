
#include "stdafx.h"
#include "_LanConvert.h"

const char* LanConvert( TCHAR* szSrc )
{	
	static char szDest[ MAX_PATH ];
	int nLen = W2M( szDest, szSrc, sizeof(szDest) );	
	szDest[ nLen ] = NULL;
	return szDest;
}

const TCHAR* LanConvertT( char* szSrc )
{	
	static TCHAR szDest[ MAX_PATH ];
	int nLen = M2W( szDest, szSrc, sizeof( szDest ) );
	szDest[ nLen ] = NULL;
	return szDest;
}



