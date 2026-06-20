#include "StdAfx.h"

#include "DataTradeContents.h"
#include "DataTradeSelect.h"

#define	SIZE_WIDTH	798
#define	SIZE_HEIGHT	480
#define	SIZE_BUTTON_WIDTH	(230)
#define	SIZE_BUTTON_HEIGHT	(256)
#define USE_SIZE	CsPoint(32, 32)

#define TRADE_BUTTON_SIZE	CsPoint(120, 54)

DataTradeSelect::DataTradeSelect(void) : 
m_pCloseButton(NULL),
m_pExitButton(NULL),
m_pSelectGrid(NULL)
{
}

DataTradeSelect::~DataTradeSelect(void)
{

}
void DataTradeSelect::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void DataTradeSelect::DeleteResource()
{
	DeleteScript();
}

void DataTradeSelect::ResetDevice()
{
	ResetDeviceScript();
}

void DataTradeSelect::Create( int nValue /*=0*/ )
{
	if( IsLive() )
		return;

	if( !cBaseWindow::Init() )
		return;

	CsPoint CenterPos = CsPoint( (int)( ( g_nScreenWidth - SIZE_WIDTH )*0.5f ), (int)( ( g_nScreenHeight - SIZE_HEIGHT )*0.5f ) );

	InitScript( "DataExchange\\Data_Exchange_Choice_BG.tga", CenterPos, CsPoint(SIZE_WIDTH,SIZE_HEIGHT), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false);
	AddTitle( UISTRING_TEXT( "DATA_TRADE_TITLE_SELECT" ).c_str(), CsPoint::ZERO, CFont::FS_11 );

	_MakeSelectInfo();

	// 닫기 버튼
	m_pCloseButton = AddButton( CsPoint( SIZE_WIDTH-16-10, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );

	// 나가기 버튼
	m_pExitButton = AddButton( CsPoint(SIZE_WIDTH/2-TRADE_BUTTON_SIZE.x/2, SIZE_HEIGHT+(int)TRADE_BUTTON_SIZE.y*0.5f), TRADE_BUTTON_SIZE, TRADE_BUTTON_SIZE, "DataExchange\\Data_Exchange_Button.tga");
	if( m_pExitButton )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DATA_TRADE_BUTTON_EXIT" ).c_str() );
		m_pExitButton->SetText( &ti );
		m_pExitButton->SetPosText( m_pExitButton->GetPos()+TRADE_BUTTON_SIZE/2 );
	}
}

void DataTradeSelect::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );
}

cBaseWindow::eMU_TYPE
DataTradeSelect::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if( m_pCloseButton && m_pCloseButton->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		GetSystem()->CloseSelect(true);
		return muReturn;
	}

	if( m_pExitButton && m_pExitButton->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		GetSystem()->CloseSelect(true);
		return muReturn;
	}

	if( m_pSelectGrid && m_pSelectGrid->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		_SetTooltip( m_pSelectGrid->GetMouseOverItem() );
		return muReturn;
	}

	return muReturn;
}

void DataTradeSelect::Render()
{
	RenderScript();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool DataTradeSelect::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void DataTradeSelect::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void DataTradeSelect::_MakeSelectInfo()
{
	if( !m_pSelectGrid )
	{
		m_pSelectGrid = NiNew cGridListBox;
		SAFE_POINTER_RET( m_pSelectGrid );
		AddChildControl( m_pSelectGrid );
	}

	m_pSelectGrid->Init( m_pRoot, CsPoint( 0, 34 ), CsPoint( 810, 452 ), CsPoint( 8, 0 ), CsPoint( 255, 452 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, false, SystemType::MAX_TRADE );
	m_pSelectGrid->SetMargin( CsPoint( 8, 0 ) );
	m_pSelectGrid->SetAutoSelection( false );
	m_pSelectGrid->RemoveAllItem();
	m_mapSelectInfo.clear();

	for( int i = SystemType::eTRADE_SCALE; i < SystemType::MAX_TRADE; ++i )
		_AddSelectInfo( i );
}

void DataTradeSelect::_AddSelectInfo(int const& nState)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	sSelectInfo sSelInfo;

	{	// 데이터 교환 타이틀
		std::wstring wsTitle;
		switch( nState )
		{
		case SystemType::eTRADE_SCALE:		wsTitle = UISTRING_TEXT( "DATA_TRADE_TITLE_SCALE" );	break;
		case SystemType::eTRADE_UPGRADE:	wsTitle = UISTRING_TEXT( "DATA_TRADE_TITLE_UPGRADE" );	break;
		case SystemType::eTRADE_EVOLUTION:	wsTitle = UISTRING_TEXT( "DATA_TRADE_TITLE_EVOLUTION" );break;
		}
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( wsTitle.c_str() );
		sSelInfo.s_pSelectTxt = pControl->AddText( &ti, CsPoint( 128, 16 ) );
		if( sSelInfo.s_pSelectTxt )
			sSelInfo.s_pSelectTxt->SetAutoPointerDelete( true );
	}

	// 데이터 교환 애니 버튼
	cAniButton* pAniBtn = NiNew cAniButton;
	if( pAniBtn )
	{
		std::string strFile;
		switch( nState )
		{
		case SystemType::eTRADE_SCALE:		strFile = "DataExchange\\Data_Exchange_Choice_Data.tga";		break;
		case SystemType::eTRADE_UPGRADE:	strFile = "DataExchange\\Data_Exchange_Choice_Enchant.tga";		break;
		case SystemType::eTRADE_EVOLUTION:	strFile = "DataExchange\\Data_Exchange_Choice_Evolution.tga";	break;
		}
		pAniBtn->Init( NULL, CsPoint::ZERO, CsPoint( SIZE_BUTTON_WIDTH, SIZE_BUTTON_HEIGHT ), strFile.c_str(), false );
		pAniBtn->SetTexToken( CsPoint( 230, 0 ) );
		pAniBtn->SetAniStartEnd( 1, 4 );
		pAniBtn->SetAniTime( 0.15f );
		pAniBtn->SetAniLoop( false );
		pAniBtn->SetAniState( cAniButton::MOUSE_ON );
		pAniBtn->AddEvent( cAniButton::BUTTON_LBUP_EVENT, this, &DataTradeSelect::_OnClickSelectButton );
		pAniBtn->SetUserData( new sSelectState( nState ) );
		sSelInfo.s_pSelectBtn = pControl->AddAniButton( pAniBtn, nState, CsPoint( 13, 60 ) );
		if( sSelInfo.s_pSelectBtn )
			sSelInfo.s_pSelectBtn->SetAutoPointerDelete( true );
	}

	{	// 필요 아이템
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DATA_TRADE_USE_ITEM" ).c_str() );
		cString::sTEXT* pTxt = pControl->AddText( &ti, CsPoint( 128, 338 ) );
		if( pTxt )
			pTxt->SetAutoPointerDelete( true );
	}

	// 필요 아이템 아이콘
	int nItemCode = GetSystem()->GetItemCode( static_cast<SystemType::eSTATE>(nState) );
	sSelInfo.s_pItemIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, nItemCode, 1, CsPoint( 112, 364 ) );
	if( sSelInfo.s_pItemIcon )
		sSelInfo.s_pItemIcon->SetAutoPointerDelete( true );

	{	// 필요 아이템 개수
		std::wstring wsCount;
		DmCS::StringFn::Format( wsCount, L"x %d", GetSystem()->GetRequireItemCount( static_cast<SystemType::eSTATE>(nState) ) );

		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, NiColor::WHITE );
		ti.s_eTextAlign = DT_LEFT | DT_VCENTER;
		ti.SetText( wsCount.c_str() );
		cString::sTEXT* pTxt = pControl->AddText( &ti, CsPoint( 155, 379 ) );
		if( pTxt )
			pTxt->SetAutoPointerDelete( true );
	}

	cGridListBoxItem* pGridItem = NiNew cGridListBoxItem;
	if( pGridItem )
	{
		pGridItem->SetItem( pControl );
		pGridItem->SetUserData( new sSelectState( nState ) );
		m_pSelectGrid->AddItem( pGridItem );
		m_mapSelectInfo.insert( std::make_pair( nState, sSelInfo ) );
	}
}

void DataTradeSelect::_SetTooltip(cGridListBoxItem const* pOverItem)
{
	SAFE_POINTER_RET( pOverItem );
	sSelectState* pState = dynamic_cast< sSelectState* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pState );

	std::map< int, sSelectInfo >::iterator it = m_mapSelectInfo.find( pState->s_nSelectState );
	if( it == m_mapSelectInfo.end() )
		return;

	cString::sICON* pItemIcon = it->second.s_pItemIcon;
	SAFE_POINTER_RET( pItemIcon );
	if( !pItemIcon->m_MouseOverCheckRect.PtInRect( CURSOR_ST.GetPos() ) )
		return;

	cItemInfo itemInfo;
	itemInfo.SetType( GetSystem()->GetItemCode( static_cast<SystemType::eSTATE>(pState->s_nSelectState) ) );
	itemInfo.SetCount( GetSystem()->GetInvenItemCount( static_cast<SystemType::eSTATE>(pState->s_nSelectState) ) );

	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );
	pTooltip->SetTooltip( pItemIcon->GetWorldPos(), CsPoint( 34, 34 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, itemInfo.m_nType, cBaseWindow::WT_INVENTORY, 0, 0, &itemInfo );
}

void DataTradeSelect::_OnClickSelectButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cAniButton* pButton = static_cast< cAniButton* >( pSender );

	sSelectState* pState = dynamic_cast< sSelectState* >( pButton->GetUserData() );
	SAFE_POINTER_RET( pState );

	GetSystem()->OpenTrade( pState->s_nSelectState );
}
