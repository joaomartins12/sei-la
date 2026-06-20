#pragma once

class cTamerStatusUI_SubViewer : public cScriptUI, public CMainFrameContents::ObserverType
{
public:
	cTamerStatusUI_SubViewer();
	virtual ~cTamerStatusUI_SubViewer();

public:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);	

	virtual void			Destroy();
	virtual void			DeleteResource(){}
	virtual void			Create(cWindow* pkRoot, int nValue = 0 ){}

	virtual void			Update(float const& fDeltaTime){}
	virtual BOOL			Update_ForMouse(){return TRUE;}
	virtual void			RenderScript(){}

	virtual void			OnMouseOutWindow(){}
	virtual void			ResetDevice(){}

public:
	virtual void			ReleaseSelect(){}
	virtual void			UpdateViewer(){}
	virtual void			OnLButtonUp( CsPoint pos ){}
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount ){return false;}
};

