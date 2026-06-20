#include "stdafx.h"
#include "NewCashshopSeliingItemViewer.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void NewCashshopSeliingItemViewer::ItemControls::SetEmpty()
{
}

void NewCashshopSeliingItemViewer::ItemControls::SetVisible( bool bValue )
{
	if( pkItemIcon )
		pkItemIcon->SetVisible(bValue);
	if( pkHitIcon )
		pkHitIcon->SetVisible(bValue);
	if( pPresentBtn )
		pPresentBtn->SetVisible(bValue);
	if( pkPurchaseBt )
		pkPurchaseBt->SetVisible(bValue);
	if( pkCartBt )
		pkCartBt->SetVisible(bValue);
	if( pkPriceText )
		pkPriceText->SetVisible(bValue);
	if( pkItemName )
		pkItemName->SetVisible(bValue);
	if( pkCashIcon )
		pkCashIcon->SetVisible(bValue);
	if( pkSaleIcon )
		pkSaleIcon->SetVisible(bValue);
	if( pkEndSale )
		pkEndSale->SetVisible( bValue );
	if( pkSalePriceText )
		pkSalePriceText->SetVisible(bValue);
	if( pkLine )
		pkLine->SetVisible(bValue);

	ChangeBtnData(0,0);
}


void NewCashshopSeliingItemViewer::ItemControls::ChangeItemIcon( DWORD const& dwIconID )
{
	SAFE_POINTER_RET( pkItemIcon );

	//아이템 이미지			
	ICON_Dynamic::eTYPE nIconType = (ICON_Dynamic::eTYPE)(dwIconID/ 100);
	cIcon* pkIcon = g_pIconMng->GetIcon(nIconType);
	SAFE_POINTER_RET(pkIcon);

	cSprite* IconSprite = g_pIconMng->GetSprite(nIconType);
	SAFE_POINTER_RET(IconSprite);

	int nCashshopItemIdx = dwIconID % 100;	
	int nColInTextrue = nCashshopItemIdx%pkIcon->GetIndexCount().x;
	int nRowInTextrue = nCashshopItemIdx/pkIcon->GetIndexCount().x;
	int Size = IconSprite->GetSize().x;	

	pkItemIcon->ChangeTexture( IconSprite->GetFilePath().c_str(), CsRect(CsPoint(nColInTextrue * Size, nRowInTextrue * Size), 
		CsPoint((nColInTextrue + 1) * Size , (nRowInTextrue + 1) * Size)) );
}

void NewCashshopSeliingItemViewer::ItemControls::ChangePrice( int const& nPrice, int const& nStandardPrice )
{
	if( nPrice != nStandardPrice )
	{
		std::wstring value;
#ifdef VERSION_STEAM
		DmCS::StringFn::Format(value, L"%.2f", nStandardPrice * 0.01f );
#else
		value = DmCS::StringFn::getNumberFormatW(nStandardPrice);
#endif
		if( pkPriceText )
		{
			pkPriceText->SetText( value.c_str() );
			pkPriceText->SetColor( FONT_GLAY );
			pkPriceText->s_ptDeltaPos.x = 80;
		}

		if( pkLine )
		{
			CsPoint nSize = pkPriceText->s_Text.GetStringSize();
			pkLine->s_ptSize = CsPoint(nSize.x, 2);
			pkLine->s_ptDeltaPos.x = (80 - (nSize.x/2));
			pkLine->s_ptDeltaPos.y = (130 + (nSize.y/2));
			pkLine->SetVisible(true);
		}
#ifdef VERSION_STEAM
		DmCS::StringFn::Format(value, L"%.2f", nPrice * 0.01f );
#else
		value = DmCS::StringFn::getNumberFormatW(nPrice);
#endif
		if( pkSalePriceText )
		{
			pkSalePriceText->SetText( value.c_str() );
			pkSalePriceText->SetVisible(true);
		}
	}
	else
	{
		std::wstring value;
#ifdef VERSION_STEAM
		DmCS::StringFn::Format(value, L"%.2f", nPrice * 0.01f );
#else
		value = DmCS::StringFn::getNumberFormatW(nPrice);
#endif
		if( pkPriceText )
		{
			pkPriceText->s_ptDeltaPos.x = 91;
			pkPriceText->SetText( value.c_str() );
			pkPriceText->SetColor( FONT_YELLOW );
		}

		if( pkLine )
			pkLine->SetVisible(false);

		if( pkSalePriceText )
		{
			pkSalePriceText->SetText( L"" );
			pkSalePriceText->SetVisible(false);
		}
	}
}

void NewCashshopSeliingItemViewer::ItemControls::ChangeSale( int const& nSaleIconIdx )
{
	SAFE_POINTER_RET( pkSaleIcon );
	pkSaleIcon->SetImangeIndex( nSaleIconIdx );
}

void NewCashshopSeliingItemViewer::ItemControls::ChangeItemName( std::wstring const& wsName )
{
	SAFE_POINTER_RET( pkItemName );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
	cStringList sl;
	g_pStringAnalysis->Cut_Parcing( &sl, 160, wsName.c_str(), &ti );

	std::wstring itemName;
	std::list< cString* >* pkList = sl.GetList();
	std::list< cString* >::iterator kIter = pkList->begin();
	for(; kIter != pkList->end(); ++kIter)
	{	
		cString::sTEXT* pText = (cString::sTEXT*)(*kIter)->GetElement(0);
		itemName += pText->s_Text.GetText();
	}

	sl.Delete();
	g_pStringAnalysis->MaxLine_Cut(L"...", &sl, 160, itemName.c_str(), &ti );
	pkItemName->SetText( ti.GetText() );
}

void NewCashshopSeliingItemViewer::ItemControls::SetHitType(int const& nHitType )
{
	SAFE_POINTER_RET( pkHitIcon );
	switch( nHitType )
	{
	case neCashsopHitType::New:
	case neCashsopHitType::Hot:
	case neCashsopHitType::Event:
		pkHitIcon->SetImangeIndex( nHitType );
		pkHitIcon->SetVisible(true);
		break;
	default:
		pkHitIcon->SetVisible(false);
		break;
	}
	
}

void NewCashshopSeliingItemViewer::ItemControls::ChangeBtnData(  DWORD const& dwGroupID, DWORD const& dwProductID )
{
	if( pkItemControl )
	{
		ItemButtonType* pUserData = static_cast<ItemButtonType*>(pkItemControl->GetUserData());
		pUserData->dwProductIDX = dwProductID;		
		pUserData->dwGroupID = dwGroupID;
	}

	if( pPresentBtn )
	{
		ItemButtonType* pUserData = static_cast<ItemButtonType*>(pPresentBtn->GetUserData());
		pUserData->dwProductIDX = dwProductID;
		pUserData->dwGroupID = dwGroupID;
	}

	if( pkPurchaseBt )
	{
		ItemButtonType* pUserData = static_cast<ItemButtonType*>(pkPurchaseBt->GetUserData());
		pUserData->dwProductIDX = dwProductID;
		pUserData->dwGroupID = dwGroupID;
	}

	if( pkCartBt )
	{
		ItemButtonType* pUserData = static_cast<ItemButtonType*>(pkCartBt->GetUserData());
		pUserData->dwProductIDX = dwProductID;
		pUserData->dwGroupID = dwGroupID;
	}
}

void NewCashshopSeliingItemViewer::ItemControls::SetEndSale( bool bSale )
{
	if( pPresentBtn )
		pPresentBtn->SetVisible( bSale );
	if( pkPurchaseBt )
		pkPurchaseBt->SetVisible( bSale );
	if( pkCartBt )
		pkCartBt->SetVisible( bSale );
	if( pkEndSale )
		pkEndSale->SetVisible( !bSale );
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

NewCashshopSeliingItemViewer::NewCashshopSeliingItemViewer()
:m_pkItems(NULL),m_nMaxCount(0),m_nMaxPage(0),m_nCurrentPage(0),m_nMainTab(0),m_nSubTab(0)
{
	Construct();
}

NewCashshopSeliingItemViewer::~NewCashshopSeliingItemViewer()
{
	DeleteResource();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void NewCashshopSeliingItemViewer::Destroy()
{

}
void NewCashshopSeliingItemViewer::DeleteResource()
{
	m_mapControls.clear();
	DeleteScript();
}
void NewCashshopSeliingItemViewer::Create( cWindow* pkRoot,  int nValue /*=0*/ )
{
	InitScript( pkRoot, NULL, CsPoint(16,106), CsPoint(551,600), false );

	m_pkItems = NiNew cGridListBox;
	if( m_pkItems )
	{
		m_pkItems->Init( GetRoot(), CsPoint::ZERO, CsPoint(551,570), 
			CsPoint(0,1), CsPoint(183,189), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		AddChildControl(m_pkItems);
		m_pkItems->AddEvent( cGridListBox::GRID_SELECTED_ITEM, this, &NewCashshopSeliingItemViewer::OnItemSelected_SellingItem );
		m_pkItems->AddEvent( cGridListBox::GRID_BUTTON_UP, this, &NewCashshopSeliingItemViewer::OnClick_SellingItemFunctionBT );
		m_pkItems->SetUsePressedMoveEvent(true);
		m_pkItems->SetMouseOverImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pkItems->SetSelectedImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pkItems->SetBackOverAndSelectedImgRender(false);
	}

	m_pkPrevPageBT = AddButton( CsPoint(205,580), CsPoint(18,20), CsPoint( 0, 20 ), "Cash\\NewCashShop\\Store_btn_A1.tga" );	
	if( m_pkPrevPageBT )
		m_pkPrevPageBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopSeliingItemViewer::OnClick_PrevPageBT);

	m_pkNextPageBT = AddButton( CsPoint(325,580), CsPoint(18,20), CsPoint( 0, 20 ), "Cash\\NewCashShop\\Store_btn_A2.tga" );	
	if( m_pkNextPageBT )
		m_pkNextPageBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopSeliingItemViewer::OnClick_NextPageBT);

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor::WHITE );
		ti.SetText(L"0 / 0");
		ti.s_eTextAlign = DT_CENTER;	
		m_pkCurrPageTT = AddText( &ti, CsPoint(275,580));
	}

	_EmptySellingItemSlot(9);
}

void NewCashshopSeliingItemViewer::Update(float const& fDeltaTime)
{
	if( m_pkItems )
		m_pkItems->Update(fDeltaTime);
}

BOOL NewCashshopSeliingItemViewer::UpdateMouse(void)
{
	if( m_pkPrevPageBT && m_pkPrevPageBT->Update_ForMouse() != cButton::ACTION_NONE )
		return TRUE;
	if( m_pkNextPageBT && m_pkNextPageBT->Update_ForMouse() != cButton::ACTION_NONE )
		return TRUE;
	if(m_pkItems && m_pkItems->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;
	return FALSE;
}

void	NewCashshopSeliingItemViewer::OnItemSelected_SellingItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	ItemButtonType* pkInfo =	dynamic_cast<ItemButtonType*>(pItem->GetUserData());
	BHPRT("OnItemSelected_SellingItem ID[%d]",pkInfo->dwProductIDX);
	GetSystem()->OnClick_SellingItem(pkInfo->dwGroupID, pkInfo->dwProductIDX);
}

void NewCashshopSeliingItemViewer::OnClick_SellingItemFunctionBT(void* pSender, void* pData)
{
	cString::sBUTTON* pkButton = static_cast< cString::sBUTTON*>(pData);
	SAFE_POINTER_RET(pkButton);
	ItemButtonType* pUserData = static_cast<ItemButtonType*>(pkButton->s_pButton->GetUserData());
	SAFE_POINTER_RET(pUserData);

	GetSystem()->OnClick_ShowPurchaseItem( (NewCashshopContents::CASHSHOP_FUNTION)pUserData->m_eType, pUserData->dwProductIDX );
}


void NewCashshopSeliingItemViewer::OnClick_NextPageBT(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pSender);

	if( m_nCurrentPage + 1 > m_nMaxPage )
		return;
	UpdatePage(m_nMainTab, m_nSubTab, m_nCurrentPage + 1);
}

void NewCashshopSeliingItemViewer::OnClick_PrevPageBT(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pSender);
	if( m_nCurrentPage - 1 < 1 )
		return;
	UpdatePage(m_nMainTab, m_nSubTab, m_nCurrentPage - 1);
}

void NewCashshopSeliingItemViewer::_EmptySellingItemSlot( int const& nMakeCount )
{
	m_mapControls.clear();
	SAFE_POINTER_RET( m_pkItems );
	m_pkItems->RemoveAllItem();

	for( int n = 0; n < nMakeCount; ++n )
	{		
		cString * pItem = NiNew cString;
		/////////////////////////////////////////////////////////
		//배경
		cSprite* pkBG = NiNew cSprite;
		pkBG->Init( NULL, CsPoint::ZERO, CsPoint(183,189),  "Cash\\NewCashShop\\Store_win_bg.tga", false );
		cString::sSPRITE* sSprite = pItem->AddSprite( pkBG, CsPoint::ZERO, CsPoint(183,189) );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);

		ItemControls addControls;		
		addControls.pkItemIcon = NiNew cSprite;
		addControls.pkItemIcon->Init( NULL, CsPoint::ZERO, CsPoint(80,80),  CsRect(CsPoint(0,0),CsPoint(0, 0)), "", false );
		sSprite = pItem->AddSprite(addControls.pkItemIcon, CsPoint(51,16), CsPoint(80,80) );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);


		//좌상단 Ribbon
		cImage* pHitIcon = NiNew cImage;
		pHitIcon->Init( NULL, CsPoint::ZERO, CsPoint(64,64), "Cash\\NewCashShop\\CashShop_Ribbon.tga", false );
		pHitIcon->SetTexToken( CsPoint( 0, 64 ) );
		addControls.pkHitIcon = pItem->AddImage( pHitIcon, 0, CsPoint(2,2), CsPoint(64,64)  );
		if( addControls.pkHitIcon )
			addControls.pkHitIcon->SetAutoPointerDelete(true);

		cImage* pkSaleIcon = NiNew cImage;
		if( pkSaleIcon )
		{
			pkSaleIcon->Init( NULL, CsPoint::ZERO, CsPoint(80,80), "Cash\\NewCashShop\\CashShop_Ribon.tga", false );
			pkSaleIcon->SetTexToken( CsPoint( 80, 80 ) );
			addControls.pkSaleIcon = pItem->AddImage( pkSaleIcon, 0, CsPoint(54,16), CsPoint(80,80) );
			if( addControls.pkSaleIcon )
				addControls.pkSaleIcon->SetAutoPointerDelete(true);
		}
		
		cText::sTEXTINFO tiprice;
		tiprice.Init(CFont::FS_12 );
		tiprice.SetText(L"0");
		tiprice.s_eTextAlign = DT_CENTER;
		tiprice.s_Color = FONT_YELLOW;
		addControls.pkPriceText = pItem->AddText(&tiprice, CsPoint(91,130));

		cSprite* pkLine = NiNew cSprite;
		pkLine->Init( NULL, CsPoint::ZERO, CsPoint(10,2), NiColorA(1.0f,0.0f,0.0f,1.0f), false );
		addControls.pkLine = pItem->AddSprite( pkLine, CsPoint(0,135), CsPoint(10,2) );	
		if( addControls.pkLine )
			addControls.pkLine->SetAutoPointerDelete(true);

		cText::sTEXTINFO tiprice2;
		tiprice2.Init(CFont::FS_12 );
		tiprice2.SetText(L"0");
		tiprice2.s_eTextAlign = DT_RIGHT;
		tiprice2.s_Color = FONT_GREEN;
		addControls.pkSalePriceText = pItem->AddText(&tiprice2, CsPoint(168,130));
		

		cText::sTEXTINFO tiName;
		tiName.Init(CFont::FS_12 );
		tiName.SetText(L" ");
		tiName.s_eTextAlign = DT_CENTER;
		addControls.pkItemName = pItem->AddText(&tiName, CsPoint(91,103));

		//CASH ICON
		cSprite* pkCashIcon = NiNew cSprite;
#ifndef VERSION_STEAM
		pkCashIcon->Init( NULL, CsPoint::ZERO, CsPoint(24, 24),  "Cash\\NewCashShop\\Store_icon_C.tga", false );
#else
		pkCashIcon->Init( NULL, CsPoint::ZERO, CsPoint(24, 24),  "Cash\\NewCashShop\\Store_icon_S.tga", false );
#endif
		addControls.pkCashIcon = pItem->AddSprite( pkCashIcon, CsPoint(20,125), CsPoint(24,24) );	
		if( addControls.pkCashIcon )
			addControls.pkCashIcon->SetAutoPointerDelete(true);

#ifndef VERSION_STEAM
#ifndef SDM_CASHSHOP_GIFT_SYSTEM_HIDE_20181207
		//PresentBT 선물하기 버튼
		addControls.pPresentBtn = NiNew cButton;
		if(addControls.pPresentBtn)
		{
			addControls.pPresentBtn->Init( NULL, CsPoint::ZERO, CsPoint(54,30), "Cash\\NewCashShop\\Store_btn_N1.tga", false );
			addControls.pPresentBtn->SetTexToken( CsPoint( 0, 30 ) );
			addControls.pPresentBtn->SetUserData(new ItemButtonType(NewCashshopContents::PRESENT, 0, 0));
			cString::sBUTTON* pAddedBT = pItem->AddButton( addControls.pPresentBtn, 0, CsPoint(7, 151), CsPoint(54,30), CsPoint(65,13) );
			if( pAddedBT )
				pAddedBT->SetAutoPointerDelete(true);
		}
#endif
#endif
		//Purchase BT
		addControls.pkPurchaseBt = NiNew cButton;
		if(addControls.pkPurchaseBt)
		{
			addControls.pkPurchaseBt->Init( NULL, CsPoint::ZERO, CsPoint(54,30), "Cash\\NewCashShop\\Store_btn_N2.tga", false );
			addControls.pkPurchaseBt->SetTexToken( CsPoint( 0, 30 ) );
			addControls.pkPurchaseBt->SetUserData(new ItemButtonType(NewCashshopContents::PURCHASE, 0, 0));
			cString::sBUTTON* pAddedBT = pItem->AddButton( addControls.pkPurchaseBt, 0, CsPoint(121,151), CsPoint(54,30), CsPoint(65,13) );
			if( pAddedBT )
				pAddedBT->SetAutoPointerDelete(true);
		}

		//CART BT
		addControls.pkCartBt = NiNew cButton;
		if(addControls.pkCartBt)
		{
			addControls.pkCartBt->Init( NULL, CsPoint::ZERO, CsPoint(54,30), "Cash\\NewCashShop\\Store_btn_N3.tga", false );
			addControls.pkCartBt->SetTexToken( CsPoint( 0, 30 ) );
			addControls.pkCartBt->SetUserData(new ItemButtonType(NewCashshopContents::CART, 0, 0));
			cString::sBUTTON* pAddedBT = pItem->AddButton( addControls.pkCartBt, 0, CsPoint(64,151), CsPoint(54,30), CsPoint(65,13) );
			if( pAddedBT )
				pAddedBT->SetAutoPointerDelete(true);
		}

		cText::sTEXTINFO tiEndSale;
		tiEndSale.Init(CFont::FS_12, FONT_RED );
		tiEndSale.s_eTextAlign = DT_CENTER;
		tiEndSale.SetText(L"판매 종료");
		addControls.pkEndSale = pItem->AddText(&tiEndSale, CsPoint(91,158));

		addControls.pkItemControl  = NiNew cGridListBoxItem(n,  CsPoint(183,189));
		addControls.pkItemControl->SetItem( pItem );
		addControls.pkItemControl->SetUserData( new ItemButtonType(NewCashshopContents::NONE, 0, 0) );
		m_pkItems->AddItem( addControls.pkItemControl );
		m_mapControls.insert(std::make_pair( n, addControls ) );
	}
}

void NewCashshopSeliingItemViewer::UpdatePage( int const& nMainTabIdx, int const& nSubTabIdx, int const & nPage, DWORD const& dwSelectGroupID)
{
	std::map<int,ItemControls>::iterator it = m_mapControls.begin();
	for( ; it != m_mapControls.end(); ++it )
		it->second.SetVisible(false);

	m_nMainTab = nMainTabIdx;
	m_nSubTab = nSubTabIdx;

	NewCashshopContents::sSubCategoryInfo const * pSubCategoryInfo = GetSystem()->GetSubCategoryInfos( nMainTabIdx, nSubTabIdx );
	SAFE_POINTER_RET( pSubCategoryInfo );

	m_nMaxCount = pSubCategoryInfo->GetShowItemCount();
	m_nMaxPage = m_nMaxCount / 9;
	if( m_nMaxCount % 9 > 0 || 0 == m_nMaxCount )
		m_nMaxPage++;
	m_nCurrentPage = nPage;

	if( m_pkCurrPageTT )
	{
		std::wstring txt;
		DmCS::StringFn::Format( txt, L"%d / %d", m_nCurrentPage, m_nMaxPage );
		m_pkCurrPageTT->SetText( txt.c_str() );
	}

	int nStartIdx = (m_nCurrentPage - 1) * 9;
	int nSelectIdx = -1;
	NewCashshopContents::LIST_PRODUCTGROUP_INFO_CIT productit = pSubCategoryInfo->itemgroup.begin();
	int nSlotIdx = 0;
	for( int n = 0; productit != pSubCategoryInfo->itemgroup.end(); ++productit )
	{
		if( !(*productit).bIsFileringShow )
			continue;

		if( n < nStartIdx || n == m_nMaxCount )
		{
			++n;
			continue;
		}

		std::map<int,ItemControls>::iterator conIt = m_mapControls.find( nSlotIdx );
		if( conIt == m_mapControls.end() )
			break;

		if( dwSelectGroupID == (*productit).dwGroupID )
			nSelectIdx = nSlotIdx;

		conIt->second.SetVisible( true );
		conIt->second.ChangeItemIcon( (*productit).nIconID );
		conIt->second.ChangeBtnData( (*productit).dwGroupID, (*productit).dwFirstProductIDX );
		conIt->second.ChangePrice( (*productit).nShowPrice, (*productit).nStandardPrice );
		conIt->second.ChangeSale( (*productit).nSaleIconIdx );
		conIt->second.ChangeItemName( (*productit).wsName );
		conIt->second.SetHitType( (*productit).nMaskType );
		conIt->second.SetEndSale( (*productit).IsPurchaseEnable() );
		++nSlotIdx;
		++n;
	}

	if( m_pkItems )
	{
		m_pkItems->releaseSelection();
		if( nSelectIdx > -1 )
			m_pkItems->SetSelectedItemFromIdx(nSelectIdx, false);		
	}
}

//////////////////////////////////////////////////////////////////////////
void NewCashshopSeliingItemViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case NewCashshopContents::Move_Page:
		{
			int nMoveMain;
			int nMoveSub;
			int nMovePage;
			DWORD dwSelectGroup;
			kStream >> nMoveMain >> nMoveSub >> nMovePage >> dwSelectGroup;
			UpdatePage( nMoveMain, nMoveSub, nMovePage, dwSelectGroup);
		}
		break;
	case SystemType::Refresh_CurrentPage:
	case SystemType::Update_FilteringKeyword:
		{
			UpdatePage( m_nMainTab, m_nSubTab, 1, 0);
		}
		break;
	case SystemType::Update_CashItem_Buy_History:
		{
			if( m_nMainTab == GetSystem()->GetHistoryIdx() )
				UpdatePage( m_nMainTab, m_nSubTab, 1, 0);
		}
		break;
	}
}

bool NewCashshopSeliingItemViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;
	GetSystem()->Register( SystemType::Move_Page, this );
	GetSystem()->Register( SystemType::Refresh_CurrentPage, this );
	GetSystem()->Register( SystemType::Update_FilteringKeyword, this );	
	GetSystem()->Register( SystemType::Update_CashItem_Buy_History, this );	
	return true;
}