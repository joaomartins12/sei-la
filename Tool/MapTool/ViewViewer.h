
#pragma once

extern CsGBDX9RendererPtr	g_pRenderer;
extern NiCullingProcessPtr	g_pCuller;
extern float				g_fElapsedTime;

#include "SceneTool.h"
#include "ScenePreview.h"



class CViewViewer : public CViewBase 
{
public:
	static bool				CALLBACK_ThreadDelete( sTCUnit* pUnit );

	// »ý¼ºÀÚ ¼Ò¸êÀÚ
public:
	CViewViewer();
	virtual ~CViewViewer();

	//==============================================================================
	//
	//	Base
	//
	//==============================================================================
protected:
	CRect					m_rcView;
public:
	BOOL					_IsPtInView( CPoint pt ){ return PtInRect( &m_rcView, pt ); }

	// ====== Delete
public:
	void					_Delete();
protected:
	void					__DeleteRenderer();
	// ====== Init
protected:
	ShaderHelper			m_ShaderHelper;
public:
	void					_Init();
	void					_PreDelete();
	void					_PreReset();
	void					_PostReset();
protected:
	void					__CreateRenderer();
	bool					__InitShader();

	// ====== Update
public:
	virtual void			_Update();

	// ====== Camera
public:
	inline void				_SetCameraAccel(){ __SetCameraAccel( _IsControl() ); }
	void					_ResetCamera();
protected:
	void					__DeleteCamera();
	void					__CreateCamera();	
	void					__SetCameraAccel( bool bSpeedUp );	


	// ====== Get

	//==============================================================================
	//
	//	Tool
	//
	//==============================================================================
	// ====== ViewState
protected:
	cSceneTool				m_SceneTool;
public:
	cSceneTool*				_GetSceneTool(){ return &m_SceneTool; }

	// ====== Base
protected:
	void					__Update_Tool();
	void					__Render_Tool();	
	

	// ====== AutoSave
protected:
	float					m_fAutoSaveTime;
public:
	void					_ResetAutoSave();
protected:	
	void					__UpdateAutoSave();

	// ======= Mouse
protected:
	void					__OnLButtonDown_Tool( CPoint pt ){ m_SceneTool.OnLButtonDown( pt ); }
	void					__OnLButtonUp_Tool( CPoint pt ){ m_SceneTool.OnLButtonUp( pt ); }
	void					__OnMButtonDown_Tool( CPoint pt ){ m_SceneTool.OnMButtonDown( pt ); }
	void					__OnMButtonUp_Tool( CPoint pt ){ m_SceneTool.OnMButtonUp( pt ); }
	void					__OnRButtonDown_Tool( CPoint pt ){ m_SceneTool.OnRButtonDown( pt ); }
	void					__OnRButtonUp_Tool( CPoint pt ){ m_SceneTool.OnRButtonUp( pt ); }
	void					__OnMouseMove_Tool( CPoint pt ){ m_SceneTool.OnMouseMove( pt ); }
	void					__OnMouseWheel_Tool( short zDelta ){ m_SceneTool.OnMouseWheel( zDelta ); }

	// ====== Key
public:
	void					_EnableConfigKey( bool bEnable ){ m_bEnableConfigKey = bEnable; }

protected:
	bool					__CheckConfigKey( DWORD vKeyCode, bool bDown );
	void					__DoConfigKeyDown( eKEY_SET eKey );
	void					__DoConfigKeyUp( eKEY_SET eKey );


	//==============================================================================
	//
	//	PreView
	//
	//==============================================================================
protected:
	cScenePreview			m_ScenePreview;
public:
	cScenePreview*			_GetScenePreview(){ return &m_ScenePreview; }
protected:
	void					__Update_Preview();
	void					__Render_Preview();

	// ======= Mouse
protected:
	void					__OnLButtonDown_Preview( CPoint pt ){ m_ScenePreview.OnLButtonDown( pt ); }
	void					__OnLButtonUp_Preview( CPoint pt ){ m_ScenePreview.OnLButtonUp( pt ); }
	void					__OnRButtonDown_Preview( CPoint pt ){ m_ScenePreview.OnRButtonDown( pt ); }
	void					__OnRButtonUp_Preview( CPoint pt ){ m_ScenePreview.OnRButtonUp( pt ); }
	void					__OnMouseMove_Preview( CPoint pt ){ m_ScenePreview.OnMouseMove( pt ); }
	void					__OnMouseWheel_Preview( short zDelta ){ m_ScenePreview.OnMouseWheel( zDelta ); }

	//==============================================================================
	//
	//	Ect
	//
	//==============================================================================
	// ====== Font
protected:
	ID3DXFont*              m_pFont;
public:
	void					_CreateFont();
	void					_RenderFont();
	void					_DeleteFont();

	// ====== Mouse
public:
	virtual void			_OnLButtonDown( CPoint pt );
	virtual void			_OnLButtonUp( CPoint pt );
	virtual void			_OnMButtonDown( CPoint pt );
	virtual void			_OnMButtonUp( CPoint pt );
	virtual void			_OnRButtonDown( CPoint pt );
	virtual void			_OnRButtonUp( CPoint pt );
	virtual void			_OnMouseMove( CPoint pt );
	virtual void			_OnMouseWheel( short zDelta );
	virtual void			_OnLButtonDBCLick(CPoint pt);
	virtual	void			ChangeCameraPos(NiTransform trTarget );

	// ====== Key
public:
	bool					m_bEnableConfigKey;
	bool					_KeyDown( DWORD vKeyCode );
	bool					_KeyUp( DWORD vKeyCode );
	void					_SysKeyDown( DWORD vKeyCode );
	void					_SysKeyUp( DWORD vKeyCode );

	bool					_IsShift(){ return ( (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0 ); }
	bool					_IsControl(){ return ( (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 ); }
	bool					_IsAlt(){ return ( (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ); }

	void					_SetControl( bool bControl );


	void					UpdateResetCamHeightRate(float fRate);
	const float&			GetCurrResetCamHeightRate(void){return m_ResetCamHeightRate;}
protected:
	float					m_ResetCamHeightRate;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void RenderObject(void);
	void RenderMonsterRegion(void);
};

