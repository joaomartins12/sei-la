

#pragma once

class CBillboard : public NiMemObject
{
public:
	CBillboard();
	virtual ~CBillboard();

public:
	static void		GlobalInit();
	static void		GlobalDelete();

	void			CreateFile( char const* pkFileName, float size_x, float size_y);
	void			CreateFile( char const* pkFileName, float size_x, float size_y, float fTexX1, float fTexX2, float fTexY1, float fTexY2 );

	void			CreateTexture( float size_x, float size_y, NiTexture* pTexture);
	void			CreateTexture( float size_x, float size_y, float fTexX1, float fTexX2, float fTexY1, float fTexY2, NiTexture* pTexture );

	void			SetColor( NiColor color );
	void			SetAlpha( float fAlpha );

	void			Render( NiPoint3 vPos, float fX, float fY );
	void			Render( NiPoint3 vPos, float fX, float fY, float fAlpha );
	void			Render( NiPoint3 vPos, float fX, float fY, float fAlpha, float fScale );

	void			RenderAbsolute( NiPoint3 vPos, float fX, float fY );
	void			RenderAbsolute( NiPoint3 vPos, float fX, float fY, float fAlpha );
	void			RenderAbsolute( NiPoint3 vPos, float fX, float fY, float fAlpha, float fScale );

	void			SetScale( float fScale ){ m_Node.m_pNiGeom->SetScale( fScale ); }
	
	float			GetSizeX() const { return m_fSizeX; }
	float			GetSizeY() const { return m_fSizeY; }
	void			SetSize( float x, float y );

	void			ChangeTexture( CsPoint ptSize, NiTexture* pTexture );
	void			ChangeTexture( const char* pFileName, CsPoint ptSize );

public:
	CsGeomObj			m_Node;
	GUI::TexturePtr		m_pTexture;

protected:
	float			m_fSizeX;
	float			m_fSizeY;
	float			m_fAlpha;
	NiColor			m_Color;

protected:
	static NiAlphaProperty*		m_AlphaProp;
	static NiZBufferProperty*	m_ZBufferProp;
	static NiMaterialProperty*	m_MapProp;
};

