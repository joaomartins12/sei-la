

#pragma once 

class CsSortObj : public NiMemObject
{
public:	
	CsSortObj():m_bTempAlpha(false) {};
	virtual ~CsSortObj() {};
	
	CsVectorPB< CGeometry* >		m_vpGeom;
	
	bool							m_bTempAlpha;

public:
	virtual void				Delete();
	void						Delete_IsFlag( DWORD dwPlag );

	float						CalCameraDist();

	virtual void				Render();
	void						RenderSort(float camDist);	//2015-11-30-nova 여기서는 Sort만 하고 EndRender에서 그려준다. camDist는 라인의 굵기
	void						RenderAbsolute();
	void						RenderAbsolute_ExceptPlag( DWORD dwPlag );
	void						SetTempAlpha( bool bAlpha );

	/*void						SetDimmer( float fDimmer );
	float						GetDimmer();*/

	void						SetShader( char* cSkinShader, char* cMeshShader );	
	void						SetShader( NiSingleShaderMaterial* pSkinShader, NiSingleShaderMaterial* pMeshShader );

	char*						ShaderPlagStr( CGeometry* pGeom, char* cShader, int nLen );
};



class CsNodeObj : public CsSortObj
{
public:
	CsNodeObj(){ m_pNiNode = NULL; }
	virtual ~CsNodeObj(){ m_pNiNode = NULL; };

public:
	NiNodePtr							m_pNiNode;	

public:	
	void					ChangeShader( int nGeomType, bool bSorting );	
	void					ActiveShader();
	void					SetNiObject( NiNodePtr pObject, int nGeomType, bool bSorting = false );
	void					SetNiObject_IncludeHide( NiNodePtr pObject, int nGeomType );
	//void					SortingObject();

	void					ResetNiObject( int nGeomType );	

	virtual void			Delete(){ CsSortObj::Delete(); m_pNiNode = 0; }
	virtual void			Render(){ CsSortObj::Render(); }	

	void					Render_GeomData();
	void					Render_GeomData_ExceptPlag( DWORD dwPlag );
	void					Render_GeomDataShadow_ExceptPlag( DWORD dwPlag );

	void					SaveNiBit();
	bool					LoadNiBit();
};

class CsGeomObj : public CsSortObj
{
public:
	CsGeomObj(){ m_pNiGeom = NULL; }
	virtual ~CsGeomObj(){ m_pNiGeom = NULL; }

public:
	NiGeometryPtr				m_pNiGeom;

public:
	void					SetNiObject( NiGeometryPtr pObject, int nGeomType );
	virtual void			Delete(){ CsSortObj::Delete(); m_pNiGeom = 0; }
	virtual void			Render(){ CsSortObj::Render(); }
};

extern CsNodeObj* g_pTamerUser;