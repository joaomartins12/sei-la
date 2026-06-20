#pragma once
#include "EquipSystemSubViewer.h"

class cEquipmentChargeQuick : public cEquipSystemSubViewer
{
public:
	cEquipmentChargeQuick();
	~cEquipmentChargeQuick();

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
	struct sItemInfo : public CUSTOMDATA
	{
		sItemInfo() : nItemType(0), nItemCount(0) {}
		~sItemInfo() {}

		uint		nItemType;
		uint		nItemCount;
	};

	struct sRegistItemInfo
	{
		sRegistItemInfo() : pItemIcon(NULL), pItemName(NULL), pItemControl(NULL) {}
		~sRegistItemInfo() {}

		void				ChangeItem( uint nItemType, std::wstring wsItemName, uint nItemCount = 1 );
		void				Reset();

		cString::sICON*		pItemIcon;
		cString::sTEXT*		pItemName;
		cListBoxItem*		pItemControl;
	};

	cListBox*			m_pChargeItem;
	cListBox*			m_pQuickItem;
	cComboBox*			m_pQuickItemSelect;
	cButton*			m_pQuickItemButton;
	cButton*			m_pHelpButton;

	sRegistItemInfo		m_sChargeItem;
	sRegistItemInfo		m_sQuickItem;

private:
	void			_MakeChargeItem();
	void			_MakeQuickChargeItem();
	void			_MakeComboBox();

	void			_AddQuickChargeItem();
	void			_AddComboBoxItem( int nDay, int nCount, NiColor niColor );

	void			_ResetQuickCharge();
	void			_RegistChargeItem( uint const& nItemType, std::wstring const& wsItemName );
	void			_RegistQuickChargeItem( uint const& nItemType, DWORD const& dwItemCount, std::wstring const& wsItemName );

	void			_SetChargeItemTooltip();
	void			_SetQuickItemTooltip();
	void			_SetExplainChargeButton();

	int				_GetDayByQuickChargeIndex(int const& nIndex);

	void			_OnClickQuickChargeButton( void* pSender, void* pData );	// 즉시 충전
	void			_OnClickComboBox( void* pSender, void* pData );			// 캐시 아이템 개수 선택
	void			_OnRClickChargeItem( void* pSender, void* pData );
	void			_OnRClickQuickItem( void* pSender, void* pData );
	void			_OnClickHelpButton( void* pSender, void* pData );
};