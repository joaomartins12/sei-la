#pragma once 
#include "../00.Contents/NewCashshopContents.h"
class NewCashshopPurchaseSubViewer : public cScriptUI, public NewCashshopContents::ObserverType
{
public:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream){};		
	virtual	bool			Construct(void){return false;};
	virtual void			Destroy(){};
	virtual void			DeleteResource(){};
	virtual void			Update(float const& fDeltaTime){};
	virtual void			Create(cWindow* pkRoot, int nValue = 0 ){};
	virtual BOOL			UpdateMouse(){return FALSE;};
	virtual void			Render(){};
	virtual void			UpdateViewer(NewCashshopContents::PurchaseItems const& kItemInfo){};

protected:
  	virtual void			_MakeString( sCashshop_Item const* pCashItem, cString* pString );
};
