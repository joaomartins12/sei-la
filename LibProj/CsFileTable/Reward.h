
#pragma once 

class CsReward
{
public:
	typedef	std::map< DWORD, CsReward* >				MAP;
	typedef	std::map< DWORD, CsReward* >::iterator		MAP_IT;
public:
	struct sINFO
	{
		USHORT		s_nID;
		TCHAR		s_szCommnet[ 1024 ];
		char		s_nStartDate[64];
		char		s_nExpDate[64];
		DWORD		s_nItemCode1;
		USHORT		s_nItemCount1;
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};
