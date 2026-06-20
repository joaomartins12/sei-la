#include "stdafx.h"
#include "NewCashshopBestViewer.h"

NewCashshopBestViewer::NewCashshopBestViewer()
:m_pkItems(NULL)
{
	Construct();
}

NewCashshopBestViewer::~NewCashshopBestViewer()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	Destroy();
}

void NewCashshopBestViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

bool NewCashshopBestViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;
	return true;
}

void NewCashshopBestViewer::Destroy()
{
	DeleteResource();
}

void NewCashshopBestViewer::DeleteResource()
{
	DeleteScript();
}

void NewCashshopBestViewer::Create( cWindow* pkRoot,  int nValue /*=0*/ )
{
	InitScript( pkRoot, "Cash\\NewCashShop\\Store_bg_best.tga", CsPoint(572,110), CsPoint(118,570), false);

	m_pkItems = NiNew cGridListBox;
	if( m_pkItems )
	{
		m_pkItems->Init( GetRoot(), CsPoint(17,43), CsPoint(84,520), 
			CsPoint(0,20), CsPoint(84,84), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		AddChildControl(m_pkItems);
		m_pkItems->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &NewCashshopBestViewer::_OnItemSelected_BestItem );
		m_pkItems->SetUsePressedMoveEvent(true);
		m_pkItems->SetMouseOverImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pkItems->SetAutoSelection(false);
		//m_pkItems->SetSelectedImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pkItems->SetBackOverAndSelectedImgRender(false);
	}
	_MakeBestItemList();
}

void NewCashshopBestViewer::Update(float const& fDeltaTime)
{
}

BOOL NewCashshopBestViewer::UpdateMouse(void)
{
	if(m_pkItems && m_pkItems->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;

	return FALSE;
}

void NewCashshopBestViewer::_MakeBestItemList()
{
	SAFE_POINTER_RET(m_pkItems);
	m_pkItems->RemoveAllItem();

	SAFE_POINTER_RET(g_pIconMng);
	
	std::list<DWORD> RandomGroup;
	GetSystem()->GetRandomCategoryItemGroup( NewCashshopContents::eMain, neCashsopHitType::Best, RandomGroup );

	int nMaxShowCount = 5;
	int nShowCount = 0;
	std::list<DWORD>::const_iterator it = RandomGroup.begin();
	for( ; it != RandomGroup.end(); ++it )
	{		
		NewCashshopContents::sProductItemInfo const* pGroupInfo = GetSystem()->GetCashshop_ItemGropInfo( NewCashshopContents::eMain, neCashsopHitType::Best, *it );
		SAFE_POINTER_CON( pGroupInfo );
		
		cString * pItem = NiNew cString;

		ICON_Dynamic::eTYPE nIconType = (ICON_Dynamic::eTYPE)(pGroupInfo->nIconID / 100);
		cIcon* pkIcon = g_pIconMng->GetIcon(nIconType);
		if( pkIcon )
		{
			cSprite* IconSprite = g_pIconMng->GetSprite(nIconType);
			if( IconSprite )
			{				
				int nCashshopItemIdx = pGroupInfo->nIconID % 100;	
				int nColInTextrue = nCashshopItemIdx%pkIcon->GetIndexCount().x;
				int nRowInTextrue = nCashshopItemIdx/pkIcon->GetIndexCount().x;
				int Size = IconSprite->GetSize().x;

				cSprite* pkItem = NiNew cSprite;
				if( pkItem )
				{
					pkItem->Init( NULL, IconSprite->GetPos(), IconSprite->GetSize(),  CsRect(CsPoint(nColInTextrue * Size, nRowInTextrue * Size), 
						CsPoint((nColInTextrue + 1) * Size , (nRowInTextrue + 1) * Size)), IconSprite->GetFilePath().c_str(), false );
					cString::sSPRITE* sSprite = pItem->AddSprite(pkItem, CsPoint(0,0), CsPoint(84,84) );	
					if( sSprite )
						sSprite->SetAutoPointerDelete(true);
				}
			}
		}

		cImage* pHitIcon = NiNew cImage;
		pHitIcon->Init( NULL, CsPoint::ZERO, CsPoint(64,64), "Cash\\NewCashShop\\CashShop_Ribbon.tga", false );
		pHitIcon->SetTexToken( CsPoint( 0, 64 ) );
		cString::sIMAGE* pImage = pItem->AddImage( pHitIcon, 
			(pGroupInfo->nMaskType == neCashsopHitType::None ? neCashsopHitType::Hot : pGroupInfo->nMaskType),	// default mask = hot
			CsPoint::ZERO, CsPoint(64,64)  );
		if( pImage )
			pImage->SetAutoPointerDelete(true);

		cImage* pkSaleIcon = NiNew cImage;
		if( pkSaleIcon )
		{
			pkSaleIcon->Init( NULL, CsPoint::ZERO, CsPoint(80,80), "Cash\\NewCashShop\\CashShop_Ribon.tga", false );
			pkSaleIcon->SetTexToken( CsPoint( 80, 80 ) );
			cString::sIMAGE* pImage = pItem->AddImage( pkSaleIcon, pGroupInfo->nSaleIconIdx, CsPoint(5,5), CsPoint(80,80) );
			if( pImage )
				pImage->SetAutoPointerDelete(true);
		}

		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(nShowCount, CsPoint(84,84));
		if( addItem )
		{
			addItem->SetItem( pItem );
			addItem->SetUserData( new BestItemInfo( pGroupInfo->dwFirstProductIDX ) );
			m_pkItems->AddItem( addItem );
		}

		nShowCount++;

		if( nShowCount >= nMaxShowCount )
			break;
	}
}

void NewCashshopBestViewer::_OnItemSelected_BestItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	BestItemInfo* pkBestInfo =	dynamic_cast<BestItemInfo*>(pItem->GetUserData());
	BHPRT("OnItemSelected_BestItem Product Item ID[%d] ",pkBestInfo->dwProductID);
	GetSystem()->Move_Item(pkBestInfo->dwProductID);
}