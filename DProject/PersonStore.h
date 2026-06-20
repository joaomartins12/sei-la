

#pragma once 

#define IF_PERSONSTORE_VIEW_COUNT		5
#define IF_PERSONSTORE_WIN_COUNT		4

//#define FT_PERSONSTORE_USE_BUY

class cPersonStore : public cBaseWindow
{
public:
	enum eVALUE_TYPE{ VT_RES_PERSON, VT_RES_PERSON_START, VT_RES_EMPLOYMENT, VT_RES_EMPLOYMENT_START, VT_OPEN_PERSON, VT_OPEN_EMPLOYMENT };

public:
	cPersonStore();

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

	virtual bool			Close( bool bSound = true );

	virtual void			UserDie(){ SetEnableWindow( true ); Close( false ); }
	virtual void			RecvDiedAlready(){ Close( false ); }

	void					OnMoveWindow();
	virtual bool			OnEscapeKey(){ return Close(); }
	virtual bool			OnMacroKey(const MSG& p_kMsg);		//2017-01-04-nova 아이템등록전에 ESC가 KEY_UP 메시지가 들어와 OnEscapeKey() 처리가 안되어 OnMacroKey에서 따로 처리


protected:
	void					_UpdateItemBtn();
	void					_UpdateDynamicBtn();
	void					_Ok();
	u8						_CalTexMoney( u8 nNeedMoney );

	// 메모리잡힌 컨트롤
protected:
	cButton*		m_pBtnDynamicOK;
	cEditBox*		m_pEditName;
	cText*			m_pStoreName;

	cSprite*		m_pBGWin[ IF_PERSONSTORE_WIN_COUNT ];
	cButton*		m_pItemBtn[ IF_PERSONSTORE_VIEW_COUNT ];
	cSprite*		m_pItemBlock;
	cText*			m_pTexRate;

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pCancelButton;	

	cScrollBar*		m_pScrollBar;

protected:
	CsC_AvObject*		m_pTarget;
	int					m_nTargetIDX;
	eVALUE_TYPE			m_eValueType;
	int					m_nSelectIndex;
	int					m_nUsedItemID;

	struct sINFO : public NiMemObject
	{
		sINFO(){}
		virtual ~sINFO(){ s_Name.Delete(); s_CountText.Delete(); s_PerItemText.Delete(); s_MoneyIF.Delete(); }

		cItemInfo		s_Item;
		cStringList		s_Name;
		cText			s_CountText;
		cText			s_PerItemText;
		cMoneyIF		s_MoneyIF;

		void			Render( CsPoint pos );
	};

	CsRect				m_rcRect[ IF_PERSONSTORE_VIEW_COUNT ];		// 아이콘 영역	

	CsVectorPB< sINFO* >	m_vpSellInfo;

	//==============================================================================================================
	//
	//		판매 등록
	//
	//==============================================================================================================
public:
	void			ReadySuccess();
	void			OpenSuccess();
	void			Open2ReadySuccess();
	void			OpenMyEmployment();
	void			DeleteResistItem();

	void			ItemTrade_BuySuccess( int nSlotIndex, int nCount );

	void			ItemTrade_BuyFail( int nResult );

	void			ResistItem_Sell( cItemInfo* pItem, sMONEY MoneyPerItem );
	void			Item_Buy( cItemInfo* pItem );			

protected:
	bool			_ResistItem( CsVectorPB< sINFO* >* pVector, cItemInfo* pItem, sMONEY MoneyPerItem );
	

	//==============================================================================================================
	//
	//		기타
	//
	//==============================================================================================================
public:
	void					ChangeWindowTitle();
	void					ChangeDefaultBtnText();

	void					SetStoreName( TCHAR const * sz );
	void					SetEditName( TCHAR const* sz );

	void					SetTarget( CsC_AvObject* pTarget );
	void					SetUsedItemID( int nItemID ){ m_nUsedItemID = nItemID; }

	CsVectorPB< sINFO* >*	GetCurVector();
	sINFO*					GetInfo( int nItemID, int nRate, sMONEY MoneyPerItem );

	eVALUE_TYPE				GetValueType(){ return m_eValueType; }
	
	int						GetResItemCount( int nItemID, int nRate );
	//chu8820
	void					ClosePerson();
#ifdef EMPLOYMENT_TAMERNAME
	void					SetTamerName( TCHAR const* sz );
protected:
	cText*			m_pTamerName;
#endif
};


