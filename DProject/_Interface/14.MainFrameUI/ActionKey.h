#pragma once

#include "../00.Contents/MainFrame_Contents.h"

class cActionKey : public cBaseWindow, public CMainFrameContents::ObserverType
{
private:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	cActionKey();
	~cActionKey();

	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	
	virtual void			Destroy();

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual eMU_TYPE		Update_ForMouse_Disable();
	virtual void			Render();

	virtual void			ResetMap();
	virtual void			ResetDevice();
	virtual bool			OnMacroKey(const MSG& p_kMsg);

private:
	cSpriteAni*		m_pActAniImg;
	cText*			m_pTalkText;
};