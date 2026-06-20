#pragma once
#include "afxwin.h"


// _DlgTimeChange 대화 상자입니다.

class _DlgTimeChange : public CDialog
{
	DECLARE_DYNAMIC(_DlgTimeChange)

public:
	_DlgTimeChange(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgTimeChange();

	float GetTime() { return m_fTime; }
	float m_fTime;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TIMECHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

protected:
	CsEdit m_EditTime;
public:
	afx_msg void OnBnClickedOk();
};
