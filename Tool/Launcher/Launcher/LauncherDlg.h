// LauncherDlg.h : 헤더 파일
//

#pragma once

enum eSTATE { eMOUSE_DEFAULT = 0, eMOUSE_ON, eMOUSE_DOWN, eMOUSE_DISABLE };

#include "MyHtmlView.h"
#include "IniFile.h"
#include "UseRegistry.h"
#include "MyMessageBox.h"
#include "ServerCheck.h"

// CLauncherDlg 대화 상자
class cProgressbar;
class cBmpButton;
class cStrButton;
class CLauncherDlg : public CDialog
{
// 생성입니다.
public:
	CLauncherDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LAUNCHER_DIALOG };	
	enum eGameState { eINSTALL = 0, eUPDATE, ePACK, eRUN };

public:	
	BOOL		Init( CString const& strCurrentPath, WORD const& wLauncherCurrentVersion, sConfiguration const& info );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	virtual BOOL DestroyWindow();	


// 구현입니다.
protected:
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam); 	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()	
protected:
	virtual void OnOK();
	virtual void OnCancel();	

public:	
	void		Init();
	void		Quit( TCHAR* sz );
	void		MyMessageBox( TCHAR* sz, CMyMessageBox::eMessageType eMT = CMyMessageBox::MT_NORMAL );
	
	void		InitVersion();			//버젼 관리 함수
	void		UpdateVersion();
	void		ReturnVersion();

	void		MainButtonEnable( bool bEnable );			// 메인버튼 설정 함수

	bool		IsInstllEnd();								// 게임 설치 완료 확인
	void		Install_End();								// 게임 설치 완료
	void		Update_End();								// 게임 업데이트 완료
	void		Pack_End();									// 게임 패킹 완료	

	void		_FullClientDownLoadComplete();

protected:
	void		_Init_Window();
	void		_Init_Web();
	void		_Init_Control();
	void		_Init_Str();
	
	void		_CheckRegestry();
	void		_SelectGame();	

	void		_Render();
	void		_Update();	

	//void		_VersionDown();
	void		_VersionCheck();

//	bool		_IsInstaller();			// 홈페이지에서 설치파일 유도 할것인가?

	bool		_PlayGame( TCHAR* cExe, TCHAR* cCommand );	 // 게임실행 함수
	//void		_SetStartPage();
	
	// 추가 기능 함수
	bool		_CheckExeFile( TCHAR* szExeName );
	bool		_IsExecute();
	bool		_Findfolder( TCHAR* sz );			
	
private:
	bool		_IsServerMaintenance();
	void		ShowMaintenanceWindow();

	BOOL		_CheckRedist2015(TCHAR const* pPath);
	BOOL		_IsRedistInstalled();
	//변수
	/*---------------------------------------------------------------------
							내부에 사용되는 Str
	----------------------------------------------------------------------*/
public:	
	sStrInfo*	m_strGameName;
	sStrInfo*	m_strGameState;
	sStrInfo*	m_strClientVersion;
	sStrInfo*	m_strGameFile;	

	sStrInfo*	m_strCurPer;
	sStrInfo*	m_strTotalPer;
	sStrInfo*	m_strTime;

	/*---------------------------------------------------------------------
							주요 변수들
	----------------------------------------------------------------------*/
protected:
	CServerCheck	 m_dlgSeverCheck;	// 설치 다이얼로그	
	CMyMessageBox	 m_dlgMessageBox;	// 메세지박스
	

	bool		m_bQuit;
	eSTATE		m_eMouseState;	// 마우스 상태		

	int			m_nServerVersion;					// 서버패치버젼
	int			m_nClientVersion;					// 클라이언트 현재버젼
	int			m_nLauncherVer;						// 런처 버젼
	TCHAR		m_szOldClientVersion[ 32 ];			// 클라이언트 원래버젼
	
public:			
	TCHAR		m_szFullDown[ 256 ]; // 풀버전 다운로드 폴더

	/*---------------------------------------------------------------------
							컨트롤 들
	----------------------------------------------------------------------*/
protected:
	HICON		m_hIcon;	
	CBitmap		m_BackBitmap;	// 배경 BMP

	CPoint		m_ptWindowPos;
	CPoint		m_ptBackupDownPos;
	bool		m_bClickTitle;		
	
	cProgressbar*	m_pProgressCur;
	cProgressbar*	m_pProgressTotal;
	
	eGameState		m_eGameState;	// 게임 실행버튼 상태값
	cBmpButton*		m_pRun;
	cBmpButton*		m_pUpdate;
	cBmpButton*		m_pInstall;
	cBmpButton*		m_pFileCheck;	

	cBmpButton*		m_pGameButton[ GAME_END ];
	
	cBmpButton*		m_pMinimalize;
	cBmpButton*		m_pClose;	

	cStrButton*		m_pHomePage;
	cStrButton*		m_pOptionPage;
	cStrButton*		m_pPatch;

// 	bool			m_bStartPage;
// 	CBitmap			m_CheckBoxOn;
// 	CBitmap			m_CheckBoxOff;
// 	cStrButton*		m_pCheckbox;	

	// 웹 컨트롤
	CMyHtmlView*	m_pMainHtmlView;
	CMyHtmlView*	m_pSubHtmlView;	

	/*---------------------------------------------------------------------
							파일 다운로드 초기과정 	
	----------------------------------------------------------------------*/
public:	
	void		GoThread();
	void		DeleteWaitQueue();	
	bool		CheckFile( TCHAR* szFileName, DWORD& dwSize, DWORD dwPatchNum, sTHREAD_INPUT::eTYPE eThread );		// 업데이트나 설치파일 체크		

	bool		PassServerCheck();	// 서버 점검중일 때도 패치가 가능한지 체크함.

protected:
	HINTERNET		m_hSession;		
	CThreadSafeQueue< sTHREAD_INPUT* >	m_WaitQueue;

	//=================================================================================================================
	//	NOWCDN
	//=================================================================================================================
public:	
	CDownEngineSDK	m_DownEngine;	// Patch Download를 받기 위한 API
	bool			m_bDownStart;	

public:	
	// 다운로드 받게 될 파일(들)의 정보
	static void CallBack_File_OnFileInfo(IN void* pThisPointer, IN CHAR* pszFileNameA, IN WCHAR* pszFileNameW, IN ULONG ulFileTotalCount, IN ULONGLONG ullFileSize, IN ULONG ulCPCode, IN ULONG ulGameCode, IN ULONG ulFileID, IN ULONG ulFileType);
	// 다운로드 시작됐다는 알림
	static void CallBack_File_OnDownloadStart(IN void* pThisPointer);
	// 다운로드 완료됐다는 알림
	static void CallBack_File_OnDownloadComplete(IN void* pThisPointer);	

private:
	CString			m_strCurrentPath;
	sConfiguration	m_launcherInfos;
};

extern CLauncherDlg*	g_pDlg;
