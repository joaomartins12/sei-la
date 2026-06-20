
#pragma once 

#define IF_PCBANGINVENTORY_MAX_COUNT	7

class cPcbangInven : public cBaseWindow
{
public:
	cPcbangInven(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_PCBANGINVEN; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual bool			Close( bool bSound = true );	

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			OnMouseOutWindow();
	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );	

	virtual void			ResetDevice();
	virtual bool			OnEscapeKey(){ Close(); return true; }

protected:		
	sIFIcon			m_IFIcon[ IF_PCBANGINVENTORY_MAX_COUNT ];

public:	
	sIFIcon			GetIFIcon( int nInven ){ return m_IFIcon[ nInven ]; }	

protected:
	bool			_UpdateIcon_ForMouse();	

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pCancelButton;	
};


