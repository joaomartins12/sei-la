#pragma once 

#include "StoreContents.h"

class cStore : public cBaseWindow, public CStoreContents::ObserverType
{
	struct sStoreSellType : public CUSTOMDATA
	{
		sStoreSellType( int nType )
			: m_nSellType(nType)
		{}
		~sStoreSellType() {};

		int m_nSellType;
	};

	struct sStoreItemInfo : public CUSTOMDATA
	{
		sStoreItemInfo( int nTableIndex, int nID, int nCount )
			: m_nIndex(nTableIndex), m_nItemID( nID ), m_nItemShowCount(nCount)
		{}
		~sStoreItemInfo() {};

		int m_nIndex;			// 인덱스
		int m_nItemID;			// 아이템 ID
		int m_nItemShowCount;	// 설정된 아이템 개수
	};
	
protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
public:
	virtual	bool			Construct(void);

public:
	cStore();
	virtual ~cStore();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_STORE; }	
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

	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			PlayOpenSound(){}

	virtual void			UserDie(){ Close( false ); }
	virtual void			RecvDiedAlready(){ SetEnableWindow( true ); Close( false ); }

	void					OnMoveWindow();

	void					MakeStoreItemList( int nSellType = 0 ); // 판매목록 리스트박스 세팅
	void					MakeRepurchaseItemList(); // 판매목록 리스트박스 세팅
	void					DeleteRepurchaseItem( int nItemID, int nItemCount );  // 재구매 아이템 삭제
	void					AddRepurchaseItem( int nItemID, int nItemCount );	  // 재구매 아이템 추가

	void					AddStoreWindowTitle( int nStoreType );

private:
	void					ClickStoreItemButton( void* pSender, void* pData );
	void					ClickSellTypeTab(void* pSender, void* pData);
	void					CloseStoreWindow(void* pSender, void* pData);

	void					UpdateMyCurrencyValue();

	void					AddMoneyInfo_ByStoreType( cString* pString, CStoreContents::BaseStoreItemData* rData );
	void					CheckItemIconMouseOver();
	void					CheckCurrencyIconMouseOver();
	void					OpenItemSeparateWindow();

	void					CreateItemTypeRadioButton();

	int						GetSelectedSellType() const;

	cButton*				m_pCancelButton;	// 닫기 버튼
	cListBox*				m_pStoreItemListBox;
	cRadioButton*			m_pRadioBtn;
	cMoneyIF*				m_pCurrentMoney;
};


