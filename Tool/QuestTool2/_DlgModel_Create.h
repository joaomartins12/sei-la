#pragma once
#include "afxwin.h"


// _DlgModel_Create 대화 상자입니다.

class _DlgModel_Create : public CDialog
{
	DECLARE_DYNAMIC(_DlgModel_Create)

protected:
	CsQuest**		m_ppFTQuest;
	int				m_nMapID;
	int				m_nLevel;

public:
	virtual void	OnOK(){}



public:
	_DlgModel_Create( CsQuest** ppFTQuest, int nMapID = -1, int nLevel = -1, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgModel_Create();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CREATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CEdit m_edit_UniqID;
	CComboBox m_combo_ST;
	CEdit m_edit_ST_ID;
	CButton m_btn_ST_Table;
	CButton m_btn_Create;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboSt();
	afx_msg void OnBnClickedButtonStTable();
	afx_msg void OnBnClickedButtonOk();
protected:
	CEdit m_edit_Level;
};
