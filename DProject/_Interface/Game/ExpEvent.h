#pragma once

#include "EventContents.h"

class cExpEventUI : public cBaseWindow, public EventContents::ObserverType
{
public:
	cExpEventUI();
	virtual ~cExpEventUI();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_EXP_EVENT_UI; }
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();			

private:
	void					UpdateExpData();
	void					ShowToolTip(void* pSender, void* pData);

private:	
	cButton*				m_pExpEventBtn;
	cButton*				m_pExpHotEventBtn;
	NUMBERSPRITE::eCOLOR 	m_nExpNumTextIdx;
};