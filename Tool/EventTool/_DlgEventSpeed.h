#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgEventSpeed 대화 상자입니다.

class _DlgEventSpeed : public CDialog
{
	DECLARE_DYNAMIC(_DlgEventSpeed)

public:
	_DlgEventSpeed(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgEventSpeed();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EVENTSPEED };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

protected:
	CsSpinCtrl m_spinEventSpeed;	
	CsEdit m_EditEventSpeed;
	CComboBox m_ComboMoveMotion;	
public:
	afx_msg void OnCbnSelendokComboMovemotion();
};
