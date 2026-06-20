
#pragma once 

#define FT_TALK_DIGIMON_LEN			100

class CsTalk_Digimon
{
public:
	typedef	std::map< DWORD, CsTalk_Digimon* >				MAP;
	typedef	std::map< DWORD, CsTalk_Digimon* >::iterator	MAP_IT;
public:

	enum TP
	{
		TP_QUEST	=	0x00000001,
		TP_NPC		=	0x00000002,		 
		TP_DIGIMON	=	0x00000004,		 
		TP_SKILL	=	0x00000008,
	};

	struct sINFO
	{
		sINFO() { s_dwParam = s_nType = 0; }
		DWORD		s_dwParam;
		USHORT		s_nType;
		TCHAR		s_szText[ FT_TALK_DIGIMON_LEN ];
		TCHAR		s_szList[ FT_TALK_DIGIMON_LEN ];
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
	TCHAR*			GetText(){ return m_Info.s_szText; }
};

