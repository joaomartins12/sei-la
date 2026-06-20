#pragma once

#include "../00.Contents/Mail_Contents.h"

class cMailButton : public cBaseWindow, public MailContents::ObserverType
{
public:
	cMailButton();
	virtual ~cMailButton();

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();	
	virtual bool			Construct();

	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();			

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					_OpenMailWindow(void* pSender, void* pData);

private:
	cButton*				m_pOpenButton;	
	cSprite*				m_pMask;	
	
	float					m_fAccumTime;
	float					m_fAlpha;
	bool					m_bIncreaseAlpha;
};