#include "stdafx.h"
#include "FPHeader.h"

namespace CsFPS
{
	// 소문자로 변환
	std::string StringToLower( LPCSTR szTrans )
	{
		std::string temp(szTrans);
		std::transform( temp.begin(), temp.end(), temp.begin(), tolower );
		return temp;
	}

	// 대문자로 변환
	std::string StringToUpper( LPCSTR szTrans )
	{
		std::string temp(szTrans);
		std::transform( temp.begin(), temp.end(), temp.begin(), toupper );
		return temp;
	}

	size_t GetHashCode( LPCSTR sz )
	{
		if( NULL == sz )
			return 0;

		std::string tr = StringToLower(sz);
		if( 0 == tr.length() )
			return 0;

		size_t result = 5381;
		for( size_t n = 0; n < tr.size(); ++n )
		{
			if( (tr.at(n) == '.') || (tr.at(n) == '\\') )
				continue;

			result = ( (result<< 5) + result ) + (DWORD)tr.at(n); // hash * 33 + ch
		}
		return result;
	}

	// 소문자로 변환
	std::wstring StringToLower( LPCWSTR szTrans )
	{
		std::wstring temp(szTrans);
		std::transform( temp.begin(), temp.end(), temp.begin(), tolower );
		return temp;
	}

	// 대문자로 변환
	std::wstring StringToUpper( LPCWSTR szTrans )
	{
		std::wstring temp(szTrans);
		std::transform( temp.begin(), temp.end(), temp.begin(), toupper );
		return temp;
	}

	size_t GetHashCode( LPCWSTR sz )
	{
		if( NULL == sz )
			return 0;

		std::wstring tr = StringToLower(sz);
		if( 0 == tr.length() )
			return 0;

		size_t result = 5381;
		for( size_t n = 0; n < tr.size(); ++n )
		{
			if( (tr.at(n) == L'.') || (tr.at(n) == L'\\') )
				continue;

			result = ( (result<< 5) + result ) + (DWORD)tr.at(n); // hash * 33 + ch
		}
		return result;
	}

	bool ExistFile(LPCSTR szPath)
	{
		if( 0 != _access_s( szPath, 0 ) )
			return false;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void tagCHUNK::GetFilePath(std::string & paht, std::string& name)
	{
		paht = ".\\";
		name.clear();
		char szPath[_MAX_PATH], szName[_MAX_FNAME], szExt[_MAX_EXT];
		_splitpath_s(s_szPath, NULL, 0, szPath, _MAX_PATH, szName, _MAX_FNAME, szExt, _MAX_EXT);
		paht += szPath;
		name = szName;
		name += szExt;
	}

};