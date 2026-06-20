
#pragma once

class CsGBShadow : public NiMemObject
{
public:
	CsGBShadow();	
	~CsGBShadow();	


	//==========================================================================================
	//
	// COMMON
	//
	//==========================================================================================
protected:
	NiRenderTargetGroupPtr		m_pRTZBufferGroup;
	NiRenderedTexturePtr		m_pRTZBufferTex;
	D3DXMATRIX					m_matTexAdjust;
	CsGBLight*					m_pShadowLight;	

public:
	static	void	GlobalShotInit();
	static	void	GlobalShotDown();
public:
	
	void			Destroy();
	void			Init( int sx, int sy );
	void			ResetDevice();

	CsGBLight*			GetShadowLight(){ return m_pShadowLight; }
	NiRenderedTexture*	GetZBufferTex(){ return m_pRTZBufferTex; }

	//==========================================================================================
	//
	//	Client
	//
	//==========================================================================================
	
protected:
	NiSingleShaderMaterial*		m_pZSCharObject;
	NiSingleShaderMaterial*		m_pZSObject;

public:	
	NiSingleShaderMaterial*		GetZSCharObjectShader(){ if( m_pZSCharObject==NULL){ m_pZSCharObject=NiSingleShaderMaterial::Create("ZS_CharObject"); m_pZSCharObject->IncRefCount();} return m_pZSCharObject; }
	NiSingleShaderMaterial*		GetZSObjectShader(){ if( m_pZSObject==NULL){ m_pZSObject=NiSingleShaderMaterial::Create("ZS_Object"); m_pZSObject->IncRefCount();} return m_pZSObject; }

public:
	void			ResetMap();
	bool			BeginShadow( NiPoint3 ptActorPos );
	void			EndShadow( NiRenderTargetGroup* pOrgTarget );


	//==========================================================================================
	//
	//	TOOL
	//
	//==========================================================================================
protected:	
	NiSingleShaderMaterial*		m_pZSAlpha;

public:
	void			Tool_ResetMap( NiColor colorShadow );
	void			Tool_ResetObjectShadowLight();	
	void			Tool_RenderTRShadow( NiPoint3 ptTerrainPos );

	
	NiSingleShaderMaterial*		GetZSAlphaShader(){ if( m_pZSAlpha==NULL){ m_pZSAlpha=NiSingleShaderMaterial::Create("ZS_ALPHA"); m_pZSAlpha->IncRefCount();} return m_pZSAlpha; }
	

protected:
	void			_Tool_BeginLight2Camera( NiPoint3 ptActorPos );
	void			_Tool_EndLight2Camera();

public:
	void			Tool_CopyBorderShadowMap();
	void			Tool_InitApplyTRShadowMap( sCS_BMPDATA** ppBmpData, D3DXMATRIX** ppLVP, D3DXMATRIX** ppLVPT );
	void			Tool_ApplyTRShadowMap( int nLeafIndex, sCS_BMPDATA* pBmpData, D3DXMATRIX* pLVP, D3DXMATRIX* pLVPT );
	void			Tool_EndApplyTRShadowMap( sCS_BMPDATA** ppBmpData, D3DXMATRIX** ppLVP, D3DXMATRIX** ppLVPT );

	void			Tool_ApplyDMObjectShadowMap();

protected:
	void			_Tool_CalLightTRColor( std::list< CsGBLight* >* pLightList, NiColor& ResultColor, float& fRate, NiPoint3 ptPos, NiPoint3 ptNormal );

	// ==== Direction Light
protected:
	void			_Tool_InitApplyTRShadowMap_DirectionLight( sCS_BMPDATA** ppBmpData, D3DXMATRIX** ppLVP, D3DXMATRIX** ppLVPT );
	bool			_Tool_ZShadow2Bmp( TCHAR* szBmpName, NiPoint3 ptPos, NiPoint2 ptOrthoSize, sCS_BMPDATA* pBmpData, D3DXMATRIX* pLVP, D3DXMATRIX* pLVPT );
	void			_Tool_ApplyTRShadowMap_DirectionLight( int nLeafIndex, sCS_BMPDATA* pBmpData, D3DXMATRIX* pLVP, D3DXMATRIX* pLVPT );
	void			_Tool_EndApplyTRShadowMap_DirectionLight( sCS_BMPDATA** ppBmpData, D3DXMATRIX** ppLVP, D3DXMATRIX** ppLVPT );	

	void			_Tool_CreateDMObjectShadowMap_DirectionLight( CsGBObject* pBaseObj );
	void			_Tool_RenderBackBufferDMObject_DirectionLight( CsGBObject* pBaseObj );
	void			_Tool_ApplyDrakMap_DMObject_DirectionLight( NiNode* pNiBase, NiSourceTexturePtr pSourceTexture, D3DXMATRIX matViewProjAdj );


	// ==== Point Light
protected:
	void			_Tool_InitApplyTRShadowMap_PointLight();
	void			_Tool_ApplyTRShadowMap_PointLight( int nLeafIndex );
	void			_Tool_EndApplyTRShadowMap_PointLight();

	void					_Tool_CreateDMObjectShadowMap_PointLight( CsGBObject* pBaseObj, int nLeafIndex );
	NiTexturingPropertyPtr	_Tool_RenderBackBufferDMObject_PointLight( CsGBObject* pBaseObj, NiSingleShaderMaterial* pShader );	
	void					_Tool_ApplyDrakMap_DMObject_PointLight( NiNode* pNiBase, NiSourceTexturePtr pSourceTexture, D3DXMATRIX matViewProjAdj );
};

extern CsGBShadow*		g_pShadow;