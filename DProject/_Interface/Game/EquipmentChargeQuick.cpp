#include "stdafx.h"
#include "EquipmentChargeQuick.h"

#define IF_CHARGE_TIME_1	1
#define IF_CHARGE_TIME_2	7
#define IF_CHARGE_TIME_3	15
#define IF_CHARGE_TIME_4	30

cEquipmentChargeQuick::cEquipmentChargeQuick() :
m_pChargeItem( NULL ),
m_pQuickItem( NULL ),
m_pQuickItemSelect( NULL ),
m_pQuickItemButton( NULL ),
m_pHelpButton( NULL )
{
	Construct();
}

cEquipmentChargeQuick::~cEquipmentChargeQuick()
{
	Destroy();
}

bool cEquipmentChargeQuick::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eRegistChargeItem_Quick, this );
	GetSystem()->Register( SystemType::eRegistQuickCharge,	this );
	GetSystem()->Register( SystemType::eQuickChargeItem,	this );

	return true;
}

void cEquipmentChargeQuick::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eRegistChargeItem_Quick:
		{
			uint nItemType = 0;
			std::wstring wsItemName;
			kStream >> nItemType >> wsItemName;
			_RegistChargeItem( nItemType, wsItemName );
		}
		break;

	case SystemType::eRegistQuickCharge:
		{
			uint nItemType = 0;
			DWORD dwItemCount = 0;
			std::wstring wsItemName;
			kStream >> nItemType >> dwItemCount >> wsItemName;
			_RegistQuickChargeItem( nItemType, dwItemCount, wsItemName );
		}
		break;

	case SystemType::eQuickChargeItem:
		{
			_ResetQuickCharge();
		}
		break;
	}
}

void cEquipmentChargeQuick::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	DeleteResource();
}

void cEquipmentChargeQuick::DeleteResource()
{
	DeleteScript();
}

void cEquipmentChargeQuick::Create(cWindow* pkRoot, int nValue /*= 0 */)
{
	InitScript( pkRoot, NULL, CsPoint( 10, 100 ), CsPoint( 600, 500 ), false );

	m_pHelpButton = AddButton( CsPoint( 570, 25 ), CsPoint( 24, 32 ), CsPoint( 0, 32 ), "DigimonStatus_New\\Digimon_help_btn.tga" );
	if( m_pHelpButton )
		m_pHelpButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cEquipmentChargeQuick::_OnClickHelpButton );

	{	// 즉시 충전 버튼
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_QUICK_CHARGE_BTN_QUICK" ).c_str() );
		m_pQuickItemButton = AddButton( CsPoint( 220, 380 ), CsPoint( 160, 35 ), CsPoint( 0, 37 ), "System\\Btn_Nor13.tga" );
		if( m_pQuickItemButton )
		{
			m_pQuickItemButton->SetText( &ti );
			m_pQuickItemButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cEquipmentChargeQuick::_OnClickQuickChargeButton );
		}
	}

	_MakeChargeItem();
	_MakeQuickChargeItem();
	_MakeComboBox();
}

void cEquipmentChargeQuick::Update(float const& fDeltaTime)
{

}

BOOL cEquipmentChargeQuick::Update_ForMouse()
{
	if( m_pChargeItem && m_pChargeItem->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		_SetChargeItemTooltip();
		return TRUE;
	}

	if( m_pQuickItem && m_pQuickItem->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		_SetQuickItemTooltip();
		return TRUE;
	}

	if( m_pQuickItemSelect && m_pQuickItemSelect->Update_ForMouse() != cComboBox::CI_INVALIDE )
		return TRUE;

	if( m_pQuickItemButton )
	{
		switch( m_pQuickItemButton->Update_ForMouse() )
		{
		case cButton::ACTION_ON:
			_SetExplainChargeButton();
			break;
		case cButton::ACTION_CLICK:
			return TRUE;
		}
	}

	if( m_pHelpButton && m_pHelpButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	return FALSE;
}

void cEquipmentChargeQuick::RenderScript()
{
	cScriptUI::RenderScript();
}

void cEquipmentChargeQuick::ResetDevice()
{
	ResetDeviceScript();
}

void cEquipmentChargeQuick::OnLButtonUp(CsPoint pos)
{

}

bool cEquipmentChargeQuick::CursorIconLBtnUp(CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount)
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			GetSystem()->RegisterItem_QuickCharge( nIconSlot );
			CURSOR_ST.ReleaseIcon();
			return true;
		}
		break;
	default:
		cPrintMsg::PrintMsg( 11018 );
	}
	return false;
}

void cEquipmentChargeQuick::UpdateViewer()
{
	_ResetQuickCharge();
}

void cEquipmentChargeQuick::_MakeChargeItem()
{
	m_pChargeItem = NiNew cListBox;
	SAFE_POINTER_RET( m_pChargeItem );
	m_pChargeItem->Init( m_pRoot, CsPoint( 150, 80 ), CsPoint( 300, 100 ), NULL, false );
	//m_pChargeItem->SetBackGorundImg( "testimg.bmp" );
	m_pChargeItem->AddEvent( cListBox::LIST_ITEM_RIGHT_UP, this, &cEquipmentChargeQuick::_OnRClickChargeItem );
	AddChildControl( m_pChargeItem );

	m_pChargeItem->RemoveAllItem();

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	// 등록 아이템 이름
	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_12, NiColor( 84.f/255.f, 189.f/225.f, 1 ) );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( L"" );
	m_sChargeItem.pItemName = pControl->AddText( &ti, CsPoint( 150, 0 ) );
	if( m_sChargeItem.pItemName )
		m_sChargeItem.pItemName->SetAutoPointerDelete( true );

	// 등록 아이템 슬롯
	cSprite* pSlot = NiNew cSprite;
	if( pSlot )
	{
		pSlot->Init( NULL, CsPoint::ZERO, CsPoint( 188, 88 ), "EquipSystem\\Recharge_slot.tga", false );
		cString::sSPRITE* pSlotImg = pControl->AddSprite( pSlot, CsPoint( 56, 7 ) );
		if( pSlotImg )
			pSlotImg->SetAutoPointerDelete( true );
	}

	// 등록 아이템 아이콘
	m_sChargeItem.pItemIcon = pControl->AddIcon( CsPoint( 42, 42 ), ICONITEM::ITEM_ICON, 0, 1, CsPoint( 129, 30 ) );
	if( m_sChargeItem.pItemIcon )
		m_sChargeItem.pItemIcon->SetAutoPointerDelete( true );

	m_sChargeItem.pItemControl = NiNew cListBoxItem;
	if( m_sChargeItem.pItemControl )
	{
		m_sChargeItem.pItemControl->SetItem( pControl );
		m_sChargeItem.pItemControl->SetUserData( new sItemInfo() );
		m_pChargeItem->AddItem( m_sChargeItem.pItemControl );
	}
}

void cEquipmentChargeQuick::_MakeQuickChargeItem()
{
	m_pQuickItem = NiNew cListBox;
	SAFE_POINTER_RET( m_pQuickItem );
	m_pQuickItem->Init( m_pRoot, CsPoint( 150, 180 ), CsPoint( 300, 300 ), NULL, false );
	//m_pQuickItem->SetBackGorundImg( "testimg2.bmp" );
	m_pQuickItem->AddEvent( cListBox::LIST_ITEM_RIGHT_UP, this, &cEquipmentChargeQuick::_OnRClickQuickItem );
	AddChildControl( m_pQuickItem );

	m_pQuickItem->RemoveAllItem();

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	// 등록 아이템 이름
	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_12, NiColor( 84.f/255.f, 189.f/225.f, 1 ) );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( L"" );
	m_sQuickItem.pItemName = pControl->AddText( &ti, CsPoint( 150, 10 ) );
	if( m_sQuickItem.pItemName )
		m_sQuickItem.pItemName->SetAutoPointerDelete( true );

	// 등록 아이템 슬롯
	cSprite* pSlot = NiNew cSprite;
	if( pSlot )
	{
		pSlot->Init( NULL, CsPoint::ZERO, CsPoint( 188, 88 ), "EquipSystem\\Recharge_slot.tga", false );
		cString::sSPRITE* pSlotImg = pControl->AddSprite( pSlot, CsPoint( 56, 17 ) );
		if( pSlotImg )
			pSlotImg->SetAutoPointerDelete( true );
	}

	// 등록 아이템 아이콘
	m_sQuickItem.pItemIcon = pControl->AddIcon( CsPoint( 42, 42 ), ICONITEM::ITEM_ICON, 0, 1, CsPoint( 129, 40 ) );
	if( m_sQuickItem.pItemIcon )
		m_sQuickItem.pItemIcon->SetAutoPointerDelete( true );

	m_sQuickItem.pItemControl = NiNew cListBoxItem;
	if( m_sQuickItem.pItemControl )
	{
		m_sQuickItem.pItemControl->SetItem( pControl );
		m_sQuickItem.pItemControl->SetUserData( new sItemInfo() );
		m_pQuickItem->AddItem( m_sQuickItem.pItemControl );
	}
}

void cEquipmentChargeQuick::_MakeComboBox()
{
	// 충전 일별 개수 콤보 박스
	m_pQuickItemSelect = NiNew cComboBox;
	if( m_pQuickItemSelect )
	{
		m_pQuickItemSelect->Init( m_pRoot, cComboBox::CT_NORMAL, CsPoint( 200, 300 ), CsPoint( 200, 22 ),
			CFont::FS_12, DT_CENTER, false, false, false );
		m_pQuickItemSelect->ChangeComboBoxImg( "SealMaster\\btn_combobox.tga", CsPoint( 0, 22 ) );
		m_pQuickItemSelect->SetComboListWindow( "SealMaster\\dropdown_list_0" );
		m_pQuickItemSelect->AddEvent( cComboBox::COMBO_SELECTITEM, this, &cEquipmentChargeQuick::_OnClickComboBox );

		m_pQuickItemSelect->RemoveAllItem();

		_AddComboBoxItem( 0, 0, NiColor::WHITE );

		m_pQuickItemSelect->SetCurSel( 0 );

		AddChildControl( m_pQuickItemSelect );
	}
}

void cEquipmentChargeQuick::_AddComboBoxItem(int nDay, int nCount, NiColor niColor)
{
	SAFE_POINTER_RET( m_pQuickItemSelect );

	cString* pComboItem = NiNew cString;
	SAFE_POINTER_RET( pComboItem );

	if( 0 == nDay && 0 == nCount )
	{
		std::wstring wsMsg = UISTRING_TEXT( "EQUIPSYSTEM_QUICK_CHARGE_SELECT_COMBO" );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, niColor );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( wsMsg.c_str() );
		pComboItem->AddText( &ti );
		m_pQuickItemSelect->AddItem( pComboItem, nCount, niColor );
		return;
	}

	{	// n 일
		std::wstring wsDay = UISTRING_TEXT( "EQUIPSYSTEM_QUICK_CHARGE_DAY" );
		DmCS::StringFn::Replace( wsDay, L"#Day#", nDay );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( wsDay.c_str() );
		pComboItem->AddText( &ti );
	}

	pComboItem->TailAddSizeX( 80 - pComboItem->GetMaxSize().x/*공백*/ );

	{	// n 개
		std::wstring wsCount = UISTRING_TEXT( "EQUIPSYSTEM_QUICK_CHARGE_COUNT" );
		DmCS::StringFn::Replace( wsCount, L"#Count#", nCount );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, niColor );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( wsCount.c_str() );
		pComboItem->AddText( &ti );
	}

	m_pQuickItemSelect->AddItem( pComboItem, nCount, niColor );
}

void cEquipmentChargeQuick::_ResetQuickCharge()
{
	m_sChargeItem.Reset();
	m_sQuickItem.Reset();
	if( m_pQuickItemSelect )
		m_pQuickItemSelect->RemoveAllItem();
}

void cEquipmentChargeQuick::_RegistChargeItem(uint const& nItemType, std::wstring const& wsItemName)
{
	m_sChargeItem.ChangeItem( nItemType, wsItemName );
	m_sQuickItem.Reset();
}

void cEquipmentChargeQuick::_RegistQuickChargeItem(uint const& nItemType, DWORD const& dwItemCount, std::wstring const& wsItemName)
{
 	// 즉시 충전 아이템 등록
 	m_sQuickItem.ChangeItem( nItemType, wsItemName, dwItemCount );

	// 콤보 박스 세팅 : 충전 시간별 아이템 필요 개수
	SAFE_POINTER_RET( m_pQuickItemSelect );
	m_pQuickItemSelect->RemoveAllItem();

	_AddComboBoxItem( 0, 0, NiColor::WHITE );

	int nSelectCount = GetSystem()->GetChargeTimeCount();

	for( int i = 1; i <= nSelectCount; ++i )
	{
		int nCount = GetSystem()->GetQuickChargeCountByIndex( i );
		int nDay = _GetDayByQuickChargeIndex( i );
		NiColor niColor = NiColor::WHITE;
		if( dwItemCount >= nCount )
			niColor = NiColor( 0, 1, 0 );
		_AddComboBoxItem( nDay, nCount, niColor );
	}
	m_pQuickItemSelect->SetCurSel( 0 );
}

void cEquipmentChargeQuick::_SetChargeItemTooltip()
{
	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	cString::sICON* pItemIcon = m_sChargeItem.pItemIcon;
	SAFE_POINTER_RET( pItemIcon );

	// item icon
	if( pItemIcon->m_MouseOverCheckRect.PtInRect( CURSOR_ST.GetPos() ) )
	{
		SAFE_POINTER_RET( m_sChargeItem.pItemControl );
		sItemInfo* pUserData = dynamic_cast< sItemInfo* >( m_sChargeItem.pItemControl->GetUserData() );
		SAFE_POINTER_RET( pUserData );
		if( 0 == pUserData->nItemType )
		{
#ifdef SIMPLE_TOOLTIP
			pTooltip->SetTooltip( pItemIcon->GetWorldPos(), CsPoint( 42, 42 ), 100, cTooltip::UIHELP, 1071 );
#endif
			return;
		}

		cItemInfo cCItem;
		cCItem.m_nType = pUserData->nItemType;
		pTooltip->SetTooltip( pItemIcon->GetWorldPos(), CsPoint( 42, 42 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, 
			pUserData->nItemType, cBaseWindow::WT_EQUIPMENT_SYSTEM, cTooltip::ITEM, 0, &cCItem );
	}
}

void cEquipmentChargeQuick::_SetQuickItemTooltip()
{
	cString::sICON* pItemIcon = m_sQuickItem.pItemIcon;
	SAFE_POINTER_RET( pItemIcon );

	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	// item icon
	if( pItemIcon->m_MouseOverCheckRect.PtInRect( CURSOR_ST.GetPos() ) )
	{
		SAFE_POINTER_RET( m_sQuickItem.pItemControl );
		sItemInfo* pUserData = dynamic_cast< sItemInfo* >( m_sQuickItem.pItemControl->GetUserData() );
		SAFE_POINTER_RET( pUserData );
		if( 0 == pUserData->nItemType )
		{
#ifdef SIMPLE_TOOLTIP
			pTooltip->SetTooltip( pItemIcon->GetWorldPos(), CsPoint( 42, 42 ), 100, cTooltip::UIHELP, 1074 );
#endif
			return;
		}

		cItemInfo cQItem;
		cQItem.Set( pUserData->nItemType, pUserData->nItemCount );
		pTooltip->SetTooltip( pItemIcon->GetWorldPos(), CsPoint( 42, 42 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, 
			pUserData->nItemType, cBaseWindow::WT_EQUIPMENT_SYSTEM, cTooltip::ITEM, 0, &cQItem );
	}
}

void cEquipmentChargeQuick::_SetExplainChargeButton()
{
#ifdef SIMPLE_TOOLTIP
	SAFE_POINTER_RET( m_pQuickItemButton );
	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );
	pTooltip->SetTooltip( m_pQuickItemButton->GetPos(), m_pQuickItemButton->GetSize(), 100, cTooltip::UIHELP, 1075 );
#endif
}

int cEquipmentChargeQuick::_GetDayByQuickChargeIndex(int const& nIndex)
{
	switch( nIndex )
	{
	case 1:
		return IF_CHARGE_TIME_1;
	case 2:
		return IF_CHARGE_TIME_2;
	case 3:
		return IF_CHARGE_TIME_3;
	case 4:
		return IF_CHARGE_TIME_4;
	}

	return 0;
}

void cEquipmentChargeQuick::_OnClickQuickChargeButton(void* pSender, void* pData)
{
	GetSystem()->QuickCharge();
}

void cEquipmentChargeQuick::_OnClickComboBox(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	int nSelectIndex = *static_cast< int* >( pData );

	GetSystem()->SelectQuickChargeItemCount( nSelectIndex );
}

void cEquipmentChargeQuick::_OnRClickChargeItem(void* pSender, void* pData)
{
	if( !GetSystem()->ResetChargeItem() )
		return;

	m_sChargeItem.Reset();
	m_sQuickItem.Reset();
	if( m_pQuickItemSelect )
	{
		m_pQuickItemSelect->RemoveAllItem();
		_AddComboBoxItem( 0, 0, NiColor::WHITE );
	}
}

void cEquipmentChargeQuick::_OnRClickQuickItem(void* pSender, void* pData)
{
	if( !GetSystem()->ResetQuickItem() )
		return;

	m_sQuickItem.Reset();
	if( m_pQuickItemSelect )
	{
		m_pQuickItemSelect->RemoveAllItem();
		_AddComboBoxItem( 0, 0, NiColor::WHITE );
	}
}

void cEquipmentChargeQuick::_OnClickHelpButton(void* pSender, void* pData)
{
	GetSystem()->HelpWindow();
}

//////////////////////////////////////////////////////////////////////////
void cEquipmentChargeQuick::sRegistItemInfo::ChangeItem(uint nItemType, std::wstring wsItemName, uint nItemCount /*= 1*/)
{
	if( pItemIcon )
		pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemType, nItemCount );

	if( pItemName )
		pItemName->SetText( wsItemName.c_str() );

	if( pItemControl )
	{
		sItemInfo* pUserData = dynamic_cast< sItemInfo* >( pItemControl->GetUserData() );
		if( pUserData )
		{
			pUserData->nItemType = nItemType;
			pUserData->nItemCount = nItemCount;
		}
	}
}

void cEquipmentChargeQuick::sRegistItemInfo::Reset()
{
	ChangeItem( 0, L"", 1 );
}
