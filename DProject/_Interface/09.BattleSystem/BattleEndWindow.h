#pragma once 

#include "../00.Contents/BattleSystemContent.h"

class cBattleEndWindow: public cBaseWindow, public CBattleSystemContents::ObserverType
{
	enum 
	{
		eNone,
		eHide_Wait,
		eResult_Show,
		eWait_End,
	};

public:
	cBattleEndWindow();
	virtual ~cBattleEndWindow();

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
	void					_ChangeNextStep( int const& nType );

private:
	float					m_fTime;
	int						m_nShowType;
};

