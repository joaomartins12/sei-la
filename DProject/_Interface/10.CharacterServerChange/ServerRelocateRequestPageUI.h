#pragma once

#include "../00.Contents/ServerRelocateContents.h"

class CRequestPageUI : public cScriptUI, public CServerRelocateContents::ObserverType
{
public:
	CRequestPageUI();
	virtual ~CRequestPageUI();

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
	void			_SendRequest(void* pSender, void* pData);
	void			_SendCheckName(void* pSender, void* pData);
	void			_SelectChangeServer(void* pSender, void* pData);
	void			_ChangeTamerName(void* pSender, void* pData);
	void			_PressCheckBox(void* pSender, void* pData);
	void			_SetUILock( bool bValue );
	void			_ReCheckEnable(int const& nServerIdx);
	void			_RecvNameCheckResult(bool const& bEnable);

private:
	cButton*		m_pRequestBtn;
	cButton*		m_pBackBtn;	
	cButton*		m_pBtnClose;
	cButton*		m_pCheckNameBtn;	
	cComboBox*		m_pSelectServer;
	cEditBox*		m_pEditBox;
	cText*			m_pTamerSlotCheck;
	cText*			m_pLimitMoneyText;
	cText*			m_pCheckNameMsg;
	cMoneyIF*		m_pLimitMoney;
	cCheckBox*		m_pCheckBox;
};