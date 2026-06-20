
#include "stdafx.h"
#include "UnionStore.h"

cUnionStore::cUnionStore()
{
	m_pCancelBtn = NULL;
	m_pEmploymentBtn = NULL;
	m_pCollectBtn = NULL;

	m_pMoneyIF = NULL;

	m_bActiveCollectBtn = false;
	m_bEnableCollectBtn = false;
}

cUnionStore::~cUnionStore()
{	
}

bool cUnionStore::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eShopWithDraw, this );
	return true;
}

void cUnionStore::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eShopWithDraw:	CollectItem();	break;
	}
}

void cUnionStore::Destroy()
{
	cBaseWindow::Delete();
}

void cUnionStore::DeleteResource()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	DeleteScript();
	SAFE_NIDELETE( m_pMoneyIF );	

	GetSystem()->DeleteUnion();
}

void cUnionStore::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "Store_G\\Win_Union.tga", CsPoint( 150, 165 ), CsPoint( 279, 261 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( UISTRING_TEXT( "UNIONSTORE_TITLE_STORAGE" ).c_str() );
	
	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_9 );
	ti.SetText( UISTRING_TEXT( "UNIONSTORE_STORED_ITEM" ).c_str() );
	ti.s_eTextAlign = DT_CENTER;

	AddText( &ti, CsPoint( 224, 35 ) );

	m_pCancelBtn = AddButton( CsPoint( 256, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );		
	m_pCancelBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cUnionStore::PressCancelBtn);

	m_pEmploymentBtn = AddButton( CsPoint( 10, 225 ), CsPoint( 125, 27 ), cButton::IMAGE_NOR_13, UISTRING_TEXT( "UNIONSTORE_BTN_RETRIEVE_SHOP" ).c_str() );
	m_pEmploymentBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cUnionStore::PressEmploymentBtn);

	m_pCollectBtn = AddButton( CsPoint( 185, 225 ), CsPoint( 85, 27 ), cButton::IMAGE_NOR_13, UISTRING_TEXT( "UNIONSTORE_BTN_RETRIEVE_ITEM" ).c_str() );
	m_pCollectBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cUnionStore::PressCollecttBtn);
	m_pCollectBtn->SetEnable( false );
	
	m_pMoneyIF = NiNew cMoneyIF;
	m_pMoneyIF->Init( cMoneyIF::DISP_FULL, (u8)0, true );

	CsPoint pos( 21, 66 );
	for( int y=0; y<IF_UNION_Y; ++y )
	{
		for( int x=0; x<IF_UNION_X; ++x )
		{
			m_IFIcon[ y*IF_UNION_X + x ].Init( CsRect( pos, (CsSIZE)IF_UNION_ICONSIZE ) );
			pos.x += 41;
		}
		pos.x = 21;
		pos.y += 40;
	}	

	GetSystem()->SetUnionMoney(0);

	if(GetSystem()->SendDataUniStore(0))
	{
		SetEnableWindow( false );
	}

	m_CollectItemTimeSeq.SetDeltaTime( 3000 );
	m_CollectItemTimeSeq.SetEnable();
}

void cUnionStore::Update(float const& fDeltaTime)
{
	m_pCollectBtn->SetEnable( m_bEnableCollectBtn && m_CollectItemTimeSeq.IsEnable() );
	m_pEmploymentBtn->SetEnable( GetSystem()->GetMapID() != 0 );

	m_pMoneyIF->SetMoney( GetSystem()->GetUnionMoney() );
	_UpdateMoveWindow();	
}

cBaseWindow::eMU_TYPE
cUnionStore::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if(m_pCancelBtn && m_pCancelBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pEmploymentBtn && m_pEmploymentBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pCollectBtn && m_pCollectBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	_UpdateIcon_ForMouse();

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( this->GetRoot()->GetSize().x, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();	

	return muReturn;
}

void cUnionStore::_UpdateIcon_ForMouse()
{
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	// 툴팁용
	for( int ec=0; ec<GetSystem()->GetItemCnt(); ++ec )
	{
		cItemInfo* pInfo = GetSystem()->GetItemInfo( ec );

		if( pInfo->IsEnable() == false )
			continue;

		if( m_IFIcon[ ec ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
		{
			CsPoint pos = m_IFIcon[ ec ].GetPos() + GetRootClient();
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->GetType(), cBaseWindow::WT_UNIONSTORE, 0, 0, pInfo );
			break;
		}
	}
}

void cUnionStore::Render()
{
	RenderScript();

	m_pMoneyIF->Render( IF_UNION_MONEY_POS + GetRootClient() );

	for( int ec=0; ec<GetSystem()->GetItemCnt(); ++ec )
	{
		cItemInfo* pInfo = GetSystem()->GetItemInfo( ec );

		if( pInfo->IsEnable() == false )
			continue;

		g_pIconMng->RenderItem( pInfo, m_IFIcon[ ec ].GetPos() + GetRootClient(), true );
	}

	EndRenderScript();
}

void cUnionStore::ResetDevice()
{	
	ResetDeviceScript();
	m_pMoneyIF->ResetDevice();
}

void cUnionStore::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
	m_pMoneyIF->ResetDevice();
}

void cUnionStore::CollectItem()
{
	SetEnableWindow( true );

	m_bActiveCollectBtn = false;
	m_bEnableCollectBtn = false;
	m_CollectItemTimeSeq.SetDisable();

 	if( !GetSystem()->IsEmptySlot() )
		m_bEnableCollectBtn = true;
}

void cUnionStore::PressCancelBtn( void* pSender, void* pData )
{
	Close();
}

void cUnionStore::PressEmploymentBtn( void* pSender, void* pData )
{
	if(GetSystem()->SendDataUniStore(1))
	{
		SetEnableWindow( false );
	}
}

void cUnionStore::PressCollecttBtn( void* pSender, void* pData )
{
	m_bActiveCollectBtn = true;
	SetEnableWindow( false );
	GetSystem()->SendDataUniStore(2);
}