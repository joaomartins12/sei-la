

#include "stdafx.h"
#include "Engine.h"


void CEngine::_ResetDevice_PostEffect( bool bBefore )
{
	if( m_pRT_Org == NULL )
		return;

	if( bBefore )
	{
		Ni2DBuffer* pkBuffer = m_pRT_Org->GetBuffer(0);
		NiDX9Direct3DBufferData* pkBufferData = (NiDX9Direct3DBufferData*)pkBuffer->GetRendererData();
		pkBufferData->PurgeBufferData();
	}
	else
	{
		switch( m_ePostEffect_Set )
		{
		case PE_GAUSSIAN_BLUR:
			{
				_DeletePostEffect();
				if( _Create_GaussianBlur() == false )
				{
					m_bEnablePostEffect = false;
					SetPostEffect( PE_NONE );
				}
			}
			break;
		case PE_DOF:
			break;
		case PE_NONE:	
			_DeletePostEffect();
			break;
		default:
			assert_cs( false );
		}		
	}
}

void CEngine::_DeletePostEffect()
{
	_Delete_GaussianBlur();
	_Delete_DOF();

	m_pRenderTexture_Org = 0;
	m_pRT_Org = 0;
}

void CEngine::_InitPostEffect()
{
	m_pRenderTexture_Org	=	NULL;
	m_pRT_Org				=	NULL;
	m_pRT_GB_Blur			=	NULL;
	m_pScreen_GB			=	NULL;
	m_pRT_DOF_Blur			=	NULL;
	m_pRT_DOF_Depth			=	NULL;
	m_pScreen_DOF			=	NULL;

	m_bEnablePostEffect		=	true;

	switch( g_pResist->m_Global.s_nPostEffect )
	{
	case 0:
		m_ePostEffect_Set		= PE_NONE;
		break;
	case 1:
		m_ePostEffect_Set		= PE_GAUSSIAN_BLUR;
		break;
	default:
		assert_cs( false );
	}

	
	//m_ePostEffect_Set		= PE_GAUSSIAN_BLUR;
	//m_ePostEffect_Set		= PE_DOF;
}

bool CEngine::_CreateMSAARenderTargetSurface()
{
	assert_cs( m_pRT_Org == NULL );
	// 이미 만들어져 있다.
	if( m_pRT_Org != NULL )
		return true;

	m_pRenderTexture_Org = NiRenderedTexture::Create( g_nScreenWidth, g_nScreenHeight, m_spRenderer );
	if( m_pRenderTexture_Org == NULL )
	{
		return false;
	}	

	NiDX9Renderer* pkDX9Renderer = NiDynamicCast(NiDX9Renderer, m_spRenderer);
	NIASSERT(pkDX9Renderer);

	// Get pixel format information from rendered texture
	D3DFORMAT eFormat = NiDX9PixelFormat::DetermineD3DFormat( *m_pRenderTexture_Org->GetPixelFormat() );

	// Get multisample information from renderer
	unsigned int uiWidth;
	unsigned int uiHeight;
	unsigned int uiUseFlags;
	NiWindowRef kWndDevice;
	NiWindowRef kWndFocus;
	unsigned int uiAdapter;
	NiDX9Renderer::DeviceDesc eDesc;
	NiDX9Renderer::FrameBufferFormat eFBFormat;
	NiDX9Renderer::DepthStencilFormat eDSFormat;
	NiDX9Renderer::PresentationInterval ePresentationInterval;
	NiDX9Renderer::SwapEffect eSwapEffect;
	unsigned int uiFBMode;
	unsigned int uiBackBufferCount; 
	unsigned int uiRefreshRate;

	pkDX9Renderer->GetCreationParameters(uiWidth, uiHeight, uiUseFlags, 
		kWndDevice, kWndFocus, uiAdapter, eDesc, eFBFormat, eDSFormat, 
		ePresentationInterval, eSwapEffect, uiFBMode, uiBackBufferCount, 
		uiRefreshRate);

	//D3DMULTISAMPLE_TYPE eMultisampleType = NiDX9Renderer::GetMultiSampleType(uiFBMode);
	D3DMULTISAMPLE_TYPE eMultisampleType = D3DMULTISAMPLE_NONE;
	m_nGaussianBlurMSAA = g_pResist->m_Global.s_nAntialicing;
	switch( g_pResist->m_Global.s_nAntialicing )
	{
	case NiDX9Renderer::FBMODE_DEFAULT:			eMultisampleType = D3DMULTISAMPLE_NONE;				break;
	case NiDX9Renderer::FBMODE_MULTISAMPLES_2:	eMultisampleType = D3DMULTISAMPLE_2_SAMPLES;		break;
	case NiDX9Renderer::FBMODE_MULTISAMPLES_4:	eMultisampleType = D3DMULTISAMPLE_4_SAMPLES;		break;
	case NiDX9Renderer::FBMODE_MULTISAMPLES_6:	eMultisampleType = D3DMULTISAMPLE_6_SAMPLES;		break;
	case NiDX9Renderer::FBMODE_MULTISAMPLES_8:	eMultisampleType = D3DMULTISAMPLE_8_SAMPLES;		break;
	default:									assert_cs( false );
	}
	unsigned int uiMultisampleQuality = NiDX9Renderer::GetMultiSampleQuality(uiFBMode);

	// Create render target surface
	LPDIRECT3DDEVICE9 pkDevice = pkDX9Renderer->GetD3DDevice();

	LPDIRECT3DSURFACE9 pkRenderSurface = NULL;
	HRESULT hr = E_FAIL;

	hr = pkDevice->CreateRenderTarget( m_pRenderTexture_Org->GetWidth(), m_pRenderTexture_Org->GetHeight(), eFormat, eMultisampleType, 
										uiMultisampleQuality, FALSE, &pkRenderSurface, NULL);

	if( SUCCEEDED(hr) == false )
	{
		m_pRenderTexture_Org = 0;
		return false;
	}

	if( pkRenderSurface == NULL )
	{
		m_pRenderTexture_Org = 0;
		return false;
	}

	Ni2DBuffer* pBuffer = NULL;
	NiDX92DBufferDataPtr pBufferData = NiDX9Direct3DBufferData::Create( pkRenderSurface, pBuffer );
	if( ( pBuffer == NULL ) || ( pBufferData == NULL ) )
	{
		pBufferData = 0;
		if( pBuffer == NULL )
		{
			SAFE_RELEASE( pkRenderSurface );
		}
		else
		{
			SAFE_NIDELETE( pBuffer );
		}
		m_pRenderTexture_Org = 0;
		return false;
	}

	m_pRT_Org = NiRenderTargetGroup::Create( pBuffer, m_spRenderer, false, true );
	if( m_pRT_Org == NULL )
	{
		pBufferData = 0;
		SAFE_NIDELETE( pBuffer );
		m_pRenderTexture_Org = 0;
		return false;
	}

	return true;
}

void CEngine::_UpdateMSAARenderTargetSurface()
{
	NiDX9Renderer* pkDX9Renderer = NiDynamicCast(NiDX9Renderer, m_spRenderer);
	NIASSERT(pkDX9Renderer);

	Ni2DBuffer* pkBuffer = m_pRT_Org->GetBuffer(0);
	NiDX92DBufferData* pkBufferData = (NiDX92DBufferData*)pkBuffer->GetRendererData();
	LPDIRECT3DSURFACE9 pkSourceSurface = pkBufferData->GetSurface();

	NiDX9TextureData* pkTexData = (NiDX9TextureData*)m_pRenderTexture_Org->GetRendererData();
	LPDIRECT3DTEXTURE9 pkD3DTexture = (LPDIRECT3DTEXTURE9)pkTexData->GetD3DTexture();
	LPDIRECT3DSURFACE9 pkDestSurface = 0;
	HRESULT hr = pkD3DTexture->GetSurfaceLevel(0, &pkDestSurface);
	NIASSERT(SUCCEEDED(hr) && pkDestSurface);

	LPDIRECT3DDEVICE9 pkDevice = pkDX9Renderer->GetD3DDevice();
	hr = pkDevice->StretchRect(pkSourceSurface, NULL, pkDestSurface, NULL, D3DTEXF_NONE );
	pkDestSurface->Release();
	NIASSERT(SUCCEEDED(hr));
}

void CEngine::SetPostEffect( ePOST_EFFECT pe )
{
	if( m_bEnablePostEffect == false )
	{
		pe = PE_NONE;
	}

	m_ePostEffect_Set = pe;	
	switch( m_ePostEffect_Set )
	{
	case PE_NONE:
		break;
	case PE_GAUSSIAN_BLUR:
		if( _Create_GaussianBlur() == false )
		{
			m_bEnablePostEffect = false;
			SetPostEffect( PE_NONE );
		}
		break;
	case PE_DOF:
		if( _Create_DOF() == false )
		{
			m_bEnablePostEffect = false;
			SetPostEffect( PE_NONE );
		}
		break;
	default:
		assert_cs( false );
	}
}

NiRenderTargetGroup* CEngine::GetRenderTargetGroup( ePOST_EFFECT pe )
{
	switch( pe )
	{
	case PE_NONE:
		return m_spRenderer->GetDefaultRenderTargetGroup();
	case PE_GAUSSIAN_BLUR:
	case PE_DOF:
		return m_pRT_Org;		
	}

	assert_cs( false );
	return m_spRenderer->GetDefaultRenderTargetGroup();
}

NiRenderTargetGroup* CEngine::GetCurrentRenderTargetGroup()
{
	switch( m_ePostEffect_Set )
	{
	case PE_NONE:
		return m_spRenderer->GetDefaultRenderTargetGroup();
	case PE_GAUSSIAN_BLUR:
	case PE_DOF:
		return m_pRT_Org;		
	}

	assert_cs( false );
	return m_spRenderer->GetDefaultRenderTargetGroup();
}