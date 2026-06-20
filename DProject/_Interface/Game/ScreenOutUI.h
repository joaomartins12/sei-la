#pragma once 


class cScreenOutUI : public cBaseWindow
{
public:
	cScreenOutUI();
	~cScreenOutUI();

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Render();
	virtual void			ResetDevice();
	virtual void			ResetMap();

	virtual void			PlayOpenSound() {}
	virtual void			PlayCloseSound() {}
};