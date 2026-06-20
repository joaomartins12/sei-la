#pragma once

class cRankView : public cBaseWindow
{
public:
	cRankView(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_GUILDRANK_VIEW; }
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();
	virtual void			ResetMap();

	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();		
	virtual bool			OnEscapeKey();

private:
	void					CloseWindow();

protected:
	cButton*				m_pCancelButton;
	cStringList				m_strExplain;

};