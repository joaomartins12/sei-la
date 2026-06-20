// SecuritySelfToolDlg.h : 헤더 파일
//

#pragma once


// CSecuritySelfToolDlg 대화 상자
class CSecuritySelfToolDlg : public CDialog
{
// 생성입니다.
public:
	CSecuritySelfToolDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SECURITYSELFTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CEdit		m_InputEdit;
	CEdit		m_OutputEncriptEdit;

	CEdit		m_InputEncriptEdit;
	CEdit		m_OutputDecriptEdit;

	CEdit		m_InputMakeTextToHexEdit;
	CEdit		m_OutputMakeTextToHexEdit;

	CEdit		m_InputMakeHextToTextEdit;
	CEdit		m_OutputMakeHextToTextEdit;

public:
	afx_msg void OnBnClickedEncriptBtn();
	afx_msg void OnBnClickedDecriptBtn();
	afx_msg void OnBnClickedButtonMakeHex();
	afx_msg void OnBnClickedButtonMakeText();
};
