#include "stdafx.h"
#include "AutoUpgradeDlg.h"

CAutoUpgradeDlg* g_pDlg = NULL;

CAutoUpgradeDlg::CAutoUpgradeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutoUpgradeDlg::IDD, pParent),m_CurrentLauncherVersion(0)
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
	DDX_Control(pDX, IDC_STATIC_OPTION, m_bmpOption);
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
END_MESSAGE_MAP()


//================================================================================================================
//
//		기본 함수들
//
//================================================================================================================
bool CAutoUpgradeDlg::Init( CString const& strCurrentPath, WORD const& wLauncherCurrentVersion, sConfiguration const& info )
{
	m_bClickTitle = false;
	m_strCurrentPath = strCurrentPath;
	m_launcherInfos = info;
	m_CurrentLauncherVersion = wLauncherCurrentVersion;

	return true;
}

int CAutoUpgradeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

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
		//PostMessage( WM_QUIT );
		return TRUE;
	}	

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CAutoUpgradeDlg::OnTimer(UINT nIDEvent)
{
	switch( nIDEvent )
	{
	case ID_TIMER_DOWNLOAD:
		switch( m_eState )
		{
		case eSTATE_VERSION_DOWN:
		case eSTATE_FILE_DOWN:
			Proc_SubState();
			break;
		}
		break;

	case ID_TIMER_UNZIP:
		Proc_UnzipState();
		break;
	case ID_TIMER_URL_STATE:
		ProcDownState();
		break;
	case ID_TIMER_DO_PACK:
		Proc_SubState();
		break;
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
 	bitmap.LoadBitmap(IDB_MAIN);

	BITMAP bm;
	bitmap.GetBitmap(&bm);

	CDC cdc;
	cdc.CreateCompatibleDC(pDC);
	CBitmap* pBit = cdc.SelectObject(&bitmap);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &cdc, 0, 0, SRCCOPY);
	cdc.SelectObject(pBit);
	cdc.DeleteDC();

	RenderText( pDC );

	return FALSE;

	return CDialog::OnEraseBkgnd(pDC);
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