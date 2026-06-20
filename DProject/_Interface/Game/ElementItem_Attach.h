
#pragma once
#include "../00.Contents/EquipSystemContents.h"

#define IF_EI_ITEM_SOCK_KIND			5

class cElementItem_Attach : public cBaseWindow, public cEquipSystemContents::ObserverType
{
public:
	enum eSOCKET{ EMPTY_SOCKET = -2, EXIST_SOCKET = -1, };

public:
	cElementItem_Attach();
	~cElementItem_Attach();

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

	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			ResetDevice();
	virtual void			ResetMap();

	virtual bool			OnEscapeKey();
	virtual void			OnMoveWindow();

	virtual void			PlayOpenSound(){}
	virtual void			UserDie();
	virtual void			RecvDiedAlready();

	// 메모리잡힌 컨트롤
private:
	cMoneyIF*				m_pMoneyAttach;
	cMoneyIF*				m_pMoneyDetach;

	// 포인터만 있는 컨트롤
private:
	cButton*				m_pBtnAttach;
	cButton*				m_pBtnDetach;

	cButton*				m_pCancelButton;
	cText*					m_pItemName;

/////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	struct sRegistItem
	{
		sRegistItem() : pItemIcon( NULL ), pItemControl( NULL ) {}
		~sRegistItem() {}

		void		ChangeRegistItem( uint nItemType, int nInvenIndex );
		void		Reset();

		cString::sICON*		pItemIcon;
		cListBoxItem*		pItemControl;
	};

	struct sSocketItem
	{
		sSocketItem() : pItemIcon( NULL ), pAttachedImg( NULL ), 
			pSelectedImg( NULL ), pItemControl( NULL ) {}
		~sSocketItem() {}

		void		ChangeSocketItem( uint nItemType, u1 nRate );
		void		SetAttached( bool bVisible );
		void		SetSelected( bool bVisible );
		void		Reset();

		cString::sICON*		pItemIcon;
		cString::sSPRITE*	pAttachedImg;
		cString::sSPRITE*	pSelectedImg;
		cGridListBoxItem*	pItemControl;
	};

	struct sItemInfo : public CUSTOMDATA
	{
		sItemInfo( int nIndex ) : nInvenIndex( nIndex ) {}
		~sItemInfo() {}

		int					nInvenIndex;
	};

	struct sElemInfo : public CUSTOMDATA
	{
		sElemInfo( uint nType, u1 nRate ) : nItemType( nType ), nItemRate( nRate ) {}
		~sElemInfo() {}

		void SetItem( uint nType, u1 nRate )
		{
			nItemType = nType;
			nItemRate = nRate;
		}

		uint				nItemType;
		u1					nItemRate;
	};

	sRegistItem			m_RegistItem;
	std::map< int, sSocketItem >	m_mapSocket;
	cListBox*			m_pListBox;
	cGridListBox*		m_pGridBox;

private:
	void			_OnClickCancelButton( void* pSender, void* pData );
	void			_OnClickAttachButton( void* pSender, void* pData );
	void			_OnClickDetachButton( void* pSender, void* pData );
	void			_OnRClickRegistItem( void* pSender, void* pData );
	void			_OnClickElementSocket( void* pSender, void* pData );
	void			_OnRClickElementSocket( void* pSender, void* pData );

	void			_MakeRegistItem();
	void			_MakeSocketSlot( int const& nSlotCount );
	void			_AddElementSocket( int const& nSlotIndex, int const& nItemType, u1 const& nRate, bool const& bAttach );

	void			_SetRegistItem( uint const& nInvenIndex, int const& nSocketCount, std::wstring const& wsItemName );
	void			_SetRegistElem( int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable );
	void			_SelectElement( int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable );
	void			_ReleaseElement( int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable, bool const& bReleaseAll );

	void			_SetMoney( cMoneyIF* pMoneyIF, bool bVisible, bool bEnable, u8 nMoney );
	void			_SetAttachButton( bool bEnable );
	void			_SetDetachButton( bool bEnable );
	void			_SetTooltipRegistItem( cListBoxItem const* pOverItem );
	void			_SetTooltipSocketItem( cGridListBoxItem const* pOverItem );
	void			_SetSimpleTooltip( CsPoint ptPos );

	void			_SuccessAttach();
	void			_SuccessDetach();
	void			_ResetRegist();
};