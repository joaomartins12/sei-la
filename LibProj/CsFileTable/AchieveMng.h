
#pragma once

#define FT_ACHIEVE_NAME			64
#define FT_ACHIEVE_COMMENT		256
#define FT_ACHIEVE_TITLE		64

class CsAchieve
{
public:
	typedef	std::map< DWORD, CsAchieve* >				MAP;
	typedef	std::map< DWORD, CsAchieve* >::iterator		MAP_IT;
public:
	struct sINFO
	{
		DWORD		s_nQuestID;
		DWORD		s_nIcon;
		USHORT		s_nPoint;
		bool		s_bDisplay;

		TCHAR		s_szName[ FT_ACHIEVE_NAME ];
		TCHAR		s_szComment[ FT_ACHIEVE_COMMENT ];	
		TCHAR		s_szTitle[ FT_ACHIEVE_TITLE ];

		int			s_nGroup;
		int			s_nSubGroup;
		int			s_nType;
		int			s_nBuffCode;	// น๖วม ฤฺตๅ
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsAchieveMng
{
public:
	struct sTYPE
	{
		struct sINFO
		{
			sINFO(){ memset( s_szType, 0, sizeof( TCHAR )*32 ); }
			TCHAR					s_szType[ 32 ];
		};
		sINFO					s_Info;
		std::list< sTYPE* >		s_listChild;

		void		Delete();
		void		Save( FILE *fp );
		void		Load( FILE* fp );
		void		Load( int nHandle );
		sTYPE*		AddChild(){ sTYPE* pChild = csnew sTYPE; s_listChild.push_back( pChild ); return pChild; }
		sTYPE*		GetChild( int nIndex );
	};

public:
	void			Delete();
	bool			Init( char* cPath );

protected:
	CsAchieve::MAP	m_Map;
	sTYPE			m_RootType;

protected:
	void			_LoadExcel();
	void			_LoadExcel_Data();
	void			_LoadExcel_Achieve();

	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	bool			IsAchieve( int nQuestID );
	CsAchieve::MAP*	GetMap(){ return &m_Map; }
	sTYPE*			GetRootType(){ return &m_RootType; }
	CsAchieve*		GetAchieve( int nQuestID );

public:
	void			SaveBin( char* cPath );
};