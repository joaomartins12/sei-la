#pragma once

#include "EventContents.h"

class cDailyEventUI : public cBaseWindow, public EventContents::ObserverType
{
private:
	struct sDailyEventItem
	{
		CsPoint	m_IconPos;
		CsPoint	m_IconSize;
		int		m_ItemCode;
		int		m_ItemCount;
	};
public:
	cDailyEventUI();
	virtual ~cDailyEventUI();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();
	
private:
	void					UpdateDailyEventData();

private:
	cUI_TimerProgressBar*		m_pTimerBar;
	std::list<sDailyEventItem>	m_ItemsInfo;
};
