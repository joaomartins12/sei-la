#include "stdafx.h"
#include "NewCashshopCartViewer.h"

bool NewCashshopCartViewer::sCartControls::IsEmpty() const
{
	SAFE_POINTER_RETVAL( pkItemIcon, false );
	return !pkItemIcon->GetVisible();
}

void NewCashshopCartViewer::sCartControls::SetEmpty()
{
	if( pkItemIcon )
		pkItemIcon->SetVisible(false);
	if( pItemControl )
	{
		CartItemInfo* pInfo = dynamic_cast<CartItemInfo*>(pItemControl->GetUserData());
		if( pInfo )
			pInfo->dwProductIDX = 0;
	}
}

void NewCashshopCartViewer::sCartControls::ChangeItemData( DWORD const& dwProductIDX )
{
	SAFE_POINTER_RET( pItemControl );
	CartItemInfo* pInfo = dynamic_cast<CartItemInfo*>(pItemControl->GetUserData());
	if( pInfo )
		pInfo->dwProductIDX = dwProductIDX;
}

void NewCashshopCartViewer::sCartControls::ChangeItemIcon( DWORD const& dwIconID )
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
	pkItemIcon->SetVisible(true);
}

NewCashshopCartViewer::NewCashshopCartViewer()
:m_pkItems(NULL)
{
	Construct();
}
NewCashshopCartViewer::~NewCashshopCartViewer()
{
	Destroy();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void NewCashshopCartViewer::Destroy()
{
	DeleteResource();
}

void NewCashshopCartViewer::DeleteResource()
{
	DeleteScript();
}

void NewCashshopCartViewer::Create( cWindow* pkRoot,  int nValue /*=0*/ )
{
	InitScript( pkRoot, NULL /*"Cash\\NewCashShop\\TestBG.tga"*/, CsPoint(710,570), CsPoint(300,110), false);

	m_pkItems = NiNew cGridListBox;
	if( m_pkItems )
	{
		m_pkItems->Init( GetRoot(), CsPoint(0,3), CsPoint(300,110), 
			CsPoint::ZERO, CsPoint(50,50), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		AddChildControl(m_pkItems);
		m_pkItems->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &NewCashshopCartViewer::OnRClicked_CartItem );
		m_pkItems->AddEvent( cGridListBox::GRID_SELECTED_ITEM, this, &NewCashshopCartViewer::OnSelected_CartItem );
		m_pkItems->SetUsePressedMoveEvent(true);
		m_pkItems->SetMouseOverImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pkItems->SetSelectedImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pkItems->SetBackOverAndSelectedImgRender(false);
	}

	
	_CreateCartSlot();
}

void NewCashshopCartViewer::_CreateCartSlot()
{
	SAFE_POINTER_RET( m_pkItems );

	NewCashshopContents::Baskets const& baskets = GetSystem()->GetBasketsItemAll();
	NewCashshopContents::BasketCIter it = baskets.begin();
	
	m_mapControls.clear();

	for( ; it != baskets.end(); ++it )
	{		
		cString * pItem = NiNew cString;
		cSprite* pImage = NiNew cSprite;	// 1
		pImage->Init( NULL, CsPoint::ZERO, CsPoint(50,50),  "Cash\\NewCashShop\\Store_bg_slot.tga", false );
		cString::sSPRITE* sSprite = pItem->AddSprite( pImage, CsPoint::ZERO, CsPoint(50,50) );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);

		sCartControls addItem;		
		addItem.pkItemIcon = NiNew cSprite;
		if( addItem.pkItemIcon )
		{
			addItem.pkItemIcon->Init( NULL, CsPoint::ZERO, CsPoint(46,46),  CsRect(CsPoint(0,0),CsPoint(0, 0)), "", false );
			sSprite = pItem->AddSprite(addItem.pkItemIcon, CsPoint(2,2), CsPoint(46,46) );	
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);
			addItem.pkItemIcon->SetVisible(false);
		}
		
		addItem.pItemControl = NiNew cGridListBoxItem(it->first, CsPoint(50,50));
		addItem.pItemControl->SetItem( pItem );
		addItem.pItemControl->SetUserData(new CartItemInfo(0));

		if( 0 != it->second )
		{
			sCashshop_Item const* pItemInfo = GetSystem()->GetProductItemInfo( it->second );
			if( pItemInfo )
			{
				addItem.ChangeItemData( it->second );
				addItem.ChangeItemIcon( pItemInfo->nIconID );
			}
		}
		
		m_pkItems->AddItem( addItem.pItemControl );

		m_mapControls.insert( std::make_pair( it->first, addItem ) );
	}
}

void NewCashshopCartViewer::Update(float const& fDeltaTime)
{
}

BOOL NewCashshopCartViewer::UpdateMouse(void)
{
	if(m_pkItems && m_pkItems->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;
	return FALSE;
}

void	NewCashshopCartViewer::OnSelected_CartItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	CartItemInfo* pkCartInfo =	dynamic_cast<CartItemInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET( pkCartInfo );

	BHPRT("OnSelected_CartItem [%d]", pkCartInfo->dwProductIDX);
	if(0 == pkCartInfo->dwProductIDX )
		return;

	GetSystem()->OnClick_ShowPurchaseItem( NewCashshopContents::PURCHASE, pkCartInfo->dwProductIDX );
	//GetSystem()->Move_Item( pkBestInfo->dwProductIDX );
}

void	NewCashshopCartViewer::OnRClicked_CartItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	CartItemInfo* pkCartInfo =	dynamic_cast<CartItemInfo*>(pItem->GetUserData());
	SAFE_POINTER_RET( pkCartInfo );
	BHPRT("OnRClicked_CartItem [%d]", pkCartInfo->dwProductIDX);
	if(0 == pkCartInfo->dwProductIDX )
		return;

	GetSystem()->OnClick_RemoveItemInBasket( pItem->getID(), pkCartInfo->dwProductIDX);
}

void NewCashshopCartViewer::_InsertItem( int const& nSlotNum, DWORD const& dwProductIDX )
{
	sCashshop_Item const* pItemInfo = GetSystem()->GetProductItemInfo( dwProductIDX );
	SAFE_POINTER_RET( pItemInfo );
	std::map<int,sCartControls>::iterator it = m_mapControls.find( nSlotNum );
	if( it == m_mapControls.end() )
		return;

	it->second.ChangeItemData( dwProductIDX );
	it->second.ChangeItemIcon( pItemInfo->nIconID );
}

void NewCashshopCartViewer::_DeleteItem( int const& nSlotNum )
{
	std::map<int,sCartControls>::iterator it = m_mapControls.find( nSlotNum );
	if( it == m_mapControls.end() )
		return;

	it->second.SetEmpty();
}

void NewCashshopCartViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::CashShop_Cart_InsertItem:
		{
			int nSlotNum = -1;
			DWORD dwProductIDX = 0;
			kStream >> nSlotNum >> dwProductIDX;
			_InsertItem(nSlotNum, dwProductIDX);
		}
		break;
	case SystemType::CashShop_Cart_DeleteItem:
		{
			int nSlotNum = 0;
			kStream >> nSlotNum;
			_DeleteItem(nSlotNum);
		}
		break;
	}
}

bool NewCashshopCartViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::CashShop_Cart_InsertItem, this );
	GetSystem()->Register( SystemType::CashShop_Cart_DeleteItem, this );

	return true;
}
