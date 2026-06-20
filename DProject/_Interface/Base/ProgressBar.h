

#pragma once 

class cProgressBar : public cWindow
{
public:
	enum eVALUE{ INVALIDE_VALUE = INT_MAX, };

public:
	cProgressBar();
	~cProgressBar(){ Delete(); }

public:
	enum eBAR_TYPE{
		BAR_3IMAGE,
		BAR_1IMAGE,
		BAR_STATIC,
	};
	enum ePOS_TEXT_TYPE{
		TEXT_NONE,
		TEXT_VALUE,
		TEXT_PERCENT,
		TEXT_PERCENT_F3,
	};
	
	// 그리는데 필요한거
protected:
	cSprite*		m_pSpriteStart;
	cSprite*		m_pSpriteCenter;
	cSprite*		m_pSpriteEnd;

	eBAR_TYPE		m_eBarType;
	CsPoint			m_ptStart_End_TextureSizeX;
	ePOS_TEXT_TYPE	m_ePosTextType;

	cText*			m_pText;

public:
	void			Delete();

	virtual eTYPE	GetType(){ return T_Progress; }

	void			Init( cWindow* pParent, eBAR_TYPE Bar_Type, CsPoint pos, CsPoint tsize, CsPoint SE_TexSX, char* cImagePath, bool bApplyWindowSize );
	void			Render();
	void			Render(CsPoint pos);
	void			ResetDevice();
	void			Update(float const& fDeltaTime);

	virtual void	SetAlpha( float fAlpha );

	// 계산
protected:
	CsPoint			m_ptBackupRange;
	CsPoint			m_ptRange;
	int				m_nCurPos;
	int				m_nRenderWidth;			// 실제 그려야 하는거리
	float			m_fWidthRate;

public:
	void			SetRange( CsPoint ptRange ){ m_ptBackupRange = ptRange; }
	CsPoint			GetBackupRange(){ return m_ptBackupRange; }

	bool			SetBarPos( int nPos );

	void			SetRenderPosText( ePOS_TEXT_TYPE type );

	void			SetColor( NiColor color );
};