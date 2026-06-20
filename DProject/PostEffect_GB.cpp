

#include "stdafx.h"
#include "Engine.h"



void CEngine::_Delete_GaussianBlur()
{
	m_pRT_GB_Blur			=	NULL;
	m_pScreen_GB			=	NULL;
}

bool CEngine::_Create_GaussianBlur()
{
	// 이미 만들어져 있다면 패스
	if( m_pRT_GB_Blur != NULL )
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

	m_pRT_GB_Blur = NiRenderTargetGroup::Create( pRenderTex_Blur->GetBuffer(), m_spRenderer, false, true );
	if( m_pRT_GB_Blur == NULL )
	{
		pRenderTex_Blur = 0;
		_DeletePostEffect();
		return false;
	}	

	NiTexturingPropertyPtr pTexProp = NiNew NiTexturingProperty;
	pTexProp->SetBaseTexture( m_pRenderTexture_Org );
	NiTexturingProperty::ShaderMap* pShaderMap = NiNew NiTexturingProperty::ShaderMap( pRenderTex_Blur, 0 );
	pTexProp->SetShaderMap( 0, pShaderMap );
	pTexProp->SetBaseFilterMode( NiTexturingProperty::FILTER_NEAREST );
	pTexProp->SetApplyMode( NiTexturingProperty::APPLY_REPLACE );

	m_pScreen_GB = NiNew NiScreenFillingRenderView;	
	m_pScreen_GB->GetScreenFillingQuad().AttachProperty( pTexProp );
	m_pScreen_GB->GetScreenFillingQuad().UpdateProperties();

	return true;
}

void CEngine::_Render_PE_GB()
{
	// 씬 종료
	m_spRenderer->EndUsingRenderTargetGroup();

	_UpdateMSAARenderTargetSurface();

	m_spRenderer->BeginUsingRenderTargetGroup( m_pRT_GB_Blur, NiRenderer::CLEAR_ALL );
	ScreenSpace();

	m_pScreen_GB->GetScreenFillingQuad().ApplyAndSetActiveMaterial( "BrightPath" );	
	m_pScreen_GB->GetScreenFillingQuad().RenderImmediate( m_spRenderer );	

	m_pScreen_GB->GetScreenFillingQuad().ApplyAndSetActiveMaterial( "GaussianBlur_2" );
	_SetGaussianBlurParameter( 0.0f, 1.0f/(float)g_nScreenHeight, MAX_GAUSSIANBLUR_SAMPLE );
	m_pScreen_GB->GetScreenFillingQuad().RenderImmediate( m_spRenderer );

	_SetGaussianBlurParameter( 1.0f/(float)g_nScreenWidth, 0.0f, MAX_GAUSSIANBLUR_SAMPLE );
	m_pScreen_GB->GetScreenFillingQuad().RenderImmediate( m_spRenderer );

	// 씬 종료
	m_spRenderer->EndUsingRenderTargetGroup();	

	// 씬시작
	m_spRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );

	m_spRenderer->SetScreenSpaceCameraData();
	m_pScreen_GB->GetScreenFillingQuad().ApplyAndSetActiveMaterial( "GaussianBlur" );
	m_pScreen_GB->GetScreenFillingQuad().RenderImmediate( m_spRenderer );
}


void CEngine::_SetGaussianBlurParameter( float fX, float fY, int nPixelCount )
{
	float totalWeights = _ComputeGaussianBlur( 0 );

	m_GaussianWeights[ 0 ] = totalWeights;
	m_GaussianOffset[ 0 ] = NiPoint2::ZERO;

	for( int i = 0; i<nPixelCount/2; ++i )
	{
		// Store weights for the positive and negative taps.
		float weight = _ComputeGaussianBlur( i + 1.0f );
		m_GaussianWeights[ i*2 + 1 ] = weight;
		m_GaussianWeights[ i*2 + 2 ] = weight;

		totalWeights += weight * 2;

		float sampleOffset = i*2 + 1.5f;

		NiPoint2 delta = NiPoint2( fX * sampleOffset, fY * sampleOffset );
		NiPoint2 delta2 = NiPoint2( -fX * sampleOffset, -fY * sampleOffset );

		// Store texture coordinate offsets for the positive and negative taps.
		m_GaussianOffset[ i*2 + 1 ] = delta;
		m_GaussianOffset[ i*2 + 2 ] = delta2;
	}

	for( int i = 0; i<nPixelCount; ++i )
	{
		m_GaussianWeights[ i ] = m_GaussianWeights[ i ]/totalWeights;
	}

	float nFloat = (float)nPixelCount;
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_sampleCount", sizeof( float ), &nFloat );
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_SampleOffsets", sizeof( NiPoint2 )*nPixelCount, m_GaussianOffset );
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_SampleWeights", sizeof( float )*nPixelCount, m_GaussianWeights );

#ifndef _GIVE
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_GaussianVal", sizeof( float )*4, &m_vGaussianBlurVal );
#endif
}

float CEngine::_ComputeGaussianBlur( float n )
{
	float m_fGaussiantheta = 7.0f;
	if( m_fGaussiantheta <= 0.1f) m_fGaussiantheta = 0.1f;
	else if( m_fGaussiantheta > 7.0f) m_fGaussiantheta = 7.0f;

	float f1 = exp( -(n * n) / (2 * m_fGaussiantheta * m_fGaussiantheta) );
	float f2 = 1.0f / sqrt(2 * NI_PI * m_fGaussiantheta );
	return f1*f2;
}