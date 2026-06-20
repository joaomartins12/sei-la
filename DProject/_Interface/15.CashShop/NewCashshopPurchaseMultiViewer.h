#pragma once 
#include "../00.Contents/NewCashshopContents.h"
class NewCashshopPurchaseMultiViewer : public NewCashshopPurchaseSubViewer
{
public:
	NewCashshopPurchaseMultiViewer();
	~NewCashshopPurchaseMultiViewer();

public:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		
	virtual	bool			Construct(void);
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Update(float const& fDeltaTime);
	virtual void			Create(cWindow* pkRoot, int nValue = 0 );
	virtual BOOL			UpdateMouse();
	virtual void			UpdateViewer(NewCashshopContents::PurchaseItems const& kItemInfo);


private:
	cGridListBox*			m_pkGB_Item;
};