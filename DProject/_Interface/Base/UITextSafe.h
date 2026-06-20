#pragma once
#include <string>

namespace UiTextSafe
{
	inline bool IsVisibleChar( TCHAR ch )
	{
		return ch == _T('\t') || ch == _T('\n') || ch == _T('\r') || ( ch >= 0x20 && ch != 0x7F );
	}

	inline const TCHAR* Safe( const TCHAR* pText )
	{
		return ( pText != NULL ) ? pText : _T( "" );
	}

	inline std::wstring Owned( const TCHAR* pText )
	{
		const TCHAR* pSafe = Safe( pText );
		std::wstring sanitized;
		for( const TCHAR* p = pSafe; *p != 0; ++p )
		{
			if( IsVisibleChar( *p ) )
				sanitized.push_back( *p );
		}
		return sanitized;
	}
}
