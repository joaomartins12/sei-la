
#pragma once

class CsGBTerrainMng : public NiMemObject
{
protected:
	CsGBTerrainMake*			m_pTerrainMake;
	CsGBTerrainRoot*			m_pOldRoot;

	DWORD						m_dwBackupMapID;
	
public:
	CsGBTerrainMng();
	~CsGBTerrainMng();

public:
	static void		InitMempool();	
	static void		DeleteMempool();	


public:
	/*sMAPPORTAL*		GetNearPortal( NiPoint2 ptSrc );
	DWORD			GetPortalCount( DWORD dwMapID );*/

	// ========= 기본 함수
public:	
	void		Delete();
	void		Init( CsGBDX9Renderer* pRenderer, int nMultiTextureCount, bool bPixelShader2, bool bUseMyShader,
					  float fViewDetailDist = 10000.0f, float fViewVisibleDist = 40000.0f );
	
	bool		CreateRoot_ByTool( const char* cFile, nsCsGBTerrain::eCREATE_TYPE ct, DWORD dwMapID );
	bool		CreateRoot_ByTool( DWORD dwMapID );
	bool		CreateRoot( const char* cFile, nsCsGBTerrain::eCREATE_TYPE ct );
	bool		CreateRoot( DWORD dwMapID );

private:
	void		_UpdateOclusionObject();

public:
	void		Update( float fElapsedTime ){ nsCsGBTerrain::g_pCurRoot->Update( fElapsedTime ); }
	void		Update_For_Shadow( float fElapsedTime ){ nsCsGBTerrain::g_pCurRoot->Update_For_Shadow( fElapsedTime ); }
	void		Update_For_Shadow_ExceptionObject( float fElapsedTime, CsGBObject* pExceptionObj ){ nsCsGBTerrain::g_pCurRoot->Update_For_Shadow_ExceptionObject( fElapsedTime, pExceptionObj ); }
	void		Update_ByTool( float fElapsedTime ){ nsCsGBTerrain::g_pCurRoot->Update_ByTool( fElapsedTime ); }

	void		BeginRender(){ nsCsGBTerrain::g_pCurRoot->BeginRender(); }
	void		EndRender(){ nsCsGBTerrain::g_pCurRoot->EndRender(); }
	void		Render( bool bClearGetter = true ){ nsCsGBTerrain::g_pCurRoot->Render( bClearGetter ); }	
	void		Render_Shader( bool bClearGetter, char* cSkinShader, char* cObjectShader ){ nsCsGBTerrain::g_pCurRoot->Render_Shader( bClearGetter, cSkinShader, cObjectShader ); }	
	void		RenderTerrain();
	/*void		RenderTBP( float fElapsedTime );*/	
	void		RenderObject( bool bClearGetter = true ){ nsCsGBTerrain::g_pCurRoot->RenderObject( bClearGetter ); }	
	//void		RenderGetterOnly( bool bClearGetter = true ){ nsCsGBTerrain::g_pCurRoot->RenderGetterOnly( bClearGetter ); }

	void		RenderTerrain_ByTool( bool bShowSkyBox );	
	void		RenderObject_ByTool( float fElapsedTime, bool bClearGetter = true );


	DWORD		GetBackupMapID(){ return m_dwBackupMapID; }
	
	// ====== SpeedTree
protected:
//	CsSpeedTreeMng		m_STMng;

	// ====== RootNode
	// == Delete 
public:
	void		DeleteRoot( CsGBTerrainRoot** ppRoot );
protected:
	void		_NewRoot();	

	// ====== Make
protected:
	CsGBTerrainMake*	_GetTerrainMake();
public:
	void		CreateMakeTerrain( CsGBTerrainRoot::sINFO* pRootInfo, LPCTSTR szBaseDetailMap );
	bool		ApplyHeightMap( LPCTSTR szMapPath );

	// ====== File	
public:
	void				DeleteTerrainFile( LPCTSTR szFile );
};

namespace nsCsGBTerrain
{	
	extern CsGBTerrainMng*		g_pTRMng;

	struct sSERVERTIME
	{
		sSERVERTIME(){ s_nServerTime = 0; }
		UINT		s_nServerTime;
		UINT		GetOneDayTime(){ return s_nServerTime % 86400; }			// 너무 큰값이라 오차를 줄이기 위해서
		UINT		GetServerTime(){ return s_nServerTime; }
		float		GetInitLoopAccumTime( float fLoopTime ){ return (float)( GetOneDayTime() - (double)( (int)( GetOneDayTime() / fLoopTime )*fLoopTime ) ); }	// 동기화 시간에 쓰는 초기 시간 계산용
	};
	extern sSERVERTIME			g_ServerTime;	
}