
#include "stdafx.h"
#include "NewCashshopViewer.h"

NewCashshopViewer::NewCashshopViewer()
:m_pMainCategory(NULL),
m_pSubCategory(NULL),
m_pCloseBT(NULL),
m_pkVWBest(NULL),
m_pkVWCart(NULL),
m_pkVWEquip(NULL),
m_pkVWSellingItem(NULL),
m_pkVWMain(NULL),
m_pkCurrItemViewer(NULL),
m_pkVWWeb(NULL),
m_pEditSerchingItem(NULL),
m_pCashInventoryTab(NULL),
m_pkHistoryMsg(NULL),
m_pCashStorageBtn(NULL),
m_pkCashTT(NULL),
m_pkBouseTT(NULL),
m_pkRefreshCashStateBT(NULL),
m_pkPurchaseAllBT(NULL),
m_pkChargeCashBT(NULL),
m_pResetSerchingBT(NULL)
{

}
NewCashshopViewer::~NewCashshopViewer()
{
}

bool NewCashshopViewer::Close( bool bSound)
{
	return cBaseWindow::Close(bSound);

}

void NewCashshopViewer::Destroy()
{
	cBaseWindow::Delete();
	m_pkPurchaseViewer.Destroy();
}

void NewCashshopViewer::DeleteResource()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	
	DeleteScript();
	if( m_pEditSerchingItem )
	{
		m_pEditSerchingItem->ReleaseFocus();
		m_pEditSerchingItem = NULL;
	}
	m_pMainCategory = NULL;
}

void NewCashshopViewer::Create( int nValue)
{
	if( !cBaseWindow::Init() )
		return ;

	InitScript( "Cash\\NewCashShop\\Store_bg.tga",  CsPoint( (g_nScreenWidth - 1024) / 2, (g_nScreenHeight - 768) / 2 ), CsPoint( 1024, 768 ) ,/*true , IFREGION_X::CENTER, IFREGION_Y::CENTER,*/ true);

	m_pCloseBT = AddButton( CsPoint(977, 9), CsPoint(42,42), CsPoint( 0, 42 ), "Cash\\NewCashShop\\Store_btn_close.tga" );	
	if( m_pCloseBT )
		m_pCloseBT->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &NewCashshopViewer::OnClick_CloseCashshop );

	CreateUI();
	CreateViewer();
	CreateEditSerchingItem();
	_CreateMainCategoryBT();
	UpdateCashBalance();
}

void NewCashshopViewer::CreateUI(void)
{
#ifndef VERSION_STEAM

	AddSprite(CsPoint( 17, 727 ), CsPoint( 256, 34 ), "Cash\\NewCashShop\\Store_money.tga");	
	AddSprite(CsPoint( 29, 730 ), CsPoint( 24, 24 ), "Cash\\NewCashShop\\Store_icon_C.tga");
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, FONT_YELLOW );
		ti.SetText(L" ");
		ti.s_eTextAlign = DT_RIGHT;	
		m_pkCashTT = AddText( &ti, CsPoint(259,736));
	}

#ifndef SDM_CASHSHOP_CASH_INTEGRATION_20181121
	AddSprite(CsPoint( 276, 727 ), CsPoint( 256, 34 ), "Cash\\NewCashShop\\Store_money.tga");	
	AddSprite(CsPoint( 298, 730 ), CsPoint( 24, 24 ), "Cash\\NewCashShop\\Store_icon_B.tga");
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, FONT_GREEN );
		ti.SetText(L" ");
		ti.s_eTextAlign = DT_RIGHT;	
		m_pkBouseTT = AddText( &ti, CsPoint(518,736));
	}

	m_pkRefreshCashStateBT= AddButton( CsPoint(533, 726), CsPoint(36,34), CsPoint( 0, 34 ), "Cash\\NewCashShop\\Store_btn_refresh.tga" );
	if( m_pkRefreshCashStateBT )
		m_pkRefreshCashStateBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopViewer::OnClick_RefreshCashState);
#else
	m_pkRefreshCashStateBT= AddButton( CsPoint(276, 726), CsPoint(36,34), CsPoint( 0, 34 ), "Cash\\NewCashShop\\Store_btn_refresh.tga" );
	if( m_pkRefreshCashStateBT )
		m_pkRefreshCashStateBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopViewer::OnClick_RefreshCashState);
#endif

#endif

	m_pCashInventoryTab = AddRadioButton( CsPoint(710,540), CsPoint(100, 30), CsPoint(0,28), "Cash\\NewCashShop\\Store_btn_tab.tga" );
	if(m_pCashInventoryTab)
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("CASHSHOP_WINDOW_RADIOBTN_CART").c_str());//"장바구니"
		ti.s_eTextAlign = DT_CENTER;
		m_pCashInventoryTab->SetCheckBoxText( &ti );
		m_pCashInventoryTab->SetUserData( new sStorageType(eCartStorage) );		
		m_pCashInventoryTab->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &NewCashshopViewer::OnClick_ShowCashStorageBT );
		m_pCashInventoryTab->SetCheckIndex(0,true);
	}

	m_pkChargeCashBT= AddButton( CsPoint(860, 722), CsPoint(124,42), CsPoint( 0, 42 ), "Cash\\NewCashShop\\Store_btn_G2.tga" );
	if( m_pkChargeCashBT )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("CASHSHOP_WINDOW_BUTTON_CASH_CHARGE").c_str());//캐시충전
		ti.s_eTextAlign = DT_CENTER;	
		m_pkChargeCashBT->SetText( &ti );
		m_pkChargeCashBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopViewer::OnClick_ChargingCash);
	}

#ifndef VERSION_STEAM
	m_pkPurchaseAllBT= AddButton( CsPoint(732, 722), CsPoint(124,42), CsPoint( 0, 42 ), "Cash\\NewCashShop\\Store_btn_G2.tga" );	
	if( m_pkPurchaseAllBT )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("CASHSHOP_WINDOW_BUTTON_CART_ALL_PURCHASES").c_str());// 모두 구입
		ti.s_eTextAlign = DT_CENTER;	
		m_pkPurchaseAllBT->SetText( &ti );
		m_pkPurchaseAllBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopViewer::OnClick_PurchaseAll);
	}
#endif

	{
		m_pCashStorageBtn =  AddButton( CsPoint( 900, 540 ),CsPoint(100, 30), CsPoint( 0, 38 ), "Cash\\NewCashShop\\Store_btn_G1.tga" );
		if( m_pCashStorageBtn )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
			ti.SetText(UISTRING_TEXT("CASHSHOP_WINDOW_BTN_CASHSTORAGE").c_str());//L"모두벗기"
			ti.s_eTextAlign = DT_CENTER;	
			m_pCashStorageBtn->SetText( &ti );
			m_pCashStorageBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopViewer::OnClick_ShowCashStorage);
		}
	}	

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, FONT_YELLOW );
		ti.SetText(UISTRING_TEXT("CASHSHOP_WINDOW_MESSAGE_HISTORY_SHOW_EXCEPTION").c_str());
		ti.s_eTextAlign = DT_RIGHT;	
		m_pkHistoryMsg = AddText( &ti, CsPoint(977,55));
	}
}

bool NewCashshopViewer::CreateViewer(void)
{
	// 메인 상품 UI 생성
	m_pkVWMain = NiNew NewCashshopMainViewer();
	if( m_pkVWMain )
	{
		m_pkVWMain->Create(GetRoot());
		AddChildScriptUI( m_pkVWMain );
		m_pkCurrItemViewer = m_pkVWMain;
	}

	// 베스트 상품 UI 생성
	m_pkVWBest = NiNew NewCashshopBestViewer();
	if( m_pkVWBest )
	{
		m_pkVWBest->Create(GetRoot());
		AddChildScriptUI( m_pkVWBest );
	}

	// 카트 UI 생성
	m_pkVWCart = NiNew NewCashshopCartViewer();
	if( m_pkVWCart )
	{
		m_pkVWCart->Create(GetRoot());
		AddChildScriptUI( m_pkVWCart );
		m_pkVWCart->SetVisible(false);
	}

	// 캐릭터 장비 UI 생성
	m_pkVWEquip = NiNew NewCashshopEquipViewer();
	if( m_pkVWEquip )
	{
		m_pkVWEquip->Create(GetRoot());
		AddChildScriptUI( m_pkVWEquip );
		m_pkVWEquip->SetVisible(false);
	}

	// 아이템 판매 UI 생성
	m_pkVWSellingItem = NiNew NewCashshopSeliingItemViewer();
	if( m_pkVWSellingItem )
	{
		m_pkVWSellingItem->Create(GetRoot());
		AddChildScriptUI( m_pkVWSellingItem );
		m_pkVWSellingItem->SetVisible(false);
	}

	// 웹데이터 테이블이 있는지와 데이터가 들어있는지 검사함.
	LIST_CASHSHOP_WEBDATA const* pCashWebData = GetSystem()->GetCashshopWebData();
	if( NULL != pCashWebData && !pCashWebData->empty() )
	{		
		// 웹 페이지 UI 생성
		m_pkVWWeb = NiNew NewCashshopWebViewer();
		if( m_pkVWWeb )
		{
			m_pkVWWeb->Create(GetRoot());
			m_pkVWWeb->SetWebImgData( pCashWebData );
			AddChildScriptUI( m_pkVWWeb );
			m_pkVWWeb->SetVisible(false);
		}
	}

	// 구매창 UI 생성
	m_pkPurchaseViewer.Create();
	m_pkPurchaseViewer.SetVisible(false);
	return true;
}

void NewCashshopViewer::CreateEditSerchingItem(void)
{
	AddSprite(CsPoint(710,110), CsPoint(294,30), "Cash\\NewCashShop\\Store_serch.tga");

	// ID 에딧박스
	m_pEditSerchingItem = NiNew cEditBox;
	if( m_pEditSerchingItem )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14, NiColor::WHITE );
		ti.SetText( _T( "" ) );
		ti.s_bOutLine = false;

		m_pEditSerchingItem->Init( GetRoot(), CsPoint(710,110), CsPoint(274,30), &ti, false );
		m_pEditSerchingItem->SetEmptyMsgText( UISTRING_TEXT("CASHSHOP_WINDOW_TEXT_STORAGE_CERCHING").c_str(), 
			NiColor(0.53125f,0.53125f,0.53125f) );
		m_pEditSerchingItem->SetFontLength( 20 );
		m_pEditSerchingItem->EnableUnderline( ti.s_Color );
		m_pEditSerchingItem->SetEnableSound( true );
		m_pEditSerchingItem->AddEvent( cEditBox::eEditbox_ChangeText, this, &NewCashshopViewer::OnChangeSechingEditBox );
		m_pEditSerchingItem->AddEvent( cEditBox::eEditbox_SetFocus, this, &NewCashshopViewer::OnEnterFocusSechingEditBox );
		m_pEditSerchingItem->AddEvent( cEditBox::eEditbox_ReleaseFocus, this, &NewCashshopViewer::OnReleaseFocusSechingEditBox );
		AddChildControl( m_pEditSerchingItem );
	}

	// 	m_pMagnifierBT = AddButton( CsPoint(971,113), CsPoint( 20, 20 ), CsPoint( 0, 21 ), "TacticsHouse\\Storage_Magnifier.tga" );		
	// 	if( m_pMagnifierBT )
	// 		m_pMagnifierBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopViewer::OnClickSerching);

	m_pResetSerchingBT = AddButton( CsPoint(971,113), CsPoint( 20, 20 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );
	if( m_pResetSerchingBT )
	{
		m_pResetSerchingBT->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &NewCashshopViewer::OnClickResetSerching);
		m_pResetSerchingBT->SetVisible(false);
	}
}

void NewCashshopViewer::Render(void)
{
	RenderScript();
	EndRenderScript();

	m_pkPurchaseViewer.Render();
}

void NewCashshopViewer::ResetDevice()
{	
	ResetDeviceScript();
}

bool NewCashshopViewer::OnEscapeKey()
{
	if( m_pkPurchaseViewer.IsVisible() )
		return m_pkPurchaseViewer.OnEscapeKey();
	else
		OnClick_CloseCashshop(NULL,NULL);
	return true;
}

bool NewCashshopViewer::OnEnterKey()
{
	if( m_pkPurchaseViewer.IsVisible() )
		return m_pkPurchaseViewer.OnEnterKey();
	return true;
}

void NewCashshopViewer::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}


cBaseWindow::eMU_TYPE NewCashshopViewer::Update_ForMouse_Disable()
{
	if(m_pkPurchaseViewer.IsVisible()) 
		m_pkPurchaseViewer.Update_ForMouse();

	return MUT_NONE;
}

cBaseWindow::eMU_TYPE NewCashshopViewer::Update_ForMouse(void)
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return MUT_NONE;

	if( m_pMainCategory && m_pMainCategory->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;

	if( m_pSubCategory && m_pSubCategory->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;

	if( m_pCashInventoryTab && m_pCashInventoryTab->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;

	if(m_pCloseBT && m_pCloseBT->Update_ForMouse() != cButton::ACTION_NONE)
		return muReturn;
	if( m_pkChargeCashBT && m_pkChargeCashBT->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;
	if( m_pkPurchaseAllBT && m_pkPurchaseAllBT->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;
	if( m_pkRefreshCashStateBT && m_pkRefreshCashStateBT->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;
	if( m_pCashStorageBtn && m_pCashStorageBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if(m_pkVWBest && m_pkVWBest->UpdateMouse() == TRUE)
		return MUT_NONE;

	if(m_pkVWCart && m_pkVWCart->IsVisible() == true && m_pkVWCart->UpdateMouse() == TRUE)
		return MUT_NONE;

	if(m_pkVWWeb && m_pkVWWeb->UpdateMouse() == TRUE)
		return MUT_NONE;

	if(m_pkVWEquip && m_pkVWEquip->UpdateMouse() == TRUE)
		return MUT_NONE;

	if(m_pkCurrItemViewer && m_pkCurrItemViewer->UpdateMouse() == TRUE)
		return MUT_NONE;

	if( m_pResetSerchingBT && m_pResetSerchingBT->Update_ForMouse() == cButton::ACTION_CLICK )
		return MUT_NONE;

	// 아이템 갯수 입력창 활성화시. 엔터키 or ESC키 누르면 활성화 해제.
	if(m_pEditSerchingItem != NULL)
	{
		if( GLOBALINPUT_ST.IsEscape() && m_pEditSerchingItem->IsFocus() ){
			std::wstring kKeyword = L"";
			m_pEditSerchingItem->SetText(L"", false);
			GetSystem()->OnClick_SetFilteringKeyword(kKeyword);
			m_pEditSerchingItem->ReleaseFocus(false);
		}
		else if( m_pEditSerchingItem->Update_ForMouse( CsPoint::ZERO ) == cEditBox::ACTION_NONE ){
			if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN)
				m_pEditSerchingItem->ReleaseFocus(false);
		}
	}

	return MUT_NONE;
}

BOOL NewCashshopViewer::UpdateMouse()
{
	return TRUE;
}

void NewCashshopViewer::_CreateMainCategoryBT(void)
{
	std::map<int,std::wstring> main;
	GetSystem()->GetMainCategoryName( main );

 	std::map<int,std::wstring>::iterator it = main.begin();
	int ButtonIdx = 0;
	for( ; it != main.end(); ++it, ++ButtonIdx )
	{
		if( NULL == m_pMainCategory )
		{
			m_pMainCategory = AddRadioButton( CsPoint::ZERO, CsPoint(110, 42), CsPoint(0, 42), "Cash\\NewCashShop\\Store_btn_MainCategory.tga" );
			if(m_pMainCategory)
			{
				cText::sTEXTINFO ti;
				ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
				ti.SetText(it->second.c_str());
				ti.s_eTextAlign = DT_CENTER;
				m_pMainCategory->SetUserData(new sCategoryType(it->first, 1));
				m_pMainCategory->SetCheckBoxText( &ti );
				m_pMainCategory->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &NewCashshopViewer::OnClick_MainCategoryBT );
			}
		}
		else
		{
			CsPoint CurrPos = CsPoint::ZERO;
			CurrPos.x += 110 * ButtonIdx;
			cRadioButton* pAddBtn = m_pMainCategory->AddNode( CurrPos, CsPoint(110, 42), CsPoint(0, 42), "Cash\\NewCashShop\\Store_btn_MainCategory.tga" );
			if( pAddBtn )
			{
				cText::sTEXTINFO ti;
				ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
				ti.SetText(it->second.c_str());
				ti.s_eTextAlign = DT_CENTER;
				pAddBtn->SetUserData(new sCategoryType(it->first, 1));
				pAddBtn->SetCheckBoxText( &ti );
				pAddBtn->SetEnable(true);
			}
		}
	}

	if( m_pMainCategory )
		m_pMainCategory->SetCheckIndex(0, true);
}


void NewCashshopViewer::_ChangeMainCateogryBT(int const& nMainType, int const& nSubType, bool bOnEvent )
{
	_ChangeSubCategoryBT( nMainType, nSubType, bOnEvent);

	if( m_pkVWMain )	m_pkVWMain->SetVisible(false);
	if( m_pkVWBest )	m_pkVWBest->SetVisible(false);
	if( m_pkVWWeb )		m_pkVWWeb->SetVisible(false);
	if( m_pkVWCart )	m_pkVWCart->SetVisible(false);
	if( m_pkVWEquip )	m_pkVWEquip->SetVisible(false);
	if( m_pkVWEquip )	m_pkVWEquip->SetVisible(false);
	if( m_pkVWSellingItem )	m_pkVWSellingItem->SetVisible(false);
	if( m_pkHistoryMsg )	m_pkHistoryMsg->SetVisible(false);
	if( m_pCashStorageBtn )	m_pCashStorageBtn->SetVisible(false);
	if( m_pCashInventoryTab )	m_pCashInventoryTab->SetVisible(false);

	if( NewCashshopContents::eMain == nMainType )
	{
		m_pkCurrItemViewer = m_pkVWMain;
		if( m_pkVWMain )	m_pkVWMain->SetVisible(true);
		if( m_pkVWBest )	m_pkVWBest->SetVisible(true);
		if( m_pkVWWeb )		m_pkVWWeb->SetVisible(true);
		else
		{
			if( m_pkVWCart )			m_pkVWCart->SetVisible(true);
			if( m_pCashStorageBtn )		m_pCashStorageBtn->SetVisible(true);
			if( m_pCashInventoryTab )	m_pCashInventoryTab->SetVisible(true);
			if( m_pkVWEquip )			m_pkVWEquip->SetVisible(true);
		}
		return;
	}
	
	if( GetSystem()->GetHistoryIdx() == nMainType )
	{
		if( m_pkHistoryMsg )	
			m_pkHistoryMsg->SetVisible(true);
	}

	m_pkCurrItemViewer = m_pkVWSellingItem;

	if( m_pkCurrItemViewer )	m_pkCurrItemViewer->SetVisible(true);
	if( m_pkVWBest )			m_pkVWBest->SetVisible(true);
	if( m_pkVWEquip )			m_pkVWEquip->SetVisible(true);
	if( m_pkVWSellingItem )		m_pkVWSellingItem->SetVisible(true);
	if( m_pkVWCart )			m_pkVWCart->SetVisible(true);
	if( m_pCashStorageBtn )		m_pCashStorageBtn->SetVisible(true);
	if( m_pCashInventoryTab )	m_pCashInventoryTab->SetVisible(true);
}

void NewCashshopViewer::_ChangeSubCategoryBT(int const& iMainCategory, int const& iSubCategory, bool bOnEvent)
{
	if(m_pSubCategory != NULL)	{
		RemoveChildControl(m_pSubCategory);
		m_pSubCategory= NULL;
	}

	std::map<int,std::wstring> infos;
	GetSystem()->GetSubCategoryName( iMainCategory, infos);

	std::map<int,std::wstring>::iterator it = infos.begin();
	for( int ButtonIdx = 0 ; it != infos.end(); ++it, ++ButtonIdx )
	{
		if( NULL == m_pSubCategory )
		{
			m_pSubCategory = AddRadioButton( CsPoint(0,50), CsPoint(100, 30), CsPoint(0, 42), "Cash\\NewCashShop\\Store_btn_MainCategory.tga" );
			if(m_pSubCategory)
			{
				cText::sTEXTINFO ti;
				ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
				ti.SetText(it->second.c_str());
				ti.s_eTextAlign = DT_CENTER;
				m_pSubCategory->SetCheckBoxText( &ti );
				m_pSubCategory->SetUserData(new sCategoryType( iMainCategory, it->first));
				m_pSubCategory->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &NewCashshopViewer::OnClick_SubCategoryBT );
			}
		}
		else
		{
			CsPoint CurrPos = CsPoint(0,50);
			CurrPos.x += 100 * ButtonIdx;
			cRadioButton* pAddBtn = m_pSubCategory->AddNode( CurrPos, CsPoint(100, 30), CsPoint(0, 42), "Cash\\NewCashShop\\Store_btn_MainCategory.tga" );
			if( pAddBtn )
			{
				cText::sTEXTINFO ti;
				ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
				ti.SetText(it->second.c_str());
				ti.s_eTextAlign = DT_CENTER;
				pAddBtn->SetCheckBoxText( &ti );
				pAddBtn->SetEnable(true);
				pAddBtn->SetUserData(new sCategoryType( iMainCategory, it->first));
			}
		}
	}

	if( m_pSubCategory )
		m_pSubCategory->SetCheckIndex(iSubCategory - 1, bOnEvent);	
}


void NewCashshopViewer::_MovePage(int const& nMoveMain, int const& nMoveSub)
{
	BHPRT("MovePage Man[%d] Sub[%d]", 	nMoveMain, nMoveSub);

	if( m_pMainCategory )
		m_pMainCategory->SetCheckIndex( nMoveMain - 1, false );	

	_ChangeMainCateogryBT( nMoveMain, nMoveSub, false );
}


void NewCashshopViewer::OnClick_MainCategoryBT(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cRadioButton* pRadioButton = static_cast<cRadioButton*>(pData);
	SAFE_POINTER_RET(pRadioButton);
	sCategoryType* userData = dynamic_cast<sCategoryType*>(pRadioButton->GetUserData());
	SAFE_POINTER_RET(userData);

	BHPRT("OnClick_MainCategoryBT %d, %d", 	userData->nMaintype, userData->nSubtype);

	_ChangeMainCateogryBT( userData->nMaintype, userData->nSubtype );
}

void NewCashshopViewer::OnClick_SubCategoryBT(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cRadioButton* pRadioButton = static_cast<cRadioButton*>(pData);
	SAFE_POINTER_RET(pRadioButton);
	sCategoryType* userData = dynamic_cast<sCategoryType*>(pRadioButton->GetUserData());
	SAFE_POINTER_RET(userData);
	BHPRT("OnClick_SubCategoryBT %d, %d", 	userData->nMaintype, userData->nSubtype);
	if( m_pkVWSellingItem )
		m_pkVWSellingItem->UpdatePage(userData->nMaintype, userData->nSubtype);
}

void NewCashshopViewer::OnClick_ShowCashStorageBT(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cRadioButton* pRadioButton = static_cast<cRadioButton*>(pData);
	SAFE_POINTER_RET(pRadioButton);
	sStorageType* userData = dynamic_cast<sStorageType*>(pRadioButton->GetUserData());
	SAFE_POINTER_RET(userData);

	switch( userData->nStorageype )
	{
	case eCashStorage:
		{
			if(m_pkVWCart != NULL)
				m_pkVWCart->SetVisible(false);
		}
		break;
	case eCartStorage:
		{
			if(m_pkVWCart != NULL)
				m_pkVWCart->SetVisible(true);
		}
		break;
	}
}

void NewCashshopViewer::OnClick_CloseCashshop(void* pSender, void* pData)
{
	GetSystem()->Req_HideCashshop(NULL);
}

void NewCashshopViewer::OnClick_RefreshCashState(void* pSender, void* pData)
{
	BHPRT("OnClick_RefreshCashState");
	GetSystem()->Req_RefrashBalance();
}

void NewCashshopViewer::OnClick_ChargingCash(void* pSender, void* pData)
{
	BHPRT("OnClick_ChargingCash");
	GetSystem()->OnClick_ChargingCash();
}

//////////////////////////////////////////////////////////////////////////
// 카트에 담겨 있는 모든 아이템 구매 버튼 클릭
//////////////////////////////////////////////////////////////////////////
void NewCashshopViewer::OnClick_PurchaseAll(void* pSender, void* pData)
{
	BHPRT("OnClick_PurchaseAll");
	GetSystem()->OnClick_PurchaseAll_Cart();
}

void NewCashshopViewer::OnClick_ShowCashStorage(void* pSender, void* pData)
{
	BHPRT("OnClick_ShowCashStorage");
	GetSystem()->OnClick_ShowCashStorage();
}

//////////////////////////////////////////////////////////////////////////
// 보유 캐쉬 정보 갱신
//////////////////////////////////////////////////////////////////////////
void NewCashshopViewer::UpdateCashBalance(void)
{	
	if( m_pkCashTT )
	{
#ifdef SDM_CASHSHOP_CASH_INTEGRATION_20181121
		DWORD dwCurrCash = GetSystem()->GetTotalCash();
#else
		DWORD dwCurrCash = GetSystem()->GetRealCash();
#endif

		std::wstring valueRealCash = DmCS::StringFn::getNumberFormatW(dwCurrCash);

		std::wstring wsValue = UISTRING_TEXT("CASHSHOP_WINDOW_TEXT_SHOW_HAVE_REALCASH");
		DmCS::StringFn::ReplaceAll( wsValue, L"#RealCash#", valueRealCash );
		m_pkCashTT->SetText(wsValue.c_str());
	}

	if( m_pkBouseTT )
	{
		DWORD dwBounsCash = GetSystem()->GetBonusCash();
		std::wstring valueBounsCash = DmCS::StringFn::getNumberFormatW(dwBounsCash);

		std::wstring wsValue = UISTRING_TEXT("CASHSHOP_WINDOW_TEXT_SHOW_HAVE_BONUSCASH");
		DmCS::StringFn::ReplaceAll( wsValue, L"#BonusCash#", valueBounsCash );
		m_pkBouseTT->SetText(wsValue.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////
// 아이템 검색 관련 함수
//////////////////////////////////////////////////////////////////////////
void NewCashshopViewer::OnChangeSechingEditBox(void* pkSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	std::wstring kKeyword = m_pEditSerchingItem->GetText()->GetText();
	if(kKeyword == L"")
	{
		//m_pMagnifierBT->SetVisible(true);
		m_pResetSerchingBT->SetVisible(false);
	}	
	else
	{
		//m_pMagnifierBT->SetVisible(false);
		m_pResetSerchingBT->SetVisible(true);
	}
	GetSystem()->OnClick_SetFilteringKeyword(kKeyword);
}

void NewCashshopViewer::OnEnterFocusSechingEditBox(void* pkSender, void* pData)
{
	SAFE_POINTER_RET( pData );
}

void NewCashshopViewer::OnReleaseFocusSechingEditBox(void* pkSender, void* pData)
{
	SAFE_POINTER_RET( pData );
}

void	NewCashshopViewer::OnClickSerching(void* pSender, void* pData)
{
	if(m_pEditSerchingItem && GetSystem())
	{
		std::wstring kKeyword = m_pEditSerchingItem->GetText()->GetText();
		GetSystem()->OnClick_SetFilteringKeyword(kKeyword);
		m_pEditSerchingItem->GetText()->SetText(L"");
	}
}

void	NewCashshopViewer::OnClickResetSerching(void* pSender, void* pData)
{
	if(m_pEditSerchingItem && GetSystem())
	{
		std::wstring kKeyword = L"";
		GetSystem()->OnClick_SetFilteringKeyword(kKeyword);
		m_pEditSerchingItem->GetText()->SetText(L"");
		m_pEditSerchingItem->ReleaseFocus();
		//m_pMagnifierBT->SetVisible(true);
		m_pResetSerchingBT->SetVisible(false);
	}
}

//////////////////////////////////////////////////////////////////////////
// 아이템 구매 창 보이고 메인창 Disable 시킴
//////////////////////////////////////////////////////////////////////////
void NewCashshopViewer::ShowPurchaseViewer(NewCashshopContents::CASHSHOP_FUNTION eBType)
{
	switch(eBType)
	{
	case NewCashshopContents::CART:
	case NewCashshopContents::PRESENT:
	case NewCashshopContents::PURCHASE:
	case NewCashshopContents::CARTALLPURCHASE:
	case NewCashshopContents::REPURCHASE_WINDOW:
		m_pkPurchaseViewer.UpdateViewer(eBType);
		break;
	default:
		return;
	}
	SetEnableWindow(false);
	m_pkPurchaseViewer.SetVisible(true);
}

//////////////////////////////////////////////////////////////////////////
// 아이템 구매창 숨기고 메인창 활성화
//////////////////////////////////////////////////////////////////////////
void NewCashshopViewer::HidePurchaseViewer(void)
{
	m_pkPurchaseViewer.SetVisible(false);
	SetEnableWindow(true);
}

void NewCashshopViewer::_MoveCategory( int const& nMainCategory, int const& nSubCategory )
{

}
//////////////////////////////////////////////////////////////////////////

bool NewCashshopViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::Move_Page, this );
	GetSystem()->Register( SystemType::Update_Cash_Balance, this );
	GetSystem()->Register( SystemType::Show_PurchaseViewer, this );
	GetSystem()->Register( SystemType::Hide_PurchaseViewer, this );

	return true;
}

void NewCashshopViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case NewCashshopContents::Update_Cash_Balance:// 보유 캐쉬 정보 갱신
		UpdateCashBalance();
		break;
	case NewCashshopContents::Move_Page:	// 
		{
			int nMoveMain;
			int nMoveSub;
 			kStream >> nMoveMain >> nMoveSub;
 			_MovePage(nMoveMain,nMoveSub);
		}		
		break;
	case NewCashshopContents::Show_PurchaseViewer:// 아이템 구매 창 띄움.
		{
			NewCashshopContents::CASHSHOP_FUNTION eBType = NewCashshopContents::END;
			kStream >> eBType;
			ShowPurchaseViewer(eBType);
		}
		break;
	case NewCashshopContents::Hide_PurchaseViewer:
		HidePurchaseViewer();
		break;
	}

}
