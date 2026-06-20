
#pragma once 

class CsItemTap
{
public:
	typedef	std::map< DWORD, CsItemTap* >			MAP;
	typedef	std::map< DWORD, CsItemTap* >::iterator	MAP_IT;
public:
	struct sINFO
	{
		USHORT		s_nSellClass;
		TCHAR		s_szName[ 32 ];
	};
protected:
	sINFO			m_Info;	

public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );

	sINFO*			GetInfo(){ return &m_Info; }
};

// Rank테이블 load관련 추가	lks007
class CsItemRank
{
public:
	typedef	std::map< int, CsItemRank* >			MAP;
	typedef	std::map< int, CsItemRank* >::iterator	MAP_IT;

public:

	struct sRANK
	{
		USHORT		s_nDropType;
		USHORT		s_nDorpCount;
	};

protected:
	sRANK					m_Rank;

public:
	void			Init( sRANK* pSrcRank );
	sRANK*			GetRank() { return &m_Rank; }
	USHORT			GetCount() { return m_Rank.s_nDorpCount; }
};

struct sRankInfo
{
	sRankInfo() : dwItemCode( 0 ), dwIconNo( 0 ), nRank( 0 ){}
	~sRankInfo() {}

	DWORD		dwItemCode;
	DWORD		dwIconNo;
	int			nRank;
};

typedef std::list< sRankInfo >					LIST_RANK_INFO;
typedef std::list< sRankInfo >::iterator		LIST_RANK_INFO_IT;
typedef std::list< sRankInfo >::const_iterator	LIST_RANK_INFO_CIT;
typedef std::map< DWORD, LIST_RANK_INFO >					MAP_RANK_EFFECT;
typedef std::map< DWORD, LIST_RANK_INFO >::iterator			MAP_RANK_EFFECT_IT;
typedef std::map< DWORD, LIST_RANK_INFO >::const_iterator	MAP_RANK_EFFECT_CIT;