#pragma once 

#include "../00.Contents/BattleSystemContent.h"

class cBattleRoundTimer: public cBaseWindow, public CBattleSystemContents::ObserverType
{
public:
	cBattleRoundTimer();
	virtual ~cBattleRoundTimer();

	virtual	bool	Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();	

	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);

	virtual void			Render();		
	virtual void			PreResetMap(){ Close( false ); }

protected:
	void					Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					SetHurryup();

private:
	float					m_fTime;

	cSprite*				m_pTimerBg;
	cSprite*				m_pMin[2];
	cSprite*				m_pSec[2];
	cSprite*				m_pMs[2];

	cSprite*				m_pDot[3];

	bool					m_bHurryup;
};

