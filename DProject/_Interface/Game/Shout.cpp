
#include "stdafx.h"
#include "Shout.h"

void cShout::Destroy()
{
	cBaseWindow::Delete();	
}

void cShout::DeleteResource()
{
	SAFE_NIDELETE( m_pEdit );
	DeleteScript();		

#ifdef IME_WINDOW_POSITION
	g_bIMEPosition = false;
#endif
}

void cShout::Create(int nValue /* = 0  */)
{
#ifdef GUILD_RENEWAL
	m_nShoutItemSlot = nValue;			// 인벤 슬롯

	int nType;		// 확성기 종류

	if( m_nShoutItemSlot == -1 )		// 길드스킬
	{
		nType = nItem::Cash_Shouter_S;	// 핑크 확성기
	}
	else
	{
		m_dwItemID = g_pDataMng->GetInven()->GetData( m_nShoutItemSlot )->GetType();
		nType = nsCsFileTable::g_pItemMng->GetItem( m_dwItemID )->GetInfo()->s_nType_L;

		if( m_nShoutItemSlot == cData_Inven::INVALIDE_INVEN_INDEX )
			return;
	}
#else
	m_nShoutItemSlot = nValue;
	m_dwItemID = g_pDataMng->GetInven()->GetData( m_nShoutItemSlot )->GetType();
	int nType = nsCsFileTable::g_pItemMng->GetItem( m_dwItemID )->GetInfo()->s_nType_L;

	if( m_nShoutItemSlot == cData_Inven::INVALIDE_INVEN_INDEX )
		return;
#endif	// GUILD_RENEWAL
	SetRootClient( CsPoint( g_nScreenWidth / 2 - 176, g_nScreenHeight / 2 - 37 ) );		

	cBaseWindow::Init();

	CsPoint	ptIconPos,	ptIconSize;	//확성기 아이콘 위치, 크기
	CsPoint	ptEditPos,	ptEditSize;	//입력창 위치, 크기
	int			nEditLength;				//입력창 최대 입력 수

	InitScript( "Shout\\Win.tga", m_ptRootClient, CsPoint( 357, 74 ), false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );		
	AddTitle( UISTRING_TEXT( "SHOUT_TITLE_MEGAPHONE" ).c_str() );

	m_pCancelButton = AddButton( CsPoint( 325, 6 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pSendButton = AddButton( CsPoint( 298, 32 ), CsPoint( 48, 33 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_SHOUT" ).c_str() );

	ptIconPos = CsPoint( 14, 43 );//확성기 아이콘 위치
	ptIconSize = CsPoint( 16, 16 );//확성기 아이콘 크기
	ptEditPos = CsPoint( 35, 42 );//입력창 위치
	ptEditSize = CsPoint( 255, 16 );//입력창 크기
	nEditLength = 64;//입력창 최대 입력 수

	switch( nType )
	{
	case nItem::Cash_Shouter_N: 
		AddSprite( ptIconPos, ptIconSize, "Chat\\Ye_Icon.tga" );
		break;
	case nItem::Cash_Shouter_S:
		AddSprite( ptIconPos, ptIconSize, "Chat\\Re_Icon.tga" );
		break;
	case nItem::Cash_Shouter_NT:
		AddSprite( ptIconPos, ptIconSize, "Chat\\Gr_Icon.tga" );
		break;
	case nItem::Cash_Shouter_ST:
		AddSprite( ptIconPos, ptIconSize, "Chat\\Bl_Icon.tga" );
		break;
	case nItem::GM_Shouter_T:
		AddSprite( ptIconPos, ptIconSize, "Chat\\Wh_Icon.tga" );
		break;
	}

	cText::sTEXTINFO tiEdit;
	tiEdit.Init();			

	m_pEdit = NiNew cEditBox;
	m_pEdit->Init( GetRoot(), ptEditPos, ptEditSize, &tiEdit, false );
	m_pEdit->EnableUnderline();
	m_pEdit->EnablePaste();
	m_pEdit->SetFontLength( nEditLength );
	m_pEdit->SetFocus();
	g_IME.Native_On( GAMEAPP_ST.GetHWnd() );	

#ifdef IME_WINDOW_POSITION
	g_bIMEPosition = true;
#endif

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value= 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

bool cShout::Close( bool bSound /* = true  */ )
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound )
	{
		PlayCloseSound();
	}

	DeleteResource();
	SetShowWindow( false );

	return true;
}

void cShout::Update(float const& fDeltaTime)
{	
	_UpdateMoveWindow();

	if( m_pEdit->IsFocus() == false )
		Close( false );
}

cBaseWindow::eMU_TYPE
cShout::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;
	default:
		return muReturn;
	}	

	switch( m_pSendButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:		
		{
			auto r = nBase::rtrim((TCHAR*)m_pEdit->GetTextAll());
			if( _tcslen( r.c_str() ) > 0 )
			{
#ifdef GUILD_RENEWAL
				if( net::game && m_nShoutItemSlot == -1 )	// 길드 확성기 스킬이면
				{
					net::game->SendUseGuildSkill( GUILD_SHOUT, (TCHAR*)r.c_str());
				}
				else
#endif
				if( net::game )
					net::game->SendMsgAllByItem( (wchar*) r.c_str(), m_nShoutItemSlot);

				Close( false );
			}
		}
		return muReturn;
	default:
		return muReturn;
	}

	if( m_pEdit )
		m_pEdit->Update_ForMouse( CsPoint::ZERO );

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 266, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
	return muReturn;
}

void cShout::Render()
{
	RenderScript();	

#ifdef IME_WINDOW_POSITION
	if( m_pEdit != NULL )
	{
		CsPoint TempPos = m_pEdit->GetDefaultPos();
		CsPoint TempStringPos = m_pEdit->GetText()->GetStringSize();
		g_nIMEWidth = GetRootClient().x + TempPos.x + TempStringPos.x;
		g_nIMEHeight = GetRootClient().y + TempPos.y + TempStringPos.y;
	}
#endif

	if( m_pEdit )
		m_pEdit->Render();	
}

void cShout::OnMouseOutWindow()
{
	cBaseWindow::OnMouseOutWindow();

	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
		return;

	if( CURSOR_ST.IsIcon() == true )
		return;	
}

bool cShout::OnMacroKey(const MSG& p_kMsg)
{
	// GlobalInput에서 KEYUP일 때 OnEnterKey를 호출해주지 않아서 OnMacroKey로 받음
	switch(p_kMsg.message)
	{
	case WM_KEYUP:
		{
			switch(p_kMsg.wParam)
			{
			case VK_RETURN:
				{
					OnEnterKey();
					return true;
				}
				break;			
			}
		}
		break;
	}

	return false;
}

bool cShout::OnEnterKey()
{
	if( m_pEdit )
	{	
		auto r = nBase::rtrim((TCHAR*)m_pEdit->GetTextAll());
		if( _tcslen( r.c_str() ) > 0 )	
		{
#ifdef GUILD_RENEWAL
			if( net::game && m_nShoutItemSlot == -1 )	// 길드 확성기 스킬이면
			{
				net::game->SendUseGuildSkill( GUILD_SHOUT, (TCHAR*)r.c_str());
			}
			else
#endif
			if( net::game )
				net::game->SendMsgAllByItem( (TCHAR*)r.c_str(), m_nShoutItemSlot );

			Close( false );			
		}
		
		return true;	
	}

	return false;
}

void cShout::ResetDevice()
{	
	ResetDeviceScript();

	if( m_pEdit )
		m_pEdit->ResetDevice();	
}