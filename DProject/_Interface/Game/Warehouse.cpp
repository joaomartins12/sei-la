
#include "stdafx.h"
#include "Warehouse.h"
#include "..\..\..\LibProj\CsFunc\CrashLogger.h"

#define IF_WAREHOUSE_ICONSIZE	CsPoint( 32, 32 )

bool cWarehouse::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void cWarehouse::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 
}

void cWarehouse::Destroy()
{
	cBaseWindow::Delete();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cWarehouse::DeleteResource()
{
	cSeparate::DeleteInstance( GetWindowType(), 0 );
#ifdef NEW_SHARESTASH
	cSeparate::DeleteInstance( GetWindowType(), 1 );
#endif
	DeleteScript();

	SAFE_NIDELETE( m_pBlock );
	SAFE_NIDELETE( m_pBlockInItem );
}

void cWarehouse::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

#ifdef NEW_SHARESTASH
	InitScript( "Warehouse\\new_Win.tga", CsPoint( 230, 175 ), CsPoint( 294, 527 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP );
#else
	InitScript( "Warehouse\\Win.tga", CsPoint( 150, 165 ), CsPoint( 294, 407 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
#endif
	AddTitle( UISTRING_TEXT( "WAREHOUSE_TITLE_STORAGE" ).c_str() );

	m_pBlock = NiNew cSprite;
	m_pBlock->Init( NULL, CsPoint::ZERO, IF_WAREHOUSE_ICONSIZE, "Inventory\\Block.tga", false );

	m_pBlockInItem = NiNew cSprite;
	m_pBlockInItem->Init( NULL, CsPoint::ZERO, IF_WAREHOUSE_ICONSIZE, "Inventory\\Block.tga", false );
	m_pBlockInItem->SetAlpha( 0.6f );

	m_pCancelButton = AddButton( CsPoint( 272, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 266, 37 ), CsPoint( 16, 359 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, m_ptRootSize ), IF_WAREHOUSE_Y );	
	assert_cs( (nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxWareHouse % IF_WAREHOUSE_X) == 0 );
	m_pScrollBar->SetRange( CsPoint( 0, nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxWareHouse / IF_WAREHOUSE_X ) );

#ifdef WAREHOUSESORT
	#ifndef NEW_WAREHOUSESORT
		m_InvenSortSeq.SetEndTime( 20.0f );
		m_InvenSortSeq.Reset();
	#endif

	#ifdef NEW_WAREHOUSESORT
		m_pWareHouseSort = AddButton( CsPoint( 10, 5 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "inventory\\Invensort.tga" );
	#else
		m_pWareHouseSort = AddButton( CsPoint( 5, 4 ), CsPoint( 33, 21 ), CsPoint( 0, 21 ), "inventory\\Invensort.tga" );
	#endif
#endif

#ifdef NEW_SHARESTASH
	m_pShareStashSort = AddButton( CsPoint ( 10, 412 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "inventory\\Invensort.tga" );
	assert_cs( (nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxShareStash % IF_WAREHOUSE_X) == 0 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( UISTRING_TEXT( "WAREHOUSE_ACCOUNT_STORAGE" ).c_str() );
	AddText( &ti, CsPoint( 294 / 2, 414 ) );
#endif
	m_pTarget = NULL;

	CsPoint pos( 9, 39 );
	for( int y=0; y<IF_WAREHOUSE_Y; ++y )
	{
		for( int x=0; x<IF_WAREHOUSE_X; ++x )
		{
			m_IFIcon[ y*IF_WAREHOUSE_X + x ].Init( CsRect( pos, (CsSIZE)IF_WAREHOUSE_ICONSIZE ) );
			pos.x += 36;
		}
		pos.x = 9;
		pos.y += 36;
	}

#ifdef NEW_SHARESTASH
	pos = CsPoint( 9, 446 );
	for( int y=0; y<IF_SHARESTASH_Y; ++y )
	{
		for(int  x=0; x<IF_SHARESTASH_X; ++x )
		{
			m_IFIcon_ex[ y*IF_SHARESTASH_X + x ].Init( CsRect( pos, (CsSIZE)IF_WAREHOUSE_ICONSIZE ) );
			pos.x += 36;
		}
		pos.x = 9;
		pos.y += 36;
	}
#endif
}

void cWarehouse::Update(float const& fDeltaTime)
{
	cSeparate::UpdateInstance( GetWindowType(), 0 );
#ifdef NEW_SHARESTASH
	cSeparate::UpdateInstance( GetWindowType(), 1 );	// 계정 창고 분할창 업데이트
#endif
	if( m_pTarget != NULL )
	{
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_WAREHOUSE );

		// 테이머와의 거리 체크
		assert_cs( m_pTarget->GetLeafRTTI() == RTTI_NPC );
		if( ( (CNpc*)m_pTarget )->CheckTamerDist() == false )
		{
			Close();
			return;
		}
	}
	else
	{
		float fCheckDist = IF_TARGET_TAMER_MAX_DIST*2.0;
		if( ( m_ptTargetPos - g_pCharMng->GetTamerUser()->GetPos2D() ).Length() > fCheckDist )
		{
			Close();
			return;
		}

		if( g_pCharMng->GetDigimonUser( 0 )->IsBattle() )
		{
			Close();
			return;
		}
	}

#ifdef WAREHOUSESORT
#ifndef NEW_WAREHOUSESORT
	if( m_SortSeq.IsCoolTime() == true ||		
		g_pDataMng->IsItemAllLock() == true ||
		g_pDataMng->IsSort() == true ) 
	{
		m_pWareHouseSort->SetEnable( false );
	}
	else
	{
		m_pWareHouseSort->SetEnable( true );
	}
#endif

#ifdef WAREHOUSE_SERVER_SORT
	if( GetSystem()->GetWareHouseSortCoolTime()->IsCoolTime() == true )
		m_pWareHouseSort->SetEnable( false );
	else
		m_pWareHouseSort->SetEnable( true );
	
	if( GetSystem()->GetShareStashSortCoolTime()->IsCoolTime() == true )
		m_pShareStashSort->SetEnable( false );
	else
		m_pShareStashSort->SetEnable( true );
#else //WAREHOUSE_SERVER_SORT
#ifdef NEW_WAREHOUSESORT		
	if( 
		GetSystem()->GetWareHouseSortCoolTime()->IsCoolTime() == true ||
//		g_pDataMng->GetWareHouseSortCoolTime()->IsCoolTime() == true ||
#ifdef NEW_SHARESTASH
		GetSystem()->GetShareStashSortCoolTime()->IsCoolTime() == true ||
//		g_pDataMng->GetShareStashSortCoolTime()->IsCoolTime() == true ||
		GetSystem()->IsShareStashSort() == true ||
#endif
		//g_pGameIF->IsOpenNpcWindow() == true ||
		g_pDataMng->IsItemAllLock() == true ||
		//g_pCharMng->GetDigimonUser( 0 )->IsBattle() == true ||
		GetSystem()->IsWareHouseSort() == true ) 
	{
		m_pWareHouseSort->SetEnable( false );
#ifdef NEW_SHARESTASH
		m_pShareStashSort->SetEnable( false );
#endif
	}
	else
	{
		m_pWareHouseSort->SetEnable( true );
#ifdef NEW_SHARESTASH
		m_pShareStashSort->SetEnable( true );
#endif
	}	
#endif

#endif

#endif	//WAREHOUSE_SERVERSORT

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cWarehouse::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cSeparate::Update_ForMouseInstance( GetWindowType(), 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;
#ifdef NEW_SHARESTASH
	muReturn = cSeparate::Update_ForMouseInstance( GetWindowType(), 1 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;
#endif
	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	// 스크롤바
	if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;
	default:
		return muReturn;
	}

#ifdef WAREHOUSESORT	 
	switch( m_pWareHouseSort->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
#ifndef NEW_WAREHOUSESORT
		g_pDataMng->SendWareHouseSort();
#endif
#ifdef NEW_WAREHOUSESORT
		GetSystem()->WareHouseSort();
		GetSystem()->GetWareHouseSortCoolTime()->Start();
#endif
#ifdef NEW_SHARESTASH
	#ifdef WAREHOUSE_SERVER_SORT
	#else
		GetSystem()->GetShareStashSortCoolTime()->Start();
	#endif
#endif

		return muReturn;
	default:
		return muReturn;
	}	
#ifdef NEW_SHARESTASH
	switch( m_pShareStashSort->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		GetSystem()->ShareStashSort();
#ifdef WAREHOUSE_SERVER_SORT
#else
		GetSystem()->GetWareHouseSortCoolTime()->Start();
#endif
		GetSystem()->GetShareStashSortCoolTime()->Start();
		return muReturn;
	default:
		return muReturn;
	}
#endif
#endif

	if( _UpdateIcon_ForMouse() == true )
		return muReturn;	

	_UpdateMoveWindow_ForMouse();	

	return muReturn;
}

bool cWarehouse::_UpdateIcon_ForMouse()
{
	bool bCheck = false;
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );


	switch( eCursor )
	{	
	case CURSOR::LBUTTON_DOWN:		
		{
			for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
			{
				if( ( bCheck == false )&&( m_IFIcon[ wc ].MouseLBtnClick( ptMouseLocalPos ) == true ) )
				{
					bCheck = true;
				}
				else
				{
					m_IFIcon[ wc ].ReleaseState();
				}
			}
#ifdef NEW_SHARESTASH
			for(int  wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
			{
				if( ( bCheck == false )&&( m_IFIcon_ex[ wc ].MouseLBtnClick( ptMouseLocalPos ) == true ) )
				{
					bCheck = true;
				}
				else
				{
					m_IFIcon_ex[ wc ].ReleaseState();
				}
			}
#endif			
		}
		break;
	case CURSOR::RBUTTON_DOWN:
		{
			for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
			{
				if( ( bCheck == false )&&( m_IFIcon[ wc ].MouseRBtnClick( ptMouseLocalPos ) == true ) )
				{					
					bCheck = true;
				}
				else
				{
					m_IFIcon[ wc ].ReleaseState();
				}
			}
#ifdef NEW_SHARESTASH
			for(int  wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
			{
				if( ( bCheck == false )&&( m_IFIcon_ex[ wc ].MouseRBtnClick( ptMouseLocalPos ) == true ) )
				{					
					bCheck = true;
				}
				else
				{
					m_IFIcon_ex[ wc ].ReleaseState();
				}
			}
#endif
		}
		break;
	case CURSOR::LBUTTON_PRESS:
		{
			OnMouseOutWindow();

			for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
			{
				if( m_IFIcon[ wc ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( ( bCheck == false )&&( m_IFIcon[ wc ].MouseOn( ptMouseLocalPos ) == true ) )
						bCheck = true;
					else
						m_IFIcon[ wc ].ReleaseState();
				}
			}
#ifdef NEW_SHARESTASH
			for(int  wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
			{
				if( m_IFIcon_ex[ wc ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( ( bCheck == false )&&( m_IFIcon_ex[ wc ].MouseOn( ptMouseLocalPos ) == true ) )
						bCheck = true;
					else
						m_IFIcon_ex[ wc ].ReleaseState();
				}
			}
#endif	
		}
		break;			
	case CURSOR::RBUTTON_PRESS:
		{
			for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
			{
				if( m_IFIcon[ wc ].s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( ( bCheck == false )&&( m_IFIcon[ wc ].MouseOn( ptMouseLocalPos ) == true ) )
						bCheck = true;
					else
						m_IFIcon[ wc ].ReleaseState();
				}
			}
#ifdef NEW_SHARESTASH
			for(int  wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
			{
				if( m_IFIcon_ex[ wc ].s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( ( bCheck == false )&&( m_IFIcon_ex[ wc ].MouseOn( ptMouseLocalPos ) == true ) )
						bCheck = true;
					else
						m_IFIcon_ex[ wc ].ReleaseState();
				}
			}
#endif
		}
		break;
	case CURSOR::LBUTTON_UP:
	case CURSOR::RBUTTON_UP:
	case CURSOR::BUTTON_NONE:
		{			
			for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
			{
				if( ( bCheck == false )&&( m_IFIcon[ wc ].MouseOn( ptMouseLocalPos ) == true ) )
				{					
					bCheck = true;
				}
				else
				{
					m_IFIcon[ wc ].ReleaseState();
				}
			}
#ifdef NEW_SHARESTASH
			for(int  wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
			{
				if( ( bCheck == false )&&( m_IFIcon_ex[ wc ].MouseOn( ptMouseLocalPos ) == true ) )
				{					
					bCheck = true;
				}
				else
				{
					m_IFIcon_ex[ wc ].ReleaseState();
				}
			}
#endif
		}	
		break;
	}


	// 툴팁용
	{
		 
		for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
		{
			int nSlotIndex = wc + m_pScrollBar->GetCurPosIndex()*IF_WAREHOUSE_X;
			
			cItemInfo const* pData = GetSystem()->GetData( nSlotIndex );

			SAFE_POINTER_CON(pData);

			if( pData->IsEnable() == false )
				continue;

			if( m_IFIcon[ wc ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{				
				cTooltip::eSLOT eSlot = ( nSlotIndex < GetSystem()->GetSlotCount() ) ? cTooltip::OPEN_SLOT : cTooltip::CLOSE_SLOT;
				CsPoint pos = m_IFIcon[ wc ].GetPos() + GetRootClient();
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_WAREHOUSE_ICONSIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->GetType(), cBaseWindow::WT_WAREHOUSE, eSlot, 0, (void*)pData );

				//////////////////////////////////////////////////////////
				//2017-01-19-nova 장착중인 아이템을 비교창에 출력
					SAFE_POINTER_CON( nsCsFileTable::g_pItemMng );
					CsItem* pWhItem = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() );
					SAFE_POINTER_CON( pWhItem );
					CsItem::sINFO* pWhInfo = pWhItem->GetInfo();
					SAFE_POINTER_CON( pWhInfo );
					int nItemType = pWhInfo->s_nType_L;

				for( int i=0; i<nLimit::Equip; ++i )
				{
						SAFE_POINTER_CON( g_pDataMng );
						SAFE_POINTER_CON( g_pDataMng->GetTEquip() );
						cItemInfo* PlayerpData = g_pDataMng->GetTEquip()->GetData( i );
						SAFE_POINTER_CON( PlayerpData );

						if(PlayerpData->m_nType != 0)
						{
							CsItem* pEquipItem = nsCsFileTable::g_pItemMng->GetItem( PlayerpData->GetType() );
							SAFE_POINTER_CON( pEquipItem );
							CsItem::sINFO* pFTSrc = pEquipItem->GetInfo();
							SAFE_POINTER_CON( pFTSrc );

						if(nItemType == pFTSrc->s_nType_L)
							TOOLTIPMNG_STPTR->GetCompareTooltip()->SetCompareTooltip( pos, IF_WAREHOUSE_ICONSIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, PlayerpData->GetType(), cBaseWindow::WT_WAREHOUSE, eSlot, 0, PlayerpData );
					}
				}	
				//////////////////////////////////////////////////////////
				break;
			}
		}
#ifdef NEW_SHARESTASH
		cItemInfo* pShareData = NULL;
		for(int  wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
		{
			pShareData = GetSystem()->GetShareData(wc);

			SAFE_POINTER_CON(pShareData);

			if( pShareData->IsEnable() == false )
				continue;

			if( m_IFIcon_ex[ wc ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				cTooltip::eSLOT eSlot = ( wc < GetSystem()->GetShareSlotCount() ) ? cTooltip::OPEN_SLOT : cTooltip::CLOSE_SLOT;
				CsPoint pos = m_IFIcon_ex[ wc ].GetPos() + GetRootClient();
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_WAREHOUSE_ICONSIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pShareData->GetType(), cBaseWindow::WT_WAREHOUSE, eSlot, 0, (void*)pShareData );
				break;
			}
		}
#endif
	}	

	return bCheck;
}

void cWarehouse::Render()
{
	RenderScript();

	cItemInfo* pData ;	
	CsItem* pTableItem = NULL;
	CsItem::sINFO* pTableInfo = NULL;
	CsPoint pos;

	CsPoint tmpIconSize = IF_WAREHOUSE_ICONSIZE;
	float fHalf = (float)tmpIconSize.x/2.0f;//쿨타임용

	int nOpenSlotCount = GetSystem()->GetSlotCount();
	for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
	{
		int nSlotIndex = wc + m_pScrollBar->GetCurPosIndex()*IF_WAREHOUSE_X;		
		// 위치
		pos = m_IFIcon[ wc ].GetPos() + GetRootClient();

		// 사용 가능한 슬롯인가
		if( nSlotIndex < nOpenSlotCount )
		{
			pData = GetSystem()->GetData( nSlotIndex ) ;

			// 아이템이 존재 하는가
			if( pData && pData->IsEnable() == true )
			{
				// 아이템 랜더링 - 숫자 포함
				g_pIconMng->RenderItem( pData, pos, true );

				// 쿨타임 중이라면 쿨타임 랜더링
				pTableItem = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() );
				pTableInfo = pTableItem ? pTableItem->GetInfo() : NULL;
				if( pTableInfo )
					g_pIconMng->RenderTime( pos, fHalf, pTableInfo );

				if( g_pDataMng->IsItemLock( TO_WAREHOUSE_SID( nSlotIndex ) ) == true )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_WAREHOUSE_ICONSIZE );
				}
			}

			// 마스크 있다면 마스크 랜더링
			if( m_IFIcon[ wc ].s_eState != sIFIcon::NONE )
			{			
				m_IFIcon[ wc ].RenderMask( pos, IF_WAREHOUSE_ICONSIZE );
			}
		}
		// 사용 불가능 슬롯 - 뺄수는 있데 집어 넣지는 못한다.
		else
		{
			if( nSlotIndex < nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxWareHouse )
			{
				pData = GetSystem()->GetData( nSlotIndex ) ;
				// 아이템이 존재 하는가
				if( pData && pData->IsEnable() == true )
				{
					// 아이템 랜더링 - 숫자 포함
					g_pIconMng->RenderItem( pData, pos, true );

					// 쿨타임 중이라면 쿨타임 랜더링
					pTableItem = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() );
					pTableInfo = pTableItem ? pTableItem->GetInfo() : NULL;
					if( pTableInfo )
						g_pIconMng->RenderTime( pos, fHalf, pTableInfo );

					if( g_pDataMng->IsItemLock( TO_WAREHOUSE_SID( nSlotIndex ) ) == true )
					{
						g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_WAREHOUSE_ICONSIZE );
					}

					m_pBlockInItem->Render( pos );

					// 마스크 있다면 마스크 랜더링 - 아이템 있는거에 한해서
					if( m_IFIcon[ wc ].s_eState != sIFIcon::NONE )
					{			
						m_IFIcon[ wc ].RenderMask( pos, IF_WAREHOUSE_ICONSIZE );
					}
				}
				else
				{
					m_pBlock->Render( pos );
				}
			}
			else
			{
				m_pBlock->Render( pos );
			}
		}
	}


#ifdef NEW_SHARESTASH
	cItemInfo* pShareData = NULL; 
	for(int wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
	{		
		// 위치
		pos = m_IFIcon_ex[ wc ].GetPos() + GetRootClient();	//

		pData = GetSystem()->GetShareData(wc);//&pShareArray[ wc ];//

		// 아이템이 존재 하는가
		if( pData && pData->IsEnable() == true )//
		{
			// 아이템 랜더링 - 숫자 포함
			g_pIconMng->RenderItem( pData, pos, true );//

			// 쿨타임 중이라면 쿨타임 랜더링
			pTableItem = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() );
			pTableInfo = pTableItem ? pTableItem->GetInfo() : NULL;
			if( pTableInfo )
				g_pIconMng->RenderTime( pos, fHalf, pTableInfo );

			if( g_pDataMng->IsItemLock( TO_SHARESTASH_SID( wc ) ) == true )
			{
				g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_WAREHOUSE_ICONSIZE );
			}
		}

		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_ex[ wc ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_ex[ wc ].RenderMask( pos, IF_WAREHOUSE_ICONSIZE );
		}				
	}

#endif

	fHalf = 10.0f;

#ifdef NEW_WAREHOUSESORT
	g_pIconMng->RenderTime( m_pWareHouseSort->GetDefaultPos() + GetRootClient(), fHalf,GetSystem()->GetWareHouseSortCoolTime() );
#endif
#ifdef NEW_SHARESTASH
	g_pIconMng->RenderTime( m_pShareStashSort->GetDefaultPos() + GetRootClient(), fHalf, 		GetSystem()->GetShareStashSortCoolTime()/*g_pDataMng->GetShareStashSortCoolTime()*/ );
#endif
	EndRenderScript();
	cSeparate::RenderInstance( GetWindowType(), 0 );
#ifdef NEW_SHARESTASH
	cSeparate::RenderInstance( GetWindowType(), 1 );	// 계정 창고	
#endif
}


void cWarehouse::OnMouseOutWindow()
{
	cBaseWindow::OnMouseOutWindow();

	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
		return;

	if( CURSOR_ST.IsIcon() == true )
		return;

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	cItemInfo* pData = NULL;
	for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
	{
		int nSlotIndex = wc + m_pScrollBar->GetCurPosIndex()*IF_WAREHOUSE_X;
		pData = GetSystem()->GetData( nSlotIndex );
		SAFE_POINTER_CON( pData );
		if( pData->IsEnable() == false )
			continue;

		if( m_IFIcon[ wc ].s_eState == sIFIcon::MOUSE_LCLICK )
		{
			if( m_IFIcon[ wc ].s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
			{
				if( g_pDataMng->IsItemLock( TO_WAREHOUSE_SID( nSlotIndex ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					return;
				}

				int nCount = pData->GetCount();
				assert_cs( nCount > 0 );
				// 분할이 아니다. 또는 아이템 갯수가 1이면 무조건 분활이 아니다
				if( ( GLOBALINPUT_ST.IsShift() == false )||( nCount == 1 )
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
					|| cData_Inven::IsTradeLimitedItem( pData )
#endif
				)
				{
					CURSOR_ST.SetIcon( CURSOR_ICON::CI_WAREHOUSE, nSlotIndex, nCount, &m_IFIcon[ wc ] );
					cSeparate::DeleteInstance( GetWindowType(), 0 );
				}
				// 분활
				else
				{
					CsPoint posSeparate = m_IFIcon[ wc ].GetPos() + GetRootClient();
					cSeparate::NewInstance( GetWindowType(), 0, posSeparate, 1, nCount, &m_IFIcon[ wc ], nSlotIndex );
				}				
			}
			break;
		}
	}

#ifdef NEW_SHARESTASH
	for( int wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
	{		
		pData = GetSystem()->GetShareData( wc );
		SAFE_POINTER_CON( pData );
		if( pData->IsEnable() == false )
			continue;

		if( m_IFIcon_ex[ wc ].s_eState == sIFIcon::MOUSE_LCLICK )
		{
			if( m_IFIcon_ex[ wc ].s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
			{
				if( g_pDataMng->IsItemLock( TO_SHARESTASH_SID( wc ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					return;
				}

				int nCount = pData->GetCount();
				assert_cs( nCount > 0 );
				// 분할이 아니다. 또는 아이템 갯수가 1이면 무조건 분활이 아니다
				if( ( GLOBALINPUT_ST.IsShift() == false )||( nCount == 1 ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
					|| cData_Inven::IsTradeLimitedItem( pData )
#endif
				)
				{
					CURSOR_ST.SetIcon( CURSOR_ICON::CI_SHARESTASH, wc, nCount, &m_IFIcon_ex[ wc ] );
					cSeparate::DeleteInstance( GetWindowType(), 1 );
				}
				// 분활
				else
				{
					CsPoint posSeparate = m_IFIcon_ex[ wc ].GetPos() + GetRootClient();
					cSeparate::NewInstance( GetWindowType(), 1, posSeparate, 1, nCount, &m_IFIcon_ex[ wc ], wc );
				}				
			}
			break;
		}
	}
#endif
}

void cWarehouse::SetTarget( CsC_AvObject* pTarget )
{
	assert_cs( pTarget != NULL ); m_pTarget = pTarget;
}

void cWarehouse::SetTargetPos( NiPoint2 pos )
{
	assert_cs( m_pTarget == NULL ); m_ptTargetPos = pos;
}

void cWarehouse::OnLButtonUp( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );
	cItemInfo* pData = NULL;
	for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
	{
		if( ( m_IFIcon[ wc ].s_eState == sIFIcon::MOUSE_LCLICK )&&( m_IFIcon[ wc ].s_Rect.PtInRect( ptLocal ) ) )
		{
			int nSlotIndex = wc + m_pScrollBar->GetCurPosIndex()*IF_WAREHOUSE_X;
			pData = GetSystem()->GetData( nSlotIndex );
			if( pData->IsEnable() == true )
			{
				if( g_pDataMng->IsItemLock( TO_WAREHOUSE_SID( nSlotIndex ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					m_IFIcon[ wc ].ReleaseState();
					return;
				}

				int nItemCount = pData->GetCount();
				assert_cs( nItemCount > 0 );
				// 분할이 아니다. 또는 아이템 갯수가 1이면 무조건 분활이 아니다
				assert_cs( nItemCount != 0 );
				if( ( GLOBALINPUT_ST.IsShift() == false )||( nItemCount == 1 ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
					|| cData_Inven::IsTradeLimitedItem( pData )
#endif
					)
				{
					CURSOR_ST.SetIcon( CURSOR_ICON::CI_WAREHOUSE, nSlotIndex, nItemCount, &m_IFIcon[ wc ] );
					cSeparate::DeleteInstance( GetWindowType(), 0 );
				}
				// 분활
				else
				{					
					CsPoint posSeparate = m_IFIcon[ wc ].GetPos() + GetRootClient();
					cSeparate::NewInstance( GetWindowType(), 0, posSeparate, 1, nItemCount, &m_IFIcon[ wc ], nSlotIndex );
				}
			}
		}

		m_IFIcon[ wc ].ReleaseState();
	}

#ifdef NEW_SHARESTASH
	for( int wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
	{
		if( ( m_IFIcon_ex[ wc ].s_eState == sIFIcon::MOUSE_LCLICK )&&( m_IFIcon_ex[ wc ].s_Rect.PtInRect( ptLocal ) ) )
		{			
			pData = GetSystem()->GetShareData( wc );
			if( pData->IsEnable() == true )
			{
				if( g_pDataMng->IsItemLock( TO_SHARESTASH_SID( wc ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					m_IFIcon_ex[ wc ].ReleaseState();
					return;
				}

				int nItemCount = pData->GetCount();
				assert_cs( nItemCount > 0 );
				// 분할이 아니다. 또는 아이템 갯수가 1이면 무조건 분활이 아니다
				assert_cs( nItemCount != 0 );
				if( ( GLOBALINPUT_ST.IsShift() == false )||( nItemCount == 1 ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
					|| cData_Inven::IsTradeLimitedItem( pData )
#endif
				)
				{
					CURSOR_ST.SetIcon( CURSOR_ICON::CI_SHARESTASH, wc, nItemCount, &m_IFIcon_ex[ wc ] );
					cSeparate::DeleteInstance( GetWindowType(), 1 );
				}
				// 분활
				else
				{					
					CsPoint posSeparate = m_IFIcon_ex[ wc ].GetPos() + GetRootClient();
					cSeparate::NewInstance( GetWindowType(), 1, posSeparate, 1, nItemCount, &m_IFIcon_ex[ wc ], wc );
				}
			}
		}

		m_IFIcon_ex[ wc ].ReleaseState();
	}
#endif
}

void cWarehouse::OnRButtonUp( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );
	for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
	{
		if( ( m_IFIcon[ wc ].s_eState == sIFIcon::MOUSE_RCLICK )&&( m_IFIcon[ wc ].s_Rect.PtInRect( ptLocal ) ) )
		{
			int nSlotIndex = wc + m_pScrollBar->GetCurPosIndex()*IF_WAREHOUSE_X;
			if( GetSystem()->GetData( nSlotIndex )->IsEnable() == true )
			{
				if( g_pDataMng->IsItemLock( TO_WAREHOUSE_SID( nSlotIndex ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					m_IFIcon[ wc ].ReleaseState();
					return;
				}
				cSeparate::DeleteInstance( GetWindowType(), 0 );
				g_pDataMng->SendItemMoveInven( TO_WAREHOUSE_SID( nSlotIndex ), 0 );
			}
		}
		m_IFIcon[ wc ].ReleaseState();
	}
#ifdef NEW_SHARESTASH
	for( int wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
	{
		if( ( m_IFIcon_ex[ wc ].s_eState == sIFIcon::MOUSE_RCLICK )&&( m_IFIcon_ex[ wc ].s_Rect.PtInRect( ptLocal ) ) )
		{			
			if( GetSystem()->GetShareData( wc )->IsEnable() == true )
			{
				if( g_pDataMng->IsItemLock( TO_SHARESTASH_SID( wc ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					m_IFIcon_ex[ wc ].ReleaseState();
					return;
				}
				cSeparate::DeleteInstance( GetWindowType(), 1 );
				g_pDataMng->SendItemMoveInven( TO_SHARESTASH_SID( wc ), 0 );				
			}
		}
		m_IFIcon_ex[ wc ].ReleaseState();
	}
#endif

}

#ifdef NEW_SHARESTASH
bool cWarehouse::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
	for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
	{
		if( m_IFIcon[ wc ].s_Rect.PtInRect( ptLocal ) )
		{
			int nSlotIndex = wc + m_pScrollBar->GetCurPosIndex()*IF_WAREHOUSE_X;
			if( g_pDataMng->IsItemLock( TO_WAREHOUSE_SID( nSlotIndex ) ) == false )
			{
				switch( eIconType )
				{
				case CURSOR_ICON::CI_INVEN:	// 인밴에서 왔다.
					{						
						MoveItemItoW( TO_ID( nIconSlot ), nSlotIndex, nIconCount );
					}
					break;
				case CURSOR_ICON::CI_WAREHOUSE:	// 같은 곳에서 왔다.
					{
						MoveItemWtoW( TO_ID( nIconSlot ), nSlotIndex, nIconCount );					
					}
					break;
				case CURSOR_ICON::CI_SHARESTASH:	// 공유 창고에서 왔다.
					{
						MoveItemStoW( TO_ID( nIconSlot ), nSlotIndex, nIconCount );					
					}
					break;
				default:
					cPrintMsg::PrintMsg( 11018 );
				}
			}
			else
			{
				cPrintMsg::PrintMsg( 10004 );				
			}
			m_IFIcon[ wc ].ReleaseState();
			CURSOR_ST.ReleaseIcon();
			return true;
		}
	}

	for( int wc=0; wc<IF_SHARESTASH_COUNT; ++wc )
	{
		if( m_IFIcon_ex[ wc ].s_Rect.PtInRect( ptLocal ) )
		{			
			if( g_pDataMng->IsItemLock( TO_SHARESTASH_SID( wc ) ) == false )
			{
				switch( eIconType )
				{
				case CURSOR_ICON::CI_INVEN:	// 인밴에서 왔습니다..
					{						
						MoveItemItoS( TO_ID( nIconSlot ), wc, nIconCount );
					}
					break;
				case CURSOR_ICON::CI_WAREHOUSE:	// 창고에서 왔습니다.~~
					{
						MoveItemWtoS( TO_ID( nIconSlot ), wc, nIconCount );				
					}
					break;
				case CURSOR_ICON::CI_SHARESTASH: // 같은 곳에서 왔다.
					{
						MoveItemStoS( TO_ID( nIconSlot ), wc, nIconCount );					
					}
					break;
				default:
					cPrintMsg::PrintMsg( 11018 );
				}
			}
			else
			{
				cPrintMsg::PrintMsg( 10004 );				
			}
			m_IFIcon_ex[ wc ].ReleaseState();
			CURSOR_ST.ReleaseIcon();
			return true;
		}
	}

	return false;
}

#else

bool cWarehouse::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
	for( int wc=0; wc<IF_WAREHOUSE_COUNT; ++wc )
	{
		if( m_IFIcon[ wc ].s_Rect.PtInRect( ptLocal ) )
		{
			int nSlotIndex = wc + m_pScrollBar->GetCurPosIndex()*IF_WAREHOUSE_X;
			if( g_pDataMng->IsItemLock( TO_WAREHOUSE_SID( nSlotIndex ) ) == false )
			{
				switch( eIconType )
				{
				case CURSOR_ICON::CI_INVEN:
					{
						SetResist( TO_ID( nIconSlot ), nSlotIndex, nIconCount );					
					}
					break;
				case CURSOR_ICON::CI_WAREHOUSE:
					{
						MoveItem( TO_ID( nIconSlot ), nSlotIndex, nIconCount );					
					}
					break;
				default:
					cPrintMsg::PrintMsg( 11018 );
				}
			}
			else
			{
				cPrintMsg::PrintMsg( 10004 );				
			}
			m_IFIcon[ wc ].ReleaseState();
			CURSOR_ST.ReleaseIcon();
			return true;
		}
	}
	return false;
}

#endif

void cWarehouse::ResetDevice()
{	
	ResetDeviceScript();

	m_pBlock->ResetDevice();
	m_pBlockInItem->ResetDevice();
}

void cWarehouse::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

//------------------------------------------------------------------------------
// 함수 하나로 통일 하자.
//------------------------------------------------------------------------------

bool cWarehouse::SetResist( int nInvenIndex, int nWarehouseIndex, int nCount )
{
	// 해당슬롯 지정 안함
	if( nWarehouseIndex == -1 )
	{
		int nIndex = GetSystem()->FindEmptySlot();
		// 창고가 가득 찼다
		if( nIndex == -1 )
		{
#ifdef NEW_SHARESTASH			
			int nEmptySlot = GetSystem()->FindEmptyShareSlot();
			// 공유 창고가 가득 찼다
			if( nEmptySlot == -1 )
			{
				cPrintMsg::PrintMsg( 11037 );
				return false;
			}
			g_pDataMng->SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_SHARESTASH_SID( nEmptySlot ), nCount );

			return true;
#else
			cPrintMsg::PrintMsg( 11037 );
			return false;
#endif
		}
		g_pDataMng->SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_WAREHOUSE_SID( nIndex ), nCount );
	}
	else
	{
		// 해당 슬롯 지정		
		cItemInfo* pInven = g_pDataMng->GetInven()->GetData( nInvenIndex );
		cItemInfo* pWarehouse = GetSystem()->GetData( nWarehouseIndex );
		assert_cs( pInven->IsEnable() == true );

		// 비여있는 창고 슬롯인가
		if( pWarehouse->IsEnable() == false )
		{
			g_pDataMng->SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_WAREHOUSE_SID( nWarehouseIndex ), nCount );
		}
		// 차있는 창고 슬롯
		else
		{
			// 해당 슬롯 아이템과 인벤의 아이템이 같은 아이템인지 체크			
			if( pInven->GetType() != pWarehouse->GetType() )
			{
				// 인벤아이템의 갯수가 전부라면 교체
				if( ( nCount == 0 )||( pInven->GetCount() == nCount ) )
				{
					g_pDataMng->SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_WAREHOUSE_SID( nWarehouseIndex ), nCount );
				}
				else
				{
					// 부분 갯수라면 빈칸에 넣어 준다.
					return SetResist( nInvenIndex, -1, nCount );
				}				
			}
			// 같은 타입이라면
			else
			{
				CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pInven->GetType() )->GetInfo();

				// 같은 타입이면서 맥스치까지 전부 차있다면 - 취소
				if( pWarehouse->GetCount() == pFTItem->s_nOverlap )
				{
					return false;
				}

				// 합쳐서 맥스 갯수가 넘는가 체크
				// 넘지 않는다면 온전히 다 옴겨 주자
				if( pInven->GetCount() + pWarehouse->GetCount() <= pFTItem->s_nOverlap )
				{
					g_pDataMng->SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_WAREHOUSE_SID( nWarehouseIndex ), nCount );
				}
				// 넘는다면 맥스 카운트 만들수 있을만큼만 옴겨주자
				else
				{
					int nMoveCnt = pFTItem->s_nOverlap - pWarehouse->GetCount();

					// 0 이면 맥스치로 인식해서 0이면 안된다.
					assert_cs( nMoveCnt > 0 );
					g_pDataMng->SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_WAREHOUSE_SID( nWarehouseIndex ), nMoveCnt );
				}
			}
		}
	}
	return true;
}

#ifdef NEW_SHARESTASH
bool cWarehouse::MoveItemStoW( int nSrc, int nDest, int nCount )
{
	// 해당슬롯 지정 안함
	if( nDest == -1 )
	{
		int nIndex = GetSystem()->FindEmptySlot();
		// 창고가 가득 찼다
		if( nIndex == -1 )
		{
			cPrintMsg::PrintMsg( 11037 );
			return false;
		}
		g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_WAREHOUSE_SID( nIndex ), nCount );
	}
	else
	{
		// 해당 슬롯 지정		
		cItemInfo* pShare = GetSystem()->GetShareData( nSrc );
		cItemInfo* pWarehouse = GetSystem()->GetData( nDest );
		assert_cs( pShare->IsEnable() == true );

		// 비여있는 창고 슬롯인가
		if( pWarehouse->IsEnable() == false )
		{
			g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
		}
		// 차있는 창고 슬롯
		else
		{
			// 해당 슬롯 아이템과 인벤의 아이템이 같은 아이템인지 체크			
			if( pShare->GetType() != pWarehouse->GetType() )
			{
				// 인벤아이템의 갯수가 전부라면 교체
				if( ( nCount == 0 )||( pShare->GetCount() == nCount ) )
				{
					g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
				}
				else
				{
					// 부분 갯수라면 빈칸에 넣어 준다.
					return MoveItemStoW( nSrc, -1, nCount );
				}				
			}
			// 같은 타입이라면
			else
			{
				CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pShare->GetType() )->GetInfo();

				// 같은 타입이면서 맥스치까지 전부 차있다면 - 취소
				if( pWarehouse->GetCount() == pFTItem->s_nOverlap )
				{
					return false;
				}

				// 합쳐서 맥스 갯수가 넘는가 체크
				// 넘지 않는다면 온전히 다 옴겨 주자
				if( pShare->GetCount() + pWarehouse->GetCount() <= pFTItem->s_nOverlap )
				{
					g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
				}
				// 넘는다면 맥스 카운트 만들수 있을만큼만 옴겨주자
				else
				{
					int nMoveCnt = pFTItem->s_nOverlap - pWarehouse->GetCount();

					// 0 이면 맥스치로 인식해서 0이면 안된다.
					assert_cs( nMoveCnt > 0 );
					g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nMoveCnt );
				}
			}
		}
	}
	return true;
}

bool cWarehouse::MoveItemItoW( int nSrc, int nDest, int nCount )
{
	// 해당슬롯 지정 안함
	if( nDest == -1 )
	{
		int nIndex = GetSystem()->FindEmptySlot();
		// 창고가 가득 찼다
		if( nIndex == -1 )
		{
			cPrintMsg::PrintMsg( 11037 );
			return false;
		}
		g_pDataMng->SendItemMove( TO_INVEN_SID( nSrc ), TO_WAREHOUSE_SID( nIndex ), nCount );
	}
	else
	{
		// 해당 슬롯 지정		
		cItemInfo* pInven = g_pDataMng->GetInven()->GetData( nSrc );
		cItemInfo* pWarehouse = GetSystem()->GetData( nDest );
		assert_cs( pInven->IsEnable() == true );

		// 비여있는 창고 슬롯인가
		if( pWarehouse->IsEnable() == false )
		{
			g_pDataMng->SendItemMove( TO_INVEN_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
		}
		// 차있는 창고 슬롯
		else
		{
			// 해당 슬롯 아이템과 인벤의 아이템이 같은 아이템인지 체크			
			if( pInven->GetType() != pWarehouse->GetType() )
			{
				// 인벤아이템의 갯수가 전부라면 교체
				if( ( nCount == 0 )||( pInven->GetCount() == nCount ) )
				{
					g_pDataMng->SendItemMove( TO_INVEN_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
				}
				else
				{
					// 부분 갯수라면 빈칸에 넣어 준다.
					return MoveItemItoW( nSrc, -1, nCount );
				}				
			}
			// 같은 타입이라면
			else
			{
				CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pInven->GetType() )->GetInfo();

				// 같은 타입이면서 맥스치까지 전부 차있다면 - 취소
				if( pWarehouse->GetCount() == pFTItem->s_nOverlap )
				{
					return false;
				}

				// 합쳐서 맥스 갯수가 넘는가 체크
				// 넘지 않는다면 온전히 다 옴겨 주자
				if( pInven->GetCount() + pWarehouse->GetCount() <= pFTItem->s_nOverlap )
				{
					g_pDataMng->SendItemMove( TO_INVEN_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
				}
				// 넘는다면 맥스 카운트 만들수 있을만큼만 옴겨주자
				else
				{
					int nMoveCnt = pFTItem->s_nOverlap - pWarehouse->GetCount();

					// 0 이면 맥스치로 인식해서 0이면 안된다.
					assert_cs( nMoveCnt > 0 );
					g_pDataMng->SendItemMove( TO_INVEN_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nMoveCnt );
				}
			}
		}
	}
	return true;
}

bool cWarehouse::MoveItemWtoW( int nSrc, int nDest, int nCount )
{
	assert_cs( nSrc >= 0 );
	assert_cs( nDest >= 0 );
	assert_cs( nCount > 0 );

	// 해당 슬롯 지정		
	cItemInfo* pSrc = GetSystem()->GetData( nSrc );
	cItemInfo* pDest = GetSystem()->GetData( nDest );
	assert_cs( pSrc->IsEnable() == true );

	// 비여있는 창고 슬롯인가
	if( pDest->IsEnable() == false )
	{
		g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
	}
	// 차있는 창고 슬롯
	else
	{
		// 같은 아이템인지 체크
		if( pSrc->GetType() != pDest->GetType() )
		{
			// 소스아이템의 갯수가 전부라면 교체
			if( ( nCount == 0 )||( pSrc->GetCount() == nCount ) )
			{
				g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
			}
			else
			{
				// 부분 갯수라면 빈칸에 넣어 준다.
				int nIndex = GetSystem()->FindEmptySlot();
				// 창고가 가득 찼다
				if( nIndex == -1 )
				{
					cPrintMsg::PrintMsg( 11037 );
					return false;
				}
				g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nIndex ), nCount );				
			}				
		}
		// 같은 타입이라면
		else
		{
			CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

			// 같은 타입이면서 맥스치까지 전부 차있다면 - 교체
			if( pDest->GetCount() == pFTItem->s_nOverlap )
			{
				g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
			}
			// 합쳐서 맥스 갯수가 넘는가 체크
			// 넘지 않는다면 온전히 다 옴겨 주자
			else if( pSrc->GetCount() + pDest->GetCount() <= pFTItem->s_nOverlap )
			{
				g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
			}
			// 넘는다면 맥스 카운트 만들수 있을만큼만 옴겨주자
			else
			{
				int nMoveCnt = pFTItem->s_nOverlap - pDest->GetCount();

				// 0 이면 맥스치로 인식해서 0이면 안된다.
				assert_cs( nMoveCnt > 0 );
				g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nMoveCnt );
			}
		}
	}
	return true;
}

bool cWarehouse::MoveItemWtoS( int nSrc, int nDest, int nCount )
{
	// 해당슬롯 지정 안함
	if( nDest == -1 )
	{
		int nIndex = GetSystem()->FindEmptyShareSlot();
		// 창고가 가득 찼다
		if( nIndex == -1 )
		{
			cPrintMsg::PrintMsg( 11037 );
			return false;
		}
		g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_SHARESTASH_SID( nIndex ), nCount );
	}
	else
	{
		// 해당 슬롯 지정		
		cItemInfo* pData = GetSystem()->GetData( nSrc );
		cItemInfo* pShare = GetSystem()->GetShareData( nDest );
		assert_cs( pData->IsEnable() == true );

		// 비여있는 창고 슬롯인가
		if( pShare->IsEnable() == false )
		{
			g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nCount );
		}
		// 차있는 창고 슬롯
		else
		{
			// 해당 슬롯 아이템과 인벤의 아이템이 같은 아이템인지 체크			
			if( pData->GetType() != pShare->GetType() )
			{
				// 인벤아이템의 갯수가 전부라면 교체
				if( ( nCount == 0 )||( pData->GetCount() == nCount ) )
				{
					g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nCount );
				}
				else
				{
					// 부분 갯수라면 빈칸에 넣어 준다.
					return MoveItemWtoS( nSrc, -1, nCount );
				}				
			}
			// 같은 타입이라면
			else
			{
				CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() )->GetInfo();

				// 같은 타입이면서 맥스치까지 전부 차있다면 - 취소
				if( pShare->GetCount() == pFTItem->s_nOverlap )
				{
					return false;
				}

				// 합쳐서 맥스 갯수가 넘는가 체크
				// 넘지 않는다면 온전히 다 옴겨 주자
				if( pData->GetCount() + pShare->GetCount() <= pFTItem->s_nOverlap )
				{
					g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nCount );
				}
				// 넘는다면 맥스 카운트 만들수 있을만큼만 옴겨주자
				else
				{
					int nMoveCnt = pFTItem->s_nOverlap - pShare->GetCount();

					// 0 이면 맥스치로 인식해서 0이면 안된다.
					assert_cs( nMoveCnt > 0 );
					g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nMoveCnt );
				}
			}
		}
	}
	return true;
}



// 인밴에서 온걸 
bool cWarehouse::MoveItemItoS( int nSrc, int nDest, int nCount )
{
	// 해당슬롯 지정 안함
	if( nDest == -1 )
	{
		int nIndex = GetSystem()->FindEmptyShareSlot();
		// 창고가 가득 찼다
		if( nIndex == -1 )
		{
			cPrintMsg::PrintMsg( 11037 );
			return false;
		}
		g_pDataMng->SendItemMove( TO_INVEN_SID( nSrc ), TO_SHARESTASH_SID( nIndex ), nCount );	// 시드값 수정 해야할듯..
	}
	else
	{
		// 해당 슬롯 지정		
		cItemInfo* pInven = g_pDataMng->GetInven()->GetData( nSrc );
		cItemInfo* pShareItem = GetSystem()->GetShareData( nDest );
		assert_cs( pInven->IsEnable() == true );

		// 비여있는 창고 슬롯인가
		if( pShareItem->IsEnable() == false )
		{
			g_pDataMng->SendItemMove( TO_INVEN_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nCount );	// 시드값 수정 해야할듯..
		}
		// 차있는 창고 슬롯
		else
		{
			// 해당 슬롯 아이템과 인벤의 아이템이 같은 아이템인지 체크			
			if( pInven->GetType() != pShareItem->GetType() )
			{
				// 인벤아이템의 갯수가 전부라면 교체
				if( ( nCount == 0 )||( pInven->GetCount() == nCount ) )
				{
					g_pDataMng->SendItemMove( TO_INVEN_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nCount );	// 시드값 수정 해야할듯..
				}
				else
				{
					// 부분 갯수라면 빈칸에 넣어 준다.
					return MoveItemItoS( nSrc, -1, nCount );
				}				
			}
			// 같은 타입이라면
			else
			{
				CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pInven->GetType() )->GetInfo();

				// 같은 타입이면서 맥스치까지 전부 차있다면 - 취소
				if( pShareItem->GetCount() == pFTItem->s_nOverlap )
				{
					return false;
				}

				// 합쳐서 맥스 갯수가 넘는가 체크
				// 넘지 않는다면 온전히 다 옴겨 주자
				if( pInven->GetCount() + pShareItem->GetCount() <= pFTItem->s_nOverlap )
				{
					g_pDataMng->SendItemMove( TO_INVEN_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nCount );	// 시드값 수정 해야할듯..
				}
				// 넘는다면 맥스 카운트 만들수 있을만큼만 옴겨주자
				else
				{
					int nMoveCnt = pFTItem->s_nOverlap - pShareItem->GetCount();

					// 0 이면 맥스치로 인식해서 0이면 안된다.
					assert_cs( nMoveCnt > 0 );
					g_pDataMng->SendItemMove( TO_INVEN_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nMoveCnt );	// 시드값 수정 해야할듯..
				}
			}
		}
	}
	return true;
}



bool cWarehouse::MoveItemStoS( int nSrc, int nDest, int nCount )
{
	assert_cs( nSrc >= 0 );
	assert_cs( nDest >= 0 );
	assert_cs( nCount > 0 );

	// 해당 슬롯 지정		
	cItemInfo* pSrc = GetSystem()->GetShareData( nSrc );
	cItemInfo* pDest = GetSystem()->GetShareData( nDest );
	assert_cs( pSrc->IsEnable() == true );

	// 비여있는 창고 슬롯인가
	if( pDest->IsEnable() == false )
	{
		g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nCount );
	}
	// 차있는 창고 슬롯
	else
	{
		// 같은 아이템인지 체크
		if( pSrc->GetType() != pDest->GetType() )
		{
			// 소스아이템의 갯수가 전부라면 교체
			if( ( nCount == 0 )||( pSrc->GetCount() == nCount ) )
			{
				g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nCount );
			}
			else
			{
				// 부분 갯수라면 빈칸에 넣어 준다.
				int nIndex = GetSystem()->FindEmptyShareSlot();
				// 창고가 가득 찼다
				if( nIndex == -1 )
				{
					cPrintMsg::PrintMsg( 11037 );
					return false;
				}
				g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_SHARESTASH_SID( nIndex ), nCount );				
			}				
		}
		// 같은 타입이라면
		else
		{
			CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

			// 같은 타입이면서 맥스치까지 전부 차있다면 - 교체
			if( pDest->GetCount() == pFTItem->s_nOverlap )
			{
				g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nCount );
			}
			// 합쳐서 맥스 갯수가 넘는가 체크
			// 넘지 않는다면 온전히 다 옴겨 주자
			else if( pSrc->GetCount() + pDest->GetCount() <= pFTItem->s_nOverlap )
			{
				g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nCount );
			}
			// 넘는다면 맥스 카운트 만들수 있을만큼만 옴겨주자
			else
			{
				int nMoveCnt = pFTItem->s_nOverlap - pDest->GetCount();

				// 0 이면 맥스치로 인식해서 0이면 안된다.
				assert_cs( nMoveCnt > 0 );
				g_pDataMng->SendItemMove( TO_SHARESTASH_SID( nSrc ), TO_SHARESTASH_SID( nDest ), nMoveCnt );
			}
		}
	}
	return true;
}
#else

bool cWarehouse::MoveItem( int nSrc, int nDest, int nCount )
{
	assert_cs( nSrc >= 0 );
	assert_cs( nDest >= 0 );
	assert_cs( nCount > 0 );

	// 해당 슬롯 지정		
	cItemInfo* pSrc = GetSystem()->GetData( nSrc );
	cItemInfo* pDest = GetSystem()->GetData( nDest );
	assert_cs( pSrc->IsEnable() == true );

	// 비여있는 창고 슬롯인가
	if( pDest->IsEnable() == false )
	{
		g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
	}
	// 차있는 창고 슬롯
	else
	{
		// 같은 아이템인지 체크
		if( pSrc->GetType() != pDest->GetType() )
		{
			// 소스아이템의 갯수가 전부라면 교체
			if( ( nCount == 0 )||( pSrc->GetCount() == nCount ) )
			{
				g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
			}
			else
			{
				// 부분 갯수라면 빈칸에 넣어 준다.
				int nIndex = GetSystem()->FindEmptySlot();
				// 창고가 가득 찼다
				if( nIndex == -1 )
				{
					cPrintMsg::PrintMsg( 11037 );
					return false;
				}
				g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nIndex ), nCount );				
			}				
		}
		// 같은 타입이라면
		else
		{
			CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

			// 같은 타입이면서 맥스치까지 전부 차있다면 - 교체
			if( pDest->GetCount() == pFTItem->s_nOverlap )
			{
				g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
			}
			// 합쳐서 맥스 갯수가 넘는가 체크
			// 넘지 않는다면 온전히 다 옴겨 주자
			else if( pSrc->GetCount() + pDest->GetCount() <= pFTItem->s_nOverlap )
			{
				g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nCount );
			}
			// 넘는다면 맥스 카운트 만들수 있을만큼만 옴겨주자
			else
			{
				int nMoveCnt = pFTItem->s_nOverlap - pDest->GetCount();

				// 0 이면 맥스치로 인식해서 0이면 안된다.
				assert_cs( nMoveCnt > 0 );
				g_pDataMng->SendItemMove( TO_WAREHOUSE_SID( nSrc ), TO_WAREHOUSE_SID( nDest ), nMoveCnt );
			}
		}
	}
	return true;
}



#endif
