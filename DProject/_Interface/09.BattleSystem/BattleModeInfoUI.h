#pragma once

#include "../00.Contents/BattleSystemContent.h"

class CBattleModeInfoUI : public cScriptUI, public CBattleSystemContents::ObserverType
{
	struct sRewardItemData : public CUSTOMDATA
	{
		sRewardItemData(DWORD dwItemIdx) : m_dwItemIdx(dwItemIdx)
		{}
		~sRewardItemData() {};

		DWORD m_dwItemIdx;
	};

public:
	CBattleModeInfoUI();
	virtual ~CBattleModeInfoUI();

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
	void			_UpdateModInfoUI( bool bVisible );

	void			_ChangeStep(BRegist::eRegistState const& step);
private:
	cText*			m_pModeName;
	cText*			m_pNeedTicketCount;
	cText*			m_pJoinCount;
	cText*			m_pMyDigimonLv;
	cText*			m_pMyDigimonTBName;
	cText*			m_pJoinEnableText;
	cGridListBox*	m_pWinRewardItemList;
	cGridListBox*	m_pLoseRewardItemList;
};