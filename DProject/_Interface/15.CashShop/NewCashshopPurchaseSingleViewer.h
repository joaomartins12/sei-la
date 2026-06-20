#pragma once 
#include "../00.Contents/NewCashshopContents.h"
class NewCashshopPurchaseSingleViewer : public NewCashshopPurchaseSubViewer
{
public:
	NewCashshopPurchaseSingleViewer();
	~NewCashshopPurchaseSingleViewer();

public:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		
	virtual	bool			Construct(void);
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Update(float const& fDeltaTime);
	virtual void			Create(cWindow* pkRoot, int nValue = 0 );
	virtual BOOL			UpdateMouse();
	virtual void			UpdateViewer(NewCashshopContents::PurchaseItems const& kItemInfo);

protected:
	//virtual void			UpdateCashBalance(CsCashShop* pkCashshopItemInfo);

private:
	void					OnClick_ItemTypeCB(void* pSender, void* pData);

private:
	cComboBox*				m_pkItemTypeCB; 
	cGridListBox*			m_pkGB_Item;
	cListBox*				m_plist_Desc;
};