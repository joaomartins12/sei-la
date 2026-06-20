
#pragma once 

#define FT_TALK_TIP_LEN			200

class CsTalk_Tip
{
public:
	typedef	std::map< DWORD, CsTalk_Tip* >				MAP;
	typedef	std::map< DWORD, CsTalk_Tip* >::iterator	MAP_IT;
public:
	struct sINFO
	{
		TCHAR		s_szTip[ FT_TALK_TIP_LEN ];
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};


class CsLoading_Tip
{
public:
	typedef	std::map< DWORD, CsLoading_Tip* >				MAP;
	typedef	std::map< DWORD, CsLoading_Tip* >::iterator	MAP_IT;
public:
	struct sINFO
	{
		TCHAR		s_szLoadingTip[ FT_TALK_TIP_LEN ];
		int			s_nLevel;
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};
