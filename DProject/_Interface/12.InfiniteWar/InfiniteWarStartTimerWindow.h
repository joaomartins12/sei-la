#pragma once 

#include "../00.Contents/BattleSystemContent.h"


class CInfiniteWarStartTimerWindow : public cBaseWindow, public CInfiniteWarContents::ObserverType
{
public:
	CInfiniteWarStartTimerWindow();
	virtual ~CInfiniteWarStartTimerWindow();

	virtual	bool	Construct(void);

	enum eTimerState
	{
		NONE,
		READY,
		START,
	};
public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();	

	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();

	virtual void			Render();		
	virtual void			PreResetMap(){ Close( false ); }

protected:
	void					Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	cSprite*				m_pTimer;
	cSprite*				m_pStartImg;
	float					m_fTime;

	eTimerState				m_state;
};

