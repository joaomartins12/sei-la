#pragma once
#include "afxwin.h"



// WinTop 폼 뷰입니다.

class WinTop : public CFormView
{
	DECLARE_DYNCREATE(WinTop)

public:
	void		_Init();

	bool		_IsViewMinimap(){ return ( m_chk_ViewMinimap.GetCheck() == BST_CHECKED ); }

	void		_SetViewState( WinViewer::eVIEWSTATE eState );


	float		_GetExpMul(){ return nsCsMfcFunc::Control2Float( &m_edit_ExpMul ); }

protected:
	WinTop();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~WinTop();


public:
	enum { IDD = IDD_DIALOG_TOP };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CComboBox m_combo_MiddleType;
public:
	afx_msg void OnCbnSelchangeComboMiddlttailType();
protected:
	CButton m_chk_ViewMinimap;
	CComboBox m_combo_ViewType;
public:
	afx_msg void OnCbnSelchangeComboViewtype();
	afx_msg void OnBnClickedButtonFindwin();
protected:
	CEdit m_edit_ExpMul;
};


extern WinTop*		g_pTop;