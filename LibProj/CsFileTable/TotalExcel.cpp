
#include "stdafx.h"
#include "TotalExcel.h"

void CsTotalExcel::sINFO::sTIME::SetCurTime()
{
	SYSTEMTIME st;
	GetLocalTime( &st );

	s_nYear = st.wYear%100;
	s_nMonth = st.wMonth;
	s_nDay = st.wDay;
	s_nHour = st.wHour;
	s_nMinute = st.wMinute;
}

void CsTotalExcel::New( DWORD nID, TCHAR* szKor )
{
	assert_cs( _tcslen( szKor ) < FT_TOTAL_EXCEL_KOR_LEN );

	m_Info.s_nID = nID;
	_tcscpy_s( m_Info.s_szKor, _tcslen( szKor ) + 1, szKor );
	m_Info.s_szEng[ 0 ] = NULL;

	m_Info.s_CreateTime.SetCurTime();
}