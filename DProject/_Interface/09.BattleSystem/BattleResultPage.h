#pragma once

#include "../00.Contents/BattleSystemContent.h"

class CBattleResult : public cScriptUI, public CBattleSystemContents::ObserverType
{
	struct sRewardItemData : public CUSTOMDATA
	{
		sRewardItemData(DWORD dwItemIdx) : m_dwItemIdx(dwItemIdx)
		{}
		~sRewardItemData() {};

		DWORD m_dwItemIdx;
	};
public:
	CBattleResult();
	virtual ~CBattleResult();

public:
	bool	Create();
	void	Render3DModel();
	void	Update3DAccumtime( float const& fDeltaTime );
	void	Update(float const& fDeltaTime);
	BOOL	UpdateMouse();						//2016-01-13-nova	마우스 입력처리

	virtual	bool	Construct(void);
	virtual void	Render();

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void			Destroy();
	void			ResetDevice();
	void			_CloseBattle(void* pSender, void* pData);
	void			_MakeResultItem();

private:
	cButton*		m_CloseBtn;
	cListBox*		m_pItemList;
};