#pragma once
#include "afxwin.h"


// CServerCheck 대화 상자입니다.

class cStrButton;
class CServerCheck : public CDialog
{
	DECLARE_DYNAMIC(CServerCheck)

public:
	CServerCheck(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CServerCheck();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERVERCHECK };

public:
	void	Init( CString const& szTitle, CString const& szMsg );
	void	Update();
	void	Render();		

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void			UpdateWindowPosition();

	DECLARE_MESSAGE_MAP()
public:
	CBitmap			m_bmpBack;
	CBitmap			m_bmpGame;	

	CPoint			m_ptWindowPos;
	CPoint			m_ptBackupDownPos;
	//bool			m_bClickTitle;

	CBitmap			m_bmpButton;	
	cStrButton*		m_Ok;
	cStrButton*		m_Cancel;

	CFont			m_fontMain;
	CFont			m_fontSub;
	eSTATE			m_eMouseState;

	CString				m_szMain;
	std::list<CString>	m_szMsgs;
 	//TCHAR			m_szMain[ 128 ];
 	//TCHAR			m_szMsg[ 512 ];
	cStrButton*		m_pOK;	

	afx_msg void OnBnClickedOk();		
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
