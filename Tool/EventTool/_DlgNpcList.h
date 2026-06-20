#pragma once
#include "afxcmn.h"


// _DlgNpcList 대화 상자입니다.

class _DlgNpcList : public CDialog
{
	DECLARE_DYNAMIC(_DlgNpcList)

public:
	_DlgNpcList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgNpcList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPC_LIST };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CListCtrl m_NpcList;

public:	
	afx_msg void OnNMDblclkListnpc(NMHDR *pNMHDR, LRESULT *pResult);
};
