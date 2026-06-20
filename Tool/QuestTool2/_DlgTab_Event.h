#pragma once
#include "afxwin.h"


// _DlgTab_Event 대화 상자입니다.

class _DlgTab_Event : public CDialog
{
	DECLARE_DYNAMIC(_DlgTab_Event)


public:
	void			_Init();
	void			_SetQuest( cQuestObj* pQuestObj );




public:
	virtual void	OnOK(){}
	virtual void	OnCancel(){}



public:
	_DlgTab_Event(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgTab_Event();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TAB_EVENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CEdit m_edit_Title;
	CEdit m_edit_QS_EventID;
public:
	afx_msg void OnBnClickedButtonQsEi();
protected:
	CEdit m_edit_QS_EventID_Discript;
public:
	afx_msg void OnEnChangeEditQsEi();
protected:
	CButton m_btn_QS_FT;
public:
	afx_msg void OnEnChangeEditQpEi();
protected:
	CEdit m_edit_QP_EventID;
	CButton m_btn_QP_FT;
	CEdit m_edit_QP_EventID_Discript;
public:
	afx_msg void OnBnClickedButtonQpEi();
protected:
	CEdit m_edit_QC_EventID;
	CButton m_btn_QC_FT;
	CEdit m_edit_QC_EventID_Discript;
public:
	afx_msg void OnEnChangeEditQcEi();
	afx_msg void OnBnClickedButtonQcEi();
protected:
	CEdit m_edit_QR_EventID;
	CButton m_btn_QR_FT;
	CEdit m_edit_QR_EventID_Discript;
public:
	afx_msg void OnEnChangeEditQrEi();
	afx_msg void OnBnClickedButtonQrEi();
};
