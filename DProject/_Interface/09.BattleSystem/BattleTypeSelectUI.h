#pragma once

#include "../00.Contents/BattleSystemContent.h"

class CBattleTypeSelectUI : public cScriptUI, public CBattleSystemContents::ObserverType
{
	struct sBattleTypeData : public CUSTOMDATA
	{
		sBattleTypeData(unsigned int idx ) : m_nTypeIdx(idx)
		{}
		~sBattleTypeData() {};

		unsigned int m_nTypeIdx;
	};

public:
	CBattleTypeSelectUI();
	virtual ~CBattleTypeSelectUI();

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
	void			_MakeBattleTypeList();
	
	void			_Forward(void* pSender, void* pData);
	void			_Backward(void* pSender, void* pData);

	void			_SelectBattleType(void* pSender, void* pData);
	void			_ChangeStep(BRegist::eRegistState const& step);

private:
	cGridListBox*	m_pBattleTypeListBox;
	cButton*		m_pTypeSelectBtn;
	cButton*		m_pTypeCloseBtn;
};