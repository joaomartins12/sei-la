
#include "stdafx.h"
#include "Csini.h"

bool Csini::IsFile()
{
	return ( _access_s( m_cFileName, 0 ) == 0 );
}

char* Csini::GetStr( char const* cApp, char const* cKey )
{
	//assert_cs( _access_s( m_cFileName, 0 ) == 0 );
	GetPrivateProfileStringA( cApp, cKey, NULL, m_cBuffer, MAX_PATH, m_cFileName );
	return m_cBuffer[0]!=NULL ? m_cBuffer : NULL;
}

int Csini::GetInt( char const * cApp, char const* cKey )
{
	//assert_cs( _access_s( m_cFileName, 0 ) == 0 );
	return GetPrivateProfileIntA( cApp, cKey, -1, m_cFileName );
}

TCHAR* Csini::GetStrW( char const* cApp, char const* cKey, TCHAR * szData )
{
	//assert_cs( _access_s( m_cFileName, 0 ) == 0 );
	GetPrivateProfileStringA( cApp, cKey, NULL, m_cBuffer, MAX_PATH, m_cFileName );
	M2W( szData, m_cBuffer, MAX_PATH );
	return m_cBuffer[0]!=NULL ? szData : NULL;
}

void Csini::WriteInt( char const* cApp, char const* cKey, int nValue )
{
	char szBuf[MAX_PATH] = {0,};
	_itoa_s(nValue, szBuf, 10);
	WritePrivateProfileStringA(cApp, cKey, szBuf, m_cFileName);
}
