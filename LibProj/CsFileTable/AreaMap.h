
#pragma once 

#define FT_WORLDMAP_NAME_LEN		48
#define FT_WORLDMAP_COMMENT_LEN		256

class CsAreaMap
{
public:
	enum eFIELD_TYPE
	{
		FT_None		=	0,
		FT_Dungeon	=	4,
	};

public:
	CsAreaMap() {};
	~CsAreaMap() {};

	typedef	std::map< DWORD, CsAreaMap* >					MAP;
	typedef	std::map< DWORD, CsAreaMap* >::iterator			MAP_IT;
	typedef	std::map< DWORD, CsAreaMap* >::const_iterator	MAP_CIT;

public:
	struct sINFO
	{
		sINFO():s_nMapID(0),s_nAreaType(0),s_nFieldType(0),s_nFTDetail(0),s_nUI_X(0),s_nUI_Y(0)
		{
		}

		USHORT		s_nMapID;
		TCHAR		s_szName[ FT_WORLDMAP_NAME_LEN ];
		TCHAR		s_szComment[ FT_WORLDMAP_COMMENT_LEN ];

		BYTE		s_nAreaType;
		BYTE		s_nFieldType;
		int			s_nFTDetail;

		USHORT		s_nUI_X;
		USHORT		s_nUI_Y;

		float		s_fGaussianBlur[ 3 ];
	};

protected:
	sINFO			m_Info;

public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

