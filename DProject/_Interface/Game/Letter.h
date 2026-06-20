#pragma once

class cLetter : public cBaseWindow
{
public:
	cLetter(){ m_pScrollBar = NULL; m_pEditBox = NULL; }

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_LETTER; }
	virtual void			Destroy();
	virtual void			DeleteResource();	
	virtual void			ResetMap() { Close( false ); }

	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();		
	virtual	void			OnMoveWindow();

protected:	
	cButton*				m_pOkButton;
	cStringList				m_strMessage;
	cEditBox*				m_pEditBox;

	cScrollBar*				m_pScrollBar;		//½ºÅ©·Ñ

	TCHAR					m_szMsgTemp[ 512 ];
};