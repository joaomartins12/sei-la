#include "stdafx.h"
#include "RewardDailyCheckUI.h"

cRewardDailyCheckUI::cRewardDailyCheckUI() :
m_pTitle(NULL),
m_pDescript(NULL),
m_pEventDate(NULL),
m_pRewardGrid(NULL),
m_nSubIndex( 0 )
{
	Construct();
}

cRewardDailyCheckUI::~cRewardDailyCheckUI()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cRewardDailyCheckUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eDailyCheckEvent_UpdateItem:
		_SetRewardItemGrid();
		break;
	case SystemType::eDailyCheckEvent_NotTime:
		{
			if( m_pRewardGrid )
			{
				m_pRewardGrid->RemoveAllItem();
				m_pRewardGrid->SetVisible( false );
			}
			if( m_pEventDate )		// 이미 이벤트가 종료되었습니다.
				m_pEventDate->SetText( UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT_TIME_END_MSG" ).c_str() );
		}
		break;
	}
}

bool cRewardDailyCheckUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eDailyCheckEvent_UpdateItem, this );
	GetSystem()->Register( SystemType::eDailyCheckEvent_NotTime,	this );

	return true;
}

void cRewardDailyCheckUI::Create(cWindow* pkRoot, int nValue /*= 0 */)
{
	InitScript( pkRoot, NULL, CsPoint( 224, 57 ), CsPoint( 550, 500 ), false );
	AddSprite( CsPoint::ZERO, CsPoint( 548, 499 ), "Event\\event_popup_new.tga" );

	{	// 이벤트 명
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_16, NiColor( 1.0f, 0.7f, 0.0f ) );
		ti.SetText( L" " );
		ti.s_eTextAlign = DT_CENTER;
		m_pTitle = AddText( &ti, CsPoint( 274, 21 ) );
	}

	{
		// 이벤트 내용
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor(0.5f, 0.8f, 1.0f) );
		ti.SetText( L" " );
		ti.s_eTextAlign = DT_CENTER;
		m_pDescript = AddText( &ti, CsPoint( 274, 70 ) );
	}

	{	// 이벤트 기간
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 1.0f, 0.0f, 0.0f ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( L" " );
		m_pEventDate = AddText( &ti, CsPoint( 274, 100 ) );
	}

	_MakeRewardItemGrid();
}

BOOL cRewardDailyCheckUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;

	if( m_pRewardGrid && m_pRewardGrid->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cGridListBoxItem const* pOverItem = m_pRewardGrid->GetMouseOverItem();
		SAFE_POINTER_RETVAL( pOverItem, TRUE );

		sRewardItemData* pInfo = static_cast< sRewardItemData* >( pOverItem->GetUserData() );
		SAFE_POINTER_RETVAL( pInfo, TRUE );

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL( pToolTip, TRUE );

		if( 0 != pInfo->s_nItemCode )
			pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 44, 44 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->s_nItemCode, cBaseWindow::WT_STORE );

		return TRUE;
	}

	return FALSE;
}

void cRewardDailyCheckUI::SetSubViewer(int const& nSubType)
{
	if( 0 > nSubType )
		return;

	m_nSubIndex = nSubType;

	std::wstring wsTitle; std::wstring wsDesc;
	std::wstring wsStart; std::wstring wsEnd;
	GetSystem()->GetDailyCheckEventInfo( m_nSubIndex, wsTitle, wsDesc, wsStart, wsEnd );

	if( m_pTitle )
		m_pTitle->SetText( wsTitle.c_str() );
	if( m_pDescript )
		m_pDescript->SetText( wsDesc.c_str() );
	if( m_pEventDate )
	{
		_ConvertEventDateFormat( wsStart );
		_ConvertEventDateFormat( wsEnd );

		// 이벤트 기간: #StartDate# ~ #EndDate#
		std::wstring wsDate = UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT_TIME_MSG" );
		DmCS::StringFn::Replace( wsDate, L"#StartDate#", wsStart.c_str() );
		DmCS::StringFn::Replace( wsDate, L"#EndDate#", wsEnd.c_str() );
		m_pEventDate->SetText( wsDate.c_str() );
	}

	_SetRewardItemGrid();
}

void cRewardDailyCheckUI::_ConvertEventDateFormat(std::wstring& wsEventDate)
{
	std::wstring wsYear;
	std::wstring wsMonth;
	std::wstring wsDate;

	wsYear = DmCS::StringFn::tokenize( wsEventDate, L"-" );
	wsMonth = DmCS::StringFn::tokenize( wsEventDate, L"-" );
	wsDate = DmCS::StringFn::tokenize( wsEventDate, L" " );

	DmCS::StringFn::Add( wsYear, wsMonth, UISTRING_TEXT( "COMMON_YEAR" ) );
	DmCS::StringFn::Add( wsYear, wsDate, UISTRING_TEXT( "COMMON_MONTH" ) );
	DmCS::StringFn::Add( wsYear, UISTRING_TEXT( "COMMON_DATE" ) );

	wsEventDate = wsYear;
}

void cRewardDailyCheckUI::_MakeRewardItemGrid()
{
	if( !m_pRewardGrid )
	{
		m_pRewardGrid = NiNew cGridListBox;
		SAFE_POINTER_RET( m_pRewardGrid );

		CsPoint gridPos( 83, 190 );
		CsPoint gridSize( 405, 270 );
		CsPoint gridItemGab( 12, 5 );
		CsPoint gridItemSize( 44, 50 );
		m_pRewardGrid->Init( m_pRoot, gridPos, gridSize, CsPoint::ZERO, CsPoint( 44, 44 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, false, 7 );
		m_pRewardGrid->SetMouseOverImg( "Event\\event_select.tga" );
		m_pRewardGrid->SetAutoSelection( false );
		m_pRewardGrid->SetBackOverAndSelectedImgRender( false );
		m_pRewardGrid->SetItemGab( gridItemGab );
		m_pRewardGrid->SetItemSize( gridItemSize );
		cScrollBar* pScroll = NiNew cScrollBar;
		if( pScroll )
		{
			gridSize.y += 30;
			pScroll->Init( cScrollBar::TYPE_1, NULL, CsPoint::ZERO, CsPoint( 16, gridSize.y + 14 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, gridSize ), 4, false );
			m_pRewardGrid->SetScrollBar( pScroll );
		}

		AddChildControl( m_pRewardGrid );
	}
}

void cRewardDailyCheckUI::_SetRewardItemGrid()
{
	SAFE_POINTER_RET( m_pRewardGrid );
	m_pRewardGrid->RemoveAllItem();

	SystemType::sCheckGroupInfo const* pEventGroup = GetSystem()->GetDailyCheckGroupInfo( m_nSubIndex );
	SAFE_POINTER_RET( pEventGroup );

	int nCheckIndex = pEventGroup->GetCheckIndex() - 1;

	for( int i = 0; i < pEventGroup->GetCheckItemListCount(); ++i )
	{
		SystemType::sCheckItemInfo const* pRewardItem = pEventGroup->GetCheckItemInfo( i + 1 );
		SAFE_POINTER_CON( pRewardItem );
		std::map< uint, uint >::const_iterator cit = pRewardItem->GetCheckItems().begin();
		if( cit == pRewardItem->GetCheckItems().end() )
			continue;

		_AddRewardItem( i, cit->first, cit->second, nCheckIndex,
						pRewardItem->GetCheckDate(), pRewardItem->GetCheckState() );
	}

	m_pRewardGrid->SetScrollBarPos( nCheckIndex - 1 );
	m_pRewardGrid->SetVisible( true );
}

void cRewardDailyCheckUI::_AddRewardItem(int const& nIndex, uint const& nItemID, uint const& nItemCount,
										 uint const& nTodayIndex, std::wstring const& wsDate, bool const& bCheck)
{
	SAFE_POINTER_RET( m_pRewardGrid );

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	// 슬롯
	cSprite* pSlot = NiNew cSprite;
	if( pSlot )
	{
		pSlot->Init( NULL, CsPoint::ZERO, CsPoint( 62, 75 ), "Event\\event_bg_new.tga", false );
		cString::sSPRITE* pImg = pControl->AddSprite( pSlot, CsPoint( -10, -10 ) );
		if( pImg )
			pImg->SetAutoPointerDelete( true );
	}

	// 아이템 아이콘
	cString::sICON* pIcon = pControl->AddIcon( CsPoint( 42, 42 ), ICONITEM::ITEM_ICON, nItemID, nItemCount, CsPoint( 1, 1 ) );
	if( pIcon )
		pIcon->SetAutoPointerDelete( true );

	// 출석 표시
	if( bCheck )
	{
		cSprite* pCheck = NiNew cSprite;
		if( pCheck )
		{
			pCheck->Init( NULL, CsPoint::ZERO, CsPoint( 44, 44 ), "Event\\event_check_new_01.tga", false );
			cString::sSPRITE* pImg = pControl->AddSprite( pCheck );
			if( pImg )
				pImg->SetAutoPointerDelete(true);
		}
	}
	else
	{
		// 결석 표시
		if( nIndex < nTodayIndex )
		{
			cSprite* pMiss = NiNew cSprite;
			if( pMiss )
			{
				pMiss->Init( NULL, CsPoint::ZERO, CsPoint( 44, 44 ), "Event\\event_check_new_02.tga", false );
				cString::sSPRITE* pImg = pControl->AddSprite( pMiss );
				if( pImg )
					pImg->SetAutoPointerDelete(true);
			}
		}
	}

	// 출석 당일
	if( nIndex == nTodayIndex )
	{
		cSprite* pToday = NiNew cSprite;
		if( pToday )
		{
			pToday->Init( NULL, CsPoint::ZERO, CsPoint( 50, 50 ), "Event\\event_frame.tga", false );
			cString::sSPRITE* pImg = pControl->AddSprite( pToday, CsPoint( -3, -3 ) );
			if( pImg )
				pImg->SetAutoPointerDelete(true);
		}
	}

	{	// 출석일
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( wsDate.c_str() );
		CsPoint datePos( 22, 47 );
		cString::sTEXT* pText = pControl->AddText( &ti, datePos );
		if( pText )
			pText->SetAutoPointerDelete( true );
	}

	cGridListBoxItem* pGridItem = NiNew cGridListBoxItem( nIndex, CsPoint( 44, 44 ) );
	if( pGridItem )
	{
		pGridItem->SetItem( pControl );
		pGridItem->SetUserData( new sRewardItemData( nItemID ) );
		m_pRewardGrid->AddItem( pGridItem );
	}
}
