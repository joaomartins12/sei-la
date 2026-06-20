
#pragma once 


class cCreateName : public cBaseWindow
{
public:
	cCreateName(){}
	virtual ~cCreateName(){ Destroy(); }

public:
	static cCreateName*	NewInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, CsPoint pos );
	static bool			UpdateInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static eMU_TYPE		Update_ForMouseInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static void			RenderInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static bool			DeleteInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 );
	static bool			DeleteInstance();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CREATE_NAME; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 ){}

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap() { Close( false ); }

 	virtual bool			OnMacroKey(const MSG& p_kMsg);
	virtual bool			OnEscapeKey(){ Close(); return true; }

	void					OnOk();
	void					OnCancel();

protected:
	bool					_CheckTamerName( TCHAR* szName );
	bool					_CheckDigimonName( TCHAR* szName, bool bCheckCurDigimon );

	bool					_CheckGuildName( TCHAR* szName );
	bool					_CheckInviteName( TCHAR* szName );

	// 메모리잡힌 컨트롤
protected:	

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
	bool						m_bValue2;

	enum eState { eTypeNone = 0, eTypeOk, eTypeCancel };
	eState						m_eType;

public:
	void			Init( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, CsPoint pos );

	cBaseWindow::eWINDOW_TYPE	GetTargetWindowType(){ return m_eTargetWindowType; }
	int							GetTargetWindowWTValue(){ return m_nTargetWTValue1; }

	void						SetValue1( int nValue ){ m_nValue1 = nValue; }
	void						SetValue2( bool nValue ){ m_bValue2 = nValue; }


	// protected:
	// 	bool			_IsEnableName();
};

extern cCreateName*		g_pCreateName;