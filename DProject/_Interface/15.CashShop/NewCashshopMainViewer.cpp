#include "stdafx.h"
#include "NewCashshopMainViewer.h"

NewCashshopMainViewer::NewCashshopMainViewer()
:m_pkUpperItems(NULL),
m_pkMiddleItems(NULL),
m_pkBottomItems(NULL)
{
	Construct();
}

NewCashshopMainViewer::~NewCashshopMainViewer()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	Destroy();
}

void NewCashshopMainViewer::Destroy()
{
	DeleteResource();
}

void NewCashshopMainViewer::DeleteResource()
{
	DeleteScript();
}

void NewCashshopMainViewer::Create( cWindow* pkRoot,  int nValue /*=0*/ )
{
	InitScript( pkRoot, NULL, CsPoint(16,106), CsPoint(551,571), false);
	m_pkUpperItems = _CreateItemControl( neCashsopHitType::New, CsPoint::ZERO, CsPoint(551,190), CsPoint::ZERO );
	m_pkMiddleItems = _CreateItemControl( neCashsopHitType::Hot, CsPoint(0,190), CsPoint(551,190), CsPoint::ZERO );
	m_pkBottomItems = _CreateItemControl( neCashsopHitType::Event, CsPoint(0,380), CsPoint(551,190), CsPoint::ZERO );
}

void	NewCashshopMainViewer::Update(float const& fDeltaTime)
{
}

BOOL NewCashshopMainViewer::UpdateMouse(void)
{
	BOOL bResult = FALSE;
	if( m_pkUpperItems && m_pkUpperItems->Update_ForMouse(CURSOR_ST.GetPos()) != cComboBox::CI_INVALIDE )
		bResult |= TRUE;
	if( m_pkMiddleItems && m_pkMiddleItems->Update_ForMouse(CURSOR_ST.GetPos()) != cComboBox::CI_INVALIDE )
		bResult |= TRUE;
	if( m_pkBottomItems && m_pkBottomItems->Update_ForMouse(CURSOR_ST.GetPos()) != cComboBox::CI_INVALIDE )
		bResult |= TRUE;
	return bResult;
}

cGridListBox * NewCashshopMainViewer::_CreateItemControl(int const& nSubCategory, const CsPoint& GridStartPos, const CsPoint& GridSize, const CsPoint& SlotGap)
{
	cGridListBox * gridControl = NiNew cGridListBox;
	SAFE_POINTER_RETVAL( gridControl, NULL );

	gridControl->Init( GetRoot(), GridStartPos, GridSize, 
		SlotGap, CsPoint(136,190), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
	gridControl->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &NewCashshopMainViewer::_OnItemSelected_BestItem );
	gridControl->SetMouseOverImg( "Cash\\NewCashShop\\Store_main_glow.tga" );
	gridControl->SetAutoSelection(false);
	gridControl->SetBackOverAndSelectedImgRender(false);
	AddChildControl( gridControl );

	const CsPoint TYPEICON_SIZE = CsPoint( 92, 44 );

	NewCashshopContents::LIST_PRODUCTGROUP_INFO const * pProductInfo = GetSystem()->GetCategoryItems( NewCashshopContents::eMain, nSubCategory );
	SAFE_POINTER_RETVAL( pProductInfo, NULL );
	NewCashshopContents::LIST_PRODUCTGROUP_INFO_CIT it = pProductInfo->begin();
	for( int iSlotIndex = 0; it != pProductInfo->end(); ++it, ++iSlotIndex )
	{
		if( !(*it).IsPurchaseEnable() )
			continue;

		//BG
		cString * pItem = NiNew cString;
		cSprite* pImage = NiNew cSprite;	// 1
		pImage->Init( NULL, CsPoint::ZERO, CsPoint(24, 34),  "Cash\\NewCashShop\\Store_main.tga", false );
		cString::sSPRITE* sSprite = pItem->AddSprite( pImage, CsPoint::ZERO, CsPoint(136,190) );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);

		ICON_Dynamic::eTYPE nIconType = (ICON_Dynamic::eTYPE)((*it).nIconID / 100);
		cIcon* pkIcon = g_pIconMng->GetIcon(nIconType);
		if(pkIcon)
		{
			cSprite* IconSprite = g_pIconMng->GetSprite(nIconType);
			if( IconSprite )
			{
				int nCashshopItemIdx = (*it).nIconID % 100;	
				int nColInTextrue = nCashshopItemIdx%pkIcon->GetIndexCount().x;
				int nRowInTextrue = nCashshopItemIdx/pkIcon->GetIndexCount().x;
				int Size = IconSprite->GetSize().x;

				cSprite* pkItem = NiNew cSprite;
				pkItem->Init( NULL, IconSprite->GetPos(), IconSprite->GetSize(),  CsRect(CsPoint(nColInTextrue * Size, nRowInTextrue * Size), 
					CsPoint((nColInTextrue + 1) * Size , (nRowInTextrue + 1) * Size)), IconSprite->GetFilePath().c_str(), false );
				sSprite = pItem->AddSprite(pkItem, CsPoint(28,16), CsPoint(80,80) );	
				if( sSprite )
					sSprite->SetAutoPointerDelete(true);
			}
		}		

		//ÁÂ»ó´Ü Ribbon
		cImage* pHitIcon = NiNew cImage;
		if( pHitIcon )
		{
			pHitIcon->Init( NULL, CsPoint::ZERO, CsPoint(64,64), "Cash\\NewCashShop\\CashShop_Ribbon.tga", false );
			pHitIcon->SetTexToken( CsPoint( 0, 64 ) );
			cString::sIMAGE* pImage = pItem->AddImage( pHitIcon, nSubCategory, CsPoint(4,4), CsPoint(64,64)  );
			if( pImage )
				pImage->SetAutoPointerDelete(true);
		}

		cImage* pkSaleIcon = NiNew cImage;
		if( pkSaleIcon )
		{
			pkSaleIcon->Init( NULL, CsPoint::ZERO, CsPoint(80,80), "Cash\\NewCashShop\\CashShop_Ribon.tga", false );
			pkSaleIcon->SetTexToken( CsPoint( 80, 80 ) );
			cString::sIMAGE* pImage = pItem->AddImage( pkSaleIcon, (*it).nSaleIconIdx, CsPoint(30,16), CsPoint(80,80) );
			if( pImage )
				pImage->SetAutoPointerDelete(true);
		}

		//Name 
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_10 );
		ti.s_eTextAlign = DT_CENTER;
		cStringList pkName;
		g_pStringAnalysis->Cut_Parcing( &pkName, 117, (*it).wsName.c_str(), &ti );

		std::list< cString* >* pkList = pkName.GetList();
		std::list< cString* >::iterator kIter = pkList->begin();
		CsPoint RenderPos = CsPoint(68,105);

		for(; kIter != pkList->end(); ++kIter)
		{	
			CsPoint csPos(0,0);
			for(int i= 0 ; i < (*kIter)->GetListSize(); ++i)
			{
				cString::sTEXT* pText = (cString::sTEXT*)(*kIter)->GetElement(i);
				pItem->AddText(pText->s_Text.GetTextInfo(), csPos + RenderPos);	
				csPos.x += pText->s_ptSize.x;
			}

			RenderPos.y += (*kIter)->GetMaxSize().y;
		}
		//TYPE ICON
		cSprite* pkTypeIcon = NiNew cSprite;
		CsPoint kTypeIconLT =  CsPoint(0.0f, TYPEICON_SIZE.y * (*it).nMaskType);
		CsPoint kTypeIconRB =  CsPoint(TYPEICON_SIZE.x, TYPEICON_SIZE.y * ((*it).nMaskType+1));
		pkTypeIcon->Init( NULL, CsPoint::ZERO, CsPoint(92, 44),  CsRect(kTypeIconLT, kTypeIconRB), "Cash\\NewCashShop\\Store_typo.tga", false );
		sSprite = pItem->AddSprite( pkTypeIcon, CsPoint(20,145), TYPEICON_SIZE );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);


		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(iSlotIndex, CsPoint(136,190));
		addItem->SetItem( pItem );
		addItem->SetUserData(new MainItemInfo((*it).dwFirstProductIDX));
		gridControl->AddItem( addItem );
	}

	return gridControl;
}	

void NewCashshopMainViewer::_OnItemSelected_BestItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	MainItemInfo* pkMainInfo =	dynamic_cast<MainItemInfo*>(pItem->GetUserData());
	BHPRT("OnItemSelected_MainItem CT[%d] SUB_CT[%d] ID[%d]",pkMainInfo->dwProductID);
	GetSystem()->Move_Item(pkMainInfo->dwProductID);
}


//////////////////////////////////////////////////////////////////////////
void NewCashshopMainViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

bool NewCashshopMainViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}