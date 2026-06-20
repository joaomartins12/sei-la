#include "stdafx.h"
#include "NewCashshopPurchaseSingleViewer.h"

NewCashshopPurchaseSingleViewer::NewCashshopPurchaseSingleViewer()
:m_pkGB_Item(NULL),
m_plist_Desc(NULL),
m_pkItemTypeCB(NULL)
{
	Construct();
}
NewCashshopPurchaseSingleViewer::~NewCashshopPurchaseSingleViewer()
{
	Destroy();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}
void NewCashshopPurchaseSingleViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

bool NewCashshopPurchaseSingleViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}
void NewCashshopPurchaseSingleViewer::Destroy()
{
	DeleteResource();
}
void NewCashshopPurchaseSingleViewer::DeleteResource()
{
	DeleteScript();
}
void NewCashshopPurchaseSingleViewer::Create( cWindow* pkRoot,  int nValue /*=0*/ )
{
	InitScript(pkRoot, NULL, CsPoint(35,75), CsPoint(330,400), false);

	m_pkGB_Item = NiNew cGridListBox;
	if( m_pkGB_Item )
	{
		m_pkGB_Item->Init( GetRoot(), CsPoint::ZERO,  CsPoint(330,102), CsPoint::ZERO, CsPoint(330,102), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		//m_pkGB_Item->SetBackGorundImg( "Cash\\NewCashShop\\Store_pop_list.tga" );
		m_pkGB_Item->SetBackOverAndSelectedImgRender(false);
		m_pkGB_Item->SetUsePressedMoveEvent(true);
		AddChildControl( m_pkGB_Item );
	}

	AddSprite(CsPoint(-8,100), CsPoint(350,320), "Cash\\NewCashShop\\Store_pop_textbox.tga");	

	m_plist_Desc = NiNew cListBox;
	if( m_plist_Desc )
	{
		m_plist_Desc->Init( GetRoot(), CsPoint(10,120), CsPoint(320,280), NULL, false );	
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint::ZERO, CsPoint(16,300), 
				cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint(16,300) ), 2, false );
			m_plist_Desc->SetScrollBar( pScrollBar );
		}
		AddChildControl( m_plist_Desc );
	}
	
	m_pkItemTypeCB = NiNew cComboBox;
	if( m_pkItemTypeCB )
	{
		m_pkItemTypeCB->Init( GetRoot(), cComboBox::CT_NORMAL, CsPoint(120,70), CsPoint(190,22), CFont::FS_12, DT_CENTER, false, false, false, true);
		m_pkItemTypeCB->ChangeComboBoxImg( "SealMaster\\btn_combobox.tga", CsPoint(0, 22));
		m_pkItemTypeCB->SetComboListWindow( "SealMaster\\dropdown_list_0" );
		m_pkItemTypeCB->AddEvent(cComboBox::COMBO_SELECTITEM, this, &NewCashshopPurchaseSingleViewer::OnClick_ItemTypeCB);
		AddChildControl( m_pkItemTypeCB );
	}
}

void	NewCashshopPurchaseSingleViewer::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

BOOL NewCashshopPurchaseSingleViewer::UpdateMouse(void)
{
	if( m_pkItemTypeCB && m_pkItemTypeCB->Update_ForMouse() != cComboBox::CI_INVALIDE )
		return TRUE;

	if( m_plist_Desc && m_plist_Desc->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;
	
	return FALSE;
}

void	NewCashshopPurchaseSingleViewer::UpdateViewer(NewCashshopContents::PurchaseItems const& kSellItemGroups)
{
	SAFE_POINTER_RET( m_pkGB_Item );

	//싱글 뷰어에서는 한 가지 아이템이 아니면 리턴시킨다.
	if(kSellItemGroups.size() != 1)
		return;

	m_pkGB_Item->RemoveAllItem();

	NewCashshopContents::PurchaseItemConstIter it = kSellItemGroups.begin();
	NewCashshopContents::sProductItemInfo const * pProductItemInfo  = GetSystem()->GetCashshop_ItemGropInfo( (*it) );
	SAFE_POINTER_RET( pProductItemInfo );	
	
	cString * pkItemViewer = NiNew cString;
	/////////////////////////////////////////////////////////
	//배경
	cSprite* pkBG = NiNew cSprite;
	pkBG->Init( NULL, CsPoint::ZERO, CsPoint(330,102),  "Cash\\NewCashShop\\Store_pop_list.tga", false );
	cString::sSPRITE* sSprite = pkItemViewer->AddSprite( pkBG, CsPoint::ZERO, CsPoint(330,102) );	
	if( sSprite )
		sSprite->SetAutoPointerDelete(true);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//아이템 이미지
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	ICON_Dynamic::eTYPE nIconType = (ICON_Dynamic::eTYPE)(pProductItemInfo->nIconID / 100);
	cIcon* pkIcon = g_pIconMng->GetIcon(nIconType);
	if( pkIcon )
	{
		cSprite* IconSprite = g_pIconMng->GetSprite(nIconType);
		if( IconSprite )
		{
			int nCashshopItemIdx = pProductItemInfo->nIconID % 100;	
			int nColInTextrue = nCashshopItemIdx%pkIcon->GetIndexCount().x;
			int nRowInTextrue = nCashshopItemIdx/pkIcon->GetIndexCount().x;
			int Size = IconSprite->GetSize().x;

			cSprite* pkItem = NiNew cSprite;
			pkItem->Init( NULL, IconSprite->GetPos(), IconSprite->GetSize(),  CsRect(CsPoint(nColInTextrue * Size, nRowInTextrue * Size), 
				CsPoint((nColInTextrue + 1) * Size , (nRowInTextrue + 1) * Size)), IconSprite->GetFilePath().c_str(), false );
			sSprite = pkItemViewer->AddSprite(pkItem, CsPoint(20,9), CsPoint(82,82) );	
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);
		}
	}		

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//이름
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	cText::sTEXTINFO ti;
	ti.Init(CFont::FS_12 );
	ti.s_eTextAlign = DT_CENTER;
	cStringList kName;
	g_pStringAnalysis->Cut_Parcing( &kName, 320, pProductItemInfo->wsName.c_str(), &ti );

	std::list< cString* >* pkList = kName.GetList();
	std::list< cString* >::iterator kIter = pkList->begin();
	CsPoint RenderPos = CsPoint(220,20);
	CsPoint PrevPos = CsPoint::ZERO;

	for(; kIter != pkList->end(); ++kIter)
	{	
		cString::sTEXT* pText = (cString::sTEXT*)(*kIter)->GetElement(0);
		pkItemViewer->AddText(pText->s_Text.GetTextInfo(), RenderPos);
		RenderPos.y += pText->s_ptSize.y;
	}

	if( neCashsopHitType::None != pProductItemInfo->nMaskType )
	{
		cImage* pHitIcon = NiNew cImage;
		pHitIcon->Init( NULL, CsPoint::ZERO, CsPoint(64,64), "Cash\\NewCashShop\\CashShop_Ribbon.tga", false );
		pHitIcon->SetTexToken( CsPoint( 0, 64 ) );
		cString::sIMAGE* pImage = pkItemViewer->AddImage( pHitIcon, pProductItemInfo->nMaskType, CsPoint::ZERO, CsPoint(64,64)  );
		if( pImage )
			pImage->SetAutoPointerDelete(true);
	}

	cImage* pkSaleIcon = NiNew cImage;
	if( pkSaleIcon )
	{
		pkSaleIcon->Init( NULL, CsPoint::ZERO, CsPoint(80,80), "Cash\\NewCashShop\\CashShop_Ribon.tga", false );
		pkSaleIcon->SetTexToken( CsPoint( 80, 80 ) );
		cString::sIMAGE* pImage = pkItemViewer->AddImage( pkSaleIcon, pProductItemInfo->nSaleIconIdx, CsPoint(22,13), CsPoint(80,80) );
		if( pImage )
			pImage->SetAutoPointerDelete(true);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	cGridListBoxItem * addItem  = NiNew cGridListBoxItem(0, CsPoint(320,102));
	addItem->SetItem( pkItemViewer );
	m_pkGB_Item->AddItem( addItem );

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// 수량 가격 콤보 박스
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	if( m_pkItemTypeCB )
	{
		m_pkItemTypeCB->RemoveAllItem();
		m_pkItemTypeCB->SetVisibleRenderPopup(false);
		int nSelIdx = 0;
		NewCashshopContents::CASHITEM_TABLE_CIT subIt = pProductItemInfo->pTableInfo.begin();
		for( int n = 0; subIt !=  pProductItemInfo->pTableInfo.end(); ++subIt, ++n)
		{
			cString* pString = NiNew cString;				
			_MakeString( *subIt, pString );	
			m_pkItemTypeCB->AddItem( pString, 0, FONT_WHITE );
			if( (*subIt)->dwProductID == (*it) )
				nSelIdx = n;
		}

		m_pkItemTypeCB->SetCurSel(nSelIdx, false);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//설명
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	if( m_plist_Desc )
	{
		m_plist_Desc->RemoveAllItem();
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		cStringList pkDesc;
		g_pStringAnalysis->Cut_Parcing( &pkDesc, 290, pProductItemInfo->wsComment.c_str(), &ti );

		std::list< cString* >* pkDestList = pkDesc.GetList();
		std::list< cString* >::iterator kDescIter = pkDestList->begin();
		for(; kDescIter != pkDestList->end(); ++kDescIter)
		{
			cString * pkCommentViewer = NiNew cString;
			CsPoint csPos(0,0);
			for(int i= 0 ; i < (*kDescIter)->GetListSize(); ++i)
			{
				cString::sTEXT* pText = (cString::sTEXT*)(*kDescIter)->GetElement(i);
				pkCommentViewer->AddText(pText->s_Text.GetTextInfo(), csPos);	
				csPos.x += pText->s_ptSize.x;
			}
			cListBoxItem * addItem  = NiNew cListBoxItem;
			addItem->SetItem( pkCommentViewer );
			m_plist_Desc->AddItem( addItem );
		}
	}

// 	/////////////////////////////////////////////////////////////////////////////////////////////////////
// 	//캐시 표시
// 	/////////////////////////////////////////////////////////////////////////////////////////////////////
// 	int  Idx = 0;
// 	SellItemGroupIter kItemIter = pkPurchaseItemGroup->begin();
// 	for(; kItemIter != pkPurchaseItemGroup->end(); ++kItemIter, ++Idx)
// 	{
// 		if(Idx == IdxInGroup)
// 		{
// 			UpdateCashBalance((*kItemIter));
// 			break;
// 		}
// 	}
}

void	NewCashshopPurchaseSingleViewer::OnClick_ItemTypeCB(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	int nSelIdx = *static_cast<int*>(pData);

	GetSystem()->OnChange_PurchaseItem(nSelIdx);	
}