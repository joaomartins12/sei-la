#pragma once
#include "afxcmn.h"


// _DlgPartObject 대화 상자입니다.

class _DlgPartObject : public CDialog
{
	DECLARE_DYNAMIC(_DlgPartObject)



protected:
	TCHAR		m_szWorkingFolder[ MAX_PATH ];


public:
	void		_Init( char* cWorkingFolder );



public:
	_DlgPartObject(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgPartObject();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_PART };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CTreeCtrl m_tree_Part;
public:
	afx_msg void OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult);
};
