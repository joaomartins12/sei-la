

#pragma once 

class cSlider : public cWindow
{
public:
	cSlider();
	~cSlider(){ Delete(); }

	// 그리는데 필요한거
protected:
	cButton*		m_pLeftBtn;
	cButton*		m_pRightBtn;
	cSprite*		m_pBar;

public:
	enum eACTION{
		ACTION_NONE, ACTION_CLICK, ACTION_ON,
	};

protected:
	// 바 클릭 타입 - 논, 바만
	enum eCLICK_BAR_TYPE{
		CB_NONE, CB_BAR,
	};
	eCLICK_BAR_TYPE	m_eBarClickType;

	// 계산
protected:
	CsPoint			m_ptBackupLocalMousePos;
	CsPoint			m_ptBarRegion;
	CsPoint			m_ptRange;
	int				m_nMaxPosIndex;				//		움직일수 잇는 칸~
	int				m_nCurPosIndex;				//		현재 존재 한는칸
	float			m_fPos_Width;				//		포스 한칸당의 넓이

	int				m_nBtnMoveRange;
	CsPoint			m_ptBarPos;
	int				m_nLastState;

	bool			m_bEnable;

public:
	void			Delete();

	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize );

	virtual void	Update(float const& fDeltaTime) {}
	virtual int		Update_ForMouse();
	void			Render();
	void			Render( CsPoint pos );

	void			ResetDevice();
	void			OnKillFocus();

	void			SetAlpha( float fAlpha );
	int				GetLastState(){ return m_nLastState; }

	void			Enable( bool bEnable );

public:	
	void			SetRange( CsPoint ptRange );

protected:
	void			_ChangeBarConstant();
	void			_CalBarPos();


public:
	void			SetCurPosIndex( int nIndex );
	int				GetCurPosIndex(){ return m_nCurPosIndex + m_ptRange.x; }
	int				GetCurPosAbs() { return m_nCurPosIndex; }
	int				GetMaxPosIndex(){ return m_nMaxPosIndex; }

	CsPoint			GetBarPos(){ return m_ptBarPos; }

	void			SetBtnMoveRange( int nRange ){ m_nBtnMoveRange = nRange; }
};