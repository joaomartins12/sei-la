#include "stdafx.h"
#include "SecondPassword.h"
#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"


// 2차 비밀 번호 입력창 생성
void cSecondPassword::_CreateWindow_AskPW()
{
	m_KeyPadWindow.InitScript( "Lobby\\SecondPassword\\lobby_popup_password.tga", CsPoint( (1024-452)/2, (768-460)/2 ), CsPoint( 452, 460 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER);
	m_KeyPadWindow.AddTitle( UISTRING_TEXT("SECONDPASSWORD_TITLE_CONFIRM").c_str(), CsPoint( 0, 20 ), CFont::FS_16, NiColor(0.65f,1.0f,0.0f) );

	m_KeyPadWindow.AddSprite( CsPoint(47, 75 ), CsPoint( 358, 48 ), "Lobby\\SecondPassword\\lobby_Password_glow.tga" );

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_14, NiColor( 140/255.0f, 1.0f, 1.0f ) );	
		m_ConversionStr = m_KeyPadWindow.AddText( &ti, CsPoint( 61, 91 ));	
	}

	m_pOk = m_KeyPadWindow.AddButton( CsPoint( (452 - 137) - 20, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Ok.tga", cWindow::SD_Bu3 );
	if( m_pOk )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_STRING_CONFIRM").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pOk->SetText( &ti );
		m_pOk->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressCheckOkButton);
	}

	m_pCancel = m_KeyPadWindow.AddButton( CsPoint((452 - (137 * 2)) - 20, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Cancel.tga", cWindow::SD_Bu3 );
	if( m_pCancel )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_STRING_CONFIRM_CANCEL").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pCancel->SetText( &ti );
		m_pCancel->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::PressCheckCancelButton);
	}

	m_pChange = m_KeyPadWindow.AddButton( CsPoint( (452 - (137 * 3)) - 20, 404 ), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Ok.tga", cWindow::SD_Bu3 );
	if( m_pChange )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("SECONDPASSWORD_STRING_CONFIRM_CHANGE").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pChange->SetText( &ti );
		m_pChange->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cSecondPassword::Press2ndPassChangeButton);
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, NiColor( 0.2f, 0.6f, 1.0f ) );
		ti.SetText( L"" );
		ti.SetLineGabHeight(3);
		cText* pMsg = m_KeyPadWindow.AddText( &ti, CsPoint( 38, 282 ) );
		if( pMsg )
			pMsg->SetText( UISTRING_TEXT("SECONDPASSWORD_CONFIRM_MESSAGE").c_str(), 380);
	}

	_CreatePassNumberButton();
}


//////////////////////////////////////////////////////////////////////////
// 2차 비밀 번호 확인 창의 확인 버튼 이벤트
void cSecondPassword::PressCheckOkButton(void* pSender, void *pData)		
{
	if( GetSystem() )
		GetSystem()->SendCurrent2ndPassword();
}

// 2차 비밀 번호 확인 창의 취소 버튼 이벤트
void cSecondPassword::PressCheckCancelButton(void* pSender, void *pData)
{
	if( GetSystem() )
		GetSystem()->GotoBackFromAskWindow();
}

void cSecondPassword::Press2ndPassChangeButton( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->OpendChangeWindow();	
}