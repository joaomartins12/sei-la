
#pragma once 
#ifdef SIMPLE_TOOLTIP
#define FT_TOOLTIP_TITLE_BODY_LEN	512
#define FT_TOOLTIP_MSG_BODY_LEN	1024

class CsTalk_Tooltip
{
public:
	typedef	std::map< DWORD, CsTalk_Tooltip* >				MAP;
	typedef	std::map< DWORD, CsTalk_Tooltip* >::iterator	MAP_IT;
public:	


	struct sINFO
	{
		DWORD			s_dwID;
		TCHAR			s_Title[ FT_TOOLTIP_TITLE_BODY_LEN ];
		TCHAR			s_Message[ FT_TOOLTIP_MSG_BODY_LEN ];
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

#endif