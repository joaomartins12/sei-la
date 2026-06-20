// CMyMessageBox.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Launcher.h"
#include "MyMessageBox.h"
#include "StrButton.h"


// CMyMessageBox 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMyMessageBox, CDialog)

CMyMessageBox::CMyMessageBox(CWnd* pParent /* = NULL */)
	: CDialog(CMyMessageBox::IDD, pParent)
{		
}

CMyMessageBox::~CMyMessageBox()
{
	SAFE_DELETE( m_pMessageOk );
}

void CMyMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);			
}


BEGIN_MESSAGE_MAP(CMyMessageBox, CDialog)			
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CMyMessageBox 메시지 처리기입니다.

BOOL CMyMessageBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	CPoint ptWindowPos;
	int cxWindow = GetSystemMetrics(SM_CXSCREEN);
	int cyWindow = GetSystemMetrics(SM_CYSCREEN);
	ptWindowPos.x = (cxWindow - 250)/2;
	ptWindowPos.y = (cyWindow - 117)/2;
	SetWindowPos(NULL, ptWindowPos.x, ptWindowPos.y, 250, 117, SWP_SHOWWINDOW);	

	m_font.CreateFont( 14, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, _T("고딕체") );
	m_Bitmap.LoadBitmap( IDB_MESSAGE );		
	m_BitButton.LoadBitmap( IDB_BUTTON );
	m_pMessageOk = new cStrButton;
	m_pMessageOk->Init( this, CPoint( 114, 89 ), STR_BUTTON_OK, false, RGB( 31, 31, 31 ), RGB( 121, 121, 121 ) );
	m_pMessageOk->ResetArea( CRect( CPoint( 91, 84 ), CPoint( 91, 84 ) + CPoint( 68, 26 ) ) );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMyMessageBox::Init( TCHAR* sz, eMessageType eType )
{ 
	if( sz == NULL )
		ZeroMemory( &m_szMessage, 256 );
	else
		_tcscpy_s( m_szMessage, 256, sz ); 

	m_eMouseState = eMOUSE_DEFAULT; 
	m_eType = eType; 
}

void CMyMessageBox::Update()
{
	m_pMessageOk->Update( m_eMouseState );
}

void CMyMessageBox::Render()
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
	BMP_RENDER::Render_Bmp( &MemDC, 0, 0, &m_Bitmap, false );

	// 버튼 이미지	
	BMP_RENDER::Render_ColorKey( &dc, &MemDC, 91, 84, &m_BitButton );

	// 버튼 그리기	
	m_pMessageOk->Render( &MemDC );

	// 글자 쓰기
	CFont* pFont = MemDC.SelectObject( &m_font );
	MemDC.SetBkMode( TRANSPARENT );

	MemDC.SetTextColor( RGB( 255, 255, 255 ) );		
	MemDC.DrawText( m_szMessage, -1, CRect( 20, 25, 230, 145 ), DT_TOP | DT_CENTER | DT_EXPANDTABS | DT_WORDBREAK );	

	dc.BitBlt( 0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY );		
	MemDC.SelectObject( pOldBitmap );		
}

void CMyMessageBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_eMouseState = eMOUSE_DEFAULT;
	if( m_pMessageOk->GetState() == eMOUSE_DOWN )
	{	
		ShowWindow( SW_HIDE );
		switch( m_eType )
		{
		case MT_NORMAL:			
			break;
		case MT_QUIT:
			g_pDlg->PostMessage( WM_QUIT );
			break;
		case MT_INSTALL:
			ShellExecute(NULL, L"open", L"iexplore.exe", g_GameInfo->GetGameInfo()->s_szInstallURL, NULL, SW_SHOWNORMAL );
			break;
		}		
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CMyMessageBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	m_eMouseState = eMOUSE_DOWN;	
	CDialog::OnLButtonDown(nFlags, point);
}

