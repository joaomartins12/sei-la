#pragma once

#include <afxpriv.h> // for WM_KICKIDLE declaration
#include "afxwin.h"

class CUpdateManagerDlg : public CDialog
{
public:
	CUpdateManagerDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_UpdateManager_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	virtual void	OnCancel();
	afx_msg void	OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam); // 핸들러 추가
	DECLARE_MESSAGE_MAP()

public:	
	BOOL			Init( CString const& nExeFilePath, CString const& szConfigUrl, CString const& szParam );

private:
	void			Install();		
	bool			VersionCheck();
	void			ExecuteLauncher();
	void			Delete();
	void			Setup();
	void			Render( int nPer );
	BOOL			GetFileRevisionNumber(CString const& FilePath, WORD & curVersion);
	bool			DownLoadLauncherVersionInfo();
	bool			GetLauncherVersion();
	BOOL			PassServerCheck();	// 서버 점검중일 때도 패치가 가능한지 체크함.
	bool			CheckServerEnable();

private:
	WORD			m_nCurVersion;
	CString			m_szParam;
	CString			m_strExeFilePath;	// 런처가 있는 폴더
	CString			m_strExeFileName;	// 런처 파일명
	CString			m_strConfigUrl;

	sConfiguration	m_VersionList;
private:
	HICON			m_hIcon;
	CFont			m_font;

	// 멤버 변수 선언
	CBitmap			m_bmpMainBG;		// Main 배경
	int				m_nMainWidth;
	int				m_nMainHeight;	

	// 진행사항 표시 변수
	CBitmap			m_bmpBar;		// Main 배경
	int				m_nBarWidth;
	int				m_nBarHeight;	
	int				m_nPersent;
	int				m_nFileCount;	

	//버튼
	CBitmap			m_bmpOk1;
	CBitmap			m_bmpOk2;
	bool			m_bOnOk;
	CBitmap			m_bmpFind1;
	CBitmap			m_bmpFind2;
	bool			m_bOnFind;

	CPoint		m_MousePoint;
};
