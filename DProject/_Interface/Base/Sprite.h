
#pragma once

class cSprite : public cWindow
{
public:
	cSprite();
	virtual ~cSprite();

	enum eSPRITE_EVENT
	{
		SPRITE_LBDOWN_EVENT,
		SPRITE_LBUP_EVENT,
		SPRITE_RBDOWN_EVENT,
		SPRITE_RBUP_EVENT,
		SPRITE_ON_MOUSE_EVENT,
	};

	enum eACTION
	{
		ACTION_NONE,
		ACTION_ON,
		ACTION_DOWN,
		ACTION_PRESS,
		ACTION_CLICK,
	};

protected:
	enum eLIMIT{		
		LIMIT_LEFT		=	0x00000001,
		LIMIT_RIGHT		=	0x00000002,
		LIMIT_TOP		=	0x00000010,
		LIMIT_BOTTOM	=	0x00000020,
		LIMIT_NONE		=	0x00000000,
	};

	//=======================================================================================================================
	//
	//		static
	//
	//=======================================================================================================================
protected:
	static NiAlphaProperty*			m_pAlphaProp;
	static NiTexturingProperty*		m_pTexProp;
	static NiZBufferProperty*		m_pZBufferProp;
	static NiVertexColorProperty*	m_pVertexColorProp;

public:
	static void					GlobalInit();
	static void					GlobalShotDown();

	//=======================================================================================================================
	//
	//		base
	//
	//=======================================================================================================================
public:
	void				Delete();
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize, NiColor color = NiColor::WHITE, bool bUseWorkingFolder = true );
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, NiTexture* pTexture, bool bApplyWindowSize, NiColor color = NiColor::WHITE );
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize, NiColor color = NiColor::WHITE );
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, NiColorA colorA, bool bApplyWindowSize );

	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, CsRect TextureRect, char const* cImagePath, bool bApplyWindowSize, NiColor color = FONT_WHITE, bool bUseWorkingFolder =true );
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, CsRect TextureRect, NiTexture* pTexture, bool bApplyWindowSize, NiColor color = FONT_WHITE );

	void				InitStencil( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize, NiStencilProperty* pStencilProp, NiColor color = NiColor::WHITE, bool bUseWorkingFolder = true );
	void				InitStencil( cWindow* pParent, CsPoint pos, CsPoint size, NiTexture* pTexture, bool bApplyWindowSize, NiStencilProperty* pStencilProp, NiColor color = NiColor::WHITE );
	void				InitStencil( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize, NiStencilProperty* pStencilProp, NiColor color = NiColor::WHITE );

	//void				InitTex2( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize );	
	virtual void		Update(float const& fDeltaTime) {}
	virtual int			Update_ForMouse();
	virtual void		Render();
	virtual void		Render( CsPoint pos /*느리다*/ );
	void				Render( CsPoint pos /*느리다*/, float fAlpha ){ SetAlpha( fAlpha ); Render( pos ); }
	void				Render( CsPoint pos, CsPoint size /*느리다*/ );
	//void				Render( CsPoint pos, CsPoint size /*느리다*/ ){ SetAlpha( fAlpha ); Render( pos, size ); }	
	void				Render( NiPoint2 texLT, NiPoint2 texRB );
	void				Render( CsPoint pos, NiPoint2 texLT, NiPoint2 texRB );
	void				Render( CsPoint pos, CsPoint size, NiPoint2 texLT, NiPoint2 texRB );
	void				Render( CsPoint pos, CsPoint size, NiPoint2 texLT, NiPoint2 texRB , float fAlpha );
	void				RenderLimit( CsPoint pos );

	virtual eTYPE		GetType(){ return T_Sprite; }

	void				SetRotation( float fAngle, CsPoint ptSize );
	void				SetRotation2( float fAngle );
	float				GetInvTextWidth() const;
	float				GetInvTextHeight() const;

protected:
	float				m_fInvTexWidth;
	float				m_fInvTexHeight;

	CsPoint				m_ptTempPos;
	NiPoint2			m_vTempTex;

	NiScreenElementsPtr m_spScreen;
	NiColorA			m_Color;	
	GUI::TexturePtr 	m_pTexter;

public:
	virtual void		ResetDevice( CsPoint pos );
	virtual void		ResetDevice();
	void				ResetTemp(){ m_ptTempPos = CsPoint::ZERO; }
	CsPoint				GetTempPos() {return m_ptTempPos;}
	const std::string& GetFilePath(){return m_strFileName;}
protected:
	void				_ResetDevice();

	std::string			m_strFileName;

	bool				m_bClick;


	//=======================================================================================================================
	//
	//		LimitTex
	//
	//=======================================================================================================================
protected:
	DWORD				m_dwLimitPlag;
	float				m_fLimit[ 2 ];
public:
	void				SetLimitLeft( int nLimit ){ m_dwLimitPlag |= LIMIT_LEFT; m_fLimit[ 0 ] = nLimit/(float)g_nScreenWidth; }	
	void				SetLimitRight( int nLimit ){ m_dwLimitPlag |= LIMIT_RIGHT; m_fLimit[ 1 ] = nLimit/(float)g_nScreenWidth; }
	//void				SetLimitTop( int nLimit ){ m_dwLimitPlag |= LIMIT_TOP; m_fLimit[ 2 ] = nLimit/(float)g_nScreenHeight; }
	//void				SetLimitBottom( int nLimit ){ m_dwLimitPlag |= LIMIT_BOTTOM; m_fLimit[ 3 ] = nLimit/(float)g_nScreenHeight; }

	//=======================================================================================================================
	//
	//		status
	//
	//=======================================================================================================================
public:
	NiTexturingProperty*	GetTexProp(){ return (NiTexturingProperty*)m_spScreen->GetProperty( NiTexturingProperty::GetType() ); }
	NiAlphaProperty*		GetAlphaProp(){ return (NiAlphaProperty*)m_spScreen->GetProperty( NiAlphaProperty::GetType() ); }
	NiScreenElements*		GetScreen(){ return m_spScreen; }

	void				SetColorA( NiColorA color );
	void				SetColor( NiColor color );
	virtual void		SetAlpha( float fAlpha );
	void				ChangeTexture( CsPoint size, NiTexture* pTexture );
	void				ChangeTexture( char const* pTextureFileName, bool bUseWorkingFolder = true );
	void				ChangeTexture( char const* pTextureFileName, CsRect const& rect, bool bUseWorkingFolder = true );
	NiColorA			GetColorA(){ return m_Color; }
	NiColor				GetColor(){ return NiColor( m_Color.r, m_Color.g, m_Color.b ); }

	virtual void		SetPos( CsPoint ptPos );
	virtual void		SetPos_Float( NiPoint2 vPos );
	virtual void		SetSize( CsPoint ptSize );
	CsPoint				GetPos();
	
	virtual void		SetPosSize( CsPoint ptPos, CsPoint ptSize );
	virtual void		SetPosSize_Float( NiPoint2 vPos, NiPoint2 vSize );

	void				SetTextureUV( NiPoint2 texLT, NiPoint2 texRB );

protected:
	void				_CalRectangle();	

#ifdef CROSSWARS_SYSTEM
public:
	
	void				RenderSquare( CsPoint pos, const NiPoint2& a, const NiPoint2& b, const NiPoint2& c, const NiPoint2& d );
#endif
	//==============================================================================
	// 트라이앵글
	//==============================================================================
public:
	void				CreateTriangle( NiColorA color );
	void				RenderTriangle( int x, int y, const NiPoint2& a, const NiPoint2& b, const NiPoint2& c );
};