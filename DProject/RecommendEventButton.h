#pragma once

#include "RecommendEvent_Contents.h"

class CRecommendEventButton : public cBaseWindow, public CRecommendEvent_Contents::ObserverType
{
public:
	CRecommendEventButton(void);
	virtual ~CRecommendEventButton(void);

	virtual	bool			Construct(void);

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

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
	void					PressEventButton( void* pSender, void* pData );

private:
	cButton					m_EventButton;

};