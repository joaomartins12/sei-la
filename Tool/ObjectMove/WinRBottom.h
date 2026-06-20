#pragma once
#include "afxwin.h"



// WinRBottom 폼 뷰입니다.

class WinRBottom : public CFormView
{
	DECLARE_DYNCREATE(WinRBottom)

public:
	void			_Delete(){}
	void			_Init();	

	void			_Update(){}
	void			_Render(){}

protected:
	void			__InitComboMapList();




protected:
	WinRBottom();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~WinRBottom();

public:
	enum { IDD = IDD_RBottom };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	CComboBox m_cmp_MapList;
public:
	afx_msg void OnBnClickedButtonMapload();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonExcelsave();
};

extern WinRBottom*		g_pWinRBottom;

