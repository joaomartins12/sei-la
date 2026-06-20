#pragma once
#include "afxwin.h"


// CMyMessageBox 대화 상자입니다.
class cStrButton;
class CMyMessageBox : public CDialog
{
	DECLARE_DYNAMIC(CMyMessageBox)

public:
	CMyMessageBox(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMyMessageBox();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MESSAGEBOX };
	enum eMessageType { MT_NORMAL = 0, MT_QUIT, MT_INSTALL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	eSTATE			m_eMouseState;
	eMessageType	m_eType;

	CBitmap			m_Bitmap;
	CBitmap			m_BitButton;
	CFont			m_font;
	TCHAR			m_szMessage[ 256 ];
	
	cStrButton*		m_pMessageOk;

public:
	void			Init( TCHAR* sz, eMessageType eType );
	void			Update();
	void			Render();
	

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();		
	
	afx_msg void OnBnClickedOk();			
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	
};
