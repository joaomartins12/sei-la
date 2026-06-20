
#include "stdafx.h"
#include "Login.h"

#include "common_vs2019/pPass2.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CLogin::CLogin() :
m_pEditBoxID(NULL),
m_pEditBoxPW(NULL),
m_pLoginBtn(NULL),
m_pExitBtn(NULL),
m_pIDSaveCheckBox(NULL),
m_pUseBarcodeCheckBox(NULL),
m_pConfigeBtn(NULL),
m_pHomePageBtn(NULL)
{

}

CLogin::~CLogin()
{ 
	if( GetSystem() )
		GetSystem()->Remove3DBackgroundData();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

void CLogin::Destroy()
{
	m_Copyright.Delete();
	m_Msg.Delete();
	m_Msg2.Delete();
	DeleteScript();
	m_LogInBack.DeleteScript();
	m_LogIn.DeleteScript();
	g_IME.SetLockConidateList( false );
}

void CLogin::_MakeCopyright()
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_11, NiColor::WHITE );

	std::wstring copyRight = UISTRING_TEXT("COPYRIGHT_TOEI");
	if( !copyRight.empty() )
	{
		ti.SetString( copyRight.c_str() );
		cString* pStr = NiNew cString;
		pStr->AddText( &ti, CsPoint::ZERO );
		m_Copyright.AddTail( pStr );
	}
	copyRight = UISTRING_TEXT("COPYRIGHT_MOVEGAMES");
	if( !copyRight.empty() )
	{
		ti.SetString( copyRight.c_str() );
		cString* pStr = NiNew cString;
		pStr->AddText( &ti, CsPoint::ZERO );
		m_Copyright.AddTail( pStr );
	}
}

void CLogin::LoginInit()
{
	//=================================================================================================================================
	//
	// Main
	//
	//=================================================================================================================================
	InitScript( "Lobby\\LogIn\\background.dds", CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), false );

	//AddSprite( CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), , true );

	_MakeCopyright();
	_MakeLoginBackWindow( !GLOBALDATA_ST.IsLoginSkiped() );
	_MakeLoginWindow( !GLOBALDATA_ST.IsLoginSkiped() );

	GetSystem()->Load3DBackgroundData();
}

void CLogin::_MakeLoginBackWindow(bool bVisible)
{
	m_LogInBack.InitScript( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), false );

	// 게임 종료
	m_pExitBtn = m_LogInBack.AddButton( CsPoint( 100, g_nScreenHeight - 80 ), 
										CsPoint( 176, 50 ), CsPoint( 0, 50 ), "CommonUI\\Simple_btn_L.tga", cWindow::SD_Bu3 );
	if( m_pExitBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("LOGIN_EXIT_GAME").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pExitBtn->SetText( &ti );
		m_pExitBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CLogin::PressExitButton);
	}

	// 게임 설정
// 	m_pConfigeBtn = m_LogInBack.AddButton( CsPoint(g_nScreenWidth - 100 - 176, g_nScreenHeight - 80 ), CsPoint( 176, 50 ), CsPoint( 0, 50 ), "CommonUI\\Simple_btn_L.tga", cWindow::SD_Bu3 );
// 	if( m_pConfigeBtn )
// 	{
// 		cText::sTEXTINFO ti;
// 		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
// 		ti.SetText(UISTRING_TEXT("LOGIN_GAME_CONFIGE").c_str());
// 		ti.s_eTextAlign = DT_CENTER;
// 		m_pConfigeBtn->SetText( &ti );
// 		m_pConfigeBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CLogin::PressConfigeButton);
// 	}

	m_LogInBack.SetVisible( bVisible );	
}

void CLogin::_MakeLoginWindow(bool bVisible)
{
	//=================================================================================================================================
	//
	// Login
	//
	//=================================================================================================================================
	m_LogIn.InitScript(NULL, CsPoint( (1024 - 412)/2, (768-470)/2 ), CsPoint( 412, 470 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );
	m_LogIn.AddSprite( CsPoint::ZERO, CsPoint( 412, 430 ), "Lobby\\LogIn\\lobby_login.tga" );

	{// 계정 로그인
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_16 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("LOGIN_WINDOW_TITLE_USER_ID").c_str() );
		m_LogIn.AddText( &ti, CsPoint(206, 30) );
	}
		
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText( UISTRING_TEXT("LOGIN_WINDOW_USER_ID").c_str() );
		m_LogIn.AddText( &ti, CsPoint( 46, 94 ) );
	}

	{
		// ID 에딧박스	
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14, NiColor::BLACK );
		ti.SetText( _T( "" ) );
		ti.s_bOutLine = false;
		m_pEditBoxID = NiNew cEditBox;
		m_pEditBoxID->Init( m_LogIn.GetRoot(), CsPoint( 51, 124 ), CsPoint( 300, 18 ), &ti, false );
		m_pEditBoxID->SetEmptyMsgText( UISTRING_TEXT("LOGIN_ID_EDITBOX_EMPTY_MSG").c_str(), NiColor(0.5f,0.5f,0.5f) );
		m_pEditBoxID->SetFontLength( 20 );
		m_pEditBoxID->EnableUnderline( ti.s_Color );
		m_pEditBoxID->SetEnableSound( true );
		m_LogIn.AddChildControl( m_pEditBoxID );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText( UISTRING_TEXT("LOGIN_WINDOW_USER_PW").c_str() );
		m_LogIn.AddText( &ti, CsPoint( 46, 167 ) );
	}

	{
		// PW 에딧박스
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14, NiColor::BLACK );
		ti.SetText( _T( "" ) );
		ti.s_bOutLine = false;
		m_pEditBoxPW = NiNew cEditBox;
		m_pEditBoxPW->Init( m_LogIn.GetRoot(), CsPoint( 51, 197 ), CsPoint( 300, 18 ), &ti, false );
		m_pEditBoxPW->SetFontLength( 20 );
		m_pEditBoxPW->EnableUnderline( ti.s_Color );
		m_pEditBoxPW->SetEmptyMsgText( UISTRING_TEXT("LOGIN_PASSWORD_EDITBOX_EMPTY_MSG").c_str(), NiColor(0.5f,0.5f,0.5f) );
		m_pEditBoxPW->SetEnableSound( true );
		m_pEditBoxPW->GetText()->SetMark( L'*' );
		m_LogIn.AddChildControl( m_pEditBoxPW );
	}
	
	// 아이디 저장 체크 박스
#ifdef USE_ID_SAVE
	m_pIDSaveCheckBox = m_LogIn.AddCheckBox( CsPoint( 46, 248 ), CsPoint( 24, 24 ), CsPoint( 0, 24 ), "CommonUI\\checkbox.tga", cWindow::SD_Wi1 );
	m_pIDSaveCheckBox->SetCheck( g_pResist->m_Global.s_bSaveID );
	m_pIDSaveCheckBox->AddEvent(cCheckBox::BUTTON_LBDOWN_EVENT, this, &CLogin::PressIDSaveCheckBox); // [4/27/2016 hyun] 추가


	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_13, NiColor(0.28,0.6,1.0) );
	ti.SetText( UISTRING_TEXT("LOGIN_WINDOW_USER_ID_SAVE").c_str() );
	m_LogIn.AddText( &ti, CsPoint( 72, 255 ) );

	TCHAR* szID = NULL;
	if( ( g_pResist->m_Global.s_bSaveID == true )&&( g_pResist->m_Global.s_szLastConnectID[ 0 ] != NULL ) )
		szID = g_pResist->m_Global.s_szLastConnectID;

	if( szID != NULL && m_pEditBoxID )
		m_pEditBoxID->SetText( szID );
#endif

	m_pLoginBtn = m_LogIn.AddButton( CsPoint(239,240), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );
	if( m_pLoginBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText( UISTRING_TEXT("LOGIN_WINDOW_LOGIN").c_str() );
		ti.s_eTextAlign = DT_CENTER;
		m_pLoginBtn->SetText( &ti );
		m_pLoginBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CLogin::PressLoginButton); // [4/27/2016 hyun] 추가
		m_pLoginBtn->AddEvent(cButton::BUTTON_RBUP_EVENT, this, &CLogin::ChangeIP);
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_11 );
		ti.SetText( UISTRING_TEXT("LOGIN_WINDOW_GOTO_HOMEPAGE").c_str() );
		ti.s_eTextAlign = DT_RIGHT;		
		m_pHomePageBtn = m_LogIn.AddButton( CsPoint(376 - ti.GetTextWidth(),390), CsPoint( ti.GetTextWidth(), ti.GetHeight() ), cButton::IMAGE_ALPHA_1, &ti, cWindow::SD_Bu3 );
		if( m_pHomePageBtn )
			m_pHomePageBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CLogin::_HomepageOpen );
	}
	
	m_LogIn.SetVisible( bVisible );

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_11, NiColor::WHITE );
		g_pStringAnalysis->Cut_Parcing( &m_Msg, 315, UISTRING_TEXT("LOGIN_WINDOW_ETC_MSG").c_str(), &ti );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12, NiColor::WHITE );
		//ti.s_eTextAlign = DT_CENTER;
		g_pStringAnalysis->Cut_Parcing( &m_Msg2, 315, UISTRING_TEXT("LOGIN_WINDOW_ETC2_MSG").c_str(), &ti );	
	}

	if( bVisible )// 에디트 박스 포커스 설정
	{		
		g_IME.SetLockConidateList( true );
		SetFocusEdit();	
	}
	else
	{
		PressLoginButton(NULL,NULL);
	}
}

void CLogin::_HomepageOpen(void* pSender, void* pData)
{
	//std::wstring homepageUrl = GLOBALDATA_ST.GetHomePageUrl();
	//if( !homepageUrl.empty() )
	//	ShellExecute(NULL, _T("open"), _T("iexplore"), homepageUrl.c_str(), NULL, SW_SHOWDEFAULT );
	GetSystem()->ShowIntro();
}

void CLogin::SetFocusEdit()
{
	SAFE_POINTER_RET( m_pEditBoxID );
	SAFE_POINTER_RET( m_pEditBoxPW );

#ifdef USE_DMO_INI
	SetAccountInfoFromIni();
#endif

	TCHAR const* pId = m_pEditBoxID->GetTextAll();
	if( NULL == pId || _tcslen(pId) <= 0 )
		m_pEditBoxID->SetFocus();
	else
		m_pEditBoxPW->SetFocus();
}

void CLogin::ReleaseFocusEdit()
{
	if(g_pFocusEdit)
		g_pFocusEdit->ReleaseFocus();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
BOOL CLogin::UpdateMouse()
{
	g_IME.Native_Off( GAMEAPP_ST.GetHWnd() );

	if( cMessageBox::UpdateList() == true )
	{
		if( GetSystem()->IsEnableLoginTime() == true )
		{
			// 메세지 박스에 취소 버튼 추가
			cMessageBox* pMsg =	cMessageBox::GetMessageBox( 10020 );
			if( pMsg && ( pMsg->GetCancelBtnControl() == NULL ) )
			{
				pMsg->SetBtnControl( CsTalk_Message::MT_CANCEL );
			}			
			GetSystem()->ResetLoginTime();
		}
		return 1;
	}

	if( m_LogInBack.IsVisible() )
	{
		if(m_pConfigeBtn && m_pConfigeBtn->Update_ForMouse() == cButton::ACTION_CLICK )
			return TRUE;

		if(m_pExitBtn && m_pExitBtn->Update_ForMouse() == cButton::ACTION_CLICK )
			return TRUE;
	}
	
	if( m_LogIn.IsVisible() )
	{
		if(m_pEditBoxID && m_pEditBoxID->Update_ForMouse( CsPoint::ZERO ) == cEditBox::ACTION_CLICK)
			return TRUE;

		if(m_pEditBoxPW && m_pEditBoxPW->Update_ForMouse( CsPoint::ZERO ) == cEditBox::ACTION_CLICK)
			return TRUE;

		if(m_pLoginBtn && m_pLoginBtn->Update_ForMouse() == cButton::ACTION_CLICK )
			return TRUE;

		if(m_pIDSaveCheckBox && m_pIDSaveCheckBox->Update_ForMouse() == cCheckBox::ACTION_CLICK )
			return TRUE;

		if(m_pUseBarcodeCheckBox && m_pUseBarcodeCheckBox->Update_ForMouse() == cCheckBox::ACTION_CLICK )
			return TRUE;

		if(m_pHomePageBtn&& m_pHomePageBtn->Update_ForMouse() == cCheckBox::ACTION_CLICK )
			return TRUE;
	}

	return FALSE;
}


void CLogin::Update(float fDeltaTime)
{	
	UpdateScript( fDeltaTime );
	m_LogInBack.UpdateScript( fDeltaTime );
	m_LogIn.UpdateScript( fDeltaTime );
}

BOOL CLogin::UpdateKeyboard(const MSG& p_kMsg)
{
	const int isBitSet = (DWORD)p_kMsg.lParam & 0x40000000; 

	switch(p_kMsg.message)
	{	
	case WM_KEYDOWN:
		{
			switch(p_kMsg.wParam)
			{
			case VK_RETURN:
				{
					if(isBitSet == 0)
					{
						if(GetSystem() && !GetSystem()->IsShowVideo())
						{	
							if(m_pLoginBtn)
								m_pLoginBtn->KeyboardBtnDn();							
						}
					}
				}
				break;
			case VK_TAB:
				{
					if(isBitSet == 0)
					{
						if( g_pFocusEdit == m_pEditBoxID )
						{
							if(m_pEditBoxPW)
								m_pEditBoxPW->SetFocus();
						}
						else
						{
							if(m_pEditBoxID)
								m_pEditBoxID->SetFocus();
						}						
					}
				}
				break;		
			}
		}
		break;

	case WM_KEYUP:
		{
			switch(p_kMsg.wParam)
			{
			case VK_RETURN:
				{
					if(GetSystem() && GetSystem()->IsShowVideo())
					{	
						GetSystem()->SetShowVideo(false);
					}
					else
					{
						if(m_pLoginBtn)
						{
							m_pLoginBtn->KeyboardBtnUp();
							PressLoginButton(m_pLoginBtn, NULL);
						}
					}				
				}
				break;

			default:
				{
					if(GetSystem() && GetSystem()->IsShowVideo())
					{	
						GetSystem()->SetShowVideo(false);
					}
				}
			}
		}
		break;
	}

	return FALSE;
}	

void CLogin::Render()
{
	if( m_Copyright.GetSize() > 0 )
		m_Copyright.Render(CsPoint( g_nScreenWidth >> 1, g_nScreenHeight - 70 ) , 0, DT_CENTER);

	m_LogInBack.RenderScript();
	m_LogIn.RenderScript();
	if( m_LogIn.IsVisible() )
	{
		m_Msg.Render( m_LogIn.GetRootClient() + CsPoint( 36, 315 ), 0 );
		m_Msg2.Render( m_LogIn.GetRootClient() + CsPoint( (m_LogIn.GetRootSize().x/2), 455 ), 0, DT_CENTER );
	}
}

// 배경 UI 랜더링
void CLogin::RenderScreenUI()
{
	RenderScript();
}


void CLogin::Render3DModel()
{
	if( GetSystem() )
		GetSystem()->RenderBackgroundMap();
}

void CLogin::Update3DAccumtime( float const& fAccumTime )
{
	if( GetSystem() )
		GetSystem()->UpdateBackgroundMap( fAccumTime );
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

void CLogin::ResetDevice()
{
	ResetDeviceScript();
	m_Copyright.ResetDevice();
	m_Msg.ResetDevice();
	m_Msg2.ResetDevice();
	m_LogInBack.ResetDeviceScript();
	m_LogIn.ResetDeviceScript();
}

bool CLogin::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eRequest_Login, this );

	return true;
}

void CLogin::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eRequest_Login:	
		{
			if( m_pUseBarcodeCheckBox )
			{
				m_pUseBarcodeCheckBox->SetCheck( g_pResist->m_Global.s_bUseBarcode == TRUE );
				PressLoginButton(NULL, NULL);
			}
		}
		break;
	}
}

// [4/27/2016 hyun] 추가
#ifdef USE_BARCODE_REDER
void CLogin::PressBarcodeCheckBox(void* pSender, void* pData)
{
	cCheckBox * pCheck = static_cast< cCheckBox*>(pSender);
	if( GetSystem() )
		GetSystem()->ToggleUseBarcode( pCheck->IsCheck() );
}
#endif // USE_BARCODE_REDER

void CLogin::PressIntroButton( void* pSender, void* pData )
{
	SAFE_POINTER_RET(GetSystem());
	GetSystem()->SetShowVideo(true);
	GetSystem()->ShowIntro();
}

void CLogin::PressOpeningButton( void* pSender, void* pData )
{
	SAFE_POINTER_RET(GetSystem());
	GetSystem()->SetShowVideo(true);
	GetSystem()->ShowOpening();
}

void CLogin::ChangeIP(void* pSender, void* pData)
{
	GLOBALDATA_ST.ToggleLiveServer();
}

void CLogin::PressLoginButton( void* pSender, void* pData )
{
	SAFE_POINTER_RET(GetSystem());

	std::wstring userid;
	std::wstring userpw;
	if( !m_LogIn.IsVisible() )
	{
		std::string id = GLOBALDATA_ST.GetAccountID();
		std::string pw = GLOBALDATA_ST.GetAccountPW();
		DmCS::StringFn::From( userid, id );
		DmCS::StringFn::From( userpw, pw );
	}
	else
	{
		if (m_pEditBoxID) {

			userid = m_pEditBoxID->GetTextAll();
#ifdef DEBUG_NETWORK
			//DBG("Uid -> %s\n", userid);
#endif 
		}
		if (m_pEditBoxPW)
		{

			userpw = m_pEditBoxPW->GetTextAll();
#ifdef DEBUG_NETWORK
			//DBG("Uid -> %s\n", userpw);
#endif 

		}
	}
	GetSystem()->TryToLogin( userid.c_str(), userpw.c_str() );
}

void CLogin::PressExitButton( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->GameExit();
}

void CLogin::PressConfigeButton(void* pSender, void* pData)
{

}

void CLogin::PressIDSaveCheckBox( void* pSender, void* pData )
{
	cCheckBox * pCheck = static_cast< cCheckBox*>(pSender);
	if( GetSystem() )
		GetSystem()->ToggleIDSaveCheckBox( pCheck->IsCheck() );
}

void CLogin::SetAccountInfoFromIni()
{
	SAFE_POINTER_RET(m_pEditBoxID);
	SAFE_POINTER_RET(m_pEditBoxPW);

	cProfile x("./dmo.ini");

#ifdef VERSION_USA
	m_pEditBoxID->SetText( nBase::m2w( x.GetStr("network_eng", "id") ).c_str());
	m_pEditBoxPW->SetText( nBase::m2w( x.GetStr("network_eng", "pw") ).c_str());
#elif defined VERSION_TH
	m_pEditBoxID->SetText( nBase::m2w( x.GetStr("network_tha", "id") ).c_str());
	m_pEditBoxPW->SetText( nBase::m2w( x.GetStr("network_tha", "pw") ).c_str());
#elif defined VERSION_TW
	m_pEditBoxID->SetText( nBase::m2w( x.GetStr("network_taiwan", "id") ).c_str());
	m_pEditBoxPW->SetText( nBase::m2w( x.GetStr("network_taiwan", "pw") ).c_str());
#elif defined VERSION_HK
	m_pEditBoxID->SetText( nBase::m2w( x.GetStr("network_hongkong", "id") ).c_str());
	m_pEditBoxPW->SetText( nBase::m2w( x.GetStr("network_hongkong", "pw") ).c_str());
#else
	m_pEditBoxID->SetText( nBase::m2w( x.GetStr("network", "id") ).c_str());
	m_pEditBoxPW->SetText( nBase::m2w( x.GetStr("network", "pw") ).c_str());
#endif
}
// @@ 여기까지