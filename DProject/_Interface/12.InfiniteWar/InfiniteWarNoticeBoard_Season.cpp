#include "stdafx.h"
#include "InfiniteWarNoticeBoard_Season.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CInfinitewarNoticeBoardSeasonUI::CInfinitewarNoticeBoardSeasonUI(int const& nType)
:CInfinitewarNoticeBoardRankBaseUI(nType)
{
}

CInfinitewarNoticeBoardSeasonUI::~CInfinitewarNoticeBoardSeasonUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}


bool CInfinitewarNoticeBoardSeasonUI::Create(cWindow* pRoot)
{
	if(!CInfinitewarNoticeBoardRankBaseUI::Create(pRoot))
		return false;

	if( m_pLastRankView )
		m_pLastRankView->SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_SEASON_PREVIOUSLY_RANKING_VIEW_BUTTON" ).c_str() );
	if( m_pRankLimitTime )
		m_pRankLimitTime->SetText(UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_SEASON_RANKING_END_TIME_TEXT" ).c_str());

	if( m_pNextRankLimitTime )
		m_pNextRankLimitTime->SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_SEASON_RANKING_START_REMAIN_TIME_TEXT" ).c_str() );
	if( m_pNoNextRankTextMsg )
		m_pNoNextRankTextMsg->SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_SEASON_RANKING_END_MESSAGE_TEXT" ).c_str() );

	if( m_pRankGetItemView )
		m_pRankGetItemView->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardSeasonUI::_OpenSeasonRankRewardItemList);
	if( m_pLastRankView )
		m_pLastRankView->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardSeasonUI::_OpenSeasonPreRankingList);

	UpdateRankData(m_nRankingType);
	UpdateRankerListData(m_nRankingType);
	return true;
}


void CInfinitewarNoticeBoardSeasonUI::_OpenSeasonRankRewardItemList(void* pSender, void* pData)
{
	GetSystem()->OpenRankRewardItemListWindow( TB::eInfiniteWarRewardType::eSeason );
}

void CInfinitewarNoticeBoardSeasonUI::_OpenSeasonPreRankingList(void* pSender, void* pData)
{
	GetSystem()->OpenPreRankingListWindow( TB::eInfiniteWarRewardType::eSeason );
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
