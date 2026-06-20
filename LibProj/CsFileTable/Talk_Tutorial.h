
#pragma once 


#define FT_TALK_TUTORIAL_LEN		256

class CsTalk_Tutorial
{
public:
	typedef	std::map< DWORD, CsTalk_Tutorial* >				MAP;
	typedef	std::map< DWORD, CsTalk_Tutorial* >::iterator	MAP_IT;
public:
	struct sINFO
	{
		TCHAR		s_szText[ FT_TALK_TUTORIAL_LEN ];
	};
protected:
	sINFO			m_Info;

public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};