
#pragma once 

class cHelpTextBtn : public cBaseWindow
{
public:
	cHelpTextBtn();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_HELPTEXTBTN; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pOpenHelpText;
	cSprite*		m_pMask;

	float			m_fAlpha;
	bool			m_bIncreaseAlpha;

	int				m_nHelpTextUniqID;
	int				m_nBodyLine;

public:
	void			SetHelpTextInfo( int nUniqID, int nBodyLine ){ m_nHelpTextUniqID = nUniqID; m_nBodyLine = nBodyLine; }
};

