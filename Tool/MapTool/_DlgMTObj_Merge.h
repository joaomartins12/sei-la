#pragma once
#include "afxcmn.h"


// _DlgMTObj_Merge 대화 상자입니다.

class _DlgMTObj_Merge : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTObj_Merge)


	//======================================================================
	//
	//		List Control
	//
	//======================================================================
protected:
	bool			m_bReadingList;
protected:	
	void			__ReadListMerge();


	//======================================================================
	//
	//		Object
	//
	//======================================================================
protected:
	CsGBObject*		m_pCsGBObject;




public:
	_DlgMTObj_Merge(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTObj_Merge();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_OBJECT_MERGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CListCtrl m_list_Merge;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();	
	afx_msg void OnLvnItemchangedListMerge(NMHDR *pNMHDR, LRESULT *pResult);
};
