
#pragma once 

class CsWorldMap
{
public:
	CsWorldMap();
	~CsWorldMap();

	typedef	std::map< DWORD, CsWorldMap* >					MAP;
	typedef	std::map< DWORD, CsWorldMap* >::iterator		MAP_IT;
	typedef	std::map< DWORD, CsWorldMap* >::const_iterator	MAP_CIT;

public:
	struct sINFO
	{
		sINFO():s_nID(0),s_nWorldType(0),s_nUI_X(0),s_nUI_Y(0)
		{
		}

		USHORT		s_nID;
		TCHAR		s_szName[ FT_WORLDMAP_NAME_LEN ];
		TCHAR		s_szComment[ FT_WORLDMAP_COMMENT_LEN ];

		BYTE		s_nWorldType;

		USHORT		s_nUI_X;
		USHORT		s_nUI_Y;
	};

public:
	void						Delete();
	void						Init( sINFO* pSrcInfo );
	sINFO*						GetInfo();
	void						AddArea( CsAreaMap* pArea );
	std::list< CsAreaMap* >*	GetAreaList();
	CsAreaMap*					GetArea( int nIndex );

protected:
	sINFO						m_Info;
	std::list< CsAreaMap*>		m_listArea;
};

