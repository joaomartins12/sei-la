#pragma once



// WinLTop 폼 뷰입니다.

class WinLTop : public CFormView
{
	DECLARE_DYNCREATE(WinLTop)


public:
	void				_Delete(){}
	void				_Init(){}	

	void				_Update(){}
	void				_Render(){}





protected:
	WinLTop();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~WinLTop();

public:
	enum { IDD = IDD_LTop };
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
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

extern WinLTop*		g_pWinLTop;

