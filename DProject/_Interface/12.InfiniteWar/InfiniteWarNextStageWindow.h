#pragma once 

#include "../00.Contents/InfiniteWar_RankReward_Contents.h"

class CInfiniteWarNextStagedWindow : public cBaseWindow, public CInfiniteWarContents::ObserverType
{
public:
	CInfiniteWarNextStagedWindow();
	virtual ~CInfiniteWarNextStagedWindow();

	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	void					ResetDevice();

private:
	void					_WaitBtnEvent( void* pSender, void* pData );
	void					_SendTryNextStageBtnEvent(void* pSender, void* pData);

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	cButton*				m_pWait;
	cButton*				m_pOk;
	cButton*				m_pCloseBtn;
	CsOnceTimeSeq			m_WaitTime;
};


