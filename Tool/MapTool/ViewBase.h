// ViewBase.h : CViewBase 클래스의 인터페이스
//


#pragma once


// CViewBase 창

class CViewBase : public CWnd
{
// 생성입니다.
public:
	CViewBase();

public:
	DWORD					m_ViewID;
	
	static CViewBase*		_GetInstance( DWORD ID );
	
public:
	virtual void			_Delete(){ if( _GetDlg() != NULL ) _GetDlg()->_Delete(); }
	virtual void			_Init(){}
	
	virtual void			_Update(){}

	// Dialog
	virtual _DlgBase*		_GetDlg(){ return NULL; }

	virtual void			_OnLButtonDBCLick( CPoint pt ){}
	virtual void			_OnLButtonDown( CPoint pt ){}
	virtual void			_OnLButtonUp( CPoint pt ){}	
	virtual void			_OnMButtonDown( CPoint pt ){}
	virtual void			_OnMButtonUp( CPoint pt ){}
	virtual void			_OnRButtonDown( CPoint pt ){}
	virtual void			_OnRButtonUp( CPoint pt ){}
	virtual void			_OnMouseMove( CPoint pt ){}
	virtual void			_OnMouseWheel( short zDelta ){}

// 재정의입니다.
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CViewBase();

	// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

extern CViewBase	g_Viewbase;