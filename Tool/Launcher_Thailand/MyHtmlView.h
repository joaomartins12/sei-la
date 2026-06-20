#if !defined(AFX_MYHTMLVIEW_H__428A234F_D0F4_4CA9_A065_790E19C7F8FD__INCLUDED_)
#define AFX_MYHTMLVIEW_H__428A234F_D0F4_4CA9_A065_790E19C7F8FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyHtmlView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyHtmlView html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

class CMyHtmlView : public CHtmlView
{
protected:
	DECLARE_DYNCREATE(CMyHtmlView)

// html Data
public:
	CMyHtmlView();           // protected constructor used by dynamic creation
	//{{AFX_DATA(CMyHtmlView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
// Attributes
public:

// Operations
public:
	void BeforeNavigate2(LPDISPATCH /* pDisp */, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName,
		VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel) ;
	void OnDocumentComplete(LPCTSTR lpszURL) ;
	HRESULT OnGetHostInfo(DOCHOSTUIINFO *pInfo) ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyHtmlView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyHtmlView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMyHtmlView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYHTMLVIEW_H__428A234F_D0F4_4CA9_A065_790E19C7F8FD__INCLUDED_)
