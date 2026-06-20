#include "stdafx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "ItemProductionShop.h"

//////////////////////////////////////////////////////////////////////////

void cItemProductionShop::sAssistItemUIControls::SetLock(bool bLock)
{
	if( m_pItemIcon )
		m_pItemIcon->SetVisible( !bLock );
	if( m_pAddSlotImg )
		m_pAddSlotImg->SetVisible( !bLock );
	if( m_pLockSlotImg )
		m_pLockSlotImg->SetVisible( bLock );

	if( m_pUserData )
	{
		m_pUserData->s_dwItemCode = 0;
		m_pUserData->s_nItemCount = 0;

		if( !bLock )
		{
			if( m_pItemIcon )
				m_pItemIcon->SetVisible( false );
			if( m_pAddSlotImg )
				m_pAddSlotImg->SetVisible( true );
		}
	}
}

void cItemProductionShop::sAssistItemUIControls::SetItemIcon(int const& nItemCode, int const& nItemCount)
{
	SAFE_POINTER_RET( m_pItemIcon );

	if( m_pLockSlotImg )
		m_pLockSlotImg->SetVisible( false );

	if( 0 != nItemCode )
	{
		m_pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemCode, nItemCount );
		m_pItemIcon->SetVisible(true);
		if( m_pAddSlotImg )
			m_pAddSlotImg->SetVisible( false );
	}
	else
	{
		m_pItemIcon->SetVisible(false);
		if( m_pAddSlotImg )
			m_pAddSlotImg->SetVisible( true );
	}

	if( m_pUserData )
	{
		m_pUserData->s_dwItemCode = nItemCode;
		m_pUserData->s_nItemCount = nItemCount;
	}
}

bool cItemProductionShop::sAssistItemUIControls::UpdateItemcount(int const& nItemCode, int const& nItemCount)
{
	SAFE_POINTER_RETVAL( m_pUserData, false );
	if( nItemCode != m_pUserData->s_dwItemCode )// 등록 되어 있는 아이템과 같지 않으면 리턴
		return false;
	
	if( 0 == nItemCount ) // 아이템의 갯수가 0인경우 아이콘 제거
		SetItemIcon( 0, 0 );
	else
		SetItemIcon( nItemCode, nItemCount );
	return true;
}

DWORD cItemProductionShop::sAssistItemUIControls::GetAssistItemCode() const
{
	SAFE_POINTER_RETVAL( m_pUserData, 0 );
	return m_pUserData->s_dwItemCode;	
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void cItemProductionShop::sMaterialItemInfo::UpdateData()
{
	SAFE_POINTER_RET( m_pText );

	DWORD dwTotalNeedItemcount = m_dwNeedItemCount * m_dwMakeItemCount;
	std::wstring need = DmCS::StringFn::getNumberFormatW( dwTotalNeedItemcount );
	std::wstring have = DmCS::StringFn::getNumberFormatW( m_dwHaveItemCount );

	std::wstring msg = have + _T(" / ") + need;
	m_pText->SetText( msg.c_str() );

	if( dwTotalNeedItemcount > m_dwHaveItemCount )
		m_pText->SetColor( NiColor(1.0f, 0.0f, 0.0f));	
	else
		m_pText->SetColor( NiColor::WHITE);	
}

void cItemProductionShop::sMaterialItemInfo::UpdateMakeItemCount( DWORD const& dwMakeItemcount )
{
	m_dwMakeItemCount = dwMakeItemcount;
	UpdateData();
}

void cItemProductionShop::sMaterialItemInfo::UpdateHaveItemCount( DWORD const& dwHaveItemcount )
{
	m_dwHaveItemCount = dwHaveItemcount;

	UpdateData();	
}

//////////////////////////////////////////////////////////////////////////

//#define MAIN_CATEGOTY_TEXT_COLOR	NiColor( 247/255.0f, 211/255.0f, 153/255.0f )
//#define SUB_CATEGOTY_TEXT_COLOR		NiColor( 247/255.0f, 211/255.0f, 153/255.0f )
#define MAIN_CATEGOTY_TEXT_COLOR	NiColor::WHITE
#define SUB_CATEGOTY_TEXT_COLOR		NiColor::WHITE

#define MAKING_MIN_COUNT			1
#define MAKING_MAX_COUNT			999
#define MAKING_UNIT_COUNT			3	// 최대 표현 자리수 999이기 때문에 3자리

#define MAKEITEM_LINE_WIDTH			140

#define MAKE_GUAGE_TIME				1.0f
#define MAKE_BUTTON_EFFECT_TIME		0.05f

cItemProductionShop::cItemProductionShop():
m_pCloseButton(NULL),m_pProductionCost(NULL)
,m_pProductionItemList(NULL),m_pMaterialItemList(NULL),m_nMakeCount(0)
,m_pBtnIncrease(NULL),m_pBtnDecrease(NULL),m_pBtnIncreaseEnd(NULL),m_pBtnDecreaseEnd(NULL)
,m_pMakingBtn(NULL),m_dwNeedCost(0),m_pSelectedItemInfo(0),m_pEditBox(NULL), m_pItemInfo(NULL)
,m_pSucessPrecentageText(NULL),m_pAddPrecentageText(NULL),m_pProgressBar(NULL),m_pAssisteItemList(NULL)
,m_pSetProtectItemImg(NULL),m_pPercentageUpText(NULL),m_pProtectText(NULL)
{

}

cItemProductionShop::~cItemProductionShop()
{
	Destroy();
}

cBaseWindow::eWINDOW_TYPE cItemProductionShop::GetWindowType()
{ 
	return WT_ITEM_PRODUCTION_SHOP; 
}

void cItemProductionShop::Destroy()
{
	cBaseWindow::Delete();	
}

void cItemProductionShop::DeleteResource()
{
	g_IME.SetNumberOnly( false );

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	//SAFE_NIDELETE( m_pProductionCost );
	if( g_pGameIF && g_pGameIF->GetPopup() )
		g_pGameIF->GetPopup()->ClosePopup( cPopUpWindow::ITEM_MAKE_ASSIST_ITEM_REG );

	DeleteScript();	
}

void cItemProductionShop::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint winPos( (g_nScreenWidth >> 1) - (770 >> 1), (g_nScreenHeight>>1) - (622>>1) );
	InitScript( "Item_Make\\Make_Main_Bag.tga", winPos, CsPoint( 770, 622 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
// 	CsPoint winPos( (g_nScreenWidth >> 1) - (700 >> 1), (g_nScreenHeight>>1) - (506>>1) );
// 	InitScript( "Item_Make\\Make_Main_Bag.tga", winPos, CsPoint( 700, 506 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	AddTitle( UISTRING_TEXT("ITEMPRODUCTION_TITLE_NAME").c_str() );

	cText::sTEXTINFO ti;
	ti.Init();
	ti.SetText( UISTRING_TEXT("ITEMPRODUCTION_PRODUCTION_LIST").c_str() );
	ti.s_eTextAlign = DT_CENTER;
	AddText( &ti, CsPoint( 125, 40 ) );

	ti.Init();
	ti.SetText( UISTRING_TEXT("ITEMPRODUCTION_ITEM_LIST").c_str() );
	ti.s_eTextAlign = DT_CENTER;
	//AddText( &ti, CsPoint( 350, 40 ) );
	AddText( &ti, CsPoint( 380, 40 ) );

	ti.Init();
	ti.SetText( UISTRING_TEXT("ITEMPRODUCTION_NEED_MATERIAL_ITEM_LIST").c_str() );
	ti.s_eTextAlign = DT_CENTER;
	//AddText( &ti, CsPoint( 585, 40 ) );
	AddText( &ti, CsPoint( 650, 40 ) );

	ti.Init();
	ti.SetText( UISTRING_TEXT("ITEMPRODUCTION_PRODUCTION_COST").c_str() );
	ti.s_eTextAlign = DT_CENTER;
	//AddText( &ti, CsPoint( 585, 420 ) );
	AddText( &ti, CsPoint( 121, 571 ) );

	m_pProgressBar = NiNew cUI_TimerProgressBar;
	if( m_pProgressBar )
	{
		m_pProgressBar->Init( m_pRoot, CsPoint( 258, 527 ), CsPoint( 318, 66 ), false );
		m_pProgressBar->SetViewTextType( cUI_ProgressBar::NONE );
		m_pProgressBar->SetGuageImage( "Item_Make\\Make_ani_gauge_pixel.tga" );
		m_pProgressBar->SetCoverImage("Item_Make\\Make_ani_gauge_bg.tga");
		m_pProgressBar->SetStepSize(0.01f);
		m_pProgressBar->SetTimer(0.0f, 2.0f);
		m_pProgressBar->AddEvent( cUI_ProgressBar::eProgressbar_Done, this, &cItemProductionShop::_ProgressbarEnd );
		AddChildControl( m_pProgressBar );
	}

	{	// 아이템 보호 텍스트
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_11, FONT_DARKBLUE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("ITEMPRODUCTION_PRODUCTION_MATERIAL_PROTECT_TEXT").c_str() );//L"아이템 보호"
		m_pProtectText = AddText( &ti, CsPoint( 283, 519 ) );
	}
	
	{	// 제작 활륙 상승 텍스트
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_11, FONT_DARKBLUE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("ITEMPRODUCTION_PRODUCTION_MAKE_SUCCESS_PRECENTAGE_UP_TEXT").c_str() );//L"제작 확률 상승" 
		m_pPercentageUpText = AddText( &ti, CsPoint( 392, 519 ) );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_11, FONT_DARKBLUE);
		ti.s_eTextAlign = DT_CENTER;
		std::wstring wsText;
		DmCS::StringFn::Format( wsText, L"+%d%%", 0 );
		ti.SetText( wsText.c_str() );
		m_pAddPrecentageText = AddText( &ti, CsPoint( 392, 585 ) );
	}

	m_pSetProtectItemImg = NiNew cImage;
	if( m_pSetProtectItemImg )
	{
		m_pSetProtectItemImg->Init( m_pRoot, CsPoint(277,583), CsPoint( 16, 16 ), "Item_Make\\Make_icon_shield.tga", false );
		m_pSetProtectItemImg->SetTexToken( CsPoint( 0, 16 ) );
		m_pSetProtectItemImg->SetState(0);
		AddChildControl( m_pSetProtectItemImg );
	}
	
	{	// 최종 확률 텍스트
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_11, FONT_DARKBLUE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("ITEMPRODUCTION_PRODUCTION_MAKE_SUCCESS_TOTAL_PRECENTAGE_TEXT").c_str() );//L"최종 확률"
		AddText( &ti, CsPoint( 495, 519 ) );
	}

	{	// 최종 확률 퍼센테이지 텍스트^
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_20, FONT_YELLOW);
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		m_pSucessPrecentageText = AddText( &ti, CsPoint( 495, 562 ) );
	}

	m_pAssisteItemList = NiNew cGridListBox;
	if( m_pAssisteItemList )
	{
		m_pAssisteItemList->Init( GetRoot(), CsPoint( 269, 544 ), CsPoint(143, 32)
			,CsPoint(77, 0), CsPoint(32,32), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		m_pAssisteItemList->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &cItemProductionShop::_SelectAssistItemSlot );
		m_pAssisteItemList->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &cItemProductionShop::_ClearAssistItemSlot );
		
		m_pAssisteItemList->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pAssisteItemList->SetBackOverAndSelectedImgRender(false);
		m_pAssisteItemList->SetAutoSelection(false);
		
		AddChildControl(m_pAssisteItemList);
	}


	//m_pCloseButton = AddButton( CsPoint( 675, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pCloseButton = AddButton( CsPoint( 745, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	if( m_pCloseButton )	
		m_pCloseButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cItemProductionShop::CloseWindow );

	//m_pMakingBtn = AddButton( CsPoint( 618, 466 ), CsPoint( 70, 26 ), CsPoint( 0, 26 ), "Item_Make\\Make_BTn.tga" );
	m_pMakingBtn = AddButton( CsPoint( 575, 536 ), CsPoint( 176, 50 ), CsPoint( 0, 50 ), "CommonUI\\Simple_btn_L.tga", cWindow::SD_Bu3 );	
	if( m_pMakingBtn )
	{
		ti.Init(CFont::FS_16);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("ITEMPRODUCTION_PRODUCTION_START").c_str() );
		m_pMakingBtn->SetText( &ti );
		m_pMakingBtn->SetButtonEffect(cSpriteAni::ONCE_LEAVE, CsPoint(-7,-7), CsPoint( 190, 64 ), "Item_Make\\Make_ani_Sequence.dds", NULL, 16, false, CsPoint(190, 64) , cSpriteAni::SPRITE_POS, 2 );
		m_pMakingBtn->SetEffectAniTime( MAKE_BUTTON_EFFECT_TIME );
		m_pMakingBtn->StopEffect();
		m_pMakingBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cItemProductionShop::_ItemMaking );
		m_pMakingBtn->AddEvent( cButton::BUTTON_EFFECT_PLAY_END, this, &cItemProductionShop::_SendItemMaking );
	}

	// 제작 트리 박스
	m_pTreeCategoty = NiNew cTreeBox;
	if( m_pTreeCategoty )
	{
		m_pTreeCategoty->Init( GetRoot(),  CsPoint(8, 57), CsPoint( 225, 440 ), NULL, false );
		m_pTreeCategoty->setChildItemGab(CsPoint(0,0));
		m_pTreeCategoty->AddEvent( cTreeBox::TreeEvent_Selection_Changed, this, &cItemProductionShop::SelectedCategoty );
		AddChildControl(m_pTreeCategoty);
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if( pScrollBar )
		{
			pScrollBar->Init( cScrollBar::TYPE_ENCY, NULL, CsPoint( 0, 0 ), CsPoint( 16, 440 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 183, 66 ), CsPoint( 548, 511 ) ), 4, false );
			pScrollBar->ChangeGBImg( "Item_Make\\Make_WheelBar_Bag_Mid.tga", false );
			m_pTreeCategoty->SetScrollBar( pScrollBar );
		}
	}

	// 제작 아이템 리스트
	m_pProductionItemList = NiNew cListBox;
	if( m_pProductionItemList )
	{
		//m_pProductionItemList->Init( GetRoot(), CsPoint(237, 57), CsPoint( 225, 440 ), NULL, false );
		m_pProductionItemList->Init( GetRoot(), CsPoint(237, 57), CsPoint( 294, 440 ), NULL, false );

		cScrollBar* pScrollBar = NiNew cScrollBar;
		if( pScrollBar )
		{
			pScrollBar->Init( cScrollBar::TYPE_ENCY, NULL, CsPoint( 0, 0 ), CsPoint( 16, 440 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 183, 66 ), CsPoint( 548, 511 ) ), 10, false );
			pScrollBar->ChangeGBImg( "Item_Make\\Make_WheelBar_Bag_Mid.tga", false );
			m_pProductionItemList->SetScrollBar( pScrollBar );
		}

		m_pProductionItemList->AddEvent( cListBox::LIST_SELECT_CHANGE_ITEM, this, &cItemProductionShop::SelectedProductionItem );
		m_pProductionItemList->SetAutoItemSelectStateChange(true);
		AddChildControl( m_pProductionItemList );
	}

	// 재료 아이템 리스트
	m_pMaterialItemList = NiNew cListBox;
	if( m_pMaterialItemList )
	{
		//m_pMaterialItemList->Init( GetRoot(), CsPoint(466, 57), CsPoint( 225, 352 ), NULL, false );
		m_pMaterialItemList->Init( GetRoot(), CsPoint(537, 57), CsPoint( 225, 440 ), NULL, false );
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if( pScrollBar )
		{
			pScrollBar->Init( cScrollBar::TYPE_ENCY, NULL, CsPoint( 0, 0 ), CsPoint( 16, 352 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 183, 66 ), CsPoint( 548, 511 ) ), 4, false );
			pScrollBar->ChangeGBImg( "Item_Make\\Make_WheelBar_Bag_Mid.tga", false );
			m_pMaterialItemList->SetScrollBar( pScrollBar );
		}
		AddChildControl( m_pMaterialItemList );
	}

	//m_pBtnDecreaseEnd = AddButton( CsPoint( 468, 471 ), CsPoint( 22, 16 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Fast_L.tga" );
	m_pBtnDecreaseEnd = AddButton( CsPoint( 47, 527 ), CsPoint( 22, 16 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Fast_L.tga" );
	if( m_pBtnDecreaseEnd )
		m_pBtnDecreaseEnd->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cItemProductionShop::OnDecreaseEnd );

	//m_pBtnDecrease = AddButton( CsPoint( 492, 471 ), CsPoint( 14, 16 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Slow_L.tga" );
	m_pBtnDecrease = AddButton( CsPoint( 72, 527 ), CsPoint( 14, 16 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Slow_L.tga" );
	if( m_pBtnDecrease )
		m_pBtnDecrease->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cItemProductionShop::OnDecrease );

	//m_pBtnIncrease = AddButton( CsPoint( 576, 471 ), CsPoint( 14, 16 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Slow_R.tga" );
	m_pBtnIncrease = AddButton( CsPoint( 157, 527 ), CsPoint( 14, 16 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Slow_R.tga" );
	if( m_pBtnIncrease )
		m_pBtnIncrease->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cItemProductionShop::OnIncrease );

	//m_pBtnIncreaseEnd = AddButton( CsPoint( 592, 471 ), CsPoint( 22, 16 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Fast_R.tga" );
	m_pBtnIncreaseEnd = AddButton( CsPoint( 174, 527 ), CsPoint( 22, 16 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Fast_R.tga" );
	if( m_pBtnIncreaseEnd )
		m_pBtnIncreaseEnd->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cItemProductionShop::OnIncreaseEnd );

	//AddSprite( CsPoint( 510, 466 ), CsPoint( 62, 26 ), "Item_Make\\Make_NBr_Bag.tga" );
	AddSprite( CsPoint( 91, 522 ), CsPoint( 62, 26 ), "Item_Make\\Make_NBr_Bag.tga" );

	//
	m_pProductionCost = NiNew cMoneyIF;
	if( m_pProductionCost )
	{
		m_pProductionCost->Init(cMoneyIF::DISP_FULL, sMONEY(m_dwNeedCost), false, FONT_WHITE, DT_RIGHT, CFont::FS_11 );
		AddChildControl( m_pProductionCost );
		m_pProductionCost->SetPos( CsPoint( 87, 588 ) );
	}
	

	m_pEditBox = NiNew cEditBox;
	if( m_pEditBox )
	{
		ti.Init();
		ti.s_eFontSize = CFont::FS_11;
		ti.s_eTextAlign = DT_CENTER;
		//m_pEditBox->Init( GetRoot(), CsPoint( 510, 466 ), CsPoint( 62, 26 ), &ti, false );
		m_pEditBox->Init( GetRoot(), CsPoint( 91, 522 ), CsPoint( 62, 26 ), &ti, false );
		m_pEditBox->SetFontLength( MAKING_UNIT_COUNT );
		m_pEditBox->EnableUnderline( ti.s_Color );
		m_pEditBox->AddEvent( cEditBox::eEditbox_ChangeText, this, &cItemProductionShop::ChangeInputMakeCount );

		AddChildControl( m_pEditBox );
	}

	_SetAssistItemSlotData();

	changeMakeCount( MAKING_MIN_COUNT );

	g_IME.SetNumberOnly( true );

	_SetAssistItemLock(true);
	_StopMakingProgress();
}

void cItemProductionShop::_SetAssistItemSlotData()
{
	m_mapAssistControl.clear();

	SAFE_POINTER_RET( m_pAssisteItemList );
	m_pAssisteItemList->RemoveAllItem();

	// 아이템 보호 
	{
		cString * pItem = NiNew cString;
		SAFE_POINTER_RET( pItem );	

		sAssistItemUIControls controls;
		controls.m_pItemIcon = pItem->AddIcon( CsPoint(32,32), ICONITEM::ITEM_ICON, 0, 1 );
		if( controls.m_pItemIcon )
			controls.m_pItemIcon->SetAutoPointerDelete(true);

		controls.m_pAddSlotImg = NiNew cSprite;
		controls.m_pAddSlotImg->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), "Item_Make\\Make_btn_plus.tga", false );
		cString::sSPRITE* paddImg = pItem->AddSprite( controls.m_pAddSlotImg, CsPoint::ZERO, CsPoint( 32, 32 )  );	
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);

		controls.m_pLockSlotImg = NiNew cSprite;
		controls.m_pLockSlotImg->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), "Item_Make\\Make_icon_lock.tga", false );
		cString::sSPRITE* pLockImg = pItem->AddSprite( controls.m_pLockSlotImg, CsPoint::ZERO, CsPoint( 32, 32 )  );	
		if( pLockImg )
			pLockImg->SetAutoPointerDelete(true);

		controls.m_pUserData = new sAssistItem( sAssistanceItemGroup::eProtectAssist, 0, 0 );

		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(0, CsPoint( 32, 32 ));
		addItem->SetItem( pItem );
		addItem->SetUserData( controls.m_pUserData );
		m_pAssisteItemList->AddItem( addItem );
		m_mapAssistControl.insert( std::make_pair(sAssistanceItemGroup::eProtectAssist, controls) );
	}

	// 제작 확률 상승 
	{
		cString * pItem = NiNew cString;
		SAFE_POINTER_RET( pItem );	

		sAssistItemUIControls controls;
		controls.m_pItemIcon = pItem->AddIcon( CsPoint(32,32), ICONITEM::ITEM_ICON, 0, 1 );
		if( controls.m_pItemIcon )
			controls.m_pItemIcon->SetAutoPointerDelete(true);

		controls.m_pAddSlotImg = NiNew cSprite;
		controls.m_pAddSlotImg->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), "Item_Make\\Make_btn_plus.tga", false );
		cString::sSPRITE* paddImg = pItem->AddSprite( controls.m_pAddSlotImg, CsPoint::ZERO, CsPoint( 32, 32 )  );	
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);

		controls.m_pLockSlotImg = NiNew cSprite;
		controls.m_pLockSlotImg->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), "Item_Make\\Make_icon_lock.tga", false );
		cString::sSPRITE* pLockImg = pItem->AddSprite( controls.m_pLockSlotImg, CsPoint::ZERO, CsPoint( 32, 32 )  );	
		if( pLockImg )
			pLockImg->SetAutoPointerDelete(true);

		controls.m_pUserData = new sAssistItem( sAssistanceItemGroup::ePercentageAssist, 0, 0 );

		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(1, CsPoint( 32, 32 ));
		addItem->SetItem( pItem );
		addItem->SetUserData( controls.m_pUserData );
		m_pAssisteItemList->AddItem( addItem );
		m_mapAssistControl.insert( std::make_pair(sAssistanceItemGroup::ePercentageAssist, controls) );
	}

}

void cItemProductionShop::_ProgressbarEnd( void* pSender, void* pData )
{
	SAFE_POINTER_RET( m_pMakingBtn );
	m_pMakingBtn->StartEffect(true);
	m_pMakingBtn->SetEnable(false);
}

void cItemProductionShop::_SelectAssistItemSlot(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( m_pItemInfo );

	cGridListBoxItem* pSelItem = static_cast<cGridListBoxItem*>(pData);
	sAssistItem* pUserData = dynamic_cast<sAssistItem*>(pSelItem->GetUserData());
	SAFE_POINTER_RET( pUserData );	

	switch( pUserData->s_nAssistType )
	{
	case sAssistanceItemGroup::ePercentageAssist:
		{
			if( 0 == m_pItemInfo->m_nPercentageAssistItemGroup )
				return;

			cPopUpWindow* pPopupWin = g_pGameIF->GetPopup();
			SAFE_POINTER_RET( pPopupWin );

			pPopupWin->SetPopup( pSelItem->GetWorldPos() + CsPoint(32,0), CsPoint( 120, 0 ), cPopUpWindow::ITEM_MAKE_ASSIST_ITEM_REG, 
								 pUserData->s_nAssistType, m_pItemInfo->m_nPercentageAssistItemGroup );
		}
		break;
	case sAssistanceItemGroup::eProtectAssist:
		{
			if( 0 == m_pItemInfo->m_nProtectAssistItemGroup )
				return;

			cPopUpWindow* pPopupWin = g_pGameIF->GetPopup();
			SAFE_POINTER_RET( pPopupWin );

			pPopupWin->SetPopup( pSelItem->GetWorldPos() + CsPoint(32,0), CsPoint( 120, 0 ), cPopUpWindow::ITEM_MAKE_ASSIST_ITEM_REG, 
								 pUserData->s_nAssistType, m_pItemInfo->m_nProtectAssistItemGroup );
		}
		break;
	}	
}

void cItemProductionShop::_ClearAssistItemSlot(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( m_pItemInfo );

	cGridListBoxItem* pSelItem = static_cast<cGridListBoxItem*>(pData);
	sAssistItem* pUserData = dynamic_cast<sAssistItem*>(pSelItem->GetUserData());
	SAFE_POINTER_RET( pUserData );	

	if( 0 != pUserData->s_dwItemCode )
		GetSystem()->ReSetAssistItem(pUserData->s_nAssistType);
}

void cItemProductionShop::Update(float const& fDeltaTime)
{
	if( GetSystem() )
	{
		CsC_AvObject* pTarget = GetSystem()->GetTargetNpc();	
		if( pTarget )
		{
			assert_cs( pTarget->GetLeafRTTI() == RTTI_NPC );
			if( ( (CNpc*)pTarget )->CheckTamerDist() == false )
				GetSystem()->closeWindow();
		}
	}

	UpdateScript( fDeltaTime );
	_UpdateMoveWindow();
}

bool cItemProductionShop::OnEscapeKey()
{
	CloseWindow(NULL, NULL);
	return true;
}

cBaseWindow::eMU_TYPE cItemProductionShop::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	//if( muReturn == MUT_OUT_WINDOW )	
	//	return muReturn;

	if( m_pCloseButton && m_pCloseButton->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pMakingBtn && m_pMakingBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pTreeCategoty && m_pTreeCategoty->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return muReturn;

	if(m_pAssisteItemList&& m_pAssisteItemList->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cGridListBoxItem const* pOverItem = m_pAssisteItemList->GetMouseOverItem();
		SAFE_POINTER_RETVAL(pOverItem, muReturn);

		sAssistItem* pInfo = static_cast<sAssistItem*>(pOverItem->GetUserData());
		SAFE_POINTER_RETVAL(pInfo, muReturn);

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		if( 0 != pInfo->s_dwItemCode )
			pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->s_dwItemCode, cBaseWindow::WT_NONE );
		return muReturn;
	}

	if( m_pProductionItemList && m_pProductionItemList->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cListBoxItem const* pOverItem = m_pProductionItemList->GetMouseOverItem();
		SAFE_POINTER_RETVAL(pOverItem, muReturn);

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);

		sProductionType* pInfo = static_cast<sProductionType*>(pOverItem->GetUserData());				
		//pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 209, 44 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->m_dwItemIdx, cBaseWindow::WT_ITEM_PRODUCTION_SHOP );
		pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 279, 44 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->m_dwItemIdx, cBaseWindow::WT_ITEM_PRODUCTION_SHOP );
		return muReturn;
	}

	if( m_pMaterialItemList && m_pMaterialItemList->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cListBoxItem const* pOverItem = m_pMaterialItemList->GetMouseOverItem();
		SAFE_POINTER_RETVAL(pOverItem, muReturn);
		
		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, muReturn);
		
		sMaterialType* pInfo = static_cast<sMaterialType*>(pOverItem->GetUserData());				
		pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 209, 44 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->m_dwItemIdx, cBaseWindow::WT_ITEM_PRODUCTION_SHOP );
		return muReturn;
	}
	
	if( m_pBtnDecrease && m_pBtnDecrease->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pBtnDecreaseEnd && m_pBtnDecreaseEnd->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pBtnIncrease && m_pBtnIncrease->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pBtnIncreaseEnd && m_pBtnIncreaseEnd->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	// 아이템 갯수 입력창 활성화시. 엔터키 or ESC키 누르면 활성화 해제.
	if( GLOBALINPUT_ST.IsEscape())
	{
		if( m_pEditBox && m_pEditBox->IsFocus() )
			m_pEditBox->ReleaseFocus(false);
	}
	else if( GLOBALINPUT_ST.IsEnter() )
	{
		if( m_pEditBox && m_pEditBox->IsFocus() )
		{
			m_pEditBox->ReleaseFocus(false);
			SendMessage(GAMEAPP_ST.GetHWnd(), WM_KEYDOWN, VK_RETURN, 0);
		}
	}
	else if( m_pEditBox && m_pEditBox->Update_ForMouse( CsPoint::ZERO ) == cEditBox::ACTION_NONE )
	{
		if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN)
			m_pEditBox->ReleaseFocus(false);
	}
	
	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	//if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 700, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 770, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();		

	return muReturn;
}

void cItemProductionShop::CloseWindow( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->closeWindow();
}

cBaseWindow::eMU_TYPE cItemProductionShop::Update_ForMouse_Disable()
{
	return cBaseWindow::Update_ForMouse_Disable();
}

void cItemProductionShop::Render()
{
	RenderScript();
	EndRenderScript();
}

void cItemProductionShop::ResetDevice()
{	
	ResetDeviceScript();
}

// 현재 가지고 있는 재료로 만들수 있는 최대 갯수를 얻는 함수
int cItemProductionShop::GetMaxCanMakeItemCount_Material() const
{
	int nMaxMakeCount = 0;
	SAFE_POINTER_RETVAL( m_pItemInfo, nMaxMakeCount);	
	LIST_MaterialList_CIT it = m_pItemInfo->m_MaterialList.begin();
	for( ; it != m_pItemInfo->m_MaterialList.end(); ++it )
	{
		DWORD dwHaveItemCount = GetSystem()->GetHaveItemCount( it->m_dwItemIdx );
		DWORD dwCanMakeCount = dwHaveItemCount/it->m_nItemNum;
		if( nMaxMakeCount == 0 )
			nMaxMakeCount = dwCanMakeCount;
		else if( nMaxMakeCount > dwCanMakeCount )
			nMaxMakeCount = dwCanMakeCount;
	}

	return nMaxMakeCount;
}

// 현재 가지고 있는 돈으로 만들수 있는 최대 갯수를 얻는 함수
DWORD cItemProductionShop::GetMaxCanMakeItemCount_Money()
{
	if( 0 == m_dwNeedCost )
		return MAKING_MAX_COUNT;

	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, 0 );

	sMONEY	myMoney = pInven->GetMoney();	// 소지금
	u8	MyPrice = myMoney.GetInt();
	DWORD dwMaxCanMakeCount = MyPrice / m_dwNeedCost;
	
	return __min( dwMaxCanMakeCount, MAKING_MAX_COUNT );
}

// 등록된 보조 재료로 만들수 있는 최대 갯수를 얻는 함수
DWORD cItemProductionShop::GetMaxCanMakeItemCount_AssistItem( int const& nAssistType ) const
{
	DWORD dwRegAssistItemCode = GetAssistItemCode( nAssistType );
	if( 0 != dwRegAssistItemCode )
		return GetSystem()->GetHaveItemCount( dwRegAssistItemCode );
	return MAKING_MAX_COUNT;// 보조 재료가 등록 되어 있지 않는 경우 최대로 만들 수 있는 갯수 셋팅
}

// 총 제작 가능한 갯수
DWORD cItemProductionShop::MakeItemTotalCount()
{
	DWORD dwMaxMakeItem_Money = GetMaxCanMakeItemCount_Money();
	DWORD dwMaxMakeItem_Material = GetMaxCanMakeItemCount_Material();
	DWORD dwMaxMakeItem_ProtectAssist = GetMaxCanMakeItemCount_AssistItem( sAssistanceItemGroup::eProtectAssist );
	DWORD dwMaxMakeItem_PercentageAssist = GetMaxCanMakeItemCount_AssistItem( sAssistanceItemGroup::ePercentageAssist );
	DWORD dwResult = __min(__min(__min( dwMaxMakeItem_Money, dwMaxMakeItem_Material ),dwMaxMakeItem_ProtectAssist),dwMaxMakeItem_PercentageAssist);
	return dwResult;
}

bool cItemProductionShop::IsMakeItemMoney(u8 makeCount)
{
	bool	bMakeSuccess = true;							// 제작 가능.불가능 판단.
	sMONEY	myMoney = g_pDataMng->GetInven()->GetMoney();	// 소지금

	u8	MyPrice = myMoney.GetInt();
	u8 dd = (u8)m_dwNeedCost * (u8)makeCount;
	sMONEY needMoney = sMONEY(dd);
	u8	MakePrice = needMoney.GetInt();
	if( MyPrice < MakePrice )								// 제작비가 부족하면.
		bMakeSuccess = false;

	return bMakeSuccess;
}

// 재료 아이템 목록
void cItemProductionShop::MakeMaterialItemList()
{
	m_mapMaterialCountText.clear();

	SAFE_POINTER_RET( m_pMaterialItemList );
	m_pMaterialItemList->RemoveAllItem();

	SAFE_POINTER_RET( m_pItemInfo );

	LIST_MaterialList const& materialItemList = m_pItemInfo->m_MaterialList;
	LIST_MaterialList_CIT it = materialItemList.begin();
	for( int n = 0; it != materialItemList.end(); ++it, ++n )
	{
		cListBoxItem* pAddItem = NiNew cListBoxItem;
		SAFE_POINTER_BEK( pAddItem );

		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );		

		cButton* pButton = NiNew cButton;
		SAFE_POINTER_BEK( pButton );

		pButton->Delete();
		pButton->Init( NULL, CsPoint::ZERO, CsPoint( 209, 44), "Item_Make\\Make_Slot_BTn3.tga", false );
		pButton->SetTexToken(CsPoint(0,44));
		pButton->SetEnable(false);

		pItem->Delete();
		pItem->AddButton( pButton, n, CsPoint( 0, 0), CsPoint( 209, 44) );
		pItem->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, it->m_dwItemIdx, 1, CsPoint( 6, 6 ) );		
		CsItem::sINFO const* pFTItemInfo = GetSystem()->GetItemInfo( it->m_dwItemIdx );
		if( pFTItemInfo )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
			ti.s_Color = g_pDataMng->GetItemColor( pFTItemInfo );
			ti.SetText( pFTItemInfo->s_szName );
			/////////////// 문자열을 길이이에 맞게 자름.//////////////////////////////
			cStringList sl;
			g_pStringAnalysis->MaxLine_Cut(L"...", &sl, MAKEITEM_LINE_WIDTH, (TCHAR*)pFTItemInfo->s_szName, &ti, 0 );
			//////////////////////////////////////////////////////////////////////////

			pItem->AddText( &ti, CsPoint( 50, 10 ) );
		}	


		DWORD dwHaveItemCount = GetSystem()->GetHaveItemCount( it->m_dwItemIdx );
		sMaterialItemInfo sInfo;
		sInfo.m_dwMakeItemCount = MAKING_MIN_COUNT;
		sInfo.m_dwNeedItemCount = it->m_nItemNum;	// 갯수별 필요 아이템 갯수로 변경
		sInfo.m_dwHaveItemCount = dwHaveItemCount;

		cText::sTEXTINFO ti2;
		ti2.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		sInfo.m_pText = pItem->AddText( &ti2, CsPoint( 50, 25 ) );
		sInfo.UpdateData();

		m_mapMaterialCountText.insert( make_pair( it->m_dwItemIdx, sInfo ) );

		pAddItem->SetUserData(new sMaterialType( it->m_dwItemIdx ));
		pAddItem->SetItem( pItem );

		m_pMaterialItemList->AddItem( pAddItem );
	}
}
// 제작 아이템 목록
void cItemProductionShop::MakeProductionItemList( LIST_ProductionList const& productionItemList, int const& nMainIdx, int const& nSubIdx )
{
	SAFE_POINTER_RET( m_pProductionItemList );
	m_pProductionItemList->RemoveAllItem();

	if(m_pMaterialItemList)
		m_pMaterialItemList->RemoveAllItem();
	m_mapMaterialCountText.clear();

	LIST_ProductionList_CIT it = productionItemList.begin();
	for( int n = 0; it != productionItemList.end(); ++it, ++n )
	{
		cListBoxItem* pAddItem = NiNew cListBoxItem;
		SAFE_POINTER_BEK( pAddItem );

		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cButton* pButton = NiNew cButton;
		SAFE_POINTER_BEK( pButton );

		pButton->Delete();
		//pButton->Init( NULL, CsPoint::ZERO, CsPoint( 209, 44), "Item_Make\\Make_Slot_BTn3.tga", false );
		pButton->Init( NULL, CsPoint::ZERO, CsPoint( 279, 44), "Item_Make\\Make_Slot_BTn4.tga", false );
		pButton->SetTexToken(CsPoint(0,44));

		pItem->Delete();
		//pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 209, 44) );
		pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 279, 44) );
		pItem->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, it->m_dwItemIdx, it->m_nItemNum, CsPoint( 6, 6 ) );
		CsItem::sINFO const* pFTItemInfo = GetSystem()->GetItemInfo( it->m_dwItemIdx );
		if( pFTItemInfo )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
			ti.s_Color = g_pDataMng->GetItemColor( pFTItemInfo );
			//ti.s_eTextAlign = DT_LEFT;
/////////////// 문자열을 길이이에 맞게 자름.//////////////////////////////
			cStringList sl;
			g_pStringAnalysis->MaxLine_Cut(L"...", &sl, MAKEITEM_LINE_WIDTH, (TCHAR*)pFTItemInfo->s_szName, &ti, 0);
//////////////////////////////////////////////////////////////////////////
			pItem->AddText( &ti, CsPoint( 50, 10 ) );
		}
		
		pItem->AddMoneyIF( cMoneyIF::DISP_FULL, it->m_dwItemCost, FONT_WHITE, TOOLTIP_FONT_1_SIZE, DT_RIGHT, CsPoint( 60, 22 ) );

		{
			int nPercentage = it->m_nProbabilityofSuccess / 100;
			NiColor fontColor = _GetPercentageFontColor( nPercentage );			

			cText::sTEXTINFO ti;
			ti.Init( CFont::FS_14, fontColor );
			ti.s_eTextAlign = DT_RIGHT;			
			std::wstring percent;
			DmCS::StringFn::Format( percent, L"%d%%", nPercentage );
			ti.SetText(percent.c_str());
			pItem->AddText( &ti, CsPoint( 270, 13 ) );
		}

		pAddItem->Delete();
		pAddItem->SetUserData(new sProductionType( nMainIdx, nSubIdx, (*it).m_nUniqueIdx, it->m_dwItemIdx ));
		pAddItem->SetItem( pItem );

		m_pProductionItemList->AddItem( pAddItem );
	}	
}

NiColor cItemProductionShop::_GetPercentageFontColor( int const& nPercentage )
{
	if( nPercentage >= 100 )		return NiColor( 0.0f, 1.0f, 0.0f);
	else if( nPercentage >= 50 )	return NiColor::WHITE;
	else							return NiColor( 235.0f/255.0f, 97.0f/255.0f, 0.0f);
}

void cItemProductionShop::MakeSubCategoty( cTreeBoxItem & Root, MAP_Sub_Categoty const& subItem, int const& nMainIdx )
{
	MAP_Sub_Categoty_CIT it = subItem.begin();
	for( ; it != subItem.end(); ++it )
	{
		cTreeBoxItem* pAddItem = NiNew cTreeBoxItem;
		SAFE_POINTER_BEK( pAddItem );

		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		//pItem->AddCheckBox( "Item_Make\\Make_Slot_BTn2.tga", false, CsPoint(0,22), CsPoint( 209, 22) );
		cButton* pButton = NiNew cButton;
		SAFE_POINTER_BEK( pButton );
		pButton->Init( NULL, CsPoint::ZERO, CsPoint( 209, 22), "Item_Make\\Make_Slot_BTn2.tga", false );
		pButton->SetTexToken(CsPoint(0,22));
		pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 209, 22) );

		cText::sTEXTINFO tiGroup;
		tiGroup.Init( CFont::FS_10, SUB_CATEGOTY_TEXT_COLOR );
		tiGroup.SetText( it->second.m_wsCategotyName.c_str() );
		pItem->AddText( &tiGroup, CsPoint(15, 5) );

		pAddItem->SetItem( pItem );			
		pAddItem->SetUserData( new sCategotyType( nMainIdx, it->first ) );
		Root.AddChildItem( pAddItem );
	}
}

void cItemProductionShop::MakeCategoty()
{
	MAP_Main_Categoty const* pList = GetSystem()->GetItemProductionList();
	SAFE_POINTER_RET( pList );

	MAP_Main_Categoty_CIT it = pList->begin();
	for( ; it != pList->end(); ++it )
	{
		// 메인 카테고리
		cTreeBoxItem* pAddItem = NiNew cTreeBoxItem;
		SAFE_POINTER_BEK( pAddItem );

		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cButton* pButton = NiNew cButton;
		SAFE_POINTER_BEK( pButton );
		pButton->Init( NULL, CsPoint::ZERO, CsPoint( 209, 22), "Item_Make\\Make_Slot_BTn1.tga", false );
		pButton->SetTexToken(CsPoint(0,22));
		pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 209, 22) );

		cText::sTEXTINFO tiGroup;
		tiGroup.Init( CFont::FS_10, MAIN_CATEGOTY_TEXT_COLOR );
		tiGroup.SetText( it->second.m_wsCategotyName.c_str() );
		pItem->AddText( &tiGroup, CsPoint(25, 5) );

		pAddItem->SetItem( pItem );

		cButton* pOpenBtn = NiNew cButton;
		SAFE_POINTER_BEK( pOpenBtn );
		pOpenBtn->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14), "Item_Make\\Make_Slot_Plus.tga", false );
		pOpenBtn->SetTexToken(CsPoint(0,14));
		pAddItem->SetOpenButton( pOpenBtn, CsPoint(5, 4) );

		cButton* pCloseBtn = NiNew cButton;
		SAFE_POINTER_BEK( pCloseBtn );
		pCloseBtn->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14), "Item_Make\\Make_Slot_Minus.tga", false );
		pCloseBtn->SetTexToken(CsPoint(0,14));
		pAddItem->SetCloseButton( pCloseBtn, CsPoint(5, 4) );

		MakeSubCategoty( *pAddItem, it->second.m_mapSubCategoty, it->first );
		m_pTreeCategoty->AddItem( pAddItem );
	}
}

void cItemProductionShop::SelectedCategoty(void* pSender, void* pData)
{
	cTreeBoxItem* pItem = static_cast<cTreeBoxItem*>(pData);
	sCategotyType* pUserData = dynamic_cast<sCategotyType*>(pItem->GetUserData());
	SAFE_POINTER_RET( pUserData );

	LIST_ProductionList const* pItemList = GetSystem()->GetProductionList(pUserData->m_nMainIdx, pUserData->m_nSubIdx);
	SAFE_POINTER_RET( pItemList );

	m_pItemInfo = NULL;
	m_pSelectedItemInfo = NULL;
	
	SetSingleCost(0);
	MakeProductionItemList( *pItemList, pUserData->m_nMainIdx, pUserData->m_nSubIdx );
	changeMakeCount( MAKING_MIN_COUNT );	// 생성 갯수 1개로 초기화

	_SetAssistItemLock(true);
	_StopMakingProgress();

	_UpdateUIAssistItemCount( sAssistanceItemGroup::ePercentageAssist );
	_UpdateUIAssistItemCount( sAssistanceItemGroup::eProtectAssist );
}

void cItemProductionShop::SelectedProductionItem(void* pSender, void* pData)
{
	cListBoxItem* pItem = static_cast<cListBoxItem*>(pData);
	sProductionType* pUserData = dynamic_cast<sProductionType*>(pItem->GetUserData());
	SAFE_POINTER_RET( pUserData );

	m_pItemInfo = GetSystem()->GetMaterialItemList(pUserData->m_nMainIdx, pUserData->m_nSubIdx, pUserData->m_nUniqueIdx );
	SAFE_POINTER_RET( m_pItemInfo );

	m_pSelectedItemInfo = pUserData;

	SetSingleCost(m_pItemInfo->m_dwItemCost);

	_SetAssistItemLock( sAssistanceItemGroup::ePercentageAssist, m_pItemInfo->m_nPercentageAssistItemGroup > 0 ? false : true );
	_SetAssistItemLock( sAssistanceItemGroup::eProtectAssist, m_pItemInfo->m_nProtectAssistItemGroup > 0 ? false : true  );

	MakeMaterialItemList();

	changeMakeCount( MAKING_MIN_COUNT );	// 생성 갯수 1개로 초기화

	_StopMakingProgress();

	_UpdateUIAssistItemCount( sAssistanceItemGroup::ePercentageAssist );
	_UpdateUIAssistItemCount( sAssistanceItemGroup::eProtectAssist );
}

void cItemProductionShop::SetSingleCost( u8 const& dwNeedCost )
{
	m_dwNeedCost = dwNeedCost;
}

void cItemProductionShop::_SetAssistItemLock( bool bLock )
{
	MAP_AssistItems_IT it = m_mapAssistControl.begin();
	for( ; it != m_mapAssistControl.end(); ++it )
		it->second.SetLock(bLock);
}

void cItemProductionShop::_SetAssistItemLock( int const& nKey, bool bLock )
{
	MAP_AssistItems_IT it = m_mapAssistControl.find(nKey);
	if( it != m_mapAssistControl.end() )
		it->second.SetLock(bLock);
}

void cItemProductionShop::_UpdateUIAssistItemCount( int const& nAssistType )
{
	if( sAssistanceItemGroup::ePercentageAssist == nAssistType )
	{
		UpdateAddPrecentageText();
		UpdateSucessPrecentageText();
	}
	else
	{
		DWORD dwRegAssistItemCode = GetAssistItemCode( sAssistanceItemGroup::eProtectAssist );
		if( 0 != dwRegAssistItemCode )
		{
			if( m_pSetProtectItemImg )
				m_pSetProtectItemImg->SetState(1);
			if( m_pProtectText )
				m_pProtectText->SetColor( FONT_GREEN );
		}
		else
		{
			if( m_pSetProtectItemImg )
				m_pSetProtectItemImg->SetState(0);
			if( m_pProtectText )
				m_pProtectText->SetColor( FONT_DARKBLUE );
		}
	}
}

void cItemProductionShop::_SetAssitItem( int const& nAssistType, DWORD const& nItemCode, DWORD const& nItemCount)
{
	MAP_AssistItems_IT it = m_mapAssistControl.find(nAssistType);
	if( it == m_mapAssistControl.end() )
		return;
	
	it->second.SetItemIcon( nItemCode, nItemCount );
	_UpdateUIAssistItemCount( nAssistType );
}

void cItemProductionShop::UpdateAddPrecentageText()
{
	SAFE_POINTER_RET( m_pAddPrecentageText );
	std::wstring wsText;
	NiColor fontColor = FONT_DARKBLUE;
	int nCurrentPercentage = 0;
	if( m_pItemInfo )
	{
		DWORD dwRegAssistItemCode = GetAssistItemCode( sAssistanceItemGroup::ePercentageAssist );
		if( 0 != dwRegAssistItemCode )
		{
			int nPercentage = GetSystem()->GetAssistItemPercentage(m_pItemInfo->m_nPercentageAssistItemGroup, dwRegAssistItemCode );
			nCurrentPercentage = nPercentage / 100;
			fontColor = FONT_GREEN;
		}
	}			

	if( m_pAddPrecentageText )
	{
		m_pAddPrecentageText->SetColor( fontColor );
		DmCS::StringFn::Format( wsText, L"+%d%%", nCurrentPercentage );
		m_pAddPrecentageText->SetText( wsText.c_str() );
	}
	if( m_pPercentageUpText )
		m_pPercentageUpText->SetColor( fontColor );
}

void cItemProductionShop::UpdateSucessPrecentageText()
{
	SAFE_POINTER_RET( m_pSucessPrecentageText );

	std::wstring wsPrecent;

	if( m_pItemInfo )
	{
		int nTotalPercentage = m_pItemInfo->m_nProbabilityofSuccess;
		DWORD dwRegAssistItemCode = GetAssistItemCode( sAssistanceItemGroup::ePercentageAssist );
		if( 0 != dwRegAssistItemCode )
		{
			int nPercentage = GetSystem()->GetAssistItemPercentage(m_pItemInfo->m_nPercentageAssistItemGroup, dwRegAssistItemCode);
			nTotalPercentage += nPercentage;
		}

		int nPrecent = nTotalPercentage / 100;
		if( nPrecent > 100 )
			nPrecent = 100;
		DmCS::StringFn::Format( wsPrecent, L"%d%%", nPrecent );

		NiColor fontColor = _GetPercentageFontColor( nPrecent );
		m_pSucessPrecentageText->SetColor( fontColor );
	}	

	m_pSucessPrecentageText->SetText( wsPrecent.c_str() );
}

void cItemProductionShop::changeProductionCost( u8 const& dwNeedCost )
{
	SAFE_POINTER_RET( m_pProductionCost );

	if( IsMakeItemMoney(m_nMakeCount) )
		m_pProductionCost->SetColor(FONT_WHITE);
	else
		m_pProductionCost->SetColor(FONT_RED);

	m_pProductionCost->SetMoney( sMONEY(dwNeedCost) );
}

void cItemProductionShop::OnDecrease(void* pSender, void* pData)
{
	changeMakeCount( __max(m_nMakeCount - 1, MAKING_MIN_COUNT) );
}

void cItemProductionShop::OnDecreaseEnd(void* pSender, void* pData)
{
	changeMakeCount( MAKING_MIN_COUNT );
}

void cItemProductionShop::OnIncrease(void* pSender, void* pData)
{
	changeMakeCount( __min(m_nMakeCount + 1, MAKING_MAX_COUNT) );
}

void cItemProductionShop::OnIncreaseEnd(void* pSender, void* pData)
{
	int maxcount = MakeItemTotalCount();

	if(maxcount < MAKING_MIN_COUNT)		// 0인경우.
		maxcount = MAKING_MIN_COUNT;

	changeMakeCount( __min(maxcount, MAKING_MAX_COUNT) );
}

void cItemProductionShop::changeMakeCount( DWORD nCount )
{
	m_nMakeCount = nCount;

	if(nCount > MAKING_MAX_COUNT)	m_nMakeCount = MAKING_MAX_COUNT;
	
	if( m_pEditBox )
		m_pEditBox->SetText( m_nMakeCount );

	u8 needCost = (u8)m_dwNeedCost * (u8)m_nMakeCount;
	changeProductionCost( needCost );

	_UpdateMaterialItemCount( m_nMakeCount );	
	_StopMakingProgress();
}

DWORD cItemProductionShop::GetAssistItemCode( int const& nAssistType ) const
{
	MAP_AssistItems_CIT it = m_mapAssistControl.find( nAssistType );
	if( it == m_mapAssistControl.end() )
		return 0;
	return it->second.GetAssistItemCode();
}

void cItemProductionShop::_ItemMaking(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pProgressBar );
	SAFE_POINTER_RET( m_pSelectedItemInfo );

	if( m_pProgressBar->IsPlay() )
		_StopMakingProgress();
	else
	{
		DWORD dwPercentageItemCode = GetAssistItemCode(sAssistanceItemGroup::ePercentageAssist);
		DWORD dwProtectItemCode = GetAssistItemCode(sAssistanceItemGroup::eProtectAssist);

		bool bMakealbe = GetSystem()->StartMakeingProgress( m_pSelectedItemInfo->m_nMainIdx, 
															m_pSelectedItemInfo->m_nSubIdx, 
															m_pSelectedItemInfo->m_nUniqueIdx,
															m_nMakeCount,				// 만들 아이템의 갯수
															dwPercentageItemCode,		
															dwProtectItemCode);
		if( bMakealbe )
			_StartMakingProgress();
	}
}

void cItemProductionShop::_SendItemMaking(void* pSender, void* pData)
{
	if( !GetSystem()->SendMakeItemData() )// 서버에 패킷 보내기 실패
		_StopMakingProgress();
}

void cItemProductionShop::_StartMakingProgress()
{
	_UILock(true);
	if( m_pProgressBar )
	{
		m_pProgressBar->SetTimer(0.0f, MAKE_GUAGE_TIME);
		m_pProgressBar->Stop();
	}

	if( m_pProgressBar )
		m_pProgressBar->Start();

	if( m_pMakingBtn )
		m_pMakingBtn->SetText( UISTRING_TEXT("ITEMPRODUCTION_PRODUCTION_CANCEL").c_str() );

	if( m_pEditBox && m_pEditBox->IsFocus() )
		m_pEditBox->ReleaseFocus(false);
}

void cItemProductionShop::_StopMakingProgress()
{
	GetSystem()->StopMakeItemProgress();

	_UILock(false);
	if( m_pProgressBar )
	{
		m_pProgressBar->SetTimer(0.0f, MAKE_GUAGE_TIME);
		m_pProgressBar->Stop();
	}

	if( m_pMakingBtn )
	{
		m_pMakingBtn->StopEffect();
		bool bEnable = m_pSelectedItemInfo != NULL ? true : false;
		m_pMakingBtn->SetText( UISTRING_TEXT("ITEMPRODUCTION_PRODUCTION_START").c_str() );

		m_pMakingBtn->SetEnable(bEnable);
		if(bEnable)		m_pMakingBtn->SetTextColor(NiColor::WHITE);
		else			m_pMakingBtn->SetTextColor(NiColor(0.8f, 0.8f, 0.8f));
	}
}

void cItemProductionShop::_EndMakingProgress()
{
	_UILock(false);
	if( m_pMakingBtn )
	{
		m_pMakingBtn->StopEffect();
		bool bEnable = m_pSelectedItemInfo != NULL ? true : false;
		m_pMakingBtn->SetText( UISTRING_TEXT("ITEMPRODUCTION_PRODUCTION_START").c_str() );

		m_pMakingBtn->SetEnable(bEnable);
		if(bEnable)		m_pMakingBtn->SetTextColor(NiColor::WHITE);
		else			m_pMakingBtn->SetTextColor(NiColor(0.8f, 0.8f, 0.8f));
	}

	if( m_pProgressBar )
	{
		m_pProgressBar->SetTimer(0.0f, 2.0f);
		m_pProgressBar->Stop();
	}
}

void cItemProductionShop::_UILock( bool bLock )
{
	if( m_pProductionItemList )
		m_pProductionItemList->SetEnable(!bLock);
	if( m_pMaterialItemList )
		m_pMaterialItemList->SetEnable(!bLock);
	if( m_pTreeCategoty )
		m_pTreeCategoty->SetEnable(!bLock);
	if( m_pEditBox )
		m_pEditBox->SetEnable(!bLock);
	if( m_pBtnIncrease )
		m_pBtnIncrease->SetEnable(!bLock);
	if( m_pBtnDecrease )
		m_pBtnDecrease->SetEnable(!bLock);
	if( m_pBtnIncreaseEnd )
		m_pBtnIncreaseEnd->SetEnable(!bLock);
	if( m_pBtnDecreaseEnd )
		m_pBtnDecreaseEnd->SetEnable(!bLock);
	if( m_pAssisteItemList )
		m_pAssisteItemList->SetEnable(!bLock);	
}

void cItemProductionShop::ChangeInputMakeCount(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );

	TCHAR* pInputData = static_cast<TCHAR*>(pData);
	m_nMakeCount = _ttoi( pInputData );

	changeMakeCount(m_nMakeCount);
}

// 재작에 필요한 아이템의 갯수가 변경瑛?때 UI 갱신 하는 함수
void cItemProductionShop::ChangeHaveItemCount( ContentsStream const& kStream )
{
	DWORD	dwItemIdx = 0;
	DWORD	dwItemCount = 0;
	kStream >> dwItemIdx >> dwItemCount;
	//
	MAP_MaterialItems_IT it = m_mapMaterialCountText.find( dwItemIdx );
	if( it != m_mapMaterialCountText.end() )
		it->second.UpdateHaveItemCount( dwItemCount );

	MAP_AssistItems_IT subIT = m_mapAssistControl.begin();
	for( ; subIT != m_mapAssistControl.end(); ++subIT )
	{
		if( subIT->second.UpdateItemcount( dwItemIdx, dwItemCount ) )
			_UpdateUIAssistItemCount( subIT->first );
	}
};

// 아이템 제작 갯수가 변경 瑛?때 재료 아이템의 갯수 갱신 하는 함수
void cItemProductionShop::_UpdateMaterialItemCount(DWORD const& dwMakeItemcount)
{
	MAP_MaterialItems_IT it = m_mapMaterialCountText.begin();
	for( ; it != m_mapMaterialCountText.end(); ++it )
		it->second.UpdateMakeItemCount( dwMakeItemcount );
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool cItemProductionShop::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eOpen_Window, this );
	GetSystem()->Register( SystemType::eUpdate_ItemCount, this );	
	GetSystem()->Register( SystemType::eUpdate_Money, this );
	GetSystem()->Register( SystemType::eSetAssistItem, this );
	GetSystem()->Register( SystemType::eRecvMakeItem, this );
	
	return true;
}

void cItemProductionShop::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eOpen_Window:		MakeCategoty();		break;
	case SystemType::eUpdate_ItemCount:	ChangeHaveItemCount( kStream ); break;
	case SystemType::eUpdate_Money:		changeProductionCost( (u8)m_dwNeedCost * (u8)m_nMakeCount );	break;
	case SystemType::eRecvMakeItem:		_EndMakingProgress();	break;
	case SystemType::eSetAssistItem:	
		{
			int nAssisType = 0;			int nAssisItemCode = 0;			DWORD dwHaveCount = 0;
			kStream >> nAssisType >> nAssisItemCode >> dwHaveCount;
			_SetAssitItem( nAssisType, nAssisItemCode, dwHaveCount );
		}break;
	}
}