#include "stdafx.h"
#include "MainFrame_DigimonStatusHelpUI.h"


#define IF_EXPLAIN_WIDTH 350

cDigimonStatusUI_Help::cDigimonStatusUI_Help() :
m_pHelpTab(NULL),
m_pCloseButton(NULL),
m_pNormalInfo(NULL),
m_pNatureInfo(NULL),
m_pAttackInfo(NULL),
m_pFieldInfo(NULL)
{

}

cDigimonStatusUI_Help::~cDigimonStatusUI_Help()
{

}

bool cDigimonStatusUI_Help::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void cDigimonStatusUI_Help::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}

cBaseWindow::eWINDOW_TYPE cDigimonStatusUI_Help::GetWindowType()
{
	return WT_DIGIMONSTATUS_HELP;
}

void cDigimonStatusUI_Help::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cDigimonStatusUI_Help::DeleteResource()
{
	DeleteScript();
}

void cDigimonStatusUI_Help::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	SetRootClient( CsPoint( g_nScreenWidth / 2 - 209, g_nScreenHeight / 2 - 233 ) );

	InitScript( "DigimonStatus_New\\Digimon_Status_help_Win.tga", m_ptRootClient, CsPoint( 418, 467 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );
	AddTitle( UISTRING_TEXT( "DIGIMONSTATUS_HELP_TITLE_TEXT" ).c_str() );

	m_pCloseButton = AddButton( CsPoint( 388, 1 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );
	if( m_pCloseButton )
		m_pCloseButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDigimonStatusUI_Help::_OnClickCloseButton );

	_MakeNormalInfo();
	_MakeNatureInfo();
	_MakeAttackInfo();
	_MakeFieldInfo();

	_MakeHelpTab();
}

void cDigimonStatusUI_Help::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cDigimonStatusUI_Help::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( MUT_OUT_WINDOW == muReturn )
		return muReturn;

	if( m_pHelpTab && m_pHelpTab->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;

	if( m_pCloseButton && m_pCloseButton->Update_ForMouse() )
		return muReturn;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 385, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cDigimonStatusUI_Help::Render()
{
	RenderScript();

	m_NormalExpl.Render( GetTotalPosition() + CsPoint( 35, 320 ), 0 );
	m_NatureExpl.Render( GetTotalPosition() + CsPoint( 35, 337 ), 0 );
	m_AttackExpl.Render( GetTotalPosition() + CsPoint( 35, 300 ), 0 );
	m_FieldExpl.Render( GetTotalPosition() + CsPoint( 35, 337 ), 0 );
}

void cDigimonStatusUI_Help::ResetDevice()
{
	ResetDeviceScript();
}

bool cDigimonStatusUI_Help::OnEscapeKey()
{
	Close( false );
	return true;
}

void cDigimonStatusUI_Help::_MakeHelpTab()
{
	CsPoint pos = CsPoint( 5, 32 );
	m_pHelpTab = AddRadioButton( pos, CsPoint( 102, 24 ), CsPoint( 0, 24 ), "DigimonStatus_New\\Digimon_Status_help_btn.tga" );
	if( m_pHelpTab )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_HELP_TAB_NORMAL" ).c_str() );
		m_pHelpTab->SetCheckBoxText( &ti );
		m_pHelpTab->SetUserData( new sTabInfo( eNormal ) );
		m_pHelpTab->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &cDigimonStatusUI_Help::_OnClickTabButton );

		pos.x += 102;
		_AddHelpTab( pos, UISTRING_TEXT( "DIGIMONSTATUS_HELP_TAB_NATURE" ), eNature );
		pos.x += 102;
		_AddHelpTab( pos, UISTRING_TEXT( "DIGIMONSTATUS_HELP_TAB_ATTACK" ), eAttack );
		pos.x += 102;
		_AddHelpTab( pos, UISTRING_TEXT( "DIGIMONSTATUS_HELP_TAB_FIELD" ), eField );

		m_pHelpTab->SetCheckIndex( eNormal );

		_SetCurrentTabInfo( eNormal );
	}
}

void cDigimonStatusUI_Help::_AddHelpTab(CsPoint ptPos, std::wstring wsTabName, eTab eType)
{
	cRadioButton* pAddBtn = m_pHelpTab->AddNode( ptPos, CsPoint( 102, 24 ), CsPoint( 0, 24 ), "DigimonStatus_New\\Digimon_Status_help_btn.tga" );
	if( pAddBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( wsTabName.c_str() );
		pAddBtn->SetCheckBoxText( &ti );
		pAddBtn->SetEnable( true );
		pAddBtn->SetUserData( new sTabInfo( eType ) );
	}
}

void cDigimonStatusUI_Help::_MakeNormalInfo()
{
	m_pNormalInfo = NiNew cScriptUI;
	SAFE_POINTER_RET( m_pNormalInfo );
	m_pNormalInfo->InitScript( m_pRoot, NULL, CsPoint::ZERO, CsPoint( 418, 467 ), false );

	m_pNormalInfo->AddSprite( CsPoint( 15, 70 ), CsPoint( 68, 12 ), "DigimonStatus_New\\Digimon_Status_deco.tga" );
	m_pNormalInfo->AddSprite( CsPoint( 47, 120 ), CsPoint( 324, 154 ), "DigimonStatus_New\\Digimon_Status_help_01.tga" );

	{
		// 기본 속성 하단 문구
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		m_NormalExpl.Delete();
		g_pStringAnalysis->Cut_Parcing( &m_NormalExpl, IF_EXPLAIN_WIDTH, UISTRING_TEXT( "DIGIMONSTATUS_HELP_NORMAL_EXPLAIN" ).c_str(), &ti );
	}

	AddChildScriptUI( m_pNormalInfo );
}

void cDigimonStatusUI_Help::_MakeNatureInfo()
{
	m_pNatureInfo = NiNew cScriptUI;
	SAFE_POINTER_RET( m_pNatureInfo );
	m_pNatureInfo->InitScript( m_pRoot, NULL, CsPoint::ZERO, CsPoint( 418, 467 ), false );

	m_pNatureInfo->AddSprite( CsPoint( 15, 70 ), CsPoint( 68, 12 ), "DigimonStatus_New\\Digimon_Status_deco.tga" );
	m_pNatureInfo->AddSprite( CsPoint( 26, 90 ), CsPoint( 366, 230 ), "DigimonStatus_New\\Digimon_Status_help_02.tga" );

	{
		// 자연 속성 하단 문구
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		m_NatureExpl.Delete();
		g_pStringAnalysis->Cut_Parcing( &m_NatureExpl, IF_EXPLAIN_WIDTH, UISTRING_TEXT( "DIGIMONSTATUS_HELP_NATURE_EXPLAIN" ).c_str(), &ti );
	}

	AddChildScriptUI( m_pNatureInfo );
}

void cDigimonStatusUI_Help::_MakeAttackInfo()
{
	m_pAttackInfo = NiNew cScriptUI;
	SAFE_POINTER_RET( m_pAttackInfo );
	m_pAttackInfo->InitScript( m_pRoot, NULL, CsPoint::ZERO, CsPoint( 418, 467 ), false );

	m_pAttackInfo->AddSprite( CsPoint( 18, 153 ), CsPoint( 382, 54 ), "DigimonStatus_New\\Digimon_Status_help_03.tga" );

	{
		// SA
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 113.f/255.f, 210.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_HELP_SA" ).c_str() );
		m_pAttackInfo->AddText( &ti, CsPoint( 62, 228 ) );
	}
	{
		// NA
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 113.f/255.f, 210.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_HELP_NA" ).c_str() );
		m_pAttackInfo->AddText( &ti, CsPoint( 160, 228 ) );
	}
	{
		// QA
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 113.f/255.f, 210.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_HELP_QA" ).c_str() );
		m_pAttackInfo->AddText( &ti, CsPoint( 258, 228 ) );
	}
	{
		// DE
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 113.f/255.f, 210.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_HELP_DE" ).c_str() );
		m_pAttackInfo->AddText( &ti, CsPoint( 356, 228 ) );
	}
	{
		// 공격 타입 하단 문구
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		m_AttackExpl.Delete();
		g_pStringAnalysis->Cut_Parcing( &m_AttackExpl, IF_EXPLAIN_WIDTH, UISTRING_TEXT( "DIGIMONSTATUS_HELP_ATTACK_EXPLAIN" ).c_str(), &ti );
	}

	AddChildScriptUI( m_pAttackInfo );
}

void cDigimonStatusUI_Help::_MakeFieldInfo()
{
	m_pFieldInfo = NiNew cScriptUI;
	SAFE_POINTER_RET( m_pFieldInfo );
	m_pFieldInfo->InitScript( m_pRoot, NULL, CsPoint::ZERO, CsPoint( 418, 467 ), false );

	m_pFieldInfo->AddSprite( CsPoint( 29, 130 ), CsPoint( 360, 130 ), "DigimonStatus_New\\Digimon_Status_help_04.tga" );

	{
		// 필드 하단 문구
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		m_FieldExpl.Delete();
		g_pStringAnalysis->Cut_Parcing( &m_FieldExpl, IF_EXPLAIN_WIDTH, UISTRING_TEXT( "DIGIMONSTATUS_HELP_FIELD_EXPLAIN" ).c_str(), &ti );
	}

	AddChildScriptUI( m_pFieldInfo );
}

void cDigimonStatusUI_Help::_SetCurrentTabInfo(eTab eType)
{
	if( m_pNormalInfo )
		m_pNormalInfo->SetVisible( false );
	if( m_pNatureInfo )
		m_pNatureInfo->SetVisible( false );
	if( m_pAttackInfo )
		m_pAttackInfo->SetVisible( false );
	if( m_pFieldInfo )
		m_pFieldInfo->SetVisible( false );

	m_NormalExpl.SetVisible( false );
	m_NatureExpl.SetVisible( false );
	m_AttackExpl.SetVisible( false );
	m_FieldExpl.SetVisible( false );

	switch( eType )
	{
	case eNormal:
		{
			if( m_pNormalInfo )
				m_pNormalInfo->SetVisible( true );
			m_NormalExpl.SetVisible( true );
		}
		break;
	case eNature:
		{
			if( m_pNatureInfo )
				m_pNatureInfo->SetVisible( true );
			m_NatureExpl.SetVisible( true );
		}
		break;
	case eAttack:
		{
			if( m_pAttackInfo )
				m_pAttackInfo->SetVisible( true );
			m_AttackExpl.SetVisible( true );
		}
		break;
	case eField:
		{
			if( m_pFieldInfo )
				m_pFieldInfo->SetVisible( true );
			m_FieldExpl.SetVisible( true );
		}
		break;
	}
}

void cDigimonStatusUI_Help::_OnClickCloseButton(void* pSender, void* pData)
{
	Close( false );
}

void cDigimonStatusUI_Help::_OnClickTabButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cRadioButton* pSelect = static_cast< cRadioButton* >( pData );
	sTabInfo* pTabInfo = dynamic_cast< sTabInfo* >( pSelect->GetUserData() );
	SAFE_POINTER_RET( pTabInfo );

	_SetCurrentTabInfo( pTabInfo->eTabType );

	if( m_pHelpTab )
		m_pHelpTab->SetCheckIndex( pTabInfo->eTabType );
}
