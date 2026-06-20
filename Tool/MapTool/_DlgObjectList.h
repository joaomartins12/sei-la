#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// _DlgObjectList 대화 상자입니다.

class _DlgObjectList : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgObjectList)

public:
	void		_PostViewReset();

public:
	void		_ResetList();
	void		_UnselectList();


protected:
	void		__InsertList( CsGBObject* pObject );
	void		__Sort( int nColume, bool bIncrease );



public:
	_DlgObjectList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgObjectList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_OBJECT_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CListCtrl m_list_Object;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListObject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDBClickListObject( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnLvnColumnclickListObject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CStatic mObjectCnt;
};
