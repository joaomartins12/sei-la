
#include "stdafx.h"
#include "Inventory.h"

#define IF_INVENTORY_ICON_SIZE CsPoint( 32, 32 )
#define ICON_HALF		16

cInventory::cInventory():m_pMoney(NULL),m_pBlock(NULL),m_pBlockInItem(NULL)
,m_pCancelButton(NULL),m_pSort(NULL),m_nInvenIndex(0),m_nDataStartIndex(NULL)
#ifdef UI_INVENTORY_RENEWAL
,m_pFilterTab(NULL), m_pScrollBar(NULL), m_pNewMark(NULL), m_bIsFiltering(false)
#endif
{
}

cInventory::~cInventory()
{

}

void cInventory::Destroy()
{
	cBaseWindow::Delete();	
}

void cInventory::DeleteResource()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_D_NAME, m_nInvenIndex );
	cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_T_NAME, m_nInvenIndex );	
	cSeparate::DeleteInstance( GetWindowType(), m_nInvenIndex );

	DeleteScript();

	SAFE_NIDELETE( m_pBlock );
	SAFE_NIDELETE( m_pBlockInItem );
#ifdef UI_INVENTORY_RENEWAL
	SAFE_NIDELETE( m_pNewMark );
#endif
}

void cInventory::ResetMap()
{
	cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_D_NAME, m_nInvenIndex );
	cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_T_NAME, m_nInvenIndex );	
	cSeparate::DeleteInstance( GetWindowType(), m_nInvenIndex );
}

bool cInventory::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eEnableWindow,		this );

	return true;
}

void cInventory::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eEnableWindow:
		{
			bool bIsEnable = false;
			kStream >> bIsEnable;
			SetEnableWindow( bIsEnable );
		}
		break;
	}
}

void cInventory::Create(int nValue /* = 0  */)
{
#ifdef UI_INVENTORY_RENEWAL
	if( !IsLive() )
	{
		SetRootClient( CsPoint( g_nScreenWidth - 400, g_nScreenHeight - 668 ) );

		InitScript( "NewInventory\\new_inventory_win.tga", m_ptRootClient, CsPoint( 398, 456 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );
		AddTitle( UISTRING_TEXT( "INVENTORY_TITLE_TEXT" ).c_str() );

		SAFE_POINTER_RET( g_pDataMng );

		// create filter button
		_MakeFilterTab();

		// set item pos
		CsPoint ptStart( 11, 66 );
		for( int row = 0; row < IF_INVENTORY_ROW; ++row )
		{
			ptStart.x = 11;
			for( int col = 0; col < IF_INVENTORY_COL; ++col )
			{
				m_IFIcon[ row * IF_INVENTORY_COL + col ].Init( CsRect( ptStart, (CsSIZE)IF_INVENTORY_ICON_SIZE ) );
				ptStart.x += 36;
			}
			ptStart.y += 36;
		}

		m_pScrollBar = AddScrollBar(cScrollBar::TYPE_1, CsPoint( 376, 60 ), CsPoint( 16, 365 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 12, 65 ), CsPoint( 372, 430 ) ), IF_INVENTORY_ROW );
		m_pScrollBar->SetRange( CsPoint( 0, (nLimit::Inven / IF_INVENTORY_COL) ) );

		cData_Inven* pInven = g_pDataMng->GetInven();
		SAFE_POINTER_RET( pInven );
		m_pMoney = NiNew cMoneyIF;
		if( m_pMoney )
		{
			m_pMoney->Init( cMoneyIF::DISP_FULL, pInven->GetMoney(), false );
			m_pMoney->SetPos( CsPoint( 232, 432 ) );
			AddChildControl( m_pMoney );
		}

		m_pSort = AddButton( CsPoint( 372, 30 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "inventory\\Invensort.tga" );
		if( m_pSort )
			m_pSort->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cInventory::SortButtonClick );

		m_pBlock = NiNew cSprite;
		m_pBlock->Init( NULL, CsPoint::ZERO, IF_INVENTORY_ICON_SIZE, "NewInventory\\new_inventory_lock.tga", false );

		m_pBlockInItem = NiNew cSprite;
		m_pBlockInItem->Init( NULL, CsPoint::ZERO, IF_INVENTORY_ICON_SIZE, "NewInventory\\new_inventory_lock.tga", false );
		m_pBlockInItem->SetAlpha( 0.6f );

		m_pNewMark = NiNew cSprite;
		m_pNewMark->Init( NULL, CsPoint::ZERO, CsPoint( 34, 34 ), "NewInventory\\new_inventory_notification.tga", false );

		m_pCancelButton = AddButton( CsPoint( 372, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
		if( m_pCancelButton )
			m_pCancelButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cInventory::CloseInvenBtnClick );
	}

#else
	if( IsLive() == false )
	{
		m_nInvenIndex = nValue;
		m_nDataStartIndex = m_nInvenIndex*IF_INVENTORY_ROW*IF_INVENTORY_COL;

		// 초기 위치
		switch( m_nInvenIndex )
		{
		case 0:
			SetRootClient( CsPoint( g_nScreenWidth - 324, g_nScreenHeight - 668 ) );
			break;
		default:
			SetRootClient( CsPoint( g_nScreenWidth - 324, g_nScreenHeight - 668 + 174 + (m_nInvenIndex-1)*148 ) );
		}

		// 아이템 위치	
		CsPoint ptStart( 9, 33 );
		for( int y=0; y<IF_INVENTORY_ROW; ++y )
		{
			ptStart.x = 9;
			for( int x=0; x<IF_INVENTORY_COL; ++x )
			{
				m_IFIcon[ y*IF_INVENTORY_COL + x ].Init( CsRect( ptStart, (CsSIZE)IF_INVENTORY_ICON_SIZE ) );
				ptStart.x += 36;
			}
			ptStart.y += 36;
		}

		if( m_nInvenIndex == 0 )
		{
			InitScript( "Inventory\\MainBag.tga", m_ptRootClient, CsPoint( 266, 174 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
			AddTitle( UISTRING_TEXT( "INVENTORY_TITLE_INVENTORY" ).c_str() );

			m_pMoney = NiNew cMoneyIF;
			if( m_pMoney )
			{
				m_pMoney->Init( cMoneyIF::DISP_FULL, g_pDataMng->GetInven()->GetMoney(), false );
				AddChildControl( m_pMoney );
				m_pMoney->SetPos( CsPoint( 112, 150 ) );
			}		

			m_pSort = AddButton( CsPoint( 12, 150 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "inventory\\Invensort.tga" );
			if( m_pSort )
				m_pSort->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cInventory::SortButtonClick );

#ifdef PC_BANG_SERVICE
			if( g_pDataMng->GetPCbang()->GetPcBang() == true )
				net::game->SendPCBangInvenRequest();
#endif
		}
		else
		{
			InitScript( "Inventory\\Sub_Bag.tga", m_ptRootClient, CsPoint( 266, 148 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );		
			AddTitle( UISTRING_TEXT( "INVENTORY_TITLE_INVENTORY_EXTRA" ).c_str() );
		}
		m_pBlock = NiNew cSprite;
		m_pBlock->Init( NULL, CsPoint::ZERO, IF_INVENTORY_ICON_SIZE, "Inventory\\Block.tga", false );

		m_pBlockInItem = NiNew cSprite;
		m_pBlockInItem->Init( NULL, CsPoint::ZERO, IF_INVENTORY_ICON_SIZE, "Inventory\\Block.tga", false );
		m_pBlockInItem->SetAlpha( 0.6f );

		m_pCancelButton = AddButton( CsPoint( 245, 6 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
		if( m_pCancelButton )
			m_pCancelButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cInventory::CloseInvenBtnClick );
	}
#endif // UI_INVENTORY

	cBaseWindow::Init();

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SPIRIT_EVOLUTION, 0 ) || 
		g_pGameIF->IsActiveWindow( cBaseWindow::WT_TALK, 0 ) ||
		g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_COMP, 0 ))
	{
		SetEnableWindow( false );
	}
}

bool cInventory::Close( bool bSound /* = true  */ )
{
	//2017-01-09-nova 스피릿진화창에서는 인벤토리가 비활성화된것을 활성화하여서 종료(#4564/#4308)
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SPIRIT_EVOLUTION, 0 ) || 
		g_pGameIF->IsActiveWindow( cBaseWindow::WT_TALK, 0 ) ||
		g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_COMP, 0 ))
	{
		if( IsEnableWindow() == false )
			SetEnableWindow( true );
	}

	if( IsEnableWindow() == false )
		return false;

	if( bSound )
	{
		PlayCloseSound();
	}
#ifdef UI_INVENTORY_RENEWAL
	m_bIsFiltering = false;
	if( m_pFilterTab )
		m_pFilterTab->SetCheckIndex( 0 );
#endif
	SetShowWindow( false );

	return true;
}

void cInventory::Update(float const& fDeltaTime)
{	
	cCreateName::UpdateInstance( cBaseWindow::WT_CHANGE_D_NAME, m_nInvenIndex );
	cCreateName::UpdateInstance( cBaseWindow::WT_CHANGE_T_NAME, m_nInvenIndex );	
	cSeparate::UpdateInstance( GetWindowType(), m_nInvenIndex );

	UpdateScript( fDeltaTime );

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	if( m_pMoney )
		m_pMoney->SetMoney( pInven->GetMoney() );

	SAFE_POINTER_RET( g_pGameIF );
	SAFE_POINTER_RET( g_pCharMng );

	if( m_nInvenIndex == 0 )
	{
		if( g_pDataMng->GetSortCoolTime()->IsCoolTime() == true ||
			g_pGameIF->IsOpenNpcWindow() == true ||
			g_pDataMng->IsItemAllLock() == true ||
			g_pCharMng->GetDigimonUser( 0 )->IsBattle() == true ||
			g_pDataMng->IsSort() == true ) 
		{
			m_pSort->SetEnable( false );
		}
		else
		{
			m_pSort->SetEnable( true );
		}
	}

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cInventory::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_CHANGE_D_NAME, m_nInvenIndex );
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;

	muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_CHANGE_T_NAME, m_nInvenIndex );
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;

	muReturn = cSeparate::Update_ForMouseInstance( GetWindowType(), m_nInvenIndex );
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;

	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;
#ifdef UI_INVENTORY_RENEWAL
	if( m_pFilterTab && m_pFilterTab->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;

	if( m_pScrollBar && m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;
#endif
	if( m_pSort && m_pSort->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pCancelButton && m_pCancelButton->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( _UpdateIcon_ForMouse() == true )
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
#ifdef UI_INVENTORY_RENEWAL
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 370, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
#else
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 266, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
#endif

	return muReturn;
}

void cInventory::SortButtonClick(void* pSender, void* pData)
{
#ifdef UI_INVENTORY_RENEWAL
	SetEnableWindow( false );
	GetSystem()->SendSortStart( nItemSort::eINVEN );
#else
	SAFE_POINTER_RET( g_pDataMng );
	g_pDataMng->InvenSort();
	CsCoolTimeSeq* pCoolTimeSeq = g_pDataMng->GetSortCoolTime();
	SAFE_POINTER_RET( pCoolTimeSeq );
	pCoolTimeSeq->Start();
#endif
}

void cInventory::CloseInvenBtnClick(void* pSender, void* pData)
{
	GetSystem()->CloseWindow();
}

bool cInventory::_UpdateIcon_ForMouse()
{
	bool bCheck = false;
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	int nIconIndex = -1;

	switch( eCursor )
	{	
	case CURSOR::LBUTTON_DOWN:		
		{
			for( int i=0; i<IF_INVENTORY_COUNT; ++i )
			{
				if( ( bCheck == false )&&( m_IFIcon[ i ].MouseLBtnClick( ptMouseLocalPos ) == true ) )
				{
					bCheck = true;
					nIconIndex = i;
				}
				else
				{
					m_IFIcon[ i ].ReleaseState();
				}
			}
		}
		break;
	case CURSOR::RBUTTON_DOWN:
		{
			for( int i=0; i<IF_INVENTORY_COUNT; ++i )
			{
				if( ( bCheck == false )&&( m_IFIcon[ i ].MouseRBtnClick( ptMouseLocalPos ) == true ) )
				{					
					bCheck = true;
					nIconIndex = i;
				}
				else
				{
					m_IFIcon[ i ].ReleaseState();
				}
			}
		}
		break;
	case CURSOR::LBUTTON_PRESS:
		{
			OnMouseOutWindow();

			for( int i=0; i<IF_INVENTORY_COUNT; ++i )
			{
				if( m_IFIcon[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( ( bCheck == false )&&( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == true ) )
					{
						bCheck = true;
						nIconIndex = i;
					}
					else
						m_IFIcon[ i ].ReleaseState();
				}
			}			
		}
		break;			
	case CURSOR::RBUTTON_PRESS:
		{
			for( int i=0; i<IF_INVENTORY_COUNT; ++i )
			{
				if( m_IFIcon[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( ( bCheck == false )&&( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == true ) )
					{
						bCheck = true;
						nIconIndex = i;
					}
					else
						m_IFIcon[ i ].ReleaseState();
				}
			}
		}
		break;
	case CURSOR::LBUTTON_UP:
	case CURSOR::RBUTTON_UP:
	case CURSOR::BUTTON_NONE:
		{
			for( int i=0; i<IF_INVENTORY_COUNT; ++i )
			{
				if( ( bCheck == false )&&( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == true ) )
				{					
					bCheck = true;
					nIconIndex = i;
#ifdef UI_INVENTORY_RENEWAL
					_SetNewMark( i, false );
#endif
				}
				else
				{
					m_IFIcon[ i ].ReleaseState();
				}
			}
		}	
		break;
	}

	_UpdateTooltip( ptMouseLocalPos, nIconIndex );

	return bCheck;
}

void cInventory::_UpdateTooltip(CsPoint ptPos, int nIndex)
{
	// 툴팁
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );	
	int nInvenCount = pInven->GetInvenSlotCount();

#ifdef UI_INVENTORY_RENEWAL
	SAFE_POINTER_RET( m_pScrollBar );
	int nCurScrollPos = m_pScrollBar->GetCurPosIndex();
	int nSlotIndex = nIndex + nCurScrollPos * IF_INVENTORY_COL;
	if( m_bIsFiltering )
	{
		nSlotIndex = GetSystem()->GetFilteringItemIdx( nSlotIndex );
		if( -1 == nSlotIndex )
			return;
	}

	if( nSlotIndex >= nLimit::Inven )
		return;
#else
	int nSlotIndex = nIndex + m_nDataStartIndex;
#endif
	cItemInfo* pData = pInven->GetData( nSlotIndex );
	SAFE_POINTER_RET( pData );
	if( !pData->IsEnable() )
		return;

	if( m_IFIcon[ nIndex ].s_Rect.PtInRect( ptPos ) == TRUE )
	{
		cTooltip::eSLOT eSlot = ( nSlotIndex < nInvenCount ) ? cTooltip::OPEN_SLOT : cTooltip::CLOSE_SLOT;
		CsPoint pos = m_IFIcon[ nIndex ].GetPos() + GetRootClient();		

		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RET( pTooltip );
		pTooltip->SetTooltip( pos, IF_INVENTORY_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->GetType(), cBaseWindow::WT_INVENTORY, eSlot, 0, pData );

		//////////////////////////////////////////////////////////
		//2017-01-19-nova 장착중인 아이템을 비교창에 출력
		SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() );
		SAFE_POINTER_RET( pFTItem );
		CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
		SAFE_POINTER_RET( pFTInfo );
		int nItemType = pFTInfo->s_nType_L;

		SAFE_POINTER_RET( g_pDataMng );
		cData_TEquip* pEquip = g_pDataMng->GetTEquip();
		SAFE_POINTER_RET( pEquip );
		for( int i = 0; i < nLimit::Equip; ++i )
		{
			cItemInfo* PlayerpData = pEquip->GetData( i );
			if( !PlayerpData->IsEnable() )
				continue;

			if(PlayerpData->m_nType != 0)
			{
				CsItem* pFTEquip = nsCsFileTable::g_pItemMng->GetItem( PlayerpData->GetType() );
				SAFE_POINTER_CON( pFTEquip );
				CsItem::sINFO* pFTSrc = pFTEquip->GetInfo();
				SAFE_POINTER_CON( pFTSrc );

				if(nItemType == pFTSrc->s_nType_L)
				{
					cTooltip* pCmpTooltip = TOOLTIPMNG_STPTR->GetCompareTooltip();
					SAFE_POINTER_CON( pCmpTooltip );
					pCmpTooltip->SetCompareTooltip( pos, IF_INVENTORY_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, PlayerpData->GetType(), cBaseWindow::WT_INVENTORY, eSlot, 0, PlayerpData );
				}
			}
		}
		//////////////////////////////////////////////////////////
	}
}

#ifdef UI_INVENTORY_RENEWAL
void cInventory::Render()
{
	RenderScript();

	SAFE_POINTER_RET( g_pDataMng );
	SAFE_POINTER_RET( g_pIconMng );
	cItemInfo* pItem;
	CsItem::sINFO* pFTItemInfo;
	CsPoint pos;

	int nOpenInvenSlotCount = GetSystem()->GetInvenSlotCount();

	CsPoint ptGap( -1, -1 );
	SAFE_POINTER_RET( m_pScrollBar );
	int nCurScrollPos = m_pScrollBar->GetCurPosIndex();
	for( int i = 0; i < IF_INVENTORY_COUNT; ++i )
	{
		int nSlotIndex = i + nCurScrollPos * IF_INVENTORY_COL;
		if( m_bIsFiltering )
		{
			nSlotIndex = GetSystem()->GetFilteringItemIdx( nSlotIndex );
			if( -1 == nSlotIndex )
				continue;
		}

		if( nSlotIndex >= nLimit::Inven )
		{
			pos = m_IFIcon[ i ].GetPos() + GetRootClient();
			m_pBlock->Render( pos );
			continue;
		}

		pItem = GetSystem()->GetInvenItem( nSlotIndex );
		SAFE_POINTER_CON( pItem );

		// 위치
		pos = m_IFIcon[ i ].GetPos() + GetRootClient();

		// 열린 인벤 토리인가
		if( nSlotIndex < nOpenInvenSlotCount )
		{
			if( pItem->IsEnable() == true )
			{
				pFTItemInfo = GetSystem()->GetFTItemInfo( pItem->GetType() );
				SAFE_POINTER_CON( pFTItemInfo );

				// 아이템 랜더링 - 숫자 포함
				switch( pFTItemInfo->s_nType_L )
				{
				case nItem::Money:
				case nItem::GoldBar:
					g_pIconMng->RenderItem( pItem, pos, false );
					break;				
				default:
					g_pIconMng->RenderItem( pItem, pos, true );
				}				

				// 쿨타임 중이라면 쿨타임 랜더링
				g_pIconMng->RenderTime( pos, ICON_HALF, pFTItemInfo );

				if( g_pDataMng->IsItemLock( TO_INVEN_SID( nSlotIndex ) ) == true )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_INVENTORY_ICON_SIZE );
				}
				// 전투중에 못먹는 아이템이라면 레드 마스크 랜더링
				else if( ( pFTItemInfo->s_nUseMode == 1 )&&( g_pCharMng->GetDigimonUser( 0 )->IsBattle() ) )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, IF_INVENTORY_ICON_SIZE );
				}
			}

			// 마스크 있다면 마스크 랜더링
			if( m_IFIcon[ i ].s_eState != sIFIcon::NONE )
			{			
				m_IFIcon[ i ].RenderMask( pos, IF_INVENTORY_ICON_SIZE );
			}

			// 새로운 아이템 획득 시
			if( GetSystem()->IsMapNewItem() && GetSystem()->IsNewItem( nSlotIndex ) )
			{
				m_pNewMark->Render( pos + ptGap );
			}
		}
		// 닫힌 인벤토리 슬롯
		else
		{
			// 아이템이 존재 하는가
			if( pItem->IsEnable() == true )
			{
				pFTItemInfo = GetSystem()->GetFTItemInfo( pItem->GetType() );
				SAFE_POINTER_CON( pFTItemInfo );

				// 아이템 랜더링 - 숫자 포함
				switch( pFTItemInfo->s_nType_L )
				{
				case nItem::Money:
				case nItem::GoldBar:	// 1bit 골드 바, 상점 처분 가격 비용으로 계산
					g_pIconMng->RenderItem( pItem, pos, false );
					break;
				default:
					g_pIconMng->RenderItem( pItem, pos, true );
				}

				// 쿨타임 중이라면 쿨타임 랜더링				
				g_pIconMng->RenderTime( pos, ICON_HALF, pFTItemInfo );

				if( g_pDataMng->IsItemLock( TO_INVEN_SID( nSlotIndex ) ) == true )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_INVENTORY_ICON_SIZE );
				}
				m_pBlockInItem->Render( pos );

				// 마스크 있다면 마스크 랜더링 - 아이템 있는거에 한해서
				if( m_IFIcon[ i ].s_eState != sIFIcon::NONE )
				{			
					m_IFIcon[ i ].RenderMask( pos, IF_INVENTORY_ICON_SIZE );
				}
			}
			else
			{
				m_pBlock->Render( pos );
			}
		}
	}
	g_pIconMng->RenderTime( m_pSort->GetDefaultPos() + GetRootClient(), 10.0f, g_pDataMng->GetSortCoolTime() );

	EndRenderScript();
	cSeparate::RenderInstance( GetWindowType(), m_nInvenIndex );
	cCreateName::RenderInstance( cBaseWindow::WT_CHANGE_D_NAME, m_nInvenIndex );
	cCreateName::RenderInstance( cBaseWindow::WT_CHANGE_T_NAME, m_nInvenIndex );
}
#else
void cInventory::Render()
{
	RenderScript();

	cItemInfo* pItemArray = g_pDataMng->GetInven()->GetDataArray();
	cItemInfo* pItem;
	CsItem::sINFO* pFTItemInfo;
	CsPoint pos;

	int nOpenInvenSlotCount = g_pDataMng->GetInven()->GetInvenSlotCount() - m_nDataStartIndex;
	for( int i=0; i<IF_INVENTORY_COUNT; ++i )
	{
		pItem = &pItemArray[ i + m_nDataStartIndex ];

		// 위치
		pos = m_IFIcon[ i ].GetPos() + GetRootClient();

		// 열린 인벤 토리인가
		if( i < nOpenInvenSlotCount )
		{
			if( pItem->IsEnable() == true )
			{
					if( nsCsFileTable::g_pItemMng == NULL )
						continue;
					CsItem* pInvenFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->GetType() );
					if( pInvenFTItem == NULL )
						continue;
					pFTItemInfo = pInvenFTItem->GetInfo();
					if( pFTItemInfo == NULL )
						continue;

				// 아이템 랜더링 - 숫자 포함
				switch( pFTItemInfo->s_nType_L )
				{
				case nItem::Money:
				case nItem::GoldBar:
					g_pIconMng->RenderItem( pItem, pos, false );
					break;				
				default:
					g_pIconMng->RenderItem( pItem, pos, true );
				}				

				// 쿨타임 중이라면 쿨타임 랜더링
				g_pIconMng->RenderTime( pos, ICON_HALF, pFTItemInfo );

				if( g_pDataMng->IsItemLock( TO_INVEN_SID( i + m_nDataStartIndex ) ) == true )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_INVENTORY_ICON_SIZE );
				}
				// 전투중에 못먹는 아이템이라면 레드 마스크 랜더링
				else if( ( pFTItemInfo->s_nUseMode == 1 )&&( g_pCharMng->GetDigimonUser( 0 )->IsBattle() ) )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, IF_INVENTORY_ICON_SIZE );
				}
			}

			// 마스크 있다면 마스크 랜더링
			if( m_IFIcon[ i ].s_eState != sIFIcon::NONE )
			{			
				m_IFIcon[ i ].RenderMask( pos, IF_INVENTORY_ICON_SIZE );
			}
		}
		// 닫힌 인벤토리 슬롯
		else
		{
			// 아이템이 존재 하는가
			if( pItem->IsEnable() == true )
			{
					if( nsCsFileTable::g_pItemMng == NULL )
						continue;
					CsItem* pInvenFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->GetType() );
					if( pInvenFTItem == NULL )
						continue;
					pFTItemInfo = pInvenFTItem->GetInfo();
					if( pFTItemInfo == NULL )
						continue;

				// 아이템 랜더링 - 숫자 포함
				switch( pFTItemInfo->s_nType_L )
				{
				case nItem::Money:
				case nItem::GoldBar:	// 1bit 골드 바, 상점 처분 가격 비용으로 계산
					g_pIconMng->RenderItem( pItem, pos, false );
					break;
				default:
					g_pIconMng->RenderItem( pItem, pos, true );
				}				

				// 쿨타임 중이라면 쿨타임 랜더링				
				g_pIconMng->RenderTime( pos, ICON_HALF, pFTItemInfo );

				if( g_pDataMng->IsItemLock( TO_INVEN_SID( i + m_nDataStartIndex ) ) == true )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_INVENTORY_ICON_SIZE );
				}
				m_pBlockInItem->Render( pos );

				// 마스크 있다면 마스크 랜더링 - 아이템 있는거에 한해서
				if( m_IFIcon[ i ].s_eState != sIFIcon::NONE )
				{			
					m_IFIcon[ i ].RenderMask( pos, IF_INVENTORY_ICON_SIZE );
				}
			}
			else
			{
				m_pBlock->Render( pos );
			}
		}
	}


	if( m_nInvenIndex == 0 )
		g_pIconMng->RenderTime( m_pSort->GetDefaultPos() + GetRootClient(), 10.0f, g_pDataMng->GetSortCoolTime() );

	EndRenderScript();

	cSeparate::RenderInstance( GetWindowType(), m_nInvenIndex );
	cCreateName::RenderInstance( cBaseWindow::WT_CHANGE_D_NAME, m_nInvenIndex );
	cCreateName::RenderInstance( cBaseWindow::WT_CHANGE_T_NAME, m_nInvenIndex );	
}
#endif

void cInventory::OnMouseOutWindow()
{
	cBaseWindow::OnMouseOutWindow();

	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
		return;

	if( CURSOR_ST.IsIcon() == true )
		return;

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pDataInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pDataInven );
	cItemInfo* pItem;

#ifdef UI_INVENTORY_RENEWAL
	SAFE_POINTER_RET( m_pScrollBar );
	int nCurScrollPos = m_pScrollBar->GetCurPosIndex();
	for( int i = 0; i < IF_INVENTORY_COUNT; ++i )
	{
		int nSlotIndex = i + nCurScrollPos * IF_INVENTORY_COL;
		if( m_bIsFiltering )
		{
			nSlotIndex = GetSystem()->GetFilteringItemIdx( nSlotIndex );
			if( -1 == nSlotIndex )
				continue;
		}			

		if( nSlotIndex >= nLimit::Inven )
			return;
#else
	for( int i=0; i<IF_INVENTORY_COUNT; ++i )
	{
		int nSlotIndex = i + m_nDataStartIndex;
#endif
		pItem = pDataInven->GetData( nSlotIndex );
		if( pItem->IsEnable() == false )
			continue;

		if( m_IFIcon[ i ].s_eState == sIFIcon::MOUSE_LCLICK )
		{
			if( m_IFIcon[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
			{
				if( g_pDataMng->IsItemLock( TO_INVEN_SID( nSlotIndex ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					return;
				}

				int nItemCount = pItem->GetCount();
				assert_cs( nItemCount > 0 );
				// 분할이 아니다. 또는 아이템 갯수가 1이면 무조건 분활이 아니다
				if( ( GLOBALINPUT_ST.IsShift() == false )||( nItemCount == 1 ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
					|| cData_Inven::IsTradeLimitedItem( pItem )
#endif
					)
				{
					CURSOR_ST.SetIcon( CURSOR_ICON::CI_INVEN, nSlotIndex, nItemCount, &m_IFIcon[ i ] );
					cSeparate::DeleteInstance( GetWindowType(), m_nInvenIndex );
				}
				// 분활
				else
				{					
					CsPoint posSeparate = m_IFIcon[ i ].GetPos() + GetRootClient();
					cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_D_NAME, m_nInvenIndex );
					cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_T_NAME, m_nInvenIndex );					
					cSeparate::NewInstance( GetWindowType(), m_nInvenIndex, posSeparate, 1, nItemCount, &m_IFIcon[ i ], nSlotIndex );
				}				
			}
			break;
		}
	}
}

void cInventory::OnLButtonUp( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );
	cItemInfo* pItem;
#ifdef UI_INVENTORY_RENEWAL
	SAFE_POINTER_RET( m_pScrollBar );
	int nCurScrollPos = m_pScrollBar->GetCurPosIndex();
	for( int i = 0; i < IF_INVENTORY_COUNT; ++i )
	{
		int nSlotIndex = i + nCurScrollPos * IF_INVENTORY_COL;
		if( m_bIsFiltering )
		{
			nSlotIndex = GetSystem()->GetFilteringItemIdx( nSlotIndex );
			if( -1 == nSlotIndex )
				continue;
		}

		if( nSlotIndex >= nLimit::Inven )
			return;
#else
	for( int i = 0; i < IF_INVENTORY_COUNT; ++i )
	{
		int nSlotIndex = i +m_nDataStartIndex;
#endif
		if( ( m_IFIcon[ i ].s_eState == sIFIcon::MOUSE_LCLICK )&&( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
#ifdef INVENCNT_CHECK
			if( nSlotIndex >= g_pDataMng->GetInven()->GetInvenSlotCount() )
				return;
#endif
			SAFE_POINTER_RET( g_pDataMng );
			cData_Inven* pInven = g_pDataMng->GetInven();
			SAFE_POINTER_RET( pInven );
			pItem = pInven->GetData( nSlotIndex );
			SAFE_POINTER_RET( pItem );
			if( pItem->IsEnable() == true )
			{
				if( g_pDataMng->IsItemLock( TO_INVEN_SID( nSlotIndex ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					m_IFIcon[ i ].ReleaseState();
					return;
				}

				int nItemCount = pItem->GetCount();
				assert_cs( nItemCount > 0 );
				// 분할이 아니다. 또는 아이템 갯수가 1이면 무조건 분활이 아니다
				if( ( GLOBALINPUT_ST.IsShift() == false )||( nItemCount == 1 ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
					|| cData_Inven::IsTradeLimitedItem( pItem )
#endif
					)
				{
					CURSOR_ST.SetIcon( CURSOR_ICON::CI_INVEN, nSlotIndex, nItemCount, &m_IFIcon[ i ] );
					cSeparate::DeleteInstance( GetWindowType(), m_nInvenIndex );
				}
				// 분활
				else
				{					
					CsPoint posSeparate = m_IFIcon[ i ].GetPos() + GetRootClient();
					cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_D_NAME, m_nInvenIndex );
					cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_T_NAME, m_nInvenIndex );					
					cSeparate::NewInstance( GetWindowType(), m_nInvenIndex, posSeparate, 1, nItemCount, &m_IFIcon[ i ], nSlotIndex );
				}
			}
		}

		m_IFIcon[ i ].ReleaseState();
	}
}

void cInventory::OnRButtonUp( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );
#ifdef UI_INVENTORY_RENEWAL
	SAFE_POINTER_RET( m_pScrollBar );
	int nCurScrollPos = m_pScrollBar->GetCurPosIndex();
	for( int i = 0; i < IF_INVENTORY_COUNT; ++i )
	{
		int nSlotIndex = i + nCurScrollPos * IF_INVENTORY_COL;
		if( m_bIsFiltering )
		{
			nSlotIndex = GetSystem()->GetFilteringItemIdx( nSlotIndex );
			if( -1 == nSlotIndex )
				continue;
		}

		if( nSlotIndex >= nLimit::Inven )
			return;
#else
	for( int i = 0; i < IF_INVENTORY_COUNT; ++i )
	{
		int nSlotIndex = i + m_nDataStartIndex;
#endif
		if( ( m_IFIcon[ i ].s_eState == sIFIcon::MOUSE_RCLICK )&&( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			GetSystem()->UseItem( nSlotIndex );
		}

		m_IFIcon[ i ].ReleaseState();
	}
}

bool cInventory::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
#ifdef UI_INVENTORY_RENEWAL
	SAFE_POINTER_RETVAL( m_pScrollBar, false );
	int nCurScrollPos = m_pScrollBar->GetCurPosIndex();
	for( int i = 0; i < IF_INVENTORY_COUNT; ++i )
	{
		int nSlotIndex = i + nCurScrollPos * IF_INVENTORY_COL;
		if( m_bIsFiltering )
		{
			nSlotIndex = GetSystem()->GetFilteringItemIdx( nSlotIndex );
			if( -1 == nSlotIndex )
				continue;
		}

		if( nSlotIndex >= nLimit::Inven )
			return false;
#else
	for( int i = 0; i < IF_INVENTORY_COUNT; ++i )
	{
		int nSlotIndex = i + m_nDataStartIndex;
#endif
		if( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) )
		{
			switch( eIconType )
			{			
			case CURSOR_ICON::CI_INVEN:
			case CURSOR_ICON::CI_WAREHOUSE:
			case CURSOR_ICON::CI_CARDINVEN:		// 카드 아이템을 이벤간 이동 가능 처리합니다.
#ifdef NEW_SHARESTASH
			case CURSOR_ICON::CI_SHARESTASH:
#endif
#ifdef CROSSWARS_SYSTEM
			case CURSOR_ICON::CI_CROSS:
#endif	
				if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
				{
					CURSOR_ST.ReleaseIcon();
					cPrintMsg::PrintMsg( 30028 );
					return false;
				}
				else
				{
					g_pDataMng->SendItemMove( nIconSlot, TO_INVEN_SID( nSlotIndex ), nIconCount );
				}
				break;

				// 같은 타입이 아니라면 빈곳에 넣는다.
			case CURSOR_ICON::CI_TEQUIP:
				{
					if( g_pDataMng->IsItemUse( nIconSlot ) == true )
					{
						cItemInfo* pTEquip = NULL;
						if( nIconSlot != SERVER_DATA_DIGIVICE_CONSTANT )
							pTEquip = g_pDataMng->GetTEquip()->GetData( TO_ID( nIconSlot ) );
						else
							pTEquip = g_pDataMng->GetTEquip()->GetDigiviceItem();

						cItemInfo* pInven = g_pDataMng->GetInven()->GetData( nSlotIndex );

						// 둘다 비여 있지 않다면
						if( ( pInven->IsEnable() )&&( pTEquip->IsEnable() ) )
						{
							SAFE_POINTER_CON( nsCsFileTable::g_pItemMng );
							CsItem* pSrcItem = nsCsFileTable::g_pItemMng->GetItem( pTEquip->GetType() );
							CsItem* pDestItem = nsCsFileTable::g_pItemMng->GetItem( pInven->GetType() );
							SAFE_POINTER_CON( pSrcItem );
							SAFE_POINTER_CON( pDestItem );
							CsItem::sINFO* pFTSrc = pSrcItem->GetInfo();
							CsItem::sINFO* pFTDest = pDestItem->GetInfo();
							SAFE_POINTER_CON( pFTSrc );
							SAFE_POINTER_CON( pFTDest );
#ifdef DEBUFF_ITEM
							if( (pFTSrc->s_nType_L == pFTDest->s_nType_L) && (pFTSrc->s_nSection == pFTDest->s_nSection) )
#else
							if(pFTSrc->s_nType_L == pFTDest->s_nType_L) 
#endif
								g_pDataMng->SendItemMove( nIconSlot, TO_INVEN_SID( nSlotIndex ), nIconCount );
							else
								g_pDataMng->SendItemMoveInven( nIconSlot, nIconCount );
						}
						// 둘중에 하나 비여 있다면 단순 이동
						else
						{
							g_pDataMng->SendItemMove( nIconSlot, TO_INVEN_SID( nSlotIndex ), nIconCount );
						}
					}
				}
				break;
			case CURSOR_ICON::CI_CHIPSET:
				{
					if( g_pDataMng->IsItemUse( nIconSlot ) == true )
					{
						cItemInfo* pChipset = g_pDataMng->GetDigivice()->GetChipset( TO_ID( nIconSlot ) );
						cItemInfo* pInven = g_pDataMng->GetInven()->GetData( nSlotIndex );

						// 둘다 비여 있지 않다면
						if( ( pInven->IsEnable() )&&( pChipset->IsEnable() ) )
						{
							SAFE_POINTER_CON( nsCsFileTable::g_pItemMng );
							CsItem* pSrcItem = nsCsFileTable::g_pItemMng->GetItem( pChipset->GetType() );
							CsItem* pDestItem = nsCsFileTable::g_pItemMng->GetItem( pInven->GetType() );
							SAFE_POINTER_CON( pSrcItem );
							SAFE_POINTER_CON( pDestItem );
							CsItem::sINFO* pFTSrc = pSrcItem->GetInfo();
							CsItem::sINFO* pFTDest = pDestItem->GetInfo();
							SAFE_POINTER_CON( pFTSrc );
							SAFE_POINTER_CON( pFTDest );
							if( pFTSrc->s_nType_L == pFTDest->s_nType_L )
								g_pDataMng->SendItemMove( nIconSlot, TO_INVEN_SID( nSlotIndex ), nIconCount );
							else
								g_pDataMng->SendItemMoveInven( nIconSlot, nIconCount );
						}
						// 둘중에 하나 비여 있다면 단순 이동
						else
						{
							g_pDataMng->SendItemMove( nIconSlot, TO_INVEN_SID( nSlotIndex ), nIconCount );
						}
					}
				}
				break;
			case CURSOR_ICON::CI_INCUBATOR:

				break;
			default:
				cPrintMsg::PrintMsg( 11014 );
				break;
			}

			m_IFIcon[ i ].ReleaseState();		//2016-03-07-nova	인벤토리 아이템 클릭으로 이동할시 초기화 해줌
			CURSOR_ST.ReleaseIcon();

			return true;
		}
	}
	return false;
}

void cInventory::ResetDevice()
{	
	ResetDeviceScript();

	if( m_pBlock )
		m_pBlock->ResetDevice();
	if( m_pBlockInItem )
		m_pBlockInItem->ResetDevice();
#ifdef UI_INVENTORY_RENEWAL
	if( m_pNewMark )
		m_pNewMark->ResetDevice();
#endif
}

void cInventory::NewInstTamerName( int nInvenIndex )
{
	CsPoint pos( -1, -1 );
	cSeparate::DeleteInstance( GetWindowType(), m_nInvenIndex );
	cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_D_NAME, m_nInvenIndex );

	cCreateName* pCName = cCreateName::NewInstance( cBaseWindow::WT_CHANGE_T_NAME, m_nInvenIndex, pos );
	if( pCName )
		pCName->SetValue1( nInvenIndex );

}

void cInventory::NewInstDigimonName( int nInvenIndex )
{
	CsPoint pos( -1, -1 );
	cSeparate::DeleteInstance( GetWindowType(), m_nInvenIndex );
	cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_T_NAME, m_nInvenIndex );	
	cCreateName* pCName = cCreateName::NewInstance( cBaseWindow::WT_CHANGE_D_NAME, m_nInvenIndex, pos );
	if( pCName )
		pCName->SetValue1( nInvenIndex );
}

CsPoint cInventory::GetInvenItemPos(int const & nInvenPos)
{
	if( nInvenPos < 0 || nInvenPos >= IF_INVENTORY_COUNT )
		return CsPoint(0,0);

	return	m_IFIcon[ nInvenPos ].GetPos() + GetRootClient();
}

bool cInventory::OnEscapeKey()
{
	GetSystem()->CloseWindow();
	return true;
}

#ifdef UI_INVENTORY_RENEWAL
void cInventory::_MakeFilterTab()
{
	CsPoint ptPos = CsPoint( 5, 32 );
	CsPoint ptSize = CsPoint( 64, 23 );

	m_pFilterTab = AddRadioButton( ptPos, ptSize, CsPoint( 0, 23 ), "NewInventory\\new_inventory_btn_tab.tga" );
	if( m_pFilterTab )
	{
		std::wstring wsUIText = _GetFilterName( eTabNone );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.SetText( wsUIText.c_str() );
		ti.s_eTextAlign = DT_CENTER;
		m_pFilterTab->SetCheckBoxText( &ti );
		m_pFilterTab->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &cInventory::OnClickFilterTab );
		m_pFilterTab->SetUserData( new sTabData( eTabNone ) );
		m_pFilterTab->SetTexNState( CsPoint( 0, 23 ), 2.0f, cCheckBox::CHECK_OK );

		ptPos.x += 65;
		wsUIText = _GetFilterName( eTabEquip );
		_AddFilterTab( eTabEquip, ptPos, ptSize, wsUIText );

		ptPos.x += 65;
		wsUIText = _GetFilterName( eTabConsume );
		_AddFilterTab( eTabConsume, ptPos, ptSize, wsUIText );

		ptPos.x += 65;
		wsUIText = _GetFilterName( eTabDigimon );
		_AddFilterTab( eTabDigimon, ptPos, ptSize, wsUIText );

		ptPos.x += 65;
		wsUIText = _GetFilterName( eTabEtc );
		_AddFilterTab( eTabEtc, ptPos, ptSize, wsUIText );

		m_pFilterTab->SetCheckIndex( 0 );
	}
}

void cInventory::_AddFilterTab(eTabType eTab, CsPoint pos, CsPoint size, std::wstring wsText)
{
	cRadioButton* pAddBtn = m_pFilterTab->AddNode( pos, size, CsPoint( 0, 23 ), "NewInventory\\new_inventory_btn_tab.tga" );
	if( pAddBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.SetText( wsText.c_str() );
		ti.s_eTextAlign = DT_CENTER;
		pAddBtn->SetCheckBoxText( &ti );
		pAddBtn->SetEnable( true );
		pAddBtn->SetUserData( new sTabData( eTab ) );
		pAddBtn->SetTexNState( CsPoint( 0, 23 ), 2.0f, cCheckBox::CHECK_OK );
	}
}

std::wstring cInventory::_GetFilterName(eTabType eTab)
{
	switch( eTab )
	{
	case eTabNone:
		return UISTRING_TEXT( "INVENTORY_FILTER_TEXT_NONE" );
	case eTabEquip:
		return UISTRING_TEXT( "INVENTORY_FILTER_TEXT_EQUIP" );
	case eTabConsume:
		return UISTRING_TEXT( "INVENTORY_FILTER_TEXT_CONSUME" );
	case eTabDigimon:
		return UISTRING_TEXT( "INVENTORY_FILTER_TEXT_DIGIMON" );
	case eTabEtc:
		return UISTRING_TEXT( "INVENTORY_FILTER_TEXT_ETC" );
	}

	return L"";
}

void cInventory::OnClickFilterTab(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cRadioButton* pButton = static_cast< cRadioButton* >( pData );
	sTabData* pUserData = dynamic_cast< sTabData* >( pButton->GetUserData() );
	SAFE_POINTER_RET( pUserData );

	if( GetSystem()->IsPlayingTutorial() )
	{
		m_pFilterTab->SetCheckIndex(0);
		return;
	}

	CURSOR_ST.ReleaseIcon();
	m_bIsFiltering = true;

	switch( pUserData->eType )
	{
	case eTabNone:
		GetSystem()->SetFilterMode( SystemType::eNone );
		m_bIsFiltering = false;
		break;
	case eTabEquip:
		GetSystem()->SetFilterMode( SystemType::eEquip );
		break;
	case eTabConsume:
		GetSystem()->SetFilterMode( SystemType::eConsume );
		break;
	case eTabDigimon:
		GetSystem()->SetFilterMode( SystemType::eDigimon );
		break;
	case eTabEtc:
		GetSystem()->SetFilterMode( SystemType::eEtc );
		break;
	}

	m_pScrollBar->SetCurPosIndex( 0 );
}

void cInventory::_SetNewMark(int nIndex, bool bIsNew)
{
	SAFE_POINTER_RET( m_pScrollBar );
	int nSlotIndex = nIndex + m_pScrollBar->GetCurPosIndex() * IF_INVENTORY_COL;
	if( m_bIsFiltering )
		nSlotIndex = GetSystem()->GetFilteringItemIdx(nIndex);

	if( -1 == nSlotIndex )
		return;

	GetSystem()->SetNewItem( nSlotIndex, bIsNew );
}

#endif
