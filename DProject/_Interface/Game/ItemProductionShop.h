#pragma once 

#include "ItemProduction_Contents.h"

class cItemProductionShop : public cBaseWindow, public CItemProductionContents::ObserverType
{
	struct sAssistItem : public CUSTOMDATA
	{
		sAssistItem(int nAssistType, DWORD dwItemCode, int nItemCount)
			: s_nAssistType(nAssistType), s_dwItemCode(dwItemCode),s_nItemCount(nItemCount)
		{}
		~sAssistItem() {};
		
		int		s_nAssistType;//
		DWORD	s_dwItemCode;
		int		s_nItemCount;
	};

	struct sCategotyType : public CUSTOMDATA
	{
		sCategotyType(int nMainIdx, int nSubIdx)
			: m_nMainIdx(nMainIdx),m_nSubIdx(nSubIdx)
		{}
		~sCategotyType() {};

		int m_nMainIdx;
		int m_nSubIdx;
	};

	struct sProductionType : public sCategotyType
	{
		sProductionType(int nMainIdx, int nSubIdx, int nUniqueIdx, DWORD dwItemIdx)
			:sCategotyType(nMainIdx, nSubIdx),m_nUniqueIdx(nUniqueIdx),m_dwItemIdx(dwItemIdx)
		{}
		~sProductionType() {};

		int m_nUniqueIdx;
		DWORD m_dwItemIdx;
	};

	struct sMaterialType : public CUSTOMDATA
	{
		sMaterialType(DWORD dwItemIdx)
			: m_dwItemIdx(dwItemIdx)
		{}
		~sMaterialType() {};
		DWORD m_dwItemIdx;
	};

	struct sMaterialItemInfo
	{
		sMaterialItemInfo():m_dwNeedItemCount(0),m_pText(NULL),m_dwHaveItemCount(0),m_dwMakeItemCount(0)
		{};
		~sMaterialItemInfo()
		{
			m_dwNeedItemCount = 0;
			m_dwHaveItemCount = 0;
			m_dwMakeItemCount = 0;
			m_pText = NULL;
		};

		void	UpdateData();
		void	UpdateMakeItemCount( DWORD const& dwMakeItemcount );
		void	UpdateHaveItemCount( DWORD const& dwHaveItemcount );

		DWORD			m_dwNeedItemCount;
		DWORD			m_dwHaveItemCount;
		DWORD			m_dwMakeItemCount;
		cString::sTEXT* m_pText;
	};

	typedef std::map<DWORD, sMaterialItemInfo>					MAP_MaterialItems;
	typedef std::map<DWORD,	sMaterialItemInfo>::iterator		MAP_MaterialItems_IT;
	typedef std::map<DWORD, sMaterialItemInfo>::const_iterator	MAP_MaterialItems_CIT;

	struct sAssistItemUIControls
	{
		sAssistItemUIControls():m_pItemIcon(NULL),m_pLockSlotImg(NULL),m_pAddSlotImg(NULL)
		{};
		~sAssistItemUIControls()
		{};

		void	SetLock(bool bLock);
		void	SetItemIcon(int const& nItemCode, int const& nItemCount);		
		bool	UpdateItemcount(int const& nItemCode, int const& nItemCount);		
		DWORD	GetAssistItemCode() const;

		sAssistItem*	m_pUserData;
		cString::sICON* m_pItemIcon;
		cSprite*		m_pLockSlotImg;
		cSprite*		m_pAddSlotImg;
	};

	typedef std::map<int, sAssistItemUIControls>					MAP_AssistItems;
	typedef std::map<int, sAssistItemUIControls>::iterator			MAP_AssistItems_IT;
	typedef std::map<int, sAssistItemUIControls>::const_iterator	MAP_AssistItems_CIT;

public:
	cItemProductionShop();
	virtual ~cItemProductionShop();

	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual eMU_TYPE		Update_ForMouse_Disable();
	virtual void			Render();

	void					ResetDevice();
	bool					OnEscapeKey();

private:
	void					CloseWindow( void* pSender, void* pData );
	void					SelectedCategoty(void* pSender, void* pData);
	void					SelectedProductionItem(void* pSender, void* pData);
	void					ChangeInputMakeCount(void* pSender, void* pData);

	void					OnDecrease(void* pSender, void* pData);
	void					OnDecreaseEnd(void* pSender, void* pData);
	void					OnIncrease(void* pSender, void* pData);
	void					OnIncreaseEnd(void* pSender, void* pData);

	DWORD					GetAssistItemCode( int const& nAssistType ) const;

	void					changeMakeCount( DWORD nCount );
	void					MakeCategoty();
	void					MakeProductionItemList( LIST_ProductionList const& productionItemList, int const& nMainIdx, int const& nSubIdx );
	void					MakeMaterialItemList();
	bool					IsMakeItemMoney(u8 makeCount);		// 아이템의 갯수만큼 필요한 금액이 있는지 체크
	DWORD					MakeItemTotalCount();				// 현재 생성 할수 있는 총 아이템의 갯수
	void					MakeSubCategoty( cTreeBoxItem & Root, MAP_Sub_Categoty const& subItem, int const& nMainIdx );
	void					SetSingleCost( u8 const& dwNeedCost );	// 단일 가격
	void					changeProductionCost( u8 const& dwNeedCost );
	void					ChangeHaveItemCount( ContentsStream const& kStream );
	void					_UpdateMaterialItemCount(DWORD const& dwMakeItemcount);

	void					SetProtectAssistItem( int const& nGroupID );
	void					SetPrecentageAssistItem( int const& nGroupID );
	void					UpdateSucessPrecentageText();
	void					UpdateAddPrecentageText();

	void					_ProgressbarEnd( void* pSender, void* pData );
	void					_SelectAssistItemSlot(void* pSender, void* pData);
	void					_ClearAssistItemSlot(void* pSender, void* pData);
	void					_ItemMaking(void* pSender, void* pData);
	void					_SendItemMaking(void* pSender, void* pData);

	void					_SetAssistItemSlotData();

	void					_SetAssistItemLock( bool bLock );
	void					_SetAssistItemLock( int const& nKey, bool bLock );
	void					_SetAssitItem( int const& nAssistType, DWORD const& nItemCode, DWORD const& nItemCount);
	void					_UpdateUIAssistItemCount( int const& nAssistType );
	
	void					_StartMakingProgress();
	void					_StopMakingProgress();
	void					_EndMakingProgress();

	NiColor					_GetPercentageFontColor( int const& nPercentage );

	void					_UILock( bool bLock );

	int						GetMaxCanMakeItemCount_Material() const;
	DWORD					GetMaxCanMakeItemCount_Money();
	DWORD					GetMaxCanMakeItemCount_AssistItem( int const& nAssistType ) const;

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	cButton*				m_pCloseButton;
	cButton*				m_pMakingBtn;
	cMoneyIF*				m_pProductionCost;
	cSprite*				m_MouseOverImg;
	cSprite*				m_SelectImg;

	cListBox*				m_pProductionItemList;
	cListBox*				m_pMaterialItemList;

	cTreeBox*				m_pTreeCategoty;
	cEditBox*				m_pEditBox;

	MAP_MaterialItems		m_mapMaterialCountText;

	cButton*				m_pBtnIncrease;
	cButton*				m_pBtnDecrease;
	cButton*				m_pBtnIncreaseEnd;
	cButton*				m_pBtnDecreaseEnd;

	cText*					m_pPercentageUpText;
	cText*					m_pProtectText;
	cText*					m_pSucessPrecentageText;
	cText*					m_pAddPrecentageText;
	cImage*					m_pSetProtectItemImg;

	cUI_TimerProgressBar*	m_pProgressBar;

	DWORD					m_nMakeCount;
	u8						m_dwNeedCost;

	sProductionType*		m_pSelectedItemInfo;

	sProductionItemList		const* m_pItemInfo;
	cGridListBox*			m_pAssisteItemList;
	MAP_AssistItems			m_mapAssistControl;
};


