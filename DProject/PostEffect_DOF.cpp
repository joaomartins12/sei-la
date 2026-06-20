
#include "stdafx.h"
#include "Engine.h"



void CEngine::_Delete_DOF()
{
	m_pRT_DOF_Blur			=	NULL;
	m_pRT_DOF_Depth			=	NULL;
	m_pScreen_DOF			=	NULL;
}

bool CEngine::_Create_DOF()
{
	// 이미 만들어져 있다면 패스
	if( m_pRT_DOF_Blur != NULL )
		return true;

	if( _CreateMSAARenderTargetSurface() == false )
	{
		return false;
	}

	NiRenderedTexturePtr pRenderTex_Blur = NiRenderedTexture::Create( g_nScreenWidth/4, g_nScreenHeight/4, m_spRenderer );
	if( pRenderTex_Blur == NULL )
	{
		_DeletePostEffect();
		return false;
	}

	m_pRT_DOF_Blur = NiRenderTargetGroup::Create( pRenderTex_Blur->GetBuffer(), m_spRenderer, false, true );
	if( m_pRT_DOF_Blur == NULL )
	{
		pRenderTex_Blur = 0;
		_DeletePostEffect();
		return false;
	}

	NiRenderedTexturePtr pRenderTex_Depth = NiRenderedTexture::Create( g_nScreenWidth, g_nScreenHeight, m_spRenderer );
	if( pRenderTex_Depth == NULL )
	{
		_DeletePostEffect();
		return false;
	}

	m_pRT_DOF_Depth = NiRenderTargetGroup::Create( pRenderTex_Depth->GetBuffer(), m_spRenderer, true, false );
	if( m_pRT_DOF_Depth == NULL )
	{
		pRenderTex_Depth = 0;
		_DeletePostEffect();
		return false;
	}
	
	NiTexturingPropertyPtr pTexProp = NiNew NiTexturingProperty;
	pTexProp->SetBaseTexture( m_pRenderTexture_Org );
	NiTexturingProperty::ShaderMap* pShaderMap_Blur = NiNew NiTexturingProperty::ShaderMap( pRenderTex_Blur, 0 );
	pTexProp->SetShaderMap( 0, pShaderMap_Blur );
	NiTexturingProperty::ShaderMap* pShaderMap_Depth = NiNew NiTexturingProperty::ShaderMap( pRenderTex_Depth, 1 );
	pTexProp->SetShaderMap( 1, pShaderMap_Depth );
	pTexProp->SetBaseFilterMode( NiTexturingProperty::FILTER_NEAREST );
	pTexProp->SetApplyMode( NiTexturingProperty::APPLY_REPLACE );

	m_pScreen_DOF = NiNew NiScreenFillingRenderView;	
	m_pScreen_DOF->GetScreenFillingQuad().AttachProperty( pTexProp );
	m_pScreen_DOF->GetScreenFillingQuad().UpdateProperties();

	return true;
}

void CEngine::Begin_PE_DOF_Depth()
{
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );

	m_spRenderer->EndUsingRenderTargetGroup();

	m_spRenderer->GetBackgroundColor( m_BackupBackGroundColor );
	m_spRenderer->SetBackgroundColor( NiColorA::WHITE );

	m_spRenderer->BeginUsingRenderTargetGroup( m_pRT_DOF_Depth, NiRenderer::CLEAR_ALL );
}

void CEngine::_Render_PE_DOF()
{
	// 씬 종료
	m_spRenderer->EndUsingRenderTargetGroup();

	//m_spRenderer->BeginUsingRenderTargetGroup( m_pRT_DOF_Blur, NiRenderer::CLEAR_ALL );
	//ScreenSpace();

	/*m_pScreen_GB->GetScreenFillingQuad().ApplyAndSetActiveMaterial( "GaussianBlur_2" );
	_SetGaussianBlurParameter( 0.0f, 1.0f/(float)g_nScreenHeight, MAX_GAUSSIANBLUR_SAMPLE );
	m_pScreen_GB->GetScreenFillingQuad().RenderImmediate( m_spRenderer );

	_SetGaussianBlurParameter( 1.0f/(float)g_nScreenWidth, 0.0f, MAX_GAUSSIANBLUR_SAMPLE );
	m_pScreen_GB->GetScreenFillingQuad().RenderImmediate( m_spRenderer );*/

	// 씬 종료
	//m_spRenderer->EndUsingRenderTargetGroup();

	_UpdateMSAARenderTargetSurface();

	// 씬시작
	m_spRenderer->SetBackgroundColor( m_BackupBackGroundColor );
	m_spRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );

	m_spRenderer->SetScreenSpaceCameraData();
	m_pScreen_DOF->GetScreenFillingQuad().ApplyAndSetActiveMaterial( "DepthOfField" );
	m_pScreen_DOF->GetScreenFillingQuad().RenderImmediate( m_spRenderer );
}
