#pragma once

class DigimonArchiveSubViewer : public cScriptUI
{
public:
	virtual void			Destroy(){}
	virtual void			DeleteResource(){}
	virtual void			Create(cWindow* pkRoot, int nValue = 0 ){}

	virtual void			Update(float const& fDeltaTime){}
	virtual BOOL			UpdateMouse(){return TRUE;}
	virtual void			Render(){}

	virtual void			OnMouseOutWindow(){}
	virtual void			ResetDevice(){}
	
public:
	virtual void			ReleaseSelect(){}
	virtual void			ReleaseSelect(CDigimonArchiveContents::SELECT_PART ePart){}

	virtual void			OnChangeSelectedDigimon(const cData_PostLoad::sDATA* pkDATA, int SelectedPart ){};
	virtual void			UpdateViewer(void){};

public:
	DigimonArchiveSubViewer();
};
