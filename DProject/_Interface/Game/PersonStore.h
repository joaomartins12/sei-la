#pragma once 

#include "PersonStoreContents.h"

#define IF_PERSONSTORE_VIEW_COUNT		5

#define IF_PERSONSTORE_NAME_EDITBOX_POS		CsPoint( 35, 36 )
#define IF_PERSONSTORE_NAME_EDITBOX_SIZE	CsPoint( 230, 16 )
#define IF_PERSONSTORE_TEX_POS				CsPoint( 27, 462 )	//판매 수수료 텍스트 위치
#define IF_PERSONSTORE_ICON_SIZE			CsPoint( 32, 32 )	//아이콘 사이즈#endif

class cPersonStore : public cBaseWindow, public PersonStoreContents::ObserverType
{
	struct sPersonStoreItem : public CUSTOMDATA
	{
		sPersonStoreItem( int nIndex ) : nSellInfoIndex( nIndex )
		{}
		~sPersonStoreItem() {}

		int			nSellInfoIndex;
// 		int			nItemCount;
// 		uint		uItemType;
// 		u8			uItemPrice;
	};

public:
	cPersonStore();
	virtual ~cPersonStore();

protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);	

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_PERSONSTORE; }	
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual eMU_TYPE		Update_ForMouse_Disable();
	virtual void			Render();

	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

	virtual void			UserDie(){ SetEnableWindow( true ); Close( false ); }
	virtual void			RecvDiedAlready(){ Close( false ); }

	void					OnMoveWindow();
	virtual bool			OnEscapeKey();


private:
	void					PressDynamicOKBtn( void* pSender, void* pData );		//2017-04-24-nova
	void					PressCancelBtn( void* pSender, void* pData );			//2017-04-24-nova
	void					PressItemBtn( void* pSender, void* pData );

	void					MakeSellItemList();
	void					AddListItem( int nIndex, u8 const& uPrice, cItemData const& ItemData );
	void					AddListBlock( bool bBlock );

private:
	cButton*				m_pBtnDynamicOK;
	cButton*				m_pCancelButton;

	cSprite*				m_pBGWin;
	//cScrollBar*				m_pScrollBar;
	cEditBox*				m_pEditName;

	cText*					m_pTexRate;
	cText*					m_pTamerName;	// EMPLOYMENT_TAMERNAME	
	
	//cSprite*				m_pItemBlock;

	cListBox*				m_pListBox;

private:
	void					ChangeBG();
	void					SetTexRate();

	//==============================================================================================================
	//
	//		기타
	//
	//==============================================================================================================
	void					ChangeWindowTitle();
	void					ChangeDefaultBtnText();

	void					SetEditNameEnable(bool enable);		//2017-04-24-nova
	void					SetTamerName( std::wstring wsName );
};


