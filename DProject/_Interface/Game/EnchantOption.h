#pragma once

#include "../00.Contents/EnchantOptionContents.h"

class cEnchantOption : public cBaseWindow, public cEnchantOptionContents::ObserverType
{
public:
	cEnchantOption();
	~cEnchantOption();

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

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey();
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			UserDie(){ Close( false ); }

private:
	int						m_nUIState;
	cUI_TimerProgressBar*	m_pTimeProgress;

	//////////////////////////////////////////////////////////////////////////
	// °¨Á¤ ui
private:
	void					_CreateIdentifyUI();

	void					_IdentifyProcessEnd( void* pSender, void* pData );

	//////////////////////////////////////////////////////////////////////////
	// °­È­ ui
private:
	struct sRegistItem
	{
		sRegistItem() : pItemIcon( NULL ), bIsRegist( false ) {}
		~sRegistItem() {}

		void				ChangeItem( uint nItemType );
		void				SetVisible( bool bVisible );

		cString::sICON*		pItemIcon;
		bool				bIsRegist;
	};
	cListBox*				m_pRegistStone;
	cListBox*				m_pRegistItem;

	sRegistItem				m_sEnchantStone;
	sRegistItem				m_sEnchantItem;

	cComboBox*				m_pOptValCombo;
	cButton*				m_pEnchantBtn;
	cButton*				m_pCancelBtn;

	cRenderTex*				m_pEffectRender;
	CsC_AvObject*			m_pEffectObj;

	cText*					m_pItemTypeName;

private:
	void					_CreateEnchantUI();
	void					_MakeEnchantItem();
	void					_MakeAccessoryItem();

	void					_RegistEnchantStone( uint nItemType, uint nEnchantType );
	void					_RegistEnchantItem( uint nItemType );
	void					_StartEnchant();
	void					_SuccessEnchant();

	void					_MakeOptionValueBox();
	void					_SetOptionValueBox();
	void					_AddOptionValueItem( ushort nOption, ushort nValue, uint nIndex );
	std::wstring			_GetAccOptionValue( ushort nOption, float fValue ) const;

	void					_SetEnchantButton();
	void					_SetTooltipEnchantStone( cListBoxItem const* pOverItem );
	void					_SetTooltipEnchantItem( cListBoxItem const* pOverItem );

	void					_EnchantProcessEnd( void* pSender, void* pData );

	void					_OnClickClose( void* pSender, void* pData );
	void					_OnClickEnchant( void* pSender, void* pData );
	void					_OnClickComboBox( void* pSender, void* pData );
	void					_OnRClickEnchantStone( void* pSender, void* pData );
	void					_OnRClickEnchantItem( void* pSender, void* pData );
};