#pragma once 

#include "../00.Contents/TutorialContents.h"

class cTutorialGuideUI : public cBaseWindow, public CTutorialContents::ObserverType
{
public:
	cTutorialGuideUI();
	virtual ~cTutorialGuideUI();

public:
	virtual	bool			Construct(void);
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_TUTORIAL; }	
	virtual void			ResetDevice();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();
	virtual void			PreResetMap();
	virtual void			ResetMap();


protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					_ChangeTooltipImg( std::string const& sChangeImg );
	void					_TutorialSkip(void* pSender, void* pData);

private:
	cSprite*				m_pTooltipImg;
	cButton*				m_pSkipButton;
	CsRect					m_rectTitleBar;
};


