#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// _DlgEventLoadList 대화 상자입니다.

class _DlgEventLoadList : public CDialog
{
	DECLARE_DYNAMIC(_DlgEventLoadList)

public:
	_DlgEventLoadList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgEventLoadList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOADEVENTLIST };	

	//창 용도	
	void	Init( int nType );

	int		m_nType;	// 0 : 로드    1 : 씬만들기 

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.	

	DECLARE_MESSAGE_MAP()
	CListCtrl m_List_LoadEvent;
	
public:
	afx_msg void OnNMDblclkListload(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRdblclkListload(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedScenemake();
	CButton m_Button_SceneMake;
};
