#pragma once

#include "InfiniteWarNoticeBoard_RankBase.h"
#include "../00.Contents/InfiniteWar_RankReward_Contents.h"

class CInfinitewarNoticeBoardMonthlyUI : public CInfinitewarNoticeBoardRankBaseUI
{
public:
	CInfinitewarNoticeBoardMonthlyUI(int const& nType);
	virtual ~CInfinitewarNoticeBoardMonthlyUI();

public:
	virtual bool	Create(cWindow* pRoot);

private:
	void			_OpenMonthlyRankRewardItemList(void* pSender, void* pData);
	void			_OpenMonthlyPreRankingList(void* pSender, void* pData);

};