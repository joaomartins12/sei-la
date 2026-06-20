#pragma once 
#include "../00.Contents/NewCashshopContents.h"
class ItemViewer : public cScriptUI, public NewCashshopContents::ObserverType
{
public:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream){};		
	virtual	bool			Construct(void){return false;};
	virtual void			Destroy(){};
	virtual void			DeleteResource(){};
	virtual void			Update(float const& fDeltaTime){};
	virtual void			Create(cWindow* pkRoot, int nValue = 0 ){};
	virtual BOOL			UpdateMouse(){return FALSE;};
	virtual void			UpdatePage(int const& nMainTabIdx, int const& nSubTabIdx, int const & nPage = 1, DWORD const& dwSelectGroupID = 0){};
};
