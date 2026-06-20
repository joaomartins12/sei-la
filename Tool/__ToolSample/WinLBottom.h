#pragma once



// WinLBottom 폼 뷰입니다.

class WinLBottom : public CFormView
{
	DECLARE_DYNCREATE(WinLBottom)

public:
	void				_Delete(){}
	void				_Init(){}	

	void				_Update(){}
	void				_Render(){}




protected:
	WinLBottom();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~WinLBottom();

public:
	enum { IDD = IDD_LBottom };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


extern WinLBottom*		g_pWinLBottom;