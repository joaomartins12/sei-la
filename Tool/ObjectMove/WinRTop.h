#pragma once



// WinRTop 폼 뷰입니다.

class WinRTop : public CFormView
{
	DECLARE_DYNCREATE(WinRTop)

public:
	void				_Delete(){}
	void				_Init();

	void				_Update(){}
	void				_Render(){}

public:
	void				_ResetMap( DWORD dwMapID );



protected:
	WinRTop();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~WinRTop();

public:
	enum { IDD = IDD_RTop };
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
protected:
	CListCtrl m_list_Object;
	CListCtrl m_list_Object2;
public:
	afx_msg void OnLvnItemchangedListObject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListObject2(NMHDR *pNMHDR, LRESULT *pResult);	
};

extern WinRTop*		g_pWinRTop;

