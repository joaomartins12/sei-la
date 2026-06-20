// SequnceTool.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "SequnceTool.h"

#include ".\_Dumper\cDumper.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSequnceToolApp

BEGIN_MESSAGE_MAP(CSequnceToolApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CSequnceToolApp::OnAppAbout)
END_MESSAGE_MAP()


// CSequnceToolApp 생성

CSequnceToolApp::CSequnceToolApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CSequnceToolApp 개체입니다.

CSequnceToolApp theApp;


// CSequnceToolApp 초기화

BOOL CSequnceToolApp::InitInstance()
{
	// 현재 경로가 서버라면 패스
	TCHAR szCurDirectory[ MAX_PATH ] = {0, };
	GetCurrentDirectory( MAX_PATH, szCurDirectory );
	if( ( _tcsicmp( szCurDirectory, L"Z:\\Digimon Masters" ) == 0 )||
		( _tcsicmp( szCurDirectory, L"\\\\dmoserver2\\디지몬마스터즈\\digimon masters\\Digimon Masters" ) == 0 ) )
	{
		MessageBox( NULL, L"자기 컴퓨터에 받아서 실행해주세요.", NULL, MB_OK );
		return FALSE;
	}

#ifdef NDEBUG
	g_Dumper;
#endif

	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	// 주 창을 만들기 위해 이 코드에서는 새 프레임 창 개체를
	// 만든 다음 이를 응용 프로그램의 주 창 개체로 설정합니다.
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 프레임을 만들어 리소스와 함께 로드합니다.

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.


	return TRUE;
}


// CSequnceToolApp 메시지 처리기




// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CSequnceToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CSequnceToolApp 메시지 처리기


BOOL CSequnceToolApp::OnIdle(LONG lCount)
{
	if( g_pMainWnd != NULL )
	{
		g_pMainWnd->_Update();
		g_pMainWnd->_Render();
	}	

	CWinApp::OnIdle(lCount);
	return TRUE;
}
