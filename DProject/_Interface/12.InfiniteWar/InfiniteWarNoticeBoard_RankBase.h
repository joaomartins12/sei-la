#pragma once

#include "../00.Contents/InfiniteWar_RankReward_Contents.h"

class CInfinitewarNoticeBoardRankBaseUI : public cScriptUI, public CInfiniteWarRankRewardContents::ObserverType
{
protected:
	struct sRewardItemData : public CUSTOMDATA
	{
		sRewardItemData(DWORD dwItemIdx) : m_dwItemIdx(dwItemIdx)
		{}
		~sRewardItemData() {};

		DWORD m_dwItemIdx;
	};

public:
	CInfinitewarNoticeBoardRankBaseUI(int const& nType);
	virtual ~CInfinitewarNoticeBoardRankBaseUI();

public:
	virtual bool	Create(cWindow* pRoot);
	virtual BOOL	UpdateMouse();					
	virtual BOOL	UpdateKeyboard(const MSG& p_kMsg);

	virtual	bool	Construct(void);

protected:
	virtual void	Destroy();
	void			UpdateMyRankData( CInfiniteWarRankRewardContents::sRankInfos const* sInfos );
	void			UpdateRenkerList( std::list<CInfiniteWarRankRewardContents::sRankerList>const* sList );

	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	virtual void	UpdateRankData(int const& nType);
	virtual void	UpdateRankerListData(int const& nType);

private:
	void			_UpdateMyRank( bool const& bAbsRank, int const& nMyRank, int const& nRankPersent );
	void			_UpdateMyRankPoint( int const& nPoint );
	void			_UpdateRewardItemList( std::list<TB::sRewardItemInfo> const& itemCon );
	void			_UpdateMyRankAndUpdownPoint( int const& nMyRank, int const& nUpdownPoint );
	void			_ResetRemainTime( __int64 const& nTime );
	void			_ResetDataUpdateTime( __int64 const& nTime );

	void			_SendRequestRankingDataUpdate(void* pSender, void* pData);
	void			_SendRankingDeadline(void* pSender, void* pData);

protected:
	cListBox*				m_pRankList;
	cText*					m_pMyCurrentRank;
	cText*					m_pMyCurrentPoint;
	cText*					m_pMyUpDownRank;

	cButton*				m_pRankGetItemView;
	cButton*				m_pLastRankView;
	cGridListBox*			m_pRewardItemLIst;
	cUI_TimerProgressBar*	m_pTimerBar;
	cUI_TimerProgressBar*	m_pDataUpdateTimerBar;


	cText*					m_pRankLimitTime;
	cText*					m_pNextRankLimitTime;
	cText*					m_pRankUpdateItemText;
	cText*					m_pNoNextRankTextMsg;
	int						m_nRankingType;
};