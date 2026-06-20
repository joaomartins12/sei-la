#pragma once

class CsSceneDataMng 
{
public:
	CsSceneDataMng(void);
	~CsSceneDataMng(void);

	struct sTargetData 
	{
	public:
		DWORD s_dwUID;		 // 타겟 ID
		DWORD s_dwModelID;		 // 모델 ID

		float s_fDist;			// 카메라거리
		float s_fHeight;		// 카메라 높이

		TCHAR s_szName[ 32 ];	// 타겟 이름
	};

public:
	typedef std::map< DWORD, CSceneData* >						MAP; 
	typedef std::map< DWORD, CSceneData* >::iterator			MAP_IT;

	typedef std::map< DWORD, sTargetData* >						MAP_TAR;
	typedef	std::map< DWORD, sTargetData* >::iterator			MAP_TAR_IT;			

public:
	void	Delete();		
	bool	Init( char* cPath );

protected:
	MAP_TAR		m_mapTargeData;
	MAP			m_mapSceneData;	

public:
	bool			IsTarget( DWORD dwID ){ return ( m_mapTargeData.find( dwID ) != m_mapTargeData.end() ); }
	sTargetData*	GetSceneTarget( DWORD dwID );
	MAP_TAR*		GetTargetMap() { return &m_mapTargeData; }

	bool			IsData( DWORD dwID ){ return ( m_mapSceneData.find( dwID ) != m_mapSceneData.end() ); }
	CSceneData*		GetSceneData( DWORD dwID );	
	MAP*			GetSceneMap() { return &m_mapSceneData;	}

protected:
	void			_LoadExcel();

	void			_LoadExcelTargetData();
	void			_LoadExcelSceneData();
	void			_LoadExcelSceneInfo();

	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	void			SaveBin( char* cPath );	
	void			ReLoad();


};

extern CsSceneDataMng*		 g_pSceneDataMng;