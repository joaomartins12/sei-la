#pragma once

#include "../00.Contents/EventNotice_Contents.h"

class EventNoticeWindow : public cBaseWindow, public EventNoticeContents::ObserverType
{

protected:
	virtual	void				Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	EventNoticeWindow();
	~EventNoticeWindow();

	virtual eWINDOW_TYPE		GetWindowType() { return WT_NOTICE_WINDOW; }
	virtual	bool				Construct(void);
	virtual void				Create(int nValue = 0);
	virtual void				DeleteResource();
	virtual void				Destroy();
	virtual void				Render();

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();

	virtual void				ResetDevice(void);
	virtual bool				Close(bool bSound = true);

private:
	void						CloseWindow(void* pSender, void* pData);

	cButton*					m_pCloseBtn;
};
