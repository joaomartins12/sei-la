#if !defined(AFX_BMPBUTTON_H__7604480E_B8E7_11D5_8A13_00E0293DE0CC__INCLUDED_)
#define AFX_BMPBUTTON_H__7604480E_B8E7_11D5_8A13_00E0293DE0CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BmpButton.h : header file
//

class CAutoUpgradeDlg ;
/////////////////////////////////////////////////////////////////////////////
// CBmpButton window
enum 
{
	BMP_BUTTON_START,
	BMP_BUTTON_CANCEL,
	BMP_BUTTON_OPTION,
	BMP_BUTTON_HOME,
} ;

class CBmpButton : public CStatic
{
// Construction
public:
	CBmpButton();

	bool	m_bEnable;
	BOOL LoadButtonBitmaps(int idDefault, int idUp = -1, int idDown = -1, int idDisable = -1);
	void ResetButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	
	int m_iButtonKind ;
	CAutoUpgradeDlg* m_pkParent ;
	void SetParent( CAutoUpgradeDlg* pkParent ) { m_pkParent = pkParent ; } ;
	void SetButtonKind( int iKind ) { m_iButtonKind = iKind ; } ;

	void SetButtonEable(bool bEnable);
	virtual ~CBmpButton();

	// Generated message map functions
protected:

	CBitmap m_bmpDefault;
	CBitmap	m_bmpUp;
	CBitmap m_bmpDown;
	CBitmap m_bmpDisable;

	//{{AFX_MSG(CBmpButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPBUTTON_H__7604480E_B8E7_11D5_8A13_00E0293DE0CC__INCLUDED_)
