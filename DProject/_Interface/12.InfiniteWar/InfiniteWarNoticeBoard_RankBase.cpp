#include "stdafx.h"
#include "InfiniteWarNoticeBoard_RankBase.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CInfinitewarNoticeBoardRankBaseUI::CInfinitewarNoticeBoardRankBaseUI(int const& nType)
:m_pRankList(NULL),m_pRankGetItemView(NULL),m_pLastRankView(NULL)
,m_pTimerBar(NULL),m_pRewardItemLIst(NULL),m_pMyCurrentRank(NULL)
,m_pMyCurrentPoint(NULL),m_pMyUpDownRank(NULL),m_pDataUpdateTimerBar(NULL)
,m_pRankLimitTime(NULL),m_nRankingType(nType),m_pRankUpdateItemText(NULL),m_pNextRankLimitTime(NULL)
,m_pNoNextRankTextMsg(NULL)
{
}

CInfinitewarNoticeBoardRankBaseUI::~CInfinitewarNoticeBoardRankBaseUI()
{ 
	Destroy(); 
}

void CInfinitewarNoticeBoardRankBaseUI::Destroy()
{
	DeleteScript();
}

bool CInfinitewarNoticeBoardRankBaseUI::Create(cWindow* pRoot)
{
	InitScript( pRoot, NULL, CsPoint( 24, 104 ), CsPoint( 972, 487 ), false );
	AddSprite( CsPoint(714, 0), CsPoint( 258, 487 ), "InfiniteNoticeBoard\\Infinite_NoticeBoard_Today_RBack.tga" );

	AddSprite( CsPoint(9,8), CsPoint( 658, 38 ), "InfiniteNoticeBoard\\Infinite_Borad_top_Rankinglist.tga" );

	{// L"랭킹"
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_RANK_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 35, 20 ) );
	}

	{//L"테이머 이름"
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_TAMERNAME_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 164, 20 ) );
	}

	{// L"소속 길드"
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_GUILD_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 385, 20 ) );
	}

	{//L"포인트"
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_POINT_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 542, 20 ) );
	}

	{//L"등락"
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_UPDOWN_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 626, 20 ) );
	}

	m_pRankList = NiNew cListBox;
	if( m_pRankList )
	{
		m_pRankList->Init( m_pRoot, CsPoint(11, 50), CsPoint( 680, 430 ), NULL, false );
		m_pRankList->SetItemHeight( 43 );
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint( 0, 0 ), CsPoint( 16, 430 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint( 680, 430 ) ), 10, false );
			m_pRankList->SetScrollBar( pScrollBar );
		}

		AddChildControl( m_pRankList );
	}

	
	{	// 랭킹 수집 기간이 아닙니다.
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_20, NiColor(1.0f,1.0f,0.0f));
		ti.SetText( L" " );
		ti.s_eTextAlign = DT_CENTER| DT_VCENTER ;
		m_pNoNextRankTextMsg = AddText( &ti, CsPoint( 351, 265 ) );
		if( m_pNoNextRankTextMsg )
			m_pNoNextRankTextMsg->SetVisible(false);
	}

	{	// 나의 랭킹(상위 99%)
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( L" " );
		ti.s_eTextAlign = DT_CENTER;
		m_pMyCurrentRank = AddText( &ti, CsPoint( 842, 12 ) );
	}

	{	// 나의 현재 포인트
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_24, NiColor(0.0f,1.0f,0.0f) );
		ti.SetText( L" " );
		ti.s_eTextAlign = DT_CENTER;
		m_pMyCurrentPoint = AddText( &ti, CsPoint( 843, 65 ) );	
	}

	{	// 순위
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, NiColor(1.0f,0.0f,0.0f) );
		ti.SetText( L" " );
		ti.s_eTextAlign = DT_CENTER;
		m_pMyUpDownRank = AddText( &ti, CsPoint( 843, 103 ) );	
	}

	{//L"현재 랭킹 유지시 획득 보상"
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_GETREWARDITEM_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER| DT_VCENTER ;
		AddText( &ti, CsPoint( 842, 142 ) );
	}

	m_pRewardItemLIst = NiNew cGridListBox;
	if( m_pRewardItemLIst )
	{
		m_pRewardItemLIst->Init( GetRoot(), CsPoint( 771, 167 ), CsPoint(142, 103)
			, CsPoint::ZERO, CsPoint(44, 44), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		AddChildControl(m_pRewardItemLIst);
	}


	m_pRankGetItemView = AddButton( CsPoint(727,276), CsPoint( 230, 54 ), CsPoint( 0, 54 ), "CommonUI\\Menu_btn.tga", cWindow::SD_Bu3 );
	if( m_pRankGetItemView )
	{//순위별 보상 아이템 확인	
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText( UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_GETREWARDITEM_BUTTON").c_str() );	
		ti.s_eTextAlign = DT_CENTER;
		m_pRankGetItemView->SetText( &ti );
	}

	m_pLastRankView = AddButton( CsPoint(727,330), CsPoint( 230, 54 ), CsPoint( 0, 54 ), "CommonUI\\Menu_btn.tga", cWindow::SD_Bu3 );
	if( m_pLastRankView )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText( L" " );
		ti.s_eTextAlign = DT_CENTER;
		m_pLastRankView->SetText( &ti );//L"전주 랭킹 확인"
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( L" " );//주간 랭킹 마감까지
		ti.s_eTextAlign = DT_CENTER;
		m_pRankLimitTime = AddText( &ti, CsPoint( 843, 390 ) );	
	}
	{

		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( L" " );//다음 주간 랭킹 시작까지 남은 시간
		ti.s_eTextAlign = DT_CENTER;
		m_pNextRankLimitTime = AddText( &ti, CsPoint( 843, 390 ) );		
		if( m_pNextRankLimitTime )
			m_pNextRankLimitTime->SetVisible(false);
	}

	m_pTimerBar = NiNew cUI_TimerProgressBar;
	if( m_pTimerBar )
	{
		m_pTimerBar->Init(GetRoot(), CsPoint(725, 415), CsPoint(243, 20), false);

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_20 );
		ti.s_Color = NiColor(0.86f,0.45f,0.0f);
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		ti.SetText(L"00 : 00 : 00");
		m_pTimerBar->SetStringControl( &ti );
		m_pTimerBar->SetShowValueType( cUI_TimerProgressBar::eRemainingTime );			
		m_pTimerBar->SetTimerText( L" : ", L" : ", L" : ", L"");
		m_pTimerBar->SetTimerType(cUI_TimerProgressBar::eHourMinSec);
		m_pTimerBar->SetRenderText(true);
		m_pTimerBar->AddEvent( cUI_ProgressBar::eProgressbar_Done, this, &CInfinitewarNoticeBoardRankBaseUI::_SendRankingDeadline );
		AddChildControl( m_pTimerBar );
	}

	{//랭킹 갱신 시간
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_RESET_TIME_TEXT").c_str() );		
		ti.s_eTextAlign = DT_CENTER;
		m_pRankUpdateItemText = AddText( &ti, CsPoint( 843, 440 ) );	
	}

	m_pDataUpdateTimerBar = NiNew cUI_TimerProgressBar;
	if( m_pDataUpdateTimerBar )
	{
		m_pDataUpdateTimerBar->Init(GetRoot(), CsPoint(725, 460), CsPoint(243, 20), false);

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_20 );
		ti.s_Color = NiColor(0.86f,0.45f,0.0f);
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		ti.SetText(L"00 : 00 : 00");
		m_pDataUpdateTimerBar->SetStringControl( &ti );
		m_pDataUpdateTimerBar->SetShowValueType( cUI_TimerProgressBar::eRemainingTime );			
		m_pDataUpdateTimerBar->SetTimerText( L" : ", L" : ", L" : ", L"");
		m_pDataUpdateTimerBar->SetTimerType(cUI_TimerProgressBar::eHourMinSec);
		m_pDataUpdateTimerBar->SetRenderText(true);
		m_pDataUpdateTimerBar->AddEvent( cUI_ProgressBar::eProgressbar_Done, this, &CInfinitewarNoticeBoardRankBaseUI::_SendRequestRankingDataUpdate );
		AddChildControl( m_pDataUpdateTimerBar );
	}
	return true;
}

BOOL CInfinitewarNoticeBoardRankBaseUI::UpdateMouse()
{
	if( !IsVisible()  || !IsEnableWindow() )
		return FALSE;

	if( m_pRankList && m_pRankList->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return TRUE;

	if( m_pRankGetItemView && m_pRankGetItemView->Update_ForMouse() )
		return TRUE;

	if( m_pLastRankView && m_pLastRankView->Update_ForMouse() )
		return TRUE;

	if( m_pRewardItemLIst && m_pRewardItemLIst->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cGridListBoxItem const* pOverItem = m_pRewardItemLIst->GetMouseOverItem();
		if( pOverItem )
		{
			sRewardItemData* pInfo = dynamic_cast<sRewardItemData*>(pOverItem->GetUserData());				
			if( pInfo )
			{
				cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
				if( pToolTip ) 
					pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 44, 44 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->m_dwItemIdx );
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CInfinitewarNoticeBoardRankBaseUI::UpdateKeyboard(const MSG& p_kMsg)
{
	return FALSE;
}	

void CInfinitewarNoticeBoardRankBaseUI::UpdateMyRankData( CInfiniteWarRankRewardContents::sRankInfos const* sInfos )
{
	SAFE_POINTER_RET( sInfos );
	switch( sInfos->nRankState )
	{
	case eRankStatus::STATUS_RUNNING://현재랭킹 진행중
		{
			_UpdateMyRank( sInfos->bAbsRanking, sInfos->nMyRanking, sInfos->nMyRankPrecent );
			_UpdateRewardItemList( sInfos->listRewardItems );
			_UpdateMyRankPoint(sInfos->nMyPoint);
			_UpdateMyRankAndUpdownPoint(sInfos->nMyRanking, sInfos->nUpDownPoint);	
			_ResetRemainTime( sInfos->i64RestRankEndSec );
			_ResetDataUpdateTime( sInfos->i64RestRankRenewSec );

			if( m_pNoNextRankTextMsg )
				m_pNoNextRankTextMsg->SetVisible( false );

			if( m_pRankLimitTime )
				m_pRankLimitTime->SetVisible( true );
			if( m_pNextRankLimitTime )
				m_pNextRankLimitTime->SetVisible( false);
			if( m_pTimerBar )
				m_pTimerBar->SetVisible(true);

			if( m_pDataUpdateTimerBar )
				m_pDataUpdateTimerBar->SetVisible( true );
			if( m_pRankUpdateItemText )
				m_pRankUpdateItemText->SetVisible( true );
		}break;
	case eRankStatus::STATUS_WAITING://다음랭킹 대기중
		{
			_UpdateMyRank( sInfos->bAbsRanking, sInfos->nMyRanking, sInfos->nMyRankPrecent );
			_UpdateRewardItemList( sInfos->listRewardItems );
			_UpdateMyRankPoint(sInfos->nMyPoint);
			_UpdateMyRankAndUpdownPoint(sInfos->nMyRanking, sInfos->nUpDownPoint);	
			_ResetRemainTime( sInfos->i64RestRankEndSec );
			_ResetDataUpdateTime( sInfos->i64RestRankRenewSec );

			if( m_pNoNextRankTextMsg )
				m_pNoNextRankTextMsg->SetVisible( false );

			if( m_pRankLimitTime )
				m_pRankLimitTime->SetVisible(false);
			if( m_pNextRankLimitTime )
				m_pNextRankLimitTime->SetVisible( true );
			if( m_pTimerBar )
				m_pTimerBar->SetVisible(true);

			if( m_pDataUpdateTimerBar )
				m_pDataUpdateTimerBar->SetVisible( false );
			if( m_pRankUpdateItemText )
				m_pRankUpdateItemText->SetVisible( false );

		}break;
	case eRankStatus::STATUS_END://다음랭킹 없음
		{
			_UpdateMyRank( sInfos->bAbsRanking, sInfos->nMyRanking, sInfos->nMyRankPrecent );
			_UpdateRewardItemList( sInfos->listRewardItems );
			_UpdateMyRankPoint(sInfos->nMyPoint);
			_UpdateMyRankAndUpdownPoint(sInfos->nMyRanking, sInfos->nUpDownPoint);	
			_ResetRemainTime( sInfos->i64RestRankEndSec );
			_ResetDataUpdateTime( sInfos->i64RestRankRenewSec );

			if( m_pNoNextRankTextMsg )
				m_pNoNextRankTextMsg->SetVisible( true );

			if( m_pRankLimitTime )
				m_pRankLimitTime->SetVisible(false);
			if( m_pNextRankLimitTime )
				m_pNextRankLimitTime->SetVisible( false );
			if( m_pTimerBar )
				m_pTimerBar->SetVisible(false);

			if( m_pDataUpdateTimerBar )
				m_pDataUpdateTimerBar->SetVisible( false );
			if( m_pRankUpdateItemText )
				m_pRankUpdateItemText->SetVisible( false );
		}break;
	}
}

void CInfinitewarNoticeBoardRankBaseUI::_UpdateMyRank( bool const& bAbsRank, int const& nMyRank, int const& nRankPersent )
{
	SAFE_POINTER_RET( m_pMyCurrentRank );
	std::wstring msg;
	if( 0 == nMyRank )
	{
		msg = UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_MYRANK_ABSOLUTE_TEXT");
		DmCS::StringFn::ReplaceAll( msg, L"#myrank#", L"-" );
	}
	else if( !bAbsRank )// 절대 수치
	{
		msg = UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_MYRANK_ABSOLUTE_TEXT");
		DmCS::StringFn::ReplaceAll( msg, L"#myrank#", nMyRank );
	}
	else// 퍼센트 수치
	{
		msg = UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_MYRANK_PERCENT_TEXT");
		DmCS::StringFn::ReplaceAll( msg, L"#myprecent#", nRankPersent );
	}

	m_pMyCurrentRank->SetText( msg.c_str() );
}

void CInfinitewarNoticeBoardRankBaseUI::_UpdateMyRankPoint( int const& nPoint )
{
	SAFE_POINTER_RET( m_pMyCurrentPoint );
	std::wstring ws = DmCS::StringFn::getNumberFormatW(nPoint);
	ws += L" P";
	m_pMyCurrentPoint->SetText( ws.c_str() );
}

void CInfinitewarNoticeBoardRankBaseUI::_UpdateMyRankAndUpdownPoint( int const& nMyRank, int const& nUpdownPoint )
{
	SAFE_POINTER_RET( m_pMyUpDownRank );
	std::wstring ws = UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_MYRANK_UPDOWN_TEXT");
	if( 0 == nMyRank )
		DmCS::StringFn::ReplaceAll(ws, L"#myrank#", L"-");
	else
		DmCS::StringFn::ReplaceAll(ws, L"#myrank#", nMyRank);

	std::wstring wud;
	if( nUpdownPoint > 0 )		DmCS::StringFn::Format( wud, L"▲%d", nUpdownPoint );
	else if( nUpdownPoint < 0 )	DmCS::StringFn::Format( wud, L"▼%d", abs(nUpdownPoint) );
	else						wud = L"-";

	DmCS::StringFn::ReplaceAll( ws, L"#updown#", wud);
	m_pMyUpDownRank->SetText( ws.c_str() );	
}

void CInfinitewarNoticeBoardRankBaseUI::_UpdateRewardItemList( std::list<TB::sRewardItemInfo> const& itemCon )
{
	SAFE_POINTER_RET( m_pRewardItemLIst );
	m_pRewardItemLIst->RemoveAllItem();

	std::list<TB::sRewardItemInfo>::const_iterator it = itemCon.begin();
	for( int n = 0; it != itemCon.end(); ++it, ++n )
	{
		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cSprite* pIconBackImg = NiNew cSprite;
		if( pIconBackImg )
		{
			pIconBackImg->Init( NULL, CsPoint::ZERO, CsPoint( 44, 44 ), "InfiniteNoticeBoard\\Infinite_NoticeBoard_RewardItemSlot_Back.tga", false );
			cString::sSPRITE* pImg = pItem->AddSprite( pIconBackImg );
			if( pImg )
				pImg->SetAutoPointerDelete(true);;
		}

		pItem->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, (*it).s_dwItemCode, (*it).s_nCount, CsPoint( 6, 6 ) );
		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(n, CsPoint( 44, 44));
		addItem->SetItem( pItem );
		addItem->SetUserData( new sRewardItemData((*it).s_dwItemCode) );
		m_pRewardItemLIst->AddItem( addItem );
	}
}

void CInfinitewarNoticeBoardRankBaseUI::UpdateRenkerList( std::list<CInfiniteWarRankRewardContents::sRankerList>const* sList )
{
	SAFE_POINTER_RET( m_pRankList );
	m_pRankList->RemoveAllItem();

	SAFE_POINTER_RET( sList );
	std::list<CInfiniteWarRankRewardContents::sRankerList>::const_iterator it = sList->begin();
	for( ; it != sList->end(); ++it )
	{
		cListBoxItem* pAddItem = NiNew cListBoxItem;
		SAFE_POINTER_RET( pAddItem );
		cString * pItem = NiNew cString;
		SAFE_POINTER_RET( pItem );

		if( (*it).nRank > 0 && (*it).nRank < 4 )
		{
			cImage* pTopRankImg = NiNew cImage;
			if( pTopRankImg )
			{
				pTopRankImg->Init( NULL, CsPoint::ZERO, CsPoint( 48, 42 ), "InfiniteNoticeBoard\\Infinite_Borad_top3.tga", false );
				pTopRankImg->SetTexToken( CsPoint( 0, 42 ) );
				cString::sIMAGE* paddImg = pItem->AddImage( pTopRankImg, (*it).nRank - 1, CsPoint::ZERO, CsPoint( 48, 42 )  );	
				if( paddImg )
					paddImg->SetAutoPointerDelete(true);
			}
		}
		else
		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER;
			ti.SetText( (*it).nRank );
			pItem->AddText( &ti, CsPoint( 24, 15) );
		}

		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER;
			ti.SetText( (*it).wsTamerName.c_str() );
			pItem->AddText( &ti, CsPoint( 153, 15) );
		}

		if( !(*it).wsGuildName.empty() )
		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12, NiColor( 243.0f / 255.0f, 228.0f / 255.0f, 42.0f / 255.0f ) );
			ti.s_eTextAlign = DT_CENTER;
			wstring guild;
			DmCS::StringFn::Format( guild, L"<%s>", (*it).wsGuildName.c_str() );
			ti.SetText( guild.c_str() );
			pItem->AddText( &ti, CsPoint( 374, 15) );
		}

		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER;
			std::wstring wsPoint = DmCS::StringFn::getNumberFormatW( (*it).dwPoint );
			ti.SetText( wsPoint.c_str() );
			pItem->AddText( &ti, CsPoint( 531, 15) );
		}

		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER;

			std::wstring ws;
			if( (*it).bIsNew )
			{
				ws = L"New";
				ti.s_Color = NiColor(1.0f, 1.0f, 0.0f);
			}
			else
			{
				if( (*it).nUpDownPoint > 0 )
				{
					ti.s_Color = NiColor(0.0f, 1.0f, 0.0f);
					DmCS::StringFn::Format( ws, L"▲ %d", (*it).nUpDownPoint );
				}
				else if( (*it).nUpDownPoint < 0 )
				{
					ti.s_Color = NiColor(1.0f, 0.0f, 0.0f);
					DmCS::StringFn::Format( ws, L"▼ %d", abs((*it).nUpDownPoint) );
				}
				else
					ws = L" - ";
			}			
			
			ti.SetText( ws.c_str() );			
			pItem->AddText( &ti, CsPoint( 615, 15) );
		}

		cSprite* pLineBack = NiNew cSprite;
		if( pLineBack )
		{
			pLineBack->Init( NULL, CsPoint::ZERO, CsPoint( 680, 7 ), "InfiniteMatch\\Infinite_Match_line.tga", false );
			cString::sSPRITE* paddImg = pItem->AddSprite( pLineBack, CsPoint(0,36), CsPoint( 680, 7 )  );
			if( paddImg )
				paddImg->SetAutoPointerDelete(true);
		}

		pAddItem->SetItem( pItem );
		m_pRankList->AddItem( pAddItem );
	}
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


void CInfinitewarNoticeBoardRankBaseUI::_ResetRemainTime( __int64 const& nTime )
{
	uint uServerTime = net::net->GetTimeTS();
	__int64 nRemainTime = nTime - uServerTime;
	if( nRemainTime < 0 )
		nRemainTime = 0;

	SAFE_POINTER_RET( m_pTimerBar );
	m_pTimerBar->Reset();
	m_pTimerBar->SetTimer( 0.0f, nRemainTime );
	m_pTimerBar->SetProgressPos( 0.0f );
	if( nRemainTime > 0 )
		m_pTimerBar->Start();
}

void CInfinitewarNoticeBoardRankBaseUI::_ResetDataUpdateTime( __int64 const& nTime )
{
	SAFE_POINTER_RET( m_pDataUpdateTimerBar );

	uint uServerTime = net::net->GetTimeTS();
	__int64 nRemainTime = nTime - uServerTime;
	if( nRemainTime < 0 )
		nRemainTime = 0;

	m_pDataUpdateTimerBar->Reset();
	m_pDataUpdateTimerBar->SetTimer( 0.0f, nRemainTime );
	m_pDataUpdateTimerBar->SetProgressPos( 0.0f );
	if( nRemainTime > 0 )
		m_pDataUpdateTimerBar->Start();
}

void CInfinitewarNoticeBoardRankBaseUI::UpdateRankData(int const& nType)
{
	SAFE_POINTER_RET( GetSystem() );
	std::list<CInfiniteWarRankRewardContents::sRankerList> const * pRankerList = GetSystem()->GetRankerListFormType( nType );
	UpdateRenkerList( pRankerList );
}

void CInfinitewarNoticeBoardRankBaseUI::UpdateRankerListData(int const& nType)
{
	SAFE_POINTER_RET( GetSystem() );
	CInfiniteWarRankRewardContents::sRankInfos const * pRankInfos = GetSystem()->GetRankInfoByType( nType );
	UpdateMyRankData( pRankInfos );
}

void CInfinitewarNoticeBoardRankBaseUI::_SendRequestRankingDataUpdate(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->SendRankingDataUpdateByType( m_nRankingType );
}

void CInfinitewarNoticeBoardRankBaseUI::_SendRankingDeadline(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->SendRankingDeadline( m_nRankingType );
}

//////////////////////////////////////////////////////////////////////////
bool CInfinitewarNoticeBoardRankBaseUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;
	GetSystem()->Register( SystemType::eUpdate_RankInfoByType, this );
	return true;
}

void CInfinitewarNoticeBoardRankBaseUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eUpdate_RankInfoByType:
		{
			int nType;
			kStream >> nType;
			if( nType == m_nRankingType )
			{
				UpdateRankData(nType);
				UpdateRankerListData(nType);
			}
		}
		break;
	}
}