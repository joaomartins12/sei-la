

#pragma once 



class WinViewer : public CView
{
protected: // serialization에서만 만들어집니다.
	WinViewer();
	DECLARE_DYNCREATE(WinViewer)

public:
	enum eVIEWSTATE{ VS_VIEWER, VS_LIST, VS_SIMPLE, };
protected:
	eVIEWSTATE		m_eViewState;
public:
	void		_SetViewState( eVIEWSTATE eState );

public:
	void		_Init();	
	void		_Delete();

	void		_Update();
	void		_Render();

	void		_ResetDevice();
	

protected:
	void		__UpdateMove();

	bool		__BeginRender();
	bool		__EndRender();

	void		__CalMove();


protected:
	ShaderHelper	m_ShaderHelper;
	cNodeMng		m_NodeMng;
	cQuestMng		m_QuestMng;
	cMinimap		m_Minimap;

	CsPoint			m_ptCurPos;
	CsPoint			m_ptPopupPos;
	cQuestObj*		m_pPopupObject;	
	NiPoint2		m_vCurPos_Float;

	enum eCLICK_STATE{ CS_SELECT, CS_LINK_PARENT, CS_LINK_CHILD, CS_MOVE, CS_COPY, CS_MAX_COUNT, };
	eCLICK_STATE	m_eClickState;
	HCURSOR			m_hCursor[ CS_MAX_COUNT ];

protected:
	void			_SetClickState( eCLICK_STATE cs );


public:
	void			_SetCurPos( CsPoint ptPos );
	CsPoint			_GetCurPos(){ return m_ptCurPos; }
	
	cNodeMng*		_GetNodeMng(){ return &m_NodeMng; }
	cQuestMng*		_GetQuestMng(){ return &m_QuestMng; }
	cMinimap*		_GetMinimap(){ return &m_Minimap; }

	void			_SetSelectCenter();




protected:
	CsPoint			m_ptMoveMouse;
	bool			m_bMoveMouseDown;
	bool			m_bMinimapPress;

	CsPoint			m_ptCursor;
	CsPoint			m_ptCursorScreen;

	enum eKEY_MOVE{
		KM_NONE		=	0,
		KM_LEFT		=	0x0001,
		KM_UP		=	0x0010,
		KM_RIGHT	=	0x0100,
		KM_DOWN		=	0x1000,
	};
	DWORD			m_dwMovePlag;

public:
	CsPoint			_GetCursor(){ return m_ptCursor; }
	CsPoint			_GetCursorScreen(){ return m_ptCursorScreen; }




	//==============================================================================================================
	//		Dialog
	//==============================================================================================================
protected:
	_DlgViewList		m_dlgList;
	_DlgViewSimple		m_dlgSimple;



protected:
	virtual void OnDraw(CDC* /*pDC*/);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern NiDX9RendererPtr g_pRenderer;
extern WinViewer*		g_pViewer;