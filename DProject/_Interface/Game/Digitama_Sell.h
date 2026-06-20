#pragma once 
#include "DigitamaContents.h"

class cDigitama_Sell : public cBaseWindow, public DigitamaContents::ObserverType
{
public:
	cDigitama_Sell();
	virtual ~cDigitama_Sell();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_DIGITAMA_SELL; }
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

		void		ChangeItem( int nIconType, uint nType )
		{
			pItemIcon->ChangeIcon( nIconType, nType, 1, 0 );
			pItemIcon->SetVisible( true );
			sRegistInfo* pInfo = dynamic_cast<sRegistInfo*>(pItemControl->GetUserData());
			SAFE_POINTER_RET(pInfo);
			pInfo->nItemType = nType;
		}

		cString::sICON*		pItemIcon;
		cListBoxItem*		pItemControl;
	};

	cButton*			m_pBtnOCE;			//2017-03-28-nova 확인|취소|나가기 버튼
	cButton*			m_pBtnAutoResist;
	cButton*			m_pCancelButton;

	cText*				m_pTextResistCount;
	
	cUI_TimerProgressBar*		m_pProgressBar;
	
	cMoneyIF*			m_pSaleMoney;

	cListBox*			m_pListBox;
	sRegisterItem		m_RegisterItem;

#ifdef SIMPLE_TOOLTIP
	CsRect				m_rcRecvBit;
#endif

private:
	void			_ProcessFail();
	void			_ProcessSuccess( void* pSender, void* pData );
	void			_ProcessCancel();
	void			_ProcessStart();
	void			_ProcessRegister(u8 nMoney, uint nCount, uint nType);
	void			_ProcessEnd();
	void			_ProcessExit();

	void			_ChangeButton( sButtonInfo::eBtnState eState );
	void			_SetProgressBar( bool bIsPlay );

	void			_AddRegisterItem();

public:
	cItemInfo*		GetResistItem();
};