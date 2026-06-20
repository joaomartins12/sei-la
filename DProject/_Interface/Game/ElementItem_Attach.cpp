
#include "stdafx.h"
#include "ElementItem_Attach.h"

// #define IF_EI_ATTACH_ICONSIZE				CsPoint( 32, 32 )
// #define IF_EI_ATTACH_RECT_0					CsRect( CsPoint( 43, 165 ), CsSIZE( 32, 32 ) )
// #define IF_EI_ATTACH_RECT_1					CsRect( CsPoint( 111, 165 ), CsSIZE( 32, 32 ) )
// #define IF_EI_ATTACH_RECT_2					CsRect( CsPoint( 180, 165 ), CsSIZE( 32, 32 ) )
// 
// #define		ATTACT_NPC_ID	922	// 합성, 분석 NPC ID 같음.


cElementItem_Attach::cElementItem_Attach() :
m_pMoneyAttach( NULL ),
m_pMoneyDetach( NULL ),
m_pBtnAttach( NULL ),
m_pBtnDetach( NULL ),
m_pCancelButton( NULL ),
m_pItemName( NULL ),
m_pListBox( NULL ),
m_pGridBox( NULL )
{

}

cElementItem_Attach::~cElementItem_Attach()
{

}

bool cElementItem_Attach::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eRegistEquip,	this );
	GetSystem()->Register( SystemType::eRegistElem,		this );
	GetSystem()->Register( SystemType::eSelectSocket,	this );
	GetSystem()->Register( SystemType::eReleaseSocket,	this );
	GetSystem()->Register( SystemType::eSuccessAttach,	this );
	GetSystem()->Register( SystemType::eSuccessDetach,	this );
	GetSystem()->Register( SystemType::eResetWindow,	this );

	return true;
}

void cElementItem_Attach::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eRegistEquip:
		{
			uint nInvenIndex = 0;
			int nSocketCount = 0;
			std::wstring wsItemName;
			kStream >> nInvenIndex >> nSocketCount >> wsItemName;
			_SetRegistItem( nInvenIndex, nSocketCount, wsItemName );
		}
		break;
	case SystemType::eRegistElem:
		{
			int nSocketIndex = 0;
			u8 nNeedAttachMoney = 0;
			bool bEnable = false;
			kStream >> nSocketIndex >> nNeedAttachMoney >> bEnable;
			_SetRegistElem( nSocketIndex, nNeedAttachMoney, bEnable );
		}
		break;
	case SystemType::eSelectSocket:
		{
			int nSocketIndex = 0;
			u8 nNeedMoney = 0;
			bool bEnable = false;
			kStream >> nSocketIndex >> nNeedMoney >> bEnable;
			_SelectElement( nSocketIndex, nNeedMoney, bEnable );
		}
		break;
	case SystemType::eReleaseSocket:
		{
			int nSocketIndex = 0;
			u8 nNeedMoney = 0;
			bool bEnable = false;
			bool bReleaseAll = false;
			kStream >> nSocketIndex >> nNeedMoney >> bEnable >> bReleaseAll;
			_ReleaseElement( nSocketIndex, nNeedMoney, bEnable, bReleaseAll );
		}
		break;
	case SystemType::eSuccessAttach:
		_SuccessAttach();
		break;
	case SystemType::eSuccessDetach:
		_SuccessDetach();
		break;
	case SystemType::eResetWindow:
		_ResetRegist();
		break;
	}
}

cBaseWindow::eWINDOW_TYPE cElementItem_Attach::GetWindowType()
{
	return WT_ELEMENTITEM_ATTACH;
}

void cElementItem_Attach::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cElementItem_Attach::DeleteResource()
{
	DeleteScript();
}

void cElementItem_Attach::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "EI_Item\\Attach_Win.tga", CsPoint( 150, 165 ), CsPoint( 255, 330 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( UISTRING_TEXT( "ELEMENT_ATTACH_TITLE_EQUIPMENT_MERGE" ).c_str() );

	{	// 합성 비용
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_ATTACH_MONEY" ).c_str() );
		AddText( &ti, CsPoint( 11, 227 ) );
	}

#ifdef SIMPLE_TOOLTIP
	// 해제 비용 배경
	AddSprite( CsPoint( 9, 261 ), CsPoint( 148, 20 ), "EI_Item\\DetachCost.tga", true );
	{	// 해제 비용
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_DETACH_MONEY" ).c_str() );
		AddText( &ti, CsPoint( 11, 264 ) );
	}
#endif

	{	// 아이템 이름
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( L"" );
		m_pItemName = AddText( &ti, CsPoint( 125, 39 ) );
	}

	// 닫기
	m_pCancelButton = AddButton( CsPoint( 234, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	if( m_pCancelButton )
		m_pCancelButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cElementItem_Attach::_OnClickCancelButton );

	m_pBtnAttach = AddButton( CsPoint( 176, 218 ), CsPoint( 61, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_BTN_ATTACH" ).c_str() );
	if( m_pBtnAttach )
	{
		m_pBtnAttach->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cElementItem_Attach::_OnClickAttachButton );
		m_pBtnAttach->SetEnable( false );
	}

	m_pBtnDetach = AddButton( CsPoint( 176, 256 ), CsPoint( 61, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_DETACH_MONEY" ).c_str() );
	if( m_pBtnDetach )
	{
		m_pBtnDetach->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cElementItem_Attach::_OnClickDetachButton );
		m_pBtnDetach->SetEnable( false );
	}

	m_pMoneyAttach = NiNew cMoneyIF;
	if( m_pMoneyAttach )
	{
		m_pMoneyAttach->Init( cMoneyIF::DISP_SIMPLE_1, (u8)0, false );
		m_pMoneyAttach->SetVisible( false );
		m_pMoneyAttach->SetPos( CsPoint( 18, 224 ) );
		AddChildControl( m_pMoneyAttach );
	}
	m_pMoneyDetach = NiNew cMoneyIF;
	if( m_pMoneyDetach )
	{
		m_pMoneyDetach->Init( cMoneyIF::DISP_SIMPLE_1, (u8)0, false );
		m_pMoneyDetach->SetVisible( false );
		m_pMoneyDetach->SetPos( CsPoint( 18, 261 ) );
		AddChildControl( m_pMoneyDetach );
	}

	m_pListBox = NiNew cListBox;
	if( m_pListBox )
	{
		m_pListBox->Init( m_pRoot, CsPoint( 111, 77 ), CsPoint( 32, 32 ), NULL, false );
		m_pListBox->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pListBox->AddEvent( cListBox::LIST_ITEM_RIGHT_UP, this, &cElementItem_Attach::_OnRClickRegistItem );
		AddChildControl( m_pListBox );
	}
	_MakeRegistItem();
}

void cElementItem_Attach::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cElementItem_Attach::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	_SetSimpleTooltip( CURSOR_ST.GetPos() );

	if( m_pListBox && m_pListBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		_SetTooltipRegistItem( m_pListBox->GetMouseOverItem() );
		return muReturn;
	}

	if( m_pGridBox && m_pGridBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		_SetTooltipSocketItem( m_pGridBox->GetMouseOverItem() );
		return muReturn;
	}

	if( m_pBtnAttach && m_pBtnAttach->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( m_pBtnDetach && m_pBtnDetach->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( m_pCancelButton && m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 255, 27 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
	return muReturn;
}

void cElementItem_Attach::Render()
{
	RenderScript();
}

bool cElementItem_Attach::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			GetSystem()->RegistElementAttachItem( &nIconSlot );
			CURSOR_ST.ReleaseIcon();
			return true;
		}
		break;
	default:
		cPrintMsg::PrintMsg( 11018 );
	}
	return false;
}

void cElementItem_Attach::ResetDevice()
{	
	ResetDeviceScript();
}

void cElementItem_Attach::ResetMap()
{
	GetSystem()->CloseElementItem( NULL );
}

bool cElementItem_Attach::OnEscapeKey()
{
	GetSystem()->CloseElementItem( NULL );
	return true;
}

void cElementItem_Attach::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cElementItem_Attach::UserDie()
{
	cBaseWindow::Close( false );
}

void cElementItem_Attach::RecvDiedAlready()
{
	cBaseWindow::Close( false );
}

void cElementItem_Attach::_OnClickCancelButton(void* pSender, void* pData)
{
	GetSystem()->CloseElementItem( NULL );
}

void cElementItem_Attach::_OnClickAttachButton(void* pSender, void* pData)
{
	GetSystem()->AttachElementItem();
}

void cElementItem_Attach::_OnClickDetachButton(void* pSender, void* pData)
{
	GetSystem()->DetachElementItem();
}

void cElementItem_Attach::_OnRClickRegistItem(void* pSender, void* pData)
{
	GetSystem()->ResetRegistItem_Equip();
}

void cElementItem_Attach::_OnClickElementSocket(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pClicked = static_cast< cGridListBoxItem* >( pData );
	GetSystem()->SelectAttachedSocket( pClicked->getID() );
}

void cElementItem_Attach::_OnRClickElementSocket(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pClicked = static_cast< cGridListBoxItem* >( pData );
	GetSystem()->ReleaseRegistSocket( pClicked->getID() );
}

void cElementItem_Attach::_MakeRegistItem()
{
	SAFE_POINTER_RET( m_pListBox );
	m_pListBox->RemoveAllItem();

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	m_RegistItem.pItemIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, 0, 1, CsPoint::ZERO );
	if( m_RegistItem.pItemIcon )
		m_RegistItem.pItemIcon->SetAutoPointerDelete( true );

	m_RegistItem.pItemControl = NiNew cListBoxItem;
	if( m_RegistItem.pItemControl )
	{
		m_RegistItem.pItemControl->SetItem( pControl );
		m_RegistItem.pItemControl->SetUserData( new sItemInfo( -1 ) );
		m_pListBox->AddItem( m_RegistItem.pItemControl );
	}
}

void cElementItem_Attach::_MakeSocketSlot(int const& nSlotCount)
{
	const CsPoint ptItemSize( 32, 32 );
	const CsPoint ptItemGab( 48, 0 );
	CsPoint ptSize = ptItemSize;
	ptSize.x = ( ptItemSize.x + ptItemGab.x ) * nSlotCount - ptItemGab.x;
	CsPoint ptPos( 0, 165 );
	ptPos.x = ( GetRootSize().x - ptSize.x ) >> 1;

	if( NULL == m_pGridBox )
	{
		m_pGridBox = NiNew cGridListBox;
		if( m_pGridBox )
		{
			m_pGridBox->Init( m_pRoot, ptPos, ptSize, ptItemGab, ptItemSize, 
				cGridListBox::LowRightDown, cGridListBox::LeftTop, false, nSlotCount );
			m_pGridBox->SetMouseOverImg( "Icon\\Mask_Over.tga" );
			m_pGridBox->SetBackOverAndSelectedImgRender( false );
			m_pGridBox->SetAutoSelection( false );
			m_pGridBox->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &cElementItem_Attach::_OnClickElementSocket );
			m_pGridBox->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &cElementItem_Attach::_OnRClickElementSocket );
			AddChildControl( m_pGridBox );
		}
	}
	else
	{
		m_pGridBox->Init( m_pRoot, ptPos, ptSize, ptItemGab, ptItemSize, 
			cGridListBox::LowRightDown, cGridListBox::LeftTop, false, nSlotCount );
		m_pGridBox->ResetDevice();
	}
}

void cElementItem_Attach::_AddElementSocket(int const& nSlotIndex, int const& nItemType, u1 const& nRate, bool const& bAttach)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	const CsPoint ptItemSize( 32, 32 );

	// 소켓 슬롯 배경
	cSprite* pSlot = NiNew cSprite;
	if( pSlot )
	{
		std::string strPath = GetSystem()->GetSlotImagePath();
		pSlot->Init( NULL, CsPoint::ZERO, CsPoint( 40, 40 ), strPath.c_str(), false );
		cString::sSPRITE* pSprite = pControl->AddSprite( pSlot, CsPoint( -4, -4 ) );
		if( pSprite )
		{
			pSprite->SetAutoPointerDelete( true );
			if( strPath.empty() )
				pSprite->SetVisible( false );
		}
	}

	sSocketItem sSocket;
	// 소켓 아이템 아이콘
	sSocket.pItemIcon = pControl->AddIcon( ptItemSize, ICONITEM::ITEM_ICON, nItemType, 1, CsPoint::ZERO );
	if( sSocket.pItemIcon )
	{
		sSocket.pItemIcon->s_nIndex_3 = nRate;
		sSocket.pItemIcon->SetAutoPointerDelete( true );
	}

	// 장착된 소켓 표시
	cSprite* pAttach = NiNew cSprite;
	if( pAttach )
	{
		pAttach->Init( NULL, CsPoint::ZERO, ptItemSize, "Icon\\Mask_Socket.tga", false );
		sSocket.pAttachedImg = pControl->AddSprite( pAttach );
		if( sSocket.pAttachedImg )
		{
			sSocket.pAttachedImg->SetAutoPointerDelete( true );
			sSocket.pAttachedImg->SetVisible( bAttach );
		}
	}

	// 선택된 소켓 표시
	cSprite* pSelect = NiNew cSprite;
	if( pSelect )
	{
		pSelect->Init( NULL, CsPoint::ZERO, ptItemSize, "Icon\\Mask_Click.tga", false );
		sSocket.pSelectedImg = pControl->AddSprite( pSelect );
		if( sSocket.pSelectedImg )
		{
			sSocket.pSelectedImg->SetAutoPointerDelete( true );
			sSocket.pSelectedImg->SetVisible( false );
		}
	}

	sSocket.pItemControl = NiNew cGridListBoxItem( nSlotIndex, ptItemSize );
	if( sSocket.pItemControl )
	{
		sSocket.pItemControl->SetItem( pControl );
		sSocket.pItemControl->SetUserData( new sElemInfo( nItemType, nRate ) );
		m_pGridBox->AddItem( sSocket.pItemControl );
		m_mapSocket.insert( std::make_pair( nSlotIndex, sSocket ) );
	}
}

void cElementItem_Attach::_SetRegistItem(uint const& nInvenIndex, int const& nSocketCount, std::wstring const& wsItemName)
{
	cWindow::PlaySound( cWindow::SD_At1 );

	uint nItemType = GetSystem()->GetInvenItemType( nInvenIndex );
	m_RegistItem.ChangeRegistItem( nItemType, nInvenIndex );

	_MakeSocketSlot( nSocketCount );

	SAFE_POINTER_RET( m_pGridBox );
	m_pGridBox->RemoveAllItem();

	m_mapSocket.clear();
	for( int i = 0; i < nSocketCount; ++i )
	{
		SystemType::sSocketInfo* pSocketInfo = GetSystem()->GetSocketInfo( i );
		SAFE_POINTER_CON( pSocketInfo );
		bool bAttach = ( SystemType::eSocketState::eAttached == pSocketInfo->eState ) ? true : false;
		_AddElementSocket( i, pSocketInfo->nItemType, pSocketInfo->nItemRate, bAttach );
	}

	if( m_pItemName )
		m_pItemName->SetText( wsItemName.c_str() );
}

void cElementItem_Attach::_SetRegistElem(int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable)
{
	std::map< int, sSocketItem >::iterator it = m_mapSocket.find( nSocketIndex );
	if( it == m_mapSocket.end() )
		return;

	cWindow::PlaySound( cWindow::SD_At1 );

	SystemType::sSocketInfo* pSocketInfo = GetSystem()->GetSocketInfo( nSocketIndex );
	if( pSocketInfo )
		it->second.ChangeSocketItem( pSocketInfo->nItemType, pSocketInfo->nItemRate );

	_SetMoney( m_pMoneyAttach, true, bEnable, nNeedMoney );
	_SetAttachButton( bEnable );
}

void cElementItem_Attach::_SelectElement(int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable)
{
	std::map< int, sSocketItem >::iterator it = m_mapSocket.begin();
	std::map< int, sSocketItem >::iterator itEnd = m_mapSocket.end();
	for( ; it != itEnd; ++it )
	{
		if( nSocketIndex != it->first )
			it->second.SetSelected( false );
		else
			it->second.SetSelected( true );
	}

	_SetMoney( m_pMoneyDetach, true, bEnable, nNeedMoney );
	_SetDetachButton( bEnable );
}

void cElementItem_Attach::_ReleaseElement(int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable, bool const& bReleaseAll)
{
	std::map< int, sSocketItem >::iterator it = m_mapSocket.find( nSocketIndex );
	if( it == m_mapSocket.end() )
		return;

	it->second.Reset();

	_SetMoney( m_pMoneyAttach, !bReleaseAll, bEnable, nNeedMoney );
	_SetAttachButton( !bReleaseAll );
}

void cElementItem_Attach::_SetMoney(cMoneyIF* pMoneyIF, bool bVisible, bool bEnable, u8 nMoney)
{
	SAFE_POINTER_RET( pMoneyIF );
	if( !bVisible )
	{
		pMoneyIF->SetVisible( false );
		return;
	}

	pMoneyIF->SetVisible( true );
	if( !bEnable )
		pMoneyIF->SetColor_BeforeSetMoney( NiColor( 1, 0, 0 ) );
	else
		pMoneyIF->SetColor_BeforeSetMoney( NiColor::WHITE );

	pMoneyIF->SetMoney( nMoney );
}

void cElementItem_Attach::_SetAttachButton(bool bEnable)
{
	if( m_pBtnAttach )
		m_pBtnAttach->SetEnable( bEnable );
}

void cElementItem_Attach::_SetDetachButton(bool bEnable)
{
	if( m_pBtnDetach )
		m_pBtnDetach->SetEnable( bEnable );
}

void cElementItem_Attach::_SetTooltipRegistItem(cListBoxItem const* pOverItem)
{
	SAFE_POINTER_RET( pOverItem );
	sItemInfo* pInfo = dynamic_cast< sItemInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pInfo );

	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	// 장비 등록 안 됐음
	if( 0 > pInfo->nInvenIndex )
	{
#ifdef SIMPLE_TOOLTIP
		pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), 100, cTooltip::UIHELP, 1034 );
#endif
		return;
	}

	cItemInfo* pRegistItem = GetSystem()->GetInvenItem( pInfo->nInvenIndex );
	SAFE_POINTER_RET( pRegistItem );	
	pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, 
		pRegistItem->GetType(), cBaseWindow::WT_ELEMENTITEM_ATTACH, cTooltip::OPEN_SLOT, 0, pRegistItem );
}

void cElementItem_Attach::_SetTooltipSocketItem(cGridListBoxItem const* pOverItem)
{
	SAFE_POINTER_RET( pOverItem );
	sElemInfo* pInfo = dynamic_cast< sElemInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pInfo );

	int nSocketIndex = pOverItem->getID();
	SystemType::sSocketInfo* pSocketInfo = GetSystem()->GetSocketInfo( nSocketIndex );
	SAFE_POINTER_RET( pSocketInfo );

	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	switch( pSocketInfo->eState )
	{
	case SystemType::eSocketState::eNoneRegist:
		{
#ifdef SIMPLE_TOOLTIP
			pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), 100, cTooltip::UIHELP, 1035 );
#endif
		}
		return;
	case SystemType::eSocketState::eAttached:
		{
			cItemInfo socketItem = GetSystem()->GetSocketItem( nSocketIndex );
			if( !socketItem.IsEnable() )
				return;
			pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM,
				socketItem.m_nType, cBaseWindow::WT_ELEMENTITEM_ATTACH, cTooltip::OPEN_SLOT, nSocketIndex, &socketItem );

		}
		return;
	case SystemType::eSocketState::eRegist:
		{
			if( 0 > pSocketInfo->nInvenIndex )
				return;
			cItemInfo* pInfo = GetSystem()->GetInvenItem( pSocketInfo->nInvenIndex );
			SAFE_POINTER_RET( pInfo );
			pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM,
				pInfo->GetType(), cBaseWindow::WT_ELEMENTITEM_ATTACH, cTooltip::OPEN_SLOT, nSocketIndex, pInfo );
		}
		return;
	}
}

void cElementItem_Attach::_SuccessAttach()
{
	std::map< int, sSocketItem >::iterator it = m_mapSocket.begin();
	std::map< int, sSocketItem >::iterator itEnd = m_mapSocket.end();
	for( ; it != itEnd; ++it )
	{
		SystemType::sSocketInfo* pSocketInfo = GetSystem()->GetSocketInfo( it->first );
		SAFE_POINTER_CON( pSocketInfo );
		if( SystemType::eSocketState::eAttached == pSocketInfo->eState )
			it->second.SetAttached( true );
	}

	_SetMoney( m_pMoneyAttach, false, false, 0 );
	_SetAttachButton( false );
}

void cElementItem_Attach::_SuccessDetach()
{
	std::map< int, sSocketItem >::iterator it = m_mapSocket.begin();
	std::map< int, sSocketItem >::iterator itEnd = m_mapSocket.end();
	for( ; it != itEnd; ++it )
	{
		SystemType::sSocketInfo* pSocketInfo = GetSystem()->GetSocketInfo( it->first );
		SAFE_POINTER_CON( pSocketInfo );
		if( SystemType::eSocketState::eNoneRegist == pSocketInfo->eState )
			it->second.Reset();
	}

	_SetMoney( m_pMoneyDetach, false, false, 0 );
	_SetDetachButton( false );
}

void cElementItem_Attach::_ResetRegist()
{
	m_RegistItem.Reset();

	m_mapSocket.clear();

	if( m_pGridBox )
		m_pGridBox->RemoveAllItem();

	_SetMoney( m_pMoneyAttach, false, false, 0 );
	_SetMoney( m_pMoneyDetach, false, false, 0 );
	_SetAttachButton( false );
	_SetDetachButton( false );
}

void cElementItem_Attach::_SetSimpleTooltip(CsPoint ptPos)
{
#ifdef SIMPLE_TOOLTIP
	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	// Attach button
	CsRect rcCheckBox( m_pBtnAttach->GetPos(), CsSIZE( m_pBtnAttach->GetSize() ) );
	if( rcCheckBox.PtInRect( ptPos ) )
		pTooltip->SetTooltip( m_pBtnAttach->GetPos(), m_pBtnAttach->GetSize(), 100, cTooltip::UIHELP, 1037 );

	// Detach button
	rcCheckBox = CsRect( m_pBtnDetach->GetPos(), CsSIZE( m_pBtnDetach->GetSize() ) );
	if( rcCheckBox.PtInRect( ptPos ) )
		pTooltip->SetTooltip( m_pBtnDetach->GetPos(), m_pBtnDetach->GetSize(), 100, cTooltip::UIHELP, 1039 );

	// Detach cost text
	rcCheckBox = CsRect( m_pMoneyDetach->GetClient(), CsSIZE( m_pMoneyDetach->GetSize() ) );
	if( rcCheckBox.PtInRect( ptPos ) )
		pTooltip->SetTooltip( m_pMoneyDetach->GetClient(), m_pMoneyDetach->GetSize(), 100, cTooltip::UIHELP, 1038 );
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void cElementItem_Attach::sRegistItem::ChangeRegistItem(uint nItemType, int nInvenIndex)
{
	if( pItemIcon )
		pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemType, 1 );

	if( pItemControl )
	{
		sItemInfo* pInfo = dynamic_cast< sItemInfo* >( pItemControl->GetUserData() );
		if( pInfo )
			pInfo->nInvenIndex = nInvenIndex;
	}
}

void cElementItem_Attach::sRegistItem::Reset()
{
	ChangeRegistItem( 0, -1 );
}

void cElementItem_Attach::sSocketItem::ChangeSocketItem(uint nItemType, u1 nRate)
{
	if( pItemIcon )
		pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemType, 1, nRate );

	if( pItemControl )
	{
		sElemInfo* pInfo = dynamic_cast< sElemInfo* >( pItemControl->GetUserData() );
		if( pInfo )
			pInfo->SetItem( nItemType, nRate );
	}
}

void cElementItem_Attach::sSocketItem::SetAttached(bool bVisible)
{
	if( pAttachedImg )
		pAttachedImg->SetVisible( bVisible );
}

void cElementItem_Attach::sSocketItem::SetSelected(bool bVisible)
{
	if( pSelectedImg )
		pSelectedImg->SetVisible( bVisible );
}

void cElementItem_Attach::sSocketItem::Reset()
{
	ChangeSocketItem( 0, 0 );
	SetAttached( false );
	SetSelected( false );
}
