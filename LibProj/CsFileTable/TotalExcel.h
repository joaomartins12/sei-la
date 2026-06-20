

#pragma once 

#define FT_TOTAL_EXCEL_KOR_LEN		1024
#define FT_TOTAL_EXCEL_ENG_LEN		2048


class CsTotalExcel
{
public:
	typedef	std::map< DWORD, CsTotalExcel* >			MAP;
	typedef	std::map< DWORD, CsTotalExcel* >::iterator	MAP_IT;

public:
	struct sINFO
	{
		DWORD		s_nID;
		TCHAR		s_szKor[ FT_TOTAL_EXCEL_KOR_LEN ];
		TCHAR		s_szEng[ FT_TOTAL_EXCEL_ENG_LEN ];

		struct sTIME
		{
			int			s_nYear;
			int			s_nMonth;
			int			s_nDay;
			int			s_nHour;
			int			s_nMinute;
			
			void		SetCurTime();
			bool		operator>(const sTIME& src){ return
				s_nYear > src.s_nYear ? true :
				s_nYear < src.s_nYear ? false :
				s_nMonth > src.s_nMonth ? true :
				s_nMonth < src.s_nMonth ? false :
				s_nDay > src.s_nDay ? true :
				s_nDay < src.s_nDay ? false :
				s_nHour > src.s_nHour ? true :
				s_nHour < src.s_nHour ? false :
				s_nMinute > src.s_nMinute ? true : false; }
			bool		operator>=(const sTIME& src){ return
				s_nYear > src.s_nYear ? true :
				s_nYear < src.s_nYear ? false :
				s_nMonth > src.s_nMonth ? true :
				s_nMonth < src.s_nMonth ? false :
				s_nDay > src.s_nDay ? true :
				s_nDay < src.s_nDay ? false :
				s_nHour > src.s_nHour ? true :
				s_nHour < src.s_nHour ? false :
				s_nMinute >= src.s_nMinute ? true : false; }
			bool		operator<(const sTIME& src){ return
				s_nYear < src.s_nYear ? true :
				s_nYear > src.s_nYear ? false :
				s_nMonth < src.s_nMonth ? true :
				s_nMonth > src.s_nMonth ? false :
				s_nDay < src.s_nDay ? true :
				s_nDay > src.s_nDay ? false :
				s_nHour < src.s_nHour ? true :
				s_nHour > src.s_nHour ? false :
				s_nMinute < src.s_nMinute ? true : false; }
			bool		operator<=(const sTIME& src){ return
				s_nYear < src.s_nYear ? true :
				s_nYear > src.s_nYear ? false :
				s_nMonth < src.s_nMonth ? true :
				s_nMonth > src.s_nMonth ? false :
				s_nDay < src.s_nDay ? true :
				s_nDay > src.s_nDay ? false :
				s_nHour < src.s_nHour ? true :
				s_nHour > src.s_nHour ? false :
				s_nMinute <= src.s_nMinute ? true : false; }
		};
		sTIME		s_CreateTime;
	};
protected:
	sINFO			m_Info;

public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
	void			New( DWORD nID, TCHAR* szKor );

	sINFO*			GetInfo(){ return &m_Info; }
};
