#include "stdafx.h"
#include "RewardDailyUI.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

//////////////////////////////////////////////////////////////////////////

void cRewardDailyUI::sTimeStepUI::SetVisible( bool bValue )
{
	if( pBackImg )	pBackImg->SetVisible( bValue );
	if( pTimeText )	pTimeText->SetVisible( bValue );
}

void cRewardDailyUI::sTimeStepUI::SetComplete( bool bValue )
{
	if( pBackImg )	pBackImg->SetVisible( bValue );
	if( pTimeText )	pTimeText->SetAlpha( bValue ? 1.0f : 0.5f );
}

void cRewardDailyUI::sTimeStepUI::SetTimeText( int const& nValue )
{
	SAFE_POINTER_RET( pTimeText );
	std::wstring msg;
	DmCS::StringFn::Format( msg, L"%d%s", nValue, UISTRING_TEXT( "COMMON_TXT_SHORT_MIN" ).c_str() );
	pTimeText->SetText( msg.c_str() );
}

//////////////////////////////////////////////////////////////////////////

void cRewardDailyUI::sTimeLineUI::SetComplete( int const& nStep, bool bValue )
{
	std::map<int,sTimeStepUI>::iterator it = mapTimeStepUI.find( nStep );
	if( it == mapTimeStepUI.end() )
		return;

	it->second.SetComplete( bValue );
}

void cRewardDailyUI::sTimeLineUI::ResetComplete()
{
	std::map<int,sTimeStepUI>::iterator it = mapTimeStepUI.begin();
	for( ; it != mapTimeStepUI.end(); ++it )
		it->second.SetComplete( false );
}

size_t cRewardDailyUI::sTimeLineUI::GetTimeStepSize() const
{
	return mapTimeStepUI.size();
}

void cRewardDailyUI::sTimeLineUI::SetVisible( bool bValue )
{
	if( pTimeLineBackImg )		pTimeLineBackImg->SetVisible( bValue );
	std::map<int,sTimeStepUI>::iterator it = mapTimeStepUI.begin();
	for( ; it != mapTimeStepUI.end(); ++it )
		it->second.SetVisible( bValue );
}

void cRewardDailyUI::sTimeLineUI::SetTimeLineText( int const& nIdx, int const& nTimeValue )
{
	std::map<int,sTimeStepUI>::iterator it = mapTimeStepUI.begin();
	for( int n = 0; it != mapTimeStepUI.end(); ++it, ++n )
	{
		if( nIdx != n )
			continue;

		it->second.SetTimeText( nTimeValue );
	}
}

//////////////////////////////////////////////////////////////////////////

void cRewardDailyUI::sRewardItemIcon::SetVisible( bool bValue )
{
	if( s_gridItem )
		s_gridItem->SetEnable(bValue);
	if( s_pItemIcon )
		s_pItemIcon->SetVisible( bValue );
}

void cRewardDailyUI::sRewardItemIcon::SetItemData( int const& nItemCode, int const& nItemCount )
{
	if( s_pItemIcon )
		s_pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemCode, nItemCount );
	if( s_pUserData )
		s_pUserData->s_nItemCode = nItemCode;
}

void cRewardDailyUI::sRewardItemIcon::SetComplete(bool bValue)
{
	SAFE_POINTER_RET( s_pUserData );
	SAFE_POINTER_RET( s_pCompleteImg );
	
	if( 0 == s_pUserData->s_nItemCode )
	{
		SetVisible(false);
		s_pCompleteImg->SetVisible( false );
	}
	else
		s_pCompleteImg->SetVisible( bValue );
}

//////////////////////////////////////////////////////////////////////////

void cRewardDailyUI::sGridItemControl::ResetData()
{
	std::list< sRewardItemIcon >::iterator it = s_Controls.begin();
	for( ; it != s_Controls.end(); ++it )
		(*it).SetVisible(false);
}

void cRewardDailyUI::sGridItemControl::SetItemData(int const& nIdx, 
												   int const& nItemCode, 
												   int const& nItemCount, 
												   bool bComplete)
{
	std::list< sRewardItemIcon >::iterator it = s_Controls.begin();
	for( int n = 0; it != s_Controls.end(); ++it, ++n )
	{
		if( nIdx != n )
			continue;

		(*it).SetVisible( true );
		(*it).SetItemData( nItemCode, nItemCount );
		(*it).SetComplete( bComplete );
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
cRewardDailyUI::cRewardDailyUI():m_pRewardGride(NULL),m_nCurrentTimeTS(-1),m_nTotalPlayTime(0)
{
	Construct();	
}

cRewardDailyUI::~cRewardDailyUI()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cRewardDailyUI::Create(cWindow* pkRoot, int nValue)
{
	IEventViewer::Create( pkRoot, nValue );

	
	{	//접속시간 이벤트
		cText::sTEXTINFO	textInfo;
		textInfo.Init(  CFont::FS_14, NiColor(1.0f, 0.7f, 0.0f) );
		textInfo.SetText(UISTRING_TEXT( "REWARD_MENU_TEXT1" ).c_str());
		textInfo.s_eTextAlign = DT_CENTER;
		AddText(&textInfo, CsPoint(274, 21));
	}

	{
		//접속 시간에 따라 아래 보상이 지급됩니다
		cText::sTEXTINFO	textInfo;
		textInfo.Init( CFont::FS_14, NiColor(0.5f, 0.8f, 1.0f) );
		textInfo.SetText(UISTRING_TEXT( "REWARD_EXPLAIN_TEXT1" ).c_str());
		textInfo.s_eTextAlign = DT_CENTER;
		AddText(&textInfo, CsPoint(274, 83));	
	}

	MakeTimeLineUI(4);
	MakeTimeLineUI(5);

	int nTimeLineCount = GetSystem()->GetDailyEventTimeLineCount();
	SetTimeLineVisible( nTimeLineCount, true );
	MakeEmptyGridItem(nTimeLineCount);

	SetRewardGridItem();
}

void cRewardDailyUI::UpdateScript(float fDeltaTime)
{
	cScriptUI::UpdateScript( fDeltaTime );

	if( -1 != GetSystem()->GetDailyEventNo() )
	{
		int nCurrTS = GetSystem()->GetDailyEventPlayTimeMinute();
		if(m_nCurrentTimeTS != nCurrTS)
		{		
			m_nCurrentTimeTS = nCurrTS;
			UpdateTimeMsg( m_nCurrentTimeTS + m_nTotalPlayTime );
		}
	}

}

BOOL cRewardDailyUI::UpdateMouse()
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

	return FALSE;
}

void cRewardDailyUI::RenderScript()
{
	cScriptUI::RenderScript();
	if( IsVisible() )
		sDateText.Render(GetTotalPosition() + CsPoint( 272, 113 ), 2, DT_CENTER);
}

// 보상 품목 그리스 UI 사이즈 변경
void cRewardDailyUI::ReSizeGridList( int const& nTimeLineCount )
{
	CsPoint grideSize;
	grideSize.x = (48/*그리드 아이템 가로 사이즈*/ + 22/* 그리드 아이템 사이의 X Gab*/) * nTimeLineCount - 22/* 그리드 아이템 사이의 X Gab*/;
	grideSize.y = 148;

	CsPoint pointSize= GetRootSize();
	CsPoint gridePos;
	gridePos.x = (pointSize.x - grideSize.x) >> 1;
	gridePos.y = 248;
	if( NULL == m_pRewardGride )
	{
		m_pRewardGride = NiNew cGridListBox;
		if( m_pRewardGride )
		{
			m_pRewardGride->Init( NULL, gridePos, grideSize, CsPoint(22,2), CsPoint(48, 48), 
				cGridListBox::ColumnDownRight, cGridListBox::LeftTop, NULL, false, 0 );
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

// 보상 품목 업데이트 
void cRewardDailyUI::MakeEmptyGridItem(int const& nMakeLine)
{
	ReSizeGridList( nMakeLine );

	if( m_GridUIControl.size() == nMakeLine )
	{
		std::map<int, sGridItemControl>::iterator it = m_GridUIControl.begin();
		for( ; it != m_GridUIControl.end(); ++it )
			it->second.ResetData();
		return;
	}
	
	m_GridUIControl.clear();	
	SAFE_POINTER_RET( m_pRewardGride );
	m_pRewardGride->RemoveAllItem();

	for( int n = 0; n < nMakeLine; ++n )
	{
		sGridItemControl gridItems;
		for( int m = 0; m < 3; ++m )
		{
			cString * pControlItem = NiNew cString;

			cSprite* pSlotBackImg = NiNew cSprite;
			if( pSlotBackImg )
			{
				pSlotBackImg->Init( NULL, CsPoint::ZERO, CsPoint(48,48), "Event\\event_slot_single.tga", false );
				cString::sSPRITE* pBack = pControlItem->AddSprite( pSlotBackImg );
				if( pBack )
					pBack->SetAutoPointerDelete(true);
			}

			sRewardItemIcon rewardControl;
			rewardControl.s_pItemIcon = pControlItem->AddIcon( CsPoint(34,34), ICONITEM::ITEM_ICON, 0, 1, CsPoint(7,7) );
			if( rewardControl.s_pItemIcon )
				rewardControl.s_pItemIcon->SetAutoPointerDelete(true);

			cSprite* pCheckImg = NiNew cSprite;
			if( pCheckImg )
			{
				pCheckImg->Init( NULL, CsPoint::ZERO, CsPoint(44,44), "Event\\event_check.tga", false );
				rewardControl.s_pCompleteImg = pControlItem->AddSprite( pCheckImg, CsPoint(2,2) );
				if( rewardControl.s_pCompleteImg )
					rewardControl.s_pCompleteImg->SetAutoPointerDelete(true);
			}

			rewardControl.s_gridItem  = NiNew cGridListBoxItem( (n*3) + m, CsPoint( 48, 48 ));	
			rewardControl.s_pUserData = new sRewardItemData( 0 );
			rewardControl.s_gridItem->SetItem( pControlItem );
			rewardControl.s_gridItem->SetUserData(rewardControl.s_pUserData);

			gridItems.s_Controls.push_back( rewardControl );
			m_pRewardGride->AddItem( rewardControl.s_gridItem );
		}

		m_GridUIControl.insert( std::make_pair(n, gridItems) );
	}
}

void cRewardDailyUI::SetAllCompleteTimeMsg()
{
	sDateText.Delete();
	SAFE_POINTER_RET( g_pStringAnalysis );

	std::wstring textMsg = UISTRING_TEXT( "REWARD_DAILY_COMPLETE");
	cText::sTEXTINFO textInfo;
	textInfo.Init( CFont::FS_12, NiColor(0.5f, 0.8f, 1.0f) );
	g_pStringAnalysis->Cut_Parcing( &sDateText, 310, const_cast<TCHAR*>(textMsg.c_str()) , &textInfo );
}

void cRewardDailyUI::UpdateTimeMsg( int const& nDate )
{
	sDateText.Delete();

	SAFE_POINTER_RET( g_pStringAnalysis );

	std::wstring textMsg = UISTRING_TEXT( "REWARD_DAILY");
	DmCS::StringFn::ReplaceAll( textMsg, L"#time#", nDate);
	cText::sTEXTINFO textInfo;
	textInfo.Init( CFont::FS_12, NiColor(0.5f, 0.8f, 1.0f) );
	g_pStringAnalysis->Cut_Parcing( &sDateText, 310, const_cast<TCHAR*>(textMsg.c_str()) , &textInfo );
}

void cRewardDailyUI::SetRewardGridItem()
{
	int nWeekStart = GetSystem()->GetDailyEventWeekStartIdx();
	int nTimeLineCount = GetSystem()->GetDailyEventTimeLineCount();
	int nCompleteCount = GetSystem()->GetDailyEventCompleteCount();
	int nSumTime = 0;
	m_nTotalPlayTime = 0;
	for( int n = 0; n < nTimeLineCount; ++n )
	{
		CsEventTable::sEVENT* pItem = nsCsFileTable::g_pEventMng->GetMap( CsEventTable::ET_DAILY, nWeekStart + n);
		SAFE_POINTER_BEK( pItem );

		bool bComplete = false;
		if( n < nCompleteCount )
		{
			bComplete = true;
			m_nTotalPlayTime += pItem->s_nMinute;
		}

		nSumTime += pItem->s_nMinute;
		SetTimeLineText( nTimeLineCount, n, nSumTime );
		SetTimeLineComplete( nTimeLineCount, n, bComplete );

		std::map<int, sGridItemControl>::iterator it = m_GridUIControl.find( n );
		if( it == m_GridUIControl.end() )
			continue;

		for( int m = 0; m < 3; ++m )
			it->second.SetItemData( m, pItem->s_nItemID[m], pItem->s_nItemCnt[m], bComplete );		
	}

	if( nCompleteCount == nTimeLineCount )
		SetAllCompleteTimeMsg();
}

void cRewardDailyUI::SetTimeLineVisible(int const& nMakeTimeLineCount, bool bValue)
{
	std::map<int, sTimeLineUI>::iterator it = m_TimeLineUIControl.find( nMakeTimeLineCount );
	if( it == m_TimeLineUIControl.end() )
		return;
	it->second.SetVisible( bValue );
}

void cRewardDailyUI::SetTimeLineText( int const& nMakeTimeLineCount, int const& nIdx, int const& nValue )
{
	std::map<int, sTimeLineUI>::iterator it = m_TimeLineUIControl.find( nMakeTimeLineCount );
	if( it == m_TimeLineUIControl.end() )
		return;
	it->second.SetTimeLineText( nIdx, nValue );	
}

void cRewardDailyUI::SetTimeLineComplete( int const& nMakeTimeLineCount, int const& nIdx, bool const& bComplete )
{
	std::map<int, sTimeLineUI>::iterator it = m_TimeLineUIControl.find( nMakeTimeLineCount );
	if( it == m_TimeLineUIControl.end() )
		return;
	it->second.SetComplete( nIdx, bComplete );	
}


// 미리 타임 라인별 UI를 만들어 놓는 함수
void cRewardDailyUI::MakeTimeLineUI(int const& nMakeTimeLineCount)
{
	if( 4 == nMakeTimeLineCount )
	{
		sTimeLineUI addControl;
		addControl.pTimeLineBackImg = AddSprite(CsPoint( 155, 408 ), CsPoint( 240,  30 ), "Event\\event_glow_03.tga");	//월~금요일 진행바	

		for( int n = 0; n < nMakeTimeLineCount; ++n )
		{
			sTimeStepUI step;
			if( n == 0 )
				step.pBackImg = AddSprite(CsPoint( 156, 410 ), CsPoint(  26,  26 ), "Event\\event_glow_01.tga");	//진행바 첫번째	
			else if( n == 1 )
				step.pBackImg = AddSprite(CsPoint( 171, 410 ), CsPoint(  81,  26 ), "Event\\event_glow_02.tga");	//진행바 첫번째	
			else
				step.pBackImg = AddSprite(CsPoint( 171 + (n - 1) * 70, 410 ), CsPoint(  81,  26 ), "Event\\event_glow_02.tga");	//진행바 첫번째	
			
			{ // 진행 시간
				cText::sTEXTINFO	textInfo;
				textInfo.Init( CFont::FS_12, NiColor( 1.0f, 0.7f, 0.0f ) );
				std::wstring msg;
				DmCS::StringFn::Format( msg, L"%d%s", (n+1) *30, UISTRING_TEXT( "COMMON_TXT_SHORT_MIN" ).c_str() );
				textInfo.SetText( msg.c_str() );
				textInfo.s_eTextAlign = DT_CENTER;
				step.pTimeText = AddText(&textInfo, CsPoint(169 + (n *70), 443));		
			} 
			addControl.mapTimeStepUI.insert( std::make_pair( n, step) );
		}

		addControl.SetVisible(false);
		m_TimeLineUIControl.insert( std::make_pair(nMakeTimeLineCount, addControl) );

	}
	else if( 5 == nMakeTimeLineCount )
	{
		sTimeLineUI addControl;
		addControl.pTimeLineBackImg = AddSprite(CsPoint( 119, 408 ), CsPoint( 310,  30 ), "Event\\event_glow_04.tga");	//토~일요일 진행바

		for( int n = 0; n < nMakeTimeLineCount; ++n )
		{
			sTimeStepUI step;
			if( n == 0 )
				step.pBackImg = AddSprite(CsPoint( 120, 410 ), CsPoint(  26,  26 ), "Event\\event_glow_01.tga");	//진행바 첫번째	
			else if( n == 1 )
				step.pBackImg = AddSprite(CsPoint( 135, 410 ), CsPoint(  81,  26 ), "Event\\event_glow_02.tga");	//진행바 첫번째	
			else
				step.pBackImg = AddSprite(CsPoint( 135 + (n - 1) * 70, 410 ), CsPoint(  81,  26 ), "Event\\event_glow_02.tga");	//진행바 첫번째	

			// 진행 시간
			cText::sTEXTINFO	textInfo;
			textInfo.Init( CFont::FS_12, NiColor( 1.0f, 0.7f, 0.0f ) );
			std::wstring msg;
			DmCS::StringFn::Format( msg, L"%d%s", (n+1) *30, UISTRING_TEXT( "COMMON_TXT_SHORT_MIN" ).c_str() );
			textInfo.SetText( msg.c_str() );
			textInfo.s_eTextAlign = DT_CENTER;
			step.pTimeText = AddText(&textInfo, CsPoint(133 + (n *70), 443));		
			 
			addControl.mapTimeStepUI.insert( std::make_pair( n, step ) );
		}
		addControl.SetVisible(false);
		m_TimeLineUIControl.insert( std::make_pair(nMakeTimeLineCount, addControl) );
	}
}


//////////////////////////////////////////////////////////////////////////

bool cRewardDailyUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eDailyEvent_Update, this );
	return true;
}

void cRewardDailyUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 
	switch( iNotifiedEvt )
	{
	case SystemType::eDailyEvent_Update:	
		{
			SetTimeLineVisible(4,false);
			SetTimeLineVisible(5,false);
			int nTimeLineCount = GetSystem()->GetDailyEventTimeLineCount();
			SetTimeLineVisible( nTimeLineCount, true );
			MakeEmptyGridItem(nTimeLineCount);
			SetRewardGridItem();
		}break;
	}
}
