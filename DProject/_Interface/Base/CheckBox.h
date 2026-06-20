
#pragma once 


class cCheckBox : public cSprite
{
public:
	cCheckBox(){ m_pBtnText = NULL; }
	virtual ~cCheckBox(){ Delete(); }

	enum CHECK_BUTTON_EVENT
	{
		BUTTON_LBDOWN_EVENT =0,
		BUTTON_LBUP_EVENT ,
		BUTTON_LBDLBCLICKED_EVENT,
		BUTTON_RBDOWN_EVENT ,
		BUTTON_RBUP_EVENT 
	};

	//=======================================================================================================================
	//
	//		Base
	//
	//=======================================================================================================================
public:
	enum eIMAGE_TYPE{ IMAGE_RADIO_CHAT1, IMAGE_RADIO_CHAT2, IMAGE_RADIO_CHAT3, IMAGE_RADIO_CHAT4, IMAGE_RADIO_CHAT5, IMAGE_RADIO_CHAT6, 
					  IMAGE_RADIO_1, IMAGE_RADIO_2, IMAGE_RADIO_3, IMAGE_RADIO_4, IMAGE_RADIO_5, IMAGE_RADIO_6,
					  IMAGE_RADIO_ENCYCLOPEDIA,IMAGE_RADIO_ENCY_DECK,
	};	

	virtual void	Delete();
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize );
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, eIMAGE_TYPE it, TCHAR const* cs, bool bApplyWindowSize );
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, eIMAGE_TYPE it, cText::sTEXTINFO* ti, bool bApplyWindowSize );

	virtual int		Update_ForMouse();	
	virtual void	Render();
	virtual eTYPE	GetType(){ return T_CheckBox; }

	virtual void	ResetDevice();

	virtual	void	OnMouseOver();
	virtual	void	OnMouseLeave();
	virtual	void	OnMouseDown();
	virtual	void	OnMouseUp();
	virtual void	OnMousePressed();

	void			SetCheckBoxText( cText::sTEXTINFO* ti );

	//=======================================================================================================================
	//
	//		state
	//
	//=======================================================================================================================
public:
	enum eSTATE{	
		NORMAL, MOUSE_ON, MOUSE_CLICK, CHECK_OK, DISABLE, MAX_STATE_COUNT, 
	};
	enum eACTION{
		ACTION_NONE, ACTION_ON, ACTION_DOWN, ACTION_PRESS, ACTION_CLICK,
	};

protected:
	bool		m_bClick;
	cText*		m_pBtnText;

	bool		bCursorIn;			//2016-02-17-nova	커서영역 여부

public:
	bool		IsCheck() const;
	void		SetCheck( bool bCheck );
	void		SetTexTokenState( eSTATE state );
	void		SetTexNState( CsPoint ptTex, float fStartPos, eSTATE eTokenState );
	void		SetTextColor( NiColor Color );

	//=======================================================================================================================
	//
	//		동작
	//
	//=======================================================================================================================
protected:



	//=======================================================================================================================
	//
	//		TexToken
	//
	//=======================================================================================================================
public:
	struct sTEX_TOKEN
	{
		sTEX_TOKEN(){ s_fLeft = s_fTop = 0.0f; s_fRight = s_fBottom = 1.0f; }
		float	s_fLeft;
		float	s_fTop;
		float	s_fRight;
		float	s_fBottom;
	};
protected:
	eSTATE			m_eTexTokenState;
	eSTATE			m_eOldTexTokenState;
	sTEX_TOKEN		m_TexToken[ MAX_STATE_COUNT ];
public:
	void			SetTexToken( CsPoint tex );
	void			SetEnable( bool bEnable ){ m_eTexTokenState = bEnable ? NORMAL : DISABLE; }
	bool			IsEnable(){ return m_eTexTokenState != DISABLE; }
};