
#pragma once 
// 
// #include "../00.Contents/MainFrame_Contents.h"
// 
// class cDigimonMainFrame : public cBaseWindow, public CMainFrameContents::ObserverType
// {
// public:
// 	cDigimonMainFrame();
// 	~cDigimonMainFrame();
// 
// public:
// 	virtual eWINDOW_TYPE		GetWindowType();
// 	virtual void				Destroy();
// 	virtual void				DeleteResource();
// 	virtual void				Create( int nValue = 0 );
// 	virtual void				Update(float const& fDeltaTime);
// 	virtual eMU_TYPE			Update_ForMouse();
// 	virtual void				Render();	
// 	virtual void				ResetDevice();
// 
// 	virtual	void				OnLButtonUp( CsPoint pos );
// 
// protected:
// 	virtual	bool				Construct(void);
// 	virtual	void				Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
// 
// private:
// 	cProgressBar*				m_pHPBar;
// 	cProgressBar*				m_pDSBar;
// 	cProgressBar*				m_pExpBar;
// 	cText*						m_pNameText;
// 	cText*						m_pLevelText;
// 	cText*						m_pExpPercentText;	
// 	cSprite*					m_pBattleSprite;
// };
