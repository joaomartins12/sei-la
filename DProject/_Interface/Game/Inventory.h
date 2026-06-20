#pragma once 
#include "InventoryContents.h"

#define IF_INVENTORY_ROW		10
#define IF_INVENTORY_COL		10
#define IF_INVENTORY_COUNT		100
#define IF_INVENTORY_MAX_COUNT	150

class cInventory : public cBaseWindow, public cInventoryContents::ObserverType
{
public:
	cInventory();
	~cInventory();

	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);	

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_INVENTORY; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual bool			Close( bool bSound = true );
	virtual void			ResetMap();

	virtual void			PlayOpenSound(){ cWindow::PlaySound( cWindow::SD_Ba1 ); }
	virtual void			PlayCloseSound(){ cWindow::PlaySound( cWindow::SD_Ba2 ); }

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			OnMouseOutWindow();
	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			ResetDevice();

	virtual bool			OnEscapeKey();

private:
	void					SortButtonClick(void* pSender, void* pData);
	void					CloseInvenBtnClick(void* pSender, void* pData);

protected:
	int				m_nInvenIndex;
	int				m_nDataStartIndex;
	sIFIcon			m_IFIcon[ IF_INVENTORY_COUNT ];

public:
	CsPoint			GetInvenItemPos( int const & nInvenPos );

	void			NewInstTamerName( int nInvenIndex );
	void			NewInstDigimonName( int nInvenIndex );	

protected:
	bool			_UpdateIcon_ForMouse();
	void			_UpdateTooltip( CsPoint ptPos, int nIndex );

	// 메모리잡힌 컨트롤
protected:
	cMoneyIF*		m_pMoney;
	cSprite*		m_pBlock;
	cSprite*		m_pBlockInItem;

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pCancelButton;	
	cButton*		m_pSort;

////////////////////////////////////////////////////////////////////////////////////////////

#ifdef UI_INVENTORY_RENEWAL
private:
	enum eTabType{ eTabNone, eTabEquip, eTabConsume, eTabDigimon, eTabEtc };
	struct sTabData : public CUSTOMDATA
	{
		sTabData( eTabType tabType ) : eType( tabType ) {}
		~sTabData() {}

		eTabType	eType;
	};

	cRadioButton*	m_pFilterTab;
	cScrollBar*		m_pScrollBar;
	cSprite*		m_pNewMark;
	bool			m_bIsFiltering;

private:
	void			_MakeFilterTab();
	void			_AddFilterTab( eTabType eTab, CsPoint pos, CsPoint size, std::wstring wsText );
	std::wstring	_GetFilterName( eTabType eTab );
	void			_SetNewMark( int nIndex, bool bIsNew );

	// button event
	void		OnClickFilterTab( void* pSender, void* pData );
#endif
};


