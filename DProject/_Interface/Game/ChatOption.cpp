

#include "stdafx.h"

#include "ChatOption.h"

void cChatOption::Destroy()
{
	cBaseWindow::Delete();	
}

void cChatOption::DeleteResource()
{
	DeleteScript();
}

void cChatOption::Create( int nValue /* = 0  */ )
{
	if( cBaseWindow::Init() == false )
		return;

	ChatContents* pChatCon = (ChatContents*)CONTENTSSYSTEM_PTR->GetContents(E_CT_CHATTING_STANDARDIZATION);

	CsPoint ptStart = g_pGameIF->GetChatMain()->_GetOptionButtonPos();

	ptStart.x += 16;
	ptStart.y -= ( 88 - 16 );

	InitScript( "Chat\\Option\\Op_Win.tga", ptStart, CsPoint( 184, 88 ), false, IFREGION_X::LEFT, IFREGION_Y::BOTTOM, false );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	m_pCheckBox[ NS_CHAT::NORMAL ] = AddCheckBox( CsPoint( 8, 9 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "System\\Check.tga" );
	m_pCheckBox[ NS_CHAT::NORMAL ]->SetCheck( pChatCon->_IsChatEnable( NS_CHAT::NORMAL ) );
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_NORMAL" ).c_str() );
	AddText( &ti, CsPoint( 28, 9 ) );

	m_pCheckBox[ NS_CHAT::PARTY ] = AddCheckBox( CsPoint( 90, 9 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "System\\Check.tga" );
	m_pCheckBox[ NS_CHAT::PARTY ]->SetCheck( pChatCon->_IsChatEnable( NS_CHAT::PARTY ) );
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_PARTY" ).c_str() );
	AddText( &ti, CsPoint( 110, 9 ) );

	m_pCheckBox[ NS_CHAT::GUILD ] = AddCheckBox( CsPoint( 8, 35 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "System\\Check.tga" );
	m_pCheckBox[ NS_CHAT::GUILD ]->SetCheck( pChatCon->_IsChatEnable( NS_CHAT::GUILD ) );
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_GUILD" ).c_str() );
	AddText( &ti, CsPoint( 28, 35 ) );

	m_pCheckBox[ NS_CHAT::WHISPER ] = AddCheckBox( CsPoint( 90, 35 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "System\\Check.tga" );
	m_pCheckBox[ NS_CHAT::WHISPER ]->SetCheck( pChatCon->_IsChatEnable( NS_CHAT::WHISPER ) );
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_WHISPER" ).c_str() );
	AddText( &ti, CsPoint( 110, 35 ) );

	m_pCheckBox[ NS_CHAT::SHOUT ] = AddCheckBox( CsPoint( 8, 61 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "System\\Check.tga" );
	m_pCheckBox[ NS_CHAT::SHOUT ]->SetCheck( pChatCon->_IsChatEnable( NS_CHAT::SHOUT ) );
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_SHOUT" ).c_str() );
	AddText( &ti, CsPoint( 28, 61 ) );

#ifdef MEGAPHONE_BAN
	if( g_pCharMng->GetTamerUser()->GetMegaPhoneBan() )
	{
		// 채팅창 옵션 버튼에 확성기 버튼 추가
		m_pCheckBox[ NS_CHAT::MEGAPHONE ] = AddCheckBox( CsPoint( 90, 61 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "System\\Check.tga" );
		m_pCheckBox[ NS_CHAT::MEGAPHONE ]->SetCheck( pChatCon->_IsChatEnable( NS_CHAT::MEGAPHONE ) );
		std::wstring wsMegaphone = L" " + UISTRING_TEXT( "COMMON_TXT_MEGAPHONE" );
		ti.SetText( wsMegaphone.c_str() );
		AddText( &ti, CsPoint( 110, 61 ) );
	}
#endif
}

void cChatOption::Update(float const& fDeltaTime)
{
}

cBaseWindow::eMU_TYPE
cChatOption::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	INT	iMaxCount = NS_CHAT::MAXCOUNT;

	for( int i=0; i<iMaxCount; ++i )
	{
#ifdef MEGAPHONE_BAN
		if( g_pCharMng->GetTamerUser()->GetMegaPhoneBan() == false && (i == iMaxCount - 1) )
			break;
#endif
		if( m_pCheckBox[ i ]->Update_ForMouse() == cCheckBox::ACTION_CLICK )
		{

			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::SET_ENABLE;
			CProtocol.m_Index = i;	//	NS_CHAT::NORMAL ~ NS_CHAT::MEGAPHONE
			CProtocol.m_value = ( m_pCheckBox[ i ]->IsCheck() ? 1 : 0 );
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
			return muReturn;
		}
	}

	return muReturn;
}

void cChatOption::Render()
{
	RenderScript();
}

void cChatOption::ResetDevice()
{	
	ResetDeviceScript();
}