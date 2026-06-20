
#pragma once 

#ifdef FT_USE_CASHSHOP

struct sCASHSHOP_ELEMENT : public NiMemObject
{
	virtual ~sCASHSHOP_ELEMENT(){ Delete(); }

	sCASHSHOP_ELEMENT(){ s_pFT = NULL; }
	void				Delete(){ s_pFT = NULL; }
	void				Init( CsCashShop* pFT );
		
	CsCashShop*			s_pFT;
};

struct sCASHSHOP_ITEM : public NiMemObject
{
	virtual ~sCASHSHOP_ITEM(){ Delete(); }

	sCASHSHOP_ITEM(){ s_pPriceStr = NULL; }
	void				Delete();
	void				ResetDevice();
	void				SetPrice( CsCashShop* pFT, int nSize );
	BOOL				IsRect( CsPoint ptPos );	
	CsCashShop*			Find( DWORD dwID ); 

	int										s_nGroup;	
	CsVectorPB< sCASHSHOP_ELEMENT* >		s_vpElement;
	CsRect									s_rcArea;

	cString*			s_pPriceStr;
	cStringList			s_NameStr;
};

struct sCASHSHOP_GROUP : public NiMemObject
{
	virtual ~sCASHSHOP_GROUP(){ Delete(); }

public:
	void				Delete();
	void				ResetDevice();
	bool				Add( CsCashShop* pFT, CsRect rcArea, int nCutSize, int nSize = CFont::FS_10 );
	CsPoint				Update_Mouse( CsPoint ptPos, CsPoint ptPage, int& nGroup );	
	
	bool				IsItem( int nIdx ) { return s_vpItem.IsExistElement( nIdx ); }
	
	sCASHSHOP_ITEM*		GetItem( int nGroup );
	CsCashShop*			GetItemFind( int nGroup ) { return GetItem( nGroup )->s_vpElement[ 0 ]->s_pFT; }	

	CsVectorPB< sCASHSHOP_ITEM* >		s_vpItem;	
};

class cCashShop_Char 
{
public:
	cCashShop_Char();
protected:
	CTamer*			m_pTamer;
	cRenderTex*		m_pRenderTex;

	cButton*		m_pRotLeft;
	cButton*		m_pRotRight;	

protected:
	float			m_fRot;

	sIFIcon			m_IFIcon_Equip[ nLimit::Equip + 1 ];
	int				m_nEquip[ nLimit::Equip + 1 ];
	int				m_nMask;

public:
	void			Delete();	
	void			Init( CsPoint ptPos );
	void			Update(float const& fDeltaTime);
	void			Updatae_Mouse();
	void			Render( CsPoint ptParentPos );
	void			ResetDevice();		
	void			SetAvatarItem( int nItemType, int nPartIdx );
	void			SetAvatarTamer();
	void			SetAvatarDel();
};

class cCashShop_Main : public cBaseWindow
{
public:
	cCashShop_Main();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CASHSHOP; }

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );
	void					CreateElement();

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

	void					ReleaseMask() { m_ptMask = CsPoint::ZERO; }

public:
	enum eELEMENT_TYPE{ ET_NEW, ET_HIT, ET_EVENT, ET_CNT };

private:
	bool					_EnableItemPurchaseTime( int const& nType, DWORD const& dwItemCode );

protected:
	sIFIcon					m_IFIcon[ ET_CNT ][ 4 ];
	sCASHSHOP_GROUP			m_Group[ ET_CNT ];
	int						m_nGroup;	// 선택한 아이템 그룹번호

	// 마우스 오버 마스크
	cSprite*				m_pMask;
	CsPoint					m_ptMask;	

	// 페이지 교체
	cButton*				m_pLeft[ ET_CNT ];
	cButton*				m_pRight[ ET_CNT ];
	cText*					m_pPageText[ ET_CNT ];
	int						m_nCurPage[ ET_CNT ];
};

#define IF_CASHSHOP_LIST_CNT		8
#define DIGIVICE					11
class cCashShop_List : public cBaseWindow
{
public:
	cCashShop_List();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CASHSHOP; }

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	void					ReleaseMask() { m_ptMask = CsPoint::ZERO; }	

protected:
	void					_DeleteButton();
	void					_AddItem( int nSmallType, DWORD dwItemID );

public:
	enum eLIST_TYPE{ LT_TAMER = CsCashShop::eTamer, LT_DIGIMON, LT_AVATAR, LT_SET };
	
	void					Init( eLIST_TYPE eType, DWORD dwItemID = 0 );

private:
	bool					_EnableItemPurchaseTime( int const& nType, DWORD const& dwItemCode );

protected:
	eLIST_TYPE				m_eCurType;

	sIFIcon					m_IFIcon[ 8 ];
	sCASHSHOP_GROUP			m_Group;
	int						m_nGroup; // 선택한 아이템 그룹번호

	// 마우스 오버 마스크
	cSprite*				m_pMask;
	CsPoint					m_ptMask;

	cRadioButton*			m_pCategory;	

	// 페이지 교체
	cButton*				m_pLeft;
	cButton*				m_pRight;
	cText*					m_pPageText;
	int						m_nCurPage;
	int						m_nMaxPage;

	CsVectorPB< cButton* >		m_vpBuy;
	//CsVectorPB< cButton* >		m_vpPresent;
};

#define IF_CASHSHOP_BEST_CNT		5

class cCashShop_Best : public cBaseWindow
{
public:
	cCashShop_Best();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CASHSHOP; }

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );
	void					CreateElement();

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	
	void					ReleaseMask() { m_ptMask = CsPoint::ZERO; }

private:
	bool					_EnableItemPurchaseTime( int const& nType, DWORD const& dwItemCode );
	void					_GetEnableItemTime( int const& nCashType, std::vector<int>& vec, CsVectorPB< int >* pTable );

protected:
	sIFIcon					m_IFIcon[ IF_CASHSHOP_BEST_CNT ];		
	sCASHSHOP_GROUP			m_Group;
	int						m_nGroup; // 선택한 아이템 그룹번호

	// 마우스 오버 마스크
	cSprite*				m_pMask;
	CsPoint					m_ptMask;
};

class cCashShop_Pay : public cBaseWindow
{
public:
	cCashShop_Pay();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CASHSHOP; }

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();	

	virtual bool			OnEnterKey();

public:
	void					BuyOne( sCASHSHOP_ITEM* pItem, const int& iRealCash, const int& iBonusCash, const int& iTotalCash);
	//void					BuyPresent( sCASHSHOP_ITEM* pItem );	

	enum PAY_TYPE { PT_NONE = 0, PT_ONE, PT_PRESENT };
	PAY_TYPE				GetState() { return m_eType; }

protected:
	void					_MakeString( CsCashShop::sINFO* pCashItem, cString* pString );
	void					_CreateComboControl(bool bShow = true );
	void					ComboBoxSelect(void* pSender, void* pData);
//	void					_CreatePayControl();


protected:
	PAY_TYPE				m_eType;

	// 복수 결제창
	sIFIcon					m_BasketIFIcon[ 3 ];		// 아이콘 영역		
	int						m_nTotalPrice;
	
	// 선물창
	//cEditBox*				m_pEditName;
	//cButton*				m_pNameButton;
	//cEditBox*				m_pEditMessage;
	//cButton*				m_pMessageButton;
	//cText*					m_pName;
		
	// 공통 
	sCASHSHOP_ITEM*			m_pItem;	
	sCASHSHOP_GROUP			m_Group;

	cComboBox*				m_pComboPay;
//	cString*				m_pPay;

	cScrollBar*				m_pScrollBar;	
	cStringList				m_pStrList;	

	cButton*				m_pBuy;
	cButton*				m_pCancel;

	TCHAR					m_szMessage[ MAX_PATH ];	
};

#endif