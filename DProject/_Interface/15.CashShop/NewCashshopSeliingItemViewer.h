#pragma once 
#include "../00.Contents/NewCashshopContents.h"
#include "NewCashshopItem.h"

class NewCashshopSeliingItemViewer : public ItemViewer
{
	struct ItemControls
	{
		ItemControls():pkItemIcon(NULL),pPresentBtn(NULL),pkPurchaseBt(NULL),pkLine(NULL)
					  ,pkCartBt(NULL),pkPriceText(NULL),pkHitIcon(NULL),pkCashIcon(NULL)
					  ,pkItemControl(NULL),pkItemName(NULL),pkSaleIcon(NULL),pkEndSale(NULL)
		{}
		~ItemControls() {};

		void SetEmpty();
		void SetVisible( bool bValue );
		void ChangeItemIcon( DWORD const& dwIconID );
		void ChangePrice( int const& nPrice, int const& nStandardPrice );
		void ChangeSale( int const& nSale );
		void ChangeItemName( std::wstring const& wsName );
		void ChangeBtnData( DWORD const& dwGroupID, DWORD const& dwProductID );
		void SetHitType(int const& nHitType );
		void SetEndSale( bool bSale );

		cSprite* pkItemIcon;
		cButton* pPresentBtn;
		cButton* pkPurchaseBt;
		cButton* pkCartBt;
		cString::sSPRITE* pkCashIcon;
		cString::sIMAGE* pkHitIcon;
		cString::sIMAGE* pkSaleIcon;
		cString::sSPRITE* pkLine;
		cString::sTEXT*	pkPriceText;
		cString::sTEXT*	pkSalePriceText;
		cString::sTEXT*	pkItemName;
		cString::sTEXT*	pkEndSale;
		cGridListBoxItem * pkItemControl;
	};

	struct ItemButtonType : public CUSTOMDATA
	{
		int m_eType;
		DWORD dwGroupID;
		DWORD dwProductIDX;
		ItemButtonType(int etype, DWORD dwGroupID, DWORD dwIdx)
			:m_eType(etype), dwGroupID(dwGroupID), dwProductIDX(dwIdx)
		{

		}
	};

// 	struct ItemProductType : public CUSTOMDATA
// 	{
// 		DWORD dwProductIDX;
// 		ItemProductType(DWORD dwIdx)
// 			:dwProductIDX(dwIdx)
// 		{
// 
// 		}
// 	};

public:
	NewCashshopSeliingItemViewer();
	~NewCashshopSeliingItemViewer();

	virtual void			UpdatePage( int const& nMainTabIdx, int const& nSubTabIdx, int const & nPage = 1, DWORD const& dwSelectGroupID = 0);
	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			UpdateMouse();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		
	virtual	bool			Construct(void);


	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create(cWindow* pkRoot, int nValue = 0 );


private:
	void					OnClick_SellingItemFunctionBT(void* pSender, void* pData);
	void					OnItemSelected_SellingItem(void* pSender, void* pData);
	void					OnClick_NextPageBT(void* pSender, void* pData);
	void					OnClick_PrevPageBT(void* pSender, void* pData);
	void					_EmptySellingItemSlot( int const& nMakeCount );

private:
	cGridListBox*			m_pkItems;
	//Page ฐทร
	cButton*				m_pkNextPageBT;
	cButton*				m_pkPrevPageBT;
	cText*					m_pkCurrPageTT;
	std::map<int,ItemControls>	m_mapControls;
	int						m_nMainTab;
	int						m_nSubTab;
	int						m_nMaxCount;
	int						m_nMaxPage;
	int						m_nCurrentPage;
};