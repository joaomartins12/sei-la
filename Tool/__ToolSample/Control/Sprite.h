
#pragma once

class cSprite : public cWindow
{
public:
	cSprite();
	virtual ~cSprite(){	Delete(); }

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
	void				Delete(){ m_spScreen = 0; }
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, char* cImagePath, bool bApplyWindowSize, NiColor color = NiColor::WHITE, bool bUseWorkingFolder = true );
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, NiTexture* pTexture, bool bApplyWindowSize, NiColor color = NiColor::WHITE );
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize, NiColor color = NiColor::WHITE );

	virtual void		Render();
	void				Render( CsPoint pos /*느리다*/ );
	void				Render( CsPoint pos /*느리다*/, CsPoint size );

	void				RenderRotation( CsPoint pos /*느리다*/ );

protected:
	float				m_fInvTexWidth;
	float				m_fInvTexHeight;
	float				m_fSpriteRot;

	CsPoint				m_ptTempPos;

	NiScreenElementsPtr m_spScreen;
	NiColorA			m_Color;

public:
	virtual void		ResetDevice( CsPoint pos );
	virtual void		ResetDevice();
	void				ResetTemp(){ m_ptTempPos = CsPoint::ZERO; }
protected:
	void				_ResetDevice();

	//=======================================================================================================================
	//
	//		status
	//
	//=======================================================================================================================
public:
	NiTexturingProperty*	GetTexProp(){ return (NiTexturingProperty*)m_spScreen->GetProperty( NiTexturingProperty::GetType() ); }
	NiAlphaProperty*		GetAlphaProp(){ return (NiAlphaProperty*)m_spScreen->GetProperty( NiAlphaProperty::GetType() ); }
	NiScreenElements*		GetScreen(){ return m_spScreen; }

	void				SetRotation( float fAngle ){ m_fSpriteRot = fAngle; }
	void				Rotation( float fAngle );

	void				SetColorA( NiColorA color );
	void				SetColor( NiColor color );
	virtual void		SetAlpha( float fAlpha );
	void				ChangeTexture( CsPoint size, NiTexture* pTexture );
	NiColorA			GetColorA(){ return m_Color; }

	virtual void		SetPos( CsPoint ptPos );
	virtual void		SetSize( CsPoint ptSize );
	virtual void		SetPosSize( CsPoint ptPos, CsPoint ptSize );

protected:
	void				_CalRectangle();	
};