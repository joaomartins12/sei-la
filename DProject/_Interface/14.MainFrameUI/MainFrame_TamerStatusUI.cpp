#include "stdafx.h"
#include "MainFrame_TamerStatusUI.h"

#include "TamerStatusUI_Tamer.h"
#include "TamerStatusUI_Digivice.h"
#include "TamerStatusUI_Skill.h"
#include "TamerStatusUI_Cross.h"

#define IF_TAB_TAMER	0
#define IF_TAB_DIGIVICE	1
#define IF_TAB_SKILL	2
#define IF_TAB_CROSS	3

cTamerStatusUI::cTamerStatusUI()
: m_pStatusTab(NULL),
m_pCancelBtn(NULL),
m_pCurViewer(NULL),
m_pTamerViewer(NULL),
m_pDigiviceViewer(NULL),
m_pSkillViewer(NULL)
#ifdef CROSSWARS_SYSTEM
,m_pCrossViewer(NULL)
#endif
#ifdef USE_BARCODE_REDER
,m_pBarcodeBtn(NULL)
#endif
{

}

cTamerStatusUI::~cTamerStatusUI()
{

}

bool cTamerStatusUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eSetTab, this );
	GetSystem()->Register( SystemType::eUpdateStatus, this );
#ifdef USE_BARCODE_REDER
	GetSystem()->Register( SystemType::eUseScanner, this );
#endif

	return true;
}

void cTamerStatusUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eSetTab:
		{
			int nTabNo = 0;
			kStream >> nTabNo;
			_SetStatusTab( nTabNo );
		}
		break;

	case SystemType::eUpdateStatus:
		{
			if( m_pCurViewer )
				m_pCurViewer->UpdateViewer();
		}
		break;
#ifdef USE_BARCODE_REDER
	case SystemType::eUseScanner:
		{
			bool bUseScanner = false;
			kStream >> bUseScanner;
			if( bUseScanner )
				_InitBarcodeState();
			else
				_DeleteBarcode();
		}
		break;
#endif
	}
}

cBaseWindow::eWINDOW_TYPE cTamerStatusUI::GetWindowType()
{
	return WT_NEW_TAMERSTATUS;
}

void cTamerStatusUI::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cTamerStatusUI::DeleteResource()
{
	DeleteScript();
#ifdef USE_BARCODE_REDER
	m_BarcodeState = 0;
#endif
}

void cTamerStatusUI::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	CsPoint ptSize = CsPoint( 406, 676 );

	SetRootClient( CsPoint( 100, 100 ) );
	InitScript( NULL, m_ptRootClient, CsPoint( 406, 676 + 37 ), true );
	AddTitle( UISTRING_TEXT( "TAMERSTATUS_TITLE_TEXT" ).c_str(), CsPoint( 0, 10 ) );

	m_pBgImg = AddSprite( CsPoint::ZERO, ptSize, "TamerStatus_New\\TamerStatus_Win_01.tga" );

	m_pTamerViewer = NiNew cTamerStatusUI_Tamer;
	if( m_pTamerViewer )
	{
		m_pTamerViewer->Create( m_pRoot );
		m_pTamerViewer->SetVisible( false );
		AddChildScriptUI( m_pTamerViewer );
	}

	m_pDigiviceViewer = NiNew cTamerStatusUI_Digivice;
	if( m_pDigiviceViewer )
	{
		m_pDigiviceViewer->Create( m_pRoot );
		m_pDigiviceViewer->SetVisible( false );
		AddChildScriptUI( m_pDigiviceViewer );
	}

	m_pSkillViewer = NiNew cTamerStatusUI_Skill;
	if( m_pSkillViewer )
	{
		m_pSkillViewer->Create( m_pRoot );
		m_pSkillViewer->SetVisible( false );
		AddChildScriptUI( m_pSkillViewer );
	}

#ifdef CROSSWARS_SYSTEM
	m_pCrossViewer = NiNew cTamerStatusUI_Cross;
	if( m_pCrossViewer )
	{
		m_pCrossViewer->Create( m_pRoot );
		m_pCrossViewer->SetVisible( false );
		AddChildScriptUI( m_pCrossViewer );
	}
#endif

	m_pCurViewer = m_pTamerViewer;
	if( m_pCurViewer )
		m_pCurViewer->SetVisible( true );

	m_pCancelBtn = AddButton( CsPoint( 376, 11 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );
	if( m_pCancelBtn )
		m_pCancelBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cTamerStatusUI::_OnClickCloseButton );

	_MakeStatusTab( nValue );

#ifdef USE_BARCODE_REDER
	_InitBarcode();
#endif
}

void cTamerStatusUI::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );

	if( m_pCurViewer )
		m_pCurViewer->Update( fDeltaTime );

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cTamerStatusUI::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( MUT_OUT_WINDOW == muReturn )
		return muReturn;
	
	if( m_pStatusTab && m_pStatusTab->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;

	if( m_pCancelBtn && m_pCancelBtn->Update_ForMouse() )
		return muReturn;

	if( m_pCurViewer && m_pCurViewer->Update_ForMouse() )
		return muReturn;

#ifdef USE_BARCODE_REDER
	if( m_pBarcodeBtn && m_pBarcodeBtn->Update_ForMouse() )
		return muReturn;
#endif

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 370, 40 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
	return muReturn;
}

void cTamerStatusUI::Render()
{
	RenderScript();
}

void cTamerStatusUI::ResetDevice()
{
	ResetDeviceScript();
}

void cTamerStatusUI::OnLButtonUp(CsPoint pos)
{
	if( m_pCurViewer )
		m_pCurViewer->OnLButtonUp( pos );
}

void cTamerStatusUI::OnRButtonUp(CsPoint pos)
{

}

bool cTamerStatusUI::CursorIconLBtnUp(CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount)
{
	if( m_pCurViewer )
		return m_pCurViewer->CursorIconLBtnUp( eIconType, nIconSlot, nIconCount );
	return false;
}

bool cTamerStatusUI::OnEscapeKey()
{
	GetSystem()->CloseTamerStatus();
	return true;
}

void cTamerStatusUI::_MakeStatusTab(int nTabCount)
{
	CsPoint ptPos = CsPoint( 10, 46 );
	CsPoint ptSize = CsPoint( 96, 24 );
	CsPoint ptTex = CsPoint( 0, 24 );

	m_pStatusTab = AddRadioButton( ptPos, ptSize, ptTex, "TamerStatus_New\\TamerStatus_btn.tga" );
	if( m_pStatusTab )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "TAMERSTATUS_TAB_TAMER_TEXT" ).c_str() );
		m_pStatusTab->SetCheckBoxText( &ti );
		m_pStatusTab->SetUserData( new sTabInfo( eTabTamer ) );
		m_pStatusTab->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &cTamerStatusUI::_OnClickTabButton );

		_AddStatusTab( ptPos, ptSize, ptTex, UISTRING_TEXT( "TAMERSTATUS_TAB_DIGIVICE_TEXT" ), eTabDigivice );
		_AddStatusTab( ptPos, ptSize, ptTex, UISTRING_TEXT( "TAMERSTATUS_TAB_SKILL_TEXT" ), eTabSkill );

#ifdef CROSSWARS_SYSTEM
		if( 4 == nTabCount )
			_AddStatusTab( ptPos, ptSize, ptTex, UISTRING_TEXT( "TAMERSTATUS_TAB_CROSS_TEXT" ), eTabCross );
#endif

		m_pStatusTab->SetCheckIndex( 0 );
	}
}

void cTamerStatusUI::_AddStatusTab(CsPoint& ptPos, CsPoint ptSize, CsPoint ptTex, std::wstring wsTabName, eTabType eType)
{
	ptPos.x += 97;

	cRadioButton* pAddBtn = m_pStatusTab->AddNode( ptPos, ptSize, ptTex, "TamerStatus_New\\TamerStatus_btn.tga" );
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

void cTamerStatusUI::_SetStatusTab(int nTabNo)
{
	if( m_pStatusTab )
		m_pStatusTab->SetCheckIndex( nTabNo );

	if( m_pCurViewer )
		m_pCurViewer->SetVisible( false );

	switch( nTabNo )
	{
	case IF_TAB_TAMER:
		{
			if( m_pTamerViewer )
			{
				if( m_pBgImg )
					m_pBgImg->ChangeTexture( "TamerStatus_New\\TamerStatus_Win_01.tga" );
				m_pCurViewer = m_pTamerViewer;
			}
		}
		break;

	case IF_TAB_DIGIVICE:
		{
			if( m_pDigiviceViewer )
			{
				if( m_pBgImg )
					m_pBgImg->ChangeTexture( "TamerStatus_New\\TamerStatus_Win_02.tga" );
				m_pCurViewer = m_pDigiviceViewer;
			}
		}
		break;

	case IF_TAB_SKILL:
		{
			if( m_pSkillViewer )
			{
				if( m_pBgImg )
					m_pBgImg->ChangeTexture( "TamerStatus_New\\TamerStatus_Win_03.tga" );
				m_pCurViewer = m_pSkillViewer;
			}
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case IF_TAB_CROSS:
		{
			if( m_pCrossViewer )
			{
				if( m_pBgImg )
					m_pBgImg->ChangeTexture( "TamerStatus_New\\TamerStatus_Win_04.tga" );
				m_pCurViewer = m_pCrossViewer;
			}
		}
		break;
#endif
	}

	m_pCurViewer->SetVisible( true );
	m_pCurViewer->UpdateViewer();
}

void cTamerStatusUI::_OnClickTabButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cRadioButton* pButton = static_cast< cRadioButton* >( pData );
	sTabInfo* pUserData = dynamic_cast< sTabInfo* >( pButton->GetUserData() );
	SAFE_POINTER_RET( pUserData );

	if( m_pCurViewer )
		m_pCurViewer->SetVisible( false );

	_SetStatusTab( pUserData->eType );

	cWindow::PlaySound( cWindow::SD_At2 );

	CURSOR_ST.ReleaseIcon();
}

void cTamerStatusUI::_OnClickCloseButton(void* pSender, void* pData)
{
	GetSystem()->CloseTamerStatus();
}
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef USE_BARCODE_REDER
void cTamerStatusUI::_DeleteBarcode()
{
	m_BarcodeState = 0;
	m_pBarcodeBtn->SetVisible( false );
}

void cTamerStatusUI::_InitBarcode()
{
	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_10, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( L"" );

	m_pBarcodeBtn = AddButton( CsPoint( 253, 673 ), CsPoint( 151, 25 ), CsPoint::ZERO, "TamerStatus_New\\btn_Reg_scanner.tga" );
	m_pBarcodeBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cTamerStatusUI::_OnClickRegisterButton );
	m_pBarcodeBtn->SetText( &ti );

	m_BarcodeState = 0;
	_CalBarcodeState();
	_InitBarcodeState();

	if( !g_pResist->m_Global.s_bUseBarcode )
	{
		m_pBarcodeBtn->SetVisible( false );
	}
}

void cTamerStatusUI::_InitBarcodeState()
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	cCondition* pCondition = pUser->GetCondition();
	SAFE_POINTER_RET( pCondition );

	std::wstring wsScannerText;
	// 스캐너 연결되어 있는지
	if( pCondition->IsCondition( nSync::Scanner0 ) == true )
	{
		// 게임 스캐너 #State#
		wsScannerText = UISTRING_TEXT( "GAMESCANNER_CONNECT" );

// 		// 한정/일반
// 		if( pCondition->IsCondition( nSync::Scanner2 ) == true )
// 			DmCS::StringFn::Replace( wsScannerText, L"#Type1#", UISTRING_TEXT( "GAMESCANNER_UNIQUE_TEXT" ) );
// 		else
// 			DmCS::StringFn::Replace( wsScannerText, L"#Type1#", UISTRING_TEXT( "GAMESCANNER_NORMAL_TEXT" ) );

		// 스캐너 등록 - 했냐(스캔/버프)/안했냐(스캔)
		if( pCondition->IsCondition( nSync::Scanner1 ) == true )
			DmCS::StringFn::Replace( wsScannerText, L"#State#", UISTRING_TEXT( "GAMESCANNER_RELEASE_TEXT" ) );
		else
			DmCS::StringFn::Replace( wsScannerText, L"#State#", UISTRING_TEXT( "GAMESCANNER_REGIST_TEXT" ) );
	}
	else
	{
		// 스캐너 연결되지 않음
		wsScannerText = UISTRING_TEXT( "GAMESCANNER_NOTCONNECT" );
	}

	m_pBarcodeBtn->SetText( wsScannerText.c_str() );
	m_pBarcodeBtn->SetVisible( true );
}

bool cTamerStatusUI::_CalBarcodeState()
{
	uint bs = 0;

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, false );
	cCondition* pCondition = pUser->GetCondition();
	SAFE_POINTER_RETVAL( pCondition, false );

	if( pCondition->IsCondition( nSync::Scanner0 ) )
		bs |= nSync::Scanner0;
	if( pCondition->IsCondition( nSync::Scanner1 ) )
		bs |= nSync::Scanner1;
	if( pCondition->IsCondition( nSync::Scanner2 ) )
		bs |= nSync::Scanner2;

	if( bs == m_BarcodeState )
		return false;

	m_BarcodeState = bs;
	return true;
}

void cTamerStatusUI::_OnClickRegisterButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	cCondition* pCondition = pUser->GetCondition();
	SAFE_POINTER_RET( pCondition );

	// 스캐너 연결되어 있는지
	if( pCondition->IsCondition( nSync::Scanner0 ) )
	{
		// 스캐너 등록 했는지
		if( pCondition->IsCondition( nSync::Scanner1 ) )	// 내 소유로 등록되어 있음
		{
			// 해제
			cPrintMsg::PrintMsg( 20040 );
		}
		else
		{
			// 등록
			cPrintMsg::PrintMsg( 20039 );
		}
	}
}

#endif // USE_BARCODE_REDER
