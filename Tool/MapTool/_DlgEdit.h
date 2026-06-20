#pragma once
#include "afxwin.h"


// _DlgEdit 대화 상자입니다.

class _DlgEdit : public CDialog
{
	DECLARE_DYNAMIC(_DlgEdit)

public:
	_DlgEdit( TCHAR* pszCaption, TCHAR* pszEdit, TCHAR* pszInitEdit, int nMaxEditLen, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgEdit();

protected:
	int			m_nMaxEditLen;
	TCHAR*		m_pszEdit;
	TCHAR*		m_pszCaption;
	TCHAR*		m_pszInitEdit;
public:
	void	OnOK(){}

// 대화 상자 데이터입니다.
public:
	enum { IDD = IDD_DLG_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOk();
protected:
	CsEditFosus m_edit_Data;
public:
	virtual BOOL OnInitDialog();
};
