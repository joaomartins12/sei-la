#include "stdafx.h"
#include "NewGotchaMachine.h"

#define IF_TEXT_WIDTH 130

cNewGotchaMachine::cNewGotchaMachine() :
m_pRareItems(NULL),
m_pUseItems(NULL),
m_pCloseButton(NULL),
m_pGotchaButton(NULL),
m_pRemainItemCount(NULL),
m_pUseMoney(NULL)
{

}

cNewGotchaMachine::~cNewGotchaMachine()
{

}

bool cNewGotchaMachine::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eRefresh, this );
	GetSystem()->Register( SystemType::eSetEnable, this );

	return true;
}

void cNewGotchaMachine::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eRefresh:
		_UpdateGotchaMachine();
		break;
	case SystemType::eSetEnable:
		{
			bool bEnable = false;
			kStream >> bEnable;
			SetEnableWindow( bEnable );
			m_pGotchaButton->SetEnable( bEnable );
		}
		break;
	}
}

cBaseWindow::eWINDOW_TYPE cNewGotchaMachine::GetWindowType()
{
	return WT_NEW_GOTCHA_MACHINE;
}

void cNewGotchaMachine::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cNewGotchaMachine::DeleteResource()
{
	DeleteScript();
}

void cNewGotchaMachine::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	SystemType::eMachineType eMachine = GetSystem()->GetMachineType();
	switch( eMachine )
	{
	case SystemType::eMachineType::GOTCHA:
	case SystemType::eMachineType::MYSTERY_NORMAL:
		_InitGotchaMachine();
		break;
	case SystemType::eMachineType::MYSTERY_EGG3:
	case SystemType::eMachineType::MYSTERY_EGG4:
	case SystemType::eMachineType::MYSTERY_EGG5:
		_InitMysteryEggMachine();
		break;
	}

	//닫기 버튼
	m_pCloseButton = AddButton( CsPoint( 285, 6 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	if( m_pCloseButton )
		m_pCloseButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cNewGotchaMachine::_OnClickCloseButton );
	//뽑기 버튼
	m_pGotchaButton = AddButton( CsPoint( 161, 437 ), CsPoint( 110, 47 ), CsPoint( 0, 47 ), "GotchaMachine\\gotcha_startbutton.tga" );
	if( m_pGotchaButton )
		m_pGotchaButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cNewGotchaMachine::_OnClickGotchaButton );
}

void cNewGotchaMachine::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cNewGotchaMachine::Update_ForMouse()
{
	eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( MUT_OUT_WINDOW == muReturn )
		return MUT_OUT_WINDOW;

	if( m_pCloseButton && m_pCloseButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( m_pGotchaButton && m_pGotchaButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( m_pRareItems && m_pRareItems->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		if( _SetRareItemTooltip( m_pRareItems->GetMouseOverItem() ) )
			return muReturn;
	}

	if( m_pUseItems && m_pUseItems->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		if( _SetUseItemTooltip( m_pUseItems->GetMouseOverItem() ) )
			return muReturn;
	}

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 290, 75 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cNewGotchaMachine::Render()
{
	RenderScript();
}

void cNewGotchaMachine::OnMouseOutWindow()
{

}

void cNewGotchaMachine::OnLButtonUp(CsPoint pos)
{

}

void cNewGotchaMachine::ResetDevice()
{
	ResetDeviceScript();
}

bool cNewGotchaMachine::OnEscapeKey()
{
	GetSystem()->CloseMachineUI();
	return true;
}

void cNewGotchaMachine::_InitGotchaMachine()
{
	InitScript( "GotchaMachine\\gotcha_back.tga", CsPoint( 30, 190 ), CsPoint( 312, 500 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( GetSystem()->GetMachineName().c_str(), CsPoint( 0, 37 ), CFont::FS_16 );

	_MakeRareItemList();
	_MakeUseItemList();

	{	// 남은 아이템 개수
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		m_pRemainItemCount = AddText( &ti, CsPoint( 200, 402 ) );

		SAFE_POINTER_RET( m_pRemainItemCount );
		SystemType::eShowFlag eFlag = GetSystem()->GetShowFlag();
		if( SystemType::eShowFlag::allhide == eFlag ||
			SystemType::eShowFlag::normalhide == eFlag ||
			SystemType::eShowFlag::mystery == eFlag )
			m_pRemainItemCount->SetText( L"?" );
		else
			m_pRemainItemCount->SetText( GetSystem()->GetRemainItemCount() );
	}
}

void cNewGotchaMachine::_InitMysteryEggMachine()
{
	InitScript( "GotchaMachine\\Mistery_back.tga", CsPoint( 30, 190 ), CsPoint( 312, 500 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( GetSystem()->GetMachineName().c_str(), CsPoint( 0, 37 ), CFont::FS_16 );

	_MakeRareItemList();
	_MakeUseItemList();

	{	// 필요 금액
		m_pUseMoney = NiNew cMoneyIF;
		SAFE_POINTER_RET( m_pUseMoney );
		m_pUseMoney->Init( cMoneyIF::DISP_SIMPLE_1, GetSystem()->GetEnableUseMoney(), false );
		m_pUseMoney->SetPos( CsPoint( 105, 400 ) );
		AddChildControl( m_pUseMoney );
	}
}

void cNewGotchaMachine::_MakeRareItemList()
{
	m_pRareItems = NiNew cListBox;
	SAFE_POINTER_RET( m_pRareItems );

	m_pRareItems->Init( GetRoot(), CsPoint( 22, 107 ), CsPoint( 273, 280 ), false );
	cScrollBar* pScroll = NiNew cScrollBar;
	if( pScroll )
	{
		pScroll->Init( cScrollBar::TYPE_GOTCHA, NULL, CsPoint::ZERO, CsPoint( 22, 280 ), CsPoint( 22, 20 ),
			CsRect( CsPoint::ZERO, CsSIZE( 268, 280 ) ), 6, false );
		m_pRareItems->SetScrollBar( pScroll );
	}
	m_pRareItems->SetMargin( CsPoint( 11, 10 ) );
	m_pRareItems->SetItemGab( CsPoint( 0, 5 ) );
	AddChildControl( m_pRareItems );

	m_pRareItems->RemoveAllItem();
	int nRareItemCount = GetSystem()->GetRareItemCountAll();
	SystemType::eShowFlag eFlag = GetSystem()->GetShowFlag();
	for( int i = 0; i < nRareItemCount; ++i )
	{
		cItemInfo* pRare = GetSystem()->GetRareItem( i );
		if( pRare && pRare->IsEnable() )
			_AddRareItem( i, pRare->GetType(), pRare->GetCount(), eFlag );
	}
}

void cNewGotchaMachine::_AddRareItem(int nIndex, uint nItemType, uint nItemCount, SystemType::eShowFlag eFlag)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	cSprite* pItemSlot = NiNew cSprite;
	if( pItemSlot )
	{
		pItemSlot->Init( NULL, CsPoint::ZERO, CsPoint( 39, 39 ), "GotchaMachine\\gotcha_number.tga", false );
		cString::sSPRITE* pSprite = pControl->AddSprite( pItemSlot, CsPoint( -2, 0 ) );
		if( pSprite )
			pSprite->SetAutoPointerDelete( true );
	}

	cSprite* pNameSlot = NiNew cSprite;
	if( pNameSlot )
	{
		pNameSlot->Init( NULL, CsPoint::ZERO, CsPoint( 147, 39 ), "GotchaMachine\\gotcha_itemname.tga", false );
		cString::sSPRITE* pSprite = pControl->AddSprite( pNameSlot, CsPoint( 39, 0 ) );
		if( pSprite )
			pSprite->SetAutoPointerDelete( true );
	}

	cSprite* pCountSlot = NiNew cSprite;
	if( pCountSlot )
	{
		pCountSlot->Init( NULL, CsPoint::ZERO, CsPoint( 39, 39 ), "GotchaMachine\\gotcha_number.tga", false );
		cString::sSPRITE* pSprite = pControl->AddSprite( pCountSlot, CsPoint( 188, 0 ) );
		if( pSprite )
			pSprite->SetAutoPointerDelete( true );
	}

	sGotchaItem sItem;
	sItem.pItemIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, nItemType, 1, CsPoint( 2, 4 ) );
	if( sItem.pItemIcon )
		sItem.pItemIcon->SetAutoPointerDelete( true );

	{	// 아이템 이름
		cText::sTEXTINFO ti;
		std::wstring wsItemName;
		CFont::eFACE_SIZE fontSize = CFont::FS_10;
		CsPoint pos( 47, 7 );
		if( SystemType::mystery == eFlag )
		{
			wsItemName = GetSystem()->GetMysteryItemName();
			fontSize = CFont::FS_12;
			pos.y += 5;
		}
		else
		{
			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemType );
			if( pFTItem )
				wsItemName = pFTItem->GetName();
		}
		ti.Init( fontSize );
		cString::sTEXT* pText = pControl->AddText( &ti, pos );
		if( pText )
		{
			pText->SetText( wsItemName.c_str(), IF_TEXT_WIDTH );
			pText->SetAutoPointerDelete( true );
		}
	}

	{	// 아이템 개수
		cText::sTEXTINFO ti;
		ti.Init();
		ti.s_eTextAlign = DT_CENTER;
		switch( eFlag )
		{
		case SystemType::allhide:
		case SystemType::rarehide:
		case SystemType::mystery:
			ti.SetText( L"?" );
			break;
		default:
			ti.SetText( nItemCount );
			break;
		}
		sItem.pItemCount = pControl->AddText( &ti, CsPoint( 207, 12 ) );
		if( sItem.pItemCount )
			sItem.pItemCount->SetAutoPointerDelete( true );
	}

	cListBoxItem* pListItem = NiNew cListBoxItem( nIndex );
	if( pListItem )
	{
		pListItem->SetUserData( new sItemInfo( nItemType, nItemCount ) );
		pListItem->SetItem( pControl );
		m_pRareItems->AddItem( pListItem );
		m_mapRareItems.insert( std::make_pair( nIndex, sItem ) );
	}
}

void cNewGotchaMachine::_MakeUseItemList()
{
	m_pUseItems = NiNew cGridListBox;
	SAFE_POINTER_RET( m_pUseItems );

	m_pUseItems->Init( GetRoot(), CsPoint( 24, 440 ), CsPoint( 130, 40 ), CsPoint( 15, 0 ), CsPoint( 32, 32 ),
		cGridListBox::LowLeftDown, cGridListBox::RightTop, false, 0 );
	AddChildControl( m_pUseItems );

	m_pUseItems->RemoveAllItem();
	int nUseCount = GetSystem()->GetUseItemCountAll();
	for( int i = 0; i < nUseCount; ++i )
		_AddUseItem( i );
}

void cNewGotchaMachine::_AddUseItem(int nIndex)
{
	SystemType::sUseItemInfo* useItem = GetSystem()->GetUseItem( nIndex );
	SAFE_POINTER_RET( useItem );
	if( 0 == useItem->nUseItemType )
		return;

	int nInvenCount = GetSystem()->GetUseInvenCount( useItem->nUseItemType );

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	sUseItem sItem;
	cString::sICON* pItemIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, useItem->nUseItemType, 1 );
	if( pItemIcon )
		pItemIcon->SetAutoPointerDelete( true );

	{	// 필요 아이템 개수 inven / use
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		std::wstring wsCount;
		DmCS::StringFn::Format( wsCount, L"%d/%d", nInvenCount, useItem->nUseItemCount );
		ti.SetText( wsCount.c_str() );
		sItem.pItemCount = pControl->AddText( &ti, CsPoint( 16, 37 ) );
		if( sItem.pItemCount )
			sItem.pItemCount->SetAutoPointerDelete( true );
	}

	sItem.pItemControl = NiNew cGridListBoxItem( nIndex, CsPoint( 32, 32 ) );
	if( sItem.pItemControl )
	{
		sItem.pItemControl->SetUserData( new sItemInfo( useItem->nUseItemType, useItem->nUseItemCount, nInvenCount ) );
		sItem.pItemControl->SetItem( pControl );
		m_pUseItems->AddItem( sItem.pItemControl );
		m_mapUseItems.insert( std::make_pair( nIndex, sItem ) );
	}
}

void cNewGotchaMachine::_UpdateGotchaMachine()
{
	// 레어 아이템 목록 갱신
	SystemType::eShowFlag eFlag = GetSystem()->GetShowFlag();
	std::map< int, sGotchaItem >::iterator it = m_mapRareItems.begin();
	std::map< int, sGotchaItem >::iterator itEnd = m_mapRareItems.end();
	for( ; it != itEnd; ++it )
	{
		cItemInfo* pRare = GetSystem()->GetRareItem( it->first );
		if( pRare && pRare->IsEnable() )
		{
			int nCount = pRare->GetCount();
			if( SystemType::eShowFlag::allhide == eFlag ||
				SystemType::eShowFlag::rarehide == eFlag ||
				SystemType::eShowFlag::mystery == eFlag )
				nCount = -1;
			it->second.ChangeItemCount( nCount );
		}
	}

	// 인벤에 있는 필요 아이템 개수 갱신
	std::map< int, sUseItem >::iterator itUse = m_mapUseItems.begin();
	std::map< int, sUseItem >::iterator itUseEnd = m_mapUseItems.end();
	for( ; itUse != itUseEnd; ++itUse )
	{
		int nCount = GetSystem()->GetUseInvenCount( itUse->second.GetItemType() );
		itUse->second.ChangeUseItemCount( nCount );
	}

	// 남은 수량 갱신
	if( m_pRemainItemCount )
	{
		if( SystemType::eShowFlag::allhide == eFlag ||
			SystemType::eShowFlag::normalhide == eFlag ||
			SystemType::eShowFlag::mystery == eFlag )
			m_pRemainItemCount->SetText( L"?" );
		else
			m_pRemainItemCount->SetText( GetSystem()->GetRemainItemCount() );
	}

// 	// 필요 금액 갱신
// 	if( m_pUseMoney )
// 		m_pUseMoney->SetMoney( GetSystem()->GetEnableUseMoney() );
}

bool cNewGotchaMachine::_SetRareItemTooltip(cListBoxItem const* pOverItem)
{
	SAFE_POINTER_RETVAL( pOverItem, false );
	std::map< int, sGotchaItem >::iterator it = m_mapRareItems.find( pOverItem->getID() );
	if( it == m_mapRareItems.end() )
		return false;

	sItemInfo* pInfo = dynamic_cast<sItemInfo*>(pOverItem->GetUserData());
	SAFE_POINTER_RETVAL( pInfo, false );

	cString::sICON* pItemIcon = it->second.pItemIcon;
	SAFE_POINTER_RETVAL( pItemIcon, false );

	if( pItemIcon->m_MouseOverCheckRect.PtInRect( CURSOR_ST.GetPos() ) )
	{
		SystemType::eMachineType eMachine = GetSystem()->GetMachineType();
		SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, false );
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL( pTooltip, false );
		if( SystemType::eMachineType::GOTCHA == eMachine )
			pTooltip->SetTooltip( pItemIcon->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->nItemType, cBaseWindow::WT_NEW_GOTCHA_MACHINE, 0, 0, NULL );
		else
			pTooltip->SetTooltip_Text( pItemIcon->GetWorldPos(), CsPoint( 32, 32 ), UISTRING_TEXT( "GOTCHAMACHINE_TOOLTIP_RANDOM" ).c_str() );
		return true;
	}
	return false;
}

bool cNewGotchaMachine::_SetUseItemTooltip(cGridListBoxItem const* pOverItem)
{
	SAFE_POINTER_RETVAL( pOverItem, false );
	sItemInfo* pUseInfo = static_cast< sItemInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pUseInfo, false );

	SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, false );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RETVAL( pTooltip, false );
	pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pUseInfo->nItemType, cBaseWindow::WT_NEW_GOTCHA_MACHINE, 0, 0, NULL );
	return true;
}

void cNewGotchaMachine::_OnClickCloseButton(void* pSender, void* pData)
{
	GetSystem()->CloseMachineUI();
}

void cNewGotchaMachine::_OnClickGotchaButton(void* pSender, void* pData)
{
	GetSystem()->StartGotcha();
}

////////////////////////////////////////////////////////////////////////////////////////
void cNewGotchaMachine::sGotchaItem::ChangeItemCount(int nCount)
{
	SAFE_POINTER_RET( pItemCount );
	std::wstring wsCount;
	if( -1 == nCount )
		wsCount = L"?";
	else
		DmCS::StringFn::From( wsCount, nCount );

	pItemCount->SetText( wsCount.c_str() );
}

uint cNewGotchaMachine::sUseItem::GetItemType()
{
	SAFE_POINTER_RETVAL( pItemControl, 0 );
	sItemInfo* pInfo = static_cast< sItemInfo* >( pItemControl->GetUserData() );
	SAFE_POINTER_RETVAL( pInfo, 0 );
	return pInfo->nItemType;
}

void cNewGotchaMachine::sUseItem::ChangeUseItemCount(int nCount)
{
	SAFE_POINTER_RET( pItemControl );
	sItemInfo* pInfo = static_cast< sItemInfo* >( pItemControl->GetUserData() );
	SAFE_POINTER_RET( pInfo );
	pInfo->nInvenItemCount = nCount;

	if( pItemCount )
	{
		std::wstring wsCount;
		DmCS::StringFn::Format( wsCount, L"%d/%d", pInfo->nInvenItemCount, pInfo->nItemCount );
		pItemCount->SetText( wsCount.c_str() );
	}
}