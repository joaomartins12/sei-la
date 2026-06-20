#pragma once

#include "InfiniteWarNoticeBoard_RankBase.h"
#include "../00.Contents/InfiniteWar_RankReward_Contents.h"

class CInfinitewarNoticeBoardSeasonUI : public CInfinitewarNoticeBoardRankBaseUI
{
public:
	CInfinitewarNoticeBoardSeasonUI(int const& nType);
	virtual ~CInfinitewarNoticeBoardSeasonUI();

public:
	virtual bool	Create(cWindow* pRoot);

private:
	void			_OpenSeasonRankRewardItemList(void* pSender, void* pData);
	void			_OpenSeasonPreRankingList(void* pSender, void* pData);

};