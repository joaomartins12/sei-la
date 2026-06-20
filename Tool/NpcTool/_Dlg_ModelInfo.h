#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// _Dlg_ModelInfo 대화 상자입니다.

class _Dlg_ModelInfo : public CDialog
{
protected:
	CsNpcMng::sMODEL_INFO*		m_pSelectInfo;

protected:
	void			__ResetModelInfo();
	void			__CreateList();

















	DECLARE_DYNAMIC(_Dlg_ModelInfo)

	// Mfc Virtual
public:
	void			OnOK(){}

public:
	_Dlg_ModelInfo(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_Dlg_ModelInfo();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCTOOL_MODELINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	CListCtrl m_list_Model;
public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnNMClickListModel(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CEdit m_edit_Comment;
	CEdit m_edit_OffsetX;
	CEdit m_edit_OffsetY;
	CEdit m_edit_OffsetZ;
public:
	afx_msg void OnEnChangeEditComment();
	afx_msg void OnEnChangeEditOffsetX();
	afx_msg void OnEnChangeEditOffsetY();
	afx_msg void OnEnChangeEditOffsetZ();
	afx_msg void OnBnClickedButtonReset();
};
