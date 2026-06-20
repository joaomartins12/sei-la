
#pragma once 

#define KIND_OF_ENCHENT_ITEM	3	// 디지클론 아이템 종류 ( 일반 / 강화 / 궁극 )

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
	#define MAX_ENCHANT_STAT_COUNT	5
#else
	#define MAX_ENCHANT_STAT_COUNT	4
#endif

class cEnchantTactics : public cBaseWindow
{
public:
	cEnchantTactics();
	~cEnchantTactics();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_ENCHANT_TACTICS; }
	virtual void			Destroy();	
	virtual void			DeleteResource();
	virtual void			PreResetMap(){ Close( false ); }
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();	
	virtual void			Render();
	
	virtual void			OnLButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );
	
	virtual void			ResetDevice();

	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			OnMoveWindow();		

	void					SetResist( int nInvenIndex );
	int						GetSelAbilityType( int const& nSelIdx );
	void					SetAutoResist();	
	int						GetSelectState() { return m_nSelectAbility; }	

	bool					SendEnchantServer();	
	void					RecvEnchantServer();	

	void					SendWeakServer();	
	void					RecvWeakServer();
	
	bool					IsSubItem();

	bool					IsDigiClon();
	void					SetNextEnchantText();

	bool					IsCheckedAbility() const;

protected:
	void					_InitEnchant();
	void					_InitWeak();

	bool					_IsDigiClonInsert();
	int						_GetSelectStat_EnchantLV();
	int						_GetDigiClonType( int nNextEnchantLV );

	void					_SetResistButtonText();			

	void					_GetEnchantMinMaxValue( DWORD const& dwItemType, int const& nEnchantType, int const& nEnchantLv, int const& nGrowLv, int& nMin, int& nMax );
	// 포인터만 있는 컨트롤
protected:	
	cButton*				m_pCancelButton;
	cButton*				m_pResistButton;	
	cButton*				m_pEnchantButton;

	cButton*				m_pDigiClonCancel;
	cButton*				m_pSubItemCancel;	

	cRadioButton*			m_pRadio;

	cSprite*				m_pOverMask;
	cSprite*				m_pNonSelectMask;
	cSprite*				m_pSelectMask;		

	cSprite*				m_pSprEnchant;
	cSprite*				m_pSprWeak;

	cSprite*				m_pHpStatimg;		
	CsRect					m_rcRect[ MAX_ENCHANT_STAT_COUNT ];		// 능력치 선택 영역
	CsRect					m_rcRectItem[ 3 ];				// 아이템 넣기
#ifdef SIMPLE_TOOLTIP
	CsRect					m_rcReqCost;
#endif
	cItemInfo*				m_DigicloneItem;
	int						m_nDigicloneSlot;	

	cItemInfo*				m_SubItem;
	int						m_nSubItemSlot;

	cMoneyIF*				m_pMoney;

	int						m_nSelectAbility;
	int						m_nOverAbility;

	bool					m_bWaitRecvServer;	

	cText*					m_pText;
	cText*					m_pExplain;

	std::vector<int>		m_nUsableClone;	// 디지클론 종류만큼

public:
	virtual cWindow*		GetChildControl( std::string const& sControlName );
	CsPoint					StateControlPos( std::string const& sControlName );
	cItemInfo*				GetRegistDigiClone();
	cItemInfo*				GetRegistSubItem();
};
