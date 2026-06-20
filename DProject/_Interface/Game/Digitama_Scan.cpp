#include "stdafx.h"
#include "Digitama_Scan.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#define IF_SCAN_RANK_NORMAL	0
#define IF_SCAN_RANK_ELITE	1
#define	IF_SCAN_RANK_UNIQUE	2

cDigitama_Scan::cDigitama_Scan() 
: m_pGridBox( NULL ),
m_nOper( 1 ),
m_pTextResistCount( NULL ),
m_pArrow( NULL ),
m_pBtnOCE( NULL ),
m_pCancelButton( NULL ), 
m_pBtnAutoResist( NULL ),
m_pMoneyIF( NULL ),
m_pProgressBar( NULL )
#ifdef UI_ITEMSCAN_RENEWAL
,m_pSkipBtn( NULL )
,m_pScanCancelBtn( NULL )
,m_pProgressBgImg( NULL )
#endif
{
	m_ptArrowPos = NiPoint2( 112, DEFAULT_ARROW_POSY );	
}

cDigitama_Scan::~cDigitama_Scan()
{
}

void cDigitama_Scan::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eScanFail:
		_ProcessFail();
		break;

	case SystemType::eScanStart:
		_ProcessStart();
		break;

	case  SystemType::eScanCancel:
		_ProcessCancel();
		break;

	case SystemType::eScanEnd:
		_ProcessEnd();
		break;

	case SystemType::eRegisterScanItem:
		{
			u8 nMoney = 0;
			uint nCount = 0;
			uint nType = 0;
			kStream >> nMoney >> nCount >> nType;
			_ProcessRegister( nMoney, nCount, nType );
		}
		break;

	case SystemType::eUpdateScanItem:
		{
#ifdef UI_ITEMSCAN_RENEWAL
			uint nScanType = 0;
			kStream >> nScanType;
			_UpdateScanItem( nScanType );
#else
			_UpdateScanItem();
#endif
		}
		break;
	}
}

bool cDigitama_Scan::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eScanFail, this );
	GetSystem()->Register( SystemType::eScanStart, this );
	GetSystem()->Register( SystemType::eScanCancel, this );
	GetSystem()->Register( SystemType::eScanEnd, this );
	GetSystem()->Register( SystemType::eRegisterScanItem, this );
	GetSystem()->Register( SystemType::eUpdateScanItem, this );

	return true;
}

void cDigitama_Scan::Destroy()
{
	cBaseWindow::Delete();	
}

void cDigitama_Scan::DeleteResource()
{	
	if( GetSystem() )
	{
		GetSystem()->UnRegisterAll( this );
	}

	DeleteScript();
}

#ifdef UI_ITEMSCAN_RENEWAL
void cDigitama_Scan::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "Scan_New\\Scan_win.tga", CsPoint( 150, 165 ), CsPoint( 344, 482 ), true);
	AddTitle( UISTRING_TEXT("ITEM_SCAN_OUTPUT_TEXT").c_str(), CsPoint( 0, 10 ) );

	{	// 획득 아이템 텍스트
		cText::sTEXTINFO	textInfo;
		textInfo.Init( CFont::FS_10, NiColor(138.0/255.0,245.0/255.0,12.0/255.0) );
		textInfo.SetText(UISTRING_TEXT( "ITEM_SCAN_GETITEM_TEXT" ).c_str());
		textInfo.s_eTextAlign = DT_LEFT;
		AddText( &textInfo, CsPoint(11, 260) );
	}

	m_pGridBox = NiNew cGridListBox;
	if( m_pGridBox )
	{
		m_pGridBox->Init( m_pRoot, CsPoint( 14, 285 ), CsPoint( 320, 160 ), CsPoint( 2, 2 ), CsPoint( 36, 36 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 8 );
		cScrollBar* pScroll = NiNew cScrollBar;
		if( pScroll )
		{
			pScroll->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint::ZERO, CsPoint( 16, 190 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint( 320, 160 ) ), 4, false );
			m_pGridBox->SetScrollBar( pScroll );
		}
		AddChildControl( m_pGridBox );
	}
	_MakeEmptyGridBox( IF_SCAN_MAX_COUNT );

	{	// Scan & Cancel Button
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "GLOBAL_CANCEL" ).c_str() );
		m_pScanCancelBtn = AddButton( CsPoint( 118, 176 ), CsPoint( 109, 37 ), CsPoint( 0, 37 ), "Scan_New\\Scan_btn_C.tga" );
		if( m_pScanCancelBtn )
		{
			m_pScanCancelBtn->SetText( &ti );
			m_pScanCancelBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cDigitama_Scan::PressScanCancelButton );
			sButtonInfo* pInfo = new sButtonInfo;
			pInfo->eState = sButtonInfo::BTS_CANCEL;
			m_pScanCancelBtn->SetUserData( pInfo );
			m_pScanCancelBtn->SetControlName( "ItemScanCancelBtn" );
		}
	}
	// Skip Button
	m_pSkipBtn = AddButton( CsPoint( 252, 446 ), CsPoint( 80, 28 ), CsPoint( 0, 28 ), "Scan_New\\Scan_btn_skip.tga" );
	if( m_pSkipBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "ITEM_SCAN_SKIP_TEXT" ).c_str() );
		m_pSkipBtn->SetText( &ti );
		m_pSkipBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cDigitama_Scan::PressSkipButton );
	}

	m_pCancelButton = AddButton( CsPoint( 310, 11 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );
	if( m_pCancelButton )
	{
		m_pCancelButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cDigitama_Scan::PressCancelButton);
		m_pCancelButton->SetControlName( "ItemScanClose" );
	}

	m_pBtnAutoResist = AddButton( CsPoint( 199, 87 ), CsPoint( 34, 28 ), CsPoint( 0, 28 ), "Scan_New\\Scan_btn_auto.tga" );
	if( m_pBtnAutoResist )
		m_pBtnAutoResist->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cDigitama_Scan::PressAutoResistButton);

	m_pArrow = AddSprite(CsPoint::ZERO, CsPoint( 25, 25 ), "DigitamaTrade\\Digitama_HelpBtn.tga");

	m_pProgressBgImg = AddSprite( CsPoint( 72, 134 ), CsPoint( 200, 35 ), "Scan_New\\Scan_gauge_bg.tga" );
	m_pProgressBgImg->SetVisible(false);

	m_pProgressBar = NiNew cUI_TimerProgressBar;
	if( m_pProgressBar )
	{
		m_pProgressBar->Init( m_pRoot, CsPoint( 72, 150 ), CsPoint( 196, 4 ), false );
		m_pProgressBar->SetViewTextType( cUI_ProgressBar::NONE );
		m_pProgressBar->SetGuageImage( "Scan_New\\Scan_gauge_bar.tga" );
		m_pProgressBar->SetStepSize( 0.01f );
		m_pProgressBar->SetTimer(0.0f, IF_DIGITAMA_PROCESS_TIME);
		m_pProgressBar->Stop();
		m_pProgressBar->AddEvent( cUI_ProgressBar::eProgressbar_Done, this, &cDigitama_Scan::_ProcessSuccess );
		AddChildControl( m_pProgressBar );
	}

	m_pMoneyIF = NiNew cMoneyIF;
	if( m_pMoneyIF )
	{
		m_pMoneyIF->Init( cMoneyIF::DISP_FULL, (u8)0, true );
		m_pMoneyIF->SetPos(CsPoint( 100, 147 ));
		AddChildControl(m_pMoneyIF);
	}

	m_pListBox = NiNew cListBox;
	if( m_pListBox )
	{
		m_pListBox->Init( m_pRoot, CsPoint( 156, 85 ), CsPoint( 32, 32 ), NULL, false );
		AddChildControl( m_pListBox );
	}
	_AddRegisterItem();

	SetArrowShow(false);
}
#else
void cDigitama_Scan::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "DigitamaTrade\\Digitama_ScanWin.tga", CsPoint( 150, 165 ), CsPoint( 255, 210 ), true);
	AddTitle( UISTRING_TEXT("ITEM_SCAN_OUTPUT_TEXT").c_str() );

	m_pGridBox = NiNew cGridListBox;
	if( m_pGridBox )
	{
		m_pGridBox->Init( m_pRoot, CsPoint( 14, 163 ), CsPoint( 350, 36 ), CsPoint( 15, 0 ), CsPoint( 32, 32 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		AddChildControl( m_pGridBox );
	}

	_MakeEmptyGridBox(IF_DSCAN_ITEM_COUNT);
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
	#ifdef VERSION_USA
		ti.s_eFontSize = CFont::FS_9;
	#endif
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "ITEMSCAN_COST_PER_ITEM" ).c_str() );
		AddText( &ti, CsPoint( 14, 120 ) );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_RIGHT;
		m_pTextResistCount = AddText( &ti, CsPoint( 85, 85 ) );
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
			m_pBtnOCE->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cDigitama_Scan::PressOKCancelExitBtn);
			m_pBtnOCE->SetUserData( new sButtonInfo() );
			m_pBtnOCE->SetControlName( "ItemScanExitBtn" );
		}
	}

	m_pCancelButton = AddButton( CsPoint( 233, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	if( m_pCancelButton )
	{
		m_pCancelButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cDigitama_Scan::PressCancelButton);
		m_pCancelButton->SetControlName( "ItemScanClose" );
	}

	m_pBtnAutoResist = AddButton( CsPoint( 94, 84 ), CsPoint( 61, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_AUTO_REGISTER" ).c_str() );
	if( m_pBtnAutoResist )
		m_pBtnAutoResist->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cDigitama_Scan::PressAutoResistButton);

	m_pArrow = AddSprite(CsPoint::ZERO, CsPoint( 25, 25 ), "DigitamaTrade\\Digitama_HelpBtn.tga");

	m_pProgressBar = NiNew cUI_TimerProgressBar;
	if( m_pProgressBar )
	{
		m_pProgressBar->Init( m_pRoot, CsPoint( 61, 60 ), CsPoint( 171, 15 ), false );
		m_pProgressBar->SetViewTextType( cUI_ProgressBar::NONE );
		m_pProgressBar->SetGuageImage( "DigitamaTrade\\Digitama_ScanBar.tga" );
		m_pProgressBar->SetStepSize( 0.01f );
		m_pProgressBar->SetTimer(0.0f, IF_DIGITAMA_PROCESS_TIME);
		m_pProgressBar->Stop();
		m_pProgressBar->AddEvent( cUI_ProgressBar::eProgressbar_Done, this, &cDigitama_Scan::_ProcessSuccess );
		AddChildControl( m_pProgressBar );
	}
	
	m_pMoneyIF = NiNew cMoneyIF;
	if( m_pMoneyIF )
	{
		m_pMoneyIF->Init( cMoneyIF::DISP_SIMPLE_1, (u8)0, false );
		m_pMoneyIF->SetPos(CsPoint( 42, 117 ));
		AddChildControl(m_pMoneyIF);
	}

	m_pListBox = NiNew cListBox;
	if( m_pListBox )
	{
		m_pListBox->Init( m_pRoot, CsPoint( 20, 52 ), CsPoint( 32, 32 ), NULL, false );
		AddChildControl( m_pListBox );
	}
	_AddRegisterItem();
	
	SetArrowShow(false);

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	// 휴대용 스캐너 lock
	pInven->ItemLock_ItemType( nItem::Portable_Item, eSCANNER);
}
#endif //UI_ITEMSCAN_RENEWAL

void cDigitama_Scan::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );

#ifdef UI_ITEMSCAN_RENEWAL
#else
	if( m_pTextResistCount )
	{
		m_pTextResistCount->SetText( GetSystem()->Get_ResistItemCount() );
		m_pTextResistCount->SetVisible(GetSystem()->Get_ResistItemEnable());
	}
#endif

	if( m_bArrowShow )
	{			
		if( m_ptArrowPos.y > DEFAULT_ARROW_POSY + 12 )
		{			
			m_nOper *= -1;
			m_ptArrowPos.y = DEFAULT_ARROW_POSY + 12;
		}
		else if( m_ptArrowPos.y < DEFAULT_ARROW_POSY )
		{		
			m_nOper *= -1;
			m_ptArrowPos.y = DEFAULT_ARROW_POSY;
		}

		m_ptArrowPos.y += ( g_fDeltaTime * (float)m_nOper * 20.0f );
		m_pArrow->SetPos( CsPoint( (int)m_ptArrowPos.x, (int)m_ptArrowPos.y ));
	}

	if( GetSystem() )
	{
		CsC_AvObject* pTarget = GetSystem()->GetTargetNpc();
		if( pTarget )
		{
			if( !pTarget->IsEnableObject() )
				return;

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
cDigitama_Scan::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

#ifdef UI_ITEMSCAN_RENEWAL
	if( m_pSkipBtn && m_pSkipBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;
	if( m_pScanCancelBtn && m_pScanCancelBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;
#else
	if(m_pBtnOCE && m_pBtnOCE->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}
#endif
	if(m_pCancelButton && m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pBtnAutoResist && m_pBtnAutoResist->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if( m_pGridBox && m_pGridBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cGridListBoxItem const* pOverItem = m_pGridBox->GetMouseOverItem();
		SAFE_POINTER_RETVAL( pOverItem, muReturn );

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		sScanItemInfo* pData = (sScanItemInfo*)pOverItem->GetUserData();
		SAFE_POINTER_RETVAL( pData, muReturn );

		if( 0 != pData->nItemType )
			pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 44, 44 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->nItemType, cBaseWindow::WT_DIGITAMA_SCAN );

		return muReturn;
	}

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

#ifdef SIMPLE_TOOLTIP
	if( GetSystem() && GetSystem()->Get_ResistItemEnable() )
	{
#ifdef UI_ITEMSCAN_RENEWAL
		if( m_pScanCancelBtn && m_pScanCancelBtn->Update_ForMouse() == cButton::ACTION_ON )
#else
		if(m_pBtnOCE && m_pBtnOCE->Update_ForMouse() == cButton::ACTION_ON )
#endif
		{
			cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
			SAFE_POINTER_RETVAL(pToolTip, muReturn);

			pToolTip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1012 );

			return muReturn;
		}
	}

	if(m_pBtnAutoResist && m_pBtnAutoResist->Update_ForMouse() == cButton::ACTION_ON )
	{
		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		pToolTip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1011 );

		return muReturn;
	}

	if( m_rcReqbit.PtInRect( CURSOR_ST.GetPos() ) )
	{
		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		pToolTip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1013 );

		return muReturn;
	}
#endif


	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 255, 27 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cDigitama_Scan::Render()
{
	RenderScript();
}

bool cDigitama_Scan::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
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
			if( pItemInfo->GetCount() != nIconCount )
			{
				cPrintMsg::PrintMsg( 20030 );
			}
			else 
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

void cDigitama_Scan::ResetDevice()
{	
	ResetDeviceScript();
}

void cDigitama_Scan::PreResetMap()
{
	_ProcessExit();
}

bool cDigitama_Scan::OnEscapeKey()
{
	_ProcessExit();
	return true; 
}

void cDigitama_Scan::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cDigitama_Scan::PressOKCancelExitBtn( void* pSender, void* pData )
{
	SAFE_POINTER_RET( m_pBtnAutoResist );
	SAFE_POINTER_RET( m_pBtnOCE );
	sButtonInfo* pBtInfo = dynamic_cast<sButtonInfo*>(m_pBtnOCE->GetUserData());
	SAFE_POINTER_RET( pBtInfo );
	switch( pBtInfo->eState )
	{
	case sButtonInfo::BTS_OK:
		m_pBtnAutoResist->SetEnable( false );
		GetSystem()->ProcessStart();
		break;
	case sButtonInfo::BTS_CANCEL:
		GetSystem()->ProcessCancel();
		break;
	case sButtonInfo::BTS_EXIT:
		_ProcessExit();
		break;
	}
}

void cDigitama_Scan::PressCancelButton( void* pSender, void* pData )
{
	_ProcessExit();
}

void cDigitama_Scan::PressAutoResistButton( void* pSender, void* pData )
{
	SetArrowShow( false );
	GetSystem()->AutoRegisterScan(); 
}

/////////////////////////////////////////////////////////////////////

void cDigitama_Scan::SetArrowShow( bool bSet )
{ 	
	m_bArrowShow = bSet;
	SAFE_POINTER_RET( m_pArrow );
	m_pArrow->SetVisible(m_bArrowShow);
}

void cDigitama_Scan::_ProcessFail()
{
	SetEnableWindow( true );

	_ChangeButton( sButtonInfo::BTS_OK );

	SAFE_POINTER_RET( m_pBtnAutoResist );
	m_pBtnAutoResist->SetEnable( true );

	_SetProgressBar( false );
}

void cDigitama_Scan::_ProcessSuccess(void* pSender, void* pData)
{
	m_RegisterItem.DecreaseItem( 1 );
	SetEnableWindow( false );
	GetSystem()->ProcessSuccess();
#ifndef UI_ITEMSCAN_RENEWAL
	cWindow::PlaySound( "System\\System_return_01.wav" );
#endif
}
#ifdef UI_ITEMSCAN_RENEWAL
void cDigitama_Scan::_ProcessCancel()
{
	SetEnableWindow( true );

	_SetProgressBar( false );

	if( m_pBtnAutoResist )
		m_pBtnAutoResist->SetEnable( true );

	if( !m_RegisterItem.IsEnable() )
		return;

	_ChangeButton( sButtonInfo::BTS_OK );
}
#else
void cDigitama_Scan::_ProcessCancel()
{
	_ChangeButton( sButtonInfo::BTS_OK );

	SAFE_POINTER_RET( m_pBtnAutoResist );
	m_pBtnAutoResist->SetEnable( true );

	_SetProgressBar( false );
}
#endif

void cDigitama_Scan::_ProcessStart()
{
	SetEnableWindow( true );

	_ChangeButton( sButtonInfo::BTS_CANCEL );

	SAFE_POINTER_RET( m_pBtnAutoResist );
	m_pBtnAutoResist->SetEnable( false );

	_SetProgressBar( true );

	cWindow::PlaySound( "System\\System_return_01.wav" );
}

void cDigitama_Scan::_ProcessEnd()
{
	SetEnableWindow( true );
#ifdef UI_ITEMSCAN_RENEWAL
	_ChangeButton( sButtonInfo::BTS_CANCEL );
#else
	_ChangeButton( sButtonInfo::BTS_EXIT );
	SAFE_POINTER_RET( m_pTextResistCount );
	m_pTextResistCount->SetVisible( false );
#endif
	SAFE_POINTER_RET( m_pBtnAutoResist );
	m_pBtnAutoResist->SetEnable( true );

	_SetProgressBar( false );

	SAFE_POINTER_RET( m_pMoneyIF );
	m_pMoneyIF->SetMoney( static_cast<u8>(0) );

	m_RegisterItem.ChangeItem( ICONITEM::ITEM_ICON, 0 );
}

#ifdef UI_ITEMSCAN_RENEWAL
void cDigitama_Scan::_ProcessRegister(u8 nMoney, uint nCount, uint nType)
{
	// 버튼 변경
	_ChangeButton( sButtonInfo::BTS_OK );

	SAFE_POINTER_RET( m_pMoneyIF );
	m_pMoneyIF->SetMoney( nMoney );

	m_RegisterItem.ChangeItem( ICONITEM::ITEM_ICON, nType, nCount );

	cWindow::PlaySound( "System\\interface_person_01.wav" );
}
#else
void cDigitama_Scan::_ProcessRegister(u8 nMoney, uint nCount, uint nType)
{
	// 버튼 변경
	_ChangeButton( sButtonInfo::BTS_OK );

	SAFE_POINTER_RET( m_pTextResistCount );
	m_pTextResistCount->SetText( nCount );
	m_pTextResistCount->SetVisible( true );

	SAFE_POINTER_RET( m_pMoneyIF );
	m_pMoneyIF->SetMoney( nMoney );

	m_RegisterItem.ChangeItem( ICONITEM::ITEM_ICON, nType, nCount );

	cWindow::PlaySound( "System\\interface_person_01.wav" );
}
#endif

void cDigitama_Scan::_ProcessExit()
{
	UINT nWindowType = GetWindowType();
	GetSystem()->CloseDigitama( &nWindowType );
}

cItemInfo* cDigitama_Scan::GetResistItem()
{
	return GetSystem()->Get_ResistItem_Ptr(); 
}

void cDigitama_Scan::_MakeEmptyGridBox(int nItemCount)
{
	SAFE_POINTER_RET( m_pGridBox );
	m_pGridBox->RemoveAllItem();

	for( int i = 0; i < nItemCount; ++i )
	{
		// empty grid item
		_AddGridItem( i );
	}
}

#ifdef UI_ITEMSCAN_RENEWAL
void cDigitama_Scan::_AddGridItem(int nIndex)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	sScanItemCon sScanItem;

	// 아이템 슬롯
	cSprite* pSlot = NiNew cSprite;
	if( pSlot )
	{
		pSlot->Init( NULL, CsPoint::ZERO, CsPoint( 34, 34 ), "Scan_New\\Scan_slot_icon.tga", false );
		sScanItem.pSlotImg = pControl->AddSprite( pSlot, CsPoint( -1, -1 ) );
		if( sScanItem.pSlotImg )
		{
			sScanItem.pSlotImg->SetAutoPointerDelete(true);
			sScanItem.pSlotImg->SetVisible(false);
		}
	}

	// 아이템 아이콘
	sScanItem.pItemIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, 0, 1, CsPoint::ZERO );
	if( sScanItem.pItemIcon )
		sScanItem.pItemIcon->SetAutoPointerDelete(true);

	// 등급 마스크
	cSprite* pRank = NiNew cSprite;
	if( pRank )
	{
		pRank->Init( NULL, CsPoint::ZERO, CsPoint( 36, 36 ), "Scan_New\\Scan_glow_V02.tga", false );
		sScanItem.pRankImg = pControl->AddSprite( pRank, CsPoint( -2, -2 ) );
		if( sScanItem.pRankImg )
		{
			sScanItem.pRankImg->SetAutoPointerDelete(true);
			sScanItem.pRankImg->SetVisible(false);
		}
	}

	sScanItem.pItemControl = NiNew cGridListBoxItem( nIndex, CsPoint( 36, 36 ), true );
	if( sScanItem.pItemControl )
	{
		sScanItem.pItemControl->SetItem( pControl );
		sScanItem.pItemControl->SetUserData( new sScanItemInfo( 0 ) );
	}
	m_pGridBox->AddItem( sScanItem.pItemControl );
	m_lScanItems.push_back( sScanItem );
}
#else
void cDigitama_Scan::_AddGridItem(int nIndex)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	sScanItemCon sScanItem;

	// 아이템 아이콘
	sScanItem.pItemIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, 0, 1, CsPoint::ZERO );
	if( sScanItem.pItemIcon )
		sScanItem.pItemIcon->SetAutoPointerDelete(true);

	sScanItem.pItemControl = NiNew cGridListBoxItem( nIndex, CsPoint( 34, 34 ) );
	if( sScanItem.pItemControl )
	{
		sScanItem.pItemControl->SetItem( pControl );
		sScanItem.pItemControl->SetUserData( new sScanItemInfo( 0 ) );
	}

	m_pGridBox->AddItem( sScanItem.pItemControl );

	m_lScanItems.push_back( sScanItem );
}
#endif //UI_ITEMSCAN_RENEWAL

#ifdef UI_ITEMSCAN_RENEWAL
void cDigitama_Scan::_UpdateScanItem(uint nScanType)
{
	std::list< cItemInfo >* pScanned = GetSystem()->GetScanItem();
	SAFE_POINTER_RET( pScanned );

	// 스캔된 아이템이 없다
	if( pScanned->empty() )
	{
		_ResetRegistItem();
		return;
	}

	// 스캔된 아이템으로 변경
	std::list< sScanItemCon >::iterator itScan = m_lScanItems.begin();
	std::list< cItemInfo >::iterator it = pScanned->begin();
	std::list< cItemInfo >::iterator itEnd = pScanned->end();
	for( ; it != itEnd; ++it, ++itScan )
	{
		if( itScan == m_lScanItems.end() )
			break;

		int nItemRank = GetSystem()->GetItemRank( nScanType, it->GetType() );
		itScan->ChangeData( ICONITEM::ITEM_ICON, it->GetType(), it->GetCount(), nItemRank );
	}
}
#else
void cDigitama_Scan::_UpdateScanItem()
{
	std::list< cItemInfo >* pScanned = GetSystem()->GetScanItem();
	SAFE_POINTER_RET( pScanned );

	// 스캔된 아이템이 없다
	if( pScanned->empty() )
	{
		_ResetRegistItem();
		return;
	}

	// 스캔된 아이템으로 변경
	std::list< sScanItemCon >::iterator itScan = m_lScanItems.begin();
	std::list< cItemInfo >::iterator it = pScanned->begin();
	std::list< cItemInfo >::iterator itEnd = pScanned->end();
	for( ; it != itEnd; ++it, ++itScan )
	{
		if( itScan == m_lScanItems.end() )
			break;
		itScan->ChangeData( ICONITEM::ITEM_ICON, it->GetType(), it->GetCount() );
	}
}
#endif


void cDigitama_Scan::_ResetRegistItem()
{
	std::list< sScanItemCon >::iterator itScan = m_lScanItems.begin();
	for( ; itScan != m_lScanItems.end(); ++itScan  )
	{
		itScan->ResetData();
	}
}

#ifdef UI_ITEMSCAN_RENEWAL
void cDigitama_Scan::_ChangeButton(sButtonInfo::eBtnState eState)
{
	SAFE_POINTER_RET( m_pScanCancelBtn );
	sButtonInfo* pBtInfo = dynamic_cast<sButtonInfo*>(m_pScanCancelBtn->GetUserData());
	SAFE_POINTER_RET( pBtInfo );

	std::wstring wsBtnTxt;
	switch( eState )
	{
	case sButtonInfo::BTS_OK:
		pBtInfo->eState = sButtonInfo::BTS_OK;
		wsBtnTxt = UISTRING_TEXT( "GLOBAL_OK" );
		m_pScanCancelBtn->SetControlName( "ItemScanStartBtn" );
		break;
	case sButtonInfo::BTS_CANCEL:
		pBtInfo->eState = sButtonInfo::BTS_CANCEL;
		wsBtnTxt = UISTRING_TEXT( "GLOBAL_CANCEL" );
		m_pScanCancelBtn->SetControlName( "ItemScanCancelBtn" );
		break;
	}

	m_pScanCancelBtn->SetText( wsBtnTxt.c_str() );
}
#else
void cDigitama_Scan::_ChangeButton(sButtonInfo::eBtnState eState)
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
		m_pBtnOCE->SetControlName( "ItemScanStartBtn" );
		break;
	case sButtonInfo::BTS_CANCEL:
		pBtInfo->eState = sButtonInfo::BTS_CANCEL;
		wsBtnTxt = UISTRING_TEXT( "GLOBAL_CANCEL" );
		m_pBtnOCE->SetControlName( "ItemScanCancelBtn" );
		break;
	case sButtonInfo::BTS_EXIT:
		pBtInfo->eState = sButtonInfo::BTS_EXIT;
		wsBtnTxt = UISTRING_TEXT( "GLOBAL_EXIT" );
		m_pBtnOCE->SetControlName( "ItemScanExitBtn" );
		break;
	}

	m_pBtnOCE->SetText( wsBtnTxt.c_str() );
}
#endif

#ifdef UI_ITEMSCAN_RENEWAL
void cDigitama_Scan::_SetProgressBar(bool bIsPlay)
{
	if( m_pProgressBar && m_pProgressBgImg )
	{
		m_pProgressBgImg->SetVisible( bIsPlay );
		m_pProgressBar->SetVisible( bIsPlay );
		m_pProgressBar->SetTimer( 0.0f, IF_DIGITAMA_PROCESS_TIME );
	}

	if( bIsPlay )
		m_pProgressBar->Start();
	else
		m_pProgressBar->Stop();

	if( m_pMoneyIF )
		m_pMoneyIF->SetVisible( !bIsPlay );
}
#else
void cDigitama_Scan::_SetProgressBar(bool bIsPlay)
{
	SAFE_POINTER_RET( m_pProgressBar );
	m_pProgressBar->SetTimer( 0.0f, IF_DIGITAMA_PROCESS_TIME );
	if( bIsPlay )
		m_pProgressBar->Start();
	else
		m_pProgressBar->Stop();
}
#endif

void cDigitama_Scan::_AddRegisterItem()
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
#ifdef UI_ITEMSCAN_RENEWAL
void cDigitama_Scan::PressScanCancelButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pScanCancelBtn );
	sButtonInfo* pBtInfo = dynamic_cast<sButtonInfo*>(m_pScanCancelBtn->GetUserData());
	SAFE_POINTER_RET( pBtInfo );
	switch( pBtInfo->eState )
	{
	case sButtonInfo::BTS_OK:
		GetSystem()->ProcessStart();
		break;
	case sButtonInfo::BTS_CANCEL:
		GetSystem()->ProcessCancel();
		break;
	}
}

void cDigitama_Scan::PressSkipButton(void* pSender, void* pData)
{
	// 실행 중인 스캔 작업 중지
	if( m_pProgressBar && m_pProgressBar->IsPlay() )
	{
		_SetProgressBar( false );
		SetEnableWindow( false );
		GetSystem()->ProcessSkip();
	}
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef UI_ITEMSCAN_RENEWAL
void cDigitama_Scan::sScanItemCon::ChangeData(ICONITEM::eTYPE nIconType, uint nItemType, uint nItemCount, int nItemRank)
{
	if( pItemIcon )
	{
		pItemIcon->ChangeIcon( nIconType, nItemType, nItemCount, 0 );
		pItemIcon->SetVisible( true );
	}

	sScanItemInfo* pData = dynamic_cast<sScanItemInfo*>(pItemControl->GetUserData());
	if( pData )
		pData->nItemType = nItemType;

	if( pRankImg )
	{
		bool bIsVisible = false;
		switch( nItemRank )
		{
		case IF_SCAN_RANK_NORMAL:
			pRankImg->ChangeTexture( "" );
			break;
		case IF_SCAN_RANK_ELITE:
			pRankImg->ChangeTexture( "Scan_New\\Scan_glow_V02.tga" );
			bIsVisible = true;
			break;
		case IF_SCAN_RANK_UNIQUE:
			pRankImg->ChangeTexture( "Scan_New\\Scan_glow_Y02.tga" );
			bIsVisible = true;
			break;
		}
		pRankImg->SetVisible( bIsVisible );
	}
}
#else
void cDigitama_Scan::sScanItemCon::ChangeData(ICONITEM::eTYPE nIconType, uint nItemType, uint nItemCount)
{
	if( pItemIcon )
	{
		pItemIcon->ChangeIcon( nIconType, nItemType, nItemCount, 0 );
		pItemIcon->SetVisible( true );
	}

	sScanItemInfo* pData = dynamic_cast<sScanItemInfo*>(pItemControl->GetUserData());
	if( pData )
		pData->nItemType = nItemType;
}
#endif

void cDigitama_Scan::sScanItemCon::ResetData()
{
	if( pItemIcon )
		pItemIcon->SetVisible( false );
#ifdef UI_ITEMSCAN_RENEWAL
	if( pRankImg )
		pRankImg->SetVisible( false );
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cDigitama_Scan::sRegisterItem::ChangeItem(int nIconType, uint nType, int nCount /*= 1 */)
{
	if( pItemIcon )
	{
		pItemIcon->ChangeIcon( nIconType, nType, nCount, 0 );
		pItemIcon->SetVisible( true );
	}

	sRegistInfo* pInfo = dynamic_cast<sRegistInfo*>(pItemControl->GetUserData());
	if( pInfo )
		pInfo->nItemType = nType;
}

void cDigitama_Scan::sRegisterItem::DecreaseItem(int nCount)
{
	if( pItemIcon )
	{
		pItemIcon->s_nIndex_2 -= nCount;
		if( 0 == pItemIcon->s_nIndex_2 )
			pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, 0 );
	}
}

bool cDigitama_Scan::sRegisterItem::IsEnable()
{
	if( 0 == pItemIcon->s_nIndex_1 )
		return false;

	return true;
}

void cDigitama_Scan::sRegisterItem::Reset()
{
	if( pItemIcon )
	{
		pItemIcon->s_nIndex_2 = 0;
		pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, 0 );
	}
}
