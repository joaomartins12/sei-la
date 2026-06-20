#pragma once 
#include "../00.Contents/NewCashshopContents.h"
#include "NewCashshopBestViewer.h"
#include "NewCashshopCartViewer.h"
#include "NewCashshopEquipViewer.h"
#include "NewCashshopSeliingItemViewer.h"
#include "NewCashshopMainViewer.h"
#include "NewCashshopWebViewer.h"
#include "NewCashshopPurchaseViewer.h"


class NewCashshopViewer : public cBaseWindow, public NewCashshopContents::ObserverType
{
	struct sCategoryType : public CUSTOMDATA
	{
	public:
		int nMaintype;
		int nSubtype;
		sCategoryType(int nMain, int nSub)
			:nMaintype(nMain),nSubtype(nSub)
		{
		}
	};

	enum eStorageType
	{
		eCashStorage,
		eCartStorage,
	};

	struct sStorageType : public CUSTOMDATA
	{
	public:
		int nStorageype;
		sStorageType(int ntype)
			:nStorageype(ntype)
		{
		}		
	};

protected:
	virtual	void				Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		

public:
	NewCashshopViewer();
	~NewCashshopViewer();

	virtual	bool				Construct(void);
	virtual eWINDOW_TYPE		GetWindowType(){ return WT_NEW_CASHSHOP; }
	virtual void				Destroy();
	virtual void				DeleteResource();
	virtual void				Create( int nValue = 0 );

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();
	virtual eMU_TYPE			Update_ForMouse_Disable();
	virtual void				Render();

	virtual void				ResetDevice(void);
	virtual void				ResetMap(void){ Close( false ); }
	virtual bool				Close( bool bSound = true );
	BOOL						UpdateMouse();					
	BOOL						UpdateKeyboard(const MSG& p_kMsg);	//2016-01-13-nova	키보드 입력처리
	bool						OnEscapeKey();
	bool						OnEnterKey();

	virtual void				UserDie(){ Close( false ); }

	///////////////////////////////////////////////////////////////////////////////////////
private:
	void						_CreateMainCategoryBT(void);	
	void						_ChangeMainCateogryBT(int const& nMainType, int const& nSubType, bool bOnEvent = true);
	void						_ChangeSubCategoryBT(int const& iMainCategory, int const& iSubCategory, bool bOnEvent = true);
	void						_MovePage(int const& nMoveMain, int const& nMoveSub);
	void						_MoveCategory( int const& nMainCategory, int const& nSubCategory );


	void						UpdateCashBalance(void);
	void						ShowPurchaseViewer(NewCashshopContents::CASHSHOP_FUNTION eBType);
	void						HidePurchaseViewer(void);
/////////////////////////////////////////////////////////////////////////////////////	//
	void						CreatePage();
	void						CreateUI(void);
	bool						CreateViewer(void);
	void						CreateEditSerchingItem(void);
	///////////////////////////////////////////////////////////////////////////////////////
	// Control Event Function
private:
	void						OnClick_MainCategoryBT(void* pSender, void* pData);
	void						OnClick_SubCategoryBT(void* pSender, void* pData);
	void						OnClick_CloseCashshop(void* pSender, void* pData);
	void						OnClick_RefreshCashState(void* pSender, void* pData);
	void						OnClick_ChargingCash(void* pSender, void* pData);
	void						OnClick_PurchaseAll(void* pSender, void* pData);
	//void						OnClick_ShowCart(void* pSender, void* pData);
	void						OnClick_ShowCashStorage(void* pSender, void* pData);
	void						OnClick_ShowCashStorageBT(void* pSender, void* pData);
/////////////////////////////////////////////////////////////////////////////////////	//
	void						OnChangeSechingEditBox(void* pkSender, void* pData);
	void						OnEnterFocusSechingEditBox(void* pkSender, void* pData);
	void						OnReleaseFocusSechingEditBox(void* pkSender, void* pData);
	void						OnClickSerching(void* pSender, void* pData);
	void						OnClickResetSerching(void* pSender, void* pData);
	///////////////////////////////////////////////////////////////////////////////////////
private:
	cRadioButton*				m_pMainCategory;	// 메인 카테고리 버튼
	cRadioButton*				m_pSubCategory;		// 서브 카테고리 버튼
	cButton*					m_pCloseBT;			// 닫기 버튼
	cButton*					m_pCashStorageBtn;	// 캐쉬 창고 버튼

	//카트 & 장바구니 탭
	cRadioButton*				m_pCashInventoryTab;

	//캐시 관련
	cButton*					m_pkChargeCashBT;
	cButton*					m_pkPurchaseAllBT;
	cButton*					m_pkRefreshCashStateBT;
	//cButton*					m_pkRefreshCashStorageBT;// 캐쉬 창고 아이템 갱신
	cText*						m_pkCashTT;
	cText*						m_pkBouseTT;
	cText*						m_pkHistoryMsg;
	
	cEditBox*					m_pEditSerchingItem;
//	cButton*					m_pMagnifierBT;		
	cButton*					m_pResetSerchingBT;  

	///////////////////////////////////////////////////////////////////////////////////////
	ItemViewer*					m_pkVWMain;
	ItemViewer*					m_pkVWSellingItem;
	ItemViewer*					m_pkCurrItemViewer;
	//ItemViewer*					m_pkBuyHistory;

	NewCashshopBestViewer*		m_pkVWBest; 
	NewCashshopWebViewer*		m_pkVWWeb;			// 메인 페이지의 웹 이미지 보여주는 창

	NewCashshopEquipViewer*		m_pkVWEquip;
//	NewCashshopStorageViewer*	m_pkVWStorage;		//
	NewCashshopCartViewer*		m_pkVWCart;

	NewCashshopPurchaseViewer	m_pkPurchaseViewer; // 아이템 구매 창 윈도우
};
