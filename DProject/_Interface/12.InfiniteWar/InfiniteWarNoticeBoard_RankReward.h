#pragma once

#include "../00.Contents/InfiniteWar_RankReward_Contents.h"

class CInfiniteNBRankRewardWindow : public cBaseWindow, public CInfiniteWarRankRewardContents::ObserverType
{
public:
	CInfiniteNBRankRewardWindow();
	virtual ~CInfiniteNBRankRewardWindow();

	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	void					ResetDevice();
	bool					OnEscapeKey();

private:
	void					_CloseWindow( void* pSender, void* pData );
	void					_SetTitleName( int const& nType );
	void					_SetRewardItemlist(int const& nType);

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	cText*					m_pTitleName;
	cButton*				m_pCloseBtn;
	cListBox*				m_pRewardItemLIst;
};

