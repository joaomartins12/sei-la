#pragma once

#include "../00.Contents/VipMemberShip_Contents.h"

class CVipSystemWindow : public cBaseWindow, public CVipMemberShipContents::ObserverType
{

public:
	CVipSystemWindow();
	virtual ~CVipSystemWindow();

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();	
	virtual	bool			Construct(void);

	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();
	

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					OpenWindow(void* pSender, void* pData);
	void					_SetMemberShipEndTime(bool bVisible, float fRemainTime);

private:
	cButton*				m_pOpenButton;
	cUI_TimerProgressBar*	m_pkRemainTime;
	cSprite*				m_pTimerBack;
};
