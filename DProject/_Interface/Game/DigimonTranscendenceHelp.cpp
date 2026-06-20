#include "stdafx.h"
#include "DigimonTranscendenceHelp.h"

cDigimonTranscendenceHelp::cDigimonTranscendenceHelp() :
m_pCloseBtn(NULL), m_pTabBtn(NULL), m_pScrollBar(NULL)
{

}

cDigimonTranscendenceHelp::~cDigimonTranscendenceHelp()
{

}

bool cDigimonTranscendenceHelp::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void cDigimonTranscendenceHelp::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}

cBaseWindow::eWINDOW_TYPE cDigimonTranscendenceHelp::GetWindowType()
{
	return WT_TRANSCENDENCE_HELP;
}

void cDigimonTranscendenceHelp::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cDigimonTranscendenceHelp::DeleteResource()
{
	DeleteScript();
}

void cDigimonTranscendenceHelp::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	SetRootClient( CsPoint( (g_nScreenWidth - 418)/2, (g_nScreenHeight - 467)/2 ) );

	InitScript( "DigimonStatus_New\\Digimon_Status_help_Win.tga", m_ptRootClient, CsPoint( 418, 467 ), true );
	AddTitle( UISTRING_TEXT( "DIGIMONTRANSCENDHELP_TITLE" ).c_str() );

	m_pCloseBtn = AddButton( CsPoint( 388, 1 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );
	if( m_pCloseBtn )
		m_pCloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDigimonTranscendenceHelp::_OnClickCloseButton );

	_MakeTab();

	m_pScrollBar = AddScrollBar(cScrollBar::TYPE_1, CsPoint( 390, 65 ), CsPoint( 16, 390 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 8, 65 ), CsSIZE( 370, 390 ) ), 20 );
	m_pScrollBar->SetRange( CsPoint( 0, 20 ) );
	_MakeExplain( eSystemTab );
}

void cDigimonTranscendenceHelp::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cDigimonTranscendenceHelp::Update_ForMouse()
{
	eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( MUT_OUT_WINDOW == muReturn )
		return muReturn;

	if( m_pCloseBtn && m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pTabBtn && m_pTabBtn->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;

	if( m_pScrollBar && m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 806, 40 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cDigimonTranscendenceHelp::Render()
{
	RenderScript();
	m_ExplainText.Render( GetTotalPosition() + CsPoint( 30, 85 ), 0 );
}

void cDigimonTranscendenceHelp::ResetDevice()
{
	ResetDeviceScript();
}

bool cDigimonTranscendenceHelp::OnEscapeKey()
{
	Close(false);
	return true;
}

void cDigimonTranscendenceHelp::_MakeTab()
{
	CsPoint pos = CsPoint( 5, 32 );
	CsPoint size = CsPoint( 102, 24 );
	CsPoint token = CsPoint( 0, 24 );
	m_pTabBtn = AddRadioButton( pos, size, token, "DigimonStatus_New\\Digimon_Status_help_btn.tga" );
	if( m_pTabBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DIGIMONTRANSCENDHELP_EXPLAIN_TAB_SYSTEM" ).c_str() );
		m_pTabBtn->SetCheckBoxText( &ti );
		m_pTabBtn->SetUserData( new sTabInfo( eSystemTab ) );
		m_pTabBtn->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &cDigimonTranscendenceHelp::_OnClickTabButton );

		pos.x += 102;
		_AddTab( pos, size, token, UISTRING_TEXT( "DIGIMONTRANSCENDHELP_EXPLAIN_TAB_TERMS" ), eTermsTab );
		pos.x += 102;
		_AddTab( pos, size, token, UISTRING_TEXT( "DIGIMONTRANSCENDHELP_EXPLAIN_TAB_PROGRESS" ), eProgressTab );
		pos.x += 102;
		_AddTab( pos, size, token, UISTRING_TEXT( "DIGIMONTRANSCENDHELP_EXPLAIN_TAB_CHARGE" ), eChargeTab );

		m_pTabBtn->SetCheckIndex( eSystemTab );
	}
}

void cDigimonTranscendenceHelp::_AddTab(CsPoint pos, CsPoint size, CsPoint token, std::wstring wsTabName, eTabType eType)
{
	SAFE_POINTER_RET( m_pTabBtn );
	cRadioButton* pAddNode = m_pTabBtn->AddNode( pos, size, token, "DigimonStatus_New\\Digimon_Status_help_btn.tga" );
	if( pAddNode )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( wsTabName.c_str() );
		pAddNode->SetCheckBoxText( &ti );
		pAddNode->SetEnable( true );
		pAddNode->SetUserData( new sTabInfo( eType ) );
	}
}

void cDigimonTranscendenceHelp::_MakeExplain( eTabType eType )
{
	m_ExplainText.Delete();

	std::wstring wsExplain;
	switch( eType )
	{
	case eSystemTab:
		wsExplain = UISTRING_TEXT( "DIGIMONTRANSCENDHELP_EXPLAIN_SYSTEM" );
		break;
	case eTermsTab:
		wsExplain = UISTRING_TEXT( "DIGIMONTRANSCENDHELP_EXPLAIN_TERMS" );
		break;
	case eProgressTab:
		wsExplain = UISTRING_TEXT( "DIGIMONTRANSCENDHELP_EXPLAIN_PROGRESS" );
		break;
	case eChargeTab:
		wsExplain = UISTRING_TEXT( "DIGIMONTRANSCENDHELP_EXPLAIN_CHARGE" );
		break;
	}

	cText::sTEXTINFO tiTemp;
	tiTemp.Init( CFont::FS_12 );
	g_pStringAnalysis->Cut_Parcing( &m_ExplainText, 350, wsExplain.c_str(), &tiTemp );

	if( m_pScrollBar )
	{
		m_ExplainText.SetBuddy( m_pScrollBar );
		m_pScrollBar->SetRange( CsPoint( 0, m_ExplainText.GetSize() ) );
	}
}

void cDigimonTranscendenceHelp::_OnClickTabButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cRadioButton* pSelect = static_cast< cRadioButton* >( pData );
	sTabInfo* pTabInfo = dynamic_cast< sTabInfo* >( pSelect->GetUserData() );
	SAFE_POINTER_RET( pTabInfo );

	_MakeExplain( pTabInfo->eType );

	if( m_pTabBtn )
		m_pTabBtn->SetCheckIndex( pTabInfo->eType );
}

void cDigimonTranscendenceHelp::_OnClickCloseButton(void* pSender, void* pData)
{
	Close( false );
}
