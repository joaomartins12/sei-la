
#pragma once 

class cTempRoot : public CsGBTerrainRoot
{
public:
	cTempRoot();

public:
	enum eTEMPROOT_TYPE
	{
		TT_NONE, TT_PORTALOBJ, TT_NPC_PORTAL,
	};

protected:
	CsGBTerrainRoot*	m_pOrgRoot;
	CsPathEngine*		m_pCsPathEngine;
	eTEMPROOT_TYPE		m_eTempRootType;

	CsGBObj_Portal::sTARGET*	m_pTarget;
	int*						m_pX;
	int*						m_pY;
	int*						m_pRadius;

	int						m_ViewX;
	int						m_ViewY;

	bool				m_bLoad;

public:
	virtual void		RenderTerrain_ByTool( bool bRenderSkyBox );
public:
	void			DeleteTemp();
	void			InitTemp();

	bool			LoadTemp( const char* cFile, eTEMPROOT_TYPE eTempRootType );

	bool			IsLoad(){ return m_bLoad; }

public:
	void			OnLButtonDown( CPoint pt );


	//============ Portal
public:
	void			SetPortalTarget( CsGBObj_Portal::sTARGET* pTarget ){ m_pTarget = pTarget; }
	void			SetPortalTarget( int* pX, int* pY, int* pRadius ){ m_pX = pX; m_pY = pY; m_pRadius = pRadius; }
	void			SetViewTarget( int X, int Y ){ m_ViewX = X; m_ViewY = Y; }



protected:
	void			_LButtonDown_PortalObj( CPoint pt );
	void			_LButtonDown_PortalNpc( CPoint pt );
};

extern cTempRoot		g_TempRoot;