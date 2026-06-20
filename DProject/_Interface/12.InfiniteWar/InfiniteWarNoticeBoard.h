#pragma once 

#include "../00.Contents/InfiniteWar_RankReward_Contents.h"

class CInfinitewarNoticeBoardDailyUI;
class CInfinitewarNoticeBoardMonthlyUI;
class CInfinitewarNoticeBoardWeekUI;
class CInfinitewarNoticeBoardSeasonUI;
class CInfinitewarNoticeBoardHallOfFameUI;


class CInfiniteWarNoticeBoardWindow : public cBaseWindow, public CInfiniteWarRankRewardContents::ObserverType
{
	enum eTabType
	{
		eReward_Daily = 0,
		eReward_Week,
		eReward_Month,
		eReward_Season,
		eHallOfFame,
	};

	struct sTabType : public CUSTOMDATA
	{
		sTabType(eTabType eType):eRewardType(eType) {}
		~sTabType() {};
		
		eTabType	eRewardType;
	};

public:
	CInfiniteWarNoticeBoardWindow();
	virtual ~CInfiniteWarNoticeBoardWindow();

	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();
	virtual void			ResetMap();

	void					ResetDevice();
	bool					OnEscapeKey();

private:
	void					_CloseWindow( void* pSender, void* pData );
	void					_ChangeTab(void* pSender, void* pData);
	void					_CreateChildWindow();

	void					_ShowTabWindow( eTabType const& changeTabType );

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	CInfinitewarNoticeBoardDailyUI*			m_DailyUI;
	CInfinitewarNoticeBoardWeekUI*			m_WeekUI;
	CInfinitewarNoticeBoardMonthlyUI*		m_MonthUI;
	CInfinitewarNoticeBoardSeasonUI*		m_SeasonUI;
	CInfinitewarNoticeBoardHallOfFameUI*	m_HallOfFameUI;

	cButton*								m_pCloseBtn;
	cRadioButton*							m_pTabMenu;
};


