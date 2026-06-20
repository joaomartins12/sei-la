#pragma once


// _DlgInputKey 대화 상자입니다.

class _DlgInputKey : public CDialog
{
	DECLARE_DYNAMIC(_DlgInputKey)

public:
	_DlgInputKey(cConfig::sKEYSET::sKEYINFO* pKeyInfo, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgInputKey();

protected:
	cConfig::sKEYSET::sKEYINFO*	m_pKeyInfo;

protected:
	void		__PushKey( DWORD vCode );

public:
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_INPUTKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonCancel();
};
