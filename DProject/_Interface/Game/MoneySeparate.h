
#pragma once 


class cMoneySeparate : public cBaseWindow
{
public:
	cMoneySeparate(){}
	virtual ~cMoneySeparate(){ Destroy(); }

public:
	static void			NewInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, sMONEY curMoney, sMONEY minMoney, sMONEY maxMoney );
	static bool			UpdateInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static eMU_TYPE		Update_ForMouseInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static void			RenderInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static bool			DeleteInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static bool			DeleteInstance();

	static bool			IsInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_MONEY_SEPARATE; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 ){}

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

	void					OnOk();
	void					OnCancel();

public:
	bool					OnChangeEdit( DWORD nCtrlID );

protected:
	void					_SetMoney( sMONEY money );
	sMONEY					_GetMoney();

	// 메모리잡힌 컨트롤
protected:

	// 포인터만 있는 컨트롤
protected:
	cButton*					m_pBtnOk;
	cButton*					m_pBtnCancel;

protected:
	cBaseWindow::eWINDOW_TYPE	m_eTargetWindowType;
	int							m_nTargetWTValue1;

	sMONEY			m_CurMoney;
	sMONEY			m_MinMoney;
	sMONEY			m_MaxMoney;

	HWND			m_hEditTera;
	HWND			m_hEditMega;
	HWND			m_hEditBit;

	HBRUSH			m_hEditBrush;
	HFONT			m_hEditFont;

public:
	void			Init( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, sMONEY curMoney, sMONEY minMoney, sMONEY maxMoney );

	cBaseWindow::eWINDOW_TYPE	GetTargetWindowType(){ return m_eTargetWindowType; }
	int							GetTargetWTValue1(){ return m_nTargetWTValue1; }

	LRESULT			ApplyEditColor( HWND hEdit, HDC dc );
};

extern cMoneySeparate*		g_pMoneySeparate;