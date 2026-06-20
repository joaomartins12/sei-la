#pragma once

#include "InfiniteWarNoticeBoard_RankBase.h"
#include "../00.Contents/InfiniteWar_RankReward_Contents.h"

class CInfinitewarNoticeBoardWeekUI : public CInfinitewarNoticeBoardRankBaseUI
{
public:
	CInfinitewarNoticeBoardWeekUI(int const& nType);
	virtual ~CInfinitewarNoticeBoardWeekUI();

public:
	bool			Create(cWindow* pRoot);

private:
	void			_OpenWeekRankRewardItemList(void* pSender, void* pData);
	void			_OpenWeekPreRankingList(void* pSender, void* pData);
};