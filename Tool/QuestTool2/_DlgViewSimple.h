#pragma once
#include "afxcmn.h"


// _DlgViewSimple 대화 상자입니다.

class _DlgViewSimple : public CDialog
{
	DECLARE_DYNAMIC(_DlgViewSimple)


public:
	void			_Init();
protected:

	void			__ReadQuest();
	void			__InsertQuest( CsQuest* pFT );




public:
	_DlgViewSimple( CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgViewSimple();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_VIEW_SIMPLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNMClickListMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListMain(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CListCtrl m_list_MainList;
public:
	afx_msg void OnNMRclickListMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};
