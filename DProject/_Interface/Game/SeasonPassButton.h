#pragma once

#include "../00.Contents/SeasonPass_Contents.h"

class cSeasonPassButton : public cBaseWindow, public SeasonPassContents::ObserverType
{

public:
	cSeasonPassButton();
	virtual ~cSeasonPassButton();

public:
	virtual eWINDOW_TYPE	GetWindowType() { return WT_SEASONPASS_BUTTON; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual	bool			Construct(void);
	virtual void			ResetDevice();
	virtual void			Create(int nValue = 0);

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();

	virtual void			Render();

	void					OpenWindow(void* pSender, void* pData);
	void					ShowToolTip(void* pData);

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	cButton* m_pOpenButton;

};
