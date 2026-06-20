
#include "stdafx.h"
#include "RenderTex.h"

cRenderTex::cRenderTex()
:m_fCurrAlpha(1.0f)
, m_fEndAlpha(1.0f)
, m_fStartAlpha(0.0f)
, m_fAlphaChangeSpeed(1.0f)
, m_bisStartAlpha(false)
, m_pEffectSubBG(NULL)
, m_fAlphaDir(1.0f)
{
	m_pRenderTargetGroup = NULL;
	m_pSprite = NULL;

	m_fDistConstant = 1.0f;
	m_fHeightConstant = 1.0f;
#ifdef CROSSWARS_SYSTEM
	m_fWidthConstant = 0.0f;
#endif
	m_pBackupCamera = NULL;
	m_pSpriteBG = NULL;
}

cRenderTex::~cRenderTex()
{
	m_pRenderTargetGroup = 0;

	SAFE_NIDELETE( m_pSpriteBG );
	SAFE_NIDELETE(m_pEffectSubBG);
	SAFE_NIDELETE( m_pSprite );
	m_StringList.Delete();
}

bool cRenderTex::InitText( CsPoint TextPos, TCHAR* szText, int nWidth, cText::sTEXTINFO* pTextInfo )
{
	cString* pString = NiNew cString;
	{
#if ( defined VERSION_USA || defined VERSION_TH || defined VERSION_HK || defined VERSION_TW ) // FT_TOOLTIP_MSG_BODY_LEN : SIMPLE_TOOLTIP 디파인 필요
		TCHAR szComment[ 1024 ];
		_tcscpy_s( szComment, 1024,  szText );
#else
		TCHAR szComment[ FT_TOOLTIP_MSG_BODY_LEN ];
		_tcscpy_s( szComment, FT_TOOLTIP_MSG_BODY_LEN,  szText );
#endif
		g_pStringAnalysis->Cut_Parcing_ForCardMaster( pString, nWidth, TextPos, szComment, pTextInfo );
		pString->CalMaxVSize();
	}
	m_StringList.AddTail( pString );

	return true;
}

bool cRenderTex::Init( CsPoint texSize, char const* pBackImg )
{
	// -------------------------------------------	
	// 랜더 타겟 텍스쳐 생성
	NiTexture::FormatPrefs kPrefs;
	if( NULL == pBackImg )
	{
		kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::FLOAT_COLOR_32;
		kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
		kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::SMOOTH;
	}

	NiRenderedTexture* pRenderTexture = NiRenderedTexture::Create( texSize.x, texSize.y, g_pEngine->m_spRenderer, kPrefs );
	if( pRenderTexture == NULL )
		return false;

	m_pRenderTargetGroup = NiRenderTargetGroup::Create( pRenderTexture->GetBuffer(), g_pEngine->m_spRenderer, false, true );
	if( m_pRenderTargetGroup == NULL )
		return false;

	m_pSprite = NiNew cSprite;
	m_pSprite->Init( NULL, CsPoint::ZERO, texSize, pRenderTexture, true );

	if( pBackImg )
	{
		m_pSpriteBG = NiNew cSprite;
		m_pSpriteBG->Init( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight), pBackImg, false );	
	}

	ResetTextAlpha();
	return true;
}
bool cRenderTex::AddEffectSprite( CsPoint texSize, char const* pBackImg  )
{
	if( pBackImg )
	{
		m_pEffectSubBG = NiNew cSprite;
		m_pEffectSubBG->Init( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight), pBackImg, false );
	}
	return true;
}
void cRenderTex::BeginRender()
{
	g_pEngine->m_spRenderer->EndUsingRenderTargetGroup();	
	g_pEngine->m_spRenderer->SetBackgroundColor( NiColorA( 0.0f, 0.0f, 0.0f, 0.0f) );
	g_pEngine->m_spRenderer->BeginUsingRenderTargetGroup( m_pRenderTargetGroup, NiRenderer::CLEAR_ALL );
}
void cRenderTex::BeginRender( NiColorA colorA, NiRenderer::ClearFlags clearFlag )
{
	g_pEngine->m_spRenderer->EndUsingRenderTargetGroup();	
	g_pEngine->m_spRenderer->SetBackgroundColor( colorA );
	g_pEngine->m_spRenderer->BeginUsingRenderTargetGroup( m_pRenderTargetGroup, clearFlag );
}
void cRenderTex::RenderObject( CsGBSubCamera* pCamera, CsC_AvObject* pObject, bool bRenderEffect )
{
	if(m_pEffectSubBG)
		m_pEffectSubBG->Render();

	if( m_pSpriteBG )
		m_pSpriteBG->Render();

	SAFE_POINTER_RET( pObject );

	CsNodeObj* pCsNode = pObject->GetCsNode();
	if( pCsNode == NULL )
		return;

#ifdef CROSSWARS_SYSTEM
	pCamera->SetTranslate( pObject->GetPos() + pObject->GetDir()*100.0f + NiPoint3( m_fWidthConstant, 0, pObject->CsC_AvObject::GetToolHeight()*0.5f*m_fHeightConstant ) );
#else
	pCamera->SetTranslate( pObject->GetPos() + pObject->GetDir()*100.0f + NiPoint3( 0, 0, pObject->CsC_AvObject::GetToolHeight()*0.5f*m_fHeightConstant ) );
#endif
	pCamera->SetRotation( -pObject->GetDir() );
	float fCameraDist = CsMax( 250.0f, pObject->CsC_AvObject::GetToolHeight()*sqrt( pObject->CsC_AvObject::GetToolHeight()*0.3f )*0.15f )*m_fDistConstant;
	pCamera->SetDist( fCameraDist );

	pCamera->ChangeStandardFov( (float)m_pSprite->GetSize().x, (float)m_pSprite->GetSize().y );
	pCamera->_UpdateCamera();
	g_pEngine->m_spRenderer->SetCameraData( pCamera->GetCameraObj() );	

	if( bRenderEffect )
		pCsNode->RenderSort(fCameraDist);
	else
		pCsNode->Render_GeomData_ExceptPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE );	
}

void cRenderTex::RenderObjectEventScene( CsGBSubCamera* pCamera, CsC_AvObject* pObject, float fCameraDist, float fCameraRot )
{
	CsNodeObj* pCsNode = pObject->GetCsNode();
	if( pCsNode == NULL )
		return;

#ifdef CROSSWARS_SYSTEM
	pCamera->SetTranslate( pObject->GetPos() + pObject->GetDir()*100.0f + NiPoint3( m_fWidthConstant, 0, pObject->GetToolHeight()*0.5f*m_fHeightConstant ) );	
#else
	pCamera->SetTranslate( pObject->GetPos() + pObject->GetDir()*100.0f + NiPoint3( 0, 0, pObject->GetToolHeight()*0.5f*m_fHeightConstant ) );	
#endif
	pCamera->SetRotation( fCameraRot, -0.3f );
	pCamera->SetDist( fCameraDist );

	pCamera->ChangeStandardFov( (float)m_pSprite->GetSize().x, (float)m_pSprite->GetSize().y );
	pCamera->_UpdateCamera();
	g_pEngine->m_spRenderer->SetCameraData( pCamera->GetCameraObj() );	

	pCsNode->Render_GeomData_ExceptPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE );	
	if( pObject->GetProp_Effect() )
		pObject->GetProp_Effect()->RenderAbsolute();
	
}

void cRenderTex::RenderObjectIncludeEffect( CsGBSubCamera* pCamera, CsC_AvObject* pObject, float fCameraDist )
{
	if(m_pEffectSubBG)
		m_pEffectSubBG->Render();

	if( m_pSpriteBG )
		m_pSpriteBG->Render();


	CsNodeObj* pCsNode = pObject->GetCsNode();
	if( pCsNode == NULL )
		return;

#ifdef CROSSWARS_SYSTEM
	pCamera->SetTranslate( NiPoint3(m_fWidthConstant, 0, pObject->GetToolHeight()*0.5f*m_fHeightConstant ) );	
#else
	pCamera->SetTranslate( NiPoint3( 0, 0, pObject->GetToolHeight()*0.5f*m_fHeightConstant ) );	
#endif
	pCamera->SetDist( fCameraDist );	

	pCamera->ChangeStandardFov( (float)m_pSprite->GetSize().x, (float)m_pSprite->GetSize().y );
	pCamera->_UpdateCamera();
	g_pEngine->m_spRenderer->SetCameraData( pCamera->GetCameraObj() );		

	pCsNode->RenderSort(1.0f);
}


void cRenderTex::RenderEffect( CsGBSubCamera* pCamera, CsC_AvObject* pEffect, float fCameraDist )
{
	CsNodeObj* pCsNode = pEffect->GetCsNode();
	if( pCsNode == NULL )
		return;

#ifdef CROSSWARS_SYSTEM
	pCamera->SetTranslate( pEffect->GetPos() + pEffect->GetDir()*100.0f + NiPoint3( m_fWidthConstant, 0, pEffect->GetToolHeight()*0.5f*m_fHeightConstant ) );
#else
	pCamera->SetTranslate( pEffect->GetPos() + pEffect->GetDir()*100.0f + NiPoint3( 0, 0, pEffect->GetToolHeight()*0.5f*m_fHeightConstant ) );
#endif
	pCamera->SetRotation( -pEffect->GetDir() );
	pCamera->SetDist( fCameraDist );

	pCamera->ChangeStandardFov( (float)m_pSprite->GetSize().x, (float)m_pSprite->GetSize().y );
	pCamera->_UpdateCamera();
	g_pEngine->m_spRenderer->SetCameraData( pCamera->GetCameraObj() );	

	pCsNode->Render_GeomData_ExceptPlag( 0 );	
}


#ifdef CROSSWARS_SYSTEM
void cRenderTex::EndRenderForSquare( CsGBSubCamera* pCamera, CsPoint pos, CsPoint uv0, CsPoint uv1, CsPoint uv2, CsPoint uv3 )
{	
	g_Sorting.Render( pCamera->GetCameraObj() );

	g_pEngine->m_spRenderer->EndUsingRenderTargetGroup();
	g_pEngine->m_spRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_NONE );
	g_pEngine->m_spRenderer->SetScreenSpaceCameraData();
	
	// r,t,l
	m_pSprite->RenderSquare(pos, NiPoint2( (float)uv0.x, (float)uv0.y ), NiPoint2( (float)uv1.x, (float)uv1.y ), NiPoint2( (float)uv2.x, (float)uv2.y ), NiPoint2( (float)uv3.x, (float)uv3.y ));

	g_pEngine->m_spRenderer->SetBackgroundColor( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_BackGroundColor );	
}
#endif

///Text Alpha 관련련
void cRenderTex::StartTextAlpha(const float& fAlphaStart, const float& fAlphaEnd, const float& fChangeSpeed)
{
	m_bisStartAlpha = true;

	m_fAlphaChangeSpeed = fChangeSpeed;
	m_fStartAlpha = fAlphaStart;
	m_fEndAlpha = fAlphaEnd;
	m_fCurrAlpha = m_fStartAlpha;
	m_StringList.SetAlpha( CsMax(m_fCurrAlpha, 0.0f));
	m_fAlphaDir = 1.0f;
}
void cRenderTex::UpdateTextAlpha(const float& fTime )
{
	if(m_bisStartAlpha == true)
	{
		m_fCurrAlpha += fTime * m_fAlphaChangeSpeed * m_fAlphaDir;
		if( m_fCurrAlpha >= m_fEndAlpha)
		{
			m_fCurrAlpha = m_fEndAlpha;
			m_fAlphaDir = -1.0f;
		}
		else if (m_fCurrAlpha <= m_fStartAlpha)
		{
			m_fCurrAlpha = m_fStartAlpha;
			m_fAlphaDir = 1.0f;
		}
		if(m_pSpriteBG)
			m_pSpriteBG->SetAlpha( m_fCurrAlpha);
	}
}
void cRenderTex::ResetTextAlpha()
{
	m_fCurrAlpha = 1.0f;
	m_fEndAlpha = 1.0f;
	m_fStartAlpha = 0.0f;
	m_fAlphaChangeSpeed = 1.0f;
	m_bisStartAlpha = false;
	m_StringList.SetAlpha(m_fCurrAlpha);
}
void cRenderTex::EndRender( CsGBSubCamera* pCamera, CsPoint pos )
{
	// 알파랜더링

	if(pCamera == NULL)	//2015-12-02-nova
	{
		g_Sorting.Render( CAMERA_ST.GetCameraObj() );
	}
	else
	{
		g_Sorting.Render( pCamera->GetCameraObj() );
	}	

	g_pEngine->m_spRenderer->EndUsingRenderTargetGroup();
	g_pEngine->m_spRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_NONE );
	g_pEngine->m_spRenderer->SetScreenSpaceCameraData();

	m_pSprite->Render( pos );	

	m_StringList.Render_Overlab_RenderText( pos );
	
	// 백그라운드 칼라 복귀
	g_pEngine->m_spRenderer->SetBackgroundColor( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_BackGroundColor );	
}
void cRenderTex::SetSpriteSize( CsPoint size)
{
	m_pSprite->SetSize(size);
}
void cRenderTex::SetSpritePos( CsPoint pos)
{
	m_pSprite->SetPos(pos);
}