#include "stdafx.h"
#include "NewCashshopStorageViewer.h"

NewCashshopStorageViewer::NewCashshopStorageViewer()
:m_pkItems(NULL),m_pCancelButton(NULL),m_pCashRefreshBtn(NULL),m_pCashShopBtn(NULL)
{
}

NewCashshopStorageViewer::~NewCashshopStorageViewer()
{
	Destroy();
}

void NewCashshopStorageViewer::Destroy()
{
	cBaseWindow::Delete();
}

void NewCashshopStorageViewer::DeleteResource()
{
	DeleteScript();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void NewCashshopStorageViewer::Create( int nValue /*=0*/ )
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "Cash\\Win.tga", CsPoint( 150, 150 ) , CsPoint( 266, 356 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );

	m_pCancelButton = AddButton( CsPoint( 244, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	if( m_pCancelButton )
		m_pCancelButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &NewCashshopStorageViewer::OnClick_CloseWindow );
	m_pCashRefreshBtn = AddButton( CsPoint( 166, 392-74 ), CsPoint( 73, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "CASHSHOP_STORAGE_BTN_REFRESH" ).c_str() );
	if( m_pCashRefreshBtn )
		m_pCashRefreshBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &NewCashshopStorageViewer::OnClick_RefreshCashStorage );

	m_pCashShopBtn = AddButton( CsPoint( 21, 386-74 ), CsPoint( 41, 41 ), CsPoint( 41, 0 ), "Cash\\CashWeb.tga" );
	if( m_pCashShopBtn )
		m_pCashShopBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &NewCashshopStorageViewer::OnClick_CashshopOpen);

	m_pkItems = NiNew cGridListBox;
	m_pkItems->Init( GetRoot(), CsPoint(7,37), CsPoint(252,252), 
		CsPoint::ZERO, CsPoint(36,36), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
	AddChildControl(m_pkItems);
	m_pkItems->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &NewCashshopStorageViewer::OnSelected_Item );
	m_pkItems->SetUsePressedMoveEvent(true);
	m_pkItems->SetMouseOverImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
	//m_pkItems->SetSelectedImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
	m_pkItems->SetBackOverAndSelectedImgRender(false);
	m_pkItems->SetUsePressedMoveEvent(true);
// 	cScrollBar* pScrollBar = NiNew cScrollBar;
// 	if(pScrollBar)
// 	{
// 		pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint::ZERO, CsPoint(16,252), 
// 			cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint(16,252) ), 2, false );
// 		m_pkItems->SetScrollBar( pScrollBar );
// 	}
	ShowCashStorage();
}

void NewCashshopStorageViewer::OnClick_CloseWindow(void* pSender, void* pData)
{
	GetSystem()->ToggleCashWarehouse(NULL);
}

void NewCashshopStorageViewer::OnClick_RefreshCashStorage(void* pSender, void* pData)
{
	BHPRT("OnClick_ShowCashStorage");
	GetSystem()->Req_RefreshCashStorage();
}

void NewCashshopStorageViewer::OnClick_CashshopOpen(void* pSender, void* pData)
{
	BHPRT("OnClick_ShowCashStorage");
	GetSystem()->Req_ShowCashshop(NULL);
}

void NewCashshopStorageViewer::Update(float const& fDeltaTime)
{
	if( (g_pGameIF && g_pGameIF->IsOpenNpcWindow()) || (g_pCharMng && g_pCharMng->GetDigimonUser( 0 )->IsBattle()) )
	{
		OnClick_CloseWindow(NULL,NULL);
		cPrintMsg::PrintMsg( 20091 );
		return;
	}

	_UpdateMoveWindow();
}

void NewCashshopStorageViewer::Render()
{
	RenderScript();
}

void NewCashshopStorageViewer::ResetDevice()
{
	ResetDeviceScript();
}

cBaseWindow::eMU_TYPE NewCashshopStorageViewer::Update_ForMouse(void)
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pCancelButton && m_pCancelButton->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pCashRefreshBtn && m_pCashRefreshBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pCashShopBtn && m_pCashShopBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if(m_pkItems && m_pkItems->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cGridListBoxItem const* pOverGrid = m_pkItems->GetMouseOverItem();
		if(pOverGrid)
		{
			cItemInfo* pkItemInfo = GetSystem()->GetCashStorage(pOverGrid->getID());
			if( pkItemInfo && pkItemInfo->IsEnable() )
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pOverGrid->GetWorldPos(), CsPoint(32,32), 310, cTooltip::ITEM, pkItemInfo->GetType(), cBaseWindow::WT_NEW_CASH_WAREHOUSE, cTooltip::OPEN_SLOT, 0, pkItemInfo);
		}
		return muReturn;
	}

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void	NewCashshopStorageViewer::OnSelected_Item(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	StorageItemInfo* pkStorageInfo =	dynamic_cast<StorageItemInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET(pkStorageInfo);

	BHPRT("OnSelected_CartItem [%d]", pkStorageInfo->id);

	if( 0 == pkStorageInfo->id )
		return;

	GetSystem()->CheckCropCashItem( pItem->getID(), pkStorageInfo->id );
}

void	NewCashshopStorageViewer::ShowCashStorage()
{
	SAFE_POINTER_RET( m_pkItems );

	const NewCashshopContents::CashStorageItems & pkCashStorage = GetSystem()->GetCashStorage();

	BHPRT("ShowCashStorage [%d]", pkCashStorage.size());

	m_pkItems->RemoveAllItem();
	
	NewCashshopContents::CashStorageConstIter it = pkCashStorage.begin();
	for( int n = 0; it != pkCashStorage.end(); ++it, ++n )
	{
		SAFE_POINTER_CON( (*it) );

		cString * pItem = NiNew cString;

// 		cSprite* pImage = NiNew cSprite;	// 0
// 		pImage->Init( NULL, CsPoint::ZERO, CsPoint(36,36),  "Cash\\NewCashShop\\Store_bg_slot.tga", false );
// 		cString::sSPRITE* sSprite = pItem->AddSprite( pImage, CsPoint::ZERO, CsPoint(36,36) );	
// 		if( sSprite )
// 			sSprite->SetAutoPointerDelete(true);

		cString::sICON* pIcon = pItem->AddIcon( CsPoint(32,32), ICONITEM::ARMOR_1, (*it)->GetType(), (*it)->GetCount(), CsPoint(2,2) );
		if( pIcon )
		{
			pIcon->SetAutoPointerDelete(true);
			if( !(*it)->IsEnable() )
				pIcon->SetVisible(false);
		}

		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(n, CsPoint( 32, 32 ));
		addItem->SetItem( pItem );

		addItem->SetUserData( new StorageItemInfo( (*it)->GetType() ) );
		m_pkItems->AddItem( addItem );
	}
}

void NewCashshopStorageViewer::_UpdateStorageSlot( int const& nUpdateSlotNum )
{
	SAFE_POINTER_RET(m_pkItems);
	
	cGridListBoxItem const * pFindItem = m_pkItems->GetItemFormGridItemID(nUpdateSlotNum);
	SAFE_POINTER_RET(pFindItem);

	cItemInfo* pSlotItem = GetSystem()->GetCashStorageItem( nUpdateSlotNum );
	SAFE_POINTER_RET(pSlotItem);

	bool bVisible = true;
	if( !pSlotItem->IsEnable() )// 빈 슬롯
		bVisible = false;

	StorageItemInfo* pItemInfo = dynamic_cast< StorageItemInfo* >(pFindItem->GetUserData());
	if( pItemInfo )
		pItemInfo->id = pSlotItem->GetType();

	cString* pItems = pFindItem->GetItem();
	if( pItems )
	{
		cString::sELEMENT* pIcon = pItems->GetElement(0);
		if( pIcon )
			pIcon->SetVisible(bVisible);
	}	
}


void NewCashshopStorageViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case NewCashshopContents::Update_Cash_Storage:
		ShowCashStorage();
		break;
	case NewCashshopContents::Update_Cash_Storage_Item:
		{
			int nUpdateSlotNum = -1;
			kStream >> nUpdateSlotNum;
			_UpdateStorageSlot( nUpdateSlotNum );
		}
		break;
	}
}

bool NewCashshopStorageViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::Update_Cash_Storage, this );
	GetSystem()->Register( SystemType::Update_Cash_Storage_Item, this );
	return true;
}
