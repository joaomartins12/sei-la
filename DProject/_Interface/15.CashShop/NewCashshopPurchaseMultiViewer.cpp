#include "stdafx.h"
#include "NewCashshopPurchaseMultiViewer.h"

NewCashshopPurchaseMultiViewer::NewCashshopPurchaseMultiViewer()
:m_pkGB_Item(NULL)
{
	Construct();
}

NewCashshopPurchaseMultiViewer::~NewCashshopPurchaseMultiViewer()
{
	Destroy();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void NewCashshopPurchaseMultiViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

bool NewCashshopPurchaseMultiViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void NewCashshopPurchaseMultiViewer::Destroy()
{
	DeleteResource();
}

void NewCashshopPurchaseMultiViewer::DeleteResource()
{
	DeleteScript();
}

void NewCashshopPurchaseMultiViewer::Create( cWindow* pkRoot,  int nValue /*=0*/ )
{
	InitScript(pkRoot, NULL, CsPoint(35,75), CsPoint(350,408), false);

	m_pkGB_Item = NiNew cGridListBox;
	if( m_pkGB_Item )
	{
		m_pkGB_Item->Init( GetRoot(), CsPoint::ZERO,  CsPoint(350,408), CsPoint::ZERO, CsPoint(330,102), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		//m_pkGB_Item->SetBackGorundImg( "Cash\\NewCashShop\\Store_pop_list.tga" );
		m_pkGB_Item->SetBackOverAndSelectedImgRender(false);
		m_pkGB_Item->SetUsePressedMoveEvent(true);
		AddChildControl( m_pkGB_Item );

		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint::ZERO, CsPoint(16,450), 
				cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint(16,450) ), 2, false );
			m_pkGB_Item->SetScrollBar( pScrollBar );
		}
	}
}

void	NewCashshopPurchaseMultiViewer::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

BOOL NewCashshopPurchaseMultiViewer::UpdateMouse(void)
{
	if( m_pkGB_Item && m_pkGB_Item->Update_ForMouse(CURSOR_ST.GetPos()) != cComboBox::CI_INVALIDE )
		return TRUE;
	return FALSE;
}

void	NewCashshopPurchaseMultiViewer::UpdateViewer(NewCashshopContents::PurchaseItems const& kSellItemGroups)
{
	SAFE_POINTER_RET( m_pkGB_Item );

	m_pkGB_Item->RemoveAllItem();

	NewCashshopContents::PurchaseItemConstIter it = kSellItemGroups.begin();
	for( ; it != kSellItemGroups.end(); ++it )
	{
		sCashshop_Item const * pProductItemInfo  = GetSystem()->GetProductItemInfo( (*it) );
		SAFE_POINTER_CON( pProductItemInfo );

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

		if( neCashsopHitType::None != pProductItemInfo->nMaskType )
		{
			cImage* pHitIcon = NiNew cImage;
			pHitIcon->Init( NULL, CsPoint::ZERO, CsPoint(64,64), "Cash\\NewCashShop\\CashShop_Ribbon.tga", false );
			pHitIcon->SetTexToken( CsPoint( 0, 64 ) );
			cString::sIMAGE* pImage = pkItemViewer->AddImage( pHitIcon, pProductItemInfo->nMaskType, CsPoint::ZERO, CsPoint(64,64)  );
			if( pImage )
				pImage->SetAutoPointerDelete(true);
		}

		if( 0 < pProductItemInfo->nSalePersent )
		{
			cImage* pkSaleIcon = NiNew cImage;
			if( pkSaleIcon )
			{
				pkSaleIcon->Init( NULL, CsPoint::ZERO, CsPoint(80,80), "Cash\\NewCashShop\\CashShop_Ribon.tga", false );
				pkSaleIcon->SetTexToken( CsPoint( 80, 80 ) );
				cString::sIMAGE* pImage = pkItemViewer->AddImage( pkSaleIcon, pProductItemInfo->nSalePersent / 5 + 4, CsPoint(22,13), CsPoint(80,80) );
				if( pImage )
					pImage->SetAutoPointerDelete(true);
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

		{
			switch( pProductItemInfo->nDispType )
			{
			case neCashshopPurchaseType::DT_NONE:		
				break;
#ifndef VERSION_KOR
			case neCashshopPurchaseType::DT_COUNT:
				{
					cSprite* pkEachIcon = NiNew cSprite;
					if( pkEachIcon )
					{
						pkEachIcon->Init( NULL, CsPoint::ZERO, CsPoint( 16, 10 ), "Cash\\Shop\\Each.tga", false );
						cString::sSPRITE* pEachIcon = pkItemViewer->AddSprite( pkEachIcon, CsPoint(120,77) );
						if( pEachIcon )
							pEachIcon->SetAutoPointerDelete(true);
					}

					cText::sTEXTINFO ti;
					ti.Init( CFont::FS_12 );
					ti.s_eTextAlign = DT_LEFT;
					ti.SetText( pProductItemInfo->nDispCount );
					pkItemViewer->AddText(&ti, CsPoint(140,75));
				}
				break;
			case neCashshopPurchaseType::DT_DAY:
				{
					cSprite* pkDayIcon = NiNew cSprite;
					if( pkDayIcon )
					{
						pkDayIcon->Init( NULL, CsPoint::ZERO, CsPoint( 14, 15 ), "Cash\\Shop\\Day.tga", false );
						cString::sSPRITE* pDayIcon = pkItemViewer->AddSprite( pkDayIcon, CsPoint(120,75) );
						if( pDayIcon )
							pDayIcon->SetAutoPointerDelete(true);
					}

					cText::sTEXTINFO ti;
					ti.Init( CFont::FS_12 );
					ti.s_eTextAlign = DT_LEFT;
					ti.SetText( pProductItemInfo->nDispCount );
					pkItemViewer->AddText(&ti, CsPoint(140,75));
				}
				break;
			case neCashshopPurchaseType::DT_PERENNITY:
				{
					cSprite* pkLongIcon = NiNew cSprite;
					if( pkLongIcon )
					{
						pkLongIcon->Init( NULL, CsPoint::ZERO, CsPoint( 17, 9 ), "Cash\\Shop\\Long.tga", false );
						cString::sSPRITE* pLongIcon = pkItemViewer->AddSprite( pkLongIcon, CsPoint(120,78) ); 
						if( pLongIcon )
							pLongIcon->SetAutoPointerDelete(true);
					}

					cText::sTEXTINFO ti;
					ti.Init( CFont::FS_12 );
					ti.s_eTextAlign = DT_LEFT;
					ti.SetText( pProductItemInfo->nDispCount );
					pkItemViewer->AddText(&ti, CsPoint(140,75));
				}
				break;
#else
			case neCashshopPurchaseType::DT_COUNT:
				{
					std::wstring wsCount;
					DmCS::StringFn::Format( wsCount, L"%d %s", pProductItemInfo->nDispCount, UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );

					cText::sTEXTINFO ti;
					ti.Init( CFont::FS_12 );
					ti.SetText( wsCount.c_str() );
					pkItemViewer->AddText(&ti, CsPoint(120,75));
				}
				break;
			case neCashshopPurchaseType::DT_DAY:
				{
					std::wstring wsCount;
					DmCS::StringFn::Format( wsCount, _T("%d %s") , pProductItemInfo->nDispCount, UISTRING_TEXT( "COMMON_TXT_DAY" ).c_str() );

					cText::sTEXTINFO ti;
					ti.Init( CFont::FS_12 );
					ti.SetText(wsCount.c_str());
					pkItemViewer->AddText(&ti, CsPoint(120,75));
				}
				break;
			case neCashshopPurchaseType::DT_PERENNITY:
				{
					cText::sTEXTINFO ti;
					ti.Init( CFont::FS_12 );
					ti.SetText( UISTRING_TEXT( "CASHSHOP_PURCHASE_PERENNITY" ).c_str() );
					pkItemViewer->AddText(&ti, CsPoint(120,75));
				}
				break;
#endif
			}

			cText::sTEXTINFO ti;
			ti.Init( CFont::FS_12 );
			ti.s_eTextAlign = DT_RIGHT;
			ti.s_Color = FONT_YELLOW;

			std::wstring sPrice;
#ifdef VERSION_STEAM
			DmCS::StringFn::Format( sPrice, L"%.2f", pProductItemInfo->nRealSellingPrice * 0.01 );
#else
			sPrice = DmCS::StringFn::getNumberFormatW(pProductItemInfo->nRealSellingPrice);
#endif
			ti.SetText(sPrice.c_str());

			pkItemViewer->AddText( &ti, CsPoint(270,75) );
		}

		//CASH ICON
		cSprite* pkCashIcon = NiNew cSprite;
#ifndef VERSION_STEAM
		pkCashIcon->Init( NULL, CsPoint::ZERO, CsPoint(24, 24),  "Cash\\NewCashShop\\Store_icon_C.tga", false );
#else 
		pkCashIcon->Init( NULL, CsPoint::ZERO, CsPoint(24, 24),  "Cash\\NewCashShop\\Store_icon_S.tga", false );
#endif
		sSprite = pkItemViewer->AddSprite( pkCashIcon, CsPoint(280,70), CsPoint(24,24) );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(0, CsPoint(330,102));
		addItem->SetItem( pkItemViewer );
		m_pkGB_Item->AddItem( addItem );
	}
}