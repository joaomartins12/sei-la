#pragma once 


class cUI_ProgressBar : public cWindow
{
public:
	enum eProgressbarEvent
	{
		eProgressbar_Change,
		eProgressbar_Done,
	};

	enum eDIR_TYPE
	{
		LEFT_RIGHT,
		RIGHT_LEFT,
		BOTTOM_TOP,
	};

	enum eViewTextType
	{
		NONE,			// 보여주지 않는다.
		INT_ONLY_CURRENT_VALUE,
		INT_SLASH_INT, //" %d / %d "
		FLOAT_SLASH_FLOT, //" %f / %f "
		INT_PERSENT, //" %d %% "
		INT_PERSENT_SLASH_INT_PERSENT, //" %d %% / %d %%"
	};

public:
	cUI_ProgressBar();
	virtual ~cUI_ProgressBar();

	virtual void	Init( cWindow* pParent, CsPoint pos, CsPoint tsize, bool bApplyWindowSize );
	virtual void	Delete();	
	virtual void	Render();
	virtual void	ResetDevice();
	virtual void	Update(float fDeltaTime);
	virtual eTYPE	GetType(){ return T_New_Progress; }
	virtual void	SetPos( CsPoint ptPos ) override;

public:
	virtual void	UpdateShowString();
	void			SetViewTextType( eViewTextType eType );

	void			SetBackImage( char* cImagePath );
	void			SetGuageImage( char* cImagePath );
	void			SetCoverImage( char* cImagePath );
	void			SetStringControl( cText::sTEXTINFO* ti );

	void			Step();
	void			AdjustProgress(float delta);
	bool			IsMaxGuage() const;

	void			SetRange( float fRange );
	void			SetProgressPos( float fPos );
	void			SetStepSize(float fStepSize);
	void			SetDirType(eDIR_TYPE eDirType);
	void			SetString( std::wstring const& str );
	void			SetProgressRect( CsRect const& rect );
	float			GetCurrentPosition() const;

	float			GetRemainVale() const;
	float			GetCurrentVale() const;

	void			SetRenderText( bool bValue );

private:
	void			UpdateProgressBar();

protected:
	cSprite*		m_pProgressBackImg;
	cSprite*		m_pProgressGuageImg;
	cSprite*		m_pProgressCoverImg;
	cText*			m_pRenderText;

	CsRect			m_rectProgress;

	cText::sTEXTINFO	m_TextInfo;

	std::wstring	m_ProgressString;	// progress bar show string.

	float			m_fProgressRange;
	float			m_fCurrentPos;

	float			m_fProgress;	//!< current progress (from 0.0f to 1.0f)	
	float			m_fStep;		//!< amount to 'step' progress by on a call to step()

	eDIR_TYPE		m_DirType;
	eViewTextType	m_ViewTextType;
};