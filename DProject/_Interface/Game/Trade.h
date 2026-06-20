
#pragma once 

#include "../00.Contents/Trade_Contents.h"


class cTrade : public cBaseWindow, public CTradeContents::ObserverType
{
public:
	cTrade();
	virtual ~cTrade();

	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			OnRButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );


	virtual void			ResetDevice();
	virtual void			PreResetMap();
	virtual bool			OnEscapeKey();
	virtual void			OnMoveWindow();

protected:
	bool					_UpdateIcon_ForMouse();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					CloseWindowEvent(void* pSender, void* pData);
	void					ShowMoneyInputWindow(void* pSender, void* pData);
	void					Trade_Inven_Lock(void* pSender, void* pData);
	void					Trade_Inven_UnLock(void* pSender, void* pData);
	void					SendTradeFinal(void* pSender, void* pData);

public:
	//void					SetTarget( CsC_AvObject* pTarget );
	bool					IsResist( int nInvenIndex );
	//void					SetResist( int nInvenIndex, int nCount );

private:

	void					SetTradeStandBy();
	void					ChangeTradeMoney( bool bMy );
	void					ChangeTradeInvenLock();
	void					RegistItem( bool bMy, int const& nSlotNum );
	void					UnRegistItem( bool bMy, int const& nSlotNum );
	void					AddTradeChatMsg(std::wstring const& msg, NiColor const& nColor);

private:
	cButton*				m_pTradeButton;
	cButton*				m_pCancelButton;
	cButton*				m_pBtnOK;
	cButton*				m_pBtnCancel;
	cSprite*				m_pMyReadyWin;
	cSprite*				m_pTargetReadyWin;
	cMoneyIF*				m_pMyMoney;
	cMoneyIF*				m_pTargetMoney;
	cButton*				m_pMyMoneyBtn;
	cListBox*				m_pTradeChat;

private:
	struct sINFO
	{
		sINFO()	{};
		~sINFO(){};
		
		void		Reset()
		{ 
			s_Item.Reset(); 
			s_ItemName.Delete(); 
		}

		cItemInfo	s_Item;
		sIFIcon		s_IFIcon;
		cStringList	s_ItemName;
	};

	sINFO			m_MyInfo[ MAX_TRADE_ITEM_SLOT ];
	sINFO			m_TargetInfo[ MAX_TRADE_ITEM_SLOT ];	
};

