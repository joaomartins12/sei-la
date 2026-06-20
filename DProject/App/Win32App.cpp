//---------------------------------------------------------------------------
//
// 파일명 : Win32App.cpp
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "Win32App.h"
#include "../resource.h"
#include "DXVersionChecker.h"

#include <shlobj.h>
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace App
{
	//---------------------------------------------------------------------------
	CWin32App* CWin32App::ms_pkWin32App = NULL;

	void __stdcall CWin32App::UnicodeToUTF8(std::wstring const& wsSource, std::string& sDest)
	{
		if( wsSource.empty() )
			return ;

		int iConverted = ::WideCharToMultiByte( CP_UTF8, 0, wsSource.c_str(), wsSource.size(), NULL, 0, NULL, NULL );
		sDest.resize( iConverted );

		::WideCharToMultiByte( CP_UTF8, 0, wsSource.c_str(), wsSource.size(), &sDest.at( 0 ), iConverted, NULL, NULL );
	}

	void __stdcall CWin32App::UnicodeToMultiByte(std::wstring const& wsSource, std::string& sDest)
	{
		if( wsSource.empty() )
			return ;

		int iConverted = ::WideCharToMultiByte( PO_CP_CODE, 0, wsSource.c_str(), wsSource.size(), NULL, 0, NULL, NULL );
		sDest.resize( iConverted );

		::WideCharToMultiByte( PO_CP_CODE, 0, wsSource.c_str(), wsSource.size(), &sDest.at( 0 ), iConverted, NULL, NULL );
	}

	void __stdcall CWin32App::UTF8ToUnicode(std::string const& sSource, std::wstring& wsDest)
	{
		if( sSource.empty() )
			return ;

		int iConverted = ::MultiByteToWideChar( CP_UTF8, 0, sSource.c_str(), sSource.size(), NULL, 0 );
		wsDest.resize( iConverted );

		::MultiByteToWideChar( CP_UTF8, 0, sSource.c_str(), sSource.size(), &wsDest.at( 0 ), iConverted );
	}

	void __stdcall CWin32App::MultiByteToUnicode(std::string const& sSource, std::wstring& wsDest)
	{
		if( sSource.empty() )
			return ;

		int iConverted = ::MultiByteToWideChar( PO_CP_CODE, 0, sSource.c_str(), sSource.size(), NULL, 0 );
		wsDest.resize( iConverted );

		::MultiByteToWideChar( PO_CP_CODE, 0, sSource.c_str(), sSource.size(), &wsDest.at( 0 ), iConverted );
	}

	CWin32App::CWin32App()
	:m_hInstance(NULL)
	,m_hWnd(NULL)
	,m_uiWidth(0)
	,m_uiHeight(0)
	,m_bFullMode(FALSE)
	,m_dwStyle(WINDOWS_FORM_STYPE)
	,m_dwExStyle(0)
	,m_eProcessState(PROCESS_RUN)
	{
		::ClipCursor( NULL );

		ms_pkWin32App = this;
		::SystemParametersInfo( SPI_GETWORKAREA, 0, &m_uiWindowModeWorkSpaceRect, 0 );
		UpdateScreenSize();
	}
	//---------------------------------------------------------------------------
	CWin32App::~CWin32App()
	{
		ms_pkWin32App = 0;
	}
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	BOOL CWin32App::Startup( LPCTSTR p_lpszWndClass, LPCTSTR p_lpszWndTitle, 
			UINT p_uiX, UINT p_uiY,
			UINT p_uiWidth, UINT p_uiHeight, 
			BOOL p_bFullMode )
	{
		m_hInstance = GetModuleHandle(NULL);

		LPCTSTR lpszClass = p_lpszWndClass;
		LPCTSTR lpszTitle = p_lpszWndTitle;

		m_uiWidth  = p_uiWidth;
		m_uiHeight = p_uiHeight;
		m_bFullMode = p_bFullMode;

		// RegisterClass
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(wc));

	#ifdef _GIVE
		wc.style         = CS_VREDRAW | CS_HREDRAW | CS_NOCLOSE;
	#else
		wc.style         = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	#endif

		wc.hbrBackground = (HBRUSH)CreateSolidBrush( RGB(30,44,81) );	//GetStockObject(WHITE_BRUSH);
		wc.hCursor       = LoadCursor(m_hInstance, (LPCWSTR)IDC_CURSOR);
		wc.hIcon         = (HICON)LoadImage( m_hInstance, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE|LR_LOADTRANSPARENT );
		//wc.hIcon         = LoadIcon( g_hInstance, (LPCWSTR)IDI_ICON ); //LoadIcon(NULL, IDI_APPLICATION);
		wc.hInstance     = m_hInstance;
		wc.lpfnWndProc   = (WNDPROC) WndProc;
		wc.lpszClassName = lpszClass;
		wc.lpszMenuName  = NULL;
		wc.cbClsExtra    = 0L;
		wc.cbWndExtra    = 0L;

		if(!RegisterClass(&wc))
			return FALSE;

		// Style
		m_dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
		if(m_bFullMode) m_dwStyle = WS_POPUP | WS_CLIPCHILDREN;
		
		// Rect
		RECT kRect = { 0, 0, m_uiWidth, m_uiHeight };
		AdjustWindowRectEx(&kRect, m_dwStyle, FALSE, m_dwExStyle);

		if( !m_bFullMode )
		{
			RECT rcWorkArea = { 0, };
			GetWorkScreenWorkArea( rcWorkArea );

			p_uiX = ( ( rcWorkArea.right - rcWorkArea.left ) - m_uiWidth) >> 1;
			p_uiY = ( ( rcWorkArea.bottom - rcWorkArea.top ) - m_uiHeight) >> 1;

			if( p_uiX < rcWorkArea.left )
				p_uiX = rcWorkArea.left;

			if( p_uiY < rcWorkArea.top )
				p_uiY = rcWorkArea.top;
		}

		// CreateWindow
		m_hWnd = CreateWindowEx( m_dwExStyle, lpszClass, lpszTitle, m_dwStyle,
			p_uiX, p_uiY, (kRect.right - kRect.left), (kRect.bottom - kRect.top), 
			NULL, (HMENU) NULL, m_hInstance, NULL );

		if(!m_hWnd)
			return FALSE;

		SendMessage( m_hWnd,WM_SETICON,ICON_BIG|ICON_SMALL,(LPARAM)wc.hIcon);
		
		// UpdateWindow
		ShowWindow(m_hWnd, SW_SHOWDEFAULT);
		UpdateWindow(m_hWnd);

		DXVersionChecker versionChecker;
		DXVersionChecker::VERSION_RESULT val = versionChecker.CheckDXVersionRequirement( 9, 904 );
		if( val != DXVersionChecker::DX_RESULT_Ok_DXVERSION )
		{
			int nRetval = CsMessageBox(MB_OKCANCEL, _LAN( "DirectX9.0 버전이 낮습니다. 최신 DirectX를 설치 하시겠습니까?" ) );
			if( IDOK == nRetval )  
			{
#ifdef VERSION_STEAM
				versionChecker.DoDxSettup( L"_CommonRedist\\DirectX\\Jun2010\\DXSETUP.exe" );
#else
				versionChecker.DoDxSettup( L"DirectX\\Jun2010\\DXSETUP.exe" );
#endif
			}
			return FALSE;
		}

		return TRUE;
	}
	//---------------------------------------------------------------------------
	int CWin32App::Run()
	{
		int iRet = 0;

		if(OnInitialize())
		{
			// Message Loop
			MSG kMsg;
			ZeroMemory(&kMsg, sizeof(kMsg));

			BOOL bQuit = FALSE;

			while(!bQuit)
			{
				OnIdleExtern();

				if(PeekMessage(&kMsg, NULL, 0, 0, PM_REMOVE))
				{
					if(kMsg.message == WM_QUIT)
					{
						bQuit = TRUE;
					}
					else
					{
						TranslateMessage(&kMsg);
						DispatchMessage(&kMsg);
					}
				}
				else
				{
					OnIdle();
				}

				if( m_eProcessState == PROCESS_STOP)
					break;
			}
			iRet = static_cast<int>(kMsg.wParam);
		}

		net::execute();
		net::exit(iRet); // check exit code	

		OnTerminate();

		return iRet;
	}
	//---------------------------------------------------------------------------
	void CWin32App::Shutdown()
	{
	}

	void CWin32App::SetProcessState( PROCESS_STATE const& eState )
	{
		m_eProcessState = eState;
	}

	//---------------------------------------------------------------------------
	LRESULT CALLBACK CWin32App::WndProc( HWND   p_hWnd, 
										 UINT   p_uiMessage, 
										 WPARAM p_wParam, 
										 LPARAM p_lParam )
	{
		if(ms_pkWin32App)
		{
			MSG kMsg;
			ZeroMemory(&kMsg, sizeof(kMsg));

			kMsg.hwnd    = p_hWnd;
			kMsg.message = p_uiMessage;
			kMsg.wParam  = p_wParam;
			kMsg.lParam  = p_lParam;

			// WndProc 메시지
			if(ms_pkWin32App->OnMsgHandler(kMsg))
				return 0;
		}

		// Default 메시지
		switch(p_uiMessage)
		{
#ifdef USE_BARCODE_REDER
	case WM_USER_CARD_DATA:
		return nsBARCODE::Proc( p_wParam, p_lParam );

#endif
		case WM_CREATE:
			{
				g_IME.InitInput();
#ifdef VERSION_USA
				g_IME.SetLockChangeLanguage( true );
#endif
			}
			break;
		case WM_ERASEBKGND:
			{
				if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_CHAT_MAIN_WINDOW ) )
				{
					if( g_pGameIF->GetChatMain()->_IsShowInput() == false )
						return 0;
				}
				if( g_pMoneySeparate != NULL )
				{
					return 0;
				}
			}
			break;
		case WM_SYSCOMMAND: 
			{
				switch( p_wParam&0xfff0 )
				{
				case SC_SCREENSAVE:// 스크린세이버 막기
				case SC_MONITORPOWER:// 모니터 절전기능 막기
				case SC_SIZE:// 윈도우 크기 조절 막기
				case SC_MAXIMIZE:// 윈도우 크기 조절 막기
				case SC_KEYMENU:
					return 0;
				case SC_MOVE:// 풀스크린에서의 화면 이동 막는다
					{
						if( g_pResist->IsFullScreen() == true )
							return 0;
					}break;
				}
			}
			break;
		case WM_SIZE:
			break;
#ifdef THAI_SPECIALWORD
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			{
				if( p_wParam == 18 || p_wParam == 229 )	// 18 왼쪽 alt 키, 229 오른쪽 alt 키
					g_IME.SetKEY_ALTDOWN();
			}
			break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			{
				if( p_wParam == 18 || p_wParam == 229 )
					g_IME.SetKEY_ALTUP();
			}
			break;
#endif
		case WM_ACTIVATE:
			{
				switch(LOWORD(p_wParam))
				{
				case WA_CLICKACTIVE:
				case WA_ACTIVE:
					g_bActive = true;
					if( g_pSoundMgr )
						g_pSoundMgr->OnActive();
					if( ( g_pEngine )&&( g_pEngine->m_pInput ) )
						g_pEngine->m_pInput->UpdateInput();

					break;

				case WA_INACTIVE:
					g_bActive = false;
					if( g_pSoundMgr )
						g_pSoundMgr->OnInactive();
					break;
				}			
			}
			break;
		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			break;
		}

		if( g_IME.IMEProc( p_hWnd, p_uiMessage, p_wParam, p_lParam ) == false )
			return 0;

		return DefWindowProc(p_hWnd, p_uiMessage, p_wParam, p_lParam);
	}
	//---------------------------------------------------------------------------
	void CWin32App::CalculateSize(WORD uiWidth, WORD uiHeight, bool bFullMode)
	{
		m_dwStyle = ( bFullMode )?( WS_POPUP | WS_CLIPCHILDREN ):( WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN );

		m_uiWidth = uiWidth;
		m_uiHeight = uiHeight;

		bool bUpdateRect = false;
		if( m_bFullMode != (BOOL)bFullMode )
		{
			m_bFullMode = bFullMode;

			//if( m_bFullMode )
				bUpdateRect = true;
		}
		else
		{
			if( !m_bFullMode )
				bUpdateRect = true;
		}

		if( bUpdateRect )
			UpdateScreenSize();

		if( !m_bFullMode )
		{
			if( uiHeight > m_uiWindowModeScreenY )
			{
				float fRate = uiWidth / static_cast< float >( uiHeight );
				m_uiWidth = static_cast< WORD >( m_uiWindowModeScreenY * fRate );
				if( m_uiWidth % 2 )
					m_uiWidth += 1;
				m_uiHeight = m_uiWindowModeScreenY;
			}
			else if( uiWidth > m_uiWindowModeScreenX )
			{
				float fRate = uiHeight / static_cast< float >( uiWidth );
				m_uiWidth = m_uiWindowModeScreenX;
				m_uiHeight = static_cast< WORD >( m_uiWindowModeScreenX * fRate );				
				if( m_uiHeight % 2 )
					m_uiHeight += 1;
			}
		}
	}
	//---------------------------------------------------------------------------
	void CWin32App::GetWorkScreenWorkArea(RECT& rcScreen)
	{
		::SystemParametersInfo( SPI_GETWORKAREA, 0, &rcScreen, 0 );
	}
	//---------------------------------------------------------------------------
	void CWin32App::ReSize(WORD uiWidth, WORD uiHeight, bool bFullMode, bool bScreenModeChanged)
	{
		DWORD dwSWPFlag = SWP_NOMOVE | SWP_SHOWWINDOW;
		UINT iPosX = 0, iPosY = 0;

		m_dwStyle = ( bFullMode )?( WS_POPUP | WS_CLIPCHILDREN ):( WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN );
		if( bScreenModeChanged )
		{
			if( !bFullMode )
			{
				RECT rcWorkArea = { 0, };
				GetWorkScreenWorkArea( rcWorkArea );

				iPosX = ( ( rcWorkArea.right - rcWorkArea.left ) - uiWidth) >> 1;
				iPosY = ( ( rcWorkArea.bottom - rcWorkArea.top ) - uiHeight) >> 1;

				if( iPosX < rcWorkArea.left )
					iPosX = rcWorkArea.left;

				if( iPosY < rcWorkArea.top )
					iPosY = rcWorkArea.top;
			}

			dwSWPFlag &= ~SWP_NOMOVE;
		}

		::SetWindowLong( GetHWnd(), GWL_STYLE, m_dwStyle );
		::SetWindowLong( GetHWnd(), GWL_EXSTYLE, m_dwExStyle );

		RECT rcRect = { 0, 0, m_uiWidth, m_uiHeight };
		::AdjustWindowRectEx( &rcRect, m_dwStyle, FALSE, m_dwExStyle);

		::SetWindowPos( GetHWnd(), HWND_NOTOPMOST, iPosX, iPosY, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top, dwSWPFlag );
		::RedrawWindow( ::GetDesktopWindow(), NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN );
	}
	//---------------------------------------------------------------------------
	void CWin32App::UpdateScreenSize(void)
	{
		RECT AdjustRect = { 0, 0, 0, 0 };
		::AdjustWindowRectEx( &AdjustRect, m_dwStyle, FALSE, m_dwExStyle);
		m_uiWindowModeScreenX = ( m_uiWindowModeWorkSpaceRect.right - m_uiWindowModeWorkSpaceRect.left ) - ( AdjustRect.right - AdjustRect.left );
		m_uiWindowModeScreenY = ( m_uiWindowModeWorkSpaceRect.bottom - m_uiWindowModeWorkSpaceRect.top ) - ( AdjustRect.bottom - AdjustRect.top );
	}
	//---------------------------------------------------------------------------
	HINSTANCE CWin32App::GetHInstance() const 
	{ 
		return m_hInstance; 
	}
	//---------------------------------------------------------------------------
	HWND CWin32App::GetHWnd() const 
	{ 
		return m_hWnd; 
	}
	//---------------------------------------------------------------------------
	UINT CWin32App::GetWidth() const 
	{ 
		return m_uiWidth; 
	}
	//---------------------------------------------------------------------------
	UINT CWin32App::GetHeight() const 
	{ 
		return m_uiHeight; 
	}
	//---------------------------------------------------------------------------
	BOOL CWin32App::GetFullMode() const 
	{ 
		return m_bFullMode; 
	}
}
//---------------------------------------------------------------------------