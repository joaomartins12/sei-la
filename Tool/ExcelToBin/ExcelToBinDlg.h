// ExcelToBinDlg.h : 헤더 파일
//

#pragma once


class CExcelToBinDlg : public CDialog
{
// 생성입니다.
public:
	CExcelToBinDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef VERSION_USA
	enum { IDD = IDD_EXCELTOBIN_DIALOG_ENG };
#else
	enum { IDD = IDD_EXCELTOBIN_DIALOG };
#endif
	

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	virtual void OnOK(){}


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonProcess();
	afx_msg void OnBnClickedButtonProcEng();
	afx_msg void OnBnClickedButtonProcTha();
#ifdef VERSION_USA
	afx_msg void OnBnClickedButtonProcAeria();//Aeria ExcelToBin용 _chu8820
#endif
	afx_msg void OnBnClickedButtonProcTaiwan();
	afx_msg void OnBnClickedButtonProcHongkong();
};
