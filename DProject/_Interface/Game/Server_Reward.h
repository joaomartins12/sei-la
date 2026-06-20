#pragma once 

#include "ItemRewardContents.h"

#define	SERVER_LINE_HEIGHT	69
#define ICON_SIZE 32

class cServer_Reward : public cBaseWindow, public ItemRewardContents::ObserverType
{
private:
	struct sINFO
	{
		cButton*	s_pBtnGetReward;
		cText*		s_pExpDate;	
		DWORD		s_dwItemCode;
		DWORD		s_dwID;
		CsRect		s_rcRegion;

		void		Init()
		{ 			
			s_pExpDate = NULL;
			s_pBtnGetReward = NULL;
			s_dwID = s_dwItemCode = 0;
		}		
	};

public:
	cServer_Reward();
	virtual ~cServer_Reward();

public:
	virtual eWINDOW_TYPE GetWindowType();

	virtual void		Destroy();
	virtual void		DeleteResource();
	virtual void		Create( int nValue = 0 );

	virtual void		Update(float const& fDeltaTime);
	virtual eMU_TYPE	Update_ForMouse();
	virtual void		Render();

	virtual void		OnRButtonUp( CsPoint pos );

	virtual void		ResetDevice();
	virtual void		ResetMap();
	virtual bool		OnEscapeKey();
	virtual void		OnMoveWindow();

protected:
	virtual	void		Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	bool		Construct(void);

private:
	void				PressCancelButton( void* pSender, void* pData );	//2017-03-15-nova
	void				PressRefreshButton( void* pSender, void* pData );	//2017-03-15-nova

	void				UpdateGift();
	void				UpdateReward();
	void				RenderGift();
	void				RenderReward();	

protected:
	cButton*			m_pCancelButton;
	cButton*			m_pRefreshBtn;
	cButton*			m_pAllRecvBtn;
	cScrollBar*			m_pScrollBar;
	cRadioButton*		m_pTabMenu;
	
	CsPoint				FirstPoint;

	GIFT_ITEM_LIST*		m_pGiftItemData;	//2017-03-14-nova
	REWARD_ITEM_LIST*	m_pRewardItemData;	//2017-03-14-nova
	sINFO				m_IconArray[ REWARD_COUNT ];	
};
