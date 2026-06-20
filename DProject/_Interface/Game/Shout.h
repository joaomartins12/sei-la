
#pragma once 

class cShout : public cBaseWindow
{
public:
	cShout(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_SHOUT_CASH; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual bool			Close( bool bSound = true );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			OnMouseOutWindow();

	virtual void			ResetDevice();
	virtual bool			OnMacroKey(const MSG& p_kMsg);
	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual bool			OnEnterKey();	
	virtual void			PreResetMap() { Close(false); }

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pCancelButton;	
	cButton*		m_pSendButton;
	cEditBox*		m_pEdit;

	DWORD			m_dwItemID;
	int				m_nShoutItemSlot;
};


