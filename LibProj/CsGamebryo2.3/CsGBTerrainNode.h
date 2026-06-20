
#pragma once

class CsGBTerrainLeaf;
class CsGBTerrainNode : public NiMemObject
{
public:
	struct sINFO
	{
		sINFO() : s_bExistChild(false),s_fHalfRadius(0.0f)
		{
			s_Bound.Reset();
		}
		~sINFO() {};

		void Reset(){
			s_Bound.Reset();
			s_bExistChild = false;			
			s_fHalfRadius = 0.0f;
			s_ptCenterPos = NiPoint2::ZERO;
		}
		void CalCenter(){
			s_ptCenterPos = ( s_Bound.s_ptMin + s_Bound.s_ptMax )*0.5f;
			s_fHalfRadius = ( s_ptCenterPos - s_Bound.s_ptMin ).Length();
		}

		bool		s_bExistChild;

		NiPoint2	s_ptCenterPos;
		float		s_fHalfRadius;
		// ==== Bound
		nsCSGBFUNC::sTERRAIN_BOUND		s_Bound;
	};

public:
	enum eQUADT_ID{ QI_LD = 0, QI_RD, QI_LT, QI_RT, QI_COUNT, QI_ROOT, QI_NONE };
	enum eNODE_STATE{ NS_NONEVISIBLE, NS_WORLD, NS_DETAIL, };

public:
	CsGBTerrainNode();
	virtual ~CsGBTerrainNode();	



	// ==== Base Func
protected:
	char						m_cQuadKey[ 32 ];
	int							m_nQuadKey;
	eQUADT_ID					m_eQuadID;
	CsGBTerrainNode*			m_pRootNode;
	CsGBTerrainNode*			m_pParentNode;
	CsGBTerrainNode*			m_pChildNode[ QI_COUNT ];
	int							m_nNodeLevel;
	int							m_nSerchIndexConstant;

public:
	CsGBTerrainLeaf*			GetNode( int const& nX, int const& nY );
	CsGBTerrainNode*			GetParent(){ return m_pParentNode; }
	char*						GetCharQuadKey(){ return m_cQuadKey; }
	eQUADT_ID					GetQuadID(){ return m_eQuadID; }
	int							GetIntQuadKey(){ return m_nQuadKey; }


public:
	virtual CsGBTerrainNode*	Create( eQUADT_ID const& eQuadID, CsNiNodePtr pBaseNiNode, CsGBTerrainNode* pRootNode,
										CsGBTerrainNode* pParentNode, int const& nTreeLevel, int const& nNodeLevel );

	virtual void				Delete();
	virtual void				UpdateTerrain( float fElapsedTime );
	virtual void				UpdateObject( float fElapsedTime );
	virtual void				UpdateObject_For_Shadow( float fElapsedTime );
	virtual void				UpdateObject_For_Shadow_ExceptionObject( float fElapsedTime, CsGBObject* pExceptionObj );

	/*virtual void				Update_RenderObject( float fElapsedTime );
	virtual void				Update_RenderObject_TBP( float fElapsedTime );
	virtual void				Update_RenderObject_NoClipping( float fElapsedTime );*/
	void						Render();
	void						Render_Shader( char* cShader );

protected:
	virtual void				_Init();
	void						_CreateChild( int const& nTreeLevel, CsNiNodePtr pBaseNiNode );

	// ==== NodeState
public:
	virtual void				SetViewerPos( NiPoint2 ptPos );

	// ==== Terrain
protected:
	bool						m_bVisible;
public:
	virtual bool				IsLeaf(){ return false; }
	void						SetVisible( bool bVisible );
	bool						IsVisible();

	// ==== NiNode
protected:
	CsNiNodePtr					m_pNiNode;
	bool						m_bEnableRender;
public:
	virtual void				BeginRenderTR();

	virtual void				EndRender();	
	virtual void				UpdateBuffer();
	CsNiNodePtr					GetNiNode();
	CsGBTerrainNode*			GetChildNode( DWORD dwChildID );
	bool						IsEnableRender();

protected:
	void						_CreateNiNode( eQUADT_ID const& eQuadID );	

	// ==== Texture
public:
	virtual void				ResetTextureFilterMode();

	// ==== Shader
public:
	virtual void				ResetShader();

	// ==== Object
public:
	virtual bool				ResistObject( CsGBObject* pObject );
	virtual CsGBObject*			FindCsGBObject( NiNode* pNiNode );
	virtual CsGBObject*			FindCsGBObject( DWORD dwUniqObjectID );
	virtual void				DeleteCsGBObject( DWORD dwUniqObjectID );
	virtual void				ObjectResetCallReady();
	virtual void				SaveObjectResetCallReady();
	virtual void				LoadObjectResetCallReady();
	// Quest
	virtual void				CheckQuestObject();
	virtual void				CheckQuestObject( int nCompQuest );

	// ==== Light
public:
	virtual	void				ApplyLight_Object( CsGBLight* pLight );
	virtual	void				ReleaseLight_Object( CsGBLight* pLight );

	// ==== Info
protected:
	sINFO						m_Info;
public:
	inline sINFO*				GetInfo(){ return &m_Info; }
	inline bool					IsExistTerrain(){ return m_Info.s_bExistChild; }
	void						CalBound( nsCSGBFUNC::sTERRAIN_BOUND Bound );
protected:
	void						_SetExistTerrain();	

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

	//======= Extra Data
public:
	virtual void				SaveExtraData( CsNiNodePtr pBaseNiNode );
protected:
	virtual void				_LoadExtraData( CsNiNodePtr pBaseNiNode );

public:
	virtual void				SaveToolExtraData( CsNiNodePtr pBaseNiNode );
	virtual void				LoadToolExtraData( CsNiNodePtr pBaseNiNode );
};

#include "CsGBTerrainNode.inl"

	