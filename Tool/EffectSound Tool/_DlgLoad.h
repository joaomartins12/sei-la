#pragma once
#include "afxcmn.h"

// _DlgLoad 대화 상자입니다.

class _DlgLoad : public CDialog
{
	DECLARE_DYNAMIC(_DlgLoad)

public:
	_DlgLoad(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgLoad();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_ListLoad;	
	afx_msg void OnNMDblclkListLoad(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListLoad(NMHDR *pNMHDR, LRESULT *pResult);
};
