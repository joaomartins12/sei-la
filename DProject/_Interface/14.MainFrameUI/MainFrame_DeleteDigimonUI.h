#pragma once

#include "../00.Contents/MainFrame_Contents.h"

class cDeleteDigimonUI : public cBaseWindow, CMainFrameContents::ObserverType
{
public:
	cDeleteDigimonUI();
	~cDeleteDigimonUI();

private:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();

	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetMap();
	virtual void			ResetDevice();

	virtual bool			OnMacroKey(const MSG& p_kMsg);

private:
	cSprite*				m_pCheckDeleteDigimon;
	cEditBox*				m_pEditResistNumber;
	cButton*				m_pDeleteDigimonCloseBtn;
	cButton*				m_pDeleteDigimonOkBtn;

private:
	void					_ChangeEditText( void* pSender, void* pData );
	void					_OkDeleteDigimon( void* pSender, void* pData );
};

