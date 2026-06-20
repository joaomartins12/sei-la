#pragma once

#include "../00.Contents/BattleSystemContent.h"

class CBattleRegistBackUI : public cScriptUI, public CBattleSystemContents::ObserverType
{
public:
	CBattleRegistBackUI();
	virtual ~CBattleRegistBackUI();

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

	void			_Backward(void* pSender, void* pData);
	void			_SendBattleRegist(void* pSender, void* pData);
	void			_ChangeModeDown(void* pSender, void* pData);
	void			_ChangeModeUp(void* pSender, void* pData);

	void			_ChangeStep(BRegist::eRegistState const& step);
	void			_ChangeMode();

private:
	cButton*		m_pBackBtn;
	cButton*		m_pRegistBtn;
	cImage*			m_pVersusType;
	cButton*		m_pModChangeLeft;
	cButton*		m_pModChangeRight;
};