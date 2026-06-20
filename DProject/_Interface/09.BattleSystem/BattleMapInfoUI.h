#pragma once

#include "../00.Contents/BattleSystemContent.h"

class CBattleMapInfoUI : public cScriptUI, public CBattleSystemContents::ObserverType
{
public:
	CBattleMapInfoUI();
	virtual ~CBattleMapInfoUI();

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
	void			_UpdateBattleMapUI();

	void			_ChangeStep(BRegist::eRegistState const& step);

private:
	cSprite*		m_pMapImage;
	cText*			m_pMapName;
};