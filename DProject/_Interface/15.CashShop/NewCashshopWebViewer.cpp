#include "stdafx.h"
#include "NewCashshopWebViewer.h"

NewCashshopWebViewer::NewCashshopWebViewer()
:m_pBannerList(NULL)
{

}
NewCashshopWebViewer::~NewCashshopWebViewer()
{
	Destroy();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void NewCashshopWebViewer::Destroy()
{
	DeleteResource();
}

void NewCashshopWebViewer::DeleteResource()
{
	DeleteScript();
}

void NewCashshopWebViewer::Create( cWindow* pkRoot,  int nValue /*=0*/ )
{
	InitScript(pkRoot, NULL, CsPoint(710,110), CsPoint(294,560), false );
	m_pBannerList = NiNew cListBox;
	if( m_pBannerList )
	{
		m_pBannerList->Init( NULL, CsPoint::ZERO, CsPoint(294,560), NULL, false );
		m_pBannerList->SetItemHeight( 280 );
		m_pBannerList->AddEvent( cListBox::LIST_SELECT_CHANGE_ITEM, this, &NewCashshopWebViewer::OnClicked_Banner );
		m_pBannerList->SetMouseOverImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pBannerList->SetSelectedImg( "Cash\\NewCashShop\\Store_win_glow.tga" );
		m_pBannerList->SetBackOverAndSelectedImgRender(false);
		AddChildControl(m_pBannerList);
	}
}

void NewCashshopWebViewer::SetWebImgData(LIST_CASHSHOP_WEBDATA const* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( m_pBannerList );
	m_pBannerList->RemoveAllItem();

	LIST_CASHSHOP_WEBDATA_CIT it = pData->begin();

	for( ; it != pData->end(); ++it )
	{		
		cListBoxItem* pAddItem = NiNew cListBoxItem;
		SAFE_POINTER_BEK( pAddItem );

		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cSprite* pSprite = NiNew cSprite;
		if( pSprite )
		{
			pSprite->Init( NULL, CsPoint::ZERO, CsPoint(294,280), it->sWebImageFile.c_str(), false );

			cString::sSPRITE* paddImg = pItem->AddSprite( pSprite, CsPoint::ZERO, CsPoint(294,280) );
			if( paddImg )
				paddImg->SetAutoPointerDelete(true);
		}

		pAddItem->SetItem( pItem );
		pAddItem->SetUserData( new sWebUrlData(it->sWebLinkUrl) );

		m_pBannerList->AddItem( pAddItem );
	}	
}

void NewCashshopWebViewer::Update(float const& fDeltaTime)
{
}

BOOL NewCashshopWebViewer::UpdateMouse(void)
{
	if(IsVisible() == false)
		return FALSE;
	if( m_pBannerList && m_pBannerList->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;
	return FALSE;
}

void NewCashshopWebViewer::OnClicked_Banner(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cListBoxItem* pItem = static_cast<cListBoxItem*>(pData);
	sWebUrlData* pkInfo =	dynamic_cast<sWebUrlData*>(pItem->GetUserData());
	SAFE_POINTER_RET(pkInfo);
	GetSystem()->OnClick_OpenBanner(pkInfo->sWebUrl);
	if( m_pBannerList )
		m_pBannerList->UnSelectionItem();
}

void NewCashshopWebViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

bool NewCashshopWebViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	return true;
}

