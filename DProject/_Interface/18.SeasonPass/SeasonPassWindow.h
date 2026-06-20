#pragma once

#include "../00.Contents/SeasonPass_Contents.h"

class SeasonPassWindow : public cBaseWindow, public SeasonPassContents::ObserverType
{

protected:
	virtual	void				Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	SeasonPassWindow();
	~SeasonPassWindow();

	virtual eWINDOW_TYPE		GetWindowType() { return WT_SEASONPASS_WINDOW; }
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
