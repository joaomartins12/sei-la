#pragma once

#include "../00.Contents/ServerRelocateContents.h"

class CTermsAndConditionUI : public cScriptUI, public CServerRelocateContents::ObserverType
{
public:
	CTermsAndConditionUI();
	virtual ~CTermsAndConditionUI();

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
	void			_NextPage(void* pSender, void* pData);
	void			_PressCheckBox(void* pSender, void* pData);

private:
	cButton*		m_pBtnClose;
	cButton*		m_pNextBtn;
	cCheckBox*		m_pCheckBox;
};