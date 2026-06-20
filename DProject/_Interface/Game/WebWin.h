
#pragma once

class cWebWin : public cBaseWindow
{
public:
	cWebWin(){}

	enum eWEBWIN { eRANK = 0, eCASH };

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_WEBWIN; }
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();
	virtual void			ResetMap();

	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();	

	virtual bool			OnEscapeKey(){ Close(); return true; }

	void					SetWebControl( eWEBWIN eType, CsPoint ptPos, CsPoint ptSize, void* pValue = NULL ); 
	eWEBWIN					GetType() { return m_eType; }

protected:
	HWND					m_hWndwebRank;	
	cPopUp*					m_pBGWindow;	
	eWEBWIN					m_eType;

	// 포인터만 있는 컨트롤
	cButton*				m_pCancelButton;	

};

