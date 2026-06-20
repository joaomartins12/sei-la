#pragma once

#include "../00.Contents/BattleSystemContent.h"

class CBattleWaitingBtn : public cBaseWindow, public CBattleSystemContents::ObserverType
{
public:
	CBattleWaitingBtn();
	virtual ~CBattleWaitingBtn();

	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();	

	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();		
	virtual void			Render();		
	virtual void			PreResetMap();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					_CancelBattleMatchingAsk(void* pSender, void* pData);

private:
	cButton*				m_pMatchingBtn;
};