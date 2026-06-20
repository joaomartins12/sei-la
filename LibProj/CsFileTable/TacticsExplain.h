
#pragma once 

#define IF_TACTICS_EXPLAIN_LEN		512

class CsTacticsExplain
{
public:
	typedef	std::map< DWORD, CsTacticsExplain* >				MAP;
	typedef	std::map< DWORD, CsTacticsExplain* >::iterator		MAP_IT;
public:
	struct sINFO
	{
		DWORD s_dwTacticsMonID;
		TCHAR s_szTacticsName[ 64 ];
		TCHAR s_szTacticsExplain[ IF_TACTICS_EXPLAIN_LEN ];
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

