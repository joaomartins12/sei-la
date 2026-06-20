

#pragma once 

class cScrollBar : public cWindow
{
public:
	static CsPoint	GetDefaultBtnSize(){ return CsPoint( 16, 15 ); }

public:
	enum eSCROLL_TYPE{ TYPE_1, TYPE_2,
		TYPE_GOTCHA,
		TYPE_ENCY,
		TYPE_SEALMASTER,
	};


public:
	cScrollBar();
	~cScrollBar(){ Delete(); }

	// 그리는데 필요한거
protected:
	cButton*		m_pUpButton;
	cButton*		m_pDownButton;
	cSprite*		m_pSpriteStart;
	cSprite*		m_pSpriteCenter;
	cSprite*		m_pSpriteEnd;
	cSprite*		m_pSpriteBG;

public:
	enum eACTION{
		ACTION_NONE, ACTION_CLICK,
	};

public:
	void			Delete();

	void			Init( eSCROLL_TYPE type, cWindow* pParent, CsPoint pos, CsPoint CtrlSize, CsPoint btnSize, CsRect rcRegion, int nRenderCount, bool bApplyWindowSize );

	virtual void	Update(float const& fDeltaTime) {}
	virtual int		Update_ForMouse();
	virtual eTYPE	GetType(){ return T_Scrollbar; }
	void			Render();

	void			ResetDevice();
	void			OnKillFocus();

	void			SetAlpha( float fAlpha );

	void			SetEnableRenderFromEnableScroll( bool bEnableRE ){ m_bEnableRenderFromEnableScroll = bEnableRE; }

	void			ChangeGBImg( char* cImagePath, bool bApplyWindowSize );
	// 계산
protected:
	CsRect			m_rcRegion;					//		입력받을 영역
	CsPoint			m_ptRange;					//		스크롤로 그리고 있는 아이템의 라인수(채팅창 100줄까지 증가가능)
	int				m_nRenderCount;				//		한번에 그리는 라인 수
	int				m_nMaxPosIndex;				//		움직일수 잇는 칸~
	int				m_nCurPosIndex;				//		현재 존재 한는칸
	float			m_fPos_Height;				//		포스 한칸당의 높이

	CsPoint			m_ptButtonSize;				//		넓이

	bool			m_bEnableScroll;
	int				m_nBarTotalHeight;			//		바의 총 길이	
	int				m_nBarViewHeight;			//		그려야 하는바의 길이

	CsRect			m_rcBarView;				//		바의 위치
	CsRect			m_rcBarTotal;				//		바가 그려질수 있는 총 위치

	int				m_nRegionClickMoveIndex;	//		영역 클릭시 한번에 이동할 최대 인덱스 갯수

	CsPoint			m_ptBackupLocalMousePos;

	CsTimeSeq		m_ScrollTimeSeq;

	bool			m_bEnableRenderFromEnableScroll;		// 스크롤 가능할경우만 그리겠다.
	int				m_nBarTopHeight;						// 바 이미지 크기 설정 가능 윗 부분
	int				m_nBarBotHeight;						// 바 이미지 크기 설정 가능 아랫 부분

protected:
	// 바 클릭 타입 - 논, 바만, 영역만
	enum eCLICK_BAR_TYPE{
		CB_NONE, CB_BAR, CB_REGION,
	};
	eCLICK_BAR_TYPE	m_eBarClickType;

public:
	void			ReSize( CsPoint ptSize, CsRect rcRegion );
	bool			SetRange( CsPoint ptRange );
	void			SetRenderCount( int nRenderCount );
	void			IncreaseMaxCount( int nLimitMax = 1000 );
	void			SetMouseWheelMove( int nWheel );

protected:
	void			_ChangeBarConstant();

	bool			_BarRegionClick( CsPoint ptMouseLocalPos );
	void			_CalBarViewRect();


public:
	bool			IsEnableScroll(){ return m_bEnableScroll; }
	void			SetCurPosIndex( int nIndex );
	int				GetCurPosIndex(){ return m_nCurPosIndex; }
	int				GetMaxPosIndex(){ return m_nMaxPosIndex; }
	int				GetRenderCount(){ return m_nRenderCount; }

	CsRect			GetRegion(){ return m_rcRegion; }
};