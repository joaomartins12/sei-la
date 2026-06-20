
#pragma once 


class cImage : public cSprite
{
public:
	cImage();
	virtual ~cImage();

	//=======================================================================================================================
	//
	//		state
	//
	//=======================================================================================================================
public:
	enum eSTATE{	
		NORMAL0, NORMAL1, NORMAL2, NORMAL3, 
		// character select 과정에서 신규 테이머 추가로 인한 tamer icon 그려주기 위해 enum값 추가	lks007
		NORMAL4, NORMAL5, NORMAL6, NORMAL7,
		NORMAL8, NORMAL9,
		//		NORMAL10, NORMAL11,
		MAX_STATE_COUNT,
	};

	enum eACTION{
		ACTION_NONE, ACTION_ON, ACTION_DOWN, ACTION_PRESS, ACTION_CLICK,
	};

	enum eIMAGE_EVENT
	{
		IMAGE_LBDOWN_EVENT,
		IMAGE_LBUP_EVENT,
		IMAGE_LBDLBCLICKED_EVENT,
		IMAGE_RBDOWN_EVENT,
		IMAGE_RBUP_EVENT,
	};

public:
	void			Init( cWindow* pParent, CsPoint pos, CsPoint size, const char* cImagePath, bool bApplyWindowSize, bool bUseWorkingFolder = true );

	virtual int		Update_ForMouse();

	virtual void	Render();
	virtual void	Render( CsPoint pos );
	virtual void	Render( CsPoint pos, int nImageIndex );
	virtual void	Render( int nImageIndex );
	virtual eTYPE	GetType() const;
	virtual void	ResetDevice();

	virtual	void	SetEnable( bool bEnable );
	bool			GetEnable() const;

	void			SetTexToken( CsPoint tex );
	void			SetState( int const& state );
	int				GetState() const;

	void			SetMouseOverImg( const char* cImagePath );

protected:
	bool			m_bEnable;
	bool			m_bClick;
	bool			m_bIsMouseOver;
	int				m_nXMaxFrame;
	int				m_nYMaxFrame;
	int				m_nCurrentFrame;
	float			m_texTokenSizeX;
	float			m_texTokenSizeY;
	cSprite*		m_pMouseOverImg;
};