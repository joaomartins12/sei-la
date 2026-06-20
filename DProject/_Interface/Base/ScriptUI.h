
#pragma once 

class cScriptUI : public NiMemObject
{
public:
	cScriptUI();
	virtual ~cScriptUI();

	//======================================================================================
	//
	//	Base
	//
	//======================================================================================
public:
	void			InitScript( char* cRoot, CsPoint pos, CsPoint size, bool bMoveWindow, IFREGION_X::eTYPE xType = IFREGION_X::LEFT, IFREGION_Y::eTYPE yType = IFREGION_Y::TOP, bool bApplyWindowSize = true );
	void			InitScript( cWindow* pParent, char* cRoot, CsPoint pos, CsPoint size, bool bMoveWindow, IFREGION_X::eTYPE xType = IFREGION_X::LEFT, IFREGION_Y::eTYPE yType = IFREGION_Y::TOP, bool bApplyWindowSize = true );
	void			SetRegionType( IFREGION_X::eTYPE xType , IFREGION_Y::eTYPE yType );

	virtual void	DeleteScript();
	virtual void	OnMouseOutWindow();

	virtual void	UpdateScript(float fDeltaTime);
	virtual void	RenderScript();
	virtual void	ResetDeviceScript();	

	void			EndRenderScript(){ if( __m_bEnableWindow == false ){ __m_pDisableMask->Render(); } }

	void			SetVisible( bool bValue );
	bool			IsVisible() const;

	void			SetRootScript( cScriptUI* pRoot );

protected:	
	void			_CalRootDelte( bool bInit );	
	void			_OnMoveWindowScript();
	void			_OnSetFocusScript();
	void			_OnKillFocusScript();

public:
	bool			SetMove( CsPoint& ptDelta );

public:
	void			SetPosSize( CsPoint pos, CsPoint size, bool bApplyWindowSize = true );
	void			SetPos( CsPoint pos );

	//=====================================================================================================
	//
	//	디스에이블
	//
	//=====================================================================================================
protected:
	cSprite*		__m_pDisableMask;
	bool			__m_bEnableWindow;

public:
	bool			IsEnableWindow(){ return __m_bEnableWindow; }
	void			SetEnableWindow( bool bEnable );
	CsPoint			GetRootSize() const;
	//======================================================================================
	//
	//	Status
	//
	//======================================================================================
protected:
	bool				m_bLimitScreen;
	bool				m_bMoveWindow;
	bool				m_bInitial;

	IFREGION_X::eTYPE	m_eXRegionType;
	IFREGION_Y::eTYPE	m_eYRegionType;

	CsPoint				m_ptRootDelta;
	CsPoint				m_ptRootClient;
	CsPoint				m_ptRootSize;

public:
	bool				IsMoveWindow(){ return m_bMoveWindow; }
	virtual bool		IsInWindow( CsPoint pos );
	void				SetRootClient( CsPoint pos );

	// 특별한 경우 아니면 건드리지 말자 IFREGION_X::CENTER, IFREGION_Y::CENTER 가 아닐경우 오작동 위험 있음
	void				__SetRootSize( CsPoint size );
	CsPoint				GetRootClient();
	CsPoint				GetTotalPosition();
	cWindow*			GetRoot();

	void				SetLimitScreen( bool bLimit );

	//======================================================================================
	//
	//	Control
	//
	//======================================================================================
protected:
	cWindow*					m_pRoot;
	cText*						m_pWindowTitle;
	cScriptUI*					m_pRootScript;

	std::list<cWindow*>			m_vpChildControls;
	std::list<cScriptUI*>		m_vpChildScript;
	std::list<cText*>			m_vpText;
	
	bool						m_bVisible;

public:
	void			AddChildScriptUI( cScriptUI* pAddUI );

	cText*			AddTitle( TCHAR const* szTitle, CsPoint ptDeltaPos = CsPoint::ZERO, CFont::eFACE_SIZE eFontSize = CFont::FS_12, NiColor fontColor = FONT_WHITE );

	cButton*		AddButton( CsPoint pos, CsPoint size, CsPoint tex, char const* cPath, cWindow::eSOUND sound = cWindow::SD_NONE, bool bUseParent = true );
	cButton*		AddButton( CsPoint pos, CsPoint size, cButton::eIMAGE_TYPE it, TCHAR const* bs, cWindow::eSOUND sound = cWindow::SD_NONE, bool bUseParent = true, CsPoint ptDeltaPos = CsPoint::ZERO );
	cButton*		AddButton( CsPoint pos, CsPoint size, cButton::eIMAGE_TYPE it, cText::sTEXTINFO* ti, cWindow::eSOUND sound = cWindow::SD_NONE, bool bUseParent = true, CsPoint ptDeltaPos = CsPoint::ZERO );

	cAniButton*		AddAniButton( CsPoint pos, CsPoint size, CsPoint tex, char const* cPath, cWindow::eSOUND sound = cWindow::SD_NONE, bool bUseParent = true );
	cAniButton*		AddAniButton( CsPoint pos, CsPoint size, TCHAR const* bs, cWindow::eSOUND sound = cWindow::SD_NONE, bool bUseParent = true, CsPoint ptDeltaPos = CsPoint::ZERO );
	cAniButton*		AddAniButton( CsPoint pos, CsPoint size, cText::sTEXTINFO* ti, cWindow::eSOUND sound = cWindow::SD_NONE, bool bUseParent = true, CsPoint ptDeltaPos = CsPoint::ZERO );


	cSprite*		AddSprite( CsPoint pos, CsPoint size, char const* cPath, bool bUseParent = true );
	cSprite*		AddSprite( CsPoint pos, CsPoint size, char const* cImagePath, bool bUseParent, NiColor color, bool bUseWorkingFolder = true );
					
	cCheckBox*		AddCheckBox( CsPoint pos, CsPoint size, CsPoint tex, char const* cPath, cWindow::eSOUND sound = cWindow::SD_NONE, bool bUseParent = true );
	cCheckBox*		AddCheckBox( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, TCHAR const* cs, cWindow::eSOUND sound = cWindow::SD_NONE, bool bUseParent = true );
	cCheckBox*		AddCheckBox( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, cText::sTEXTINFO* ti, cWindow::eSOUND sound = cWindow::SD_NONE, bool bUseParent = true );

	cText*			AddText( cText::sTEXTINFO* pTextInfo, CsPoint pos, bool bUseParent = true );	

	cScrollBar*		AddScrollBar( cScrollBar::eSCROLL_TYPE type, CsPoint pos, CsPoint CtrlSize, CsPoint btnSize, CsRect rcRegion, int nRenderCount, bool bUseParent = true );

	cRadioButton*	AddRadioButton( CsPoint pos, CsPoint size, CsPoint tex, char* cImagePath, bool bUseParent = true);
	cRadioButton*	AddRadioButton( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, cText::sTEXTINFO* ti, bool bUseParent = true);
	cRadioButton*	AddRadioButton( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, TCHAR const* cs, bool bUseParent = true );

	cPopUp*			AddPopup( CsPoint pos, CsPoint size, CsPoint patch, char* cPathName, char* cExpName, bool bUseParent = true);

	bool			AddChildControl( cWindow* pAddControl );
	bool			RemoveChildControl( cWindow* pRemoveControl );
	bool			RemoveChildText( cText* pRemoveText );

	//======================================================================================
	//
	//	ResetDevice
	//
	//======================================================================================
protected:
	CsPoint			m_ptOldScreenSize;
public:
	static void ResetDevice( IFREGION_X::eTYPE ifx, IFREGION_Y::eTYPE ify, CsPoint& pos, CsPoint size, CsPoint& delta, bool bMoveWindow, bool bLimitScreen );
	static void	ApplyCurrentWindowSize( IFREGION_X::eTYPE ifx, IFREGION_Y::eTYPE ify, CsPoint& pos, CsPoint size, CsPoint& delta, bool bMoveWindow, bool bLimitScreen );
	CsPoint			GetOldScreenSize() const{return m_ptOldScreenSize;}


	//======================================================================================
	//
	//	Mouse
	//
	//======================================================================================
public:
	CsPoint			MousePosToWindowPos( CsPoint ptMousePos );
	void			SetWindowPosition( CsPoint ptPos );

	virtual cWindow* GetChildControl( std::string const& sControlName );
};
