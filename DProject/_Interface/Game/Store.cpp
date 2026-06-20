 
#include "stdafx.h"
#include "Store.h"

cStore::cStore() : 
m_pCancelButton( NULL ),
m_pStoreItemListBox( NULL),
m_pRadioBtn( NULL),
m_pCurrentMoney( NULL )
{
}

cStore::~cStore()
{
	Destroy();
}

bool cStore::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eEnable_Window, this );
	GetSystem()->Register( SystemType::eSet_StoreItemList, this );
	GetSystem()->Register( SystemType::eSet_RepurchaseList, this );
	GetSystem()->Register( SystemType::eDelete_Repurchase, this );
	GetSystem()->Register( SystemType::eAdd_Repurchase, this );
	GetSystem()->Register( SystemType::eUpdate_Currency, this );

	return true;
}

void cStore::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eEnable_Window:
		{
			SetEnableWindow( true );
		}break;
	case SystemType::eSet_StoreItemList:
		{
			MakeStoreItemList();
		}break;

	case SystemType::eSet_RepurchaseList:
		{
			MakeRepurchaseItemList();
		}break;
	case SystemType::eDelete_Repurchase:
		{
			int nItemTypeID(0);
			int nBuyCount(0);
			kStream >> nItemTypeID >> nBuyCount;
			DeleteRepurchaseItem( nItemTypeID, nBuyCount );
		}break;
	case SystemType::eAdd_Repurchase:
		{
			int nItemTypeID(0);
			int nCount(0);
			kStream >> nItemTypeID >> nCount;
			AddRepurchaseItem( nItemTypeID, nCount );
		}break;
	case SystemType::eUpdate_Currency:
		{
			UpdateMyCurrencyValue();
		}break;
	}
}

void cStore::Destroy()
{
	cBaseWindow::Delete();	
}

void cStore::DeleteResource()
{
	if( GetSystem() )
	{
		GetSystem()->UnRegisterAll( this );
		GetSystem()->CloseStoreWindow();
	}

	cItemSeparate::DeleteInstance( GetWindowType(), 0 );

	DeleteScript();

	SAFE_NIDELETE( m_pCurrentMoney );
}

void cStore::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "Store_G\\Win_Store.tga", CsPoint( 150, 105 ), CsPoint( 333, 541 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );

	m_pCancelButton = AddButton( CsPoint( 311, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga");
	SAFE_POINTER_RET(m_pCancelButton);
	m_pCancelButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cStore::CloseStoreWindow );

	AddStoreWindowTitle( nValue );

	cScrollBar* pScrollBar = NiNew cScrollBar;	
	pScrollBar->Init( cScrollBar::TYPE_1, NULL, CsPoint( 310, 75 ), CsPoint( 16, 431 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 9, 77 ), CsPoint( 304, 504 ) ), 6, false );

	// 제작 아이템 리스트
	m_pStoreItemListBox = NiNew cListBox;
	m_pStoreItemListBox->Init( GetRoot(), CsPoint(9, 77), CsPoint( 314, 419 ), NULL, false );
	m_pStoreItemListBox->SetScrollBar( pScrollBar );
	m_pStoreItemListBox->AddEvent( cListBox::LIST_SELECTED_ITEM, this, &cStore::ClickStoreItemButton );
	m_pStoreItemListBox->AddEvent( cListBox::LIST_SELECT_CHANGE_ITEM, this, &cStore::ClickStoreItemButton );
	m_pStoreItemListBox->SetItemGab( CsPoint(0, 6) );
	m_pStoreItemListBox->SetMargin( CsPoint(11, 11) );
	AddChildControl( m_pStoreItemListBox );

	CreateItemTypeRadioButton();

	UpdateMyCurrencyValue();
}

void cStore::Update(float const& fDeltaTime)
{
	CsC_AvObject* pNpc = GetSystem()->GetInteractNPC();
	SAFE_POINTER_RET( pNpc );
	g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_STORE );

	// 테이머와의 거리 체크
	assert_cs( pNpc->GetLeafRTTI() == RTTI_NPC );
	if( ( (CNpc*)pNpc )->CheckTamerDist() == false )
	{
		if( cItemSeparate::IsInstance( GetWindowType(), 0 ) )
			SetEnableWindow( true );

		Close();
		return;
	}

	cItemSeparate::UpdateInstance( GetWindowType(), 0 );
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE	cStore::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cItemSeparate::Update_ForMouseInstance( GetWindowType(), 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	// 캔슬 버튼
 	if( m_pCancelButton->Update_ForMouse() )
 		return muReturn;

	if( m_pStoreItemListBox && m_pStoreItemListBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		CheckItemIconMouseOver();
		CheckCurrencyIconMouseOver();
		return muReturn;
	}

	if( m_pRadioBtn->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 255, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();		

	return muReturn;
}

cBaseWindow::eMU_TYPE	cStore::Update_ForMouse_Disable()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cItemSeparate::Update_ForMouseInstance( GetWindowType(), 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	return cBaseWindow::Update_ForMouse_Disable();
}

void cStore::Render()
{
	RenderScript();

	if(m_pCurrentMoney)
		m_pCurrentMoney->Render( CsPoint( 160, 512 ) + GetRootClient() );

	EndRenderScript();

	cItemSeparate::RenderInstance( GetWindowType(), 0 );
}

bool cStore::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	assert_cs( IsInWindow( CURSOR_ST.GetPos() ) == true );

	CsRect rc( CsPoint( 11, 36 ), CsSIZE( 205, 345 ) );
	if( rc.PtInRect( MousePosToWindowPos( CURSOR_ST.GetPos() ) ) == TRUE )
	{
		switch( eIconType )
		{
		case CURSOR_ICON::CI_INVEN:
			if( GetSystem()->IsEnableSellItem( nIconSlot ) )
			{
				cData_Inven* pInven = GetSystem()->GetInventory();
				SAFE_POINTER_RETVAL(pInven, false);

				cItemInfo* pItemInfo = pInven->GetData( nIconSlot );
				SAFE_POINTER_RETVAL(pItemInfo, false);

				CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() );
				SAFE_POINTER_RETVAL(pItem, false);

				CsItem::sINFO* pFTItem = pItem->GetInfo();
				SAFE_POINTER_RETVAL(pFTItem, false);

				sMONEY money( pFTItem->s_dwSale * nIconCount );
				cPrintMsg::PrintMsg( 11028, pFTItem->s_szName, &money );
				cMessageBox::GetFirstMessageBox()->SetValue1( nIconSlot );
				cMessageBox::GetFirstMessageBox()->SetValue2( nIconCount );
			}
			break;
		default:
			cPrintMsg::PrintMsg( 11018 );
		}
		CURSOR_ST.ReleaseIcon();
		return true;
	}
	return false;
}

void cStore::ResetDevice()
{	
	ResetDeviceScript();
}

void cStore::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cStore::ClickStoreItemButton( void* pSender, void* pData )
{
	SAFE_POINTER_RET( GetSystem() );
	SAFE_POINTER_RET( pData );

	cListBoxItem* pItem = static_cast<cListBoxItem*>(pData);
	sStoreItemInfo* pItemInfo = dynamic_cast<sStoreItemInfo*>(pItem->GetUserData());

	int nCustomIndex = pItemInfo->m_nIndex;
	int nItemID		 = pItemInfo->m_nItemID;
	int nItemCount	 = pItemInfo->m_nItemShowCount;

	GetSystem()->SetBuyButtonIndex( nCustomIndex );

	if(GetSelectedSellType() == REPURCHASE_ITEM_SELLCLASS)
	{
		SAFE_POINTER_RET( m_pStoreItemListBox );
		GetSystem()->RequestStoreItemRepurchase( m_pStoreItemListBox->GetItemIndex(pItem), nItemID, nItemCount );
	}
	
	else // 일반 구매탭 일때
		GetSystem()->RequestNormalItemBuy( nItemID );
}

void cStore::ClickSellTypeTab( void* pSender, void* pData )
{
	SAFE_POINTER_RET(pData);
	cRadioButton* pRadioButton = static_cast<cRadioButton*>(pData);
	SAFE_POINTER_RET(pRadioButton);
	sStoreSellType* userData = dynamic_cast<sStoreSellType*>(pRadioButton->GetUserData());
	SAFE_POINTER_RET(userData);

	if( userData->m_nSellType == REPURCHASE_ITEM_SELLCLASS ) // 재구매 탭 클릭
		MakeRepurchaseItemList();
	
	else // 일반 구매탭
		MakeStoreItemList( userData->m_nSellType );

	cWindow::PlaySound( cWindow::SD_At2 );
}

void cStore::UpdateMyCurrencyValue()
{
	if( m_pCurrentMoney == NULL )
		m_pCurrentMoney = NiNew cMoneyIF;

	SAFE_POINTER_RET( m_pCurrentMoney );

	switch( GetSystem()->GetInteractNPCType() )
	{
	case nsCsNpcTable::NT_TRADE:		
	case nsCsNpcTable::NT_GUILD:
		{
			cData_Inven* pInven = GetSystem()->GetInventory();
			SAFE_POINTER_RET(pInven);
			m_pCurrentMoney->Init( cMoneyIF::DISP_FULL, pInven->GetMoney(), true );
		}
		break;
	case nsCsNpcTable::NT_DIGICORE:
		{
			cData_Inven* pInven = GetSystem()->GetInventory();
			SAFE_POINTER_RET(pInven);
			sMONEY money = pInven->GetCount_Item_TypeLS( DIGICORE_ITEM_TYPE_L, 0 );
			m_pCurrentMoney->Init( cMoneyIF::DISP_DIGICORE, money, true );
		}
		break;
	case nsCsNpcTable::NT_EVENT_STORE:
		{
			CStoreContents::BaseStoreItemData* pItemData = GetSystem()->GetStoreItemData( 0 );
			SAFE_POINTER_RET(pItemData);

			CStoreContents::EventStoreItemData* pEventData = static_cast<CStoreContents::EventStoreItemData*>(pItemData);
			SAFE_POINTER_RET(pEventData);

			const cData_Inven* pInven = GetSystem()->GetInventory();
			SAFE_POINTER_RET(pInven);
			sMONEY money = pInven->GetCount_Item_ID( pEventData->m_nEventItemType );
			m_pCurrentMoney->Init_ForEventStore( pEventData->m_nEventItemType, cMoneyIF::DISP_EVENT, money, true );
		}
		break;
	case nsCsNpcTable::NT_PROPERTY_STORE: // 속성상점에서는 표시 하지 않는다.
		{
			SAFE_NIDELETE(m_pCurrentMoney);
		}
		break;
	default:
		assert_cs( false );
		break;
	}

}
// 상점 판매 목록 세팅
void cStore::MakeStoreItemList( int nSellType )
{
	SAFE_POINTER_RET( m_pStoreItemListBox );
	m_pStoreItemListBox->RemoveAllItem(); // 이전 리스트 박스 삭제

	if( nSellType == 0) // 최초 설정
		nSellType = GetSelectedSellType();

	std::list<CStoreContents::BaseStoreItemData*> StoreItemList = GetSystem()->GetStoreItemDataList();
	std::list<CStoreContents::BaseStoreItemData*>::iterator iter = StoreItemList.begin();
	std::list<CStoreContents::BaseStoreItemData*>::iterator enditer = StoreItemList.end();

	for( int nIndex = 0; iter != enditer; ++iter, ++nIndex )
	{
		CStoreContents::BaseStoreItemData* pData = (*iter);
		SAFE_POINTER_BEK(pData);

		if( pData->m_nSellType == nSellType )
		{
			cListBoxItem* pAddItem = NiNew cListBoxItem;
			SAFE_POINTER_BEK( pAddItem );
			cString * pItemString = NiNew cString;
			SAFE_POINTER_BEK( pItemString );
			cButton* pButton = NiNew cButton;
			SAFE_POINTER_BEK( pButton );
			pButton->Init( NULL, CsPoint::ZERO, CsPoint( 272, 63), "Store_G\\Win_Store_SelMask.tga", false );
			pButton->SetTexToken(CsPoint(0,63));

			pItemString->AddButton( pButton, nIndex, CsPoint::ZERO, CsPoint( 272, 63) );
			pItemString->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, pData->m_nItemID, 1/*세팅 개수*/, CsPoint( 14, 16 ) );
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
			ti.s_Color = g_pDataMng->GetItemColor( pData->m_nClass );
			ti.SetText( pData->m_strName.c_str() );
			pItemString->AddText( &ti, CsPoint( 60, 15 ) );
			AddMoneyInfo_ByStoreType( pItemString, pData );

			pAddItem->SetItem( pItemString );
			pAddItem->SetUserData( new sStoreItemInfo( nIndex, pData->m_nItemID, 1 ) );

			m_pStoreItemListBox->AddItem( pAddItem );
		}
	}
}
// 재구매 목록 세팅
void cStore::MakeRepurchaseItemList()
{
	if( GetSelectedSellType() != REPURCHASE_ITEM_SELLCLASS)
		return;

	SAFE_POINTER_RET( m_pStoreItemListBox );
	m_pStoreItemListBox->RemoveAllItem(); // 이전 리스트 박스 삭제

	const std::list< CStoreContents::BaseStoreItemData* >& RepurchaseItemList = GetSystem()->GetRepurchaseItemList();
	std::list<CStoreContents::BaseStoreItemData*>::const_reverse_iterator iter	= RepurchaseItemList.rbegin();
	std::list<CStoreContents::BaseStoreItemData*>::const_reverse_iterator enditer = RepurchaseItemList.rend();

	for( int nIndex = 0; iter != enditer; ++iter, ++nIndex )
	{
		CStoreContents::BaseStoreItemData* pData = (*iter);
		SAFE_POINTER_BEK(pData);

		cListBoxItem* pAddItem = NiNew cListBoxItem;
		SAFE_POINTER_BEK( pAddItem );

		cString * pItemString = NiNew cString;
		SAFE_POINTER_BEK( pItemString );

		cButton* pButton = NiNew cButton;
		SAFE_POINTER_BEK( pButton );

		pButton->Init( NULL, CsPoint::ZERO, CsPoint( 272, 63), "Store_G\\Win_Store_SelMask.tga", false );
		pButton->SetTexToken(CsPoint(0,63));

		pItemString->AddButton( pButton, nIndex, CsPoint::ZERO, CsPoint( 272, 63) );
		pItemString->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, pData->m_nItemID, pData->m_nItemShowCount/*세팅 개수*/, CsPoint( 14, 16 ) );
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		ti.s_Color = g_pDataMng->GetItemColor( pData->m_nClass );
		ti.SetText( pData->m_strName.c_str() );
		pItemString->AddText( &ti, CsPoint( 60, 15 ) );
		pItemString->AddMoneyIF( cMoneyIF::DISP_FULL, pData->m_nSale * pData->m_nItemShowCount, FONT_WHITE,CFont::FS_9,DT_RIGHT, CsPoint( 115, 35 ), true );

		pAddItem->SetItem( pItemString );
		pAddItem->SetUserData( new sStoreItemInfo( 0, pData->m_nItemID, pData->m_nItemShowCount ) );

		m_pStoreItemListBox->AddItem( pAddItem );
	}
}

void cStore::DeleteRepurchaseItem( int nItemID, int nItemCount )
{
	if( GetSelectedSellType() != REPURCHASE_ITEM_SELLCLASS)
		return;

	SAFE_POINTER_RET(m_pStoreItemListBox);
	for( int i = 0; i < m_pStoreItemListBox->GetItemCount(); ++i )
	{
		 const cListBoxItem* pItem = m_pStoreItemListBox->GetItemFormPos( i );
		 SAFE_POINTER_CON(pItem);

		 sStoreItemInfo* pStoreInfo = dynamic_cast<sStoreItemInfo*>(pItem->GetUserData());
		 SAFE_POINTER_CON(pStoreInfo);

		 if(pStoreInfo->m_nItemID == nItemID && pStoreInfo->m_nItemShowCount == nItemCount )
		 {
			m_pStoreItemListBox->RemoveItem(i);
			return;
		 }
	}
}

void cStore::AddRepurchaseItem( int nItemID, int nItemCount )
{
	if( GetSelectedSellType() != REPURCHASE_ITEM_SELLCLASS)
		return;

	CStoreContents::BaseStoreItemData* pRepurchaseItem = GetSystem()->GetRepurchaseItemData( nItemID, nItemCount );
	SAFE_POINTER_RET(pRepurchaseItem);

	cListBoxItem* pAddItem = NiNew cListBoxItem;
	SAFE_POINTER_RET( pAddItem );

	cString * pItemString = NiNew cString;
	SAFE_POINTER_RET( pItemString );

	cButton* pButton = NiNew cButton;
	SAFE_POINTER_RET( pButton );

	pButton->Init( NULL, CsPoint::ZERO, CsPoint( 272, 63), "Store_G\\Win_Store_SelMask.tga", false );
	pButton->SetTexToken(CsPoint(0,63));

	pItemString->AddButton( pButton, 0, CsPoint::ZERO, CsPoint( 272, 63) );
	pItemString->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, pRepurchaseItem->m_nItemID, pRepurchaseItem->m_nItemShowCount/*세팅 개수*/, CsPoint( 14, 16 ) );
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_Color = g_pDataMng->GetItemColor( pRepurchaseItem->m_nClass );
	//ti.s_eTextAlign = DT_RIGHT;
	ti.SetText( pRepurchaseItem->m_strName.c_str() );
	pItemString->AddText( &ti, CsPoint( 60, 15 ) );
	pItemString->AddMoneyIF( cMoneyIF::DISP_FULL, pRepurchaseItem->m_nSale * pRepurchaseItem->m_nItemShowCount, FONT_WHITE,CFont::FS_9,DT_RIGHT, CsPoint( 115, 35 ), true );

	pAddItem->SetItem( pItemString );
	pAddItem->SetUserData( new sStoreItemInfo( 0, pRepurchaseItem->m_nItemID, pRepurchaseItem->m_nItemShowCount ) );

	m_pStoreItemListBox->InsertItem( 0, pAddItem );
}

void cStore::AddMoneyInfo_ByStoreType( cString* pString, CStoreContents::BaseStoreItemData* pData )
{
	SAFE_POINTER_RET( pString );

	CsPoint ptPos( 115, 35 );
	switch( GetSystem()->GetInteractNPCType() )
	{
	case nsCsNpcTable::NT_TRADE:		
	case nsCsNpcTable::NT_GUILD:
		{
			CStoreContents::NormalStoreItemData* pNormalData = static_cast<CStoreContents::NormalStoreItemData*>(pData);
			SAFE_POINTER_RET(pNormalData);
			pString->AddMoneyIF( cMoneyIF::DISP_FULL, pNormalData->m_nScanPrice, FONT_WHITE ,CFont::FS_9,DT_RIGHT, ptPos, true );
		}break;
	case nsCsNpcTable::NT_DIGICORE:
		{
			CStoreContents::DigicoreStoreItemData* pDigicoreData = static_cast<CStoreContents::DigicoreStoreItemData*>(pData);
			SAFE_POINTER_RET(pDigicoreData);
			pString->AddMoneyIF( cMoneyIF::DISP_DIGICORE, pDigicoreData->m_nDigiCorePrice, FONT_WHITE, CFont::FS_9,DT_RIGHT, ptPos, true );
		}break;
	case nsCsNpcTable::NT_EVENT_STORE:
		{
			CStoreContents::EventStoreItemData* pEventData = static_cast<CStoreContents::EventStoreItemData*>(pData);
			SAFE_POINTER_RET(pEventData);
			pString->AddMoneyIF( cMoneyIF::DISP_EVENT, pEventData->m_nEventItemPrice, FONT_WHITE, CFont::FS_9,DT_RIGHT, ptPos, true, pEventData->m_nEventItemType );
		}break;
	case nsCsNpcTable::NT_PROPERTY_STORE:
		{
			CStoreContents::PropertyStoreItemData* pPropertyData = static_cast<CStoreContents::PropertyStoreItemData*>(pData);
			SAFE_POINTER_RET(pPropertyData);

			CsC_AvObject* pNpc = GetSystem()->GetInteractNPC();
			SAFE_POINTER_RET(pNpc);
			
			DWORD dwNpcID = pNpc->GetFTID();

			std::list<CStoreContents::PropertyStoreItemData::NeedItemInfo> NeedItemList = pPropertyData->m_NeedItemsList;
			std::list<CStoreContents::PropertyStoreItemData::NeedItemInfo>::const_iterator iter	 = NeedItemList.begin();
			std::list<CStoreContents::PropertyStoreItemData::NeedItemInfo>::const_iterator enditer = NeedItemList.end();

			std::vector<int> vecItemID, vecItemPrice;
			for(; iter != enditer; ++iter)
			{
				vecItemID.push_back(iter->m_nNeedItemID);
				vecItemPrice.push_back(iter->m_nNeedItemCount);
			}

			pString->AddPropertyPrice( dwNpcID, vecItemID, vecItemPrice );
		}break;
	default:
		assert_cs( false );
		break;
	}
}

void cStore::CheckItemIconMouseOver()
{
	cListBoxItem const* pOverItem = m_pStoreItemListBox->GetMouseOverItem();
	SAFE_POINTER_RET(pOverItem);

	cString* pString = pOverItem->GetItem();
	SAFE_POINTER_RET(pString);

	cString::sELEMENT* pElement = pString->GetElement( 1 );
	SAFE_POINTER_RET( pElement );

	cString::sICON* pIcon = dynamic_cast<cString::sICON*>(pElement);
	SAFE_POINTER_RET(pIcon);

	bool bMouseOverd = pIcon->m_MouseOverCheckRect.PtInRect( CURSOR_ST.GetPos() );
	if( bMouseOverd ) // 판매 아이콘 툴팁
	{
		sStoreItemInfo* pInfo = dynamic_cast<sStoreItemInfo*>(pOverItem->GetUserData());	
		SAFE_POINTER_RET( pInfo );
		SAFE_POINTER_RET(g_pGameIF);
		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RET(pToolTip);

		CsPoint pos = CsPoint(pIcon->m_MouseOverCheckRect.left, pIcon->m_MouseOverCheckRect.top); 

		pToolTip->SetTooltip( pos, IF_STORE_ITEMICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->m_nItemID, cBaseWindow::WT_STORE );
		
		//////////////////////////////////////////////////////////
		//2017-01-19-nova 장착중인 아이템을 비교창에 출력
		int nItemType = nsCsFileTable::g_pItemMng->GetItem( pInfo->m_nItemID )->GetInfo()->s_nType_L;

		for( int i=0; i<nLimit::Equip; ++i )
		{
			cItemInfo* PlayerpData = g_pDataMng->GetTEquip()->GetData( i );

			if(PlayerpData->m_nType != 0)
			{
				CsItem::sINFO* pFTSrc = nsCsFileTable::g_pItemMng->GetItem( PlayerpData->GetType() )->GetInfo();

				if(nItemType == pFTSrc->s_nType_L)
					TOOLTIPMNG_STPTR->GetCompareTooltip()->SetCompareTooltip( pos, IF_STORE_ITEMICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, PlayerpData->GetType(), cBaseWindow::WT_STORE, cTooltip::OPEN_SLOT, 0, PlayerpData);
			}
		}
		//////////////////////////////////////////////////////////
	}
}

void cStore::CheckCurrencyIconMouseOver()
{
	cListBoxItem const* pOverItem = m_pStoreItemListBox->GetMouseOverItem();
	SAFE_POINTER_RET(pOverItem);

	cString* pString = pOverItem->GetItem();
	SAFE_POINTER_RET(pString);
	// 교환 아이템
	cString::sELEMENT* pMoneyElement = pString->GetElement( 3 );
	SAFE_POINTER_RET( pMoneyElement );

	cString::sMONEYIF* pMoney = dynamic_cast<cString::sMONEYIF*>(pMoneyElement);
	SAFE_POINTER_RET(pMoney);
	SAFE_POINTER_RET(pMoney->s_pMoney);

	cString* pMoneyString = dynamic_cast<cMoneyIF*>(pMoney->s_pMoney)->GetString();

	std::list< cString::sELEMENT* > ElementList;
	pMoneyString->FindElement( cString::sELEMENT::ICON, ElementList );

	std::list< cString::sELEMENT* >::const_iterator iter = ElementList.begin();
	std::list< cString::sELEMENT* >::const_iterator Eiter = ElementList.end();

	for( ; iter != Eiter; ++iter )
	{
		cString::sELEMENT* pElement = (*iter);
		SAFE_POINTER_RET(pElement);
		cString::sICON* pIcon = dynamic_cast<cString::sICON*>(pElement);
		SAFE_POINTER_RET(pIcon);

		bool bMouserOverd = pIcon->m_MouseOverCheckRect.PtInRect( CURSOR_ST.GetPos() );
		if( bMouserOverd ) // 마우스 오버 되었다면
		{
			SAFE_POINTER_RET(g_pGameIF);
			cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
			SAFE_POINTER_RET(pToolTip);

			CsPoint TootipPos( pIcon->m_MouseOverCheckRect.GetPos() );
			switch(GetSystem()->GetInteractNPCType())
			{
			case nsCsNpcTable::NT_DIGICORE:
				{
					int nDispID = nsCsFileTable::g_pItemMng->TypeLS_to_Disp( DIGICORE_ITEM_TYPE_L, 0 );
					pToolTip->SetTooltip( TootipPos, IF_STORE_ITEMICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, 
						nDispID, cBaseWindow::WT_STORE );
				}break;
			case nsCsNpcTable::NT_EVENT_STORE:
				{
					pToolTip->SetTooltip( TootipPos, IF_STORE_ITEMICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, 
						pIcon->s_nIndex_1, cBaseWindow::WT_STORE );
				}break;
			case nsCsNpcTable::NT_PROPERTY_STORE:
				{
					pToolTip->SetTooltip( TootipPos, IF_STORE_ITEMICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, 
						pIcon->s_nIndex_1, cBaseWindow::WT_STORE );
				}break;
			}
		}
	}
}

void cStore::CloseStoreWindow( void* pSender, void* pData )
{
	Close();
}

void cStore::CreateItemTypeRadioButton()
{
	// 아이템 목록
	std::list< CStoreContents::BaseStoreItemData* > StoreItemDataList = GetSystem()->GetStoreItemDataList();
	std::list< CStoreContents::BaseStoreItemData* >::iterator iter	= StoreItemDataList.begin();
	std::list< CStoreContents::BaseStoreItemData* >::iterator iterEnd	= StoreItemDataList.end();

	std::set< int > setProductType; // 판매 타입 임시 저장
	for( ; iter!=iterEnd; ++iter )
		setProductType.insert( (*iter)->m_nSellType );
	// 재구매 탭 등록
#ifdef REPURCHASE
	setProductType.insert( REPURCHASE_ITEM_SELLCLASS );
#endif

	CsPoint ptRadioBtnSize = CsPoint( 70, 24 );

	std::set<int>::const_iterator siter = setProductType.begin();
	std::set<int>::const_iterator sEnditer = setProductType.end();
	for( int i = 0; siter != sEnditer; ++i, ++siter ) // 라디오 버튼에 클릭 이벤트와 판매물품 타입을 세팅한다
	{
		if( i == 0 ) // 처음 등록
		{
			m_pRadioBtn = AddRadioButton( IF_STORE_RADIOBTN_POS, CsPoint( 70, 24 ), cCheckBox::IMAGE_RADIO_2, nsCsFileTable::g_pItemMng->GetCategory( *siter ), true );
		}
		else // 이후
		{
			if( *siter == REPURCHASE_ITEM_SELLCLASS )
				m_pRadioBtn->AddNode(  CsPoint( 10, 511 ), ptRadioBtnSize, cCheckBox::IMAGE_RADIO_2, nsCsFileTable::g_pItemMng->GetCategory( *siter ) );
			else
				m_pRadioBtn->AddNode( IF_STORE_RADIOBTN_POS + CsPoint( m_pRadioBtn->GetSize() * ( ptRadioBtnSize.x + 1 ), 0 ), ptRadioBtnSize, cCheckBox::IMAGE_RADIO_2,
				nsCsFileTable::g_pItemMng->GetCategory( *siter ) );
		}	

		cRadioButton* pRadioButton = m_pRadioBtn->GetNode( i );
		SAFE_POINTER_RET( pRadioButton );
		pRadioButton->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &cStore::ClickSellTypeTab );
		pRadioButton->SetUserData( new sStoreSellType( *siter ) );
	}
	m_pRadioBtn->SetCheckIndex( 0 );
}

void cStore::AddStoreWindowTitle( int nStoreType )
{
	switch( nStoreType )
	{
	case nsCsNpcTable::NT_TRADE:		
	case nsCsNpcTable::NT_GUILD:
		{			
			AddTitle( UISTRING_TEXT( "STORE_TITLE_SHOP" ).c_str() );			
		}
		break;
	case nsCsNpcTable::NT_DIGICORE:
		{
			AddTitle( UISTRING_TEXT( "STORE_TITLE_DIGICORE_SHOP" ).c_str() );
		}
		break;
	case nsCsNpcTable::NT_EVENT_STORE:
		{
			AddTitle( UISTRING_TEXT( "STORE_TITLE_EVENT_SHOP" ).c_str() );
		}
		break;
	case nsCsNpcTable::NT_PROPERTY_STORE:
		{
			AddTitle( UISTRING_TEXT( "STORE_TITLE_SHOP" ).c_str() );
		}
		break;
	default:
		assert_cs( false );
		break;
	}
}

int cStore::GetSelectedSellType() const
{
	const cRadioButton* pRadioButton = m_pRadioBtn->GetSelectedNode();
	SAFE_POINTER_RETVAL(pRadioButton, 0);
	sStoreSellType* pSellType = dynamic_cast<sStoreSellType*>(pRadioButton->GetUserData());
	SAFE_POINTER_RETVAL(pSellType, 0);
	return pSellType->m_nSellType;
}