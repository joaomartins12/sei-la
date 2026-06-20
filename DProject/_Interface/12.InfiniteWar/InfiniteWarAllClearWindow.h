#pragma once 

#include "../00.Contents/InfiniteWar_Contents.h"

class CInfiniteWarAllClearWindow : public cBaseWindow, public CInfiniteWarContents::ObserverType
{
public:
	CInfiniteWarAllClearWindow();
	virtual ~CInfiniteWarAllClearWindow();

	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();
	virtual bool			OnEscapeKey();

	void					ResetDevice();

private:
	void					_CloseWindow( void* pSender, void* pData );
	void					_LeavCurrentMap(void* pSender, void* pData);

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	cButton*				m_pLeaveBtn;
	cButton*				m_pCloseBtn;
};


