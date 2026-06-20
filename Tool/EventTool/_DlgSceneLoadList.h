#pragma once
#include "afxcmn.h"


// _DlgSceneLoadList 대화 상자입니다.

class _DlgSceneLoadList : public CDialog
{
	DECLARE_DYNAMIC(_DlgSceneLoadList)

public:
	_DlgSceneLoadList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgSceneLoadList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOADSCENELIST };

	void	Init( int nType ) { m_nType = nType; }

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.	

protected:
	int		m_nType;
	DECLARE_MESSAGE_MAP()
	CListCtrl m_ListSceneLoad;
public:
	afx_msg void OnNMDblclkListsceneload(NMHDR *pNMHDR, LRESULT *pResult);
};
