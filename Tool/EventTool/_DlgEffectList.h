#pragma once
#include "afxcmn.h"


// _DlgEffectList 대화 상자입니다.

class _DlgEffectList : public CDialog
{
	DECLARE_DYNAMIC(_DlgEffectList)

public:
	_DlgEffectList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgEffectList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EFFECT_LIST };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.	

	DECLARE_MESSAGE_MAP()
	CListCtrl m_ListEffect;
public:
	afx_msg void OnNMDblclkListEffect(NMHDR *pNMHDR, LRESULT *pResult);
};
