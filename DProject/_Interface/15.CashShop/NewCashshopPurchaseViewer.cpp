#include "stdafx.h"
#include "NewCashshopPurchaseViewer.h"
#include "NewCashshopPurchaseSingleViewer.h"
#include "NewCashshopPurchaseMultiViewer.h"
#include "NewCashshopPresentViewer.h"

NewCashshopPurchaseViewer::NewCashshopPurchaseViewer(void)
:m_pkCloseBT(NULL),
m_pkPurchaseBT(NULL),
m_pkCancelBT(NULL),
m_pkCurrSubViewer(NULL),
m_pkPurchaseSingleViewer(NULL),
m_pkPurchaseMultiViewer(NULL),
m_pkTotalCash(NULL),
m_pkPriceCash(NULL),
m_pkRemainCash(NULL),
m_pTitle(NULL),
m_pkPresentViewer(NULL),
m_nCurrentPurchaseType(NewCashshopContents::NONE)
{
	Construct();
}

NewCashshopPurchaseViewer::~NewCashshopPurchaseViewer()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	Destroy();
}

cBaseWindow::eWINDOW_TYPE	NewCashshopPurchaseViewer::GetWindowType(void)
{
	return WT_NEW_CASHSHOP; 
}

void NewCashshopPurchaseViewer::Destroy(void)
{
	DeleteResource();
}

void NewCashshopPurchaseViewer::DeleteResource(void)
{
	DeleteScript();
}

void NewCashshopPurchaseViewer::Create( int nValue )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
	ti.s_eTextAlign = DT_CENTER;

	InitScript( "cash\\NewCashShop\\Store_pop_bg.tga", CsPoint( (g_nScreenWidth - 396) / 2, (g_nScreenHeight - 648) / 2 ), CsPoint(396,648), false/*, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER*/ );				
	m_pTitle = AddTitle( L" ", CsPoint( 0, 20 ), CFont::FS_16/*, NiColor(0.65f,1.0f,0.0f)*/ );
	
	m_pkCloseBT = AddButton(CsPoint(340,15), CsPoint(42,42), CsPoint( 0, 42 ), "Cash\\NewCashShop\\Store_btn_close.tga" );	
	if( m_pkCloseBT )
		m_pkCloseBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopPurchaseViewer::OnClickClosePurchaseViewer);

	m_pkPurchaseBT = AddButton(CsPoint(60,590), CsPoint( 120, 38 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );	
	if( m_pkPurchaseBT )
	{
		ti.SetText(L" ");
		m_pkPurchaseBT->SetText(&ti);
		m_pkPurchaseBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopPurchaseViewer::OnClickPurchaseBT);
	}

	m_pkCancelBT = AddButton(CsPoint(220,590), CsPoint( 120, 38 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );	
	if( m_pkCancelBT )
	{
		ti.SetText(UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_WINDOW_BTN_CANCEL").c_str());//L"취 소"
		m_pkCancelBT->SetText(&ti);
		m_pkCancelBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopPurchaseViewer::OnClickCancelBT);
	}

	m_pkPurchaseSingleViewer = NiNew NewCashshopPurchaseSingleViewer();
	if( m_pkPurchaseSingleViewer )
	{
		m_pkPurchaseSingleViewer->Create(GetRoot());
		AddChildScriptUI( m_pkPurchaseSingleViewer );
		m_pkPurchaseSingleViewer->SetVisible(false);
	}

	m_pkPresentViewer = NiNew NewCashshopPresentViewer();
	if( m_pkPresentViewer )
	{
		m_pkPresentViewer->Create(GetRoot());
		AddChildScriptUI( m_pkPresentViewer );
		m_pkPresentViewer->SetVisible(false);
	}

	m_pkPurchaseMultiViewer = NiNew NewCashshopPurchaseMultiViewer();
	if( m_pkPurchaseMultiViewer )
	{
		m_pkPurchaseMultiViewer->Create(GetRoot());
		AddChildScriptUI( m_pkPurchaseMultiViewer );
		m_pkPurchaseMultiViewer->SetVisible(false);
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );

#ifndef VERSION_STEAM
		ti.SetText(L"0");
		ti.s_eTextAlign = DT_RIGHT;
		m_pkTotalCash = AddText(&ti, CsPoint(336,495));
#endif
		ti.SetText(L"0");
		ti.s_eTextAlign = DT_RIGHT;
		m_pkPriceCash = AddText(&ti, CsPoint(336,520));
		m_pkPriceCash->SetColor( FONT_RED );

#ifndef VERSION_STEAM
		ti.SetText(L"0");
		ti.s_eTextAlign = DT_RIGHT;
		m_pkRemainCash = AddText(&ti, CsPoint(336,557));
#endif

#ifndef VERSION_STEAM
		ti.SetText(UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_WINDOW_TEXT_TOTALCASH").c_str());//L"보유 캐시 : "
		ti.s_eTextAlign = DT_LEFT;
		AddText(&ti, CsPoint(60,495));
#endif
		ti.SetText(UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_WINDOW_TEXT_PAYCASH").c_str());//L"지불 캐시 : "
		ti.s_eTextAlign = DT_LEFT;
		AddText(&ti, CsPoint(60,520));

#ifndef VERSION_STEAM
		ti.SetText(UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_WINDOW_TEXT_REMAINCASH").c_str());//L"남은 캐시 : "
		ti.s_eTextAlign = DT_LEFT;
		AddText(&ti, CsPoint(60,557));
#endif
	}
}

void NewCashshopPurchaseViewer::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

cBaseWindow::eMU_TYPE	NewCashshopPurchaseViewer::Update_ForMouse()
{
	if( m_pkCloseBT && m_pkCloseBT->Update_ForMouse() != cButton::ACTION_NONE )
		return MUT_NONE;
	if( m_pkPurchaseBT && m_pkPurchaseBT->Update_ForMouse() != cButton::ACTION_NONE )
		return MUT_NONE;
	if( m_pkCancelBT && m_pkCancelBT->Update_ForMouse() != cButton::ACTION_NONE )
		return MUT_NONE;

	if( m_pkCurrSubViewer && m_pkCurrSubViewer->UpdateMouse() )
		return MUT_NONE;
		
	return MUT_NONE;
}

void NewCashshopPurchaseViewer::Render()
{
	RenderScript();
}

void NewCashshopPurchaseViewer::ResetDevice()
{
	ResetDeviceScript();
}

bool NewCashshopPurchaseViewer::OnEscapeKey()
{
	OnClickClosePurchaseViewer(NULL,NULL);
	return true;
}

void NewCashshopPurchaseViewer::OnClickClosePurchaseViewer(void* pSender, void* pData)
{
	GetSystem()->OnClick_ClosePurchaceItem();
}

void NewCashshopPurchaseViewer::OnClickPurchaseBT(void* pSender, void* pData)
{
	switch( m_nCurrentPurchaseType )
	{
	case NewCashshopContents::CART:// 카트에 담기
		GetSystem()->OnClick_AddBasket();
		break;
	case NewCashshopContents::PRESENT:// 선물
		{
			NewCashshopPresentViewer* pPresentViewer = dynamic_cast<NewCashshopPresentViewer*>(m_pkCurrSubViewer);
			if( pPresentViewer )
				GetSystem()->OnClick_PurchasePresent(pPresentViewer->GetPresentFriendName());
		}
		break;
	case NewCashshopContents::PURCHASE:// 구매
		GetSystem()->OnClick_PurchaseItem();
		break;
	case NewCashshopContents::CARTALLPURCHASE://카트에 담긴 물품 모두 구매
	case NewCashshopContents::REPURCHASE_WINDOW://재구매 창
		GetSystem()->OnClick_PurchaseAllItem();
		break;
	}	
}

void NewCashshopPurchaseViewer::OnClickCancelBT(void* pSender, void* pData)
{
	GetSystem()->OnClick_ClosePurchaceItem();
}

void NewCashshopPurchaseViewer::UpdateViewer(NewCashshopContents::CASHSHOP_FUNTION eBType)
{
	m_nCurrentPurchaseType = eBType;
	std::wstring titleName;
	std::wstring PurchaseBtnName;

	if( m_pkCurrSubViewer )
		m_pkCurrSubViewer->SetVisible(false);

	switch( eBType )
	{
	case NewCashshopContents::CART:// 카트에 담기
		titleName = UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_WINDOW_TITLE_CART");//L"장바구니에 담기";
		PurchaseBtnName = UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_CART_WINDOW_OKBTN");//L"담기";
		m_pkCurrSubViewer = m_pkPurchaseSingleViewer;
		break;
	case NewCashshopContents::PRESENT:// 선물
		titleName = UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_WINDOW_TITLE_PRESENT");//L"선물 하기";
		PurchaseBtnName = UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_PRESENT_WINDOW_OKBTN");//L"보내기";
		m_pkCurrSubViewer = m_pkPresentViewer;
		break;
	case NewCashshopContents::PURCHASE:// 구매
		titleName = UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_WINDOW_TITLE_PURCHASE");//L"아아템 구매";
		PurchaseBtnName = UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_PURCHASE_WINDOW_OKBTN");//L"구매";
		m_pkCurrSubViewer = m_pkPurchaseSingleViewer;
		break;
	case NewCashshopContents::CARTALLPURCHASE:
		{
			titleName = UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_WINDOW_TITLE_CARTPURCHASE");//L"장바구니 아이템 구매";
			PurchaseBtnName = UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_CARTPURCHASE_WINDOW_OKBTN");//L"구매";
			m_pkCurrSubViewer = m_pkPurchaseMultiViewer;
		}
		break;
	case NewCashshopContents::REPURCHASE_WINDOW:
		{
			titleName = UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_WINDOW_TITLE_REPURCHASE");//L"재구매";
			PurchaseBtnName = UISTRING_TEXT("CASHSHOP_PURCHASEVIEWER_REPURCHASE_WINDOW_OKBTN");//L"재구매";
			m_pkCurrSubViewer = m_pkPurchaseMultiViewer;
		}
		break;
	}

	SAFE_POINTER_RET( m_pkCurrSubViewer );

	if( m_pTitle )
		m_pTitle->SetText( titleName.c_str() );
	if( m_pkPurchaseBT )
		m_pkPurchaseBT->SetText( PurchaseBtnName.c_str() );

	NewCashshopContents::PurchaseItems const& items = GetSystem()->GetPurchaseItems();
	if( items.empty() )
		return;

	m_pkCurrSubViewer->SetVisible(true);
	m_pkCurrSubViewer->UpdateViewer( items );

	UpdateCashBalance();
}


void	NewCashshopPurchaseViewer::UpdateCashBalance()
{
	int nHaveCash = GetSystem()->GetTotalCash();
	DWORD nPruchasePrice = GetSystem()->GetTotalPurchasePrice();
	int nRemainCash = nHaveCash - nPruchasePrice;

	if( m_pkTotalCash )
	{

		std::wstring kTotalCash;
#ifdef VERSION_STEAM	
		DmCS::StringFn::Format( kTotalCash, L"%.2f", nHaveCash * 0.01 );	
#else
		kTotalCash = DmCS::StringFn::getNumberFormatW(nHaveCash);
#endif
		m_pkTotalCash->SetText(kTotalCash.c_str());
	}

	if( m_pkPriceCash )
	{
		std::wstring kItemPrice = _T("-");
		std::wstring wsPrice;
#ifdef VERSION_STEAM	
		DmCS::StringFn::Format( wsPrice, L"%.2f", nPruchasePrice * 0.01 );	
#else
		wsPrice = DmCS::StringFn::getNumberFormatW(nPruchasePrice);
#endif
		kItemPrice += wsPrice;
		m_pkPriceCash->SetText(kItemPrice.c_str());
	}

	if( m_pkRemainCash )
	{				
		std::wstring kRemainCash;
		if( nRemainCash < 0 )
		{
			kRemainCash.append( _T("-") );
			m_pkRemainCash->SetColor( FONT_RED );
		}
		else
			m_pkRemainCash->SetColor( FONT_YELLOW );

		kRemainCash += DmCS::StringFn::getNumberFormatW(abs(nRemainCash));
		m_pkRemainCash->SetText(kRemainCash.c_str());
	}	
}



void NewCashshopPurchaseViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::PurchaseViewer_ChangeSelect:
		UpdateCashBalance();
		break;
	}
}

bool NewCashshopPurchaseViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::PurchaseViewer_ChangeSelect, this );

	return true;
}

bool NewCashshopPurchaseViewer::OnEnterKey()
{
	OnClickPurchaseBT( NULL, NULL );
	return true;
}
