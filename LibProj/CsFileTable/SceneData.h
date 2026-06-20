#pragma once

#define MAX_SCENE_CHARNUM 5

class CSceneData 
{
public:
	CSceneData(void);
	~CSceneData(void);

public:
	struct sSceneInfo
	{
	public:		
		DWORD			s_dwSceneID;	
		bool			s_bUse;
		TCHAR			s_szTitle[ 64 ];
		int				s_nMap;
		int				s_nResion;			// 지역 조건
		bool			s_bSkip;
		char			s_szMusic[ 64 ];	// Music
	};		

	struct sData
	{
	public:
		DWORD s_dwMainTarget;   // 행동 주체
		DWORD s_dwTarget[ MAX_SCENE_CHARNUM ];

		DWORD s_dwAni;				// Ani
		TCHAR s_szTalk[ 1024 ];		// Talk
		char  s_szEffect[ 64 ];		// Effect
		char  s_szSound[ 64 ];		// Sound
	};

protected:
	sSceneInfo						m_pInfo;
	std::list< sData* >				m_pListData;	

public:
	void	Delete();		
	
	sSceneInfo*				GetInfo() { return &m_pInfo; }	
	std::list< sData* >*	GetList() { return &m_pListData; }

	void			InsertListData( sData* pData ) { m_pListData.push_back( pData ); }	
};