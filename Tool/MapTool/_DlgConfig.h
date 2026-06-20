#pragma once
#include "afxwin.h"


// _DlgConfig 대화 상자입니다.

class _DlgConfig : public CDialog
{
	DECLARE_DYNAMIC(_DlgConfig)

public:
	_DlgConfig(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgConfig();

protected:
	void					__ReadInfo();
	void					__SaveInfo();
	void					__EndInfo();

	// Key
protected:
	cConfig::sKEYSET		m_KeySet;
protected:
	void					__ReadInfo_Key();
	LPTSTR					__MakeInfo( cConfig::sKEYSET::sKEYINFO* pKeyInfo );

	// Auto Save
protected:
	cConfig::sAUTOSAVE		m_AutoSave;

public:
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	CStatic m_static_Key[ KS_COUNT ];	
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedStaticKey0();
	afx_msg void OnStnClickedStaticKey1();
	afx_msg void OnStnClickedStaticKey2();
	afx_msg void OnStnClickedStaticKey3();
	afx_msg void OnStnClickedStaticKey4();
	afx_msg void OnStnClickedStaticKey5();
	afx_msg void OnStnClickedStaticKey6();
	afx_msg void OnStnClickedStaticKey7();
	afx_msg void OnStnClickedStaticKey8();
	afx_msg void OnStnClickedStaticKey9();
	afx_msg void OnStnClickedStaticKey10();
	afx_msg void OnStnClickedStaticKey11();
	afx_msg void OnStnClickedStaticKey12();
	afx_msg void OnStnClickedStaticKey13();
	afx_msg void OnStnClickedStaticKey14();
	afx_msg void OnStnClickedStaticKey15();
	afx_msg void OnStnClickedStaticKey16();
	afx_msg void OnStnClickedStaticKey17();
	afx_msg void OnStnClickedStaticKey18();
	afx_msg void OnStnClickedStaticKey19();
	afx_msg void OnStnClickedStaticKey20();
	afx_msg void OnStnClickedStaticKey21();
	afx_msg void OnStnClickedStaticKey22();
	afx_msg void OnStnClickedStaticKey23();
	afx_msg void OnStnClickedStaticKey24();
	afx_msg void OnStnClickedStaticKey25();
	afx_msg void OnStnClickedStaticKey26();
	afx_msg void OnStnClickedStaticKey27();
	afx_msg void OnStnClickedStaticKey28();
	afx_msg void OnStnClickedStaticKey29();
	afx_msg void OnStnClickedStaticKey30();
	afx_msg void OnStnClickedStaticKey31();
	afx_msg void OnStnClickedStaticKey32();
	afx_msg void OnStnClickedStaticKey33();
	afx_msg void OnStnClickedStaticKey34();
	afx_msg void OnStnClickedStaticKey35();
	afx_msg void OnStnClickedStaticKey36();
	afx_msg void OnStnClickedStaticKey37();
	afx_msg void OnStnClickedStaticKey38();
	afx_msg void OnStnClickedStaticKey39();
	afx_msg void OnStnClickedStaticKey40();

	afx_msg void OnBnClickedButtonDefaultKey();
protected:
	CsEditFosus m_edit_AutoSaveTime;
	CButton m_check_UseAutoSave;
public:
	afx_msg void OnBnClickedCheckUseAutosave();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
protected:
	CsEditFosus m_edit_SleepTick;
	CButton m_chk_WriteExcel;
	CButton m_check_WriteList;
	CButton m_check_WritePortal;
	CButton m_check_WriteCharLight;
	CSliderCtrl m_CamAutoMoveHeightRate;
public:
	afx_msg void OnNMReleasedcaptureSliderRateCamHeight(NMHDR *pNMHDR, LRESULT *pResult);
};
