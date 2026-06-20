
#pragma once 
// 
// #include "../00.Contents/MainFrame_Contents.h"
// 
// class cTamerMainFrame : public cBaseWindow, public CMainFrameContents::ObserverType
// {
// public:
// 	cTamerMainFrame();
// 	~cTamerMainFrame();
// 
// public:
// 	virtual eWINDOW_TYPE	GetWindowType();
// 	virtual void			Destroy();
// 	virtual void			DeleteResource();
// 	virtual void			Create( int nValue = 0 );
// 
// 	virtual void			PlayOpenSound(){}
// 	virtual void			PlayCloseSound(){}
// 
// 	virtual void			Update(float const& fDeltaTime);
// 	virtual eMU_TYPE		Update_ForMouse();	
// 	virtual void			Render();
// 
// 	virtual void			OnRButtonUp( CsPoint pos );
// 	virtual	void			OnLButtonUp( CsPoint pos );
// 
// 	virtual void			ResetDevice();
// 
// protected:
// 	virtual	bool				Construct(void);
// 	virtual	void				Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
// 
// private:
// 	cProgressBar*			m_pHPBarMin;
// 	cProgressBar*			m_pHPBarMax;
// 	cProgressBar*			m_pFPBar;
// 	cProgressBar*			m_pExpBar;
// 	cText*					m_pNameText;
// 	cText*					m_pLevelText;
// 	cText*					m_pExpPercentText;
// 
// 	cSprite*				m_pHostSprite;	
// 	cSprite*				m_pTamerImg;	
// };