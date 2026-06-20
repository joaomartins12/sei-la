#include "stdafx.h"
#include "EquipSystemHelp.h"

cEquipSystemHelp::cEquipSystemHelp() :
m_pTabName(NULL), m_pCloseBtn(NULL), m_pScrollBar(NULL)
{

}

cEquipSystemHelp::~cEquipSystemHelp()
{

}

bool cEquipSystemHelp::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eOpenHelp, this );

	return true;
}

void cEquipSystemHelp::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eOpenHelp:
		{
			SystemType::eTabState eTab;
			kStream >> eTab;
			_MakeExplain( eTab );
		}
		break;
	}
}

cBaseWindow::eWINDOW_TYPE cEquipSystemHelp::GetWindowType()
{
	return WT_EQUIPMENT_SYSTEM_HELP;
}

void cEquipSystemHelp::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cEquipSystemHelp::DeleteResource()
{
	DeleteScript();
}

void cEquipSystemHelp::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	SetRootClient( CsPoint( (g_nScreenWidth - 418)/2, (g_nScreenHeight - 467)/2 ) );

	InitScript( "DigimonStatus_New\\Digimon_Status_help_Win.tga", m_ptRootClient, CsPoint( 418, 467 ), true );
	AddTitle( UISTRING_TEXT( "EQUIPSYSTEM_HELP_TITLE" ).c_str() );

	{	// 어느 탭 도움말인지
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( L"" );
		m_pTabName = AddText( &ti, CsPoint( 209, 34 ) );
	}

	m_pCloseBtn = AddButton( CsPoint( 388, 1 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );
	if( m_pCloseBtn )
		m_pCloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cEquipSystemHelp::_OnClickCloseButton );

	m_pScrollBar = AddScrollBar(cScrollBar::TYPE_1, CsPoint( 390, 65 ), CsPoint( 16, 390 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 8, 65 ), CsSIZE( 370, 390 ) ), 20 );
	m_pScrollBar->SetRange( CsPoint( 0, 20 ) );
}

void cEquipSystemHelp::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cEquipSystemHelp::Update_ForMouse()
{
	eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( MUT_OUT_WINDOW == muReturn )
		return muReturn;

	if( m_pCloseBtn && m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pScrollBar && m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 385, 25 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cEquipSystemHelp::Render()
{
	RenderScript();
	m_ExplainText.Render( GetTotalPosition() + CsPoint( 30, 85 ), 0 );
}

void cEquipSystemHelp::ResetDevice()
{
	ResetDeviceScript();
}

bool cEquipSystemHelp::OnEscapeKey()
{
	Close(false);
	return true;
}

void cEquipSystemHelp::_MakeExplain( SystemType::eTabState eTab )
{
	m_ExplainText.Delete();

	std::wstring wsTabName;
	std::wstring wsExplain;
	switch( eTab )
	{
	case SystemType::eTabState::eTabNormal:
		{
			wsTabName = UISTRING_TEXT( "EQUIPSYSTEM_TAB_NORMAL_CHARGE" );
			wsExplain = UISTRING_TEXT( "EQUIPSYSTEM_EXPLAIN_NORMAL_CHARGE" );
		}
		break;
	case SystemType::eTabState::eTabQuick:
		{
			wsTabName = UISTRING_TEXT( "EQUIPSYSTEM_TAB_QUICK_CHARGE" );
			wsExplain = UISTRING_TEXT( "EQUIPSYSTEM_EXPLAIN_QUICK_CHARGE" );
		}
		break;
	case SystemType::eTabState::eTabElem:
		{
			wsTabName = UISTRING_TEXT( "EQUIPSYSTEM_TAB_ELEM_MANAGE" );
			wsExplain = UISTRING_TEXT( "EQUIPSYSTEM_EXPLAIN_ELEM_MANAGE" );
		}
		break;
	}

	if( m_pTabName )
		m_pTabName->SetText( wsTabName.c_str() );

	cText::sTEXTINFO tiTemp;
	tiTemp.Init( CFont::FS_12 );
	g_pStringAnalysis->Cut_Parcing( &m_ExplainText, 330, wsExplain.c_str(), &tiTemp );

	if( m_pScrollBar )
	{
		m_ExplainText.SetBuddy( m_pScrollBar );
		m_pScrollBar->SetRange( CsPoint( 0, m_ExplainText.GetSize() ) );
	}
}

void cEquipSystemHelp::_OnClickCloseButton(void* pSender, void* pData)
{
	Close( false );
}
