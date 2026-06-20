
#include "stdafx.h"
#include "Engine.h"


bool CEngine::StartRender( ePOST_EFFECT PostEffectStart )
{
	if( m_spRenderer->IsDeviceLost() == true )
	{
		ResetDevice();
		return false;
	}

	assert_csm1( m_spRenderer->GetFrameState() == NiRenderer::FRAMESTATE_OUTSIDE_FRAME, _T( "%d" ), m_spRenderer->GetFrameState() );
	assert_cs( g_Sorting.empty() == true );

	m_bScreenSpace = false;
	if( m_bOffScreen == false )
	{
		if( m_spRenderer->BeginFrame() == false )
		{
			//assert_cs( false );
			return false;
		}
	}
	else
	{
		if( m_spRenderer->BeginOffScreenFrame() == false )
		{
			//assert_cs( false );
			return false;
		}
	}

	m_ePostEffect_Start = PostEffectStart;

	bool bSuccess = m_spRenderer->BeginUsingRenderTargetGroup( GetRenderTargetGroup( m_ePostEffect_Start ), NiRenderer::CLEAR_ALL );
	assert_cs( bSuccess );
	if( !bSuccess )
		return false;

	m_spRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	// 셋팅된 값 얻어 놓는다 - 쉐이더에서 쓰임
	m_matView = m_spRenderer->GetD3DView();
	m_matProj = m_spRenderer->GetD3DProj();
	D3DXMatrixMultiply( &m_matViewProj, &m_matView, &m_matProj );
	m_spRenderer->GetD3DDevice()->GetViewport( &m_ViewPort );

	return true;
}

void CEngine::Render()
{
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );

	switch( m_ePostEffect_Set )
	{
	case PE_GAUSSIAN_BLUR:	_Render_PE_GB();	break;
	case PE_DOF:			_Render_PE_DOF();	break;
	case PE_NONE:			break;
	default:				assert_cs( false );	break;
	}
}

bool CEngine::EndRender()
{
// 	if( g_pUserGrade )
// 	{
// 		if( g_pUserGrade->IsEnd() == false )
// 		{
// 			ScreenSpace();
// 			g_pUserGrade->Render();
// 		}
// 		else
// 		{
// 			SAFE_NIDELETE( g_pUserGrade );
// 		}
// 	}
// 	if( g_pFade )
// 	{
// 		ScreenSpace();
// 		if( g_pFade->Render() == false )
// 		{			
// 			SAFE_DELETE( g_pFade );
// 		}
// 	}	

	m_spRenderer->EndUsingRenderTargetGroup();

	// 반디캠 녹화
// #ifdef MOVIECAPTURE
// 	MovieCapture::GetInstance().RecordRender(m_spRenderer->GetD3DDevice());
// #endif

	if( m_bOffScreen == false )
	{
		if( m_spRenderer->EndFrame() == false )
		{
			//assert_cs( false );
			return false;
		}
		if( m_spRenderer->DisplayFrame() == false )
		{
			assert_cs( false );
			return false;
		}		
	}
	else
	{
		if( m_spRenderer->EndOffScreenFrame() == false )
		{
			assert_cs( false );
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
NiDX9Renderer* CEngine::GetDX9Renderer()
{
	NiDX9Renderer* pkD3DRenderer = NiDynamicCast(NiDX9Renderer, NiDX9Renderer::GetRenderer());
	assert(pkD3DRenderer && "Created Renderer Failed!");
	return pkD3DRenderer;
}
//---------------------------------------------------------------------------
LPDIRECT3DDEVICE9 CEngine::GetD3DDevice()
{
	NiDX9Renderer* pkD3DRenderer = GetDX9Renderer();
	return pkD3DRenderer->GetD3DDevice();
}
//---------------------------------------------------------------------------
LPDIRECT3D9 CEngine::GetD3D()
{
	NiDX9Renderer* pkD3DRenderer = GetDX9Renderer();
	return pkD3DRenderer->GetDirect3D();
}

//---------------------------------------------------------------------------
void CEngine::SetClearColor(NiColorA p_ClearColor)
{
	if( m_spRenderer )
		m_spRenderer->SetBackgroundColor(p_ClearColor);
	else
		NiDX9Renderer::GetRenderer()->SetBackgroundColor(p_ClearColor);
}
//---------------------------------------------------------------------------
void CEngine::Clear()
{
	LPDIRECT3DDEVICE9 pd3dDevice = GetD3DDevice();
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
		D3DCOLOR_XRGB(128,128,128), 1.0f, 0);
}
//---------------------------------------------------------------------------
bool CEngine::BeginFrame()
{
	if( m_eDeviceStatus == NiDX9Renderer::RECREATESTATUS_FAILED)
		return false;

	// BeginScene
	assert(m_spRenderer && "Created Renderer Failed!(BeginFrame)");
	return m_spRenderer->BeginFrame();
}
//---------------------------------------------------------------------------
bool CEngine::BeginUsingRenderTargetGroup(NiRenderTargetGroup* pGroup, NiRenderer::ClearFlags p_eFlags)
{
	// RenderTarget - Begin
	assert(m_spRenderer && "Created Renderer Failed!(BeginRenderTarget)");
	return m_spRenderer->BeginUsingRenderTargetGroup(pGroup, p_eFlags);
}
//---------------------------------------------------------------------------
bool CEngine::BeginUsingCurrentRenderTargetGroup(NiRenderer::ClearFlags p_eFlags)
{
	// RenderTarget - Begin
	assert(m_spRenderer && "Created Renderer Failed!(BeginRenderTarget)");
	//return m_spRenderer->BeginUsingRenderTargetGroup(GetRenderTargetGroup(m_ePostEffect_Set), p_eFlags);
	bool bResult = m_spRenderer->BeginUsingRenderTargetGroup(GetRenderTargetGroup(m_ePostEffect_Set), p_eFlags);
	if( bResult )
		m_spRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	// 셋팅된 값 얻어 놓는다 - 쉐이더에서 쓰임
	m_matView = m_spRenderer->GetD3DView();
	m_matProj = m_spRenderer->GetD3DProj();
	D3DXMatrixMultiply( &m_matViewProj, &m_matView, &m_matProj );
	m_spRenderer->GetD3DDevice()->GetViewport( &m_ViewPort );
	return bResult;
}
//---------------------------------------------------------------------------
bool CEngine::BeginDefaultRenderTarget(NiRenderer::ClearFlags p_eFlags)
{
	// RenderTarget - Begin
	assert(m_spRenderer && "Created Renderer Failed!(BeginRenderTarget)");
	bool bResult = m_spRenderer->BeginUsingDefaultRenderTargetGroup(p_eFlags);
	if( bResult )
		m_spRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	// 셋팅된 값 얻어 놓는다 - 쉐이더에서 쓰임
	m_matView = m_spRenderer->GetD3DView();
	m_matProj = m_spRenderer->GetD3DProj();
	D3DXMatrixMultiply( &m_matViewProj, &m_matView, &m_matProj );
	m_spRenderer->GetD3DDevice()->GetViewport( &m_ViewPort );
	return bResult;
}
//---------------------------------------------------------------------------
bool CEngine::EndRenderTarget()
{
	// RenderTarget - End
	assert(m_spRenderer && "Created Renderer Failed!(EndRenderTarget)");
	return m_spRenderer->EndUsingRenderTargetGroup();
}
//---------------------------------------------------------------------------
bool CEngine::EndFrame()
{
	// EndScene
	assert(m_spRenderer && "Created Renderer Failed!(EndFrame)");
	return m_spRenderer->EndFrame();
}
//---------------------------------------------------------------------------
bool CEngine::DisplayFrame()
{
	// Present
	assert(m_spRenderer && "Created Renderer Failed!(DisplayFrame)");
	return m_spRenderer->DisplayFrame();
}
//---------------------------------------------------------------------------
bool CEngine::BeginOffScreenFrame()
{
	// Present
	assert(m_spRenderer && "Created Renderer Failed!(BeginOffScreenFrame)");
	return m_spRenderer->BeginOffScreenFrame();
}
//---------------------------------------------------------------------------
bool CEngine::EndOffScreenFrame()
{
	// Present
	assert(m_spRenderer && "Created Renderer Failed!(EndOffScreenFrame)");
	return m_spRenderer->EndOffScreenFrame();
}
void CEngine::ScreenSpace()
{ 
	//if( m_bScreenSpace == false )
		m_spRenderer->SetScreenSpaceCameraData(); 
	//else
	//	m_bScreenSpace = true; 
}