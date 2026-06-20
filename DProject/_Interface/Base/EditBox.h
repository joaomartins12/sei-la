
#pragma once

class cEditBox;
extern cEditBox*		g_pFocusEdit;

class cEditBox : public cWindow
{
public:
	enum EDITBOX_EVENT
	{
		eEditbox_ChangeText,
		eEditbox_SetFocus,
		eEditbox_ReleaseFocus,
	};

	cEditBox(){
		m_pTextUnderline = NULL;
		m_nFontLength = MAX_PATH;
		m_bEnableSound = false;		
		m_bEnableLingChange = false;
		m_bEnableWantReturn = false;
		m_bEnablePaste = false;
		m_nLimitNumber = 0; // [4/7/2016 hyun] 추가
		m_bEnable = true;
	}
	virtual ~cEditBox(){
		ReleaseFocus();
		Delete();
		SAFE_NIDELETE( m_pTextUnderline );
	}

	//=======================================================================================
	//
	//		전역
	//
	//=======================================================================================
public:
	static void		UpdateFocusEdit();
	static void		OnPaste();

	//=======================================================================================
	//
	//		베이스
	//
	//=======================================================================================
protected:	
	bool		m_bEnable;
	cText		m_Text;
	cText		m_EmptyText;
	int			m_nFontLength;
	int			m_nTempInputIndex;

	float		m_fCursorTime;
	cText*		m_pTextUnderline;

	std::wstring	m_strText;

	bool		m_bEnableSound;	
	bool		m_bEnableLingChange;
	bool		m_bEnableWantReturn;
	bool		m_bEnablePaste;

	int			m_nLimitNumber; // [4/7/2016 hyun] 숫자 제한 추가

public:
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, cText::sTEXTINFO* pTextInfo, bool bApplyWindowSize );
	virtual void	Delete();
	virtual void	SetAlpha( float fAlpha );
	virtual int		Update_ForMouse( CsPoint ptParent );	
	virtual void    Update(float const& fDeltaTime) {}
	virtual void	Render();
	virtual void	Render( CsPoint pos );
	void			RenderUnderLine( CsPoint pos );
	virtual void	SetVisible( bool bValue );
	virtual	void	SetEnable( bool bValue );

	virtual void	ResetDevice();
	virtual eTYPE	GetType(){ return T_EditBox; }

	//=======================================================================================
	//
	//		Get
	//
	//=======================================================================================
public:
	cText*				GetText(){ return &m_Text; }
	const TCHAR*		GetTextAll() { return m_strText.data(); }
	bool				CalTempInputIndex();
	int					GetTempInputIndex(){ return m_nTempInputIndex; }
	int					GetFontLength(){ return m_nFontLength; }
	bool				IsEnableSound(){ return m_bEnableSound; }
	TCHAR const*		GetString();

	//=======================================================================================
	//
	//		동작
	//
	//=======================================================================================
public:
	enum eACTION{
		ACTION_NONE, ACTION_CLICK,
	};
public:
	bool			SetText( TCHAR const* szText, bool bCallFocus=false );
	bool			SetText( int nText );
	void			SetEmptyMsgText( TCHAR const* szText, NiColor niColor );
	void			SetTextAll( TCHAR const* szText );
	void			AddText( TCHAR const* szText );

	void			SetFocus();
	void			SetFontLength( int nLength ){ m_nFontLength = nLength != 0 ? nLength : MAX_PATH; }
	void			EnableUnderline( NiColor color = FONT_WHITE );
	void			EnablePaste(){ m_bEnablePaste = true; }
	bool			IsPaste() { return m_bEnablePaste; }
	
	void			ReleaseFocus( bool bClearText = true );	
	bool			IsFocus(){ return ( g_pFocusEdit == this ); }

	void			SetEnableSound( bool bEnable ){ m_bEnableSound = bEnable; }

	void			SetWantReturn( bool bReturn ){ m_bEnableWantReturn = bReturn; }
	bool			IsWantReturn(){ return m_bEnableWantReturn; }

	// [4/7/2016 hyun] 추가
	void			SetLimitNumber(int Num);

	void			SetFontColor(NiColor color = FONT_WHITE);
};

