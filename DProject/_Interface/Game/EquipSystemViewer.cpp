#include "stdafx.h"
#include "EquipSystemViewer.h"
#include "ElementManagement.h"
#include "EquipmentCharge.h"
#include "EquipmentChargeQuick.h"

cEquipSystemViewer::cEquipSystemViewer() :
m_pTabButton( NULL ),
m_pCloseButton( NULL ),
m_pCurViewer( NULL ),
m_pElemViewer( NULL ),
m_pNormalChargeViewer( NULL ),
m_pQuickChargeViewer( NULL )
{

}

cEquipSystemViewer::~cEquipSystemViewer()
{

}

bool cEquipSystemViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	return true;
}

void cEquipSystemViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}

cBaseWindow::eWINDOW_TYPE cEquipSystemViewer::GetWindowType()
{
	return WT_EQUIPMENT_SYSTEM;
}

void cEquipSystemViewer::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cEquipSystemViewer::DeleteResource()
{
	DeleteScript();
}

void cEquipSystemViewer::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	CsPoint ptSize( 650, 700 );
	CsPoint ptPos( g_nScreenWidth, g_nScreenHeight );
	ptPos -= ptSize;
	ptPos.x *= 0.1f;
	ptPos.y *= 0.4f;

	InitScript( "EquipSystem\\Recharge_bg.tga", ptPos, ptSize, true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( UISTRING_TEXT( "EQUIPSYSTEM_TITLE" ).c_str(), CsPoint( 0, 12 ), CFont::FS_16 );

	m_pCloseButton = AddButton( CsPoint( ptSize.x - 44, 12 ), CsPoint( 32, 32 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );
	if( m_pCloseButton )
		m_pCloseButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cEquipSystemViewer::_OnClickCloseButton );

	CsPoint ptTabPos( 22, 64 );
	CsPoint ptTabSize( 168, 38 );
	CsPoint ptTabToken( 0, 38 );
	m_pTabButton = AddRadioButton( ptTabPos, ptTabSize, ptTabToken, "CommonUI\\Menu_tap_btn.tga" );
	if( m_pTabButton )
	{
		{	// 일반 충전
			cText::sTEXTINFO ti;
			ti.Init( CFont::FS_12, NiColor::WHITE );
			ti.s_eTextAlign = DT_CENTER;
			ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_TAB_NORMAL_CHARGE" ).c_str() );
			m_pTabButton->SetCheckBoxText( &ti );
		}
		m_pTabButton->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &cEquipSystemViewer::_OnClickTabButton );
		m_pTabButton->SetUserData( new sTabInfo( eTabEquipCharge ) );
		m_pTabButton->SetEnable( true );
		m_pTabButton->SetCheckIndex( 0 );

		ptTabPos.x += ptTabSize.x;
		cRadioButton* pAddButton = m_pTabButton->AddNode( ptTabPos, ptTabSize, ptTabToken, "CommonUI\\Menu_tap_btn.tga" );
		if( pAddButton )
		{
			{	// 즉시 충전
				cText::sTEXTINFO ti;
				ti.Init( CFont::FS_12, NiColor::WHITE );
				ti.s_eTextAlign = DT_CENTER;
				ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_TAB_QUICK_CHARGE" ).c_str() );
				pAddButton->SetCheckBoxText( &ti );
			}
			pAddButton->SetUserData( new sTabInfo( eTabEquipChargeQuick ) );
			pAddButton->SetEnable( true );
		}

		ptTabPos.x += ptTabSize.x;
		pAddButton = m_pTabButton->AddNode( ptTabPos, ptTabSize, ptTabToken, "CommonUI\\Menu_tap_btn.tga" );
		if( pAddButton )
		{
			{	// 속성 관리
				cText::sTEXTINFO ti;
				ti.Init( CFont::FS_12, NiColor::WHITE );
				ti.s_eTextAlign = DT_CENTER;
				ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_TAB_ELEM_MANAGE" ).c_str() );
				pAddButton->SetCheckBoxText( &ti );
			}
			pAddButton->SetUserData( new sTabInfo( eTabElemManage ) );
			pAddButton->SetEnable( true );
		}
	}

	m_pElemViewer = NiNew cElementManagement;
	if( m_pElemViewer )
	{
		m_pElemViewer->Create( m_pRoot );
		AddChildScriptUI( m_pElemViewer );
	}

	m_pNormalChargeViewer = NiNew cEquipmentCharge;
	if( m_pNormalChargeViewer )
	{
		m_pNormalChargeViewer->Create( m_pRoot );
		AddChildScriptUI( m_pNormalChargeViewer );
	}

	m_pQuickChargeViewer = NiNew cEquipmentChargeQuick;
	if( m_pQuickChargeViewer )
	{
		m_pQuickChargeViewer->Create( m_pRoot );
		AddChildScriptUI( m_pQuickChargeViewer );
	}

	_SetViewer( m_pNormalChargeViewer, true );
	_SetViewer( m_pQuickChargeViewer, false );
	_SetViewer( m_pElemViewer, false );
}

void cEquipSystemViewer::Update(float const& fDeltaTime)
{
	if( m_pCurViewer )
		m_pCurViewer->Update( fDeltaTime );

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cEquipSystemViewer::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( MUT_OUT_WINDOW == muReturn )
		return muReturn;

	if( m_pCurViewer && m_pCurViewer->Update_ForMouse() )
		return muReturn;

	if( m_pTabButton && m_pTabButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( m_pCloseButton && m_pCloseButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 750, 40 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cEquipSystemViewer::Render()
{
	RenderScript();
}

bool cEquipSystemViewer::CursorIconLBtnUp(CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount)
{
	SAFE_POINTER_RETVAL( m_pCurViewer, false );
	return m_pCurViewer->CursorIconLBtnUp( eIconType, nIconSlot, nIconCount );
}

void cEquipSystemViewer::ResetDevice()
{
	ResetDeviceScript();
}

void cEquipSystemViewer::ResetMap()
{
	cBaseWindow::Close( false );
}

bool cEquipSystemViewer::OnEscapeKey()
{
	GetSystem()->CloseEquipSystem( NULL );
	return true;
}

void cEquipSystemViewer::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cEquipSystemViewer::UserDie()
{
	cBaseWindow::Close( false );
}

void cEquipSystemViewer::RecvDiedAlready()
{
	cBaseWindow::Close( false );
}

void cEquipSystemViewer::_OnClickTabButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cRadioButton* pTab = static_cast<cRadioButton*>( pData );
	sTabInfo* pInfo = dynamic_cast< sTabInfo* >( pTab->GetUserData() );
	SAFE_POINTER_RET( pInfo );

	switch( pInfo->eTab )
	{
	case eTabType::eTabElemManage:
		{
			if( !GetSystem()->SetTabState( SystemType::eTabState::eTabElem ) )
				return;

			_SetViewer( m_pNormalChargeViewer, false );
			_SetViewer( m_pQuickChargeViewer, false );
			_SetViewer( m_pElemViewer, true );
		}
		break;
	case eTabType::eTabEquipCharge:
		{
			if( !GetSystem()->SetTabState( SystemType::eTabState::eTabNormal ) )
				return;

			_SetViewer( m_pElemViewer, false );
			_SetViewer( m_pQuickChargeViewer, false );
			_SetViewer( m_pNormalChargeViewer, true );
		}
		break;
	case eTabType::eTabEquipChargeQuick:
		{
			if( !GetSystem()->SetTabState( SystemType::eTabState::eTabQuick ) )
				return;

			_SetViewer( m_pElemViewer, false );
			_SetViewer( m_pNormalChargeViewer, false );
			_SetViewer( m_pQuickChargeViewer, true );
		}
		break;
	}

	if( m_pTabButton )
		m_pTabButton->SetCheckIndex( pInfo->eTab );
}

void cEquipSystemViewer::_OnClickCloseButton(void* pSender, void* pData)
{
	GetSystem()->CloseEquipSystem( NULL );
}

void cEquipSystemViewer::_SetViewer(cEquipSystemSubViewer* pViewer, bool bEnable)
{
	SAFE_POINTER_RET( pViewer );
	pViewer->SetEnableWindow( bEnable );
	pViewer->SetVisible( bEnable );
	pViewer->UpdateViewer();

	// 활성화된 탭으로 갱신
	if( bEnable )
		m_pCurViewer = pViewer;
}

