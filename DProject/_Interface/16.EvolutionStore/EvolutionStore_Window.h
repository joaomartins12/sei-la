#pragma once

#include "../00.Contents/EvolutionStore_Contents.h"
#include "EvolutionStore_PasswordUI.h"
#include "EvolutionStore_OverlapSeletUI.h"

class cExtraEvolutionUI : public cBaseWindow, public cEvolutionStoreContents::ObserverType
{
private:
	struct sItemInfo : public CUSTOMDATA
	{
		sItemInfo(DWORD const& dwObjectIDX):dwIDX(dwObjectIDX),nObjectType(0),nSubValue(0)//  1 : 아이템, 2 : 디지몬
		{};
		~sItemInfo() {};

		DWORD dwIDX;
		int nObjectType;
		int	nSubValue;
	};

	struct sExchangeType : public CUSTOMDATA
	{
		sExchangeType(int const& nType):nExchangeType(nType)
		{};
		~sExchangeType() {};

		int nExchangeType;
	};

	struct sExchangeListData : public CUSTOMDATA
	{
		sExchangeListData(unsigned short const& nType,unsigned short const& wayType,DWORD const& dwItemIdx)
			:dwExchangeItemIDX(dwItemIdx),usExchangeType(nType),usWayType(wayType)
		{}
		~sExchangeListData() {};

		unsigned short usExchangeType;
		unsigned short usWayType;
		DWORD dwExchangeItemIDX;
	};

	struct sNeedItemControls
	{
		sNeedItemControls():pkItemIcon(NULL),pListItemBox(NULL),pkDigimonIcon(NULL),pkEnableSlot(NULL)
		{}
		~sNeedItemControls() {}
		
		void			SetHide();
		void			SetShowItemIcon( DWORD const& dwItemIDX, int const& nCount );
		void			SetShowDigimonIcon( DWORD const& dwItemIDX, std::string const& sDIgimonIconFile, int const& nCount );
		void			SetEnableSlot( bool bValue );

		cGridListBoxItem*	pListItemBox;
		cString::sICON*		pkItemIcon;
		cString::sSPRITE*	pkDigimonIcon;
		cString::sSPRITE*	pkEnableSlot;
	};

public:
	cExtraEvolutionUI();
	~cExtraEvolutionUI();

	virtual	bool				Construct(void);
	virtual eWINDOW_TYPE		GetWindowType();
	virtual void				Destroy();
	virtual void				DeleteResource();
	virtual void				Create( int nValue = 0 );

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();
	virtual eMU_TYPE			Update_ForMouse_Disable();	// ActiveWindow 이외에 마우스 Click 안되도록... 
	virtual void				Render();

	virtual bool				OnEscapeKey();
	virtual void				ResetDevice(void);
	virtual void				ResetMap(void){ Close( false ); }
	virtual bool				Close( bool bSound = true );

protected:
	void						Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void						OnClick_ExChangeList( void* pSender, void* pData );
	void						OnClick_CloseUI( void* pSender, void* pData );
	void						OnClick_Evolution(void* pSender, void* pData);
	void						OnClick_TabMenu(void* pSender, void* pData);

	void						_LoadItemList( int const& nExchangeType );
	void						_ResetNeedItemList( std::list<sNeedItemControls>& itemList );
	void						_LoadNeedItemList( std::list<sNeedItemControls>& pControls, std::list<cEvolutionStoreContents::sMaterialData> const& pItemList );
	void						_EmptyItemSlot( cGridListBox* pGridList, std::list<sNeedItemControls>& continer, int const& nMakeCount );
	void						_UpdateSelectedNeedItemEnalbe();

private:
	cButton*					m_pCloseBtn;
	cButton*					m_pEvolutionBtn;
	cRadioButton*				m_pTabMenu;
	cMoneyIF*					m_pNeedMoneyIF;			// 필요한 돈
	cListBox*					m_pListBox;
	cGridListBox*				m_pNeedItemGrid;
	cGridListBox*				m_pNeedSubItemGrid;

	cOverlapDigimonSelectUI		m_OverlapDigimonSelectUI;
	cInputPasswordUI			m_InputPassowrdUI;

	std::list<sNeedItemControls>	m_NeedItemControl;
	std::list<sNeedItemControls>	m_NeedSubItemControl;
};