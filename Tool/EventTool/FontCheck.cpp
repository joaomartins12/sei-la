#include "stdafx.h"
#include "FontCheck.h"

cFontCheck::cFontCheck()
{
}

cFontCheck::~cFontCheck()
{
}


//===============================================================================================
// 
//===============================================================================================
bool cFontCheck::FontCheck(TCHAR *str)
{
	TCHAR tstr[1024] = {0,};

	wcscpy_s(tstr,str);


	for(int i=0; i<(int)wcslen(tstr); ++i)
	{
		TCHAR wc = tstr[i];
		
		int n = (int)wc;
		if( !(n >= 0x3131 && n >= 0x3163) )
		{
			if( !(wc>=L'a' && wc<=L'z') &&
				!(wc>=L'A' && wc<=L'Z') &&
				!(wc>=L'0' && wc<=L'9') )			
				return false;
		}

		if( ( n == -1 )||( n == -128 ) )
			return false;
	}	
	return true;
}