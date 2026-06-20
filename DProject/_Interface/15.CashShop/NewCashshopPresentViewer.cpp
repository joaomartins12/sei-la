#include "stdafx.h"
#include "NewCashshopPresentViewer.h"

NewCashshopPresentViewer::NewCashshopPresentViewer()
:m_pkGB_Item(NULL),
m_plist_Desc(NULL),
m_pkItemTypeCB(NULL)
{
	Construct();
}
NewCashshopPresentViewer::~NewCashshopPresentViewer()
{
	Destroy();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}
void NewCashshopPresentViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

bool NewCashshopPresentViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}
void NewCashshopPresentViewer::Destroy()
{
	DeleteResource();
}
void NewCashshopPresentViewer::DeleteResource()
{
	DeleteScript();
}
void NewCashshopPresentViewer::Create( cWindow* pkRoot,  int nValue /*=0*/ )
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

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14 );
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText(UISTRING_TEXT("CASHSHOP_PRESENT_WINDOW_TEXT_FRIENT_SELECT").c_str());//L"친구 선택"
		AddText( &ti, CsPoint(100,115));
	}

	AddSprite(CsPoint(-8,140), CsPoint(350,280), "Cash\\NewCashShop\\Store_pop_textbox.tga");	

	m_plist_Desc = NiNew cListBox;
	if( m_plist_Desc )
	{
		m_plist_Desc->Init( GetRoot(), CsPoint(10,160), CsPoint(320,250), NULL, false );	
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint::ZERO, CsPoint(16,300), 
				cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint(16,300) ), 2, false );
			m_plist_Desc->SetScrollBar( pScrollBar );
		}
		AddChildControl( m_plist_Desc );
	}

	m_pkFriendCB = NiNew cComboBox;
	if( m_pkFriendCB )
	{
		m_pkFriendCB->Init( GetRoot(), cComboBox::CT_NORMAL, CsPoint(120,115), CsPoint(190,22), CFont::FS_12, DT_CENTER, false, false, false, true);
		m_pkFriendCB->ChangeComboBoxImg( "SealMaster\\btn_combobox.tga", CsPoint(0, 22));
		m_pkFriendCB->SetComboListWindow( "SealMaster\\dropdown_list_0" );
		//m_pkFriendCB->AddEvent(cComboBox::COMBO_SELECTITEM, this, &NewCashshopPresentViewer::OnClick_ItemTypeCB);
		AddChildControl( m_pkFriendCB );
	}

	m_pkItemTypeCB = NiNew cComboBox;
	if( m_pkItemTypeCB )
	{
		m_pkItemTypeCB->Init( GetRoot(), cComboBox::CT_NORMAL, CsPoint(120,70), CsPoint(190,22), CFont::FS_12, DT_CENTER, false, false, false, true);
		m_pkItemTypeCB->ChangeComboBoxImg( "SealMaster\\btn_combobox.tga", CsPoint(0, 22));
		m_pkItemTypeCB->SetComboListWindow( "SealMaster\\dropdown_list_0" );
		m_pkItemTypeCB->AddEvent(cComboBox::COMBO_SELECTITEM, this, &NewCashshopPresentViewer::OnClick_ItemTypeCB);
		AddChildControl( m_pkItemTypeCB );
	}
}

void	NewCashshopPresentViewer::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

BOOL NewCashshopPresentViewer::UpdateMouse(void)
{
	if( m_pkItemTypeCB && m_pkItemTypeCB->Update_ForMouse() != cComboBox::CI_INVALIDE )
		return TRUE;

	if( m_pkFriendCB && m_pkFriendCB->Update_ForMouse() != cComboBox::CI_INVALIDE )
		return TRUE;

	if( m_plist_Desc && m_plist_Desc->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;

	return FALSE;
}

void	NewCashshopPresentViewer::UpdateViewer(NewCashshopContents::PurchaseItems const& kSellItemGroups)
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
	g_pStringAnalysis->Cut_Parcing( &kName, 330, pProductItemInfo->wsName.c_str(), &ti );

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
	cGridListBoxItem * addItem  = NiNew cGridListBoxItem(0, CsPoint(330,102));
	addItem->SetItem( pkItemViewer );
	m_pkGB_Item->AddItem( addItem );

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// 수량 가격 콤보 박스
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	if( m_pkItemTypeCB )
	{
		m_pkItemTypeCB->RemoveAllItem();
		m_pkItemTypeCB->SetVisibleRenderPopup(false);
		NewCashshopContents::CASHITEM_TABLE_CIT subIt = pProductItemInfo->pTableInfo.begin();
		for( ; subIt !=  pProductItemInfo->pTableInfo.end(); ++subIt)
		{
			cString* pString = NiNew cString;				
			_MakeString( *subIt, pString );	
			m_pkItemTypeCB->AddItem( pString, 0, FONT_WHITE );
		}

		m_pkItemTypeCB->SetCurSel(0, false);
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

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// 수량 가격 콤보 박스
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	if( m_pkFriendCB )
	{
		m_pkFriendCB->RemoveAllItem();
		m_pkFriendCB->SetVisibleRenderPopup(false);
		std::list<std::wstring> friendList;
		GetSystem()->GetFriendList( friendList );

		std::list<std::wstring>::iterator it = friendList.begin();
		for( ; it != friendList.end(); ++it )
			m_pkFriendCB->AddItem( &(*it) );
		
		//m_pkFriendCB->SetCurSel((*it).IndexInGroup, false);
	}
}

void	NewCashshopPresentViewer::OnClick_ItemTypeCB(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	int nSelIdx = *static_cast<int*>(pData);

	GetSystem()->OnChange_PurchaseItem(nSelIdx);	
}

std::wstring NewCashshopPresentViewer::GetPresentFriendName()
{
	std::wstring friendName;
	SAFE_POINTER_RETVAL( m_pkFriendCB, friendName );

	cString* pSel = m_pkFriendCB->GetSelectedItem();
	SAFE_POINTER_RETVAL( pSel, friendName );

	cString::sELEMENT* pEl = pSel->GetElement( 0 );
	SAFE_POINTER_RETVAL( pEl, friendName );

	if( cString::sELEMENT::TEXT != pEl->GetType() )
		return friendName;
	friendName = static_cast<cString::sTEXT*>(pEl)->s_Text.GetText();
	return friendName;
}