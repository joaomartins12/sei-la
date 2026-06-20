#include "stdafx.h"
#include "PersonStore.h"

cPersonStore::cPersonStore() 
: m_pBGWin( NULL ), m_pBtnDynamicOK( NULL ), m_pCancelButton( NULL ),
m_pEditName( NULL ), m_pTexRate( NULL ), m_pTamerName( NULL ), m_pListBox( NULL )
{
#ifdef IME_WINDOW_POSITION
	g_bIMEPosition = true;
#endif
}

cPersonStore::~cPersonStore()
{	
}

bool cPersonStore::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eBtnDynamicOK, this );
	GetSystem()->Register( SystemType::eEnableWindow, this );
	GetSystem()->Register( SystemType::eDisableWindow, this );
	GetSystem()->Register( SystemType::eSetTamerName, this );
	GetSystem()->Register( SystemType::eSetStoreName, this );
	GetSystem()->Register( SystemType::eReady, this );
	GetSystem()->Register( SystemType::eOpen, this );
	GetSystem()->Register( SystemType::eUpdateItem, this );

	return true;
}

void cPersonStore::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 
	switch( iNotifiedEvt )
	{
	case SystemType::eBtnDynamicOK:	
		{
			if( m_pBtnDynamicOK )
				m_pBtnDynamicOK->SetEnable( true );
		}
		break;

	case SystemType::eEnableWindow:	
		{
			SetEnableWindow( true );
		}
		break;

	case SystemType::eDisableWindow:	
		{
			SetEnableWindow( false );
		}
		break;

	case SystemType::eSetTamerName:
		{
			std::wstring wsTamerName;
			kStream >> wsTamerName;
			SetTamerName( wsTamerName );			
		}
		break;

	case SystemType::eSetStoreName:
		{
			std::wstring wsStoreName;
			kStream >> wsStoreName;
			m_pEditName->SetText(wsStoreName.c_str());		
		}
		break;

	case SystemType::eReady:
		{
			SetEnableWindow( true );
			SetEditNameEnable( true );
			ChangeWindowTitle();
			ChangeDefaultBtnText();
		}
		break;
	case SystemType::eOpen:
		{
			SetEnableWindow( true );
			if( GetSystem()->GetValueType() == PersonStoreContents::VT_RES_PERSON_START )
				SetEditNameEnable(false);
			ChangeWindowTitle();
			ChangeDefaultBtnText();
		}
		break;

	case SystemType::eUpdateItem:
		{
			MakeSellItemList();
		}
		break;
	}
}

void cPersonStore::Destroy()
{
	cBaseWindow::Delete();	
}

void cPersonStore::DeleteResource()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	cItemSeparate::DeleteInstance( cBaseWindow::WT_PERSONSTORE, 0 );

	DeleteScript();	

	// ¾ÆÀÌÅÛ ¾ðŽô
	if( g_pDataMng )
	{
		g_pDataMng->GetInven()->ItemUnlock_ItemType( nItem::PersonStore, 1 );
		g_pDataMng->GetInven()->ItemUnlock_ItemType( nItem::PersonStore, 3 );
	}

#ifdef IME_WINDOW_POSITION
	g_bIMEPosition = false;
#endif

}

void cPersonStore::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;
	
	InitScript( NULL, CsPoint( 80, 80 ), CsPoint( 300, 493 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );

	m_pBGWin = AddSprite(CsPoint::ZERO, CsPoint( 300, 493 ), "Store_G\\Win_Person01.tga");

	ChangeBG();

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );

	{	// #Name# »óÁ¡
		std::wstring wsStoreName = UISTRING_TEXT( "PERSONSTORE_STORE_NAME" );
		DmCS::StringFn::Replace( wsStoreName, L"#Name#", pUser->GetName() );
		cText::sTEXTINFO ti;
		ti.Init();
		m_pEditName = NiNew cEditBox;
		m_pEditName->Init( GetRoot(), IF_PERSONSTORE_NAME_EDITBOX_POS, IF_PERSONSTORE_NAME_EDITBOX_SIZE, &ti, false );
		m_pEditName->EnableUnderline();
		m_pEditName->EnablePaste();	
		m_pEditName->SetText( wsStoreName.c_str() );
		AddChildControl(m_pEditName);
	}

	{
		cText::sTEXTINFO ti;
		ti.Init();
		m_pTexRate = AddText(&ti, IF_PERSONSTORE_TEX_POS);
	}

	m_pBtnDynamicOK = AddButton( CsPoint( 182, 456 ), CsPoint( 85, 27 ), cButton::IMAGE_NOR_13, _T( "" ) );
	m_pBtnDynamicOK->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cPersonStore::PressDynamicOKBtn);

	m_pCancelButton = AddButton( CsPoint( 278, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pCancelButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cPersonStore::PressCancelBtn);
	
	/*m_pItemBlock = NiNew cSprite;
	m_pItemBlock->Init( NULL, CsPoint::ZERO, IF_PERSONSTORE_ICON_SIZE, "Inventory\\Block.tga", false );*/

	// ¾ÆÀÌÅÛ ¸®½ºÆ® ¹Ú½º ÃÊ±âÈ­
	m_pListBox = NiNew cListBox;
	if( m_pListBox )
	{
		m_pListBox->Init( GetRoot(), CsPoint( 7, 85 ), CsPoint( 290, 365 ), NULL, false );
		cScrollBar* pScroll = NiNew cScrollBar;
		if( pScroll )
		{
			pScroll->Init( cScrollBar::TYPE_1, NULL, CsPoint( 265, 82 ), CsPoint( 16, 365 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 13, 83 ), CsPoint( 290, 448 ) ), 5, false );
		}
		m_pListBox->SetScrollBar( pScroll );
		m_pListBox->AddEvent( cListBox::LIST_SELECT_CHANGE_ITEM, this, &cPersonStore::PressItemBtn );
		m_pListBox->SetItemGab( CsPoint( 0, 37 ) );
		m_pListBox->SetMargin( CsPoint( 25, 19 ) );
	}

	AddChildControl( m_pListBox );

	MakeSellItemList();
	
	SetTexRate();

	ChangeWindowTitle();
	ChangeDefaultBtnText();
}

void cPersonStore::Update(float const& fDeltaTime)
{
	cItemSeparate::UpdateInstance( cBaseWindow::WT_PERSONSTORE, 0 );
	
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cPersonStore::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cItemSeparate::Update_ForMouseInstance( GetWindowType(), 0 );

	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	muReturn = cBaseWindow::Update_ForMouse();
	
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch(GetSystem()->GetValueType())
	{
	case PersonStoreContents::VT_RES_PERSON:
	case PersonStoreContents::VT_RES_EMPLOYMENT:
		{
			if( m_pEditName )
				m_pEditName->Update_ForMouse( CsPoint::ZERO );
		}
		break;
	}	

	if( m_pListBox && m_pListBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cListBoxItem const* pOverItem = m_pListBox->GetMouseOverItem();
		SAFE_POINTER_RETVAL( pOverItem, muReturn );

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		// ¾ÆÀÌÅÛ Á¤º¸
		sPersonStoreItem* pSellInfo = dynamic_cast< sPersonStoreItem* >( pOverItem->GetUserData() );
		SAFE_POINTER_RETVAL( pSellInfo, muReturn );
		
		PersonStoreContents::sSellInfo* pInfo = GetSystem()->GetSellInfo( pSellInfo->nSellInfoIndex );
		SAFE_POINTER_RETVAL( pSellInfo, muReturn );

 		pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 226, 44 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->ItemData.m_nType, cBaseWindow::WT_PERSONSTORE, cTooltip::OPEN_SLOT, 0, &pInfo->ItemData );
		return muReturn;
	}

	// ½ÂÀÎ ¹öÆ°
	if(m_pBtnDynamicOK && m_pBtnDynamicOK->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	// Äµ½½ ¹öÆ°
	if(m_pCancelButton && m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	// À§¿¡ ¹Ù µå·¡±×ÇØ¾ß¸¸ ÀÌµ¿ ½ÃÅ°µµ·Ï ÇÏ°Ú´Ù.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( this->GetRoot()->GetSize().x, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();		

	return muReturn;
}

cBaseWindow::eMU_TYPE cPersonStore::Update_ForMouse_Disable()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cItemSeparate::Update_ForMouseInstance( GetWindowType(), 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	return cBaseWindow::Update_ForMouse_Disable();
}

void cPersonStore::Render()
{
	RenderScript();	

#ifdef IME_WINDOW_POSITION
	if( m_pEditName != NULL )
	{
		CsPoint TempPos = m_pEditName->GetDefaultPos();
		CsPoint TempStringPos = m_pEditName->GetText()->GetStringSize();
		g_nIMEWidth = GetRootClient().x + TempPos.x + TempStringPos.x;
		g_nIMEHeight = GetRootClient().y + TempPos.y + TempStringPos.y;
	}
#endif

	EndRenderScript();
	cItemSeparate::RenderInstance( cBaseWindow::WT_PERSONSTORE, 0 );
}

bool cPersonStore::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	switch( eIconType )
	{			
	case CURSOR_ICON::CI_INVEN:
		{
			GetSystem()->RegisterSeparateItem( nIconSlot );
			CURSOR_ST.ReleaseIcon();
		}
		return true;
	}
	return false;
}

void cPersonStore::ResetDevice()
{	
	ResetDeviceScript();
}

void cPersonStore::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

bool cPersonStore::OnEscapeKey()
{
	GetSystem()->CloseWindowUI();
	return true;
}

void cPersonStore::PressDynamicOKBtn( void* pSender, void* pData )
{
	SetEditNameEnable( false );

	GetSystem()->PressDynamicButton();

	if( m_pEditName )
		m_pEditName->ReleaseFocus( false );
}

void cPersonStore::PressCancelBtn( void* pSender, void* pData )
{
	GetSystem()->CloseWindowUI();
}

void cPersonStore::ChangeBG()
{
	switch(GetSystem()->GetValueType())
	{
	case PersonStoreContents::VT_RES_PERSON:
	case PersonStoreContents::VT_RES_EMPLOYMENT:
		{
			m_pBGWin->ChangeTexture("Store_G\\Win_Person02.tga");
		}
		break;

	case PersonStoreContents::VT_RES_PERSON_START:
	case PersonStoreContents::VT_OPEN_PERSON:
	case PersonStoreContents::VT_RES_EMPLOYMENT_START:
	case PersonStoreContents::VT_OPEN_EMPLOYMENT:
		{
			m_pBGWin->ChangeTexture("Store_G\\Win_Person01.tga");
		}
		break;
	}
}

void cPersonStore::SetTexRate()
{
	SAFE_POINTER_RET( nsCsFileTable::g_pBaseMng );

	CsBaseMng::sPERSON_STORE* pFTPersonStore = nsCsFileTable::g_pBaseMng->GetPersonStore();
	SAFE_POINTER_RET( pFTPersonStore );

	int nTex = 0;

	switch( GetSystem()->GetValueType() )
	{
	case PersonStoreContents::VT_RES_PERSON:
		{
			SetEditNameEnable( true );
			nTex = static_cast< int >( pFTPersonStore->s_BaseInfo.s_fPerson_Charge * 100.0f );
		}
		break;

	case PersonStoreContents::VT_RES_EMPLOYMENT:
		{
			SetEditNameEnable( true );
			nTex = static_cast< int >( pFTPersonStore->s_BaseInfo.s_fEmployment_Charge*100.0f );
		}
		break;

	case PersonStoreContents::VT_OPEN_PERSON:
	case PersonStoreContents::VT_OPEN_EMPLOYMENT:
		{
			m_pTexRate->SetVisible( false );
		}		
		break;

	default:
		assert_cs( false );
	}


	// ÆÇ¸Å ¼ö¼ö·á : #Tex#%
	std::wstring wsTex = UISTRING_TEXT( "PERSONSTORE_SELL_TEX" );
	DmCS::StringFn::Replace( wsTex, L"#Tex#", nTex );

	m_pTexRate->SetText( wsTex.c_str() );
}

//==============================================================================================================
//
//		±âÅ¸
//
//==============================================================================================================

void cPersonStore::ChangeWindowTitle()
{
	std::wstring wsTitle;
	bool bEnableEdit = false;
	switch( GetSystem()->GetValueType() )
	{
	case PersonStoreContents::VT_RES_PERSON:
		wsTitle = UISTRING_TEXT( "PERSONSTORE_TITLE_REGIST_PERSON" );
		break;
	case PersonStoreContents::VT_RES_EMPLOYMENT:
		wsTitle = UISTRING_TEXT( "PERSONSTORE_TITLE_REGIST_EMPLOY" );
		break;
	case PersonStoreContents::VT_RES_PERSON_START:
	case PersonStoreContents::VT_OPEN_PERSON:
		wsTitle = UISTRING_TEXT( "PERSONSTORE_TITLE_PERSONSTORE" );
		break;
	case PersonStoreContents::VT_RES_EMPLOYMENT_START:
	case PersonStoreContents::VT_OPEN_EMPLOYMENT:
		wsTitle = UISTRING_TEXT( "PERSONSTORE_TITLE_EMPLOYSTORE" );
		break;
	default:
		assert_cs( false );
	}
	AddTitle( wsTitle.c_str() );
}

void cPersonStore::ChangeDefaultBtnText()
{
	switch( GetSystem()->GetValueType() )
	{
	case PersonStoreContents::VT_OPEN_PERSON:
	case PersonStoreContents::VT_OPEN_EMPLOYMENT:
		{
			m_pBtnDynamicOK->SetVisible(false);
		}
		break;		

	case PersonStoreContents::VT_RES_PERSON:
		{
			//ÆÇ¸Å½ÃÀÛ
			m_pBtnDynamicOK->SetText( UISTRING_TEXT( "PERSONSTORE_BUTTON_START_SELL" ).c_str() );
			m_pBtnDynamicOK->SetEnable( GetSystem()->GetSellInfoSize() != 0 );
		}		
		break;

	case PersonStoreContents::VT_RES_PERSON_START:
		{
			// ÆÇ¸ÅÁßÁö
			m_pBtnDynamicOK->SetText( UISTRING_TEXT( "PERSONSTORE_BUTTON_STOP_SELL" ).c_str() );
		}		
		break;

	case PersonStoreContents::VT_RES_EMPLOYMENT:
		{
			// À§Å¹»óÁ¡ ¿ÀÇÂ
			m_pBtnDynamicOK->SetText( UISTRING_TEXT( "PERSONSTORE_BUTTON_OPEN_EMPLOY" ).c_str() );
			m_pBtnDynamicOK->SetEnable( false );
		}
		break;

	case PersonStoreContents::VT_RES_EMPLOYMENT_START:
		{
			// À§Å¹»óÁ¡ È¸¼ö
			m_pBtnDynamicOK->SetText( UISTRING_TEXT( "PERSONSTORE_BUTTON_CLOSE_EMPLOY" ).c_str() );
			m_pBtnDynamicOK->SetVisible(true);
		}
		break;

	default:
		assert_cs( false );
	}
}

void cPersonStore::SetEditNameEnable(bool enable)
{
	if(enable)
	{
		m_pEditName->SetFontLength( Language::pCountry::MAX_LenShopName );	
		m_pEditName->SetFocus();
		m_pEditName->SetFontColor(FONT_WHITE);

		g_IME.Native_On( GAMEAPP_ST.GetHWnd() );

		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
		CProtocol.m_value= 0;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
	}
	else
	{
		GetSystem()->SetStoreName( m_pEditName->GetTextAll() );

		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, FONT_GOLD );
		m_pEditName->SetFontColor(FONT_GOLD);
	}	
}

void cPersonStore::SetTamerName(std::wstring wsName)
{
#ifndef EMPLOYMENT_TAMERNAME
	return;
#endif

	std::wstring wsTamerName = L"< #Name# >";
	DmCS::StringFn::ReplaceAll( wsTamerName, L"#Name#", wsName );

	if( !m_pTamerName )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, FONT_WHITE );
		ti.s_eTextAlign = DT_CENTER;
	
		ti.SetText( wsTamerName.c_str() );
	
		m_pTamerName = AddText(&ti, CsPoint( 150, 62 ));
	}
	
	m_pTamerName->SetText( wsTamerName.c_str() );
}

void cPersonStore::MakeSellItemList()
{
	SAFE_POINTER_RET( m_pListBox );
	m_pListBox->RemoveAllItem();
	
	int nSellCount = GetSystem()->GetSellInfoSize();
	int nSlotCount = GetSystem()->GetOpenSlotCount();

	// ÆÇ¸Å ¸ñ·Ï ¾ÆÀÌÅÛ Ãß°¡
	for( int i = 0; i < nSellCount; ++i )
	{
		PersonStoreContents::sSellInfo* pInfo = GetSystem()->GetSellInfo( i );
		SAFE_POINTER_CON( pInfo );
		AddListItem( i, pInfo->uPrice, pInfo->ItemData );
	}

	if( IF_PERSONSTORE_VIEW_COUNT <= nSlotCount )
		return;

	// ºó ¾ÆÀÌÅÛ Ãß°¡
	for( int i = nSellCount; i <= IF_PERSONSTORE_VIEW_COUNT - nSlotCount; ++i )
	{
		AddListBlock( false );
	}

	// ºí·Ï(µî·ÏºÒ°¡½½·Ô) ¾ÆÀÌÅÛ Ãß°¡
	for( int i = IF_PERSONSTORE_VIEW_COUNT - nSlotCount + 1; i < IF_PERSONSTORE_VIEW_COUNT; ++i )
	{
		AddListBlock( true );
	}
}

void cPersonStore::AddListItem(int nIndex, u8 const& uPrice, cItemData const& ItemData)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	// ¼±ÅÃµÈ ¾ÆÀÌÅÛ
	cButton* pButton = NiNew cButton;
	SAFE_POINTER_RET( pButton );
	pButton->Init( NULL, CsPoint::ZERO, CsPoint( 226, 40 ), "Store_G\\Win_Person_SelMask.tga", false );
	pButton->SetTexToken( CsPoint( 0, 40 ) );
	cString::sBUTTON* pStrButton = pControl->AddButton( pButton, nIndex, CsPoint( -5, -5 ) );
	if( pStrButton )
		pStrButton->SetAutoPointerDelete( true );

	int nItemType = ItemData.m_nType;	
	
	// ¾ÆÀÌÅÛ ¾ÆÀÌÄÜ
	cString::sICON* pAddCon = pControl->AddIcon( IF_PERSONSTORE_ICON_SIZE, ICONITEM::ITEM_ICON, nItemType, 1, CsPoint::ZERO );
	if( pAddCon )
		pAddCon->s_nIndex_3 = ItemData.m_nRate;
	
	{
		// ¾ÆÀÌÅÛ °³¼ö
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.SetText( ItemData.m_nCount );
		cString::sTEXT* pStrText = pControl->AddText( &ti, CsPoint( 2, 35 ) );
		if( pStrText )
			pStrText->SetAutoPointerDelete( true );
	}
	
	{
		// °³´ç
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.SetText( UISTRING_TEXT( "PERSONSTORE_PRICE_PER_ITEM" ).c_str() );
		cString::sTEXT* pStrText = pControl->AddText( &ti, CsPoint( 42, 35 ) );
		if( pStrText )
			pStrText->SetAutoPointerDelete( true );
	}
	
	{
		// ¾ÆÀÌÅÛ ÀÌ¸§
		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( nItemType );
		SAFE_POINTER_RET( pItem );
		CsItem::sINFO* pItemInfo = pItem->GetInfo();
		SAFE_POINTER_RET( pItemInfo );

		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, g_pDataMng->GetItemColor( pItemInfo ) );
		cString::sTEXT* pStrText = pControl->AddText( &ti, CsPoint( 42, 0 ) );
		if( pStrText )
		{
			pStrText->SetText( pItem->GetName().c_str(), 180 );
			pStrText->SetAutoPointerDelete( true );
		}
	}

	// °¡°Ý
	pControl->AddMoneyIF( cMoneyIF::DISP_FULL, uPrice, FONT_WHITE, CFont::FS_9, DT_RIGHT, CsPoint( 70, 30 ), true );

	cListBoxItem* pListItem = NiNew cListBoxItem;
	if( pListItem )
	{
		pListItem->SetItem( pControl );
		pListItem->SetUserData( new sPersonStoreItem( nIndex ) );
	}

	m_pListBox->AddItem( pListItem );
}

void cPersonStore::AddListBlock(bool bBlock)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	if( bBlock )
	{
		cSprite* pSprite = NiNew cSprite;
		SAFE_POINTER_RET( pSprite );

		pSprite->Init( NULL, CsPoint::ZERO, IF_PERSONSTORE_ICON_SIZE, "Inventory\\Block.tga", false );
		cString::sSPRITE* pStrSprite = pControl->AddSprite( pSprite );
		if( pStrSprite )
			pStrSprite->SetAutoPointerDelete( true );
	}

	cListBoxItem* pListItem = NiNew cListBoxItem;
	if( pListItem )
		pListItem->SetItem( pControl );

	m_pListBox->AddItem( pListItem );
}

void cPersonStore::PressItemBtn(void* pSender, void* pData)
{
	cListBoxItem* pItem = static_cast< cListBoxItem* >( pData );
	SAFE_POINTER_RET( pItem );
	sPersonStoreItem* pUserItem = dynamic_cast< sPersonStoreItem* >( pItem->GetUserData() );
	SAFE_POINTER_RET( pUserItem );

	int	nSize = GetSystem()->GetSellInfoSize();

	GetSystem()->PressItemButton( pUserItem->nSellInfoIndex );

	nSize = GetSystem()->GetSellInfoSize();

	if( nSize == 0 )
		m_pBtnDynamicOK->SetEnable( false );
}