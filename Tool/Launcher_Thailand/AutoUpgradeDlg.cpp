// AutoUpgradeDlg.cpp : 구현 파일
//

// AutoUpgradeDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "AutoUpgradeDlg.h"

// CAutoUpgradeDlg 대화 상자
CAutoUpgradeDlg* g_pDlg = NULL;

#define  WS_EX_LAYERED	0x00080000

#define  LWA_COLORKEY   0x00000001
#define  LWA_ALPHA      0x00000002
typedef BOOL(WINAPI * SLWA)(HWND, COLORREF, BYTE, DWORD);



CAutoUpgradeDlg::CAutoUpgradeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutoUpgradeDlg::IDD, pParent),m_pHtmlView(NULL),m_pHtmlView2(NULL),m_bClickTitle(false),m_wCurrentVersion(0)
{
	g_pDlg = this;
	m_hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);
	m_eState = eSTATE_CONNECT;
	m_hSession = NULL;
	m_nLastPatchNum = 0;
}

void CAutoUpgradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_START, m_bmpStart);
	DDX_Control(pDX, IDC_STATIC_CANCLE, m_bmpCancle);
	DDX_Control(pDX, IDC_TAIWAN_OPTION, m_bmpOption);
	DDX_Control(pDX, IDC_PROGRESS_MAIN, m_ctrlProgressMain);
	DDX_Control(pDX, IDC_PROGRESS_SUBDOWN, m_ctrlProgressSubDown);
	DDX_Control(pDX, IDC_PROGRESS_SUBUNZIP, m_ctrlProgressSubUnzip);
}

BEGIN_MESSAGE_MAP(CAutoUpgradeDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_CREATE()
	//ON_STN_CLICKED(IDC_TAIWAN_OPTION, &CAutoUpgradeDlg::OnStnClickedTaiwanOption)
END_MESSAGE_MAP()


//================================================================================================================
//
//		기본 함수들
//
//================================================================================================================


int CAutoUpgradeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

// CAutoUpgradeDlg 메시지 처리기

BOOL CAutoUpgradeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CThread::GlobalInit();

	_Init_Window();
	_Init_Control();
	_Init_Member();

	if( _Init_Version() == false )
	{
		PostMessage( WM_QUIT );
		return TRUE;
	}

	if( _Init_Down() == false )
	{
		PostMessage( WM_QUIT );
		return TRUE;
	}	

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CAutoUpgradeDlg::OnTimer(UINT nIDEvent)
{
	switch( nIDEvent )
	{
	case ID_TIMER_DOWNLOAD:
		{
			switch( m_eState )
			{
			case eSTATE_VERSION_DOWN:
			case eSTATE_FILE_DOWN:
				Proc_SubState();
				break;
			}break;
		}break;
	case ID_TIMER_UNZIP:
		Proc_UnzipState();
		break;
	case ID_TIMER_URL_STATE:
		ProcDownState();
		break;
	case ID_TIMER_DO_PACK:
		Proc_SubState();
		break;
	default:
		return;
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CAutoUpgradeDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//--------------------------------------------------------------------------
	// Background Bitmap
	//--------------------------------------------------------------------------
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_MAIN_TH);

	BITMAP bm;
	bitmap.GetBitmap(&bm);

	CDC cdc;
	cdc.CreateCompatibleDC(pDC);
	CBitmap* pBit = cdc.SelectObject(&bitmap);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &cdc, 0, 0, SRCCOPY);
	//pDC->TransparentBlt(0, 0, bm.bmWidth, bm.bmHeight, &cdc, 0, 0, bm.bmWidth, bm.bmHeight, RGB(255,0,255));
	cdc.SelectObject(pBit);
	cdc.DeleteDC();

	LONG ExtendedStyle = GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE );
	SetWindowLong( GetSafeHwnd(), GWL_EXSTYLE, ExtendedStyle | WS_EX_LAYERED );

	double TransparencyPercentage = 50.0;

	double fAlpha = TransparencyPercentage * (255.0 / 50 );
	BYTE byAlpha = static_cast<BYTE>( fAlpha);
	//SetLayeredWindowAttributes( GetSafeHwnd(), 0, byAlpha, LWA_ALPHA );
	SLWA pSetLayeredWindowAttributes = NULL;
	HINSTANCE hmodUSER32 = LoadLibrary(L"USER32.DLL");
	pSetLayeredWindowAttributes = (SLWA)GetProcAddress( hmodUSER32,"SetLayeredWindowAttributes");
	pSetLayeredWindowAttributes( GetSafeHwnd(), RGB(255,0,255), fAlpha, LWA_COLORKEY | LWA_ALPHA );

	RenderText( pDC );

	return FALSE;

	//return CDialog::OnEraseBkgnd(pDC);
}

void CAutoUpgradeDlg::OnPaint() 
{
	CPaintDC dc(this);
	RenderText( (CDC*)&dc );

	CDialog::OnPaint();
}

void CAutoUpgradeDlg::OnOK()
{
	if( m_bmpStart.m_bEnable == false )
		return;

	GameStart();
}

void CAutoUpgradeDlg::OnCancel()
{
}

void CAutoUpgradeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	_DeleteDlg();
}

void CAutoUpgradeDlg::OnClose()
{
	CDialog::OnClose();

	_DeleteDlg();
}


void CAutoUpgradeDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();

	m_bClickTitle = false;
	if( point.y < 26 )
	{
		m_bClickTitle = true;
		GetCursorPos( &m_ptBackupDownPos );
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CAutoUpgradeDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();

	m_bClickTitle = false;

	CDialog::OnLButtonUp(nFlags, point);
}

void CAutoUpgradeDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// 윈도우 이동
	if( m_bClickTitle == true )
	{
		CPoint ptCurPos;
		GetCursorPos( &ptCurPos );
		m_ptWindowPos += ( ptCurPos - m_ptBackupDownPos );
		m_ptBackupDownPos = ptCurPos;

		SetWindowPos( NULL, m_ptWindowPos.x, m_ptWindowPos.y, 0, 0, SWP_NOSIZE );
	}

	_ResetButton();
	CDialog::OnMouseMove(nFlags, point);
}

void CAutoUpgradeDlg::SetLastPatchNumber(int nLastPatchNum)
{
	m_nLastPatchNum = nLastPatchNum;
}

void CAutoUpgradeDlg::SetCurrentVersion( WORD const& wVersion )
{
	m_wCurrentVersion = wVersion;
}