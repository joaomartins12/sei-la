
#include "stdafx.h"
#include "Render.h"

NiWireframePropertyPtr		cRender::m_pPropWire = NULL;
NiZBufferPropertyPtr		cRender::m_pPropZ = NULL;
NiVertexColorPropertyPtr	cRender::m_pPropVertColor = NULL;
NiAlphaPropertyPtr			cRender::m_pPropAlpha = NULL;

void cRender::Delete()
{
	m_pPropWire = 0;
	m_pPropZ = 0;
	m_pPropVertColor = 0;
	m_pPropAlpha = 0;
}

void cRender::Init()
{
	m_pPropWire = NiNew NiWireframeProperty;
	m_pPropWire->SetWireframe( true );	

	m_pPropZ = NiNew NiZBufferProperty();
	m_pPropZ->SetZBufferTest(true);
	m_pPropZ->SetZBufferWrite(false);
	m_pPropZ->SetTestFunction(NiZBufferProperty::TEST_LESSEQUAL);
	
	m_pPropVertColor = NiNew NiVertexColorProperty;
	m_pPropVertColor->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
	m_pPropVertColor->SetLightingMode(NiVertexColorProperty::LIGHTING_E);

	m_pPropAlpha = NiNew NiAlphaProperty;
	m_pPropAlpha->SetAlphaBlending( true );
	m_pPropAlpha->SetAlphaTesting( false );

	//m_pPropVertColor->SetLightingMode(NiVertexColorProperty::LIGHTING_E);
	//m_pPropMaterial = NiNew NiMaterialProperty;
	//m_pPropMaterial->SetEmittance(NiColor(1.0f, 1.0f, 0.0f));
}

void cRender::RenderWireFrame( NiNode* pNode )
{
	NiTriStrips* pObject = (NiTriStrips*)pNode->GetAt( 0 );
	assert_cs( NiIsKindOf( NiTriStrips, pObject ) == true ); 
	
	pObject->AttachProperty( m_pPropWire );
	pObject->AttachProperty( m_pPropZ );
	pObject->AttachProperty( m_pPropVertColor );
	NiPropertyPtr pTexProp = pObject->RemoveProperty( NiTexturingProperty::GetType() );
	pNode->UpdateProperties();

	NiMaterial* pMaterial = (NiMaterial*)pObject->GetActiveMaterial();
	if( pMaterial != NULL )
	{
		pObject->ApplyAndSetActiveMaterial( NULL );
		pNode->UpdateEffects();
	}
	
	NiDrawScene( CAMERA_ST.GetCameraObj(), pNode, *g_pCuller );
	
	pObject->DetachProperty( m_pPropWire );
	pObject->DetachProperty( m_pPropZ );
	pObject->DetachProperty( m_pPropVertColor );

	if( pTexProp != NULL )
		pObject->AttachProperty( pTexProp );
	pNode->UpdateProperties();
	
	if( pMaterial != NULL )
	{
		pObject->ApplyAndSetActiveMaterial( pMaterial );
		pNode->UpdateEffects();
	}
}

void cRender::RenderToTexture( eR2T ert )
{
	//g_MapMng.Update_ByTool( 0.0f );
	nsCsGBTerrain::g_pCurRoot->GetRootNode()->GetNiNode()->Update( 0.0f, false );
	nsCsGBTerrain::g_pCurRoot->GetRootNode()->UpdateTerrain( 0.0f );
	nsCsGBTerrain::g_pCurRoot->GetRootNode()->UpdateObject( 0.0f );
	nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->Update_ByTool( 0.0f );

	g_pRenderer->BeginOffScreenFrame();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );

	g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	g_MapMng.BeginRender();

	// 지형 및 오브젝트 랜더링
	switch( ert )
	{
	case R2T_DETAILMAP:
		{

			g_MapMng.RenderObject_ByTool( g_fElapsedTime, true );
			g_MapMng.RenderTerrain_ByTool( GET_MV_MAINTAB_DLG->_GetDlgPlayer()->_IsShowSkyBos() );
		}
		break;
	case R2T_BASEMAP:
		{			
			if( nsCsGBTerrain::g_pCurRoot->GetRenderObject_ByTool() == true )
			{
				bool bFog = nsCsGBTerrain::g_pCurRoot->GetFogProperty()->GetFog();
				nsCsGBTerrain::g_pCurRoot->GetFogProperty()->SetFog( false );
				g_MapMng.RenderTerrain();
				nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_FILELOAD_BYTOOL;				
				g_MapMng.RenderObject_ByTool( g_fElapsedTime );
				g_MapMng.RenderTerrain_ByTool( GET_MV_MAINTAB_DLG->_GetDlgPlayer()->_IsShowSkyBos() );
				nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_FILELOAD;
				nsCsGBTerrain::g_pCurRoot->GetFogProperty()->SetFog( bFog );
			}
			else
			{
				nsCsGBTerrain::g_pCurRoot->GetRootNode()->Render();
			}
		}
		break;
	}
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );
	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->EndOffScreenFrame();

	g_MapMng.EndRender();
}

