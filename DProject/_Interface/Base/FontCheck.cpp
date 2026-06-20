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

#ifdef VERSION_USA
	for(int i=0; i<(int)wcslen(tstr); ++i)
	{
		TCHAR wc = tstr[i];
		if( !(wc>=L'a' && wc<=L'z') &&
			!(wc>=L'A' && wc<=L'Z') &&
			!(wc>=L'0' && wc<=L'9') )			
			return false;
	}	
	return true;
#elif defined THAI_CHAR_CHECK	// 태국문자로 캐릭터 및 길드 생성 관련 문자 check 코드 추가  lks007 12.08.09
	for(int i=0; i<(int)wcslen(tstr); ++i)
	{
		TCHAR wc = tstr[i];

		int n = (int)wc;
		if( !(n >= 0x0E00 && n <= 0x0E7F) )
		{
			if( !(wc>=L'a' && wc<=L'z') &&
				!(wc>=L'A' && wc<=L'Z') &&
				!(wc>=L'0' && wc<=L'9') )			
				return false;
		}		
	}	
	return true;
#elif ( defined VERSION_TW || defined VERSION_HK )
	for(int i=0; i<(int)wcslen(tstr); ++i)
	{
		TCHAR wc = tstr[i];

		if( wc == ' ' )
			return false; // 공백이면 실패

		int n = (int)wc;

		if( (wc>='a' && wc<='z') || (wc>='A' && wc<='Z') || (wc>='0' && wc<='9') )
			continue;

		if(	!(n >= 0x3000 && n <= 0x303F) &&		// 한중일 기호 및 구두점
			!(n >= 0x3105 && n <= 0x312B) &&     // 주음 부호
			!(n >= 0x31C0 && n <= 0x31EF) &&		// 한중일 한자 획
			//!(n >= 0x3200 && n <= 0x32FF) &&		// 한중일 괄호 문자 
			!(n >= 0x3300 && n <= 0x33FF) &&		// 한중일 호환용
			!(n >= 0x3400 && n <= 0x4DBF) &&		// 한중일 통합 한자 확장 A
			!(n >= 0x4E00 && n <= 0x9FFF) &&		// 한중일 한자
			!(n >= 0xF900 && n <= 0xFAFF) &&		// 한중일 호환용 한자
			!(n >= 0xFE30 && n <= 0xFE4F) &&		// 한중일 호환용 꼴
			!(n >= 0x20000 && n <= 0x2A6DF) &&		// 한중일 통합 한자 확장 B
			!(n >= 0x2F800 && n <= 0x2FA1F) &&		// 한중일 호환용 한자 보충
			!(n == 0x02C7 || n == 0x02CA || n == 0x02CB || n == 0x02D9) &&			// Phonetic 입력법에서 조합 특수 문자
			!(n >= 0xFF00 && n <= 0xFFFF ) &&		// liunt.ime 입력법에서 조합 특수 문자 범위
			//!(n == 0xFF20 || n == 0xFF29 || n == 0xFF37 || n == 0xFFFA || n == 0xFF32) &&			// liunt.ime 입력법에서 조합 특수 문자
			!(n == 0x2191 || n == 0x2295) &&		// 처음 문자 입력 시 공백 제어문자(space와 다름) || 길드 생성 시 이름 공백 제어문자(space와 다름) 
			!(n >= 0x00 && n <= 0x02F ) && !(n >= 0x03A && n <= 0x040) && !(n >= 0x05B && n <= 0x060 ) && !(n >= 0x07B && n <= 0x7F ) ) // 각 공백 제어 문자 통합
		{
//			//CsMessageBox( MB_OK, L"CODE: %d", n );
//			cPrintMsg::PrintMsg( 10030 );	// 캐릭터 ID에는 공백과 특수문자가 들어갈 수 없습니다.
//			//g_IME.ReleaseText();
//			HIMC hIme = ImmGetContext(GAMEAPP_ST.GetHWnd());
//			ImmNotifyIME( hIme, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
			return false;
		}
	}	
	return true;
#else
	for(int i=0; i<(int)wcslen(tstr); ++i)
	{
		TCHAR wc = tstr[i];

		int n = (int)wc;
		if( !(n >= 0x3131 && n > 0x3163) )
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
#endif
}