#pragma once
#include "../00.Contents/EquipSystemContents.h"

class cEquipSystemSubViewer : public cScriptUI, public cEquipSystemContents::ObserverType
{
public:
	cEquipSystemSubViewer();
	virtual ~cEquipSystemSubViewer();

public:
	virtual	bool			Construct(void){return true;}
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream){}

	virtual void			Destroy(){}
	virtual void			DeleteResource(){}
	virtual void			Create(cWindow* pkRoot, int nValue = 0 ){}

	virtual void			Update(float const& fDeltaTime){}
	virtual BOOL			Update_ForMouse(){return TRUE;}
	virtual void			RenderScript(){}
	virtual void			ResetDevice(){}

public:
	virtual void			OnLButtonUp( CsPoint pos ){}
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount ){return false;}
	virtual void			UpdateViewer() {}
};