
#pragma once 

#include "SpriteAni.h"

class cButton : public cSprite
{
private:
	static cText::sTEXTINFO		__g_TextInfo;
public:
	static CsPoint				DefSize();
	static cText::sTEXTINFO*	DefTextInfo( TCHAR const* sz );
	static void					GlobalInit();

public:
	enum eSTATE{
		NORMAL, MOUSE_ON, MOUSE_CLICK, DISABLE,
#ifdef MINIGAME

		SUCCESS, FAIL, READY,THREE, FOUR, FIVE, SIX, SEVEN,
#endif
		MAX_STATE_COUNT,
	};
	enum eACTION{
		ACTION_NONE, ACTION_ON, ACTION_DOWN, ACTION_PRESS, ACTION_CLICK,
	};


public:
	cButton();
	virtual ~cButton();

	enum BUTTON_EVENT
	{
		BUTTON_LBDOWN_EVENT =0,
		BUTTON_LBUP_EVENT ,
		BUTTON_LBDLBCLICKED_EVENT,
		BUTTON_RBDOWN_EVENT ,
		BUTTON_RBUP_EVENT,
		BUTTON_EFFECT_PLAY_END,
	};

	//=======================================================================================================================
	//
	//		Base
	//
	//=======================================================================================================================
public:
	virtual void	Delete();
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize );

	enum eIMAGE_TYPE{ IMAGE_NOR_1, IMAGE_NOR_2, IMAGE_NOR_3, IMAGE_NOR_4, IMAGE_NOR_5, IMAGE_NOR_6, IMAGE_NOR_7, IMAGE_NOR_8, 
		IMAGE_NOR_9, IMAGE_NOR_10, IMAGE_NOR_11, IMAGE_NOR_12, IMAGE_NOR_13, IMAGE_NOR_13_R, IMAGE_NOR_14, IMAGE_NOR_15,
		IMAGE_NOR_16,IMAGE_NOR_17,
		IMAGE_BACK, IMAGE_FORWARD, IMAGE_OK, IMAGE_CANCEL,
		IMAGE_ALPHA_1, IMAGE_ALPHA_2,
#ifdef MINIGAME
		IMAGE_MINI_CANCEL, IMAGE_MINI_STOP, 
#endif//MINIGAME 버튼 (나가기), (멈추기) 추가 chu8820
#ifdef SIMPLEFICATION
		IMAGE_SIMPLE_OK, IMAGE_SIMPLE_CANCEL,
#endif
		IMAGE_GOTCHA_GET,
#ifdef CROSSWARS_SYSTEM
		IMAGE_CROSSBUTTON,
#endif
		IMAGE_ENCYCLOPEDIA_GET,	// 계열체 오픈 보상 획득 버튼
		IMAGE_PINOKIMON_EVENT_CHOICE,
		IMAGE_PINOKIMON_EVENT_VOTE,
	};

	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, eIMAGE_TYPE it, TCHAR const* bs, bool bApplyWindowSize, CsPoint ptDeltaPos = CsPoint::ZERO );
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, eIMAGE_TYPE it, cText::sTEXTINFO* ti, bool bApplyWindowSize, CsPoint ptDeltaPos = CsPoint::ZERO );

	virtual void	SetPos( CsPoint ptPos );
	void			SetPosText( CsPoint ptPos );
	void			SetTextColor( NiColor Color );
	void			SetText( TCHAR const* szText );
	void			SetText( cText::sTEXTINFO* ti );
	cText*			const GetText()const {return m_pBtnText;}

	virtual int		Update_ForMouse();
	virtual void	Update(float const& fDeltaTime);
	virtual void	Render();			
	virtual void	ResetDevice();

	virtual eTYPE	GetType(){ return T_Button; }

	virtual	void	OnMouseOver();
	virtual	void	OnMouseLeave();
	virtual	void	OnMouseDown();
	virtual	void	OnMouseUp();
	virtual void	OnMousePressed();

	//=======================================================================================================================
	//
	//		effect
	//
	//=======================================================================================================================
private:
	cSpriteAni*		m_pButtonEffectAni;

	DWORD			m_dwEffectOn;
public:
	void		SetEffectOn(eSTATE eStateData, bool bFirstEffectSetting/*처음 세팅시 무조건 true*/);
	

	void		SetButtonEffect( cSpriteAni::ePLAY_TYPE eType, CsPoint pos, CsPoint size, char* cImagePath, char* cExt, int nAniCount, bool bApplyWindowSize, CsPoint textoken = CsPoint::ZERO, cSpriteAni::eSPRITE_PLAY eSprite = cSpriteAni::SPRITE_PAGE, int nEnterCount = 0/*스프라이트가 아래로 내려가게하는 조건 수 */);
	void		SetEffectAniTime(float fTime);
	void		StopEffect(bool bVisible = false);
	void		StartEffect(bool bResetAni);
	bool		IsEffectPlay() const;

	//=======================================================================================================================
	//
	//		state
	//
	//=======================================================================================================================
protected:
	cText*		m_pBtnText;
	bool		m_bClick;
	bool		m_bMouseOn;
	eIMAGE_TYPE m_eImeageType;

	bool		bKeyboardInput;		//2016-01-14-nova	키보드 입력
	bool		bCursorIn;			//2016-02-17-nova	커서영역 여부

	//=======================================================================================================================
	//
	//		동작
	//
	//=======================================================================================================================
public:
	void		ActiveLBtnDown();
	void		ACtiveLBtnDownPress();
	bool		ActiveLBtnUp();
	void		SetEnable( bool bEnable );
	bool		IsEnable();
	void		setTexTokenState(eSTATE type);
	void		SetMouseOnMode(bool	bMode);
	bool		IsClicked();

	void		KeyboardBtnDn();	//2016-01-13-nova	키보드 버튼 체크
	void		KeyboardBtnUp();	//2016-01-13-nova	키보드 버튼 체크

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

private:
	void			_ButtonEffectPlayEnd(void* pSender, void* pData);
};