#pragma once

class cAniButton : public cSprite
{
public:
	cAniButton(void);
	virtual			~cAniButton(void);

	enum eSTATE{
		NORMAL, MOUSE_ON, MOUSE_CLICK, MOUSE_PRESS, DISABLE,	
		MAX_STATE_COUNT,
	};

	enum eACTION{
		ACTION_NONE, ACTION_ON, ACTION_DOWN, ACTION_PRESS, ACTION_CLICK,
	};

	enum BUTTON_EVENT
	{
		BUTTON_LBDOWN_EVENT =0,
		BUTTON_LBUP_EVENT ,
		BUTTON_LBDLBCLICKED_EVENT,
		BUTTON_RBDOWN_EVENT ,
		BUTTON_RBUP_EVENT 
	};

	struct sTEX_TOKEN
	{
		sTEX_TOKEN(){ s_fLeft = s_fTop = 0.0f; s_fRight = s_fBottom = 1.0f; }
		float	s_fLeft;
		float	s_fTop;
		float	s_fRight;
		float	s_fBottom;
	};

	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize );
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, TCHAR const* bs, bool bApplyWindowSize, CsPoint ptDeltaPos = CsPoint::ZERO );
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, cText::sTEXTINFO* ti, bool bApplyWindowSize, CsPoint ptDeltaPos = CsPoint::ZERO );

	virtual void	Delete();
	virtual void	SetPos( CsPoint ptPos );
	int				Update_ForMouse();
	virtual void	Update(float fTime);
	virtual void	Render();			
	virtual void	ResetDevice();

	virtual eTYPE	GetType();

	virtual	void	OnMouseOver();
	virtual	void	OnMouseLeave();
	virtual	void	OnMouseDown();
	virtual	void	OnMouseUp();
	virtual void	OnMousePressed();

protected:
	bool			m_bClick;
	eSTATE			m_eTexTokenState;
	eSTATE			m_eOldTexTokenState;
	eSTATE			m_AniState;

protected:		// 애니메이션 변수
	bool			m_bLoop;			// 반복 할지 여부
	float			m_fSetAniTime;
	float			m_fCurAniTime;

	int				m_nAniIndex;		// 현재 애니 인덱스
	int				m_nAniStartIndex;	// 애니 시작 인덱스
	int				m_nAniEndIndex;		// 애니 마지막 인덱스
	int				m_nTotalIndex;		// 전체 이미지 갯수

	sTEX_TOKEN		m_TexToken;			

public:
	void			SetAniLoop(bool	Loop);
	void			SetAniState(eSTATE);
	void			SetAniTime( float fTime );
	void			SetAniStartEnd(int StartId, int EndID);
	void			SetTexToken( CsPoint tex );
	void			ResetTokenState();
};
