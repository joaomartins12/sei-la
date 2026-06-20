
#include "stdafx.h"
#include "PcbangInven.h"
#define IF_PCBANG_INVEN_ICONSIZE		CsPoint( 32, 32 )

void cPcbangInven::Destroy()
{
	cBaseWindow::Delete();	
}

void cPcbangInven::DeleteResource()
{
	DeleteScript();		
}

void cPcbangInven::Create(int nValue /* = 0  */)
{
	if( IsLive() == false )
	{	
		// 초기 위치
		SetRootClient( CsPoint( g_nScreenWidth - 324, g_nScreenHeight - 191 ) );
		
		// 아이템 위치
		CsPoint ptStart( 9, 33 );
		int nDeltaX = 36;
		
		for( int i=0; i<IF_PCBANGINVENTORY_MAX_COUNT; ++i )
		{
			m_IFIcon[ i ].Init( CsRect( ptStart, (CsSIZE)IF_PCBANG_INVEN_ICONSIZE ) );
			ptStart.x += nDeltaX;
		}		
	}

	cBaseWindow::Init();
	InitScript( "Inventory\\PC_Win.tga", m_ptRootClient, CsPoint( 266, 73 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );		
	m_pCancelButton = AddButton( CsPoint( 245, 6 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	AddTitle( _ONLY_KOR( "Pc방 전용 아이템" ) );
}

bool cPcbangInven::Close( bool bSound /* = true  */ )
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound )
	{
		PlayCloseSound();
	}

	DeleteResource();
	SetShowWindow( false );

	return true;
}

void cPcbangInven::Update(float const& fDeltaTime)
{	
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cPcbangInven::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
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

	if( _UpdateIcon_ForMouse() == true )
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( GetRoot()->GetSize().x , 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
	return muReturn;
}

bool cPcbangInven::_UpdateIcon_ForMouse()
{
	bool bCheck = false;
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	switch( eCursor )
	{	
	case CURSOR::LBUTTON_DOWN:		
		{
			for( int i=0; i<IF_PCBANGINVENTORY_MAX_COUNT; ++i )
			{
				if( ( bCheck == false )&&( m_IFIcon[ i ].MouseLBtnClick( ptMouseLocalPos ) == true ) )
				{
					bCheck = true;
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
			for( int i=0; i<IF_PCBANGINVENTORY_MAX_COUNT; ++i )
			{
				if( ( bCheck == false )&&( m_IFIcon[ i ].MouseRBtnClick( ptMouseLocalPos ) == true ) )
				{					
					bCheck = true;
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

			for( int i=0; i<IF_PCBANGINVENTORY_MAX_COUNT; ++i )
			{
				if( m_IFIcon[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( ( bCheck == false )&&( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == true ) )
						bCheck = true;
					else
						m_IFIcon[ i ].ReleaseState();
				}
			}			
		}
		break;			
	case CURSOR::RBUTTON_PRESS:
		{
			for( int i=0; i<IF_PCBANGINVENTORY_MAX_COUNT; ++i )
			{
				if( m_IFIcon[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( ( bCheck == false )&&( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == true ) )
						bCheck = true;
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
			for( int i=0; i<IF_PCBANGINVENTORY_MAX_COUNT; ++i )
			{
				if( ( bCheck == false )&&( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == true ) )
				{					
					bCheck = true;
				}
				else
				{
					m_IFIcon[ i ].ReleaseState();
				}
			}
		}	
		break;
	}


	// 툴팁용
	for( int i=0; i<IF_PCBANGINVENTORY_MAX_COUNT; ++i )
	{
		cItemInfo* pData = g_pDataMng->GetPCbang()->GetData( i );			

		if( pData->IsEnable() == false )
			continue;

		if( m_IFIcon[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
		{
			int nItemType = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() )->GetInfo()->s_nType_L;
			cTooltip::eSLOT eSlot = cTooltip::OPEN_SLOT;
			CsPoint pos = m_IFIcon[ i ].GetPos() + GetRootClient();					

			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_PCBANG_INVEN_ICONSIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->GetType(), cBaseWindow::WT_PCBANGINVEN, eSlot, 0, pData );
		}
	}	

	return bCheck;
}

void cPcbangInven::Render()
{
	RenderScript();	

	cItemInfo* pItemArray = g_pDataMng->GetPCbang()->GetDataArray();
	cItemInfo* pItem;
	CsItem::sINFO* pFTItemInfo;
	CsPoint pos;
	
	for( int i=0; i<IF_PCBANGINVENTORY_MAX_COUNT; ++i )
	{
		pItem = &pItemArray[ i ];

		// 위치
		pos = m_IFIcon[ i ].GetPos() + GetRootClient();

		// 열린 인벤 토리인가
		if( pItem->IsEnable() == true )
		{
			pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( pItem->GetType() )->GetInfo();

			// 아이템 랜더링 - 숫자 포함
			g_pIconMng->RenderItem( pItem, pos, true );		
		}		

		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon[ i ].RenderMask( pos, IF_PCBANG_INVEN_ICONSIZE );
		}
	}	
}

void cPcbangInven::OnMouseOutWindow()
{
	cBaseWindow::OnMouseOutWindow();

	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
		return;

	if( CURSOR_ST.IsIcon() == true )
		return;	
}

void cPcbangInven::OnLButtonUp( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );	
	for( int i=0; i<IF_PCBANGINVENTORY_MAX_COUNT; ++i )
	{
		if( ( m_IFIcon[ i ].s_eState == sIFIcon::MOUSE_LCLICK )&&( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			int nInvenIndex = i; 
			cItemInfo* pItem = g_pDataMng->GetPCbang()->GetData( nInvenIndex );
			if( pItem->IsEnable() == true )
			{
				// 인벤이 비여 있는가
				if( g_pDataMng->GetInven()->GetEmptySlotCount() == 0 )
				{
					cPrintMsg::PrintMsg( 11015 );
				}
				else
				{
					if( g_pDataMng->GetPCbang()->GetPcBang() == true )
					{
						net::game->SendCropPCBangInvenItem( i, pItem->GetType(), pItem->GetCount() );
						g_pDataMng->GetPCbang()->GetData( i )->Reset();
					}
				}
			}
		}

		m_IFIcon[ i ].ReleaseState();
	}
}

void cPcbangInven::OnRButtonUp( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos );
	for( int i=0; i<IF_PCBANGINVENTORY_MAX_COUNT; ++i )
	{
		if( ( m_IFIcon[ i ].s_eState == sIFIcon::MOUSE_RCLICK )&&( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			int nInvenIndex = i;
			cItemInfo* pItem = g_pDataMng->GetPCbang()->GetData( nInvenIndex );
			if( pItem->IsEnable() == true )
			{
				// 인벤이 비여 있는가
				if( g_pDataMng->GetInven()->GetEmptySlotCount() == 0 )
				{
					cPrintMsg::PrintMsg( 11015 );
				}
				else
				{
					if( g_pDataMng->GetPCbang()->GetPcBang() == true )
					{
						net::game->SendCropPCBangInvenItem( i, pItem->GetType(), pItem->GetCount() );
						g_pDataMng->GetPCbang()->GetData( i )->Reset();
					}
				}
			}
		}

		m_IFIcon[ i ].ReleaseState();
	}
}

void cPcbangInven::ResetDevice()
{	
	ResetDeviceScript();
}






