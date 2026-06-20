#pragma once 
#include "DigitamaContents.h"
class cDigitama_Scan : public cBaseWindow, public DigitamaContents::ObserverType
{
public:
	cDigitama_Scan();
	virtual ~cDigitama_Scan();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_DIGITAMA_SCAN; }	
	virtual bool			Close( bool bSound = true ){ return cBaseWindow::Close( bSound ); }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			ResetDevice();
	virtual void			PreResetMap();
	virtual void			ResetMap(){ Close( false ); }

	virtual bool			OnEscapeKey();
	virtual void			OnMoveWindow();

	virtual void			PlayOpenSound(){}

	virtual void			UserDie(){ Close( false ); }
	virtual void			RecvDiedAlready(){ Close( false ); }

protected:
	void					PressOKCancelExitBtn( void* pSender, void* pData );	//2017-03-23-nova
	void					PressCancelButton( void* pSender, void* pData );	//2017-03-23-nova
	void					PressAutoResistButton( void* pSender, void* pData );//2017-03-23-nova
#ifdef UI_ITEMSCAN_RENEWAL
	void					PressScanCancelButton( void* pSender, void* pData );
	void					PressSkipButton( void* pSender, void* pData );
#endif

private:
	struct sButtonInfo : public CUSTOMDATA
	{
		sButtonInfo() : eState( BTS_EXIT ) {}
		~sButtonInfo() {}
		enum eBtnState { BTS_OK, BTS_CANCEL, BTS_EXIT };

		eBtnState	eState;
	};

	struct sRegistInfo : public CUSTOMDATA
	{
		sRegistInfo(uint nType) : nItemType(nType) {}
		~sRegistInfo() {}

		uint	nItemType;
	};

	struct sRegisterItem
	{
		sRegisterItem() : pItemIcon( NULL ), pItemControl( NULL ) {}
		~sRegisterItem() {}

		void		ChangeItem( int nIconType, uint nType, int nCount = 1 );
		void		DecreaseItem( int nCount );
		void		Reset();
		bool		IsEnable();

		cString::sICON*		pItemIcon;
		cListBoxItem*		pItemControl;
	};

	struct sScanItemInfo : public CUSTOMDATA
	{
		sScanItemInfo(uint nType) : nItemType( nType ) {}
		~sScanItemInfo() {}

		uint		nItemType;
	};

	struct sScanItemCon
	{
		sScanItemCon() : pItemIcon(NULL), pItemControl(NULL) {}
		~sScanItemCon() {}

#ifdef UI_ITEMSCAN_RENEWAL
		void		ChangeData( ICONITEM::eTYPE nIconType, uint nItemType, uint nItemCount, int nItemRank );
#else
		void		ChangeData( ICONITEM::eTYPE nIconType, uint nItemType, uint nItemCount );
#endif
		void		ResetData();

#ifdef UI_ITEMSCAN_RENEWAL
		cString::sSPRITE*	pSlotImg;
		cString::sSPRITE*	pRankImg;
#endif
		cString::sICON*		pItemIcon;
		cGridListBoxItem*	pItemControl;
	};
	
	std::list< sScanItemCon >	m_lScanItems;

	cGridListBox*		m_pGridBox;
#ifdef UI_ITEMSCAN_RENEWAL
	cButton*			m_pSkipBtn;
	cButton*			m_pScanCancelBtn;
	cSprite*			m_pProgressBgImg;
#endif
	cListBox*			m_pListBox;
	sRegisterItem		m_RegisterItem;

#ifdef SIMPLE_TOOLTIP
	CsRect				m_rcReqbit;
#endif

	bool				m_bArrowShow;
	int					m_nOper;
	NiPoint2			m_ptArrowPos;

	cText*				m_pTextResistCount;
	cSprite*			m_pArrow;	

	cButton*			m_pBtnOCE;			//2017-03-23-nova 확인|취소|나가기 버튼
	cButton*			m_pCancelButton;	//취소
	cButton*			m_pBtnAutoResist;	//자동등록

	cMoneyIF*			m_pMoneyIF;

	cUI_TimerProgressBar*	m_pProgressBar;

public:	
	void			SetArrowShow( bool bSet );

private:
	void			_ProcessFail();
	void			_ProcessSuccess(void* pSender, void* pData);
	void			_ProcessCancel();
	void			_ProcessStart();
	void			_ProcessRegister(u8 nMoney, uint nCount, uint nType);
	void			_ProcessEnd();
	void			_ProcessExit();

	void			_ResetRegistItem();
#ifdef UI_ITEMSCAN_RENEWAL
	void			_UpdateScanItem( uint nScanType );
#else
	void			_UpdateScanItem();
#endif
	void			_MakeEmptyGridBox( int nItemCount );
	void			_AddGridItem( int nIndex );

	void			_ChangeButton( sButtonInfo::eBtnState eState );
	void			_SetProgressBar( bool bIsPlay );

	void			_AddRegisterItem();
	
public:
	cItemInfo*		GetResistItem();
};