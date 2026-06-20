#pragma once

#include "../00.Contents/InfiniteWar_RankReward_Contents.h"

class CInfiniteNBPreRankListWindow : public cBaseWindow, public CInfiniteWarRankRewardContents::ObserverType
{
public:
	CInfiniteNBPreRankListWindow();
	virtual ~CInfiniteNBPreRankListWindow();

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

	void					_SetMyRank( int const& nMyPreviouslyRank );
	void					_SetTitleName( int const& nType );
	void					_SetPreviouslyRankData( int const& nType );

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	cButton*				m_pCloseBtn;
	cListBox*				m_pRankList;
	cText*					m_pTitleName;
	cText*					m_pMyRank;
};
