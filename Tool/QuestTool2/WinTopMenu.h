#pragma once



// WinTopMenu 폼 뷰입니다.

class WinTopMenu : public CFormView
{
	DECLARE_DYNCREATE(WinTopMenu)

public:
	void			_Init();




protected:
	WinTopMenu();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~WinTopMenu();

public:
	enum { IDD = IDD_DIALOG_TOPMENU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoadTable();
};

extern WinTopMenu*		g_pTopMenu;


