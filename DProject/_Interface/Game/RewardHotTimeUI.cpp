#include "stdafx.h"
#include "RewardHotTimeUI.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"


cRewardHotTimeUI::cRewardHotTimeUI():m_pRewardGride(NULL)
{
	Construct();	
}

cRewardHotTimeUI::~cRewardHotTimeUI()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cRewardHotTimeUI::Create(cWindow* pkRoot, int nValue)
{
	IEventViewer::Create( pkRoot, nValue );

	{
		cText::sTEXTINFO	textInfo;
		textInfo.Init(  CFont::FS_14, NiColor(1.0f, 0.7f, 0.0f) );
		textInfo.SetText(UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT" ).c_str());
		textInfo.s_eTextAlign = DT_CENTER;
		AddText(&textInfo, CsPoint(274, 21));
	}

	{
		//접속 시간에 따라 아래 보상이 지급됩니다
		cText::sTEXTINFO	textInfo;
		textInfo.Init( CFont::FS_14, NiColor(0.5f, 0.8f, 1.0f) );
		textInfo.SetText(UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT_MESSAGE" ).c_str());
		textInfo.s_eTextAlign = DT_CENTER;
		AddText(&textInfo, CsPoint(274, 83));	
	}	

	{
		// 이벤트 기간
		cText::sTEXTINFO textInfo;
		textInfo.Init( CFont::FS_12, NiColor( 1, 0, 0 ) );
		textInfo.SetText( UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT_TIME_MSG" ).c_str() );
		textInfo.s_eTextAlign = DT_CENTER;
		m_pEventTimeText = AddText( &textInfo, CsPoint( 274, 110 ) );
	}

	{
		// 수령 버튼
		cText::sTEXTINFO textInfo;
		textInfo.Init( CFont::FS_10, NiColor::WHITE);
		textInfo.s_eTextAlign = DT_CENTER;
		textInfo.SetText( UISTRING_TEXT( "REWARD_BTN_ACCEPT" ).c_str() );

		m_pAcceptBtn = AddButton( CsPoint( 123, 350 ), CsPoint( 102, 38 ), CsPoint( 0, 48 ), "Event\\event_btn.tga" );
		if( m_pAcceptBtn )
		{
			m_pAcceptBtn->SetText( &textInfo );
			m_pAcceptBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cRewardHotTimeUI::PressAcceptButton );
		}
	}

	// 시스템한테 핫타임 이벤트 정보 받아서 갱신
	UpdateHotTimeItem();
}

BOOL cRewardHotTimeUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;

	if( m_pRewardGride && m_pRewardGride->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cGridListBoxItem const* pOverItem = m_pRewardGride->GetMouseOverItem();
		SAFE_POINTER_RETVAL( pOverItem, TRUE );

		sRewardItemData* pInfo = static_cast<sRewardItemData*>(pOverItem->GetUserData());
		SAFE_POINTER_RETVAL(pInfo, TRUE);

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, TRUE);

		if( 0 != pInfo->s_nItemCode )
			pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 44, 44 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->s_nItemCode, cBaseWindow::WT_STORE );
		return TRUE;
	}

	if( m_pAcceptBtn && m_pAcceptBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	return FALSE;
}

void cRewardHotTimeUI::UpdateHotTimeItem()
{
	EventContents::sHotTimeEventData const& EventInfo = GetSystem()->GetHotTimeEvent_Info();

	ReSizeGridList( 2 );
	SAFE_POINTER_RET( m_pRewardGride );
	m_pRewardGride->RemoveAllItem();

	AddGridItem( 0, EventInfo.bIsGetItem, EventInfo.pHotTimeCur );
	AddGridItem( 1, false, EventInfo.pHotTimeNext );

	m_pAcceptBtn->SetEnable( !EventInfo.bIsGetItem );

	// 현재 이벤트 없음
	if( 0 == EventInfo.nCurrentTableIdx )
	{
		m_pEventTimeText->SetText( UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT_TIME_END_MSG" ).c_str() );
		m_pRewardGride->RemoveAllItem();
		m_pAcceptBtn->SetVisible( false );
		return;
	}

	// 다음 보상이 없음
	if( 0 == EventInfo.nNextTableIdx )
	{
		m_pRewardGride->RemoveItem( 1 );
	}

	UpdateTimeMsg( EventInfo.szStartDate, EventInfo.szEndDate );
}

std::wstring cRewardHotTimeUI::SplitEventDate(wstring wsEventDate)
{
	std::wstring wsYear;
	std::wstring wsMonth;
	std::wstring wsDate;

	wsYear = DmCS::StringFn::tokenize( wsEventDate, L"-" );
	wsMonth = DmCS::StringFn::tokenize( wsEventDate, L"-" );
	wsDate = DmCS::StringFn::tokenize( wsEventDate, L" " );

	DmCS::StringFn::Add( wsYear, UISTRING_TEXT( "COMMON_YEAR" ) );
	DmCS::StringFn::Add( wsMonth, UISTRING_TEXT( "COMMON_MONTH" ) );
	DmCS::StringFn::Add( wsDate, UISTRING_TEXT( "COMMON_DATE" ) );

	DmCS::StringFn::Add( wsYear, wsMonth );
	DmCS::StringFn::Add( wsYear, wsDate );

	return wsYear;
}

std::wstring cRewardHotTimeUI::SplitRewardTime(wstring wsStartTime, wstring wsEndTime)
{
	wstring wsRewardTime = UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT_REWARD_ITEMTIME_TEXT" );

	DmCS::StringFn::ReplaceAll( wsRewardTime, L"#StartTime#", wsStartTime.substr( 0, 5 ) );	// 12:00:00 -> 12:00
	DmCS::StringFn::ReplaceAll( wsRewardTime, L"#EndTime#", wsEndTime.substr( 0, 5 ) );

	return wsRewardTime;
}

void cRewardHotTimeUI::PressAcceptButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( GetSystem() );
	GetSystem()->Send_HotTimeEvent_ItemGet();
}

void cRewardHotTimeUI::UpdateTimeMsg(std::wstring const& wsStartDate, std::wstring const& wsEndDate)
{
	std::wstring wsStart = wsStartDate;
	std::wstring wsEnd = wsEndDate;

	wsStart = SplitEventDate( wsStart );
	wsEnd = SplitEventDate( wsEnd );

	std::wstring wsEventTime = UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT_TIME_MSG" ).c_str();

	DmCS::StringFn::ReplaceAll( wsEventTime, L"#StartDate#", wsStart );
	DmCS::StringFn::ReplaceAll( wsEventTime, L"#EndDate#", wsEnd );

	m_pEventTimeText->SetText( wsEventTime.c_str() );
}

//////////////////////////////////////////////////////////////////////////
// 보상 품목 그리스 UI 사이즈 변경
void cRewardHotTimeUI::ReSizeGridList( int const& nItemCount )
{
	const int gapX = 150;
	CsPoint grideSize;
	grideSize.x = ( 128 + gapX ) * nItemCount - gapX;
	grideSize.y = 100;

	CsPoint pointSize= GetRootSize() + CsPoint( gapX, 0 );
	CsPoint gridePos;
	gridePos.x = (pointSize.x - grideSize.x) >> 1;
	gridePos.y = 245;
	if( NULL == m_pRewardGride )
	{
		m_pRewardGride = NiNew cGridListBox;
		if( m_pRewardGride )
		{
			m_pRewardGride->Init( NULL, gridePos, grideSize, CsPoint(gapX, 0), CsPoint(48, 48), 
				cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, nItemCount );
			m_pRewardGride->SetMouseOverImg( "Event\\event_select.tga" );
			m_pRewardGride->SetBackOverAndSelectedImgRender(false);
			m_pRewardGride->SetAutoSelection(false);
			AddChildControl( m_pRewardGride );
		}
	}
	else
	{
		m_pRewardGride->SetPos( gridePos );
		m_pRewardGride->SetSize( grideSize );
		m_pRewardGride->ResetDevice();
	}
}

// nItemNo : 0 = 오늘의 보상 / 1 = 다음 보상
void cRewardHotTimeUI::AddGridItem(int const& nItemNo, bool bItemGet, CsEventTable::sHotTimeEvent const* sEventInfo)
{
	SAFE_POINTER_RET( sEventInfo );

	if( 0 == sEventInfo->s_nItemNo || 0 == sEventInfo->s_nItemCnt )
		return;

	cString * pControlItem = NiNew cString;

	cSprite* pSlotBackImg = NiNew cSprite;
	if( pSlotBackImg )
	{
		pSlotBackImg->Init( NULL, CsPoint::ZERO, CsPoint(48,48), "Event\\event_slot_single.tga", false );
		cString::sSPRITE* pBack = pControlItem->AddSprite( pSlotBackImg );
		if( pBack )
			pBack->SetAutoPointerDelete(true);
	}

	cString::sICON*  pItemIcon = pControlItem->AddIcon( CsPoint(34,34), ICONITEM::ITEM_ICON, sEventInfo->s_nItemNo, sEventInfo->s_nItemCnt, CsPoint(7,7) );
	if( pItemIcon )
		pItemIcon->SetAutoPointerDelete(true);

	cSprite* pCheckImg = NiNew cSprite;
	if( pCheckImg )
	{
		pCheckImg->Init( NULL, CsPoint::ZERO, CsPoint(44,44), "Event\\event_check.tga", false );
		cString::sSPRITE*  pCompleteImg = pControlItem->AddSprite( pCheckImg, CsPoint(2,2) );
		if( pCompleteImg )
			pCompleteImg->SetAutoPointerDelete(true);

		if( bItemGet )
			pCompleteImg->SetVisible(true);
		else
			pCompleteImg->SetVisible(false);
	}
	
	{
		// 보상 텍스트
		cText::sTEXTINFO	textInfo;
		textInfo.Init( CFont::FS_14, NiColor(0.5f, 0.8f, 1.0f) );
		textInfo.s_eTextAlign = DT_CENTER;

		wstring wsRewardDay;
		if( nItemNo == 0 )	// 오늘의 보상
			wsRewardDay = UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT_REWARD_CURRENT_TEXT" );
		else				// 다음 보상
			wsRewardDay = UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT_REWARD_NEXT_TEXT" );
		
		textInfo.SetText( wsRewardDay.c_str() );
		cString::sTEXT* pName = pControlItem->AddText( &textInfo, CsPoint( 25, -75 ) );
		if( pName )
			pName->SetAutoPointerDelete(true);
	}

	{
		// 보상 요일 텍스트
		cText::sTEXTINFO	textInfo;
		textInfo.Init( CFont::FS_12, NiColor(0.5f, 0.8f, 1.0f) );
		textInfo.s_eTextAlign = DT_CENTER;	
		wstring wsRewardDay = GetWeekString( sEventInfo->s_nDay );
		textInfo.SetText( wsRewardDay.c_str() );
		cString::sTEXT* pName = pControlItem->AddText( &textInfo, CsPoint( 25, -50 ) );
		if( pName )
			pName->SetAutoPointerDelete(true);
	}

	{
		std::wstring wsItemName = GetSystem()->GetItemName( sEventInfo->s_nItemNo );
		NiColor niItemColor = GetSystem()->GetItemColor( sEventInfo->s_nItemNo );

		// 아이템 이름
		cText::sTEXTINFO	textInfo;
		textInfo.Init( CFont::FS_12, niItemColor );
		textInfo.s_eTextAlign = DT_CENTER;	
		textInfo.SetText( wsItemName.c_str() );
		cString::sTEXT* pName = pControlItem->AddText( &textInfo, CsPoint( 25, 50 ) );
		if( pName )
			pName->SetAutoPointerDelete( true );
	}

	{
		// 보상 시간
		cText::sTEXTINFO	textInfo;
		textInfo.Init( CFont::FS_12, NiColor::WHITE );
		textInfo.s_eTextAlign = DT_CENTER;		
		wstring wsRewardTime = SplitRewardTime( sEventInfo->s_szStartTime, sEventInfo->s_szEndTime );		
		textInfo.SetText( wsRewardTime.c_str() );
		cString::sTEXT* pName = pControlItem->AddText( &textInfo, CsPoint( 25, 75 ) );
		if( pName )
			pName->SetAutoPointerDelete( true );
	}
	
	cGridListBoxItem* pGridItem = NiNew cGridListBoxItem( nItemNo, CsPoint( 48, 48 ) );	
	pGridItem->SetItem( pControlItem );
	pGridItem->SetUserData( new IEventViewer::sRewardItemData( sEventInfo->s_nItemNo ) );
	
	m_pRewardGride->AddItem( pGridItem );
}

//////////////////////////////////////////////////////////////////////////
void cRewardHotTimeUI::RenderScript()
{
	cScriptUI::RenderScript();
}

std::wstring cRewardHotTimeUI::GetWeekString( int const& nWeek )
{
	std::wstring wsWeek;
	switch( nWeek )
	{
	case 0:
		DmCS::StringFn::Format( wsWeek, L"[%s]", UISTRING_TEXT( "COMMON_WEEK_SUNDAY").c_str() );
		break;
	case 1:
		DmCS::StringFn::Format( wsWeek, L"[%s]", UISTRING_TEXT( "COMMON_WEEK_MONDAY").c_str() );
		break;
	case 2:
		DmCS::StringFn::Format( wsWeek, L"[%s]", UISTRING_TEXT( "COMMON_WEEK_TUESDAY").c_str() );
		break;
	case 3:
		DmCS::StringFn::Format( wsWeek, L"[%s]", UISTRING_TEXT( "COMMON_WEEK_WEDNESDAY").c_str() );
		break;
	case 4:
		DmCS::StringFn::Format( wsWeek, L"[%s]", UISTRING_TEXT( "COMMON_WEEK_THURSDAY").c_str() );
		break;
	case 5:
		DmCS::StringFn::Format( wsWeek, L"[%s]", UISTRING_TEXT( "COMMON_WEEK_FRIDAY").c_str() );
		break;
	case 6:
		DmCS::StringFn::Format( wsWeek, L"[%s]", UISTRING_TEXT( "COMMON_WEEK_SATURDAY").c_str() );
		break;
	default:	return L"";
	}

	return wsWeek;
}

bool cRewardHotTimeUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eHotTimeEvent_Update, this );
	return true;
}

void cRewardHotTimeUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eHotTimeEvent_Update:	
		{
			UpdateHotTimeItem();
		}
		break;
	}
}
