#pragma once
#include "afxwin.h"


// _DlgAnimation 대화 상자입니다.

class _DlgEtc : public CFormView
{
	DECLARE_DYNCREATE(_DlgEtc)


public:
	void			_Delete();
	void			_Init();

	void			_Update();

	bool			_IsAction(){ return ( m_combo_Action.GetCurSel() != 0 ); }
	bool			_CheckAction();

	void			_ReleaseAction();


protected:
	float			m_fCheckTime;
	DWORD			m_dwActionAni;





protected:
	_DlgEtc();   // 표준 생성자입니다.
	virtual ~_DlgEtc();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ETC };

public:
	virtual void OnOK(){}
	virtual void OnCancel(){}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CComboBox m_combo_Action;
public:
	afx_msg void OnCbnSelchangeComboAction();
	afx_msg void OnBnClickedButtonAtt1();
	afx_msg void OnBnClickedButtonAtt2();
	afx_msg void OnBnClickedButtonSkill1();
	afx_msg void OnBnClickedButtonSkill3();
	afx_msg void OnBnClickedButtonSkill2();
	afx_msg void OnBnClickedButtonSkill4();
	afx_msg void OnBnClickedButtonFtDigimon();
	afx_msg void OnBnClickedButtonFtSkill();
};
