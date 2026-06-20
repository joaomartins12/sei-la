#pragma once 
#include "EventContents.h"
#include "EventUIViewer.h"

class cRewardDailyUI : public IEventViewer
{
	struct sTimeStepUI
	{
		sTimeStepUI():pBackImg(NULL), pTimeText(NULL){};
		~sTimeStepUI(){};

		void		SetComplete( bool bValue );
		void		SetVisible( bool bValue );
		void		SetTimeText( int const& nValue );

		cSprite*	pBackImg;
		cText*		pTimeText;
	};

	struct sTimeLineUI
	{
		sTimeLineUI():pTimeLineBackImg(NULL) {};
		~sTimeLineUI() {};

		void		SetVisible( bool bValue );

		void		SetComplete( int const& nStep, bool bValue );
		void		ResetComplete();
		size_t		GetTimeStepSize() const;
		void		SetTimeLineText( int const& nIdx, int const& nTimeValue );

		cSprite*	pTimeLineBackImg;
		std::map<int,sTimeStepUI>	mapTimeStepUI;
	};

	struct sRewardItemIcon
	{
		sRewardItemIcon():s_pItemIcon(NULL),s_pCompleteImg(NULL),s_pUserData(NULL),s_gridItem(NULL)
		{};
		
		void SetVisible( bool bValue );
		void SetItemData( int const& nItemCode, int const& nItemCount );
		void SetComplete(bool bValue);

		cGridListBoxItem *				s_gridItem;
		cString::sICON*					s_pItemIcon;
		cString::sSPRITE*				s_pCompleteImg;
		IEventViewer::sRewardItemData*	s_pUserData;
	};

	struct sGridItemControl
	{
		void ResetData();
		void SetItemData(int const& nIdx, int const& nItemCode, int const& nItemCount, bool bComplete);

		std::list< sRewardItemIcon >	s_Controls;
	};

public:
	cRewardDailyUI();
	virtual ~cRewardDailyUI();	

public:
	virtual void		Create(cWindow* pkRoot, int nValue = 0 );
	virtual BOOL		UpdateMouse();
	virtual void		RenderScript();
	virtual void		UpdateScript(float fDeltaTime);

protected:
	virtual	void		Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	bool		Construct(void);

private:
	void				MakeEmptyGridItem(int const& nMakeLine);
	void				MakeTimeLineUI(int const& nMakeTimeLineCount);
	void				SetTimeLineVisible(int const& nMakeTimeLineCount, bool bValue);
	void				ReSizeGridList( int const& nTimeLineCount );
	void				SetRewardGridItem();
	void				SetTimeLineText( int const& nMakeTimeLineCount, int const& nIdx, int const& nValue );
	void				SetTimeLineComplete( int const& nMakeTimeLineCount, int const& nIdx, bool const& bComplete );
	void				UpdateTimeMsg( int const& nDate );
	void				SetAllCompleteTimeMsg();

private:
	cGridListBox*					m_pRewardGride;
	std::map<int, sTimeLineUI>		m_TimeLineUIControl;
	std::map<int, sGridItemControl>	m_GridUIControl;
	cStringList						sDateText;
	int								m_nCurrentTimeTS;
	int								m_nTotalPlayTime;
};