
#pragma once

class CsGBTerrainLeaf : public CsGBTerrainNode
{
public:
	CsGBTerrainLeaf();
	~CsGBTerrainLeaf();

public:	
	struct sNiTriINFO
	{
		sNiTriINFO();
		~sNiTriINFO();

		void		Reset();
		void		Delete();
		
		NiPoint2			s_ptOffsetPos;
		int					s_nVertCount;
		int					s_nTemp1;				// 이거도 남는다
		float				s_fTemp2;				// 이거도 남는다
		unsigned short		s_usNiTerrainIndex;		

		// ==== Object
		DWORD				s_dwObjectCount;

		// ==== Texture
		char				s_cDetail_Map0[ MAX_PATH ];
		char				s_cDetail_Map1[ MAX_PATH ];
		char				s_cDetail_Map2[ MAX_PATH ];
		char				s_cDetail_Map3[ MAX_PATH ];
		char				s_cDetail_Map4[ MAX_PATH ];

		// ==== Shader
		int					s_nShaderCord;

		// ==== Data
		float*				s_pfHeight;
	};

	struct sLEAF_TOOL_INFO
	{
		sLEAF_TOOL_INFO():s_nHaveChildObjectCount(0) {};

		int					s_nHaveChildObjectCount;
	};

	// ==== Base 
public:
	virtual void				_Init();
	virtual void				Delete();

	void						MakeNiTerrain( CsNiNodePtr pNiNode, sNiTriINFO* pNiTriInfo );

	virtual CsGBTerrainNode*	Create( eQUADT_ID const& eQuadID, CsNiNodePtr pBaseNiNode, CsGBTerrainNode* pRootNode, 
										CsGBTerrainNode* pParentNode, int const& nTreeLevel, int const& nNodeLevel  );

	virtual void				UpdateObject( float fElapsedTime );
	virtual void				UpdateObject_For_Shadow( float fElapsedTime );
	virtual void				UpdateObject_For_Shadow_ExceptionObject( float fElapsedTime, CsGBObject* pExceptionObj );

	// ==== NodeState
protected:
	eNODE_STATE					m_eNodeState;
public:
	virtual void				SetViewerPos( NiPoint2 ptPos );
	void						SetNodeState( eNODE_STATE ns );


	// ==== Terrain	
public:
	virtual bool				IsLeaf(){ return true; }

	// ==== NiNode
protected:
	NiTriStrips*				m_pTriStrip;
public:
	virtual void				UpdateBuffer();
	virtual void				BeginRenderTR();
	virtual void				EndRender(){ m_bEnableRender = false; }
	NiTriStrips*				GetNiTriStrip(){ return m_pTriStrip; }
protected:
	void						_SetNiNodeTranslate();	
	void						_SetNiNodeName();
	void						_SetNiTriObjectName();

	// ==== Object
public:
	static DWORD				m_dwObjectUniqIDConstant;
protected:
	MAP_GBTERRAIN_OBJECT		m_mapObject;
	MAP_GBTERRAIN_OBJECT		m_mapBasePickObj;
	MAP_GBTERRAIN_OBJECT		m_mapCameraAlphaObj;
	MAP_GBTERRAIN_OBJECT		m_mapPortalObj;
	MAP_GBTERRAIN_OBJECT		m_mapCameraCollision;

	MAP_GBTERRAIN_OBJECT		m_mapQuestOn;
	MAP_GBTERRAIN_OBJECT		m_mapQuestOff;

protected:
	void						_DeleteObject();
	void						_ResistObject( CsGBObject* pObject );
public:
	MAP_GBTERRAIN_OBJECT*		GetObjectMap(){ return &m_mapObject; }
	MAP_GBTERRAIN_OBJECT*		GetBasePickObjMap(){ return &m_mapBasePickObj; }
	MAP_GBTERRAIN_OBJECT*		GetCameraAlphaObjMap(){ return &m_mapCameraAlphaObj; }
	MAP_GBTERRAIN_OBJECT*		GetPortalObjMap(){ return &m_mapPortalObj; }
	MAP_GBTERRAIN_OBJECT*		GetCameraCollisionObjMap(){ return &m_mapCameraCollision; }

	virtual bool				ResistObject( CsGBObject* pObject );	
	virtual CsGBObject*			FindCsGBObject( NiNode* pNiNode );
	virtual CsGBObject*			FindCsGBObject( DWORD dwUniqObjectID );
	virtual void				DeleteCsGBObject( DWORD dwUniqObjectID );
	virtual inline void			ObjectResetCallReady();
			void				ForceResistObject( CsGBObject* pObject ){ _ResistObject( pObject ); }

	DWORD						CheckPortal( float fElapsedTime, NiBoundingVolume* pNiBoundingVolume );
	// Quest
	virtual void				CheckQuestObject();
	virtual void				CheckQuestObject( int nCompQuest );

	// ==== Texture
public:
	virtual void				ResetTextureFilterMode();

	// ==== Shader
public:
	virtual void				ResetShader();

	// ==== Light
public:
	virtual	void				ApplyLight_Object( CsGBLight* pLight );
	virtual	void				ReleaseLight_Object( CsGBLight* pLight );

	// ==== NiTriInfo
protected:
	sLEAF_TOOL_INFO*			m_pLeafToolInfo;
	sNiTriINFO					m_NiTriInfo;
public:
	sNiTriINFO*					GetNiTriInfo(){ return &m_NiTriInfo; }
	sLEAF_TOOL_INFO*			GetLeafToolInfo(){ return m_pLeafToolInfo; }

	// ==== Texture
public:
	void						SetBaseMap( NiPixelData* pData );
	void						SetShaderMap( int nShaderIndex, NiPixelData* pPData );
	void						SetShaderMap( int nShaderIndex, NiRenderedTexture* pTex );
	void						SetDetail_Map0_ByTool( char* cpTex );
	void						SetDetail_Map1_ByTool( char* cpTex );
	void						SetDetail_Map2_ByTool( char* cpTex );
	void						SetDetail_Map3_ByTool( char* cpTex );
	void						SetDetail_Map4_ByTool( char* cpTex );
	void						CreateShadowMap_ByTool();
protected:
	void						_SetNiNodeTexture_ByTool( bool bCreateShadowMap = false );

	// ==== Effect
protected:
	NiSingleShaderMaterial*		m_pWorldShaderMaterial;
	NiSingleShaderMaterial*		m_pDetailShaderMaterial;
	NiSingleShaderMaterial*		m_pDetailShadowShaderMaterial;
protected:
	void						_CreateShaderMaterial();

	// ==== Picking
public:
	virtual void			Pick_Terrain( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir );
	virtual void			Pick_TBP( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir );

	virtual void			Pick_T_TB_TBP_ByTool( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir );
	virtual void			Pick_T_ByTool( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir );
	virtual void			Pick_T_TB_TBP_CAP( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir );
	virtual void			Pick_T_TB_TBP_CAP_ByTool( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir );

	virtual void			ResetObject_Picking();

	virtual void			Reset_TBP_Picking();
	virtual void			Reset_TBP_CAP_Picking();
	virtual void			Reset_TB_TBP_Picking();
	virtual void			Reset_TB_TBP_CAP_Picking();

	bool					GetTBPickHeight( NiPoint2 ptPos, float& fTBPickHeight );

	// ==== ExtraData
public:
	virtual void				SaveExtraData( CsNiNodePtr pBaseNiNode );	
protected:
	virtual void				_LoadExtraData( CsNiNodePtr pBaseNiNode );
	void						_LoadExtraData_Obj( CsNiNodePtr pBaseNiNode );
	void						_LoadExtraData_Obj_Tool( CsNiNodePtr pBaseNiNode );
	void						_LoadExtraData_TableObj();
	void						_LoadExtraData_TableObj_Tool();

public:
	virtual void				SaveToolExtraData( CsNiNodePtr pBaseNiNode );
	virtual void				LoadToolExtraData( CsNiNodePtr pBaseNiNode );
};

#include "CsGBTerrainLeaf.inl"