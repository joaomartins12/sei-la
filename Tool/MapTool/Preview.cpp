
#include "stdafx.h"
#include "Preview.h"

cPreview		g_Preview;

void cPreview::Delete()
{
	m_Char.Delete();
	m_TObj.Delete();
	m_Texture.Delete();

	//m_PreviewCamera.Delete();

	m_pRenderTarget = 0;
	m_pScreenElement = 0;	
}

void cPreview::Init()
{
	sCAMERAINFO ci;
	ci.s_fDist = 400.0f;
	ci.s_fFarPlane = 100000.0f;
	ci.s_fInitPitch = 0.0f;	
	ci.s_ptInitPos = NiPoint3( 0.0f, 0.0f, 0.0f );

	GET_SUBCAMERA(CAMERA_02)->Reset( &ci );
	GET_SUBCAMERA(CAMERA_02)->ChangeStandardFov( 200.0f, 200.0f );
	GET_SUBCAMERA(CAMERA_01)->_UpdateCamera();

	//m_PreviewCamera.Create( g_pRenderer, &ci );
	//m_PreviewCamera.ChangeStandardFov( 200.0f, 200.0f );
	//m_PreviewCamera._UpdateCamera();

#define PRE_VIEW_TEX_SIZE		150.0f
	NiRenderedTexture* pRenderTexture = NiRenderedTexture::Create( (UINT)PRE_VIEW_TEX_SIZE, (UINT)PRE_VIEW_TEX_SIZE, g_pRenderer);
	m_pRenderTarget = NiRenderTargetGroup::Create( pRenderTexture->GetBuffer(), g_pRenderer, true, true );
	NiTexturingPropertyPtr pTexProp = NiNew NiTexturingProperty;
	pTexProp->SetBaseTexture( pRenderTexture );
	pTexProp->SetApplyMode( NiTexturingProperty::APPLY_REPLACE );
	m_pScreenElement = NiNew NiScreenElements( NiNew NiScreenElementsData( true, true, 1 ) );
	m_pScreenElement->UpdateBound();
	int nScreenHandel = m_pScreenElement->Insert( 4 );
	m_pScreenElement->SetTextures( nScreenHandel, 0, 0.0f, 0.0f, 1.0f, 1.0f );
	m_pScreenElement->SetRectangle( nScreenHandel, (VIEWER_WIDTH-PRE_VIEW_TEX_SIZE)/(float)VIEWER_WIDTH,
									0.0f,
									PRE_VIEW_TEX_SIZE/(float)VIEWER_WIDTH,
									PRE_VIEW_TEX_SIZE/(float)VIEWER_HEIGHT );
	NiAlphaPropertyPtr pAlphaProp = NiNew NiAlphaProperty;
	pAlphaProp->SetAlphaBlending( true );
	pAlphaProp->SetAlphaTesting( true );
	m_pScreenElement->AttachProperty( pTexProp );
	m_pScreenElement->AttachProperty( pAlphaProp );
	m_pScreenElement->UpdateProperties();
	m_pScreenElement->Update( 0.0f );


	m_Char.Init();
	m_TObj.Init();
	m_Texture.Init();
}

void cPreview::PreViewReset()
{
}

void cPreview::PostViewReset()
{

}

void cPreview::Update()
{
	if( m_pCurPreview == NULL )
		return;

	m_pCurPreview->Update( GET_SUBCAMERA(CAMERA_02) );
}

void cPreview::OnMainViewerSizeChange(int const& x, int const& y)
{
	if( m_pScreenElement )
	{
		m_pScreenElement->SetRectangle( 0, (x-PRE_VIEW_TEX_SIZE)/(float)x,	0.0f,	PRE_VIEW_TEX_SIZE/(float)x,	PRE_VIEW_TEX_SIZE/(float)y );
		m_pScreenElement->Update( 0.0f );
	}
}

void cPreview::Render()
{
	if( m_pCurPreview == NULL )
		return;

	if( m_pCurPreview->IsRender() == false )
		return;

	g_pRenderer->EndUsingRenderTargetGroup();

	// 텍스쳐에 랜더링
	g_pRenderer->SetBackgroundColor( NiColorA( 0.0f, 0.0f, 0.0f, 0.0f ) );
	g_pRenderer->BeginUsingRenderTargetGroup( m_pRenderTarget, NiRenderer::CLEAR_ALL );

	m_pCurPreview->Render( GET_SUBCAMERA(CAMERA_02) );

	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_NONE );
	g_pRenderer->SetScreenSpaceCameraData();
	// 텍스쳐 랜더링
	m_pScreenElement->Draw( g_pRenderer );

	g_pRenderer->SetBackgroundColor( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_BackGroundColor );
}

void cPreview::SetPreview( cPreviewObj::ePREVIEW_OBJ_TYPE eType )
{
	switch( eType )
	{
	case cPreviewObj::POT_NONE:
		m_pCurPreview = NULL;
		break;
	case cPreviewObj::POT_CHAR:
		m_pCurPreview = &m_Char;
		break;
	case cPreviewObj::POT_TERRAIN_OBJ:
		m_pCurPreview = &m_TObj;
		break;
	case cPreviewObj::POT_TEXTURE:
		m_pCurPreview = &m_Texture;
		break;
	default:
		assert_cs( false );
	}
}

