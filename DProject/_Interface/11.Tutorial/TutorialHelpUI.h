#pragma once 


class cTutorialHelpUI : public cBaseWindow, public CTutorialContents::ObserverType
{
public:
	cTutorialHelpUI();
	~cTutorialHelpUI();

	virtual	bool	Construct(void);
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Render();
	virtual void			ResetDevice();

	virtual void			PlayOpenSound() {}
	virtual void			PlayCloseSound() {}

	virtual bool			OnMacroKey(const MSG& p_kMsg);
	virtual bool			OnEscapeKey();

private:
	cButton*				m_pCloseBtn;
};