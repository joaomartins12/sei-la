
#include "stdafx.h"
#include "Trade.h"

#define IF_TRADE_MY_POS		CsPoint( 225, 0 )

#define IF_TRADE_ICON_SIZE		CsPoint( 32, 32 )


cTrade::cTrade():m_pCancelButton(NULL),m_pBtnOK(NULL),m_pBtnCancel(NULL),
m_pMyReadyWin(NULL),m_pTargetReadyWin(NULL),m_pMyMoney(NULL),m_pTargetMoney(NULL),
m_pMyMoneyBtn(NULL),m_pTradeButton(NULL),m_pTradeChat(NULL)
{
}

cTrade::~cTrade()
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}


cBaseWindow::eWINDOW_TYPE cTrade::GetWindowType()
{ 
	return WT_TRADE; 
}

void cTrade::Destroy()
{
	cBaseWindow::Delete();	
}

void cTrade::DeleteResource()
{
	if( g_pNpcMng )
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_NONE );

	cMoneySeparate::DeleteInstance( cBaseWindow::WT_TRADE, 0 );

	DeleteScript();
}

void cTrade::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( NULL, CsPoint( 210, 77 ), CsPoint( 450, 485 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );

	// 거래 대상의 창
	AddSprite( CsPoint::ZERO, CsPoint( 225, 365 ), "Trade\\TargetWin.tga" );
	m_pTargetReadyWin = AddSprite( CsPoint( 9, 36 ), CsPoint( 206, 246 ), "Trade\\Mask.tga" );
	if( m_pTargetReadyWin )
		m_pTargetReadyWin->SetVisible( false );

	{// 거래 대상 이름 셋팅
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		ti.s_bOutLine = false;
		ti.s_eTextAlign = DT_CENTER;
		std::wstring wTargetName = GetSystem()->GetTradeTamerName();
		ti.SetText( wTargetName.c_str() );
		AddText( &ti, CsPoint( 108, 8 ) );
	}
	// 상대방 돈 이미지
	AddButton( CsPoint( 199, 291 ), CsPoint( 19, 24 ), CsPoint( 19, 0 ), "System\\money.tga" );

	m_pTargetMoney = NiNew cMoneyIF;
	m_pTargetMoney->Init( cMoneyIF::DISP_FULL, (u8)0, false, FONT_WHITE, DT_RIGHT, CFont::FS_10 );	
	m_pTargetMoney->SetPos( CsPoint( 45, 291 ) );
	AddChildControl( m_pTargetMoney );

	// 내 거래의 창
	AddSprite( IF_TRADE_MY_POS, CsPoint( 225, 365 ), "Trade\\MyWin.tga" );
	m_pMyReadyWin = AddSprite( CsPoint( 9, 36 )+ IF_TRADE_MY_POS, CsPoint( 206, 246 ), "Trade\\Mask.tga" );
	if( m_pMyReadyWin )
		m_pMyReadyWin->SetVisible(false);

	{// 내 이름 셋팅
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		ti.s_bOutLine = false;
		ti.s_eTextAlign = DT_CENTER;
		std::wstring wMyName = GetSystem()->GetMyName();
		ti.SetText( wMyName.c_str() );
		AddText( &ti, CsPoint( 108, 8 ) + IF_TRADE_MY_POS );
	}

	m_pMyMoneyBtn = AddButton( CsPoint( 199, 291 ) + IF_TRADE_MY_POS, CsPoint( 19, 24 ), CsPoint( 19, 0 ), "System\\money.tga" );
	if( m_pMyMoneyBtn )
		m_pMyMoneyBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cTrade::ShowMoneyInputWindow);

	m_pMyMoney = NiNew cMoneyIF;
	m_pMyMoney->Init( cMoneyIF::DISP_FULL, (u8)0, false, FONT_WHITE, DT_RIGHT, CFont::FS_10 );
	m_pMyMoney->SetPos( CsPoint( 45, 291 ) + IF_TRADE_MY_POS );
	AddChildControl( m_pMyMoney );


	m_pBtnOK = AddButton( CsPoint( 151, 328 ) + IF_TRADE_MY_POS, CsPoint( 61, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "TRADE_BTN_OK" ).c_str() );
	if( m_pBtnOK )
		m_pBtnOK->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cTrade::Trade_Inven_Lock );

	m_pBtnCancel = AddButton( CsPoint( 151, 328 ) + IF_TRADE_MY_POS, CsPoint( 61, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_CANCEL" ).c_str() );
	if( m_pBtnCancel )
	{
		m_pBtnCancel->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cTrade::Trade_Inven_UnLock );
		m_pBtnCancel->SetVisible(false);
	}

	m_pCancelButton = AddButton( IF_TRADE_MY_POS + CsPoint( 201, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	if( m_pCancelButton )
		m_pCancelButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cTrade::CloseWindowEvent);


	m_pTradeButton = AddButton( CsPoint( 240, 328 ), CsPoint( 61, 28 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_TRADE" ).c_str() );
	if( m_pTradeButton )
	{
		m_pTradeButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cTrade::SendTradeFinal );
		m_pTradeButton->SetEnable(false);
	}

	cPopUp* pTradeChatBg = NiNew cPopUp;
	if( pTradeChatBg )
	{
		pTradeChatBg->Init( m_pRoot, CsPoint( 0, 365 ), CsPoint( 450, 120 ), CsPoint( 3, 3 ), "Control_G\\Popup\\NDlg", "tga", false );
		AddChildControl( pTradeChatBg );
	}

	m_pTradeChat = NiNew cListBox;
	if( m_pTradeChat )
	{
		m_pTradeChat->Init( GetRoot(), CsPoint( 7, 368 ), CsPoint( 440, 114 ), NULL, false );

		m_pTradeChat->SetMargin( CsPoint(0, 5) );
		m_pTradeChat->SetItemGab( CsPoint(0, 2) );

		cScrollBar* pScrollBar = NiNew cScrollBar;
		if( pScrollBar )
		{
			pScrollBar->Init( cScrollBar::TYPE_1, NULL, CsPoint( 0, 0 ), CsPoint( 16, 114 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint(0, 365 ), CsSIZE( 450, 120) ), 6, false );
			m_pTradeChat->SetScrollBar( pScrollBar );
		}
		AddChildControl( m_pTradeChat );
	}

	CsPoint ptStart( 24, 53 );
	for( int i=0; i<MAX_TRADE_ITEM_SLOT; ++i )
	{
		m_MyInfo[ i ].s_IFIcon.Init( CsRect( ptStart + IF_TRADE_MY_POS, (CsSIZE)IF_TRADE_ICON_SIZE ) );
		m_TargetInfo[ i ].s_IFIcon.Init( CsRect( ptStart, (CsSIZE)IF_TRADE_ICON_SIZE ) );
		ptStart.y += 46;
	}
}

void cTrade::CloseWindowEvent(void* pSender, void* pData)
{
	GetSystem()->SendTradeCancel();
}

void cTrade::Trade_Inven_Lock(void* pSender, void* pData)
{
	GetSystem()->SendTradeInvenLock(true);
}

void cTrade::Trade_Inven_UnLock(void* pSender, void* pData)
{
	GetSystem()->SendTradeInvenLock(false);
}

void cTrade::SendTradeFinal(void* pSender, void* pData)
{
	GetSystem()->SendTradeFinal();
}

void cTrade::PreResetMap()
{ 
	OnEscapeKey();
}

bool cTrade::OnEscapeKey()
{ 
	CloseWindowEvent(m_pCancelButton,NULL);
	return true;
}

void cTrade::ShowMoneyInputWindow(void* pSender, void* pData)
{
	if( GetSystem()->IsMyTradeInvenLock() )
		return;

	if( cMoneySeparate::IsInstance( cBaseWindow::WT_TRADE, 0 ) )
		return;
		
	sMONEY max = CsMin( 999999999, g_pDataMng->GetInven()->GetMoney().GetInt() );
	cMoneySeparate::NewInstance( cBaseWindow::WT_TRADE, 0, m_pMyMoney->GetMoney(), m_pMyMoney->GetMoney(), max );
}

void cTrade::Update(float const& fDeltaTime)
{
	cMoneySeparate::UpdateInstance( cBaseWindow::WT_TRADE, 0 );

	if( g_pNpcMng )
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_TRADE );

	CTamer* pTamer = g_pCharMng->GetTamerFromUIDX( GetSystem()->GetTradeTamerUID() );
	if( NULL == pTamer )
	{
		GetSystem()->SendTradeCancel();
		return;
	}
	else
	{
		float fLen = ( g_pCharMng->GetTamerUser()->GetPos2D() - pTamer->GetPos2D() ).Length();
		if( fLen > TRADE_DISTANCE )
		{
			cPrintMsg::PrintMsg( 30022 );	//	거리가 너무 멉니다.
			GetSystem()->SendTradeCancel();
			return;
		}
	}

	// 전투중이라면 종료
	if( g_pCharMng->GetTamerUser()->IsBattle() == true )
	{
		GetSystem()->SendTradeCancel();
		return;
	}

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cTrade::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;
	if( cMoneySeparate::IsInstance( cBaseWindow::WT_TRADE, 0 ) )
	{
		muReturn = cMoneySeparate::Update_ForMouseInstance( GetWindowType(), 0 );

		return muReturn;
	}

	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if(m_pCancelButton && m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return cBaseWindow::MUT_NONE;

	if( m_pTradeButton && m_pTradeButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return cBaseWindow::MUT_NONE;

	if( m_pBtnOK && m_pBtnOK->Update_ForMouse() == cButton::ACTION_CLICK )
		return cBaseWindow::MUT_NONE;

	if( m_pBtnCancel && m_pBtnCancel->Update_ForMouse() == cButton::ACTION_CLICK )
		return cBaseWindow::MUT_NONE;

	if( m_pMyMoneyBtn && m_pMyMoneyBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return cBaseWindow::MUT_NONE;

	if(m_pTradeChat && m_pTradeChat->Update_ForMouse(CURSOR_ST.GetPos()) )
		return cBaseWindow::MUT_NONE;

	if( _UpdateIcon_ForMouse() )
		return muReturn;

	_UpdateMoveWindow_ForMouse();
	return muReturn;
}

bool cTrade::_UpdateIcon_ForMouse()
{
	bool bCheck = false;
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	for( int i=0; i<MAX_TRADE_ITEM_SLOT; ++i )
	{
		if( m_MyInfo[ i ].s_IFIcon.MouseOn( ptMouseLocalPos ) == false )
		{
			m_MyInfo[ i ].s_IFIcon.ReleaseState();
		}
		else
		{
			bCheck = true;
		}

		if( m_TargetInfo[ i ].s_IFIcon.MouseOn( ptMouseLocalPos ) == false )
		{
			m_TargetInfo[ i ].s_IFIcon.ReleaseState();
		}
		else
		{
			bCheck = true;
		}
	}
	if( bCheck == false )
		return false;


	// 툴팁용
	{
		for( int i=0; i<MAX_TRADE_ITEM_SLOT; ++i )
		{
			if( m_MyInfo[ i ].s_Item.IsEnable() == true )
			{
				if( m_MyInfo[ i ].s_IFIcon.s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
				{
					CsPoint pos = m_MyInfo[ i ].s_IFIcon.GetPos() + GetRootClient();
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TRADE_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM,
						m_MyInfo[ i ].s_Item.m_nType, cBaseWindow::WT_TRADE, cTooltip::OPEN_SLOT, 0, &m_MyInfo[ i ].s_Item );

					//////////////////////////////////////////////////////////
					//2017-01-19-nova 장착중인 아이템을 비교창에 출력
					int nItemType = nsCsFileTable::g_pItemMng->GetItem( m_MyInfo[ i ].s_Item.m_nType )->GetInfo()->s_nType_L;

					for( int i=0; i<nLimit::Equip; ++i )
					{
						cItemInfo* PlayerpData = g_pDataMng->GetTEquip()->GetData( i );

						if(PlayerpData->m_nType != 0)
						{
							CsItem::sINFO* pFTSrc = nsCsFileTable::g_pItemMng->GetItem( PlayerpData->GetType() )->GetInfo();

							if(nItemType == pFTSrc->s_nType_L)
								TOOLTIPMNG_STPTR->GetCompareTooltip()->SetCompareTooltip( pos, IF_TRADE_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, PlayerpData->GetType(), cBaseWindow::WT_TRADE, cTooltip::OPEN_SLOT, 0, PlayerpData );
						}
					}	
					//////////////////////////////////////////////////////////

					break;
				}
			}

			if( m_TargetInfo[ i ].s_Item.IsEnable() == true )
			{
				if( m_TargetInfo[ i ].s_IFIcon.s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
				{					
					CsPoint pos = m_TargetInfo[ i ].s_IFIcon.GetPos() + GetRootClient();
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TRADE_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM,
						m_TargetInfo[ i ].s_Item.m_nType, cBaseWindow::WT_TRADE, cTooltip::OPEN_SLOT, 0, &m_TargetInfo[ i ].s_Item );

					//////////////////////////////////////////////////////////
					//2017-01-19-nova 장착중인 아이템을 비교창에 출력
					int nItemType = nsCsFileTable::g_pItemMng->GetItem( m_TargetInfo[ i ].s_Item.m_nType )->GetInfo()->s_nType_L;

					for( int i=0; i<nLimit::Equip; ++i )
					{
						cItemInfo* PlayerpData = g_pDataMng->GetTEquip()->GetData( i );

						if(PlayerpData->m_nType != 0)
						{
							CsItem::sINFO* pFTSrc = nsCsFileTable::g_pItemMng->GetItem( PlayerpData->GetType() )->GetInfo();

							if(nItemType == pFTSrc->s_nType_L)
								TOOLTIPMNG_STPTR->GetCompareTooltip()->SetCompareTooltip( pos, IF_TRADE_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, PlayerpData->GetType(), cBaseWindow::WT_TRADE, cTooltip::OPEN_SLOT, 0, PlayerpData );
						}
					}	
					//////////////////////////////////////////////////////////

					break;
				}
			}
		}
	}
	return true;
}

void cTrade::Render()
{
	RenderScript();	

	CsPoint pos = CsPoint( 45, 291 ) + GetRootClient();
	for( int i=0; i<MAX_TRADE_ITEM_SLOT; ++i )
	{
		pos = m_TargetInfo[ i ].s_IFIcon.GetPos() + GetRootClient();
		if( m_TargetInfo[ i ].s_Item.IsEnable() == true )
		{
			g_pIconMng->RenderItem( &m_TargetInfo[ i ].s_Item, pos, true );	
			if( m_TargetInfo[ i ].s_ItemName.GetSize() == 1 )
				m_TargetInfo[ i ].s_ItemName.Render( pos + CsPoint( 45, 8 ), 0 );
			else
				m_TargetInfo[ i ].s_ItemName.Render( pos + CsPoint( 45, 1 ), 0 );
		}
		// 마스크 있다면 마스크 랜더링 - 아이템 있는거에 한해서
		if( m_TargetInfo[ i ].s_IFIcon.s_eState != sIFIcon::NONE )
		{			
			m_TargetInfo[ i ].s_IFIcon.RenderMask( pos, IF_TRADE_ICON_SIZE );
		}

		pos += IF_TRADE_MY_POS;
		if( m_MyInfo[ i ].s_Item.IsEnable() == true )
		{
			g_pIconMng->RenderItem( &m_MyInfo[ i ].s_Item, pos, true );
			if( m_MyInfo[ i ].s_ItemName.GetSize() == 1 )
				m_MyInfo[ i ].s_ItemName.Render( pos + CsPoint( 45, 8 ), 0 );
			else
				m_MyInfo[ i ].s_ItemName.Render( pos + CsPoint( 45, 1 ), 0 );
		}

		// 마스크 있다면 마스크 랜더링 - 아이템 있는거에 한해서
		if( m_MyInfo[ i ].s_IFIcon.s_eState != sIFIcon::NONE )
		{			
			m_MyInfo[ i ].s_IFIcon.RenderMask( pos, IF_TRADE_ICON_SIZE );
		}
	}

	cMoneySeparate::RenderInstance( cBaseWindow::WT_TRADE, 0 );
}

void cTrade::OnRButtonUp( CsPoint pos )
{
	if( GetSystem()->IsMyTradeInvenLock() )
		return;

	CsPoint ptMouseLocalPos = MousePosToWindowPos( pos );
	for( int i=0; i<MAX_TRADE_ITEM_SLOT; ++i )
	{
		if( !m_MyInfo[ i ].s_Item.IsEnable() )
			continue;

		if( !m_MyInfo[ i ].s_IFIcon.s_Rect.PtInRect( ptMouseLocalPos ) )
			continue;

		GetSystem()->SendMyTradeItemUnRegist( i );
		break;
	}
}
bool cTrade::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	GetSystem()->SendMyTradeItemRegist( eIconType, nIconSlot, nIconCount );
	return true;
}

void cTrade::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cTrade::ResetDevice()
{	
	ResetDeviceScript();
}

void cTrade::ChangeTradeMoney( bool bMy )
{
	if( bMy )
	{
		if( m_pMyMoney )
		{
			u8 nMoney = GetSystem()->GetMyTradeMoney();
			m_pMyMoney->SetMoney( nMoney );
		}
	}
	else
	{
		if( m_pTargetMoney )
		{
			u8 nMoney = GetSystem()->GetTradeTamerMoney();
			m_pTargetMoney->SetMoney( nMoney );
		}
	}
}

void cTrade::ChangeTradeInvenLock()
{
	bool bMyReady = GetSystem()->IsMyTradeInvenLock();
	if( m_pMyReadyWin )
		m_pMyReadyWin->SetVisible( bMyReady );

	if( m_pBtnOK )
		m_pBtnOK->SetVisible( !bMyReady );

	if( m_pBtnCancel )
		m_pBtnCancel->SetVisible( bMyReady );
	
	if( m_pMyMoneyBtn )
		m_pMyMoneyBtn->SetEnable( !bMyReady );

	bool bTamerReady = GetSystem()->IsTradeTamerInvenLock();
	if( m_pTargetReadyWin )
		m_pTargetReadyWin->SetVisible(bTamerReady);

	if( m_pTradeButton )
	{
		if( bMyReady && bTamerReady )
			m_pTradeButton->SetEnable(true);
		else
			m_pTradeButton->SetEnable(false);
	}
}

void cTrade::RegistItem( bool bMy, int const& nSlotNum )
{
	cItemData const * pData = NULL;
	if( bMy )
		pData = GetSystem()->GetMyRegistItemData( nSlotNum );
	else
		pData = GetSystem()->GetTargetRegistItemData( nSlotNum );

	if( bMy )
	{
		m_MyInfo[ nSlotNum ].s_Item = *pData;

		CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_MyInfo[ nSlotNum ].s_Item.m_nType )->GetInfo();
		cText::sTEXTINFO ti;

#ifdef VERSION_USA
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_9, g_pDataMng->GetItemColor( pFTItem ) );
#else
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, g_pDataMng->GetItemColor( pFTItem ) );
#endif

		g_pStringAnalysis->Cut_Parcing( &m_MyInfo[ nSlotNum ].s_ItemName, 9999, pFTItem->s_szName, &ti );
	}
	else
	{
		m_TargetInfo[ nSlotNum ].s_Item = *pData;

		CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_TargetInfo[ nSlotNum ].s_Item.m_nType )->GetInfo();
		cText::sTEXTINFO ti;

#ifdef VERSION_USA
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_9, g_pDataMng->GetItemColor( pFTItem ) );
#else
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, g_pDataMng->GetItemColor( pFTItem ) );
#endif

		g_pStringAnalysis->Cut_Parcing( &m_TargetInfo[ nSlotNum ].s_ItemName, 9999, pFTItem->s_szName, &ti );
	}

}

void cTrade::UnRegistItem( bool bMy, int const& nSlotNum )
{
	if( bMy )
		m_MyInfo[ nSlotNum ].Reset();
	else
		m_TargetInfo[ nSlotNum ].Reset();
}

void cTrade::AddTradeChatMsg(std::wstring const& msg, NiColor const& nColor)
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, nColor );

	cStringList sl;
	g_pStringAnalysis->Cut( &sl, 420, msg.c_str(), &ti );

	LIST_STRING * pList = sl.GetList();
	LIST_STRING_IT it = pList->begin();
	for( ; it != pList->end(); ++it )
	{
		cListBoxItem * addItem  = NiNew cListBoxItem;
		addItem->SetItem( (*it) );

		m_pTradeChat->AddItem( addItem );
	}
	pList->clear();

	m_pTradeChat->SetScrollBarEndPos();
}

void cTrade::SetTradeStandBy()
{
	SAFE_POINTER_RET( m_pTradeButton );
	m_pTradeButton->SetEnable(false);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool cTrade::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eMyTrade_Money_Change, this );
	GetSystem()->Register( SystemType::eMyTradeItem_Regist, this );
	GetSystem()->Register( SystemType::eMyTradeItem_UnRegist, this );

	GetSystem()->Register( SystemType::eTargetTrade_Money_Change, this );
	GetSystem()->Register( SystemType::eTargetTradeItem_Regist, this );
	GetSystem()->Register( SystemType::eTargetTradeItem_UnRegist, this );

	GetSystem()->Register( SystemType::eChange_Inven_Lock, this );
	
	GetSystem()->Register( SystemType::eAdd_Trade_Chat_Msg, this );
	GetSystem()->Register( SystemType::eTrade_StandBy, this );
	return true;
}

void cTrade::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eChange_Inven_Lock:		ChangeTradeInvenLock();		break;
	case SystemType::eMyTrade_Money_Change:		ChangeTradeMoney(true);		break;
	case SystemType::eTrade_StandBy:			SetTradeStandBy();			break;

	case SystemType::eMyTradeItem_Regist:		
		{
			int nSlotNum =0;
			kStream >> nSlotNum;
			RegistItem( true, nSlotNum );
		}
		break;
	case SystemType::eMyTradeItem_UnRegist:		
		{
			int nSlotNum =0;
			kStream >> nSlotNum;
			UnRegistItem( true, nSlotNum );
		}
		break;

	case SystemType::eTargetTrade_Money_Change:	ChangeTradeMoney(false);		break;
	case SystemType::eTargetTradeItem_Regist:
		{
			int nSlotNum =0;
			kStream >> nSlotNum;
			RegistItem( false, nSlotNum );
		}
		break;
	case SystemType::eTargetTradeItem_UnRegist:
		{
			int nSlotNum =0;
			kStream >> nSlotNum;
			UnRegistItem( false, nSlotNum );
		}
		break;
	case SystemType::eAdd_Trade_Chat_Msg:
		{
			std::wstring msg;
			NiColor textColor;
			kStream >> msg >> textColor;
			AddTradeChatMsg( msg, textColor );
		}
		break;
	}
}