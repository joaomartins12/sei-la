#pragma once
#include "afxcmn.h"
#include "afxwin.h"



// _DlgChar 폼 뷰입니다.

class _DlgChar : public CFormView
{
	DECLARE_DYNCREATE(_DlgChar)

protected:
	_DlgChar();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~_DlgChar();

public:
	void		Init();
	bool		IsDestView(){ return ( m_Check_ChrLook.GetCheck() == BST_CHECKED ); }


public:
	enum { IDD = IDD_DIALOG_CHR };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CListCtrl	m_List_Char;
	CButton		m_Check_ChrLook;

	bool m_bSort;
	int m_nSortColumn;

public:	
	afx_msg void OnBnClickedCheckChrlook();
	afx_msg void OnLvnItemchangedListChar(NMHDR *pNMHDR, LRESULT *pResult);
	

	
	afx_msg void OnHdnItemdblclickListChar(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListChar(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListChar(NMHDR *pNMHDR, LRESULT *pResult);
};


