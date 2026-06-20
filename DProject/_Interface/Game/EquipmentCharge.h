#pragma once
#include "EquipSystemSubViewer.h"

class cEquipmentCharge : public cEquipSystemSubViewer
{
public:
	cEquipmentCharge();
	~cEquipmentCharge();

public:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create(cWindow* pkRoot, int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			Update_ForMouse();
	virtual void			RenderScript();
	virtual void			ResetDevice();

public:
	virtual void			OnLButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );
	virtual void			UpdateViewer();

private:
	enum eButtonState
	{
		eChargeBtn,
		eCancelBtn,
		eWithdrawBtn,
		eUnlockBtn,
	};

	struct sButtonInfo : public CUSTOMDATA
	{
		sButtonInfo( eButtonState eState, int nIndex ) : eBtnState(eState), nSlotIndex(nIndex) {}
		~sButtonInfo() {}

		eButtonState		eBtnState;
		int					nSlotIndex;
	};

	struct sChargeInfo : public CUSTOMDATA
	{
		sChargeInfo() : nLeftTime(0) {}
		~sChargeInfo() {}

		uint				nLeftTime;
	};

	struct sChargeControl
	{
		sChargeControl() : pItemIcon(NULL), pItemLock(NULL), pChargeText(NULL), pKeepText(NULL), pKeepMoneyIF(NULL), 
			pChargeButton(NULL), pItemControl(NULL) {}
		~sChargeControl() {}

		void				SetLock( u8 nMoney );
		void				SetEmpty();
		void				SetRegist( uint nItemType, uint nChargeTime, uint nKeepTime, u8 nMoney, bool bEnableMoney );
		void				SetChargeButton( bool bEnable, eButtonState eState );
		void				SetMoneyIF( u8 nKeepMoney, bool bEnableMoney );
		void				UpdateLeftTime( uint nKeepTime );
		void				ChangeItem( uint nItemType, uint nKeepTime );
		std::wstring		ConvertToKeepTimeText( uint nTime );
		std::wstring		ConvertToChargeTimeText( uint nTime );

		cString::sICON*		pItemIcon;
		cString::sSPRITE*	pItemLock;		// 잠금표시
		cString::sTEXT*		pChargeText;	// 충전 시간
		cString::sTEXT*		pKeepText;		// 보관 시간
		cString::sMONEYIF*	pKeepMoneyIF;	// 보관 비용
		cString::sBUTTON*	pChargeButton;	// 충전/취소/회수/잠금해제
		cListBoxItem*		pItemControl;
	};

	cButton*				m_pHelpButton;
	cListBox*				m_pChargeList;
	std::map< int, sChargeControl >	m_mapCharge;

private:
	void			_MakeChargeList();
	void			_AddChargeList( int const& nIndex );

	void			_RegistChargeItemSlot( int const& nSlotIndex, bool const& bEnableKeep );
	void			_UpdateChargeList( int const& nUnlockCount );
	void			_UpdateChargeTime( int const& nSlotIndex, bool bComplete );
	void			_SetNormalChargeItem( int const& nSlotIndex );
	void			_CancelChargeItem( int const& nSlotIndex );
	void			_WithdrawChargeItem( int nSlotIndex );
	void			_UnlockSlot( int const& nSlotIndex );

	void			_SetChargeItemTooltip( cListBoxItem const* pOverItem );

private:
	void			_OnClickChargeButton( void* pSender, void* pData );
	void			_OnRClickChargeItem( void* pSender, void* pData );
	void			_OnClickHelpButton( void* pSender, void* pData );
};