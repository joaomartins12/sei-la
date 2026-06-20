#include "stdafx.h"
#include "EvolutionStore_PasswordUI.h"

cInputPasswordUI::cInputPasswordUI():m_pEditResistNumber(NULL),
m_pCloseBtn(NULL),m_pOkBtn(NULL)
{
	Construct();
}

cInputPasswordUI::~cInputPasswordUI()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cInputPasswordUI::SetInitData()
{
	if( m_pEditResistNumber )
		m_pEditResistNumber->SetFocus();
}

void cInputPasswordUI::ResetData()
{
	if( m_pEditResistNumber )
	{
		m_pEditResistNumber->ReleaseFocus();
		m_pEditResistNumber->SetText( L"" );
	}
}

//////////////////////////////////////////////////////////////////////////
void cInputPasswordUI::Init( cWindow* pRootWin )
{	
	InitScript(NULL, "Lobby\\Popup_Notification.tga", CsPoint( ( g_nScreenWidth - 482 ) / 2, 150 ), CsPoint( 482, 288 ), false );
	std::wstring title;
	std::wstring msg;
	switch( GLOBALDATA_ST.Get2ndPassType() )
	{
	case GData::eEmail:
		{
			title = UISTRING_TEXT( "CHARACTER_SELECT_EMAIL_INPUT_TITLE" );
			msg = UISTRING_TEXT( "CHARACTER_SELECT_EMAIL_MSG" );
		}
		break;
	case GData::eAccountPass:	
		{
			title = UISTRING_TEXT( "CHARACTER_SELECT_PASSWORD_INPUT_TITLE" );
			msg = UISTRING_TEXT( "CHARACTER_SELECT_PASSWORD_MSG" );
		}break;
	case GData::eStringPass:
		{
			title = UISTRING_TEXT( "CHARACTER_SELECT_WORD_INPUT_TITLE" );
			msg = UISTRING_TEXT( "CHARACTER_SELECT_WORD_MSG" );

			std::wstring deleteKey;
			DmCS::StringFn::From( deleteKey, DigimonSpritSteamSecondPass );
			DmCS::StringFn::ReplaceAll( msg, L"#Value#", deleteKey );

		}break;
	case GData::e2ndNumberPass:
		{
			title = UISTRING_TEXT( "CHARACTER_SELECT_2nd_PASSWORD_INPUT_TITLE" );
			msg = UISTRING_TEXT( "CHARACTER_SELECT_2nd_PASSWORD_MSG" );
		}
		break;
	}

	AddTitle( title.c_str(), CsPoint( 0, 20 ), CFont::FS_16, NiColor(1,0.17,0.17) );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( msg.c_str() );
	AddText( &ti, CsPoint(482/2, 90) );

	AddSprite( CsPoint((482 - 358)/2, 160 ), CsPoint( 358, 48 ), "Lobby\\CharacterSelect\\lobby_Password_glow.tga" );

	// PW
	m_pEditResistNumber = NiNew cEditBox;	
	if( m_pEditResistNumber )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText( _T( "" ) );
		ti.s_eTextAlign = DT_LEFT;
		m_pEditResistNumber->Init( m_pRoot, CsPoint( 80, 175 ), CsPoint( 355, 40 ), &ti, false );
		m_pEditResistNumber->SetFontLength( 64 );
		m_pEditResistNumber->EnableUnderline();
		m_pEditResistNumber->SetEnableSound( true );
		m_pEditResistNumber->AddEvent(cEditBox::eEditbox_ChangeText, this, &cInputPasswordUI::_CheckEditBoxText);
		AddChildControl( m_pEditResistNumber );

		switch( GLOBALDATA_ST.Get2ndPassType() )
		{
		case GData::eEmail:			break;
		case GData::eStringPass:	break;
		case GData::eAccountPass:	m_pEditResistNumber->GetText()->SetMark( L'*' );	break;
		case GData::e2ndNumberPass:	m_pEditResistNumber->GetText()->SetMark( L'*' );	break;
		}
	}


	m_pCloseBtn = AddButton( CsPoint( 482/2 - 150, 230), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Cancel.tga", cWindow::SD_Bu3 );
	if( m_pCloseBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText( UISTRING_TEXT("CHARACTER_SELECT_DELETE_CANCEL").c_str() );
		ti.s_eTextAlign = DT_CENTER;
		m_pCloseBtn->SetText( &ti );
		m_pCloseBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cInputPasswordUI::_OnClick_Close);
	}

	m_pOkBtn = AddButton( CsPoint( 482/2 + 13, 230), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_Ok.tga", cWindow::SD_Bu3 );
	if( m_pOkBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
		ti.s_eTextAlign = DT_CENTER;
		m_pOkBtn->SetText( &ti );
		m_pOkBtn->SetEnable( false );
		m_pOkBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cInputPasswordUI::_OnClick_OkBtn);
	}
}

void cInputPasswordUI::_CheckEditBoxText( void* pSender, void *pData )
{
	SAFE_POINTER_RET(pData);
	SAFE_POINTER_RET(m_pOkBtn);

	TCHAR* szResistNumber = static_cast<TCHAR*>(pData);
	m_pOkBtn->SetEnable( ( _tcslen( szResistNumber ) >= 4 ) );
}

void cInputPasswordUI::_OnClick_Close(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->ClosePasswordInput();
}

void cInputPasswordUI::_OnClick_OkBtn(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pEditResistNumber );
	cText* pText = m_pEditResistNumber->GetText();
	SAFE_POINTER_RET( pText );

	cText::sTEXTINFO* pTextInfo = pText->GetTextInfo();
	SAFE_POINTER_RET( pTextInfo );

	if( GetSystem() )
		GetSystem()->SendDigimonToItem( pTextInfo->GetText() );
}

bool cInputPasswordUI::UpdateMouse()
{
	if(!IsVisible())
		return false;

	if( m_pCloseBtn && m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return true;

	if( m_pOkBtn && m_pOkBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return true;

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool cInputPasswordUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eOpenPasswordInputUI, this );
	GetSystem()->Register( SystemType::eClosePasswordInputUI, this );
	return true;
}

void cInputPasswordUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eOpenPasswordInputUI:
		{
			SetInitData();
			SetVisible(true);
		}
		break;
	case SystemType::eClosePasswordInputUI:
		{
			ResetData();
			SetVisible(false);	
		}
		break;
	}
}