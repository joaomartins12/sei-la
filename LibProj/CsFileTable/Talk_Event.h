#pragma once

#define FT_EVENTTALK_LEN			200

class CsTalk_Event
{
public:
	CsTalk_Event(void){}
	~CsTalk_Event(void){}

public:
	typedef	std::map< DWORD, CsTalk_Event* >				MAP;
	typedef	std::map< DWORD, CsTalk_Event* >::iterator		MAP_IT;

public:
	struct sINFO
	{
		DWORD		s_dwTalkNum;
		TCHAR		s_szText[ FT_EVENTTALK_LEN ];
	};

protected:
	sINFO			m_Info;

public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};






