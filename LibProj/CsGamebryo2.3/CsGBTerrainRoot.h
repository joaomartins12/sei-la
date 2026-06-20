
#pragma once

#define CsGBTerrainRootExtraKey			"RootInfo"
#define CsGBTR_FieldExtraKey			"FieldExtra"
#define CsGBTR_NoPickObjKey				"NoPickObj"
#define CsGBTR_PathExtraKey				"PathExtra"
#define CsGBTerrainRootToolExtraKey		"ToolRootInfo"

#define CsGBSkyBox_ExtraKey				"SkyBoxInfo"

#define CsGBLight_ExtraKey				"LightInfo"

#define CsGBCELL_ExtraKey				"CellObjectInfo"

class CsGBTerrainMng;
class CGeometryTerrain;
class CGeometryTREffect;
class CsGBTerrainRoot : public NiMemObject
{
public:
	CsGBTerrainRoot();
	~CsGBTerrainRoot();

public:

	struct sFIELD_INFO
	{
		sFIELD_INFO(){ s_pfHeightField = NULL; }
		void	Reset(){}
		void	Delete();
		
		float*				s_pfHeightField;
	};
	struct sTR_PATHINFO
	{
		enum eTYPE{ MESH4, MESH6, MESH8 };
		sTR_PATHINFO():s_nDataTotalCount(0),s_pType(NULL)
		{}

		void	Reset();
		void	Delete();

		CsGBObject::sPATHINFO	s_PathInfo;
		char*					s_pType;
		int						s_nDataTotalCount;
	};

	struct sINFO
	{		
		sINFO();
		~sINFO();

		void	Reset();
		void	Delete(){}
		DWORD	GetNewObjectID(){ return ++s_dwUniqObjectID; }
		void	CheckObjectID( DWORD nCheck ){ if( s_dwUniqObjectID < nCheck ) s_dwUniqObjectID = nCheck; }

		void	WorldPosToTexturePixel( int* xPixel, int* yPixel, NiPoint2 ptWorld, int nLeafIndex, int nMPPG );
		void	TexturePixelToWorldPos( float* xPos, float* yPos, int xPixel, int yPixel, int nLeafIndex, int nMPPG );
		void	CopyTextureBorder( int nMPPG, NiPixelData** ppPixelDataArray );

		int		GetTextureRightFigure( int nMPPG );
		int		GetTextureTopFigure( int nMPPG );
		int		GetTextureBottomFigure( int nMPPG );
		int		GetTextureLeftFigure( int nMPPG );

		int		CalNodeSizeX() const;
		int		CalNodeSizeY() const;
		int		CalSizeX() const;
		int		CalSizeY() const;

		// ===== Map
		DWORD	s_dwMapResVersion;
		
		// X Vert
		int		s_nXVertCount;
		int		s_nXVertCountInNode;
		// Y Vert
		int		s_nYVertCount;
		int		s_nYVertCountInNode;

		// Height
		float	s_fHeightRate;
		// Grid
		int		s_nGridSize;							// 한 패치의 그리드 넓이
		// Tree
		int		s_nTreeLevel;							// 쿼드트리 레벨
		int		s_nNodeCountInSide;						// 한변에서의 노드 갯수
		int		s_nTotalLeafCount;						// 총 리프 갯수 ( 빈노드 포함 )

		// Height
		TCHAR	s_szHeightMapFile[ MAX_PATH ];

		// Init Size
		int		s_nInitMapCoverSize;
		int		s_nInitMapSize;
		float	s_fDetailMapSize;
		
		// BaseMap
		int		s_nBaseMPPG;

		// AlphaMap
		int		s_nAlphaMPPG;		

		// ShadowMap
		int		s_nShadowMPPG;

		//====================================================================================
		// 더미
		//====================================================================================
		char	s_Dummy2[ 6 ];

		// EffectMap
		int		s_nEffectMPPG;
		
	protected:
		// Object
		DWORD	s_dwUniqObjectID;

	public:
		// ===== Fog
		NiColor				s_FogColor;
		float				s_fFogStart;
		float				s_fFogEndTerrain;
		float				s_fFogEndObject;

		// ===== Clipping
		float				s_fClipObject_Far;
		float				s_fClipTerrain_Far;

		DWORD				s_dwMapID;

		// ==== ShadowColor
		NiColor				s_ShadowColor;

		NiColor				s_BackGroundColor;

		// ===== Clip
		int					s_nClipCellObject;
		int					s_nCellObject_Size;

		// ==== Dummy
		char				s_Dummy[ 32 ];
	};

	struct sTOOL_INFO
	{
		sTOOL_INFO(){ s_dwMapToolResVersion = MAPTOOL_RES_VERSION; }
		DWORD				s_dwMapToolResVersion;
	};

public:
	void				Delete();
	void				Init();
	void				ReloadMap();

	void				CreateFromFile( CsNiNodePtr pNiNode, nsCsGBTerrain::eCREATE_TYPE ct );
	void				CreateFromMake( sINFO* pRootInfo );

protected:
	bool				m_bRenderObject;
protected:
	friend CsGBTerrainMng;
	void				Update( float fElapsedTime );
	void				Update_For_Shadow( float fElapsedTime );
	void				Update_For_Shadow_ExceptionObject( float fElapsedTime, CsGBObject* pExceptionObj );
	void				Update_ByTool( float fElapsedTime );

	void				BeginRender(){ ObjectResetCallReady(); }
	void				EndRender(){ m_RootNode.EndRender(); }
	void				Render( bool bClearGetter );
	void				Render_Shader( bool bClearGetter, char* cSkinShader, char* cObjectShader );
	void				RenderTerrain();
	void				RenderObject( bool bClearGetter );
	
	//void				RenderGetterOnly( bool bClearGetter );	

	virtual void		RenderTerrain_ByTool( bool bRenderSkyBox );
	virtual void		RenderObject_ByTool( float fElapsedTime, bool bClearGetter );

	//====== Node
protected:
	CsGBTerrainNode		m_RootNode;
public:
	CsGBTerrainNode*	GetRootNode(){ return &m_RootNode; }
	void				SetViewerPos( NiPoint3 ptPos );
	void				SetViewerPos_Tool( NiPoint3 ptPos );

	//====== NiNode
protected:
	CsNiNodePtr			m_pBaseNiNode;
public:
	void				SetBaseNiNode( NiTriStrips* pNiTriObject );
	CsNiNodePtr			GetCloneBaseNiNode();
	CsNiNodePtr			GetDeepCloneBaseNiNode();
	CsNiNodePtr			GetBaseNiNode(){ return m_pBaseNiNode; }
	
	//====== Pick
public:
	inline void					ResetObject_Picking(){ m_RootNode.ResetObject_Picking(); }

	bool					Pick_IsEnable( NiCamera* pNiCamera, int MouseX, int MouseY );
	bool					Pick_IsEnable( NiPoint3 origin, NiPoint3 dir );

	NiPick::Record*		Pick_Terrain( NiPoint3 origin, NiPoint3 dir );
	NiPick::Record*		Pick_Terrain( NiCamera* pNiCamera, int MouseX, int MouseY );
	NiPick::Record*		Pick_TBP( NiPoint3 origin, NiPoint3 dir );

	NiPick::Record*		Pick_T_TB_TBP_ByTool( NiPoint3 origin, NiPoint3 dir );
	NiPick::Record*		Pick_T_TB_TBP_ByTool( NiCamera* pNiCamera, int MouseX, int MouseY );	

	NiPick::Record*		Pick_T_ByTool( NiCamera* pNiCamera, int MouseX, int MouseY );

	NiPick::Record*		Pick_T_TB_TBP_CAP( NiPoint3 origin, NiPoint3 dir );
	NiPick::Record*		Pick_T_TB_TBP_CAP_ByTool( NiPoint3 origin, NiPoint3 dir );

	//====== Object
protected:
	MAP_GBTERRAIN_OBJECT		m_mapNoPick;
	MAP_GBTERRAIN_OBJECT		m_mapNoClipping;
	LIST_GBTERRAIN_OBJECT		m_listActorObj;

public:
	void					Insert_No_Pick( CsGBObject* pObject );
	void					Delete_No_Pick( CsGBObject* pObject );
	MAP_GBTERRAIN_OBJECT*	GetMap_No_Pick(){ return &m_mapNoPick; }

	void					Insert_No_Clipping( CsGBObject* pObject );	

	void					Insert_ActorObj( CsGBObject* pObject ){ m_listActorObj.push_back( pObject ); }

	CsGBObject*			ResistObject( CsGBObject::sINFO* pObjectInfo );
	void				ResistObject_ChangeRotation( CsGBObject::sINFO* pObjectInfo );
	CsGBObject*			FindCsGBObject( NiNode* pNiNode );
	CsGBObject*			FindCsGBObject( DWORD dwUniqObjectID );
	void				DeleteCsGBObject( DWORD dwUniqObjectID );
	void				ObjectResetCallReady();
	inline void			SaveObjectResetCallReady(){ m_RootNode.SaveObjectResetCallReady(); }
	inline void			LoadObjectResetCallReady(){ m_RootNode.LoadObjectResetCallReady(); }

	void				ConvertPortalID();
	DWORD				CheckPortal( float fElapsedTime, NiPoint3 ptPos, NiBoundingVolume* pNiBoundingVolume );
	DWORD				GetPortalTotalCount();
	CsGBObject*			GetPortal( DWORD dwPortalID );
	bool				GetPotalCheck( NiPoint2 pUserPos, float fDist );//2015.03.18(lks007)	위탁상점이 NPC 주변 및 포탈 주변의 가까운 위치에 생성되지 않도록... 

	void				ResetOption();

protected:
	void				_Resist_OutRegionObject( CsGBObject* pObject );


public:
	void				SetActor_Animation( DWORD nSeqID, char* cKey );
	LIST_GBTERRAIN_OBJECT*	GetList_ActorObj(){ return &m_listActorObj; }

	bool				CheckActorObject( NiPoint3 niStartPos, NiPoint3 niEndPos );
	NiPick::Record*		Pick_ActorObj( char const* cKey, NiPoint3 const& origin, NiPoint3 const&  dir);
	NiPick::Record*		Pick_ActorObj( char const* cKey, NiPoint3 const& origin, NiPoint3 const&  dir, float const& dist);

	CsGBObject*			GetActor_Object_NearPos( char* cKey, NiPoint2 pos );
	bool				GetActor_Object_2DPick( NiPoint2 & pickPos, char const* cKey, NiPoint2 const& StartPos, NiPoint2 const& TargetPos );
	void				DoAct_Object( CsGBObject::eACT_TYPE act, char* key );
	void				DoPath_Object( CsGBObject::ePATH_TYPE type, char* key );
	
	//===== Region
public:
	enum eCHECK_REGION{ CR_INVALIDE = -2, CR_WORLD = -1, };
protected:
	int					m_nOldCheckRegionID;
public:	
	int					CheckRegion( NiPoint3 ptPos, std::wstring& szDiscript,std::string& cBGSound );
protected:
	int					_WriteCheckRegion(	CsMapRegion::sINFO* pResInfo, int& nOldResIndex, int nCurResIndex,
											std::wstring& szDiscript, std::string& cBGSound );

	//====== Info
protected:
	sTOOL_INFO*			m_pToolInfo;

	enum eCALCULATE_NORMAL_SIDE{ CNS_LEFT, CNS_RIGHT, CNS_TOP, CNS_BOTTOM };
	sINFO				m_Info;
	sFIELD_INFO			m_FieldInfo;
	sTR_PATHINFO		m_TrPathInfo;
	float				m_fInverseGrid;
	float				m_fInverseXNodeSize;
	float				m_fInverseYNodeSize;
	float				m_fWidth;
	float				m_fHeight;
	float				m_fMinimapScale;
	float				m_fFogFactorObject;
	float				m_fFogFactorTerrain;

	float				m_fOrgClipObject_Far;
	float				m_fOrgFogObject_Start;
	float				m_fOrgFogObject_End;

	float*				m_pfBackupHeightField;
	float*				m_pfOrgHeightField;

	char				m_cFilePath[ MAX_PATH ];	

public:
	void				SetWireframe(bool bWireframe);
	sINFO*				GetInfo(){ return &m_Info; }
	sFIELD_INFO*		GetFieldInfo(){ return &m_FieldInfo; }
	sTR_PATHINFO*		GetTrPathInfo(){ return &m_TrPathInfo; }
	sTOOL_INFO*			GetToolInfo(){ return m_pToolInfo; }

	void				Alloc_HField_Normal();
	void				MakeOrgHeightField();
	void				MakeHeightField(bool UpdateObjectPos = true);
	//void				ResetHeightField_TBObject( CsGBObject* pTBObject );
	//void				CalculateNormal();
	//void				CheckInTerrainObject();

	void				ChangeMapUVSize( float fMapSize );
	void				UpdateMapUVSize();

	float				GetHeight( NiPoint2 ptWorld ){ return GetHeight( ptWorld.x, ptWorld.y ); }
	float				GetHeight( NiPoint3 ptWorld ){ return GetHeight( ptWorld.x, ptWorld.y ); }	
	float				GetHeight( float xWorld, float yWorld );	
	float				GetHeight( float xWorld, float yWorld, float* pfHeightField );
	float				GetHeight_Normal( float xWorld, float yWorld, NiPoint3& Normal );
	float				GetHeight_Normal( float xWorld, float yWorld, NiPoint3& Normal, float* pfHeightField );

	float				GetHeight_NoneTBP( float xWorld, float yWorld );
	float				GetHeight_NoneTBP( float xWorld, float yWorld, float* pfHeightField );

	bool				IsInTerrain( float xWorld, float yWorld );
	float				GetMapSizeWidth() const;
	float				GetMapSizeHeight() const;
	float				GetMinimapScale() const;
	char*				GetMinimapName( char* pOut, size_t siBufferSize );
	char*				GetZonemapName( int nMapID, char* pOut, size_t siBufferSize );
	char*				GetEMRPath( char* pOut, size_t siBufferSize );
	char*				GetMapName( char* pOut, size_t siBufferSize );
	const char*			GetMapFilePath(){ return m_cFilePath; }
	void				SetMapFilePath( char* pPaht ){ strcpy_s( m_cFilePath, MAX_PATH, pPaht ); }

	float*				GetOrgHeightField(){ return m_pfOrgHeightField; }
	float				GetOrgClipObject_Far() const { return m_fOrgClipObject_Far; }
	float				GetOrgFogObject_Start() const { return m_fOrgFogObject_Start; }
	float				GetOrgFogObject_End() const { return m_fOrgFogObject_End; }
	float				GetInverseGrid() const { return m_fInverseGrid; }

public:
	void				BackupHeightField();
	void				SaveOrgHeightField();
	//void				MakeHeightField_TBObject( CsGBObject* pTBObject, bool bOtherObjectReposition );		

protected:
	void				_InitDataValue();
	void				_ChangeUV( NiPoint2* pUV );
	void				_ChangeUV2( int iX, int iY, NiPoint2* pUV );
	//void				_CalculateNormal_Side( int nNodeIndex, NiPoint3** ppNormal, int nVertIndex, eCALCULATE_NORMAL_SIDE eCns );	

	//====== QuadTree
public:
	CsGBTerrainLeaf*	GetNode( int nObjectIndex );
	CsGBTerrainLeaf*	GetNode( int nX, int nY );
	CsGBTerrainLeaf*	GetNode( NiPoint3 WorldPos );

	//======= SkyBox
protected:
	CsGBSpeedCell*		m_pSpeedCell;
public:
	CsGBSpeedCell*		GetSpeedCell(){ return m_pSpeedCell; }

	//======= SkyBox
protected:
	CsGBSkyBox			m_SkyBox;
public:
	inline void			LoadSkyBox(){ m_SkyBox.Create(); }
	inline CsGBSkyBox*	GetSkyBox(){ return &m_SkyBox; }	
	//======= Fog
protected:
	NiFogPropertyPtr	m_pFogProp;
public:
	void				CalFogDateValue();
	//NiFogProperty*		GetFogProperty(){ return NiDynamicCast( NiFogProperty, m_pFogProp->Clone() ); }
	NiFogProperty*		GetFogProperty(){ return m_pFogProp; }
protected:
	void				_DeleteFog();
	void				_CreateFog();

	// ====== Texture
public:
	void				ResetTextureFilterMode();

	// ====== Shader
public:
	void				ResetShader();

//	// ====== Clip
//protected:
//	D3DXMATRIX			m_matInvPoseViewProj;
//	
//protected:
//	void				_CalClipViewProj();	
//	void				_SetFarClipPlane( float fFarClipDist, bool bSetShader );
//public:
//	inline void			EnableFarClipPlane(){ nsCsGBTerrain::g_pRenderer->GetD3DDevice()->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 ); }
//	inline void			ReleaseFarClipPlane(){ nsCsGBTerrain::g_pRenderer->GetD3DDevice()->SetRenderState( D3DRS_CLIPPLANEENABLE, 0 ); }
	

	//======= Light
protected:
	CsGBLightMng		m_LightMng;
public:	
	CsGBLightMng*		GetLightMng(){ return &m_LightMng; }
	void				ApplyLight_Object( CsGBLight* pLight ){ m_RootNode.ApplyLight_Object( pLight ); m_pSpeedCell->ApplyLight( pLight ); }
	void				ReleaseLight_Object( CsGBLight* pLight ){ m_RootNode.ReleaseLight_Object( pLight ); m_pSpeedCell->ReleaseLight( pLight ); }

	//======= TerrainPath
public:
	int					ApplyTerrainPath();

	// Quest
public:
	void				CheckQuestObject(){ m_RootNode.CheckQuestObject(); }
	void				CheckQuestObject( int nCompQuest ){ m_RootNode.CheckQuestObject( nCompQuest ); }

	//======= EMR Info
protected:
	UINT				m_nEmrBlockWidth;
	UINT				m_nEmrBlockHeight;
	UINT				m_nEmrBlockScale;
	char**				m_ppEmrBlock;

protected:
	void				_DeleteEmrFile();
public:
	void				LoadEmrFile( const char* cFile );
	
public:
	UINT				GetEmrBScale(){ return m_nEmrBlockScale; }
	UINT				GetEmrBWidth(){ return m_nEmrBlockWidth; }
	UINT				GetEmrBHeight(){ return m_nEmrBlockHeight; }

	inline bool			IsValidEmr( UINT x, UINT y );
	inline bool			IsValidEmr2( UINT x, UINT y );
	inline bool			IsBValidEmr( UINT bx, UINT by );
	
	bool				IsValidEmr( NiPoint2 v ){ return IsValidEmr( (UINT)v.x, (UINT)v.y ); }
	bool				IsValidEmr( NiPoint3 v ){ return IsValidEmr( (UINT)v.x, (UINT)v.y ); }	

	bool				IsReachableEmr( UINT x1, UINT y1, UINT x2, UINT y2);
	bool				IsReachableEmr( NiPoint2 v1, NiPoint2 v2 ){ return IsReachableEmr( (UINT)v1.x, (UINT)v1.y, (UINT)v2.x, (UINT)v2.y ); }
	bool				IsReachableEmr( NiPoint3 v1, NiPoint3 v2 ){ return IsReachableEmr( (UINT)v1.x, (UINT)v1.y, (UINT)v2.x, (UINT)v2.y ); }	

	//======= Save
protected:
	struct sCST_INFO
	{
		sCST_INFO():s_usNodeIndex(0),s_usShaderIndex(0)
		{}

		unsigned short	s_usNodeIndex;
		unsigned short	s_usShaderIndex;
	};
public:
	bool				Load( const char* cFile, nsCsGBTerrain::eCREATE_TYPE ct );
	void				PreSave( const char* cFile );
	void				Save( LPCTSTR szFile );	

	// Device
protected:
	void				_LoadDeviceSetting();

	// PathEngine
protected:
	void				_LoadPathEngine( const char* cFile);

	// Texture
protected:
	void				_LoadBMTexture( const char* cFile );
	void				_LoadAMTexture( const char* cFile );
	void				_LoadAMswTexture( const char* cFile );
	void				_LoadSMTexture( const char* cFile );
	void				_LoadSMorgTexture( const char* cFile );
	void				_LoadEMTexture( const char* cFile );

	void				_SaveSMorgTexture( LPCTSTR szFile );
	void				_SaveEMTexture( LPCTSTR szFile );
	void				_SaveSMTexture( LPCTSTR szFile, NiPixelDataPtr* ppMixSMPixelData );
	void				_SaveBaseTexture( LPCTSTR szFile, NiPixelDataPtr* ppMixSMPixelData );
	void				_SaveAMTexture( LPCTSTR szFile );
	void				_SaveAMswTexture( LPCTSTR szFile );

	// Extra Data	
protected:
	void				_SaveExtraData();
	void				_SaveToolExtraData();
public:
	bool				LoadExtraData( CsNiNodePtr pNiNode, const char* cFile );
	void				LoadToolExtraData( CsNiNodePtr pNiNode, const char* cFile );	

	//====== Terrain WireFrame
protected:
	NiWireframePropertyPtr	m_pWireframeProp;
public:
	void					ToggleWireFrame();
	bool					IsWireFrame();

	//======= Tool
public:
	void				SetRenderObject_ByTool( bool bRenderObject ){ m_bRenderObject = bRenderObject; }
	bool				GetRenderObject_ByTool(){ return m_bRenderObject; }	

	// ====== Version
protected:
	DWORD				m_dwOrgVersion;
protected:
	bool				_VersionUp( CsNiNodePtr pNiNode, const char* cFile, DWORD dwVersion );
public:
	DWORD				GetOriginalVersion(){ return m_dwOrgVersion; }
};

#include "CsGBTerrainRoot.inl"