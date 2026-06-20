
#pragma once 


class cItemSeparate : public cBaseWindow
{
public:
	enum eTYPE
	{ 
		SELL_RES, // 판매 등록 확인창
		BUY_RES,  // 구매 등록 확인창 /*160421 미구현*/
		SELL,     // 
		BUY, 
	};

	enum eEditBox{ EDITBOX_ITEMCOUNT, EDITBOX_TERA, EDITBOX_MEGA, EDITBOX_BIT, EDITBOX_MAX}; // [4/8/2016 hyun] 에디트박스 enum

public:
	cItemSeparate();
	virtual ~cItemSeparate(){ Destroy(); }

public:
	static void		NewInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, eTYPE eSepType, cItemInfo* pItemInfo, int nItemMaxCnt, sMONEY sMoneyPerItem, cMoneyIF::eDISP_TYPE disp );
	static void		NewInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, eTYPE eSepType, cItemInfo* pItemInfo, int nItemMaxCnt, sMONEY sMoneyPerItem, cMoneyIF::eDISP_TYPE disp, DWORD dwPriceItemCode );

	static bool		UpdateInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static eMU_TYPE Update_ForMouseInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static void		RenderInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static bool		DeleteInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static bool		DeleteInstance();

	static bool		IsInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_ITEM_SEPARATE; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 ){}

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

	void					OnOk();
	void					OnCancel();

protected:
	void					_OnOk_PersonStore();
	void					_OnOk_Store();

public:
	bool					OnChangeEdit( DWORD nCtrlID );

	uint						GetRegistItemTypeID() const;

protected:
	void					_SetCount( int nCount, bool bSetEditControl );
	int						_GetCount();

	void					_SetMoney( sMONEY money );
	sMONEY					_GetMoney();

	// 메모리잡힌 컨트롤
protected:

	// 포인터만 있는 컨트롤
protected:
	cButton*					m_pBtnOk;
	cButton*					m_pBtnCancel;
	cButton*					m_pCntDecrease;
	cButton*					m_pCntIncrease;
	cButton*					m_pCntDecreaseEnd;
	cButton*					m_pCntIncreaseEnd;
	cStringList					m_strItemName;
	cMoneyIF*					m_pMoneyIF;

	//  [4/6/2016 hyun] 추가
private:
	std::vector<cEditBox*>		m_vecEditBox;
	// @@ 여기까지

protected:
	cBaseWindow::eWINDOW_TYPE	m_eTargetWindowType;
	eTYPE						m_eSepType;
	int							m_nTargetWTValue1;
	cMoneyIF::eDISP_TYPE		m_eDispType;

	sMONEY			m_MoneyPerItem;
	cItemInfo		m_ItemInfo;
	int				m_nItemMaxCount;
	int				m_EndTime;
	HWND			m_hEditTera;
	HWND			m_hEditMega;
	HWND			m_hEditBit;
	HWND			m_hEditCnt;

	HWND			m_EditPropertyA;
	HWND			m_EditPropertyB;
	HWND			m_EditPropertyC;
	HWND			m_EditPropertyD;
	DWORD			m_dwNpcID;

	HBRUSH			m_hEditBrush;
	HFONT			m_hEditFont;

public:
	static void		NewInstance( cBaseWindow::eWINDOW_TYPE wt, DWORD dwNpcID, int nWTValue1, eTYPE eSepType, cItemInfo* pItemInfo, int nItemMaxCnt, sMONEY sMoneyPerItem, cMoneyIF::eDISP_TYPE disp );
	void			_Set_NpcID(DWORD dwNpcID) { m_dwNpcID = dwNpcID; }
	void			Init( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, eTYPE eSepType, cItemInfo* pItemInfo, int nItemMaxCnt, sMONEY sMoneyPerItem, cMoneyIF::eDISP_TYPE disp, DWORD dwPriceItemCode = 0 );

	cBaseWindow::eWINDOW_TYPE	GetTargetWindowType(){ return m_eTargetWindowType; }
	int							GetTargetWTValue1(){ return m_nTargetWTValue1; }

	LRESULT			ApplyEditColor( HWND hEdit, HDC dc );

public:
	struct sDiscount
	{
		cText*				s_pText;
		CsPoint				s_Pos;
		cText::sTEXTINFO	s_Font;
		CFont::eFACE_SIZE	s_FontSize;	
	};
protected:
	sDiscount	m_sDiscountInfo;
	bool			_IsEnableDiscountStore()const;	// NPC 할인 적용이 가능한 상점인가?
public:
	void			InitPassiveText();
	void			SetDiscountText(TCHAR* szName, int nValue);
	void			Passive_UpdateDiscount(sMONEY sRefreshCost, int nMaxCount);	// 낱개 구매 가격 변동 시 업데이트

	// [4/5/2016 hyun] 추가
private:
	void			CreateEditBox();
	bool			IsEditMoney(eTYPE eSeparateType);
	void			SetMoneyInEditBox(sMONEY money);
	sMONEY			GetMoneyInEditBox();
	void			ChangeInputItemCount(void* pSender, void* pData);
	//@@ 여기까지

	void			SetMaxBuyCount( int nCount );// 구매할 때 구매할 수 있는 최대 갯수 셋팅
	void			SetMaxSelCount( int nCount );// 판매할 때 판매할 수 있는 아이템의 최대 갯수 셋팅
};

extern cItemSeparate*		g_pItemSeparate;