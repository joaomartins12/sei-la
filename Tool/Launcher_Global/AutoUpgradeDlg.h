// AutoUpgradeDlg.h : 헤더 파일
//

#pragma once


#include "BmpButton.h"
#include "MyHtmlView.h"
#include "ProgressST.h"
#include "IniFile.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <map>
#include <list>

#define ID_TIMER_URL_STATE				101
#define ID_TIMER_URL_STATE_TICK			10
#define ID_TIMER_DOWNLOAD				12
#define ID_TIMER_DOWNLOAD_TICK			80
#define ID_TIMER_UNZIP					13
#define ID_TIMER_UNZIP_TICK				80
#define ID_TIMER_DO_PACK				14
#define ID_TIMER_DO_PACK_TICK			80
#define MAX_ERROR_CODE_COUNT			5

class CAutoUpgradeDlg : public CDialog
{
	//================================================================================================================
	//		초기화
	//================================================================================================================
protected:
	void			_Init_Window();
	void			_Init_Control();
	void			_Init_Member();
	bool			_Init_Version();
	bool			_Init_Down();

	//================================================================================================================
	//		제거
	//================================================================================================================
protected:
	void			_DeleteDlg();

	//================================================================================================================
	//		버젼
	//================================================================================================================
protected:
	CIniFile		m_VersionFile;					// client local version.ini file
	CString			m_strClientVersion;
	CString			m_strServerVersion;

	int				m_nDownStartVersion;
	int				m_nDownCompFileCount;			// 현재 다운로드 받은 파일 수
	int				m_nDownVersion;					// 현재 다운받는 버젼
	int				m_nLastPatchNum;				// 마지막 패치버젼

public:
	bool			Init( CString const& strCurrentPath, WORD const& wLauncherCurrentVersion, sConfiguration const& info );

	CString*		GetClientVersion(){ return &m_strClientVersion; }
	void			IncreaseDownCompFileCount(){ ++m_nDownCompFileCount; }

protected:
	bool			_ServerVersionFile();

	//================================================================================================================
	//		정보
	//================================================================================================================
protected:
	int				m_eState;

	int				m_nTotalFileCount;				// 전체 다운로드 받을 파일 수
	int				m_nServerVersion;					// 최신 버전(서버버전)
	CString			m_szMainState;					// 전체 상황 정보
	CString			m_szSubState;					// 다운로드 상황 정보
	
	//CString		m_szUnzipState;					// 언집 상황 정보	

	CString			m_strCurrentPath;
	sConfiguration	m_launcherInfos;

public:
	void			SetState( int eState ){ m_eState = eState; }
	void			Set_FileInfo( ULONGLONG ullFileSize );

	bool			VersionCheck();
	void			NextFile();

	void			Proc_MainState();
	void			Proc_SubState();
	void			Proc_UnzipState();

	BOOL			PassServerCheck();	// 서버 점검중일 때도 패치가 가능한지 체크함.

	void			SetLastPatchNumber(int nLastPatchNum);

protected:
	void			_Comp_Download();

	//================================================================================================================
	//		패킹
	//================================================================================================================
protected:
	bool			_Check_Pack();	

	//================================================================================================================
	//		Unzip
	//================================================================================================================
public:
	void			UnZip_File();
	//================================================================================================================
	//		Patch Pack
	//================================================================================================================
	void			DoPathPack_File();

	//================================================================================================================
	//		에러
	//================================================================================================================
protected:
	int				m_nErrorCode[ MAX_ERROR_CODE_COUNT ];
	int				m_nErrorCount;

public:
	void			ResetError();
	bool			SetError( int nCode );
	int				GetLastError();
	void			DisplayError();	

protected:
	TCHAR*			_GetErrorStr( int nCode );

	BOOL			_CheckRedist2015();
	//================================================================================================================
	//		UI
	//================================================================================================================
protected:
	bool			m_bAlreadyStart;				// 한번만 시작하게 막아주는 변수

public:
	void			RenderText(CDC *pDC);

protected:
	void			_ResetButton();

	void			_SetRange_MainProgress( int nMaxPos );
	void			_SetPos_MainProgress( int nPos );
	void			_SetRange_SubDownProgress( int nMaxPos );
	void			_SetPos_SubDownProgress( int nPos );
	void			_SetRange_SubUnzipProgress( int nMaxPos );
	void			_SetPos_SubUnzipProgress( int nPos );

	//================================================================================================================
	//		기타 함수
	//================================================================================================================
public:
	bool			DoExe( TCHAR* cCommand );

public:
	// 런처자신이 업데이트 된지 여부	

	void			GameStart();
	void			ProcessButtonUp( int iKind ) ;

	//=================================================================================================================
	//	URL
	//=================================================================================================================

protected:
	HINTERNET				m_hSession;

public:
	enum eURLDOWN_STATE
	{
		US_VERSION_ERROR,
		US_VERSION_INFO,
		US_VERSION_START,
		US_VERSION_END,
		US_FILE_ERROR,
		US_FILE_INFO,
		US_FILE_START,
		US_FILE_END,
	};

protected:
	struct sURL_INFO
	{
		int			s_nState;
		DWORD		s_nFileSize;
	};
	CsCriticalSection				m_CSTimer;
	CThreadSafeQueue< sURL_INFO* >	m_queueState;

public:
	void				SetDownState( int nState, DWORD nFileSize );
	void				ProcDownState();

	// 생성
public:
	CAutoUpgradeDlg(CWnd* pParent = NULL);	// 표준 생성자

	// 대화 상자 데이터
	enum{ IDD = IDD_AUTOUPGRADE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


	// 구현
protected:
	HICON m_hIcon;
	CMyHtmlView* pHtmlView;				//Html쓰기위해서

	bool			m_bClickTitle;
	CPoint			m_ptBackupDownPos;
	CPoint			m_ptWindowPos;

	CBmpButton		m_bmpStart;			//Start button bmp 정보 저장
	CBmpButton		m_bmpCancle;		//X(Cancle) button bmp 정보 저장
	CBmpButton		m_bmpOption;		//X(Cancle) button bmp 정보 저장
	CProgressST		m_ctrlProgressMain;
	CProgressST		m_ctrlProgressSubDown;
	CProgressST		m_ctrlProgressSubUnzip;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
	WORD			m_CurrentLauncherVersion;
};

extern CAutoUpgradeDlg*		g_pDlg;
