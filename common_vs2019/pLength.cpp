#include "pLength.h"
#include <algorithm>
#include <iostream>
#include <cwchar>


int Language::CheckUnicodeChar(wchar c, wchar nc) // nc is next wchar
{
	if(c>=L'a' && c<=L'z')	return nCode::english;
	if(c>=L'A' && c<=L'Z')	return nCode::english;
	if(c>=L'0' && c<=L'9')	return nCode::english;

#pragma warning( disable : 4129 )

	switch(c)
	{
	case L'_' : return nCode::special;
	case L' ' : return nCode::space;
	case L'+' :
	case L'*' :
	case L'.' :
	case L'~' :
	case L'@' : return nCode::special2;
	case L'!' :
	case L'"' :
	case L'#' :
	case L'$' :
	case L'\%':
	case L'&' :
	case L'\'':
	case L'(' :
	case L')' :
	case L',' :
	case L'-' :
	case L'/' :
	case L'\\':
	case L':' :
	case L';' :
	case L'<' :
	case L'=' :
	case L'>' :
	case L'?' :
	case L'[' :
	case L']' :
	case L'^' :
	case L'`' :
	case L'{' :
	case L'|' :
	case L'}' : return nCode::special3;
	case (wchar)0xA3 :
		if(nc>(wchar)0xA0 && nc<(wchar)0xFF)	// 전각 문자
		{
			return nCode::none;
		}
		break;
	case (wchar)0xA4 :
		if(nc == (wchar)0xFD)	// 전각 문자	'ㆍ'
		{
			return nCode::none;
		}
		break;
	}

#pragma warning( default : 4129 )

	int n = (int)c;	

	if ( (n >= 0xAC00 && n <= 0xD7A3)		// 한글 소리 영역
	//	|| (n >= 0x1100 && n <= 0x11FF)		// 한글 자모 영역
	)
	{
		return nCode::hangle;
	}

	if(	(n >= 0x3000 && n <= 0x303F) ||     // 한중일 기호 및 구두점
		(n >= 0x3105 && n <= 0x312B) ||     // 주음 부호
		(n >= 0x31C0 && n <= 0x31EF) ||     // 한중일 한자 획
		//(n >= 0x3200 && n <= 0x32FF) ||     // 한중일 괄호 문자 
		(n >= 0x3300 && n <= 0x33FF) ||     // 한중일 호환용
		(n >= 0x3400 && n <= 0x4DBF) ||		// 한중일 통합 한자 확장 A
		(n >= 0x4E00 && n <= 0x9FFF) ||		// 한중일 한자
		(n >= 0xF900 && n <= 0xFAFF) ||     // 한중일 호환용 한자
		(n >= 0xFE30 && n <= 0xFE4F) ||     // 한중일 호환용 꼴
		(n >= 0x20000 && n <= 0x2A6DF) ||   // 한중일 통합 한자 확장 B
		(n >= 0x2F800 && n <= 0x2FA1F) ||   // 한중일 호환용 한자 보충
		n == 0x02C7 || n == 0x02CA || n == 0x02CB || n == 0x02D9 ||		// Phonetic 입력법에서 조합 특수 문자
		(n >= 0xFF00 && n <= 0xFF26 ) ||	// liunt.ime 입력법에서 조합 특수 문자 범위
		(n >= 0xFF28 && n <= 0xFF2C ) ||	// liunt.ime 입력법에서 조합 특수 문자 범위 (G 제외)
		(n >= 0xFF2E && n <= 0xFFFF ) ||	// liunt.ime 입력법에서 조합 특수 문자 범위 (M 제외)
		//n == 0xFF20 || n == 0xFF29 || n == 0xFF37 || n == 0xFFFA ||	n == 0xFF32 ||  	// liunt.ime 입력법에서 조합 특수 문자
		n == 0x2191 || n == 0x2295 ||		// 처음 문자 입력 시 공백 제어문자(space와 다름) || 길드 생성 시 이름 공백 제어문자(space와 다름) 
		(n >= 0x00 && n <= 0x02F ) || (n >= 0x03A && n <= 0x040) || (n >= 0x05B && n <= 0x060 ) || (n >= 0x07B && n <= 0x7F ) ) // 각 공백 제어 문자 통합
	{
		// 		n == 0x03F || n == 0x00 ||			// Give 버전에서의 공백 제어문자(space와 다름) || xp 에서의 공백 제어문자(space와 다름)
		// 		n == 0x020 ||				// Chinese Traditional DaYi 입력법에서 문자 입력 시 최초 space 넘김, 입력창에서만 space 허용하고 생성버튼 누를때 space 있으면 false로 해야함.
		// 		n == 0x040 ||				// Chinese Traditional Phonetic 입력법에서 공백 제어문자(space와 다름)
		// 		n == 0x07D || n == 0x7C || n == 5C )	// Chinese (Traditional) - Quick 입력법에서 제어문자
		//n == 0xE1CC )	// Chinese Traditional New Phonetic 입력법에서 공백 문자(space와 다름)
		return nCode::hanja;
	}

	if( (n >= 0x0E00 && n <= 0x0E7F) )
	{
		return nCode::thai;
	}	

	return nCode::none;
}

int Language::__CheckMultibytes(std::string &str, int min, int max, std::vector<int> &allowtype, int characterset, bool charCheck)
{
	int len = str.size();	
	if (0 == len)
	{
		if (0 == min)
		{
			return SUCCEEDED;
		}
		return STRING_EMPTY;
	}

	std::wstring dest;
	int iConverted = ::MultiByteToWideChar( characterset, 0, str.c_str(), str.size(), NULL, 0 );
	dest.resize( iConverted );
	::MultiByteToWideChar( characterset, 0, str.c_str(), str.size(), &dest.at( 0 ), iConverted );

	return __CheckUnicode(dest, min, max, allowtype, characterset, charCheck);
}

int Language::__CheckUnicode(std::wstring &str, int min, int max, std::vector<int> &allowtype, int characterset, bool charCheck) 
{ 	
	characterset;
	int len = str.size();
	if (0 == len)
	{
		if (0 == min)
		{
			return SUCCEEDED;
		}
		return STRING_EMPTY;
	}
	if (len < min)
	{
		return STRING_SIZE_MIN;
	}
	if (len > max)
	{
		return STRING_SIZE_MAX;
	}
	
	//문자 언어체크
	if(charCheck)
	{
		for(int i = 0; i < len; i++)
		{		
			wchar c = str[i];
			wchar nc = (i == len - 1) ? L'\0' : str[i+1];
			int lan = CheckUnicodeChar(c, nc);

			std::vector<int>::iterator iter = std::find(allowtype.begin(), allowtype.end(), lan);
			if(iter == allowtype.end())
				return STRING_UNKNOWN_TYPE;

			if( i == 0 &&  nCode::thai == lan )// 태국어 문자의 첫 문자가 자음이 아닌경우 에러
			{
				int d = (int)c;
				if( d < 0x0E01 || d > 0x0E2E )
					return STRING_UNKNOWN_TYPE;				
			}
		}
	}

	return SUCCEEDED;
}