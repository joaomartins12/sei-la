
#pragma once 

class cRenderTex : public NiMemObject
{
public:
	cRenderTex();
	virtual ~cRenderTex();

protected:
	NiRenderTargetGroupPtr		m_pRenderTargetGroup;
	NiCamera*					m_pBackupCamera;
	cSprite*					m_pSprite;
	cSprite*					m_pSpriteBG;
	cSprite*					m_pEffectSubBG;
	cStringList					m_StringList;
	float						m_fCurrAlpha;
	float						m_fEndAlpha;
	float						m_fStartAlpha;
	float						m_fAlphaChangeSpeed;
	float						m_fAlphaDir;
	bool						m_bisStartAlpha;
	float						m_fDistConstant;
	float						m_fHeightConstant;
#ifdef CROSSWARS_SYSTEM
	float						m_fWidthConstant;
#endif
public:
	bool			Init( CsPoint texSize, char const* pBackImg = NULL );	//2016-03-03-nova 캐쉬샵일때 따로 처리
	bool			AddEffectSprite( CsPoint texSize, char const* pBackImg = NULL );
	bool			InitText( CsPoint TextPos, TCHAR* szText, int nWidth, cText::sTEXTINFO* pTextInfo );
	void			StartTextAlpha(const float& fAlphaStart, const float& fAlphaEnd, const float& fChangeSpeed);
	void			UpdateTextAlpha(const float& fTime );
	void			ResetTextAlpha();

	void			SetDistConstant( float fDistConstant ){ m_fDistConstant = fDistConstant; }
	void			SetHeightConstant( float fHeightConstant ){ m_fHeightConstant = fHeightConstant; }
#ifdef CROSSWARS_SYSTEM
	void			SetWidthConstant( float fWidthConstant ){ m_fWidthConstant = fWidthConstant; }
#endif
	void			BeginRender();
	void			BeginRender( NiColorA colorA, NiRenderer::ClearFlags clearFlag );
	void			RenderObject( CsGBSubCamera* pCamera, CsC_AvObject* pObject, bool bRenderEffect = false );						//2015-11-26-nova pCamera 추가
	void			RenderObjectEventScene( CsGBSubCamera* pCamera, CsC_AvObject* pObject, float fCameraDist, float fCameraRot );	//2015-11-30-nova pCamera 추가
	void			RenderObjectIncludeEffect( CsGBSubCamera* pCamera, CsC_AvObject* pObject, float fCameraDist );
	void			EndRender( CsGBSubCamera* pCamera, CsPoint pos );
	void			RenderEffect( CsGBSubCamera* pCamera, CsC_AvObject* pEffect, float fCameraDist );
#ifdef CROSSWARS_SYSTEM
	void			EndRenderForSquare( CsGBSubCamera* pCamera, CsPoint pos, CsPoint uv0, CsPoint uv1, CsPoint uv2, CsPoint uv3 );
#endif
	void			ResetDevice(){ m_pSprite->ResetDevice(); }
	void			SetSpriteSize( CsPoint size);
	void			SetSpritePos( CsPoint pos);
};