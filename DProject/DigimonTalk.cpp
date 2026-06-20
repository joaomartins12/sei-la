#include "stdafx.h"
#include "DigimonTalk.h"

void cDigimonTalk_Resist::Reset()
{
	memset( &m_DigimonTalk, 0, sizeof( sResist ) * MAX_TALK );
	for( int i = 0; i < MAX_TALK; i++ )
	{
		m_DigimonTalk[ i ].s_nPersent = 10;
	}
}

bool cDigimonTalk_Resist::SaveTalk( DWORD dwTalk, TCHAR* szTalk, int nPer )
{
	assert_cs( _tcslen( szTalk ) <= 120 );

	for( int i = 0; i < MAX_TALK; i++ )
	{
		if( m_DigimonTalk[ i ].s_nKey == dwTalk )
		{
			m_DigimonTalk[ i ].s_nKey = dwTalk;
			m_DigimonTalk[ i ].s_nPersent = nPer;
			_tcscpy_s( m_DigimonTalk[ i ].s_szTalk, MAX_LEN, szTalk );	
			return true;
		}
	}

	for( int i = 0; i < MAX_TALK; i++ )
	{
		if( m_DigimonTalk[ i ].s_nKey == 0 )
		{
			m_DigimonTalk[ i ].s_nKey = dwTalk;
			m_DigimonTalk[ i ].s_nPersent = nPer;
			_tcscpy_s( m_DigimonTalk[ i ].s_szTalk, MAX_LEN, szTalk );	
			return true;
		}
	}

	return false;
}

cDigimonTalk_Resist::sResist* cDigimonTalk_Resist::GetTalk( DWORD dwTalk )
{
	for( int i = 0; i < MAX_TALK; i++ )
	{
		if( m_DigimonTalk[ i ].s_nKey == dwTalk )
			return &m_DigimonTalk[ i ];
	}

	return NULL;
}