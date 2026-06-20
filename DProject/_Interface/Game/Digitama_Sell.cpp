
#include "stdafx.h"
#include "Digitama_Sell.h"

cDigitama_Sell::cDigitama_Sell() : m_pProgressBar( NULL ), m_pBtnOCE( NULL ), m_pCancelButton( NULL ),
m_pBtnAutoResist( NULL ), m_pTextResistCount( NULL )
{
}

cDigitama_Sell::~cDigitama_Sell()
{
}

bool cDigitama_Sell::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eSellFail, this );
	GetSystem()->Register( SystemType::eSellStart, this );
	GetSystem()->Register( SystemType::eSellCancel, this );
	GetSystem()->Register( SystemType::eSellEnd, this );
	GetSystem()->Register( SystemType::eRegisterSellItem, this );

	return true;
}

void cDigitama_Sell::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eSellFail:
		_ProcessFail();
		break;

	case SystemType::eSellStart:
		_ProcessStart();
		break;

	case SystemType::eSellCancel:
		_ProcessCancel();
		break;

	case SystemType::eSellEnd:
		_ProcessEnd();
		break;

	case SystemType::eRegisterSellItem:
		u8 nMoney = 0;
		uint nCount = 0;
		uint nType = 0;
		kStream >> nMoney >> nCount >> nType;
		_ProcessRegister( nMoney, nCount, nType );
		break;
	}
}

void cDigitama_Sell::Destroy()
{
	cBaseWindow::Delete();	
}

void cDigitama_Sell::DeleteResource()
{
	if( GetSystem() )
	{
		GetSystem()->UnRegisterAll( this );
	}

	DeleteScript();
}

void cDigitama_Sell::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "DigitamaTrade\\Digitama_DigitaWin.tga", CsPoint( 150, 165 ), CsPoint( 255, 146 ), true );
	AddTitle( UISTRING_TEXT("ITEM_RECOVERY_OUTPUT_TEXT").c_str() );

	m_pProgressBar = NiNew cUI_TimerProgressBar;
	if( m_pProgressBar )
	{
		m_pProgressBar->Init( m_pRoot, CsPoint( 61, 60 ), CsPoint( 171, 15 ), false );
		m_pProgressBar->SetViewTextType( cUI_ProgressBar::NONE );
		m_pProgressBar->SetGuageImage( "DigitamaTrade\\Digitama_ScanBar.tga" );
		m_pProgressBar->SetStepSize( 0.01f );
		m_pProgressBar->SetTimer(0.0f, IF_DIGITAMA_PROCESS_TIME);
		m_pProgressBar->Stop();
		m_pProgressBar->AddEvent( cUI_ProgressBar::eProgressbar_Done, this, &cDigitama_Sell::_ProcessSuccess );
		AddChildControl( m_pProgressBar );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_RIGHT;
		m_pTextResistCount = AddText( &ti, CsPoint( 81, 85 ) );
		m_pTextResistCount->SetVisible( false );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "GLOBAL_EXIT" ).c_str() );
		m_pBtnOCE = AddButton( CsPoint( 198, 88 ), CsPoint( 48, 48 ), cButton::IMAGE_NOR_2, &ti );
		if( m_pBtnOCE )
		{
			m_pBtnOCE->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cDigitama_Sell::PressOKCancelExitBtn);
			m_pBtnOCE->SetUserData( new sButtonInfo() );
			m_pBtnOCE->SetControlName( "ItemSellExitBtn" );
		}
	}

	m_pCancelButton = AddButton( CsPoint( 233, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	if( m_pCancelButton )
		m_pCancelButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cDigitama_Sell::PressCancelButton);

	m_pBtnAutoResist = AddButton( CsPoint( 94, 84 ), CsPoint( 61, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_AUTO_REGISTER" ).c_str() );
	if( m_pBtnAutoResist )
		m_pBtnAutoResist->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cDigitama_Sell::PressAutoResistButton);

	m_pSaleMoney = NiNew cMoneyIF;
	if( m_pSaleMoney )
	{
		m_pSaleMoney->Init( cMoneyIF::DISP_SIMPLE_1, (u8)0, false );
		m_pSaleMoney->SetPos(CsPoint( 42, 117 ));
		AddChildControl(m_pSaleMoney);
	}

	m_pListBox = NiNew cListBox;
	if( m_pListBox )
	{
		m_pListBox->Init( m_pRoot, CsPoint( 20, 52 ), CsPoint( 32, 32 ), NULL, false );
		AddChildControl( m_pListBox );
	}
	_AddRegisterItem();

#ifdef SIMPLE_TOOLTIP
	m_rcRecvBit.SetRect( CsPoint( 44, 118 ), CsSIZE( 142, 20 ) );
#endif
}

void cDigitama_Sell::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);

	if( GetSystem() )
	{
		CsC_AvObject* pTarget = GetSystem()->GetTargetNpc();
		if( pTarget )
		{
			if( dynamic_cast<CNpc*>(pTarget)->CheckTamerDist() == false )
			{
				_ProcessExit();
				return;
			}
		}
		else
		{
			SAFE_POINTER_RET( g_pCharMng );
			CTamerUser* pTamer = g_pCharMng->GetTamerUser();
			SAFE_POINTER_RET( pTamer );

			float fCheckDist = IF_TARGET_TAMER_MAX_DIST*2.0;
			NiPoint2 niPos = GetSystem()->GetTargetPos();
			if( ( niPos - pTamer->GetPos2D() ).Length() > fCheckDist )
			{
				_ProcessExit();
				return;
			}

			CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
			SAFE_POINTER_RET( pDigimon );
			if( pDigimon->IsBattle() )
			{
				_ProcessExit();
				return;
			}
		}
	}

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cDigitama_Sell::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if(m_pBtnOCE && m_pBtnOCE->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pCancelButton && m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pBtnAutoResist && m_pBtnAutoResist->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

#ifdef SIMPLE_TOOLTIP
	if( GetSystem() && GetSystem()->Get_ResistItemEnable() )
	{
		if( m_pListBox && m_pListBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
		{
			cListBoxItem const* pOverItem = m_pListBox->GetMouseOverItem();
			SAFE_POINTER_RETVAL( pOverItem, muReturn );

			cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
			SAFE_POINTER_RETVAL(pToolTip, muReturn);

			sRegistInfo* pData = dynamic_cast<sRegistInfo*>(m_RegisterItem.pItemControl->GetUserData());
			SAFE_POINTER_RETVAL(pData, muReturn);

			pToolTip->SetTooltip( pOverItem->GetWorldPos(), IF_DIGITAMA_ICONSIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->nItemType, 
				cBaseWindow::WT_DIGITAMA_SELL, cTooltip::OPEN_SLOT, 0, GetSystem()->Get_ResistItem_Ptr() );
		}

		if(m_pBtnOCE && m_pBtnOCE->Update_ForMouse() == cButton::ACTION_ON )
		{
			cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
			SAFE_POINTER_RETVAL(pToolTip, muReturn);

			pToolTip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1015 );

			return muReturn;
		}
	}
	if(m_pBtnAutoResist && m_pBtnAutoResist->Update_ForMouse() == cButton::ACTION_ON )
	{
		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		pToolTip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1014 );

		return muReturn;
	}
	if( m_rcRecvBit.PtInRect( CURSOR_ST.GetPos() ) )
	{
		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		pToolTip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1016 );

		return muReturn;
	}
#else
	if( GetSystem() && GetSystem()->Get_ResistItemEnable() )
	{
		if( m_pListBox && m_pListBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
		{
			cListBoxItem const* pOverItem = m_pListBox->GetMouseOverItem();
			SAFE_POINTER_RETVAL( pOverItem, muReturn );

			cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
			SAFE_POINTER_RETVAL(pToolTip, muReturn);

			sRegistInfo* pData = dynamic_cast<sRegistInfo*>(m_RegisterItem.pItemControl->GetUserData());
			SAFE_POINTER_RETVAL(pData, muReturn);

			pToolTip->SetTooltip( pOverItem->GetWorldPos(), IF_DIGITAMA_ICONSIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->nItemType, 
				cBaseWindow::WT_DIGITAMA_SELL, cTooltip::OPEN_SLOT, 0, GetSystem()->Get_ResistItem_Ptr() );
		}
	}
#endif

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 255, 27 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();		

	return muReturn;
}

void cDigitama_Sell::Render()
{
	RenderScript();
}

bool cDigitama_Sell::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );
	cItemInfo* pItemInfo = pInven->GetData( nIconSlot );
	SAFE_POINTER_RETVAL( pItemInfo, false );

	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			// 분리해서
			if( pItemInfo->GetCount() != nIconCount )
			{
				cPrintMsg::PrintMsg( 20030 );
			}
			else if( g_pDataMng->IsSellItem( pItemInfo ) == true )
			{
				GetSystem()->RegisterItem( &nIconSlot );
			}
			CURSOR_ST.ReleaseIcon();
			return true;
		}
		break;
	}

	return false;
}

void cDigitama_Sell::ResetDevice()
{	
	ResetDeviceScript();
}

void cDigitama_Sell::PreResetMap()
{
	_ProcessExit();
}

bool cDigitama_Sell::OnEscapeKey()
{ 
	_ProcessExit();
	return true; 
}

void cDigitama_Sell::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cDigitama_Sell::PressOKCancelExitBtn( void* pSender, void* pData )
{
	SAFE_POINTER_RET( m_pBtnAutoResist );
	SAFE_POINTER_RET( m_pBtnOCE );
	sButtonInfo* pBtInfo = dynamic_cast<sButtonInfo*>(m_pBtnOCE->GetUserData());
	SAFE_POINTER_RET( pBtInfo );
	switch( pBtInfo->eState )
	{
	case sButtonInfo::BTS_OK:
		m_pBtnAutoResist->SetEnable( false );
		GetSystem()->ProcessStartSell();
		break;
	case sButtonInfo::BTS_CANCEL:
		GetSystem()->ProcessCancelSell();
		break;
	case sButtonInfo::BTS_EXIT:
		_ProcessExit();
		break;
	}
}

void cDigitama_Sell::PressCancelButton( void* pSender, void* pData )
{
	_ProcessExit();
}

void cDigitama_Sell::PressAutoResistButton( void* pSender, void* pData )
{
	GetSystem()->AutoRegisterItemSell();
}

void cDigitama_Sell::_ProcessFail()
{
	SetEnableWindow( true );

	_ChangeButton( sButtonInfo::BTS_OK );

	SAFE_POINTER_RET( m_pBtnAutoResist );
	m_pBtnAutoResist->SetEnable( true );

	_SetProgressBar( false );
}

void cDigitama_Sell::_ProcessSuccess(void* pSender, void* pData)
{
	SetEnableWindow( false );
	GetSystem()->ProcessSuccessSell();
}

void cDigitama_Sell::_ProcessCancel()
{
	_ChangeButton( sButtonInfo::BTS_OK );

	SAFE_POINTER_RET( m_pBtnAutoResist );
	m_pBtnAutoResist->SetEnable( true );

	_SetProgressBar( false );
}

void cDigitama_Sell::_ProcessStart()
{
	SetEnableWindow( true );

	_ChangeButton( sButtonInfo::BTS_CANCEL );

	SAFE_POINTER_RET( m_pBtnAutoResist );
	m_pBtnAutoResist->SetEnable( false );

	_SetProgressBar( true );

	cWindow::PlaySound( "System\\Item_Disc_02.wav" );
}

void cDigitama_Sell::_ProcessRegister(u8 nMoney, uint nCount, uint nType)
{
	// 버튼 변경
	_ChangeButton( sButtonInfo::BTS_OK );

	SAFE_POINTER_RET( m_pTextResistCount );
	m_pTextResistCount->SetText( nCount );
	m_pTextResistCount->SetVisible( true );

	SAFE_POINTER_RET( m_pSaleMoney );
	m_pSaleMoney->SetMoney( nMoney );

	m_RegisterItem.ChangeItem( ICONITEM::ITEM_ICON, nType );

	cWindow::PlaySound( "System\\interface_person_01.wav" );
}

void cDigitama_Sell::_ProcessEnd()
{
	SetEnableWindow(true);

	_ChangeButton( sButtonInfo::BTS_EXIT );

	SAFE_POINTER_RET( m_pTextResistCount );
	m_pTextResistCount->SetVisible( false );

	SAFE_POINTER_RET( m_pBtnAutoResist );
	m_pBtnAutoResist->SetEnable( true );

	_SetProgressBar( false );

	m_RegisterItem.ChangeItem( ICONITEM::ITEM_ICON, 0 );

	SAFE_POINTER_RET( m_pSaleMoney );
	m_pSaleMoney->SetMoney( (u8)0 );
}

void cDigitama_Sell::_ProcessExit()
{
	UINT nWindowType = GetWindowType();
	GetSystem()->CloseDigitama( &nWindowType );
}

cItemInfo* cDigitama_Sell::GetResistItem()
{
	return GetSystem()->Get_ResistItem_Ptr(); 
}

void cDigitama_Sell::_ChangeButton(sButtonInfo::eBtnState eState)
{
	SAFE_POINTER_RET( m_pBtnOCE );
	sButtonInfo* pBtInfo = dynamic_cast<sButtonInfo*>(m_pBtnOCE->GetUserData());
	SAFE_POINTER_RET( pBtInfo );

	std::wstring wsBtnTxt;
	switch( eState )
	{
	case sButtonInfo::BTS_OK:
		pBtInfo->eState = sButtonInfo::BTS_OK;
		wsBtnTxt = UISTRING_TEXT( "GLOBAL_OK" );
		m_pBtnOCE->SetControlName( "ItemSellStartBtn" );
		break;
	case sButtonInfo::BTS_CANCEL:
		pBtInfo->eState = sButtonInfo::BTS_CANCEL;
		wsBtnTxt = UISTRING_TEXT( "GLOBAL_CANCEL" );
		m_pBtnOCE->SetControlName( "ItemSellCancelBtn" );
		break;
	case sButtonInfo::BTS_EXIT:
		pBtInfo->eState = sButtonInfo::BTS_EXIT;
		wsBtnTxt = UISTRING_TEXT( "GLOBAL_EXIT" );
		m_pBtnOCE->SetControlName( "ItemSellExitBtn" );
		break;
	}

	m_pBtnOCE->SetText( wsBtnTxt.c_str() );
}

void cDigitama_Sell::_SetProgressBar(bool bIsPlay)
{
	SAFE_POINTER_RET( m_pProgressBar );
	m_pProgressBar->SetTimer( 0.0f, IF_DIGITAMA_PROCESS_TIME );
	if( bIsPlay )
		m_pProgressBar->Start();
	else
		m_pProgressBar->Stop();
}

void cDigitama_Sell::_AddRegisterItem()
{
	SAFE_POINTER_RET( m_pListBox );
	m_pListBox->RemoveAllItem();

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	// 아이템 아이콘
	m_RegisterItem.pItemIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, 0, 1, CsPoint::ZERO );
	if( m_RegisterItem.pItemIcon )
		m_RegisterItem.pItemIcon->SetAutoPointerDelete(true);

	m_RegisterItem.pItemControl = NiNew cListBoxItem;
	if( m_RegisterItem.pItemControl )
	{
		m_RegisterItem.pItemControl->SetItem( pControl );
		m_RegisterItem.pItemControl->SetUserData( new sRegistInfo( 0 ) );
	}
	m_pListBox->AddItem( m_RegisterItem.pItemControl );
	m_pListBox->SetSelectedItemFromItemPtr( m_RegisterItem.pItemControl );
}