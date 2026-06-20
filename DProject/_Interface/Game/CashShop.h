
#pragma once 

#ifdef FT_USE_CASHSHOP

#include "CashShop__Etc.h"
#include "CashShopContents.h"

class cCashShop : public cBaseWindow, public CashShopContents::ObserverType
{
protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual	bool			Construct(void);
	cCashShop();
	virtual ~cCashShop();

#ifndef VERSION_KOR
	enum eUNIT { U_Each, U_Long, U_Day, U_MAXCNT };
#endif

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CASHSHOP; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );
	void					CreateElement();

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual eMU_TYPE		Update_ForMouse_Disable();
	virtual void			Render();

	virtual void			OnRButtonUp( CsPoint pos );
	virtual bool			OnEscapeKey();
	virtual bool			OnEnterKey();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

	void					SetItemLink( CsCashShop* pItem );
	void					SetAvatarItem( int nItemType, int nPartIdx ) { m_SubWin_Char.SetAvatarItem( nItemType, nPartIdx ); }	

	void					SetPay( sCASHSHOP_ITEM* pItem, cCashShop_Pay::PAY_TYPE ePay ); 
	void					ClosedPay() { m_SubWin_Pay.Close(); }
#ifndef VERSION_KOR
	cSprite*				GetSprite( eUNIT Type ) { return m_pCashUnit[ Type ]; }
#endif

	int						GetCashType();

	cImage*					GetMoneyImg(){ return m_pMoneyImg; }
	void					SetCashMoney( int nCashType, int nMoney );


private:
	void					_OpenFreeChargeWebPage(void* pSender, void* pData);
	void					_ResetCashShopItemData();
	void					_RefreshCashMoney();

	// 포인터만 있는 컨트롤
private:
	cButton*		m_pCancelButton;	

	cButton*		m_pEquipAllDel;
	cButton*		m_pEquipReset;

	cButton*		m_pCashWareHouse;

	cButton*		m_pCashRefresh;
	cButton*		m_pFreeCashCharge;

	cImage*			m_pMoneyImg;
	cText*			m_pCashMoney[ CsCashShopMng::CT_MAXCNT ];

	cButton*		m_pHaveCashRefresh;

	// 메모리잡힌 컨트롤
protected:
#ifndef VERSION_KOR
	cSprite*		m_pCashUnit[ U_MAXCNT ];
#endif
	cRadioButton*	m_pCategory_First;	

protected:	
	cCashShop_Main	m_SubWin_Main;
	cCashShop_Best	m_SubWin_Best;
	cCashShop_Char	m_SubWin_Char;
	cCashShop_List	m_SubWin_List;
	cCashShop_Pay	m_SubWin_Pay;
};

#endif

