
#include "stdafx.h"
#include "Server_Reward.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

cServer_Reward::cServer_Reward()
{
	m_pCancelButton = NULL;
	m_pRefreshBtn	= NULL;
	m_pAllRecvBtn	= NULL;
	m_pScrollBar	= NULL;
	m_pTabMenu		= NULL;
	FirstPoint		= CsPoint(34, 103);
	
	m_pGiftItemData		= NULL;
	m_pRewardItemData	= NULL;
}

cServer_Reward::~cServer_Reward()
{
}

cBaseWindow::eWINDOW_TYPE cServer_Reward::GetWindowType()
{ 
	return WT_REWARD; 
}

void cServer_Reward::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	cBaseWindow::Delete();
}

void cServer_Reward::DeleteResource()
{
	DeleteScript();
}

void cServer_Reward::Create( int nValue /*=0*/ )
{
	cBaseWindow::Init();
	InitScript( "GiftStorage\\Gift_Win.tga", CsPoint( 150, 150 ), CsPoint( 332, 486 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( UISTRING_TEXT( "ITEMS_PAID_WAREHOUSE_TITLE" ).c_str() );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.SetText( 0 );
	ti.s_eTextAlign = DT_RIGHT;

	for( int i=0; i<REWARD_COUNT; ++i )
	{
		int offset = i % REWARD_COUNT;

		m_IconArray[ i ].Init();
		m_IconArray[ i ].s_rcRegion.SetRect( FirstPoint + CsPoint( 0, (SERVER_LINE_HEIGHT * offset) ) /*+ GetRootClient()*/, CsSIZE( ICON_SIZE, ICON_SIZE ) );

		m_IconArray[ i ].s_pBtnGetReward = AddButton(FirstPoint+ CsPoint( 178, (SERVER_LINE_HEIGHT * offset)), CsPoint( 70, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "ITEMS_PAID_WAREHOUSE_RECEIPT" ).c_str());
		m_IconArray[ i ].s_pExpDate = AddText(&ti, FirstPoint + CsPoint( 160, 7 + (SERVER_LINE_HEIGHT * offset) ) );

	}

	// 스크롤바
	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 309, FirstPoint.y-ICON_SIZE+2 ), CsPoint( 16, 366 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 9, 77 ), CsPoint( 304, 504 ) ), 5 );
	m_pScrollBar->SetRange( CsPoint( 0, REWARD_COUNT ) );

	// 닫기 버튼
	m_pCancelButton = AddButton( CsPoint( 310, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pCancelButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cServer_Reward::PressCancelButton);

	// 새로 고침 버튼
	m_pRefreshBtn = AddButton( CsPoint( 28, 444 ), CsPoint( 34, 34 ), CsPoint( 0, 38 ), "GiftStorage\\Btn_Refresh.tga" );
	m_pRefreshBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cServer_Reward::PressRefreshButton);

	m_pAllRecvBtn = AddButton( CsPoint( FirstPoint.x+178, 444 ), CsPoint( 70, 34 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "ITEMS_PAID_WAREHOUSE_ALL_RECEIPT" ).c_str() );
	m_pAllRecvBtn->SetEnable( g_pDataMng->GetCash()->IsRefreshEnable() );

	// 선물, 보상 지급 탭
	m_pTabMenu = AddRadioButton(CsPoint( 10, 40 ), CsPoint( 104, 24 ), cCheckBox::IMAGE_RADIO_1, UISTRING_TEXT( "ITEMS_PAID_WAREHOUSE_PAYMENTGIFT" ).c_str());
	m_pTabMenu->AddNode( CsPoint( 10+104, 40 ), CsPoint( 104, 24 ), cCheckBox::IMAGE_RADIO_1, UISTRING_TEXT( "ITEMS_PAID_WAREHOUSE_COMPENSATIONPAID" ).c_str() );
	m_pTabMenu->SetCheckIndex( 0 );	

	if( g_pGameIF->IsOpenNpcWindow() || g_pCharMng->GetDigimonUser( 0 )->IsBattle() )
	{
		GetSystem()->CloseItemRewardUI();
		cPrintMsg::PrintMsg( 20091 );
		return;
	}

	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::POSTAL_REFRESH, 0 );
	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::REWARD_REFRESH, 0 );
}

void cServer_Reward::Update(float const& fDeltaTime)
{
	if( g_pGameIF->IsOpenNpcWindow() || g_pCharMng->GetDigimonUser( 0 )->IsBattle() )
	{
		GetSystem()->CloseItemRewardUI();
		cPrintMsg::PrintMsg( 20091 );
		return;
	}		

	m_pRefreshBtn->SetEnable( g_pDataMng->GetCash()->IsRefreshEnable() );
	m_pAllRecvBtn->SetEnable( g_pDataMng->GetCash()->IsRefreshEnable() );

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cServer_Reward::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if(m_pCancelButton && m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pRefreshBtn && m_pRefreshBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	int nRadioAction = m_pTabMenu->Update_ForMouse();

	if( nRadioAction != cRadioButton::INVALIDE_RADIO_CLICK )
	{
		if( nRadioAction == cRadioButton::ACTION_CLICK )
		{
			cWindow::PlaySound( cWindow::SD_At2 );		
		}
		return muReturn;
	}

	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		UpdateGift();	
		break;
	case 1:
		UpdateReward();
		break;

	}

	// 스크롤바
	m_pScrollBar->Update_ForMouse();

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cServer_Reward::Render()
{
	RenderScript();
	EndRenderScript();

	if(m_pTabMenu->GetCheckIndex() == 0)
	{
		RenderGift();
	}
	else
	{
		RenderReward();
	}
}

void cServer_Reward::OnRButtonUp( CsPoint pos )
{
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		{
			// 새로고침중이라면 패쓰
			if( ( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::POSTAL_REFRESH ) == false )||
				( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::REWARD_REFRESH ) == false ) )
			{
				cPrintMsg::PrintMsg( 10004 );
				return;
			}
		}
		return;
	case 1:

		return;

	}
	assert_cs( false );
}

void cServer_Reward::ResetDevice()
{	
	ResetDeviceScript();
}

void cServer_Reward::ResetMap()
{ 
	Close( false ); 
}

bool cServer_Reward::OnEscapeKey()
{ 
	Close(); 
	return true; 
}

void cServer_Reward::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cServer_Reward::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 

}

bool cServer_Reward::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	return true;
}

void cServer_Reward::PressCancelButton( void* pSender, void* pData )
{
	SAFE_POINTER_RET(GetSystem());
	GetSystem()->CloseItemRewardUI();
}

void cServer_Reward::PressRefreshButton( void* pSender, void* pData )
{
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::POSTAL_REFRESH, 0 );

	case 1:
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::REWARD_REFRESH, 0 );
	}		
}

void cServer_Reward::RenderGift()
{
	GIFT_ITEM_LIST* list = GetSystem()->GetGiftData();

	int	ScrollPos = 0;

	ScrollPos = m_pScrollBar->GetCurPosIndex();

	for( int i=0; i<REWARD_COUNT; ++i)
	{
		m_IconArray[ i ].s_pBtnGetReward->SetVisible(false);
		m_IconArray[ i ].s_pExpDate->SetVisible(false);
	}

	for( int i=0; i<REWARD_COUNT; ++i)
	{
		int	Num = i+ScrollPos;

		GIFT_ITEM_LIST::iterator	miter = list->find(Num);

		if(miter == list->end())
			continue;

		cItemInfo*	item = &miter->second;

		if( item->IsEnable() == false )
			continue;

		uint nStartTime = net::game->GetTimeTS();
		uint nEndTime = item->m_nEndTime;
		double result = difftime(nEndTime , nStartTime);

		uint nyDays = ((((uint)result / 60 ) / 60 ) / 24);

		std::wstring msg;
		if( -1 >= result )
			msg = UISTRING_TEXT( "ITEMS_PAID_WAREHOUSE_EXPIRATION" );
		else
			DmCS::StringFn::Format( msg, L"%d%s", nyDays, UISTRING_TEXT( "COMMON_TXT_DAY" ).c_str() );

		m_IconArray[ i ].s_pExpDate->SetText(msg.c_str());
		m_IconArray[ i ].s_pExpDate->SetVisible(true);
		m_IconArray[ i ].s_pBtnGetReward->SetVisible(true);

		CsPoint pos = m_IconArray[ i ].s_rcRegion.GetPos() + GetRootClient();	

		g_pIconMng->RenderItem( item, pos, true );

		m_IconArray[ i ].s_dwID = item->GetType();
	}
}

void cServer_Reward::RenderReward()
{
	REWARD_ITEM_LIST* list = GetSystem()->GetRewardData();

	int	ScrollPos = 0;

	ScrollPos = m_pScrollBar->GetCurPosIndex();

	for( int i=0; i<REWARD_COUNT; ++i)
	{
		m_IconArray[ i ].s_pBtnGetReward->SetVisible(false);
		m_IconArray[ i ].s_pExpDate->SetVisible(false);
	}

	for( int i=0; i<REWARD_COUNT; ++i)
	{
		int	Num = i+ScrollPos;

		REWARD_ITEM_LIST::iterator	miter = list->find(Num);

		if(miter == list->end())
			continue;

		int	RewardIdx = miter->second;

		if( RewardIdx == 0 )
			continue;

		if(NULL == GetSystem()->GetReward(RewardIdx))
			continue;

		CsReward::sINFO* pInfo = GetSystem()->GetReward(RewardIdx)->GetInfo();

		uint nStartTime = net::game->GetTimeTS();
		uint nEndTime = nBase::GetTime( pInfo->s_nExpDate );
		double result = difftime(nEndTime , nStartTime);

		uint nyDays = ((((uint)result / 60 ) / 60 ) / 24);

		std::wstring msg;
		if( -1 >= result )
			msg = UISTRING_TEXT( "ITEMS_PAID_WAREHOUSE_EXPIRATION" );
		else
			DmCS::StringFn::Format( msg, L"%d%s", nyDays, UISTRING_TEXT( "COMMON_TXT_DAY" ).c_str() );

		m_IconArray[ i ].s_pExpDate->SetText(msg.c_str());	
		m_IconArray[ i ].s_pExpDate->SetVisible(true);
		m_IconArray[ i ].s_pBtnGetReward->SetVisible(true);

		CsPoint pos = m_IconArray[ i ].s_rcRegion.GetPos() + GetRootClient();	

		g_pIconMng->RenderItem( pInfo->s_nItemCode1, pos, pInfo->s_nItemCount1 );

		m_IconArray[ i ].s_dwID = pInfo->s_nID;			
	}	
}

void cServer_Reward::UpdateGift()
{
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	GIFT_ITEM_LIST* list = GetSystem()->GetGiftData();

	SAFE_POINTER_RET(list);

	if( m_pAllRecvBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		if( 0 == list->size())
			return;

		cPrintMsg::PrintMsg( 20085 );	// 아이템 E수 있는가 체크
		cMessageBox::GetFirstMessageBox()->SetValue2( 1 );
	}

	int	ScrollPos = 0;

	m_pScrollBar->SetRange( CsPoint( 0, list->size() ) );
	ScrollPos = m_pScrollBar->GetCurPosIndex();
	
	for( int i=0; i<REWARD_COUNT; ++i)
	{
		int	Num = i+ScrollPos;

		GIFT_ITEM_LIST::iterator	miter = list->find(Num);

		if(miter == list->end())
			continue;

		cItemInfo*	item = &miter->second;

		if( item->IsEnable() == false )
			continue;

		if(m_IconArray[ i ].s_pBtnGetReward->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			// 인벤이 비여있는가 체크
			if( g_pDataMng->GetInven()->IsSlot( item->GetType(), item->GetCount() ) )
			{
				// 아이템 E수 있는가 체크					
				if( nsCsFileTable::g_pItemMng->GetItem( item->GetType() )->GetInfo()->s_nUseTime_Min )
				{
					cPrintMsg::PrintMsg( 20086 );
				}
				else
				{
					cPrintMsg::PrintMsg( 20085 );
				}
				cMessageBox::GetFirstMessageBox()->SetValue1( Num );	
				cMessageBox::GetFirstMessageBox()->SetValue2( 0 );
			}
			else
			{
				cWindow::PlaySound( cWindow::SD_Wa1 );
				// 인벤 부족
				cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
			}

		}	
		if(m_IconArray[ i ].s_rcRegion.PtInRect( ptMouseLocalPos ) == TRUE )
		{
			//cItemInfo* pData = g_pDataMng->GetCash()->GetPostalData( i );
			if( item->IsEnable() == true )
			{
				CsPoint pos = m_IconArray[ i ].s_rcRegion.GetPos() + GetRootClient();			
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, item->GetType(), cBaseWindow::WT_DIGITAMA_SELL );
			}
		}
	}
}
void cServer_Reward::UpdateReward()
{
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	REWARD_ITEM_LIST* list = GetSystem()->GetRewardData();

	SAFE_POINTER_RET(list);

	if( m_pAllRecvBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		if( 0 == list->size())
			return;

		net::game->SendRequestRewardAll();
	}

	int	ScrollPos = 0;

	m_pScrollBar->SetRange( CsPoint( 0, list->size() ) );
	ScrollPos = m_pScrollBar->GetCurPosIndex();

	for( int i=0; i<REWARD_COUNT; ++i)
	{
		int	Num = i+ScrollPos;

		REWARD_ITEM_LIST::iterator	miter = list->find(Num);

		if(miter == list->end())		
			continue;

		int	RewardIdx = miter->second;

		if( RewardIdx == 0 )
			continue;

		if(NULL == GetSystem()->GetReward(RewardIdx))
			continue;

		if(m_IconArray[ i ].s_pBtnGetReward->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			net::game->SendRequestRewards( 1, RewardIdx );
		}	
		if(m_IconArray[ i ].s_rcRegion.PtInRect( ptMouseLocalPos ) == TRUE )
		{
			CsReward::sINFO*	pInfo = GetSystem()->GetReward(RewardIdx)->GetInfo();

			CsPoint pos = m_IconArray[ i ].s_rcRegion.GetPos() + GetRootClient();			
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->s_nItemCode1, cBaseWindow::WT_DIGITAMA_SELL );
		}
	}
}
