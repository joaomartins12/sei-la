

#pragma once

class cMessageBox : public cBaseWindow
{
public:
	enum eSTATE{
		MBOX_NONE, MBOX_CALL_OK, MBOX_CALL_CANCEL,
	};

public:
	cMessageBox( TCHAR const* szTitle, TCHAR const* szBody, int nMsgType, int nBoxType, int nMessageListIndex );
	virtual ~cMessageBox(){ Destroy(); }

public:
	static std::list< cMessageBox* >	g_MessageBoxList;
public:
	static void		DeleteList( bool bCalEndMessage );
	static bool		DelMsg( int nMsgType, bool bCallEndMsg );
	static void		InsertList( TCHAR const* szTitle, TCHAR const* szBody, int nMsgType, int nBoxType );
	static bool		UpdateList();
	static void		RenderList();

	static bool		OnEnterKeyList();
	static bool		OnEscapeKeyList();
	static bool		OnLButtonUpList( CsPoint pos );
	static bool		OnRButtonUpList( CsPoint pos );

	static void		ResetDeviceList();

	static bool			IsMessageBox(){ return ( g_MessageBoxList.empty() == false ); }
	static bool			IsMessageBox( int nMsgType );
	static cMessageBox*	GetFirstMessageBox();
	static cMessageBox*	GetMessageBox( int nMsgType );
	static void			MessageBoxEraseList( cMessageBox* pBox );
	static int			GetFristType(){ return IsMessageBox() == false ? 0 : GetFirstMessageBox()->GetMessageType(); }
	static int			GetCount(){ return (int)g_MessageBoxList.size(); }

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_MESSAGE_BOX; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 ){}

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	
	virtual void			ResetDevice();

public:
	void			Init( TCHAR const* szTitle, TCHAR const* szBody, int nBoxType, int nMessageListIndex );

	void			SetValue1( int nValue1 ){ m_nValue1 = nValue1; }
	void			SetValue2( int nValue2 ){ m_nValue2 = nValue2; }
	void			SetValue3( int nValue3 ){ m_nValue3 = nValue3; }
	void			SetValuePointer1( void* pValue ){ m_pValuePointer1 = pValue; }	
	void			SetUintValue1( UINT unValue1 ) { m_unValue1 = unValue1; }
	void			SetCharValue1( LPCTSTR szValue ){ _tcscpy_s( m_szValue1, MAX_PATH, szValue ); }

	eSTATE			GetState(){ return m_eState; }
	UINT			GetMessageType(){ return m_eMsgType; }	

	void			_Cancel( bool bAbsolute );
	void			_Ok( bool bAbsolute );
	void			_Default();

	bool			ExistOkBtnControl(){ return ( m_ButtonOk != NULL ); }
	cButton*		GetOkBtnControl(){ return m_ButtonOk; }	
	bool			ExistCancelBtnControl(){ return ( m_ButtonCancel != NULL ); }
	cButton*		GetCancelBtnControl(){ return m_ButtonCancel; }

	void			SetBtnControl( int nBtnType );

protected:
	bool			m_bCallProc;

	eSTATE			m_eState;
	UINT			m_eMsgType;
	int				m_eTextAlign;

	cStringList*	m_pBody;

	cButton*		m_ButtonOk;
	cButton*		m_ButtonCancel;
	cProgressBar*	m_pProgress;

	float			m_fValue;
	int				m_nValue1;
	int				m_nValue2;
	int				m_nValue3;
	void*			m_pValuePointer1;	
	UINT			m_unValue1;	
	TCHAR			m_szValue1[ MAX_PATH ];

};

