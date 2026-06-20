#pragma once
#include "afxcmn.h"


// _DlgEventlist 대화 상자입니다.

class _DlgEventlist : public CDialog
{
	DECLARE_DYNAMIC(_DlgEventlist)

public:
	_DlgEventlist(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgEventlist();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EVENTLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
protected:
	CListCtrl m_List_Char;	
public:
	afx_msg void OnHdnItemdblclickListChar(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListChar(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListChar(NMHDR *pNMHDR, LRESULT *pResult);
};
