
#pragma once 

class cTamerSummon : public cBaseWindow
{
public:
	cTamerSummon(){}
	virtual ~cTamerSummon(){ Destroy(); }

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_TAMER_SUMMON; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap() { Close( false ); }

	virtual bool			OnEscapeKey(){ Close(); return true; }

	void					OnOk();
	void					OnCancel();

protected:
	bool					_CheckTamerName( TCHAR* szName );

	// 포인터만 있는 컨트롤
protected:
	cButton*				m_pBtnOk;
	cButton*				m_pBtnCancel;

	cEditBox*				m_pEditName;
	int						m_nMaxLengthByte; //바이트수
	int						m_nMaxLength;

protected:
	cBaseWindow::eWINDOW_TYPE	m_eTargetWindowType;
	int							m_nTargetWTValue1;
	int							m_nValue1;

	enum eState { eTypeNone = 0, eTypeOk, eTypeCancel };
	eState						m_eType;

public:
	void			Init( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, CsPoint pos );

	cBaseWindow::eWINDOW_TYPE	GetTargetWindowType(){ return m_eTargetWindowType; }
	int							GetTargetWindowWTValue(){ return m_nTargetWTValue1; }

	void						SetValue1( int nValue ){ m_nValue1 = nValue; }

//protected:
	//bool			_IsEnableName();
};

extern cTamerSummon*		g_pTamerSummon;

