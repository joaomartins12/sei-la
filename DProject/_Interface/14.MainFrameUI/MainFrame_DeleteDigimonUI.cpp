#include "stdafx.h"
#include "MainFrame_TamerStatusUI.h"

cDeleteDigimonUI::cDeleteDigimonUI() :
m_pCheckDeleteDigimon(NULL),
m_pEditResistNumber(NULL),
m_pDeleteDigimonCloseBtn(NULL),
m_pDeleteDigimonOkBtn(NULL)
{
}

cDeleteDigimonUI::~cDeleteDigimonUI()
{

}

bool cDeleteDigimonUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void cDeleteDigimonUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}

cBaseWindow::eWINDOW_TYPE cDeleteDigimonUI::GetWindowType()
{
	return WT_DELETEDIGIMON;
}

void cDeleteDigimonUI::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cDeleteDigimonUI::DeleteResource()
{
	DeleteScript();
	g_IME.SetNumberOnly( false );
}

void cDeleteDigimonUI::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	SetRootClient( CsPoint( g_nScreenWidth / 2 - 172, g_nScreenHeight / 2 - 78 ) );

	int nFontLen = 0;
	switch( GLOBALDATA_ST.Get2ndPassType() )
	{
	case GData::eEmail:
		nFontLen = 64;
		break;
	case GData::eAccountPass:
	case GData::eStringPass:
	case GData::e2ndNumberPass:
		nFontLen = 20;
		break;
	}
	InitScript( NULL, "CharSelect\\DeleteChar.tga", m_ptRootClient, CsPoint( 383, 159 ), false );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( _T( "" ) );

	// PW
	m_pEditResistNumber = NiNew cEditBox;
	if( m_pEditResistNumber )
	{
		m_pEditResistNumber->Init( m_pRoot, CsPoint( 55, 98 ), CsPoint( 280, 20 ), &ti, false );
		m_pEditResistNumber->SetFontLength( nFontLen );
		m_pEditResistNumber->SetEnableSound( true );
		m_pEditResistNumber->SetFocus();
		m_pEditResistNumber->AddEvent( cEditBox::eEditbox_ChangeText, this, &cDeleteDigimonUI::_ChangeEditText );
		AddChildControl( m_pEditResistNumber );
	}

	m_pDeleteDigimonCloseBtn = AddButton( CsPoint( 361, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );

	m_pDeleteDigimonOkBtn = AddButton( CsPoint( 150, 125 ), CsPoint( 84, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
	if( m_pDeleteDigimonOkBtn )
	{
		m_pDeleteDigimonOkBtn->SetEnable( false );
		m_pDeleteDigimonOkBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDeleteDigimonUI::_OkDeleteDigimon );
	}

	switch( GLOBALDATA_ST.Get2ndPassType() )
	{
	case GData::eEmail:			g_IME.SetNumberOnly( false );	break;
	case GData::eStringPass:	g_IME.SetNumberOnly( false );	break;
	case GData::eAccountPass:	g_IME.SetNumberOnly( false );	m_pEditResistNumber->GetText()->SetMark( L'*' );	break;
#ifdef SDM_SECONDPASSWORD_REINFORCE_20180330
	case GData::e2ndNumberPass:	g_IME.SetNumberOnly( false );	m_pEditResistNumber->GetText()->SetMark( L'*' );	break;
#else
	case GData::e2ndNumberPass:	g_IME.SetNumberOnly( true );	m_pEditResistNumber->GetText()->SetMark( L'*' );	break;
#endif
	}

	if( g_pGameIF )
	{
		ChatMainWindow* pChatMain = g_pGameIF->GetChatMain();
		SAFE_POINTER_RET( pChatMain );
		pChatMain->OnEscapeKey();
	}

	std::wstring title;
	std::wstring msg;
	if( GLOBALDATA_ST.Is2ndPassType(GData::eEmail) )
	{
		title = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_EMAIL_INPUT_TITLE" );
		msg = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_EMAIL_INPUT_MSG" );
	}
	else if( GLOBALDATA_ST.Is2ndPassType(GData::eStringPass) )
	{
		title = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_WORD_INPUT_TITLE" );
		msg = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_WORD_INPUT_MSG" );

		std::wstring deleteKey;
		DmCS::StringFn::From( deleteKey, DeleteDigimonSteamSecondPass );
		DmCS::StringFn::ReplaceAll( msg, L"#Value#", deleteKey );
	}
	else if( GLOBALDATA_ST.Is2ndPassType( GData::e2ndNumberPass ) )
	{
		title = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_2nd_PASSWORD_INPUT_TITLE" );
		msg = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_2nd_PASSWORD_INPUT_MSG" );
	} 
	else if( GLOBALDATA_ST.Is2ndPassType( GData::eAccountPass ) )
	{
		title = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_PASSWORD_INPUT_TITLE" );
		msg = UISTRING_TEXT( "TAMERDIGIVICE_DIGIMON_DELETE_PASSWORD_INPUT_TITLE" );
	}

	{
		cText::sTEXTINFO windowTitle;
		windowTitle.Init( CFont::FS_10, NiColor::WHITE );
		windowTitle.s_eTextAlign = DT_CENTER;
		windowTitle.SetText( title.c_str() );
		AddText( &windowTitle, CsPoint( 383/2, 7 ) );
	}

	{
		cText::sTEXTINFO windowMsg;
		windowMsg.Init( CFont::FS_10, NiColor::WHITE );
		windowMsg.s_eTextAlign = DT_CENTER;
		windowMsg.SetText( msg.c_str() );
		AddText( &windowMsg, CsPoint( 383/2, 55 ) );
	}
}

void cDeleteDigimonUI::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );
}

cBaseWindow::eMU_TYPE cDeleteDigimonUI::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( cBaseWindow::MUT_OUT_WINDOW == muReturn )
		return muReturn;

	if( m_pDeleteDigimonCloseBtn && m_pDeleteDigimonCloseBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		Close( false );
		return muReturn;
	}

	if( m_pDeleteDigimonOkBtn && m_pDeleteDigimonOkBtn->Update_ForMouse() )
		return muReturn;

	return muReturn;
}

void cDeleteDigimonUI::Render()
{
	RenderScript();
}

void cDeleteDigimonUI::ResetMap()
{
	Close( false );
}

void cDeleteDigimonUI::ResetDevice()
{
	ResetDeviceScript();
}

bool cDeleteDigimonUI::OnMacroKey(const MSG& p_kMsg)
{
	switch(p_kMsg.message)
	{
	case WM_KEYUP:
		{
			switch(p_kMsg.wParam)
			{
			case VK_RETURN:
				{
					if( m_pDeleteDigimonOkBtn && !m_pDeleteDigimonOkBtn->IsEnable() )
						return false;

					if( m_pEditResistNumber )
					{
						GetSystem()->DeleteDigimon( m_pEditResistNumber->GetTextAll() );
						return true;
					}
				}
				break;	
			case VK_ESCAPE:
				{
					Close( false );
					return true;
				}
				break;
			}
		}
		break;
	}

	return false;
}

void cDeleteDigimonUI::_ChangeEditText(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	wchar_t* pPass = static_cast< wchar_t* >( pData );

	int nPassSize = wcslen( pPass );
	if( 4 <= nPassSize )
		m_pDeleteDigimonOkBtn->SetEnable( true );
	else
		m_pDeleteDigimonOkBtn->SetEnable( false );
}

void cDeleteDigimonUI::_OkDeleteDigimon(void* pSender, void* pData)
{
	if( m_pEditResistNumber )
		GetSystem()->DeleteDigimon( m_pEditResistNumber->GetTextAll() );
}

