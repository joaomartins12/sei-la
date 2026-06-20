#pragma once

#include "../00.Contents/InfiniteWar_RankReward_Contents.h"

class CInfinitewarNoticeBoardHallOfFameUI : public cScriptUI, public CInfiniteWarRankRewardContents::ObserverType
{
protected:
	struct sSeasonIndex : public CUSTOMDATA
	{
		sSeasonIndex(int nIdx) : m_nIdx(nIdx)
		{}
		~sSeasonIndex() {};

		int m_nIdx;
	};

public:
	CInfinitewarNoticeBoardHallOfFameUI();
	virtual ~CInfinitewarNoticeBoardHallOfFameUI();

public:
	bool			Create(cWindow* pRoot);
	BOOL			UpdateMouse();					
	BOOL			UpdateKeyboard(const MSG& p_kMsg);
	virtual	bool	Construct(void);

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void			Destroy();
	void			_UpdateRenkerList( int const& nSeasonIdx );
	void			_AddPreSeasonList( std::wstring const& name, int const& nSeasonIdx );

	void			_UpdatePreSeasonList();
	void			_ResetList(void* pSender, void* pData);

private:
	cListBox*				m_pRankList;
	cListBox*				m_pSeasonList;
};
