
#pragma once

#pragma pack(4)

class cResist
{
public:
	cResist();
	~cResist();

public:
#define	RESIST_PATH				L".\\ToolRgs\\MapTool.rgs"
#define RESIST_VERSION			0x00000108
#define RESIST_STRUCT_COUNT		12

	struct sSTRUCT_HEADER{
		DWORD	s_dwStructSize;
	};
	struct sHEADER : public sSTRUCT_HEADER{
		void Reset();
		DWORD	s_dwVersion;
		DWORD	s_dwStructCount;
	};	
	struct sVIEW_STATE : public sSTRUCT_HEADER{
		void Reset();
	};
	struct sMAINMENU : public sSTRUCT_HEADER{
		void Reset();
		TCHAR s_szSaveLoad[ MAX_PATH ];
		TCHAR s_szCMI[ MAX_PATH ];
	};
	struct sNEW_TERRAIN : public sSTRUCT_HEADER{
		void Reset();
		int s_nWidth;
		int s_nHeight;
		int s_nGrid;
		int s_nTreeLevel;
		int s_nBaseMPPG;
		int s_nAlphaMPPG;
	};
	struct sHEIGHT_MAP : public sSTRUCT_HEADER{
		void Reset();
		float s_fHeightMapRate;
		TCHAR s_szFile[ MAX_PATH ];
		TCHAR s_szPath[ MAX_PATH ];
	};
	struct sMAPPING : public sSTRUCT_HEADER{
		void Reset();
		TCHAR s_szTexturePath[ MAX_PATH ];
		float s_fAlphaDecreaseConstant;
		int s_nMappingSize;
		int s_nMappingDecrement;
		float s_fGrayDecreaseConstant;
		int s_nGraySize;
		int s_nGrayDecrement;
		//BYTE s_btGrayColor;
		NiColor s_GrayColor;
	};
	struct sOBJECT : public sSTRUCT_HEADER{
		void Reset();
		TCHAR s_szObjectPath[ MAX_PATH ];
	};	
	struct sETC : public sSTRUCT_HEADER{
		void Reset();
		// 미니맵
		TCHAR s_szMinimapPath[ MAX_PATH ];
		// 이동가능지역
		TCHAR s_szEMRPath[ MAX_PATH ];
	};
	struct sSKYBOX : public sSTRUCT_HEADER{
		void Reset();
		TCHAR s_szSkyBoxPath[ MAX_PATH ];
	};
	struct sMAP : public sSTRUCT_HEADER{
		void Reset();
		TCHAR s_szBGSound[ MAX_PATH ];
	};
	struct sPATH : public sSTRUCT_HEADER{
		void Reset();
		int	s_nTerrainPathHeight;
		int	s_nObjectPathHeight;
		int	s_nMaxPathHeight;
	};
	struct sMONSTER : public sSTRUCT_HEADER{
		void Reset();
		int	s_nDefRadius;
		int	s_nDefMonCount;
		int	s_nDefMonResfonTime;
		float s_fViewMonScale;
		int	s_nDefMove;
		bool s_bInsRegen;
	};

protected:
	sHEADER			m_Header;
	sVIEW_STATE		m_ViewState;
	sMAINMENU		m_MainMenu;
	sNEW_TERRAIN	m_NewTerrain;
	sHEIGHT_MAP		m_HeightMap;	
	sMAPPING		m_Mapping;
	sOBJECT			m_Object;
	sSKYBOX			m_SkyBox;
	sETC			m_Etc;
	sMAP			m_Map;
	sPATH			m_Path;
	sMONSTER		m_Monster;

public:	
	sNEW_TERRAIN*	GetNewTerrain(){ return &m_NewTerrain; }
	sHEIGHT_MAP*	GetHeightMap(){ return &m_HeightMap; }
	sVIEW_STATE*	GetViewState(){ return &m_ViewState; }
	sMAPPING*		GetMapping(){ return &m_Mapping; }
	sMAINMENU*		GetMainMenu(){ return &m_MainMenu; }
	sOBJECT*		GetObject(){ return &m_Object; }
	sSKYBOX*		GetSkyBox(){ return &m_SkyBox; }
	sETC*			GetEtc(){ return &m_Etc; }
	sMAP*			GetMap(){ return &m_Map; }
	sPATH*			GetPath(){ return &m_Path; }
	sMONSTER*		GetMonster(){ return &m_Monster; }
	

public:
	void	Reset();
	void	Save();
	void	Load();

};

extern cResist			g_Resist;