#include "stdafx.h"
#include "EvolutionStore_Window.h"

//////////////////////////////////////////////////////////////////////////

void cExtraEvolutionUI::sNeedItemControls::SetHide()
{
	if( pListItemBox )
	{
		pListItemBox->SetVisible(false);
		sItemInfo* pInfo = dynamic_cast<sItemInfo*>(pListItemBox->GetUserData());
		if( pInfo )
		{
			pInfo->dwIDX = 0;
			pInfo->nObjectType = 0;
			pInfo->nSubValue = 0;
		}
	}

	if( pkItemIcon )
		pkItemIcon->SetVisible(false);
	if( pkDigimonIcon )
		pkDigimonIcon->SetVisible(false);
// 	if( pkEnableSlot )
// 		pkEnableSlot->SetVisible(false);
}

void cExtraEvolutionUI::sNeedItemControls::SetShowItemIcon( DWORD const& dwItemIDX, int const& nCount )
{
	SAFE_POINTER_RET( pListItemBox );
	pListItemBox->SetVisible(true);
	sItemInfo* pInfo = dynamic_cast<sItemInfo*>(pListItemBox->GetUserData());
	if( pInfo )
	{
		pInfo->dwIDX = dwItemIDX;
		pInfo->nObjectType = 1;
		pInfo->nSubValue = nCount;
	}

	if( pkItemIcon )
	{
		pkItemIcon->ChangeIcon(ICONITEM::ITEM_ICON, dwItemIDX, nCount);
		pkItemIcon->SetVisible(true);
	}

	if( pkDigimonIcon )
		pkDigimonIcon->SetVisible(false);
}

void cExtraEvolutionUI::sNeedItemControls::SetShowDigimonIcon( DWORD const& dwItemIDX, std::string const& sDIgimonIconFile, int const& nCount )
{
	SAFE_POINTER_RET( pListItemBox );
	pListItemBox->SetVisible(true);
	sItemInfo* pInfo = dynamic_cast<sItemInfo*>(pListItemBox->GetUserData());
	if( pInfo )
	{
		pInfo->dwIDX = dwItemIDX;
		pInfo->nObjectType = 2;
		pInfo->nSubValue = nCount;
	}

	if( pkItemIcon )
		pkItemIcon->SetVisible(false);
	if( pkDigimonIcon )
	{
		if( pkDigimonIcon->s_pSprite )
		{
			pkDigimonIcon->s_pSprite->ChangeTexture( sDIgimonIconFile.c_str(), false );
			pkDigimonIcon->SetVisible(true);
		}
	}
}

void cExtraEvolutionUI::sNeedItemControls::SetEnableSlot( bool bValue )
{
	SAFE_POINTER_RET( pkEnableSlot );
	pkEnableSlot->SetVisible(!bValue);
}

//////////////////////////////////////////////////////////////////////////
cBaseWindow::eWINDOW_TYPE cExtraEvolutionUI::GetWindowType()
{ 
	return WT_SPIRIT_EVOLUTION; 
}

cExtraEvolutionUI::cExtraEvolutionUI()
:m_pCloseBtn(NULL),m_pEvolutionBtn(NULL),m_pTabMenu(NULL),m_pNeedMoneyIF(NULL)
,m_pListBox(NULL),m_pNeedItemGrid(NULL),m_pNeedSubItemGrid(NULL)
{

}

cExtraEvolutionUI::~cExtraEvolutionUI()
{

}

//////////////////////////////////////////////////////////////////////////

bool cExtraEvolutionUI::Close( bool bSound)
{
	return cBaseWindow::Close(bSound);

}

void cExtraEvolutionUI::Destroy()
{
	
	cBaseWindow::Delete();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cExtraEvolutionUI::DeleteResource()
{
	m_OverlapDigimonSelectUI.DeleteScript();
	m_InputPassowrdUI.DeleteScript();
	DeleteScript();
}

void cExtraEvolutionUI::Create( int nValue)
{
	if( !cBaseWindow::Init() )
		return ;

	int nPosX = (g_nScreenWidth >> 1 ) - (478 >> 1);
	int nPosY = (g_nScreenHeight >> 1) - (327 >> 1);
	InitScript( "SpiritEvo\\Spirit_Main.tga", CsPoint( nPosX, nPosY ), CsPoint( 478, 327 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP );

	std::wstring titleName = GetSystem()->getNpcTypeFromUITitleName();
	AddTitle( titleName.c_str(), CsPoint(0, 0) );

	m_pCloseBtn = AddButton( CsPoint( 450, 9 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga");
	if( m_pCloseBtn )
		m_pCloseBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cExtraEvolutionUI::OnClick_CloseUI);

	m_pEvolutionBtn = AddButton( CsPoint(380, 290), CsPoint(88, 30), cButton::IMAGE_NOR_1, UISTRING_TEXT("EVOLUTION_STORE_ITEM_GET_BUTTON").c_str() );//_LAN( "지급받기" )
	if( m_pEvolutionBtn )
		m_pEvolutionBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cExtraEvolutionUI::OnClick_Evolution);

	// 탭 메뉴
	m_pTabMenu = AddRadioButton( CsPoint( 18, 50 ), CsPoint( 92, 25 ), cCheckBox::IMAGE_RADIO_1, UISTRING_TEXT("EVOLUTION_STORE_DIGIMON_TAB").c_str() );//_LAN( "디지몬" )
	if( m_pTabMenu )
	{
		m_pTabMenu->SetUserData( new sExchangeType(1) );
		m_pTabMenu->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &cExtraEvolutionUI::OnClick_TabMenu );
		cRadioButton* pSecTab = m_pTabMenu->AddNode( CsPoint( 110, 50 ), CsPoint( 92, 25 ), cCheckBox::IMAGE_RADIO_1, UISTRING_TEXT("EVOLUTION_STORE_ITEM_TAB").c_str() );//_LAN( "아이템" )
		if( pSecTab )
			pSecTab->SetUserData( new sExchangeType(2) );
	}

	m_pListBox = NiNew cListBox;
	if( m_pListBox )
	{
		m_pListBox->Init( m_pRoot, CsPoint(17, 81), CsPoint( 210, 187 ), NULL, false );
		m_pListBox->SetItemHeight(34);
		m_pListBox->SetItemGab(CsPoint(0,4));

		cImage* pSelImg = NiNew cImage;
		if( pSelImg )
		{
			pSelImg->Init( NULL, CsPoint( 0, 0), CsPoint(206, 34), "SpiritEvo\\Spirit_Highlight.tga", false );
			pSelImg->SetAlpha(0.5f);
			m_pListBox->SetSelectedImg( pSelImg, true );
		}

		cImage* pOverImg = NiNew cImage;
		if( pOverImg )
		{
			pOverImg->Init( NULL, CsPoint( 0, 0), CsPoint(206, 34), "SpiritEvo\\Spirit_Highlight.tga", false );
			pOverImg->SetAlpha(0.2f);
			m_pListBox->SetMouseOverImg( pOverImg, true );
		}

		m_pListBox->AddEvent( cListBox::LIST_SELECT_CHANGE_ITEM, this, &cExtraEvolutionUI::OnClick_ExChangeList );
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_1, NULL, CsPoint::ZERO, CsPoint(16,230), 
				cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint(16,230) ), 5, false );
			m_pListBox->SetScrollBar( pScrollBar );
		}
		
		AddChildControl( m_pListBox );
	}
	
	m_pNeedItemGrid = NiNew cGridListBox;
	if( m_pNeedItemGrid )
	{
		m_pNeedItemGrid->Init( m_pRoot, CsPoint(253, 81), CsPoint(205, 86), CsPoint(11,11), CsPoint(32,32),cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		AddChildControl( m_pNeedItemGrid );

		_EmptyItemSlot( m_pNeedItemGrid, m_NeedItemControl, 10 );
	}

	m_pNeedSubItemGrid = NiNew cGridListBox;
	if( m_pNeedSubItemGrid )
	{
		m_pNeedSubItemGrid->Init( m_pRoot, CsPoint(253, 206), CsPoint(205, 32), CsPoint(11,0), CsPoint(32,32),cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		AddChildControl( m_pNeedSubItemGrid );
		_EmptyItemSlot( m_pNeedSubItemGrid, m_NeedSubItemControl, 5 );
	}


	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		ti.s_bOutLine = true;
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("EVOLUTION_STORE_NEED_MAIN_MATERIAL_TITLE").c_str() );//_LAN( "필요 아이템 및 디지몬" )
		AddText( &ti, CsPoint( 354, 54 ));
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		ti.s_bOutLine = true;
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("EVOLUTION_STORE_NEED_SUB_MATERIAL_TITLE").c_str() );//_LAN( "보조 재료" )
		AddText( &ti, CsPoint( 354, 180 ));
	}

	{
		m_pNeedMoneyIF = NiNew cMoneyIF;
		m_pNeedMoneyIF->Init( cMoneyIF::DISP_FULL, sMONEY(0, 0, 0), false );
		m_pNeedMoneyIF->SetPos( CsPoint( 318, 248 ) );
		
		AddChildControl( m_pNeedMoneyIF );
	}

	_ResetNeedItemList( m_NeedItemControl );
	_ResetNeedItemList( m_NeedSubItemControl );

	if( m_pTabMenu )
		m_pTabMenu->SetCheckIndex( 0, true );

	m_OverlapDigimonSelectUI.Init( m_pRoot );
	m_OverlapDigimonSelectUI.SetVisible(false);
	m_InputPassowrdUI.Init( m_pRoot );
	m_InputPassowrdUI.SetVisible(false);
	
}

void cExtraEvolutionUI::Render(void)
{
	RenderScript();
	EndRenderScript();
	m_OverlapDigimonSelectUI.RenderScript();
	m_InputPassowrdUI.RenderScript();
	cCreateName::RenderInstance( GetWindowType(), 0 );
}

void cExtraEvolutionUI::ResetDevice()
{	
	ResetDeviceScript();
	m_OverlapDigimonSelectUI.ResetDeviceScript();
	m_InputPassowrdUI.ResetDeviceScript();
}

bool cExtraEvolutionUI::OnEscapeKey()
{
	SAFE_POINTER_RETVAL( GetSystem(), false );

	if(	m_InputPassowrdUI.IsVisible() )
	{
		GetSystem()->ClosePasswordInput();
		return true;
	}
	
	if( m_OverlapDigimonSelectUI.IsVisible() )
	{
		GetSystem()->CloseOverlapDigimonSelectUI();
		return true;
	}

	GetSystem()->CloseWindow();
	return true;
}

void cExtraEvolutionUI::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
	if( GetSystem() )
	{
		CsC_AvObject* pTarget = GetSystem()->GetTargetNpc();	
		if( pTarget )
		{
			assert_cs( pTarget->GetLeafRTTI() == RTTI_NPC );
			if( ( (CNpc*)pTarget )->CheckTamerDist() == false )
				GetSystem()->CloseWindow();
		}
	}

	m_OverlapDigimonSelectUI.UpdateScript( fDeltaTime );
	m_InputPassowrdUI.UpdateScript( fDeltaTime );
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cExtraEvolutionUI::Update_ForMouse(void)
{	
	if( m_OverlapDigimonSelectUI.UpdateMouse() )
		return MUT_NONE;

	if( m_InputPassowrdUI.UpdateMouse() )
		return MUT_NONE;

	cBaseWindow::eMU_TYPE muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_INVITE_GUILD_NAME, 0 );
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;

	muReturn = cCreateName::Update_ForMouseInstance( GetWindowType(), 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;


	if( m_pCloseBtn && m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pEvolutionBtn && m_pEvolutionBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pTabMenu && m_pTabMenu->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;

	if( m_pListBox && m_pListBox->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cListBoxItem const* pOverItem = m_pListBox->GetMouseOverItem();
		SAFE_POINTER_RETVAL(pOverItem, muReturn);

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		sExchangeListData* pInfo = static_cast<sExchangeListData*>(pOverItem->GetUserData());
		SAFE_POINTER_RETVAL(pInfo, muReturn);

		switch( pInfo->usExchangeType )
		{
		case 1:
			pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint(206, 34), TOOLTIP_MAX_SIZE, cTooltip::SPIRIT_DIGIMON, cBaseWindow::WT_SPIRIT_EVOLUTION, pInfo->dwExchangeItemIDX, 0, 0 );
			break;
		case 2:
			pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint(206, 34), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->dwExchangeItemIDX, cBaseWindow::WT_SPIRIT_EVOLUTION );
			break;
		}
		return muReturn;
	}

	if( m_pNeedItemGrid && m_pNeedItemGrid->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cGridListBoxItem const* pOverGrid = m_pNeedItemGrid->GetMouseOverItem();
		SAFE_POINTER_RETVAL(pOverGrid, muReturn);

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		sItemInfo* pInfo = dynamic_cast< sItemInfo* >(pOverGrid->GetUserData());
		SAFE_POINTER_RETVAL(pInfo, muReturn);

		switch( pInfo->nObjectType )
		{
		case 1:
			pToolTip->SetTooltip( pOverGrid->GetWorldPos(), CsPoint(32, 32), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->dwIDX, cBaseWindow::WT_SPIRIT_EVOLUTION );
			break;
		case 2:
			pToolTip->SetTooltip( pOverGrid->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::SPIRIT_DIGIMON, cBaseWindow::WT_SPIRIT_EVOLUTION, pInfo->dwIDX, pInfo->nSubValue, 0 );
			break;
		}
		return muReturn;
	}

	if( m_pNeedSubItemGrid && m_pNeedSubItemGrid->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cGridListBoxItem const* pOverGrid = m_pNeedSubItemGrid->GetMouseOverItem();
		SAFE_POINTER_RETVAL(pOverGrid, muReturn);

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		sItemInfo* pInfo = dynamic_cast< sItemInfo* >(pOverGrid->GetUserData());
		SAFE_POINTER_RETVAL(pInfo, muReturn);

		pToolTip->SetTooltip( pOverGrid->GetWorldPos(), CsPoint(32, 32), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->dwIDX, cBaseWindow::WT_SPIRIT_EVOLUTION );
		return muReturn;
	}

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 478, 30 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

cBaseWindow::eMU_TYPE cExtraEvolutionUI::Update_ForMouse_Disable()	// ActiveWindow 이외에 마우스 Click 안되도록... 
{
	if( m_InputPassowrdUI.UpdateMouse() )
		return MUT_NONE;

	if( m_OverlapDigimonSelectUI.UpdateMouse() )
		return MUT_NONE;

	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cCreateName::Update_ForMouseInstance( GetWindowType(), 0 );

	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	return cBaseWindow::Update_ForMouse_Disable();
}

//////////////////////////////////////////////////////////////////////////

void cExtraEvolutionUI::_EmptyItemSlot( cGridListBox* pGridList, std::list<sNeedItemControls>& continer, int const& nMakeCount )
{
	SAFE_POINTER_RET( pGridList );
	continer.clear();
	
	for( int n = 0; n < nMakeCount; ++n )
	{		
		cString * pItem = NiNew cString;

		sNeedItemControls addControls;
		
		cSprite * pkItemIcon = NiNew cSprite;
		if( pkItemIcon )
		{
			pkItemIcon->Init( NULL, CsPoint::ZERO, CsPoint(32,32), " ", false);
			addControls.pkDigimonIcon = pItem->AddSprite( pkItemIcon, CsPoint(0,0), CsPoint(32,32));	
			if( addControls.pkDigimonIcon )
				addControls.pkDigimonIcon->SetAutoPointerDelete(true);
		}
		
		addControls.pkItemIcon = pItem->AddIcon(CsPoint(32,32), ICONITEM::ITEM_ICON, 0, 0, CsPoint(0,0));	
		if( addControls.pkItemIcon )
			addControls.pkItemIcon->SetAutoPointerDelete(true);

		cSprite* pEnableSlot = NiNew cSprite;
		if( pEnableSlot )
		{
			pEnableSlot->Init( NULL, CsPoint::ZERO, CsPoint(32,32), NiColorA(1.0f,0.0f,0.0f, 0.5f), false);
			addControls.pkEnableSlot = pItem->AddSprite( pEnableSlot, CsPoint(0,0), CsPoint(32,32));	
			if( addControls.pkEnableSlot )
				addControls.pkEnableSlot->SetAutoPointerDelete(true);
		}

		cGridListBoxItem * pGridItem  = NiNew cGridListBoxItem(n,  CsPoint(32,32));
		pGridItem->SetItem( pItem );
		pGridItem->SetUserData( new sItemInfo(0) );
		pGridList->AddItem( pGridItem );
		addControls.pListItemBox = pGridItem;
		continer.push_back( addControls );
	}
}

// 1 : 아이템을 디지몬으로 변환, 2 : 디지몬을 아이템으로 변환
void cExtraEvolutionUI::_LoadItemList( int const& nExchangeType )
{
	SAFE_POINTER_RET( m_pListBox );
	m_pListBox->RemoveAllItem();

	std::list<cEvolutionStoreContents::sExtraExchageData> const* pList = GetSystem()->GetExchageTargetList( nExchangeType );
	SAFE_POINTER_RET( pList );

	std::list<cEvolutionStoreContents::sExtraExchageData>::const_iterator it = pList->begin();
	for( ; it != pList->end(); ++it )
	{
		cListBoxItem* pAddItem = NiNew cListBoxItem;
		SAFE_POINTER_BEK( pAddItem );

		cString * pItem = NiNew cString;
		if( NULL == pItem )
		{
			NISAFE_DELETE( pAddItem );
			break;
		}

		if( !(*it).sDigimonIconName.empty() )
		{
			cSprite* pEvoImage = NiNew cSprite;	// 1
			if( pEvoImage )
			{
				pEvoImage->Init( NULL, CsPoint(1,1), CsPoint( 32, 32 ),  (*it).sDigimonIconName.c_str(), false, NiColor::WHITE, false );
				cString::sSPRITE* sImg = pItem->AddSprite( pEvoImage, CsPoint::ZERO, CsPoint( 32, 32 )  );	
				if( sImg )
					sImg->SetAutoPointerDelete(true);
			}
		}
		else
		{
			cString::sICON* pSkillIcon = pItem->AddIcon( CsPoint(32,32), ICONITEM::ITEM_ICON, (*it).dwObjectIDX, 1,CsPoint(1,1) );
			if( pSkillIcon )
				pSkillIcon->SetAutoPointerDelete(true);
		}

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		ti.SetText( (*it).wsExchageObjectName.c_str() );
		pItem->AddText( &ti, CsPoint(40, 10) );

		pAddItem->SetItem( pItem );
		pAddItem->SetUserData( new sExchangeListData(nExchangeType, (*it).usWayType, (*it).dwObjectIDX) );
		m_pListBox->AddItem( pAddItem );
	}
}

void cExtraEvolutionUI::_LoadNeedItemList( std::list<sNeedItemControls>& pControls, std::list<cEvolutionStoreContents::sMaterialData> const& pItemList )
{
	std::list<sNeedItemControls>::iterator controlIT = pControls.begin();
	std::list<cEvolutionStoreContents::sMaterialData>::const_iterator it = pItemList.begin();
	for( ; it != pItemList.end(); ++it, ++controlIT )
	{
		if( controlIT == pControls.end() )
			break;

		switch( (*it).eMaterialType )
		{
		case cEvolutionStoreContents::sMaterialData::eItemMaterial:
			(*controlIT).SetShowItemIcon( (*it).dwMaterialIDX, (*it).nCount );
			break;
		case cEvolutionStoreContents::sMaterialData::eDigimonMaterial:
			(*controlIT).SetShowDigimonIcon( (*it).dwMaterialIDX, (*it).sDigimonIconName, (*it).nCount );
			break;
		}
		(*controlIT).SetEnableSlot( (*it).bEnable );
	}
}

void cExtraEvolutionUI::_ResetNeedItemList( std::list<sNeedItemControls>& itemList )
{
	std::list<sNeedItemControls>::iterator it = itemList.begin();
	for( ; it != itemList.end(); ++it )
		(*it).SetHide();
}

//////////////////////////////////////////////////////////////////////////
void cExtraEvolutionUI::OnClick_ExChangeList( void* pSender, void* pData )
{
	SAFE_POINTER_RET( pData );
	cListBoxItem* pItem = static_cast<cListBoxItem*>(pData);
	sExchangeListData* pkInfo =	dynamic_cast<sExchangeListData*>(pItem->GetUserData());
	SAFE_POINTER_RET(pkInfo);

	_ResetNeedItemList( m_NeedItemControl );
	_ResetNeedItemList( m_NeedSubItemControl );
	if( m_pNeedMoneyIF )
		m_pNeedMoneyIF->SetMoney( sMONEY( (u8)0 ) );

	cEvolutionStoreContents::sExtraExchageData const* pExData = GetSystem()->GetExchageData( pkInfo->usExchangeType, pkInfo->dwExchangeItemIDX, pkInfo->usWayType );
	if( pExData )
	{
		_LoadNeedItemList( m_NeedItemControl, pExData->MainNeedMaterial );	
		_LoadNeedItemList( m_NeedSubItemControl, pExData->SubNeedMaterial );
		if( m_pNeedMoneyIF )
			m_pNeedMoneyIF->SetMoney( sMONEY( pExData->dwNeedMoney ) );
	}
}

void cExtraEvolutionUI::OnClick_CloseUI( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->CloseWindow();
}

void cExtraEvolutionUI::OnClick_Evolution(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pListBox );
	cListBoxItem const* pSelItem = m_pListBox->GetSelectedItem();
	SAFE_POINTER_RET( pSelItem );
	sExchangeListData* pkInfo =	dynamic_cast<sExchangeListData*>(pSelItem->GetUserData());
	SAFE_POINTER_RET(pkInfo);

	GetSystem()->SendExtraEvolution( pkInfo->usExchangeType, pkInfo->usWayType, pkInfo->dwExchangeItemIDX );
}

void cExtraEvolutionUI::OnClick_TabMenu(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cRadioButton* pRadioButton = static_cast<cRadioButton*>(pData);	
	sExchangeType* userData = dynamic_cast<sExchangeType*>(pRadioButton->GetUserData());
	SAFE_POINTER_RET(userData);

	_LoadItemList( userData->nExchangeType );

	_ResetNeedItemList( m_NeedItemControl );
	_ResetNeedItemList( m_NeedSubItemControl );
	if( m_pNeedMoneyIF )
		m_pNeedMoneyIF->SetMoney( sMONEY( (u8)0 ) );
}

// 아이템 및 디지몬 레벨 변경으로 현재 선택된 필요 및 보조 재료 아이콘 갱신
void cExtraEvolutionUI::_UpdateSelectedNeedItemEnalbe()
{
	cListBoxItem const* pSelItem = m_pListBox->GetSelectedItem();
	SAFE_POINTER_RET( pSelItem );
	sExchangeListData* pkInfo =	dynamic_cast<sExchangeListData*>(pSelItem->GetUserData());
	SAFE_POINTER_RET(pkInfo);

	_ResetNeedItemList( m_NeedItemControl );
	_ResetNeedItemList( m_NeedSubItemControl );
	if( m_pNeedMoneyIF )
		m_pNeedMoneyIF->SetMoney( sMONEY( (u8)0 ) );

	cEvolutionStoreContents::sExtraExchageData const* pExData = GetSystem()->GetExchageData( pkInfo->usExchangeType, pkInfo->dwExchangeItemIDX, pkInfo->usWayType );
	if( pExData )
	{
		_LoadNeedItemList( m_NeedItemControl, pExData->MainNeedMaterial );	
		_LoadNeedItemList( m_NeedSubItemControl, pExData->SubNeedMaterial );
		if( m_pNeedMoneyIF )
			m_pNeedMoneyIF->SetMoney( sMONEY( pExData->dwNeedMoney ) );
	}
}

//////////////////////////////////////////////////////////////////////////
bool cExtraEvolutionUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eUpdateNeedItemCount, this );
	return true;
}

void cExtraEvolutionUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eUpdateNeedItemCount:
		_UpdateSelectedNeedItemEnalbe();
		break;
	}
}
