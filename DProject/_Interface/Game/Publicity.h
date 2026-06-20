
#pragma once 
#include "../00.Contents/NeonDisplayContents.h"

class cPublicity : public cBaseWindow, public CNeonDisplayContents::ObserverType
{
public:
	cPublicity();
	~cPublicity();

public:
	virtual	bool			Construct(void);
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		

private:
	void					_InitData();
	void					_SetNextMessage();
	void					_InsertMessage( std::wstring const& wsMsg, bool bSound = true );
	void					_UpdateTextAlpha(float const& fDeltaTime);

private:
	float					m_fCurrentTime;
	bool					m_bShow;
	const float				m_fShowTime;

	float					m_fAlpha;
	bool					m_bFadeIn;

	cListBox*				m_pMessageBox;
	cSprite*				m_pTextBack;
	std::wstring			m_wsShowMsg;
	std::list<cString::sTEXT*>	m_pControls;
};
