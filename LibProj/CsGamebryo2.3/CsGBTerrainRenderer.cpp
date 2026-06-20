
#include "stdafx.h"
#include "CsGBTerrainRenderer.h"

//---------------------------------------------------------------------------
void CsNiDrawTerrainScene()
{
	CsGBTerrainNode* pRoot = nsCsGBTerrain::g_pCurRoot->GetRootNode();	
	for( int i=0; i<4; ++i )
	{
		if( pRoot->IsVisible() == true )
		{
			CsNiDrawTerrainScene( pRoot->GetChildNode( i ) );
		}		
	}
}

void CsNiDrawTerrainScene( CsGBTerrainNode* pNode )
{
	if( pNode->IsEnableRender() == false )
		return;

	if( pNode->IsVisible() == false )
		return;

	if( pNode->IsLeaf() == true )
	{
		CsGBTerrainLeaf* pLeaf = (CsGBTerrainLeaf*)pNode;
#ifdef TERRAIN_RENDER_DEEPCOPY
#else
		pLeaf->UpdateBuffer();
#endif
		pLeaf->GetNiTriStrip()->RenderImmediate( NiRenderer::GetRenderer() );
	}
	else
	{
		for( int i=0; i<4; ++i )
		{
			CsNiDrawTerrainScene( pNode->GetChildNode( i ) );
		}
	}
}

void CsNiDrawTerrainScene( char* cShader )
{
	CsGBTerrainNode* pRoot = nsCsGBTerrain::g_pCurRoot->GetRootNode();	
	for( int i=0; i<4; ++i )
	{
		if( pRoot->IsVisible() == true )
		{
			CsNiDrawTerrainScene( pRoot->GetChildNode( i ), cShader );
		}		
	}
}

void CsNiDrawTerrainScene( CsGBTerrainNode* pNode, char* cShader )
{
	if( pNode->IsEnableRender() == false )
		return;

	if( pNode->IsVisible() == false )
		return;

	if( pNode->IsLeaf() == true )
	{
		CsGBTerrainLeaf* pLeaf = (CsGBTerrainLeaf*)pNode;

		//NiMaterial* pOldMat = (NiMaterial*)pLeaf->GetNiTriStrip()->GetActiveMaterial();
		pLeaf->GetNiTriStrip()->ApplyAndSetActiveMaterial( cShader );		
#ifdef TERRAIN_RENDER_DEEPCOPY
#else
	pLeaf->UpdateBuffer();
#endif
		pLeaf->GetNiTriStrip()->RenderImmediate( NiRenderer::GetRenderer() );

		//pLeaf->GetNiTriStrip()->SetActiveMaterial( pOldMat );
	}
	else
	{
		for( int i=0; i<4; ++i )
		{
			CsNiDrawTerrainScene( pNode->GetChildNode( i ), cShader );
		}
	}
}

//void CsNiDrawTerrainScene_ByTool(NiCamera* pkCamera, NiAVObject* pkScene, NiCullingProcess& kCuller )
//{
//	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
//	assert_cs( pkRenderer != NULL );
//
//	// ===== Cull
//	// Set up the renderer's camera data.
//	pkRenderer->SetCameraData(pkCamera);
//
//	// Construct the potentially visible set.
//	NiVisibleArray* pkVisibleSet = kCuller.GetVisibleSet();
//	assert_cs( pkVisibleSet != NULL );	
//	pkVisibleSet->RemoveAll();
//	kCuller.Process( pkCamera, pkScene, pkVisibleSet );
//
//	// ===== Visible Append	
//	const unsigned int uiQuantity = pkVisibleSet->GetCount();
//	unsigned int i;
//	NiTriStrips* pTriObject = NULL;
//	for (i = 0; i < uiQuantity; i++)
//	{
//		pTriObject = (NiTriStrips*)&pkVisibleSet->GetAt(i);
//		assert_cs( NiIsKindOf( NiTriStrips, pTriObject ) );		
//		assert_cs( NiIsKindOf( CsNiNode, pTriObject->GetParent() ) );
//		pTriObject->RenderImmediate( nsCsGBTerrain::g_pRenderer );
//	}
//	pkVisibleSet->RemoveAll();
//}
//

