#pragma once

#include "../00.Contents/EvolutionStore_Contents.h"

class cInputPasswordUI : public cScriptUI, public cEvolutionStoreContents::ObserverType
{
public:
	cInputPasswordUI();
	~cInputPasswordUI();

	void						Init(cWindow* pRootWin);
	virtual	bool				Construct(void);

	bool						UpdateMouse();	

protected:
	void						Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void						SetInitData();
	void						ResetData();
	void						_CheckEditBoxText( void* pSender, void *pData );
	void						_OnClick_Close(void* pSender, void* pData);
	void						_OnClick_OkBtn(void* pSender, void* pData);

private:
	cEditBox*					m_pEditResistNumber;
	cButton*					m_pCloseBtn;
	cButton*					m_pOkBtn;
};