// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

class CMainFrame : public CFrameWnd
{

public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

	// 특성입니다.
protected:
	CSplitterWnd		m_wndSplitter;
public:
	CSplitterWnd*		GetSplitterWnd(){ return &m_wndSplitter; }


protected:
	NiViewer*			m_pNiViewer;
	_DlgController*		m_pDlgController;
	_DlgChar*			m_pDlgChar;
	_DlgEtc*			m_pDlgEtc;

public:
	_DlgController*		GetDlgController(){ return m_pDlgController; }
	_DlgChar*			GetDlgChar(){ return m_pDlgChar; }
	_DlgEtc*			GetDlgEtc(){ return m_pDlgEtc; }


	// 작업입니다.
protected:
	void				__NiDelete();
	void				__NiInit();

public:
	void				_Update();
	void				_Render();



	// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
public:
	afx_msg void OnDestroy();
};


extern CsGBDX9RendererPtr	g_pRenderer;
extern CMainFrame*			g_pMainWnd;