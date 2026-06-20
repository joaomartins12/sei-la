
#include "stdafx.h"
#include "ElementItem_Check.h"

#define IF_EI_PROCESS_TIME		3.5f

cElementItem_Check::cElementItem_Check() :
m_pProgressBar( NULL ),
m_pMoney( NULL ),
m_pAnalysisBtn( NULL ),
m_pCancelButton( NULL ),
m_pListBox( NULL )
{

}

cElementItem_Check::~cElementItem_Check()
{

}

bool cElementItem_Check::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eRegistCheckItem,	this );
	GetSystem()->Register( SystemType::eSuccessAnalysis,	this );
	GetSystem()->Register( SystemType::eResetWindow,		this );

	return true;
}

void cElementItem_Check::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eRegistCheckItem:
		{
			uint nItemType = 0;
			u8 nMoney = 0;
			bool bEnable = false;
			kStream >> nItemType >> nMoney >> bEnable;
			_SetRegistItem( nItemType, nMoney, bEnable );
			_SetProgressBar( false );
			_SetAnalysisButton( sButtonInfo::eAnalysis, bEnable );
		}
		break;
	case SystemType::eSuccessAnalysis:
		{
			u1 nRate = 0;
			kStream >> nRate;
			_SuccessAnalysis( nRate );
		}
		break;
	case SystemType::eResetWindow:
		{
			_SetRegistItem( 0, 0, true );
			_SetProgressBar( false );
		}
		break;
	}
}

cBaseWindow::eWINDOW_TYPE cElementItem_Check::GetWindowType()
{
	return WT_ELEMENTITEM_CHECK;
}

void cElementItem_Check::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();	
}

void cElementItem_Check::DeleteResource()
{
	DeleteScript();
}		

void cElementItem_Check::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "EI_Check\\Win.tga", CsPoint( 150, 165 ), CsPoint( 255, 183 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );

	m_pCancelButton = AddButton( CsPoint( 233, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	if( m_pCancelButton )
		m_pCancelButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cElementItem_Check::_OnClickCancelButton );

	m_pProgressBar = NiNew cUI_TimerProgressBar;
	if( m_pProgressBar )
	{
		m_pProgressBar->Init( m_pRoot, CsPoint( 77, 57 ), CsPoint( 156, 13 ), false );
		m_pProgressBar->SetViewTextType( cUI_ProgressBar::NONE );
		m_pProgressBar->SetGuageImage( "DigitamaTrade\\Digitama_ScanBar.tga" );
		m_pProgressBar->SetStepSize( 0.01f );
		m_pProgressBar->SetTimer(0.0f, IF_EI_PROCESS_TIME);
		m_pProgressBar->Stop();
		m_pProgressBar->AddEvent( cUI_ProgressBar::eProgressbar_Done, this, &cElementItem_Check::_ProcessSuccess );
		AddChildControl( m_pProgressBar );
	}

	m_pAnalysisBtn = AddButton( CsPoint( 181, 146 ), CsPoint( 61, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_BTN_ANALYSIS" ).c_str() );
	if( m_pAnalysisBtn )
	{
		m_pAnalysisBtn->SetUserData( new sButtonInfo() );
		m_pAnalysisBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cElementItem_Check::_OnClickAnalysisButton );
	}

#ifdef SIMPLE_TOOLTIP
	m_rcReqCost.SetRect( CsPoint( 20, 150 ), CsSIZE( 145, 20 ) );
#endif

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	
	ti.SetText( UISTRING_TEXT( "ELEMENT_CHECK_AGENT" ).c_str() );
	AddText( &ti, CsPoint( 16, 101 ) );

	ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_ANALYSIS_MONEY" ).c_str() );
	AddText( &ti, CsPoint( 18, 153 ) );
	AddTitle( UISTRING_TEXT( "ELEMENT_CHECK_TITLE_ATTRIBUTE_INFO" ).c_str() );

	m_pMoney = NiNew cMoneyIF;
	if( m_pMoney )
	{
		m_pMoney->Init( cMoneyIF::DISP_SIMPLE_1, (u8)0, false );
		m_pMoney->SetVisible( false );
		m_pMoney->SetPos( CsPoint( 18, 150 ) );
		AddChildControl( m_pMoney );
	}

	m_pListBox = NiNew cListBox;
	if( m_pListBox )
	{
		m_pListBox->Init( m_pRoot, CsPoint( 20, 47 ), CsPoint( 32, 32 ), NULL, false );
		AddChildControl( m_pListBox );
	}
	_MakeRegistItem();
}

void cElementItem_Check::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cElementItem_Check::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pCancelButton && m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( m_pAnalysisBtn && m_pAnalysisBtn->Update_ForMouse() /*== cButton::ACTION_CLICK*/ )
	{
#ifdef SIMPLE_TOOLTIP
		SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, muReturn );
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL( pTooltip, muReturn );
		pTooltip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 32, 32 ), 100, cTooltip::UIHELP, 1033 );
#endif
		return muReturn;
	}

	if( m_pListBox && m_pListBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		_SetRegisterTooltip( m_pListBox->GetMouseOverItem() );
		return muReturn;
	}

#ifdef SIMPLE_TOOLTIP
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	if(m_rcReqCost.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1032 );
		return muReturn;
	}
#endif

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 255, 27 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
	return muReturn;
}

void cElementItem_Check::Render()
{
	RenderScript();
}

bool cElementItem_Check::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			GetSystem()->SetRegistAnalysis( nIconSlot );
			CURSOR_ST.ReleaseIcon();
			return true;
		}
		break;
	}

	return false;
}

void cElementItem_Check::ResetDevice()
{	
	ResetDeviceScript();
}

void cElementItem_Check::ResetMap()
{
	GetSystem()->CloseElementItem( NULL );
}

bool cElementItem_Check::OnEscapeKey()
{
	GetSystem()->CloseElementItem( NULL );
	return true;
}

void cElementItem_Check::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cElementItem_Check::UserDie()
{
	cBaseWindow::Close( false );
}

void cElementItem_Check::RecvDiedAlready()
{
	cBaseWindow::Close( false );
}

void cElementItem_Check::_OnClickCancelButton(void* pSender, void* pData)
{
	GetSystem()->CloseElementItem( NULL );
}

void cElementItem_Check::_OnClickAnalysisButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pAnalysisBtn );
	sButtonInfo* pBtnInfo = static_cast< sButtonInfo* >( m_pAnalysisBtn->GetUserData() );
	SAFE_POINTER_RET( pBtnInfo );

	switch( pBtnInfo->eBtnState )
	{
	case sButtonInfo::eAnalysis:
		{
			if( !GetSystem()->StartAnalysis() )
				return;

			_SetProgressBar( true );

			_SetAnalysisButton( sButtonInfo::eCancel );

			cWindow::PlaySound( "System\\System_return_01.wav" );
		}
		break;
	case sButtonInfo::eSuccess:
		{
			GetSystem()->ResetRegistItem_Check();

			_SetAnalysisButton( sButtonInfo::eAnalysis, false );

			cWindow::PlaySound( cWindow::SD_Np1 );
		}
		break;
	case sButtonInfo::eCancel:
		{
			if( !GetSystem()->CancelAnalysis() )
				return;

			_SetProgressBar( false );

			pBtnInfo->eBtnState = sButtonInfo::eAnalysis;
			m_pAnalysisBtn->SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_BTN_ANALYSIS" ).c_str() );
		}
		break;
	}
}

void cElementItem_Check::_ProcessSuccess(void* pSender, void* pData)
{
	GetSystem()->EndAnalysis();
}

void cElementItem_Check::_MakeRegistItem()
{
	SAFE_POINTER_RET( m_pListBox );
	m_pListBox->RemoveAllItem();

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	m_RegistItem.pItemIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, 0, 1, CsPoint::ZERO );
	if( m_RegistItem.pItemIcon )
		m_RegistItem.pItemIcon->SetAutoPointerDelete( true );

	m_RegistItem.pItemControl = NiNew cListBoxItem;
	if( m_RegistItem.pItemControl )
	{
		m_RegistItem.pItemControl->SetItem( pControl );
		m_RegistItem.pItemControl->SetUserData( new sItemInfo( 0, 0 ) );
		m_pListBox->AddItem( m_RegistItem.pItemControl );
	}
}

void cElementItem_Check::_SetProgressBar(bool bPlay)
{
	SAFE_POINTER_RET( m_pProgressBar );

	m_pProgressBar->SetTimer( 0.0f, IF_EI_PROCESS_TIME );

	if( bPlay )
		m_pProgressBar->Start();
	else
		m_pProgressBar->Stop();
}


void cElementItem_Check::_SetMoney(bool bVisible, bool bEnable, u8 nMoney)
{
	SAFE_POINTER_RET( m_pMoney );
	if( !bVisible )
	{
		m_pMoney->SetVisible( false );
		return;
	}

	m_pMoney->SetVisible( true );
	if( !bEnable )
		m_pMoney->SetColor_BeforeSetMoney( NiColor( 1, 0, 0 ) );
	else
		m_pMoney->SetColor_BeforeSetMoney( NiColor::WHITE );

	m_pMoney->SetMoney( nMoney );
}

void cElementItem_Check::_SetRegistItem(uint nItemType, u8 nMoney, bool bEnable)
{
	m_RegistItem.ChangeItem( nItemType, 0 );

	_SetMoney( nItemType, bEnable, nMoney );

	cWindow::PlaySound( "System\\interface_person_01.wav" );
}

void cElementItem_Check::_SetRegisterTooltip(cListBoxItem const* pOverItem)
{
	SAFE_POINTER_RET( pOverItem );
	sItemInfo* pInfo = dynamic_cast< sItemInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pInfo );

	// 아이템 등록된 상태
	if( 0 != pInfo->nItemType )
	{
		SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RET( pTooltip );
		cItemInfo* pRegiseItem = GetSystem()->GetRegistItem_Check();
		if( pRegiseItem )
			pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->nItemType, cBaseWindow::WT_ELEMENTITEM_CHECK,
			cTooltip::OPEN_SLOT, pInfo->nItemRate, pRegiseItem);
	}
#ifdef SIMPLE_TOOLTIP
	else
	{
		SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RET( pTooltip );
		pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), 100, cTooltip::UIHELP, 1031 );
	}
#endif
}


void cElementItem_Check::_SetAnalysisButton(sButtonInfo::eButtonState eState, bool bEnable /*= true*/)
{
	SAFE_POINTER_RET( m_pAnalysisBtn );

	switch( eState )
	{
	case sButtonInfo::eAnalysis:
		m_pAnalysisBtn->SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_BTN_ANALYSIS" ).c_str() );
		break;
	case sButtonInfo::eCancel:
		m_pAnalysisBtn->SetText( UISTRING_TEXT( "COMMON_TXT_CANCEL" ).c_str() );
		break;
	case sButtonInfo::eSuccess:
		m_pAnalysisBtn->SetText( UISTRING_TEXT( "COMMON_TXT_COMPLETE" ).c_str() );
		break;
	}

	m_pAnalysisBtn->SetEnable( bEnable );

	sButtonInfo* pBtnInfo = static_cast< sButtonInfo* >( m_pAnalysisBtn->GetUserData() );
	if( pBtnInfo )
		pBtnInfo->eBtnState = eState;
}

void cElementItem_Check::_SuccessAnalysis(u1 nRate)
{
	m_RegistItem.ChangeRate( nRate );

	_SetAnalysisButton( sButtonInfo::eSuccess );
}

////////////////////////////////////////////////////////////////////////////////////////////
void cElementItem_Check::sRegistItem::ChangeItem(uint nType, u1 nRate)
{
	if( pItemIcon )
		pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, nType, 1, nRate );

	if( pItemControl )
	{
		sItemInfo* pInfo = dynamic_cast< sItemInfo* >( pItemControl->GetUserData() );
		if( pInfo )
			pInfo->SetItem( nType, nRate );
	}
}

void cElementItem_Check::sRegistItem::ChangeRate(u1 nRate)
{
	if( pItemIcon )
		pItemIcon->s_nIndex_3 = nRate;

	if( pItemControl )
	{
		sItemInfo* pInfo = dynamic_cast< sItemInfo* >( pItemControl->GetUserData() );
		if( pInfo )
			pInfo->nItemRate = nRate;
	}
}

void cElementItem_Check::sRegistItem::Reset()
{
	ChangeItem( 0, 0 );
}
