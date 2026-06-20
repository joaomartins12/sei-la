#pragma once

#include "../00.Contents/GotchaMachineContents.h"

class cNewGotchaMachine : public cBaseWindow, public cGotchaMachineContents::ObserverType
{
public:
	cNewGotchaMachine();
	~cNewGotchaMachine();

	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);	

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			OnMouseOutWindow();
	virtual void			OnLButtonUp( CsPoint pos );

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey();

	virtual void			UserDie(){ Close( false ); }

private:
	struct sItemInfo : public CUSTOMDATA
	{
		sItemInfo( uint nType, uint nCount, uint nInvenCount = 0 )
			: nItemType( nType ), nItemCount( nCount ), nInvenItemCount( nInvenCount ) {}
		~sItemInfo() {}

		uint		nItemType;
		uint		nItemCount;
		uint		nInvenItemCount;
	};
	struct sGotchaItem
	{
		sGotchaItem() : pItemIcon(NULL), pItemCount(NULL) {}
		~sGotchaItem() {}

		void				ChangeItemCount( int nCount );

		cString::sICON*		pItemIcon;
		cString::sTEXT*		pItemCount;
	};
	cListBox*			m_pRareItems;
	std::map< int, sGotchaItem >		m_mapRareItems;
	struct sUseItem
	{
		sUseItem() : pItemCount(NULL), pItemControl(NULL) {}
		~sUseItem() {}

		uint				GetItemType();
		void				ChangeUseItemCount( int nCount );

		cString::sTEXT*		pItemCount;
		cGridListBoxItem*	pItemControl;
	};
	cGridListBox*		m_pUseItems;
	std::map< int, sUseItem >		m_mapUseItems;
	cButton*			m_pCloseButton;
	cButton*			m_pGotchaButton;
	cText*				m_pRemainItemCount;
	cMoneyIF*			m_pUseMoney;

private:
	void				_InitGotchaMachine();
	void				_InitMysteryEggMachine();

	void				_MakeRareItemList();
	void				_AddRareItem( int nIndex, uint nItemType, uint nItemCount, SystemType::eShowFlag eFlag );

	void				_MakeUseItemList();
	void				_AddUseItem( int nIndex );

	void				_UpdateGotchaMachine();
	bool				_SetRareItemTooltip( cListBoxItem const* pOverItem );
	bool				_SetUseItemTooltip( cGridListBoxItem const* pOverItem );

	void				_OnClickCloseButton( void* pSender, void* pData );
	void				_OnClickGotchaButton( void* pSender, void* pData );
};