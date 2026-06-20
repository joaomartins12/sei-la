#include "stdafx.h"
#include "EquipmentCharge.h"

#define IF_CHARGE_LIST_COUNT	7


cEquipmentCharge::cEquipmentCharge() :
m_pHelpButton(NULL),
m_pChargeList(NULL)
{
	Construct();
}

cEquipmentCharge::~cEquipmentCharge()
{
	Destroy();
}

bool cEquipmentCharge::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eRegistChargeItem,	this );
	GetSystem()->Register( SystemType::eUpdateChargeList,	this );
	GetSystem()->Register( SystemType::eUpdateChargeTime,	this );
	GetSystem()->Register( SystemType::eNormalChargeItem,	this );
	GetSystem()->Register( SystemType::eWithdrawItem,		this );
	GetSystem()->Register( SystemType::eChargeCancel,		this );
	GetSystem()->Register( SystemType::eUnlockSlot,			this );

	return true;
}

void cEquipmentCharge::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eRegistChargeItem:
		{
			int nSlotIndex = 0;
			bool bEnableMoney = false;
			kStream >> nSlotIndex >> bEnableMoney;
			_RegistChargeItemSlot( nSlotIndex, bEnableMoney );
		}
		break;

	case SystemType::eUpdateChargeList:
		{
			int nUnlockCount = 0;
			kStream >> nUnlockCount;
			_UpdateChargeList( nUnlockCount );
		}
		break;

	case SystemType::eUpdateChargeTime:
		{
			int nSlotIndex = 0;
			bool bComplete = false;
			kStream >> nSlotIndex >> bComplete;
			_UpdateChargeTime( nSlotIndex, bComplete );
		}
		break;

	case SystemType::eNormalChargeItem:
		{
			int nSlotIndex = 0;
			kStream >> nSlotIndex;
			_SetNormalChargeItem( nSlotIndex );
		}
		break;

	case SystemType::eChargeCancel:
		{
			int nSlotIndex = 0;
			kStream >> nSlotIndex;
			_CancelChargeItem( nSlotIndex );
		}
		break;

	case SystemType::eWithdrawItem:
		{
			int nSlotIndex = 0;
			kStream >> nSlotIndex;
			_WithdrawChargeItem( nSlotIndex );
		}
		break;

	case SystemType::eUnlockSlot:
		{
			int nSlotIndex = 0;
			kStream >> nSlotIndex;
			_UnlockSlot( nSlotIndex );
		}
		break;
	}
}

void cEquipmentCharge::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	DeleteResource();
}

void cEquipmentCharge::DeleteResource()
{
	DeleteScript();
}

void cEquipmentCharge::Create(cWindow* pkRoot, int nValue /*= 0 */)
{
	InitScript( pkRoot, NULL, CsPoint( 40, 120 ), CsPoint( 600, 600 ), false );

	m_pHelpButton = AddButton( CsPoint( 540, 5 ), CsPoint( 24, 32 ), CsPoint( 0, 32 ), "DigimonStatus_New\\Digimon_help_btn.tga" );
	if( m_pHelpButton )
		m_pHelpButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cEquipmentCharge::_OnClickHelpButton );

	_MakeChargeList();
}

void cEquipmentCharge::Update(float const& fDeltaTime)
{

}

BOOL cEquipmentCharge::Update_ForMouse()
{
	if( m_pChargeList && m_pChargeList->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		_SetChargeItemTooltip( m_pChargeList->GetMouseOverItem() );
		return TRUE;
	}

	if( m_pHelpButton && m_pHelpButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	return FALSE;
}

void cEquipmentCharge::RenderScript()
{
	cScriptUI::RenderScript();
}

void cEquipmentCharge::ResetDevice()
{
	ResetDeviceScript();
}

void cEquipmentCharge::OnLButtonUp(CsPoint pos)
{

}

bool cEquipmentCharge::CursorIconLBtnUp(CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount)
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			GetSystem()->RegisterItem_NormalCharge( nIconSlot );
			CURSOR_ST.ReleaseIcon();
			return true;
		}
		break;
	default:
		cPrintMsg::PrintMsg( 11018 );
	}
	return false;
}

void cEquipmentCharge::UpdateViewer()
{
	_UpdateChargeList( m_mapCharge.size() );
}

void cEquipmentCharge::_MakeChargeList()
{
	m_pChargeList = NiNew cListBox;
	SAFE_POINTER_RET( m_pChargeList );
	m_pChargeList->Init( m_pRoot, CsPoint( 25, 15 ), CsPoint( 550, 550 ), NULL, false );
	m_pChargeList->SetBackOverAndSelectedImgRender( false );
	m_pChargeList->SetSelectedImg( "Icon\\Mask_Socket.tga" );
	m_pChargeList->SetItemGab( CsPoint( 0, 10 ) );
	m_pChargeList->SetMargin( CsPoint( 0, 5 ) );
	m_pChargeList->AddEvent( cListBox::LIST_ITEM_RIGHT_UP, this, &cEquipmentCharge::_OnRClickChargeItem );
	AddChildControl( m_pChargeList );

	m_pChargeList->RemoveAllItem();

	for( int i = 0; i < IF_CHARGE_LIST_COUNT; ++i )
		_AddChargeList( i );
}

void cEquipmentCharge::_AddChargeList(int const& nIndex)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	// 슬롯 배경
	cSprite* pSlotBg = NiNew cSprite;
	if( pSlotBg )
	{
		pSlotBg->Init( NULL, CsPoint::ZERO, CsPoint( 490, 64 ), "EquipSystem\\Recharge_win.tga", false );
		cString::sSPRITE* pBgImg = pControl->AddSprite( pSlotBg, CsPoint( 10, 0 ) );
		if( pBgImg )
			pBgImg->SetAutoPointerDelete( true );
	}

// 	// 아이템 슬롯
// 	cSprite* pSlot = NiNew cSprite;
// 	if( pSlot )
// 	{
// 		pSlot->Init( NULL, CsPoint::ZERO, CsPoint( 42, 42 ), "DigimonStatus_New\\Digimon_evo_slot.tga", false );
// 		cString::sSPRITE* pSlotImg = pControl->AddSprite( pSlot, CsPoint(a.GetInt("slot","x"), a.GetInt("slot","y")) );
// 		if( pSlotImg )
// 			pSlotImg->SetAutoPointerDelete( true );
// 	}

	sChargeControl sCharge;

	// 아이템 아이콘
	sCharge.pItemIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, 0, 1, CsPoint( 25, 16 ) );
	if( sCharge.pItemIcon )
		sCharge.pItemIcon->SetAutoPointerDelete( true );

	// 아이템 슬롯 잠금
	cSprite* pSlotLock = NiNew cSprite;
	if( pSlotLock )
	{
		pSlotLock->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), "DigimonStatus_New\\Block.tga", false );
		sCharge.pItemLock = pControl->AddSprite( pSlotLock, CsPoint( 25, 16 ) );
		if( sCharge.pItemLock )
			sCharge.pItemLock->SetAutoPointerDelete( true );
	}

	{	// 충전 기간
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor::WHITE );
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText( L"" );

		sCharge.pChargeText = pControl->AddText( &ti, CsPoint( 320, 20 ) );
		if( sCharge.pChargeText )
		{
			sCharge.pChargeText->SetColor( NiColor( 0, 1, 0 ) );
			sCharge.pChargeText->SetVisible( false );
			sCharge.pChargeText->SetAutoPointerDelete( true );
		}
	}

	{	// 보관 기간
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_UNLOCK_MONEY" ).c_str() );

		sCharge.pKeepText = pControl->AddText( &ti, CsPoint( 80, 10 ) );
		if( sCharge.pKeepText )
		{
			sCharge.pKeepText->SetColor( NiColor( 84.f/255.f, 189.f/225.f, 1 ) );
			sCharge.pKeepText->SetEnable( false );
			sCharge.pKeepText->SetAutoPointerDelete( true );
		}
	}

	// 보관 비용
	sCharge.pKeepMoneyIF = pControl->AddMoneyIF( cMoneyIF::DISP_FULL, static_cast<u8>(0), NiColor::WHITE, CFont::FS_9, DT_RIGHT, CsPoint( 80, 34 ) );
	if( sCharge.pKeepMoneyIF )
	{
		sCharge.pKeepMoneyIF->SetEnable( false );
		sCharge.pKeepMoneyIF->SetAutoPointerDelete( true );
	}

	{	// 충전 버튼
		cButton* pChargeBtn = NiNew cButton;
		if( pChargeBtn )
		{
			cText::sTEXTINFO ti;
			ti.Init( CFont::FS_12, NiColor::WHITE );
			ti.s_eTextAlign = DT_CENTER;
			ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_BTN_UNLOCK" ).c_str() );
			CsPoint ptSize( 160, 35 );
			pChargeBtn->Init( NULL, CsPoint::ZERO, ptSize, "System\\Btn_Nor13.tga", false );
			pChargeBtn->SetTexToken( CsPoint( 0, 37 ) );
			pChargeBtn->SetText( &ti );
			pChargeBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cEquipmentCharge::_OnClickChargeButton );
			pChargeBtn->SetUserData( new sButtonInfo( eButtonState::eUnlockBtn, nIndex ) );

			sCharge.pChargeButton = pControl->AddButton( pChargeBtn, 0/*btn value*/, CsPoint( 330, 15 ) );
			if( sCharge.pChargeButton )
			{
				sCharge.pChargeButton->m_ptTextPos = CsPoint( ptSize.x*0.5f, ptSize.y*0.25f );
				sCharge.pChargeButton->SetEnable( false );
				sCharge.pChargeButton->SetAutoPointerDelete( true );
			}
		}
	}

	sCharge.pItemControl = NiNew cListBoxItem( nIndex );
	if( sCharge.pItemControl )
	{
		sCharge.pItemControl->SetItem( pControl );
		sCharge.pItemControl->SetUserData( new sChargeInfo() );
		m_pChargeList->AddItem( sCharge.pItemControl );
		m_mapCharge.insert( std::make_pair( nIndex, sCharge ) );
	}
}

void cEquipmentCharge::_RegistChargeItemSlot(int const& nSlotIndex, bool const& bEnableMoney)
{
	SystemType::sChargeItem const* pChargeItem = GetSystem()->GetChargeItem( nSlotIndex );
	SAFE_POINTER_RET( pChargeItem );

	std::map< int, sChargeControl >::iterator it = m_mapCharge.find( nSlotIndex );
	if( it == m_mapCharge.end() )
		return;

	it->second.SetRegist( pChargeItem->GetItemType(), pChargeItem->nChargeTime, pChargeItem->nKeepTime, pChargeItem->nKeepMoney, bEnableMoney );
}

void cEquipmentCharge::_UpdateChargeList(int const& nUnlockCount)
{
	for( int i = 0; i < nUnlockCount; ++i )
	{
		std::map< int, sChargeControl >::iterator it = m_mapCharge.find( i );
		if( it == m_mapCharge.end() )
			continue;

		SystemType::sChargeItem const* pCItem = GetSystem()->GetChargeItem( i );
		SAFE_POINTER_CON( pCItem );

		// 빈 슬롯이면
		if( pCItem->IsEmptySlot() )
		{
			it->second.SetEmpty();
			continue;
		}

		// 충전 완료이면
		if( pCItem->IsComplete() )
		{
			it->second.SetRegist( pCItem->GetItemType(), pCItem->nChargeTime, pCItem->nKeepTime, 0, true );
			it->second.SetChargeButton( true, eButtonState::eWithdrawBtn );
			continue;
		}

		// 충전 중이면
		it->second.SetRegist( pCItem->GetItemType(), pCItem->nChargeTime, pCItem->nKeepTime, 0, true );
		it->second.SetChargeButton( true, eButtonState::eCancelBtn );
	}

	std::map< int, sChargeControl >::iterator it = m_mapCharge.find( nUnlockCount );
	std::map< int, sChargeControl >::iterator itEnd = m_mapCharge.end();
	for( ; it != itEnd; ++it )
	{
		if( it->first == nUnlockCount )
			it->second.SetChargeButton( true, eButtonState::eUnlockBtn );

		uint nUnlockMoney = GetSystem()->GetUnlockMoney( it->first );
		it->second.SetLock( nUnlockMoney );
	}
}

void cEquipmentCharge::_UpdateChargeTime(int const& nSlotIndex, bool bComplete)
{
	std::map< int, sChargeControl >::iterator it = m_mapCharge.find( nSlotIndex );
	if( it == m_mapCharge.end() )
		return;

	SystemType::sChargeItem const* pCItem = GetSystem()->GetChargeItem( nSlotIndex );
	SAFE_POINTER_RET( pCItem );

	it->second.UpdateLeftTime( pCItem->nKeepTime );

	if( bComplete )
		it->second.SetChargeButton( true, eButtonState::eWithdrawBtn );
}

void cEquipmentCharge::_SetNormalChargeItem(int const& nSlotIndex)
{
	std::map< int, sChargeControl >::iterator it = m_mapCharge.find( nSlotIndex );
	if( it == m_mapCharge.end() )
		return;

	SystemType::sChargeItem const* pCItem = GetSystem()->GetChargeItem( nSlotIndex );
	SAFE_POINTER_RET( pCItem );

	it->second.SetRegist( pCItem->GetItemType(), pCItem->nChargeTime, pCItem->nKeepTime, 0, true );
	it->second.SetChargeButton( true, eButtonState::eCancelBtn );
}

void cEquipmentCharge::_CancelChargeItem(int const& nSlotIndex)
{
	std::map< int, sChargeControl >::iterator it = m_mapCharge.find( nSlotIndex );
	if( it == m_mapCharge.end() )
		return;

	it->second.SetEmpty();
}

void cEquipmentCharge::_WithdrawChargeItem(int nSlotIndex)
{
	std::map< int, sChargeControl >::iterator it = m_mapCharge.find( nSlotIndex );
	if( it == m_mapCharge.end() )
		return;

	it->second.SetEmpty();
}

void cEquipmentCharge::_UnlockSlot(int const& nSlotIndex)
{
	std::map< int, sChargeControl >::iterator it = m_mapCharge.find( nSlotIndex );
	if( it == m_mapCharge.end() )
		return;

	it->second.SetEmpty();

	it = m_mapCharge.find( nSlotIndex + 1 );
	if( it == m_mapCharge.end() )
		return;

	it->second.SetChargeButton( true, eButtonState::eUnlockBtn );
}

void cEquipmentCharge::_SetChargeItemTooltip(cListBoxItem const* pOverItem)
{
	SAFE_POINTER_RET( pOverItem );

	uint nSlotIndex = pOverItem->getID();

	std::map< int, sChargeControl >::iterator it = m_mapCharge.find( nSlotIndex );
	if( it == m_mapCharge.end() )
		return;

	SystemType::sChargeItem const* pSelItem = GetSystem()->GetChargeItem( nSlotIndex );
	SAFE_POINTER_RET( pSelItem );

	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	cString::sICON* pItemIcon = it->second.pItemIcon;
	SAFE_POINTER_RET( pItemIcon );

	// item icon
	if( pItemIcon->m_MouseOverCheckRect.PtInRect( CURSOR_ST.GetPos() ) )
	{
		// 빈 슬롯일 경우
		if( pSelItem->IsEmptySlot() )
		{
#ifdef SIMPLE_TOOLTIP
			pTooltip->SetTooltip( pItemIcon->GetWorldPos(), CsPoint( 32, 32 ), 100, cTooltip::UIHELP, 1071 );
#endif
			return;
		}

		cItemInfo cCItem = pSelItem->cChargeItem;

		pTooltip->SetTooltip( pItemIcon->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, 
			pSelItem->GetItemType(), cBaseWindow::WT_EQUIPMENT_SYSTEM, cTooltip::ITEM, 0, &cCItem );
		return;
	}

#ifdef SIMPLE_TOOLTIP
	if( !pSelItem->IsRegist() )
		return;

	CsRect rcCheck;
	// 충전 비용 툴팁
	cString::sMONEYIF* pMoneyIF = it->second.pKeepMoneyIF;
	if( pMoneyIF )
	{
		rcCheck = CsRect( pOverItem->GetWorldPos() + pMoneyIF->s_ptDeltaPos, CsSIZE( 128, 10 ) );
		if( rcCheck.PtInRect( CURSOR_ST.GetPos() ) )
		{
			pTooltip->SetTooltip( rcCheck.GetPos(), rcCheck.GetSize(), 100, cTooltip::UIHELP, 1072 );
			return;
		}
	}

	// 충전 버튼 툴팁
	cString::sBUTTON* pChargeBtn = it->second.pChargeButton;
	if( pChargeBtn )
	{
		rcCheck = pChargeBtn->GetWorldRect();
		if( rcCheck.PtInRect( CURSOR_ST.GetPos() ) )
		{
			pTooltip->SetTooltip( pChargeBtn->GetWorldPos(), rcCheck.GetSize(), 100, cTooltip::UIHELP, 1073 );
			return;
		}
	}
#endif
}

void cEquipmentCharge::_OnClickChargeButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cButton* pButton = static_cast< cButton* >( pSender );

	sButtonInfo* pBtnInfo = dynamic_cast< sButtonInfo* >( pButton->GetUserData() );
	SAFE_POINTER_RET( pBtnInfo );

	switch( pBtnInfo->eBtnState )
	{
	case eButtonState::eChargeBtn:
		{
			if( !GetSystem()->NormalCharge( pBtnInfo->nSlotIndex ) )
				return;

			pBtnInfo->eBtnState = eButtonState::eCancelBtn;
			pButton->SetText( UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_BTN_CANCEL" ).c_str() );
		}
		break;
	case eButtonState::eCancelBtn:
		GetSystem()->CancelCharge( pBtnInfo->nSlotIndex );
		break;
	case eButtonState::eWithdrawBtn:
		GetSystem()->WithdrawItem( pBtnInfo->nSlotIndex );
		break;
	case eButtonState::eUnlockBtn:
		GetSystem()->UnlockChargeList( pBtnInfo->nSlotIndex );
		break;
	}
}

void cEquipmentCharge::_OnRClickChargeItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cListBoxItem* pOverItem = static_cast< cListBoxItem* >( pData );

	int nSlotIndex = pOverItem->getID();
	std::map< int, sChargeControl >::iterator it = m_mapCharge.find( nSlotIndex );
	if( it == m_mapCharge.end() )
		return;

	if( !GetSystem()->ResetChargeListItem( nSlotIndex ) )
		return;

	it->second.SetEmpty();
}

void cEquipmentCharge::_OnClickHelpButton(void* pSender, void* pData)
{
	GetSystem()->HelpWindow();
}

//////////////////////////////////////////////////////////////////////////
void cEquipmentCharge::sChargeControl::SetLock(u8 nMoney)
{
	if( pItemLock )
		pItemLock->SetVisible( true );

	if( pKeepText )
		pKeepText->SetText( UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_UNLOCK_MONEY" ).c_str() );

	if( pKeepMoneyIF )
		pKeepMoneyIF->SetMoney( nMoney );

	ChangeItem( 0, 0 );
}

void cEquipmentCharge::sChargeControl::SetEmpty()
{
	if( pItemLock )
		pItemLock->SetVisible( false );

	if( pKeepText )
	{
		pKeepText->SetEnable( false );
		pKeepText->SetVisible( false );
	}

	if( pChargeText )
		pChargeText->SetVisible( false );

	SetMoneyIF( 0, true );
	SetChargeButton( false, eButtonState::eChargeBtn );
	ChangeItem( 0, 0 );
}

void cEquipmentCharge::sChargeControl::SetRegist(uint nItemType, uint nChargeTime, uint nKeepTime, u8 nMoney, bool bEnableMoney)
{
	if( pItemLock )
		pItemLock->SetVisible( false );

	// 보관 시간
	if( pKeepText )
	{
		pKeepText->SetEnable( true );
		pKeepText->SetVisible( true );

		std::wstring wsKeep = ConvertToKeepTimeText( nKeepTime );
		pKeepText->SetText( wsKeep.c_str() );
	}

	// 충전 시간
	if( pChargeText )
	{
		std::wstring wsCharge = ConvertToChargeTimeText( nChargeTime );
		pChargeText->SetText( wsCharge.c_str() );
		pChargeText->SetVisible( true );
	}

	ChangeItem( nItemType, nKeepTime );
	SetMoneyIF( nMoney, bEnableMoney );
	SetChargeButton( bEnableMoney, eButtonState::eChargeBtn );
}

void cEquipmentCharge::sChargeControl::SetChargeButton(bool bEnable, eButtonState eState)
{
	SAFE_POINTER_RET( pChargeButton );

	cButton* pCBtn = pChargeButton->s_pButton;
	SAFE_POINTER_RET( pCBtn );

	sButtonInfo* pBtnInfo = dynamic_cast<sButtonInfo*>(pCBtn->GetUserData());
	SAFE_POINTER_RET( pBtnInfo );

	switch( eState )
	{
	case eButtonState::eUnlockBtn:
		pCBtn->SetText( UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_BTN_UNLOCK" ).c_str() );
		break;
	case eButtonState::eChargeBtn:
		pCBtn->SetText( UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_BTN_CHARGE" ).c_str() );
		break;
	case eButtonState::eCancelBtn:
		pCBtn->SetText( UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_BTN_CANCEL" ).c_str() );
		break;
	case eButtonState::eWithdrawBtn:
		pCBtn->SetText( UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_BTN_WITHDRAW" ).c_str() );
		break;
	}

	pBtnInfo->eBtnState = eState;
	pChargeButton->SetEnable( bEnable );
}

void cEquipmentCharge::sChargeControl::SetMoneyIF(u8 nKeepMoney, bool bEnableMoney)
{
	// 보관 비용
	if( pKeepMoneyIF )
	{
		if( bEnableMoney )
			pKeepMoneyIF->SetColor( NiColor::WHITE );
		else
			pKeepMoneyIF->SetColor( NiColor( 1, 0, 0 ) );
		pKeepMoneyIF->SetMoney( nKeepMoney );
	}

	// 충전 버튼
	if( pChargeButton )
		pChargeButton->SetEnable( bEnableMoney );
}

void cEquipmentCharge::sChargeControl::UpdateLeftTime( uint nKeepTime )
{
	// 남은 보관 시간
	if( pKeepText )
	{
		std::wstring wsKeep = ConvertToKeepTimeText( nKeepTime );
		pKeepText->SetText( wsKeep.c_str() );
		pKeepText->SetVisible( true );
	}

	if( pItemControl )
	{
		sChargeInfo* pCInfo = dynamic_cast< sChargeInfo* >( pItemControl->GetUserData() );
		if( pCInfo )
			pCInfo->nLeftTime = nKeepTime;
	}
}

void cEquipmentCharge::sChargeControl::ChangeItem(uint nItemType, uint nKeepTime)
{
	// 장비 아이템
	if( pItemIcon )
		pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemType, 1 );

	if( pItemControl )
	{
		sChargeInfo* pCInfo = dynamic_cast< sChargeInfo* >( pItemControl->GetUserData() );
		if( pCInfo )
			pCInfo->nLeftTime = nKeepTime;
	}
}

std::wstring cEquipmentCharge::sChargeControl::ConvertToKeepTimeText(uint nTime)
{
	if( 0 == nTime )
		return UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_COMPLETE" );

	std::wstring wsCharge;

	int nDay = nTime / 86400;
	int nHour = nDay / 3600;
	int nMin = ( nTime % 3600 ) / 60;

	wsCharge = UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_KEEP_TIME" );
	if( 0 != nDay )
	{
		std::wstring wsTime;
		DmCS::StringFn::Format( wsTime, L"%d%s", nDay, UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_KEEP_DAY" ).c_str() );
		DmCS::StringFn::Add( wsCharge, wsTime );
	}
	if( 0 != nHour )
	{
		std::wstring wsTime;
		DmCS::StringFn::Format( wsTime, L"%d%s", nHour, UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_KEEP_HOUR" ).c_str() );
		DmCS::StringFn::Add( wsCharge, wsTime );
	}
	if( 0 != nMin )
	{
		std::wstring wsTime;
		DmCS::StringFn::Format( wsTime, L"%d%s", nMin, UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_KEEP_MIN" ).c_str() );
		DmCS::StringFn::Add( wsCharge, wsTime );
	}

	if( 0 == nDay && 0 == nHour && 0 == nMin )
		DmCS::StringFn::Add( wsCharge, UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_KEEP_SOON" ) );

	return wsCharge;
}

std::wstring cEquipmentCharge::sChargeControl::ConvertToChargeTimeText(uint nTime)
{
	if( 0 == nTime )
		return L"";

	std::wstring wsCharge;

	int nDay = nTime / 86400/* == 1일*/;
	if( 0 == nDay )
	{
		int nHour = nTime / 3600;
		wsCharge = UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_CHARGE_HOUR" );
		DmCS::StringFn::Replace( wsCharge, L"#Hour#", nHour );
	}
	else
	{
		wsCharge = UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_CHARGE_DAY" );
		DmCS::StringFn::Replace( wsCharge, L"#Day#", nDay );
	}

	return wsCharge;
}
