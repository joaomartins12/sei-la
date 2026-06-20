///////////////////////////////////////////
//2017-02-16-nova 접속보상 시스템
#pragma once

#include "EventContents.h"
#include "RewardDailyUI.h"
#include "RewardMonthlyUI.h"
#include "RewardPromotionUI.h"
#include "RewardHotTimeUI.h"
#include "RewardDailyCheckUI.h"

class cRewardEventUI : public cBaseWindow, public EventContents::ObserverType
{
	struct sEventType : public CUSTOMDATA
	{
		sEventType(int const&nType):nEventType(nType) {};
		~sEventType() {};

		int nEventType;
	};

public:
	cRewardEventUI();
	virtual ~cRewardEventUI();

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();

	virtual void			OnMoveWindow();
	virtual bool			OnEscapeKey(){ Close(); return true; }

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	bool			Construct(void);

private:
	void					ShowViewer( int const& nViewerType, int const& nViewerSubType );
	bool					InsertEventMap( int const& nIdx );
	void					SelectedCategoty(void* pSender, void* pData);
	void					SelectedSubCategory(void* pSender, void* pData);
	void					MakeCategory(int const& nIdx, std::wstring const& wsTitleName, int nSubCount = 0);
	void					MakeSubCategory( cTreeBoxItem* pRoot, int const& nCount );

	void					PressCancelButton( void* pSender, void* pData );
	void					PressAcceptButton( void* pSender, void* pData );

	void					DeleteEventMap( int const& nIdx );
	void					DeleteCategory( int const& nIdx );

private:
	int						iSelMenu;
	int						iSelSubMenu;
	cButton*				CancelButton;
	cButton*				AcceptButton;
	cTreeBox*				m_EventList;
	//cTreeBox*				tbMenuCategoty;

	std::map<int, IEventViewer*>	m_mapEventMap;
};
