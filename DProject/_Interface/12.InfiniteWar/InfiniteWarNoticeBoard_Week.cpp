#include "stdafx.h"
#include "InfiniteWarNoticeBoard_Week.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CInfinitewarNoticeBoardWeekUI::CInfinitewarNoticeBoardWeekUI(int const& nType)
:CInfinitewarNoticeBoardRankBaseUI(nType)
{
}

CInfinitewarNoticeBoardWeekUI::~CInfinitewarNoticeBoardWeekUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

bool CInfinitewarNoticeBoardWeekUI::Create(cWindow* pRoot)
{
	if(!CInfinitewarNoticeBoardRankBaseUI::Create(pRoot))
		return false;

	if( m_pLastRankView )
		m_pLastRankView->SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_WEEK_PREVIOUSLY_RANKING_VIEW_BUTTON" ).c_str() );
	if( m_pRankLimitTime )
		m_pRankLimitTime->SetText(UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_WEEK_RANKING_END_TIME_TEXT" ).c_str());
	if( m_pNextRankLimitTime )
		m_pNextRankLimitTime->SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_WEEK_RANKING_START_REMAIN_TIME_TEXT" ).c_str() );
	if( m_pNoNextRankTextMsg )
		m_pNoNextRankTextMsg->SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_WEEK_RANKING_END_MESSAGE_TEXT" ).c_str() );

	if( m_pRankGetItemView )
		m_pRankGetItemView->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardWeekUI::_OpenWeekRankRewardItemList);
	if( m_pLastRankView )
		m_pLastRankView->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardWeekUI::_OpenWeekPreRankingList);

	UpdateRankData(m_nRankingType);
	UpdateRankerListData(m_nRankingType);
	return true;
}

void CInfinitewarNoticeBoardWeekUI::_OpenWeekRankRewardItemList(void* pSender, void* pData)
{
	GetSystem()->OpenRankRewardItemListWindow( TB::eInfiniteWarRewardType::eWeek );
}

void CInfinitewarNoticeBoardWeekUI::_OpenWeekPreRankingList(void* pSender, void* pData)
{
	GetSystem()->OpenPreRankingListWindow( TB::eInfiniteWarRewardType::eWeek );
}

//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
