#pragma once

#include "../00.Contents/ServerRelocateContents.h"

class CBeCautiousUI : public cScriptUI, public CServerRelocateContents::ObserverType
{
public:
	CBeCautiousUI();
	virtual ~CBeCautiousUI();

public:
	virtual	bool	Construct(void);
	virtual void	Render();

	bool			Create();
	BOOL			UpdateMouse();	
	void			ResetDevice();

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void			Destroy();
	void			_CloseWindow(void* pSender, void* pData);
	void			_BackPage(void* pSender, void* pData);
	void			_NextPage(void* pSender, void* pData);

private:
	cButton*		m_pNextBtn;
	cButton*		m_pBackBtn;	
	cButton*		m_pBtnClose;
};