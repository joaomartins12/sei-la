

#pragma once

class cSceneTool
{
public:
	cSceneTool();
	~cSceneTool();

	// ====== Delete
public:
	void					Delete();	

	// ====== Initialition
public:
	void					Init();
	void					Reset();
protected:
	void					_InitCamera();	

	// ====== Render
public:
	void					Render();

	// ====== View
protected:
	bool					m_bWireFrame;
public:	
	void					ToggleWireFrame();

	// ====== Node Zoom
protected:
	bool					m_bEnableZoomIn;
public:
	void					SetEnableZoom( bool bEnable ){ m_bEnableZoomIn = bEnable; }

	// ====== Mouse
public:
	virtual void			OnLButtonDown( CPoint pt );
	virtual void			OnLButtonUp( CPoint pt );
	virtual void			OnMButtonDown( CPoint pt );
	virtual void			OnMButtonUp( CPoint pt );
	virtual void			OnRButtonDown( CPoint pt ); 
	virtual void			OnRButtonUp( CPoint pt );
	virtual void			OnMouseMove( CPoint pt );
	virtual void			OnMouseWheel( short zDelta );

	// ====== Picking
public:
		enum ePICK_TYPE
		{
			PT_NONE = 0,
			PT_ALPHA,
		};
protected:
	ePICK_TYPE				m_ePickType;
	bool					m_bEnablePickingForMapping;

protected:
	virtual bool			_IsEnablePickingForMapping();
	
	void					_PickStart( CPoint pt );
	void					_Picking( CPoint pt );
	void					_PickEnd( CPoint pt );

	// == RollBack
protected:
	std::map< DWORD, DWORD >			m_mapAlphaChangeObject;	

	// == Alpha
public:
	enum	eMAPPING_TYPE{ MT_ALPHA, MT_GRAY_PLUS, MT_GRAY_MINUS, };
protected:
	std::map< DWORD, NiPixelData* >		m_mapAlphaPixelData;
protected:
	struct sPICK_INFO
	{
		int						s_nObjectIndex;
		NiTexturingProperty*	s_pTexProp;
		NiPixelData*			s_pPData;
		BYTE*					s_pPixel;
		int						s_nWidth;
		int						s_nPPG;
		CPoint					s_ptMapOrg;
		CPoint					s_ptMap;
		float					s_fSizeConstant;
		float					s_fDecrementPow;
		eMAPPING_TYPE			s_eMT;
		int						s_nNTM;
		int						s_nChannel;
		int						s_nStride;
		BYTE					s_btColor;
		NiColor					s_Color;		
	};

	void		_PickingMapping( CPoint pt );
	void		_PickingGray( NiPick::Record* pRecord, sPICK_INFO* pi );
	void		_PickingAlpha( NiPick::Record* pRecord, sPICK_INFO* pi );
	void		_SetAlphaValue( sPICK_INFO pi );

	bool		_PickingAlphaDirection( sPICK_INFO* pi );
	void		_PickingAlphaUp( sPICK_INFO pi );
	void		_PickingAlphaDown( sPICK_INFO pi );
	void		_PickingAlphaRight( sPICK_INFO pi );
	void		_PickingAlphaLeft( sPICK_INFO pi );	
};

extern cSceneTool		g_SceneTool;