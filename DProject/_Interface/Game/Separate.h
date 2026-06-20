
#pragma once 


class cSeparate : public cBaseWindow
{
public:
	cSeparate(){}
	virtual ~cSeparate(){ Destroy(); }

public:
	static void			NewInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, CsPoint pos, int nMinCount, int nMaxCount, sIFIcon* pIFIcon, int nIconValue1 );
	static bool			UpdateInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static eMU_TYPE		Update_ForMouseInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static void			RenderInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static bool			DeleteInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static bool			DeleteInstance();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_SEPARATE; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 ){}

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

	void					OnOk();
	void					OnCancel();

protected:
	void					_UpdateKeboard();
	


	// 메모리잡힌 컨트롤
protected:	

	// 포인터만 있는 컨트롤
protected:
	cButton*				m_pBtnOk;
	cButton*				m_pBtnCancel;
	cButton*				m_pBtnIncrease;
	cButton*				m_pBtnDecrease;
	cButton*				m_pBtnIncreaseEnd;
	cButton*				m_pBtnDecreaseEnd;
	cEditBox*				m_pEditCount;

protected:
	cBaseWindow::eWINDOW_TYPE	m_eTargetWindowType;
	int							m_nTargetWTValue1;
	int							m_nMinCount;
	int							m_nMaxCount;
	int							m_nCurCount;
	sIFIcon*					m_pIFIcon;
	int							m_nIconValue1;
	CsTimeSeq					m_KeyTimeSeq;
	bool						m_bFirstIMEInput;

public:
	void			Init( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, CsPoint pos, int nMinCount, int nMaxCount, sIFIcon* pIFIcon, int nIconValue1 );

	cBaseWindow::eWINDOW_TYPE	GetTargetWindowType(){ return m_eTargetWindowType; }
	int							GetTargetWTValue1(){ return m_nTargetWTValue1; }
};

extern cSeparate*		g_pSeparate;