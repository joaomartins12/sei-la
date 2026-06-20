#include "stdafx.h"
#include "SecondPassword.h"
#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"


//////////////////////////////////////////////////////////////////////////
// 2차 비밀번호 등록창 생성
void cSecondPassword::_CreateWindow_Regist()
{
	m_KeyPadWindow.InitScript( "Lobby\\SecondPassword\\lobby_popup_password.tga", CsPoint( (1024-452)/2, (768-460)/2 ), CsPoint( 452, 460 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER);
	m_KeyPadWindow.AddTitle( UISTRING_TEXT("SECONDPASSWORD_TITLE_SETUP").c_str(), CsPoint( 0, 20 ), CFont::FS_16, NiColor(0.65f,1.0f,0.0f) );

	m_KeyPadWindow.AddSprite( CsPoint(47, 75 ), CsPoint( 358, 48 ), "Lobby\\SecondPassword\\lobby_Password_glow.tga" );

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_14, NiColor( 140/255.0f, 1.0f, 1.0f ) );	
		m_ConversionStr = m_KeyPadWindow.AddText( &ti, CsPoint( 61, 91 ));	
	}

	m_pOk = m_KeyPadWindow.AddButton( CsPoint( 227, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Ok.tga", cWindow::SD_Bu3 );
	if( m_pOk )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_STRING_REGISTER").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pOk->SetText( &ti );
		m_pOk->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::Press2ndPassRegistFirst);
	}

	m_pCancel = m_KeyPadWindow.AddButton( CsPoint( 90, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Cancel.tga", cWindow::SD_Bu3 );
	if( m_pCancel )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_STRING_REGISTER_CANCEL").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pCancel->SetText( &ti );
		m_pCancel->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressRegistCancelButton);
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, NiColor( 0.2f, 0.6f, 1.0f ) );
		ti.SetText( L"" );
		ti.SetLineGabHeight(3);
		cText* pMsg = m_KeyPadWindow.AddText( &ti, CsPoint( 38, 282 ) );
		if( pMsg )
			pMsg->SetText( UISTRING_TEXT("SECONDPASSWORD_SETUP_MESSAGE").c_str(), 380);
	}
	_CreatePassNumberButton();
}

// 2차 비밀번호 등록 확인창 생성
void cSecondPassword::_CreateWindow_RegistConfirm()
{
	m_KeyPadWindow.InitScript( "Lobby\\SecondPassword\\lobby_popup_password.tga", CsPoint( (1024-452)/2, (768-460)/2 ), CsPoint( 452, 460 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER);
	m_KeyPadWindow.AddTitle( UISTRING_TEXT("SECONDPASSWORD_TITLE_CONFIRMSETUP").c_str(), CsPoint( 0, 20 ), CFont::FS_16, NiColor(0.65f,1.0f,0.0f) );

	m_KeyPadWindow.AddSprite( CsPoint(47, 75 ), CsPoint( 358, 48 ), "Lobby\\SecondPassword\\lobby_Password_glow.tga" );

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_14, NiColor( 140/255.0f, 1.0f, 1.0f ) );	
		m_ConversionStr = m_KeyPadWindow.AddText( &ti, CsPoint( 61, 91 ));	
	}

	m_pOk = m_KeyPadWindow.AddButton( CsPoint( 227, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_OK.tga", cWindow::SD_Bu3 );
	if( m_pOk )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_STRING_REGISTER").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pOk->SetText( &ti );
		m_pOk->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::Press2ndPassRegistSend);
	}

	m_pCancel = m_KeyPadWindow.AddButton( CsPoint( 90, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Cancel.tga", cWindow::SD_Bu3 );
	if( m_pCancel )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_STRING_REGISTER_CANCEL").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pCancel->SetText( &ti );
		m_pCancel->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressRegistCancelButton);
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, NiColor( 0.2f, 0.6f, 1.0f ) );
		ti.SetText( L"" );
		ti.SetLineGabHeight(3);
		cText* pMsg = m_KeyPadWindow.AddText( &ti, CsPoint( 38, 282 ) );
		if( pMsg )
			pMsg->SetText( UISTRING_TEXT("SECONDPASSWORD_SETUP_MESSAGE").c_str(), 380);
	}
	_CreatePassNumberButton();
}
//////////////////////////////////////////////////////////////////////////

// 2차 비밀 번호 등록 확인 요청 창
void cSecondPassword::_CreateWindow_UsingAsk()
{
	m_AskWindow.InitScript( "Lobby\\popup_Notification.tga", CsPoint( (1024-482)/2, (768-288) / 2 ), CsPoint( 482, 288 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER);
	m_AskWindow.AddTitle( UISTRING_TEXT("SECONDPASSWORD_REGIST_ASK_TITLE").c_str(), CsPoint( 0, 20 ), CFont::FS_16, NiColor(1.0f,0.17f,0.17f) );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( L"" );
	ti.SetLineGabHeight(5);
	cText* pMsg = m_AskWindow.AddText( &ti, CsPoint( 241, 110 ));
	pMsg->SetText( UISTRING_TEXT("SECONDPASSWORD_REGIST_ASK_MSG").c_str(), 350);

	m_pCheckUseSPW = m_AskWindow.AddCheckBox( CsPoint( 420, 195 ), CsPoint( 19, 19 ), CsPoint( 0, 19 ), "CommonUI\\CheckBT.tga", cWindow::SD_Wi1 );

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, NiColor( 140/255.0f, 1.0f, 1.0f ) );	
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_CHECKBOX_ONEDAY_SKIP").c_str());
		m_AskWindow.AddText( &ti, CsPoint( 410, 197 ));
	}

	m_pUsingSPW = m_AskWindow.AddButton( CsPoint( 482/2 - 150, 230), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );
	if( m_pUsingSPW )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_REGIST_CONFIG_BUTTON").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pUsingSPW->SetText( &ti );
		m_pUsingSPW->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressUsingButton); // [4/27/2016 hyun] 추가
	}

	m_pUsingLaterSPW = m_AskWindow.AddButton( CsPoint( 482/2 + 13, 230), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );
	if( m_pUsingLaterSPW )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_REGIST_LATER_BUTTON").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pUsingLaterSPW->SetText( &ti );
		m_pUsingLaterSPW->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressLaterButton); // [4/27/2016 hyun] 추가
	}

	m_AskWindow.SetVisible(true);
}

void cSecondPassword::PressUsingButton(void* pSender, void* pData)
{
	m_AskWindow.SetVisible( false );
	m_KeyPadWindow.SetEnableWindow(true);
}

void cSecondPassword::PressLaterButton(void* pSender, void* pData)
{
	bool bHideOneDay = false;// 하루동안 물어보는창 띄우지 않는가?
	if( m_pCheckUseSPW && m_pCheckUseSPW->IsCheck() )
		bHideOneDay = true;

	GetSystem()->SkipInputSecondPassword(bHideOneDay);
	m_AskWindow.SetVisible( false );
	m_KeyPadWindow.SetEnableWindow(true);
}

// 2차 비밀 번호 등록 창의 확인 버튼 이벤트
void cSecondPassword::Press2ndPassRegistFirst(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->SaveRegistPassword();
}

// 2차 비밀 번호 등록 확인창의 확인 버튼 이벤트
void cSecondPassword::Press2ndPassRegistSend(void* pSender, void* pData)
{	
	if( GetSystem() )
		GetSystem()->CheckRegistPasswordAndSend();
}

// 비밀 번호 등록창에서 취소 버튼 이벤트 
void cSecondPassword::PressRegistCancelButton(void* pSender, void* pData)
{	
	if( GetSystem() )
		GetSystem()->Cancel2ndPassRegistWindow();
}