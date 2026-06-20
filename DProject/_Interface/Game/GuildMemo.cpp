#include "stdafx.h"
#ifdef GUILD_RENEWAL
#include "GuildMemo.h"

void cGuildMemo::Destroy()
{
	cBaseWindow::Delete();
}

void cGuildMemo::DeleteResource()
{
	m_strMemo.Delete();

	if( m_eStat == WRITE )
	{
		if( m_pEditMemo )
			m_pEditMemo->ReleaseFocus();
		if( m_pEditMemo )
			SAFE_NIDELETE( m_pEditMemo );

		SAFE_NIDELETE( m_pOKBtn );
 	}

	SAFE_NIDELETE( m_pCancelBtn );

	for( int i=0; i<9; i++ )
	{
		SAFE_NIDELETE( m_pSprite[ i ] );
	}

	DeleteScript();
}

void cGuildMemo::Create( int nValue /*= 0 */ )
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) == false )
		return;
	
	cBaseWindow::Init();

	std::pair<int, cCommunityContent::sGUILD_PERSON_INFO*> guildPersonInfo;
	guildPersonInfo.first = nValue;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILD_PERSON_INFO, &guildPersonInfo );
	cCommunityContent::sGUILD_PERSON_INFO* m_sInfo = guildPersonInfo.second;

	if( m_sInfo->s_szName.compare( g_pCharMng->GetTamerUser()->GetName() ) == 0 )			// 길드원 정보가 나일 때 : 메모 쓰기
		m_eStat = WRITE;
	else	// 길드원 정보가 다른 길드원일 때 : 읽기만 가능
		m_eStat = READ;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );

	// 저장되 있는 길드원 메모

	m_szMemoInput = m_sInfo->s_szGuildMemo;
	m_strMemo.Delete();
	g_pStringAnalysis->Cut( &m_strMemo, 130, m_szMemoInput.c_str(), &ti );

	// 메모 윈도우 띄울 위치
	CsPoint ptRootPos = CsPoint( 15, 0 ) + CURSOR_ST.GetPos();

	switch( m_eStat )
	{
	case READ:
		ptRootPos += CsPoint( 130, 0 );	// 길드 메뉴 팝업창 크기만큼 옆으로
		break;
	case WRITE:
		m_pEditMemo = NULL;
		OnEditBox( m_szMemoInput.c_str() );
		m_strMemo.SetUnderLineCursor();
		break;
	}

	InitScript( NULL, ptRootPos, CsPoint( 230, 190 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );

	// 이름 
	ti.s_eFontSize = CFont::FS_12;
	ti.s_Color = NiColor( 34/255.0f, 255/255.0f, 73/255.0f );

	ti.SetText( m_sInfo->s_szName.c_str() );
	AddText( &ti, CsPoint( 15, 15 ) );

	// BG
	CsPoint patch = CsPoint( 15, 15 );
	CsPoint size = CsPoint( 180, 190 );

	m_pSprite[ 0 ] = NiNew cSprite;
	m_pSprite[ 0 ]->Init( m_pRoot, CsPoint::ZERO, CsPoint( patch.x, patch.y ), "Control_G\\Popup\\ON2Dlg1.tga", false );

	m_pSprite[ 1 ] = NiNew cSprite;
	m_pSprite[ 1 ]->Init( m_pRoot, CsPoint( patch.x, 0 ), CsPoint( size.x-patch.x*2, patch.y ), "Control_G\\Popup\\ON2Dlg2.tga", false );

	m_pSprite[ 2 ] = NiNew cSprite;
	m_pSprite[ 2 ]->Init( m_pRoot, CsPoint( size.x - patch.x, 0 ), CsPoint( patch.x, patch.y ), "Control_G\\Popup\\ON2Dlg3.tga", false );


	m_pSprite[ 3 ] = NiNew cSprite;
	m_pSprite[ 3 ]->Init( m_pRoot, CsPoint( 0, patch.y ), CsPoint( patch.x, size.y-patch.y*2 ), "Control_G\\Popup\\ON2Dlg4.tga", false );

	m_pSprite[ 4 ] = NiNew cSprite;
	m_pSprite[ 4 ]->Init( m_pRoot, CsPoint( patch.x, patch.y ), CsPoint( size.x-patch.x*2, size.y-patch.y*2 ), "Control_G\\Popup\\ON2Dlg5.tga", false );

	m_pSprite[ 5 ] = NiNew cSprite;
	m_pSprite[ 5 ]->Init( m_pRoot, CsPoint( size.x-patch.x, patch.y ), CsPoint( patch.x, size.y-patch.y*2 ), "Control_G\\Popup\\ON2Dlg6.tga", false );

	m_pSprite[ 6 ] = NiNew cSprite;
	m_pSprite[ 6 ]->Init( m_pRoot, CsPoint( 0, size.y-patch.y ), CsPoint( patch.x, patch.y ), "Control_G\\Popup\\ON2Dlg7.tga", false );

	m_pSprite[ 7 ] = NiNew cSprite;
	m_pSprite[ 7 ]->Init( m_pRoot, CsPoint( patch.x, size.y-patch.y ), CsPoint( size.x-patch.x*2, patch.y ), "Control_G\\Popup\\ON2Dlg8.tga", false );

	m_pSprite[ 8 ] = NiNew cSprite;
	m_pSprite[ 8 ]->Init( m_pRoot, CsPoint( size.x-patch.x, size.y-patch.y ), CsPoint( patch.x, patch.y ), "Control_G\\Popup\\ON2Dlg9.tga", false );

	for( int i=0; i<9; ++i )
		m_pSprite[ i ]->SetAlpha( 0.7f );

	// 텍스트 BG
	m_pInputBox = AddSprite( CsPoint( 18, 50 ), CsPoint( 145, 80 ), "Talk\\Common_Seletbar.tga", true );

	// 버튼
	CsPoint ptCancelBtn;
	TCHAR	szBtnText[32];
	if( m_eStat == WRITE )
	{
		// 저장 버튼
		m_pOKBtn = NiNew cButton;
		m_pOKBtn->Init( m_pRoot, CsPoint( 20, 150 ), CsPoint( 60, 26 ) , cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_SAVE" ).c_str(), false );

		ptCancelBtn = CsPoint( 95, 150 );
		_stprintf_s( szBtnText, 32, _T( "%s" ), UISTRING_TEXT( "COMMON_TXT_CANCEL" ).c_str() );
	}
	else
	{
		ptCancelBtn = CsPoint( 60, 150 );
		_stprintf_s( szBtnText, 32, _T( "%s" ), UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
	}
	// 취소 or 확인 버튼
	m_pCancelBtn = NiNew cButton;
	m_pCancelBtn->Init( m_pRoot, ptCancelBtn, CsPoint( 60, 26 ) , cButton::IMAGE_NOR_1, szBtnText, false );

	// 구분선
	AddSprite( CsPoint( 15, 40 ), CsPoint( 150, 1 ), "Control_G\\Popup\\N2Dlg_Line.tga", true );
	AddSprite( CsPoint( 15, 140 ), CsPoint( 150, 1 ), "Control_G\\Popup\\N2Dlg_Line.tga", true );	
}

void cGuildMemo::Update(float const& fDeltaTime)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) == false )
		Close(false);

	if( m_pEditMemo && m_eStat == WRITE )
	{
		if( m_szMemoInput.compare( m_pEditMemo->GetTextAll() ) != 0 ) 
		{
			m_szMemoInput = m_pEditMemo->GetTextAll();

			m_strMemo.Delete( false );

			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			
			g_pStringAnalysis->Cut( &m_strMemo, 130, m_szMemoInput.c_str(), &ti );
		}		
	}

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cGuildMemo::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
	{
		return muReturn;
	}

	switch( m_pCancelBtn->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close(false);
		return muReturn;
	default:
		return muReturn;
	}

	if( m_eStat == WRITE )
	{
		switch( m_pOKBtn->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				// 클라 길드정보에 저장
				// 적힌 메모 서버로 쏘기
				if( m_szMemoInput.length() == 0 )
					m_szMemoInput = L" "; // 입력 문자가 없을 경우 공백으로 보내야 패킷을 보낼수 있어서 공백을 넣음.

				GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_GUILDMEMO, &m_szMemoInput );
				Close(false);
			}
			return muReturn;
		default:
			return muReturn;
		}
	}

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 420 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cGuildMemo::Render()
{
	for( int i=0; i<9; ++i )
		m_pSprite[ i ]->Render();

	RenderScript();

	m_strMemo.Render( CsPoint( 25, 60 ) + GetRootClient(), 1 );

	m_pCancelBtn->Render();

	if( m_eStat == WRITE )
		m_pOKBtn->Render();
}

void cGuildMemo::ResetDevice()
{
	m_strMemo.ResetDevice();

	if( m_eStat == WRITE )
	{
		if( m_pEditMemo )
			m_pEditMemo->ResetDevice();

		m_pOKBtn->ResetDevice();
	}
	for( int i=0; i<9; ++i )
		m_pSprite[ i ]->ResetDevice();

	m_pCancelBtn->ResetDevice();

	ResetDeviceScript();
}

void cGuildMemo::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();	
	m_strMemo.ResetDevice();

	if( m_eStat == WRITE )
	{
		if( m_pEditMemo )
			m_pEditMemo->ResetDevice();

		m_pOKBtn->ResetDevice();
	}
	for( int i=0; i<9; ++i )
		m_pSprite[ i ]->ResetDevice();

	m_pCancelBtn->ResetDevice();
}

void cGuildMemo::OnEditBox( TCHAR const* szMemoInput )
{
	SAFE_NIDELETE( m_pEditMemo );	

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );

 	ti.SetText( szMemoInput );
	m_pEditMemo = NiNew cEditBox;	
	m_pEditMemo->Init( m_pRoot, CsPoint::ZERO, CsPoint( 417, 23 ), &ti, false );	
	m_pEditMemo->SetFontLength( GUILD_MEMO_SIZE - 1 );
	m_pEditMemo->EnableUnderline();		
	m_pEditMemo->EnablePaste();

	if( g_pFocusEdit != NULL )
		g_pFocusEdit->ReleaseFocus();

	m_pEditMemo->SetFocus();

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value= 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

}

void cGuildMemo::OnRButtonUp( CsPoint pos )
{
	if( IsInWindow( pos ) == false )
		Close(false);
}

void cGuildMemo::OnLButtonUp( CsPoint pos )
{
	if( IsInWindow( pos ) == false )
		Close(false);
}

void cGuildMemo::OnMouseOutWindow()
{
	if( CURSOR_ST.GetButtonState() != CURSOR::RBUTTON_DOWN
		|| CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_DOWN )
		return;

	Close(false);
}

#endif