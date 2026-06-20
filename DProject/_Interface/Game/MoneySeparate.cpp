

#include "stdafx.h"
#include "MoneySeparate.h"

#define IF_MONEY_SEP_TERA_EDIT_ID			5001		// _EDIT_ID 로 검색후 사용
#define IF_MONEY_SEP_MEGA_EDIT_ID			5002		// _EDIT_ID 로 검색후 사용
#define IF_MONEY_SEP_BIT_EDIT_ID			5003		// _EDIT_ID 로 검색후 사용

#define IF_MONEY_SEP_MAX_MONEY				((u8)4000000000)

LRESULT CALLBACK EditSubProc_MoneySeparate(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
WNDPROC EditProc_MoneySeparate;

LRESULT CALLBACK EditSubProc_MoneySeparate(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{	
	case EM_UNDO:
		{
			return 0;
		}
		break;
	}

	return CallWindowProc(EditProc_MoneySeparate,hWnd,uMsg,wParam,lParam);
}



cMoneySeparate*		g_pMoneySeparate = NULL;

void cMoneySeparate::NewInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, sMONEY curMoney, sMONEY minMoney, sMONEY maxMoney )
{
	if( g_pGameIF->IsActiveWindow( wt, nWTValue1 ) == true )
	{
		if( g_pGameIF->_GetPointer( wt, nWTValue1 )->IsEnableWindow() == false )
			return;
	}

	DeleteInstance();

	assert_cs( g_pMoneySeparate == NULL );
	g_pMoneySeparate = NiNew cMoneySeparate;
	g_pMoneySeparate->Init( wt, nWTValue1, curMoney, minMoney, maxMoney );
}

bool cMoneySeparate::UpdateInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pMoneySeparate )
	{
		if( ( g_pMoneySeparate->GetTargetWindowType() == wt )&&( g_pMoneySeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			g_pMoneySeparate->Update(g_fDeltaTime);
			if( g_pMoneySeparate->IsShowWindow() == false )
			{
				DeleteInstance();
				return false;
			}
			return true;
		}		
	}
	return false;
}

cBaseWindow::eMU_TYPE
cMoneySeparate::Update_ForMouseInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	cBaseWindow::eMU_TYPE muReturn = MUT_OUT_WINDOW;
	if( g_pMoneySeparate )
	{
		if( ( g_pMoneySeparate->GetTargetWindowType() == wt )&&( g_pMoneySeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			muReturn = g_pMoneySeparate->Update_ForMouse();
		}

		if( g_pMoneySeparate )
		{
			if( g_pMoneySeparate->IsShowWindow() == false )
			{
				DeleteInstance();			
			}
		}
		else
		{
			return MUT_OUT_WINDOW;
		}
	}
	return muReturn;
}

void cMoneySeparate::RenderInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pMoneySeparate )
	{
		if( ( g_pMoneySeparate->GetTargetWindowType() == wt )&&( g_pMoneySeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			g_pEngine->ScreenSpace();
			g_pMoneySeparate->Render();
		}		
	}
}

bool cMoneySeparate::DeleteInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pMoneySeparate )
	{
		if( ( g_pMoneySeparate->GetTargetWindowType() == wt )&&( g_pMoneySeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			SAFE_NIDELETE( g_pMoneySeparate );
			return true;
		}		
	}
	return false;
}

bool cMoneySeparate::DeleteInstance()
{
	if( g_pMoneySeparate )
	{
		SAFE_NIDELETE( g_pMoneySeparate );
		return true;
	}
	return false;
}

bool cMoneySeparate::IsInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pMoneySeparate )
	{
		if( ( g_pMoneySeparate->GetTargetWindowType() == wt )&&( g_pMoneySeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			return true;
		}		
	}
	return false;
}


void cMoneySeparate::Destroy()
{
	cBaseWindow::Delete();
}

void cMoneySeparate::DeleteResource()
{
	DeleteScript();

	DeleteObject( m_hEditBrush );
	DeleteObject( m_hEditFont );

	::DestroyWindow( m_hEditTera );
	::DestroyWindow( m_hEditMega );
	::DestroyWindow( m_hEditBit );
}

void cMoneySeparate::Init( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1,  sMONEY curMoney, sMONEY minMoney, sMONEY maxMoney )
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint ptSize = CsPoint( 235, 102 );
	CsPoint SepPos = CsPoint( g_nScreenWidth/2 - ptSize.x/2, g_nScreenHeight/2 - ptSize.y/2 );

	InitScript( "Separate\\Money_Win.tga", SepPos, ptSize, false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );

	m_pBtnOk = AddButton( CsPoint( 25 ,55 ), CsPoint( 84, 33 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
	m_pBtnCancel = AddButton( CsPoint( 125, 55 ), CsPoint( 84, 33 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_CANCEL" ).c_str() );

	m_hEditTera = CreateWindow( _T( "edit" ), NULL, WS_CHILD | ES_AUTOHSCROLL | ES_NUMBER, 0, 0, 0, 0, GAMEAPP_ST.GetHWnd(), (HMENU)IF_MONEY_SEP_TERA_EDIT_ID, GAMEAPP_ST.GetHInstance(), NULL );
	SendMessage( m_hEditTera, EM_LIMITTEXT, (WPARAM)(5), 0 );
	MoveWindow( m_hEditTera, SepPos.x + 19, SepPos.y + 25, 44, 13, FALSE );
	ShowWindow( m_hEditTera, SW_SHOW );
	m_hEditMega = CreateWindow( _T( "edit" ), NULL, WS_CHILD | ES_AUTOHSCROLL | ES_NUMBER, 0, 0, 0, 0, GAMEAPP_ST.GetHWnd(), (HMENU)IF_MONEY_SEP_MEGA_EDIT_ID, GAMEAPP_ST.GetHInstance(), NULL );
	SendMessage( m_hEditMega, EM_LIMITTEXT, (WPARAM)(3), 0 );
	MoveWindow( m_hEditMega, SepPos.x + 96, SepPos.y + 25, 35, 13, FALSE );
	ShowWindow( m_hEditMega, SW_SHOW );
	m_hEditBit = CreateWindow( _T( "edit" ), NULL, WS_CHILD | ES_AUTOHSCROLL | ES_NUMBER, 0, 0, 0, 0, GAMEAPP_ST.GetHWnd(), (HMENU)IF_MONEY_SEP_BIT_EDIT_ID, GAMEAPP_ST.GetHInstance(), NULL );
	SendMessage( m_hEditBit, EM_LIMITTEXT, (WPARAM)(3), 0 );
	MoveWindow( m_hEditBit, SepPos.x + 164, SepPos.y + 25, 35, 13, FALSE );
	ShowWindow( m_hEditBit, SW_SHOW );
	
	m_hEditFont = CreateFont( 12, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								FIXED_PITCH | FF_DONTCARE, _T( "Data\\Font\\NanumGothicBold.ttf" ) );

	SendMessage( m_hEditTera, WM_SETFONT, (WPARAM)m_hEditFont, MAKELPARAM( FALSE, 0 ) );
	SendMessage( m_hEditMega, WM_SETFONT, (WPARAM)m_hEditFont, MAKELPARAM( FALSE, 0 ) );
	SendMessage( m_hEditBit, WM_SETFONT, (WPARAM)m_hEditFont, MAKELPARAM( FALSE, 0 ) );

	m_hEditBrush = CreateSolidBrush( RGB( 0, 16, 36 ) );	

#pragma warning( disable : 4311 )
#pragma warning( disable : 4312 )
	EditProc_MoneySeparate=(WNDPROC)SetWindowLong(m_hEditTera,GWL_WNDPROC,(LONG)EditSubProc_MoneySeparate);
	EditProc_MoneySeparate=(WNDPROC)SetWindowLong(m_hEditMega,GWL_WNDPROC,(LONG)EditSubProc_MoneySeparate);
	EditProc_MoneySeparate=(WNDPROC)SetWindowLong(m_hEditBit,GWL_WNDPROC,(LONG)EditSubProc_MoneySeparate);

#pragma warning( default : 4311 )
#pragma warning( default : 4312 )

	if( g_pFocusEdit != NULL )
		g_pFocusEdit->ReleaseFocus();

	m_CurMoney = curMoney;
	m_MinMoney = minMoney;
	m_MaxMoney = CsMin( IF_MONEY_SEP_MAX_MONEY, maxMoney.GetInt() );

	assert_cs( m_CurMoney >= m_MinMoney );
	assert_cs( m_CurMoney <= m_MaxMoney );
	assert_cs( m_MaxMoney >= m_MinMoney );

	_SetMoney( m_CurMoney );

	if( m_MaxMoney.GetTera() )
		SetFocus( m_hEditTera );
	else if( m_MaxMoney.GetMega() )
		SetFocus( m_hEditMega );
	else
		SetFocus( m_hEditBit );

	m_eTargetWindowType = wt;
	m_nTargetWTValue1 = nWTValue1;	

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value= 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

bool cMoneySeparate::OnChangeEdit( DWORD nCtrlID )
{
	HWND hWnd = NULL;
	bool bLimit_999 = false;
	switch( nCtrlID )
	{
	case IF_MONEY_SEP_TERA_EDIT_ID:		hWnd = m_hEditTera;								break;
	case IF_MONEY_SEP_MEGA_EDIT_ID:		hWnd = m_hEditMega;		bLimit_999 = true;		break;
	case IF_MONEY_SEP_BIT_EDIT_ID:		hWnd = m_hEditBit;		bLimit_999 = true;		break;

	default:							return false;
	}

	TCHAR sz[ 16 ];
	GetWindowText( hWnd, sz, 16 );

	if( _ttoi( sz ) == 0 )
		SetWindowText( hWnd, _T( "" ) );
	
	if( bLimit_999 )
	{
		if( _ttoi( sz ) > 999 )
		{
			SetWindowText( hWnd, _T( "999" ) );
			SendMessage( m_hEditMega, EM_SETSEL, -2, -1 );
		}
	}

	return true;
}

void cMoneySeparate::_SetMoney( sMONEY money )
{
	TCHAR sz[ 32 ];
	if( money.GetTera() )
	{
		_stprintf_s( sz, 32, _T( "%d" ), money.GetTera() );
		SetWindowText( m_hEditTera, sz );
		SendMessage( m_hEditTera, EM_SETSEL, -2, -1 );
	}
	else
	{
		SetWindowText( m_hEditTera, _T( "" ) );
	}
	if( money.GetMega() )
	{
		_stprintf_s( sz, 32, _T( "%d" ), money.GetMega() );
		SetWindowText( m_hEditMega, sz );
		SendMessage( m_hEditMega, EM_SETSEL, -2, -1 );
	}
	else
	{
		SetWindowText( m_hEditMega, _T( "" ) );
	}
	if( money.GetBit() )
	{
		_stprintf_s( sz, 32, _T( "%d" ), money.GetBit() );
		SetWindowText( m_hEditBit, sz );
		SendMessage( m_hEditBit, EM_SETSEL, -2, -1 );
	}
	else
	{
		SetWindowText( m_hEditBit, _T( "" ) );
	}

	m_CurMoney = money;
}

sMONEY cMoneySeparate::_GetMoney()
{
	sMONEY money = (u8)0;

	TCHAR sz[ 32 ];
	GetWindowText( m_hEditTera, sz, 32 );
	money.IncreaseTera( _ttoi( sz ) );

	GetWindowText( m_hEditMega, sz, 32 );
	money.IncreaseMega( _ttoi( sz ) );

	GetWindowText( m_hEditBit, sz, 32 );
	money.IncreaseBit( _ttoi( sz ) );

	return money;
}

void cMoneySeparate::Update(float const& fDeltaTime)
{
	if( IsShowWindow() == false )
		return;
}


void cMoneySeparate::OnOk()
{
	m_CurMoney = _GetMoney();
	if( m_CurMoney < m_MinMoney )
	{
		cPrintMsg::PrintMsg( 30030 );
		_SetMoney( m_MinMoney );
		return;
	}
	else if( m_CurMoney > m_MaxMoney )
	{
		cPrintMsg::PrintMsg( 30029 );
		_SetMoney( m_MaxMoney );
		return;
	}

	switch( m_eTargetWindowType )
	{
	case cBaseWindow::WT_TRADE:
		{
			if( net::game )
			{
				// 거래 가능 금액은 40억보다 적어야 한다.				
				net::game->SendTradeAddMoney( (u4)m_CurMoney.GetInt() );
			}
		}
		break;
	default:
		assert_cs( false );
	}

	Close();
}

void cMoneySeparate::OnCancel()
{
	Close();
}

cBaseWindow::eMU_TYPE
cMoneySeparate::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pBtnOk->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		OnOk();
		return muReturn;
	}

	if( m_pBtnCancel->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		OnCancel();
		return muReturn;
	}

	return muReturn;
}

void cMoneySeparate::Render()
{
	if( IsShowWindow() == false )
		return;

	RenderScript();
}

void cMoneySeparate::ResetDevice()
{
	if( IsShowWindow() == false )
		return;

	ResetDeviceScript();
}

LRESULT cMoneySeparate::ApplyEditColor( HWND hEdit, HDC dc )
{
	if( ( hEdit != m_hEditTera )&&
		( hEdit != m_hEditMega )&&
		( hEdit != m_hEditBit ))
	{
		return NULL;
	}

	SetTextColor( dc, RGB( 255, 255, 255 ) );
	SetBkColor( dc, RGB( 0, 16, 36 ) );
	return (LRESULT)m_hEditBrush;
}
