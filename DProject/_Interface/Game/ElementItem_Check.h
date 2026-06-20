#pragma once
#include "../00.Contents/EquipSystemContents.h"

class cElementItem_Check : public cBaseWindow, public cEquipSystemContents::ObserverType
{
public:
	cElementItem_Check();
	~cElementItem_Check();

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

private:
#ifdef SIMPLE_TOOLTIP
	CsRect					m_rcReqCost;
#endif

//////////////////////////////////////////////////////////////////////////
private:
	struct sButtonInfo : public CUSTOMDATA
	{
		sButtonInfo() : eBtnState( eAnalysis ) {}
		~sButtonInfo() {}
		enum eButtonState{ eAnalysis, eSuccess, eCancel };

		eButtonState eBtnState;
	};

	struct sRegistItem
	{
		sRegistItem() : pItemIcon( NULL ), pItemControl( NULL ) {}
		~sRegistItem() {}

		void		ChangeItem( uint nType, u1 nRate );
		void		ChangeRate( u1 nRate );
		void		Reset();

		cString::sICON*		pItemIcon;
		cListBoxItem*		pItemControl;
	};

	struct sItemInfo : public CUSTOMDATA
	{
		sItemInfo( uint nType, u1 nRate ) : nItemType( nType ), nItemRate( nRate ) {}
		~sItemInfo() {}

		void SetItem( uint nType, u1 nRate )
		{
			nItemType = nType;
			nItemRate = nRate;
		}

		uint				nItemType;
		u1					nItemRate;
	};

	cUI_TimerProgressBar*	m_pProgressBar;
	cMoneyIF*				m_pMoney;
	cButton*				m_pAnalysisBtn;
	cButton*				m_pCancelButton;

	cListBox*				m_pListBox;
	sRegistItem				m_RegistItem;

private:
	void			_OnClickCancelButton( void* pSender, void* pData );
	void			_OnClickAnalysisButton( void* pSender, void* pData );
	void			_ProcessSuccess( void* pSender, void* pData );

	void			_MakeRegistItem();

	void			_SetProgressBar( bool bPlay );
	void			_SetMoney( bool bVisible, bool bEnable, u8 nMoney );
	void			_SetRegistItem( uint nItemType, u8 nMoney, bool bEnable );
	void			_SetRegisterTooltip( cListBoxItem const* pOverItem );

	void			_SetAnalysisButton(sButtonInfo::eButtonState eState, bool bEnable = true);
	void			_SuccessAnalysis( u1 nRate );
};