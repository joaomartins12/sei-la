#pragma once
#include "afxwin.h"


// _DlgModal_Simple 대화 상자입니다.

class _DlgModal_Simple : public CDialog
{
	DECLARE_DYNAMIC(_DlgModal_Simple)


protected:
	DWORD				m_dwQuestID;

public:
	virtual void	OnOK(){ OnBnClickedButtonOk(); }


public:
	_DlgModal_Simple( DWORD dwQuestID, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgModal_Simple();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SIMPLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CEdit m_edit_Level;
	CEdit m_edit_Money;
	CEdit m_edit_Exp;
public:
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
};
