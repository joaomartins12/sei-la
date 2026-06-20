// FrmBase.h : CFrameBase 클래스의 인터페이스
//


#pragma once

class CFrameBase : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFrameBase)
public:
	CFrameBase();

protected:
	DWORD					m_ID;
	CViewBase*				m_pView;

	bool					m_bShow;

public:
	virtual void			_Delete();
	virtual void			_Init( DWORD ID, CRect rcView );
	virtual void			_Update(){ m_pView->_Update(); }
	
	CViewBase*				_GetView(){ return m_pView; }

	virtual void			_BringToTop(){}

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);	

// 구현입니다.
public:		
	virtual ~CFrameBase();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 생성된 메시지 맵 함수
public:
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};
