#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgEventTime 대화 상자입니다.

class _DlgEventTime : public CDialog
{
	DECLARE_DYNAMIC(_DlgEventTime)
protected:
	CsSpinCtrl m_spinEventtime;
	CsEdit	m_EditEventtime;

	CsSpinCtrl m_spinStartTime;
	CsEdit m_Edit_StartTime;
	

public:
	_DlgEventTime(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgEventTime();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EVENTTIME };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();			

};
