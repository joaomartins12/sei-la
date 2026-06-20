#include "stdafx.h"
#include "SecondPassword.h"
#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

// 2차 비밀 번호 변경 전 현재 사용하고 있는 비밀 번호 입력 창 생성
void cSecondPassword::_CreateWindow_CurrentPwWindow()
{
	m_KeyPadWindow.InitScript( "Lobby\\SecondPassword\\lobby_popup_password.tga", CsPoint( (1024-452)/2, (768-460)/2 ), CsPoint( 452, 460 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER);
	m_KeyPadWindow.AddTitle( UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CURRENT_INPUT_TITLE").c_str(), CsPoint( 0, 20 ), CFont::FS_16, NiColor(0.65f,1.0f,0.0f) );

	m_KeyPadWindow.AddSprite( CsPoint(47, 75 ), CsPoint( 358, 48 ), "Lobby\\SecondPassword\\lobby_Password_glow.tga" );

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_14, NiColor( 140/255.0f, 1.0f, 1.0f ) );	
		m_ConversionStr = m_KeyPadWindow.AddText( &ti, CsPoint( 61, 91 ));	
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, NiColor( 0.2f, 0.6f, 1.0f ) );
		ti.SetText( L"" );
		ti.SetLineGabHeight(3);
		cText* pMsg = m_KeyPadWindow.AddText( &ti, CsPoint( 38, 282 ) );
		if( pMsg )
			pMsg->SetText( UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CURRENT_INPUT_MESSAGE").c_str(), 380);
	}

	m_pOk = m_KeyPadWindow.AddButton( CsPoint( 227, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Ok.tga", cWindow::SD_Bu3 );
	if( m_pOk )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CURRENT_INPUT_CONFIRM").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pOk->SetText( &ti );
		m_pOk->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::Press2ndPassSaveCurrent);// 현재 사용하고 있는 비밀 번호 저장
	}

	m_pCancel = m_KeyPadWindow.AddButton( CsPoint( 90, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Cancel.tga", cWindow::SD_Bu3 );
	if( m_pCancel )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CURRENT_INPUT_CANCEL").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pCancel->SetText( &ti );
		m_pCancel->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressCancleChangeButton);
	}

	_CreatePassNumberButton();
}

//////////////////////////////////////////////////////////////////////////
// 2차 비밀번호 등록창 생성
void cSecondPassword::_CreateWindow_PwChange()
{
	m_KeyPadWindow.InitScript( "Lobby\\SecondPassword\\lobby_popup_password.tga", CsPoint( (1024-452)/2, (768-460)/2 ), CsPoint( 452, 460 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER);
	m_KeyPadWindow.AddTitle( UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CHANGE_INPUT_TITLE").c_str(), CsPoint( 0, 20 ), CFont::FS_16, NiColor(0.65f,1.0f,0.0f) );

	m_KeyPadWindow.AddSprite( CsPoint(47, 75 ), CsPoint( 358, 48 ), "Lobby\\SecondPassword\\lobby_Password_glow.tga" );

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_14, NiColor( 140/255.0f, 1.0f, 1.0f ) );	
		m_ConversionStr = m_KeyPadWindow.AddText( &ti, CsPoint( 61, 91 ));	
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, NiColor( 0.2f, 0.6f, 1.0f ) );
		ti.SetText( L"" );
		ti.SetLineGabHeight(3);
		cText* pMsg = m_KeyPadWindow.AddText( &ti, CsPoint( 38, 282 ) );
		if( pMsg )
			pMsg->SetText( UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CHANGE_INPUT_MESSAGE").c_str(), 380);
	}

	m_pOk = m_KeyPadWindow.AddButton( CsPoint( 227, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Ok.tga", cWindow::SD_Bu3 );
	if( m_pOk )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CHANGE_INPUT_CONFIRM").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pOk->SetText( &ti );
		m_pOk->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::Press2ndPassSaveChange);// 변경할 비밀번호 저장
	}

	m_pCancel = m_KeyPadWindow.AddButton( CsPoint( 90, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Cancel.tga", cWindow::SD_Bu3 );
	if( m_pCancel )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CHANGE_INPUT_CANCEL").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pCancel->SetText( &ti );
		m_pCancel->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressCancleChangeButton);
	}

	_CreatePassNumberButton();
}

// 2차 비밀번호 등록 확인창 생성
void cSecondPassword::_CreateWindow_PwChangeConfirm()
{
	m_KeyPadWindow.InitScript( "Lobby\\SecondPassword\\lobby_popup_password.tga", CsPoint( (1024-452)/2, (768-460)/2 ), CsPoint( 452, 460 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER);
	m_KeyPadWindow.AddTitle( UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CHANGECONFIRM_INPUT_TITLE").c_str(), CsPoint( 0, 20 ), CFont::FS_16, NiColor(0.65f,1.0f,0.0f) );

	m_KeyPadWindow.AddSprite( CsPoint(47, 75 ), CsPoint( 358, 48 ), "Lobby\\SecondPassword\\lobby_Password_glow.tga" );

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_14, NiColor( 140/255.0f, 1.0f, 1.0f ) );	
		m_ConversionStr = m_KeyPadWindow.AddText( &ti, CsPoint( 61, 91 ));	
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, NiColor( 0.2f, 0.6f, 1.0f ) );
		ti.SetText( L"" );
		ti.SetLineGabHeight(3);
		cText* pMsg = m_KeyPadWindow.AddText( &ti, CsPoint( 38, 282 ) );
		if( pMsg )
			pMsg->SetText( UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CHANGECONFIRM_INPUT_MESSAGE").c_str(), 380);
	}

	m_pOk = m_KeyPadWindow.AddButton( CsPoint( 227, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_OK.tga", cWindow::SD_Bu3 );
	if( m_pOk )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CHANGECONFIRM_INPUT_CONFIRM").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pOk->SetText( &ti );
		m_pOk->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::Press2ndPassChangeSend);
	}

	m_pCancel = m_KeyPadWindow.AddButton( CsPoint( 90, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Cancel.tga", cWindow::SD_Bu3 );
	if( m_pCancel )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_CHANGE_WINDOW_CHANGECONFIRM_INPUT_CANCEL").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pCancel->SetText( &ti );
		m_pCancel->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressCancleChangeButton);
	}

	_CreatePassNumberButton();
}
//////////////////////////////////////////////////////////////////////////
void cSecondPassword::Press2ndPassSaveCurrent( void* pSender, void* pData )
{	
	if( GetSystem() )
		GetSystem()->SaveCurrentPassword();// 기존 비밀 번호 저장
}

// 2차 비밀 번호 변경 창의 확인 버튼 이벤트
void cSecondPassword::Press2ndPassSaveChange(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->SaveChangePassword();// 기존 비밀 번호 저장
}

// 2차 비밀 번호 변경 확인창의 확인 버튼 이벤트
void cSecondPassword::Press2ndPassChangeSend(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->CheckChangePasswordAndSend();// 변경할 비밀 번호 확인 및 서버에 요청
}

void cSecondPassword::PressCancleChangeButton( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->Cancel2ndPassChangingWindow();	//
}

//////////////////////////////////////////////////////////////////////////
// 2차 비밀 번호 변경 요청 창
//////////////////////////////////////////////////////////////////////////
void cSecondPassword::_CreateWindow_Change2ndPassword_Ask()
{
	m_AskWindow.InitScript( "Lobby\\popup_Notification.tga", CsPoint( (1024-482)/2, (768-288) / 2 ), CsPoint( 482, 288 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER);
	m_AskWindow.AddTitle( UISTRING_TEXT("SECONDPASSWORD_CHANGE_ASK_WINDOW_TITLE").c_str(), CsPoint( 0, 20 ), CFont::FS_16, NiColor(1.0f,0.17f,0.17f) );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( L"" );
	ti.SetLineGabHeight(5);
	cText* pMsg = m_AskWindow.AddText( &ti, CsPoint( 241, 110 ));
	pMsg->SetText( UISTRING_TEXT("SECONDPASSWORD_CHANGE_ASK_WINDOW_MESSAGE").c_str(), 350);


	m_pUsingSPW = m_AskWindow.AddButton( CsPoint( 482/2 - 150, 230), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );
	if( m_pUsingSPW )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_CHANGE_ASK_WINDOW_CHANGE_BUTTON").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pUsingSPW->SetText( &ti );
		m_pUsingSPW->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressChangePWOkButton);
	}

	m_pUsingLaterSPW = m_AskWindow.AddButton( CsPoint( 482/2 + 13, 230), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );
	if( m_pUsingLaterSPW )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_CHANGE_ASK_WINDOW_CHANGE_LATER").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pUsingLaterSPW->SetText( &ti );
		m_pUsingLaterSPW->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressChangePWCancelButton);
	}

	m_AskWindow.SetVisible(true);
}

// 2차 비밀 번호 변경 승인
void cSecondPassword::PressChangePWOkButton(void* pSender, void *pData)
{
	if( GetSystem() )
		GetSystem()->OpendChangeWindow();	
}

// 2차 비밀 번호 변경 취소
void cSecondPassword::PressChangePWCancelButton(void* pSender, void *pData)
{
	if( GetSystem() )
		GetSystem()->Cancel2ndPassChange();	// 
}