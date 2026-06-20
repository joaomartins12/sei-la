
#pragma once 




class cButton : public cSprite
{
public:
	cButton(){}
	virtual ~cButton(){}

	//=======================================================================================================================
	//
	//		Base
	//
	//=======================================================================================================================
public:
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, char* cImagePath, bool bApplyWindowSize );
	virtual int		Update_ForMouse();
	virtual void	Render();

	//=======================================================================================================================
	//
	//		state
	//
	//=======================================================================================================================
public:
	enum eSTATE{	
		NORMAL, MOUSE_ON, MOUSE_CLICK, DISABLE, MAX_STATE_COUNT,
	};
	enum eACTION{
		ACTION_NONE, ACTION_ON, ACTION_DOWN, ACTION_PRESS, ACTION_CLICK,
	};

protected:
	bool		m_bClick;

	//=======================================================================================================================
	//
	//		µ¿ÀÛ
	//
	//=======================================================================================================================
public:
	void		ActiveLBtnDown(){ m_eTexTokenState = MOUSE_CLICK; m_bClick = true; }
	void		ACtiveLBtnDownPress(){ m_eTexTokenState = MOUSE_CLICK; }
	bool		ActiveLBtnUp(){ if( m_bClick == true ){	m_eTexTokenState = MOUSE_CLICK;	} return m_bClick; }
	void		SetEnable( bool bEnable ){ m_eTexTokenState = bEnable ? NORMAL : DISABLE; }
	bool		IsEnable(){ return ( m_eTexTokenState != DISABLE ); }


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
};