#pragma once 
#include "../00.Contents/NewCashshopContents.h"

class NewCashshopStorageViewer : public cBaseWindow, public NewCashshopContents::ObserverType
{
	struct StorageItemInfo : public CUSTOMDATA
	{
		StorageItemInfo(int iid)
			:id(iid)
		{}
		int id;
	};

public:
	NewCashshopStorageViewer();
	~NewCashshopStorageViewer();

	virtual eWINDOW_TYPE	GetWindowType(){ return WT_NEW_CASH_WAREHOUSE; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create(int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();
	virtual void			ResetDevice();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		
	virtual	bool			Construct(void);

private:
	void					_UpdateStorageSlot( int const& nUpdateSlotNum );
	void					ShowCashStorage(void);
	void					OnSelected_Item(void* pSender, void* pData);
	void					OnClick_CloseWindow(void* pSender, void* pData);
	void					OnClick_RefreshCashStorage(void* pSender, void* pData);
	void					OnClick_CashshopOpen(void* pSender, void* pData);

private:
	cGridListBox*			m_pkItems;
	cButton*				m_pCancelButton;
	cButton*				m_pCashRefreshBtn;
	cButton*				m_pCashShopBtn;
};