// Clause.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Launcher.h"
#include "ServerCheck.h"
#include "StrButton.h"


// CServerCheck 대화 상자입니다.

IMPLEMENT_DYNAMIC(CServerCheck, CDialog)

CServerCheck::CServerCheck(CWnd* pParent /*=NULL*/)
	: CDialog(CServerCheck::IDD, pParent)
{

}

CServerCheck::~CServerCheck()
{	
	m_fontMain.DeleteObject();
	m_fontSub.DeleteObject();
	m_bmpGame.DeleteObject();
	SAFE_DELETE( m_pOK );	
}

void CServerCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CServerCheck, CDialog)
	ON_BN_CLICKED(IDOK, &CServerCheck::OnBnClickedOk)		
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
//	ON_WM_MOVE()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

static int CALLBACK BrowseForFolder_CallbackProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	if( uMsg == BFFM_INITIALIZED )
		SendMessage( hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpData );
	return 0;
}


// CServerCheck 메시지 처리기입니다.

BOOL CServerCheck::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.	
	int cxWindow = GetSystemMetrics(SM_CXSCREEN);
	int cyWindow = GetSystemMetrics(SM_CYSCREEN);
	m_ptWindowPos.x = (cxWindow - 572)/2;
	m_ptWindowPos.y = (cyWindow - 471)/2;
	SetWindowPos(NULL, m_ptWindowPos.x, m_ptWindowPos.y, 572, 471, SWP_SHOWWINDOW);	

	// 외형 라운드주기
	CRect rect;
	CRgn rgn;

	GetClientRect(rect);
	rgn.CreateRoundRectRgn( rect.left, rect.top, rect.right, rect.bottom, 10, 10 );
	::SetWindowRgn( this->m_hWnd, (HRGN)rgn, TRUE );	
	m_fontMain.CreateFont( 15, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, _T("고딕체") );
	m_fontSub.CreateFont( 14, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, _T("고딕체") );

	switch( g_GameInfo->GetGameType() )
	{
	case GAME_DIGIMONMASTERS:
		m_bmpGame.LoadBitmap( IDB_MASTERS_INST );	
		break;			
	}

	m_bmpBack.LoadBitmap( IDB_INSTALLBACK );	
	m_bmpButton.LoadBitmap( IDB_BUTTON );

	m_pOK = new cStrButton;
	m_pOK->Init( this, CPoint( 267, 439 ), STR_BUTTON_OK, false, RGB( 31, 31, 31 ), RGB( 121, 121, 121 ) );
	m_pOK->ResetArea( CRect( CPoint( 244, 432 ), CPoint( 244, 432 ) + CPoint( 68, 26 ) ) );	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CServerCheck::Init( CString const& szTitle, CString const& szMsg )
{	
	UpdateWindowPosition();
	m_szMain = szTitle;
	m_szMsgs.clear();
	int nPos = 0;
	CString strToken = szMsg.Tokenize(_T("\r\n"), nPos);
	while (!strToken.IsEmpty())
	{
		m_szMsgs.push_back(strToken);
		strToken = szMsg.Tokenize(_T("\r\n"), nPos);
	}

	//m_bClickTitle = false;
}

void CServerCheck::UpdateWindowPosition()
{
	RECT rt;
	CWnd* pMainWindow = GetParent();
	if( pMainWindow )
	{
		pMainWindow->GetWindowRect( &rt );
		m_ptWindowPos.x = (rt.left + rt.right - 572)/2;
		m_ptWindowPos.y = (rt.top + rt.bottom - 471)/2;
	}
	else
	{
		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		m_ptWindowPos.x = (nScreenWidth - 572)/2;
		m_ptWindowPos.y = (nScreenHeight - 471)/2;
	}

	// TODO:  여기에 추가 초기화 작업을 추가합니다.	
	SetWindowPos(NULL, m_ptWindowPos.x, m_ptWindowPos.y, 572, 471, SWP_SHOWWINDOW);	
}

void CServerCheck::Update()
{
	m_pOK->Update( m_eMouseState );	
}

void CServerCheck::Render()
{
	CClientDC dc(this); // device context for painting	

	CRect	rect;
	GetClientRect( &rect );

	CBitmap memBitmap;
	memBitmap.DeleteObject();
	memBitmap.CreateCompatibleBitmap( &dc, rect.Width(), rect.Height() );

	CDC MemDC;	
	CBitmap* pOldBitmap;	

	MemDC.CreateCompatibleDC( &dc );	
	pOldBitmap = MemDC.SelectObject( &memBitmap );

	// 배경그리기
	BMP_RENDER::Render_Bmp( &MemDC, 0, 0, &m_bmpBack, false );
	BMP_RENDER::Render_Bmp( &MemDC, 13, 41, &m_bmpGame, false );

	// 버튼 이미지	
	BMP_RENDER::Render_ColorKey( &dc, &MemDC, 245, 433, &m_bmpButton ); // 확인

	// 버튼 그리기	
	m_pOK->Render( &MemDC );

	// 글자 쓰기
	CFont* pFont = MemDC.SelectObject( &m_fontMain );		
	MemDC.SetBkMode( TRANSPARENT );
	MemDC.SetTextColor( RGB( 2, 2, 2 ) );
	MemDC.TextOut( 42, 350, m_szMain );

	pFont = MemDC.SelectObject( &m_fontSub );		
	MemDC.SetBkMode( TRANSPARENT );
	MemDC.SetTextColor( RGB( 2, 2, 2 ) );

	std::list<CString>::const_iterator it = m_szMsgs.begin();
	for (int nY = 0; it != m_szMsgs.end(); ++it, ++nY)
	{
		CRect rc;
		rc.SetRect(34, 372 + (nY * 18), 534, 372 + (nY * 18) + 300);
		MemDC.DrawText( *it , &rc, DT_CENTER);
	}
	
	dc.BitBlt( 0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY );		
	MemDC.SelectObject( pOldBitmap );	
	
}

void CServerCheck::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.			
	OnOK();	
	//g_pDlg->PostMessage( WM_QUIT );
}

void CServerCheck::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//m_bClickTitle = false;
	m_eMouseState = eMOUSE_DEFAULT;
	if( m_pOK->GetState() == eMOUSE_DOWN )
		OnBnClickedOk();		

	CDialog::OnLButtonUp(nFlags, point);
}

void CServerCheck::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

 	m_eMouseState = eMOUSE_DOWN;
// 	m_bClickTitle = false;
// 	if( point.y < 46 )
// 	{
// 		m_bClickTitle = true;
// 		GetCursorPos( &m_ptBackupDownPos );
// 	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CServerCheck::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

// 	// 윈도우 이동
// 	if( m_bClickTitle == true )
// 	{
// 		CPoint ptCurPos;
// 		GetCursorPos( &ptCurPos );
// 		m_ptWindowPos += ( ptCurPos - m_ptBackupDownPos );
// 		m_ptBackupDownPos = ptCurPos;
// 
// 		SetWindowPos( NULL, m_ptWindowPos.x, m_ptWindowPos.y, 0, 0, SWP_NOSIZE );
// 	}	

	CDialog::OnMouseMove(nFlags, point);
}
