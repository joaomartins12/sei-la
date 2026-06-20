
#include "stdafx.h"

#include "NiD3DRendererPCH.h"
#include "NiDX9TextureManager.h"
#include "NiD3DGeometryGroupManager.h"
#include "NiDynamicGeometryGroup.h"
#include "NiUnsharedGeometryGroup.h"
#include "NiDX9PersistentSrcTextureRendererData.h"

#include "CsGBDX9Renderer.h"

//---------------------------------------------------------------------------
CsGBDX9Renderer* CsGBDX9Renderer::CsGBCreate(unsigned int uiWidth, 
										 unsigned int uiHeight, unsigned int uiUseFlags, NiWindowRef kWndDevice, 
										 NiWindowRef kWndFocus, unsigned int uiAdapter, DeviceDesc eDesc, 
										 FrameBufferFormat eFBFormat, DepthStencilFormat eDSFormat, 
										 PresentationInterval ePresentationInterval, SwapEffect eSwapEffect, 
										 unsigned int uiFBMode, unsigned int uiBackBufferCount, 
										 unsigned int uiRefreshRate)
{
	// Create the Direct3D9 interface
	if (CreateDirect3D9() == -1)
	{
		SetLastErrorString("Creation failed: Could not initialize DirectX9");
		Error("NiDX9Renderer::Create> Invalid device ID specified "
			"- FAILING\n");
		return NULL;
	}

	// Create an 'empty' instance of the renderer
	CsGBDX9Renderer* pkR = NiNew CsGBDX9Renderer;
	NIASSERT(pkR);

	ms_kD3DCriticalSection.Lock();

	bool bSuccess = pkR->Initialize(uiWidth, uiHeight, uiUseFlags, kWndDevice, 
		kWndFocus, uiAdapter, eDesc, eFBFormat, eDSFormat, 
		ePresentationInterval, eSwapEffect, uiFBMode, uiBackBufferCount, 
 		uiRefreshRate);

	ms_kD3DCriticalSection.Unlock();

	if (bSuccess == false)
	{
		NiDelete pkR;
		pkR = NULL;
	}
	else
	{
		// Registration of the DX9-specific
		// NiDX9PersistentSrcTextureRendererData::LoadBinary() that overrides
		// NiPersistentSrcTextureRendererData::LoadBinary();
		NiStream::UnregisterLoader("NiPersistentSrcTextureRendererData");
		NiStream::RegisterLoader("NiPersistentSrcTextureRendererData", NiDX9PersistentSrcTextureRendererData::CreateObject);
	}


	return pkR;
}
//---------------------------------------------------------------------------
//
//void CsGBDX9Renderer::Do_RenderTristrips( NiTriStrips* pkTriStrips )
//{
//	NIMETRICS_DX9RENDERER_SCOPETIMER(DRAW_TIME_STRIPS);
//
//	NiTriStripsData* pkData = (NiTriStripsData*)pkTriStrips->GetModelData();
//	NIASSERT(pkData);
//
//	unsigned int uiActiveVertexCount = pkData->GetActiveVertexCount();
//	if (m_bDeviceLost || uiActiveVertexCount == 0)
//		return;
//
//	// Unskinned
//	NiGeometryGroup* pkGeometryGroup = m_pkDefaultGroup;
//	if (pkData->GetConsistency() == NiGeometryData::VOLATILE)
//		pkGeometryGroup = m_pkDynamicGroup;
//
//	m_pkGeometryGroupManager->AddObjectToGroup(pkGeometryGroup,
//		pkData, NULL, false, 0, 0);
//
//	_DrawPrimitive( pkTriStrips, pkData, NULL, 
//					pkTriStrips->GetWorldTransform(), pkTriStrips->GetWorldBound(),
//					(NiGeometryBufferData*)pkData->GetRendererData() );
//}
//
//void CsGBDX9Renderer::_DrawPrimitive( NiGeometry* pkGeometry, NiGeometryData* pkData, NiSkinInstance* pkSkinInstance,
//										const NiTransform& kWorld, const NiBound& kWorldBound, NiGeometryBufferData* pkBuffData)
//{
//	NIASSERT(!m_bDeviceLost);
//
//	NiD3DShaderInterface* pkShader = NULL;
//	assert_cs( pkGeometry != NULL );
//
//	
//	pkShader = NiDynamicCast(NiD3DShaderInterface, pkGeometry->GetShaderFromMaterial());
//	if (pkShader == 0)
//	{
//		const NiMaterial* pkTempMat = pkGeometry->GetActiveMaterial();
//		pkGeometry->ApplyAndSetActiveMaterial(m_spCurrentDefaultMaterial);
//		pkShader = NiDynamicCast(NiD3DShaderInterface, pkGeometry->GetShaderFromMaterial());
//		pkGeometry->SetActiveMaterial(pkTempMat);
//	}
//	
//	
//
//	// If no shader is found, use error shader
//	assert_cs( pkShader != NULL );
//	/*{
//		Error("DrawPrimitive> No shader found for object %s!\n"
//			"Using Error Shader!\n", pkGeometry->GetName());
//		NIASSERT(NiIsKindOf(NiD3DShaderInterface, GetErrorShader()));
//		pkShader = (NiD3DShaderInterface*)GetErrorShader();
//	}*/
//
//	NIASSERT(m_pkCurrProp);
//
//	unsigned int uiRet;
//
//	// Preprocess the pipeline
//	pkShader->PreProcessPipeline(pkGeometry, pkSkinInstance, pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);
//	// Update the pipeline
//	pkShader->UpdatePipeline(pkGeometry, pkSkinInstance, pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);
//
//	unsigned int uiRemainingPasses = pkShader->FirstPass();
//
//	unsigned int uiMetricDPCalls = 0;
//
//	//uiPasses
//	while (uiRemainingPasses != 0)
//	{
//		// Setup the rendering pass
//		uiRet = pkShader->SetupRenderingPass(pkGeometry, pkSkinInstance, pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);
//
//		// Set the transformations
//		uiRet = pkShader->SetupTransformations(pkGeometry, pkSkinInstance, NULL, pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);
//
//		// Prepare the geometry for rendering
//		pkBuffData = (NiGeometryBufferData*)pkShader->PrepareGeometryForRendering(pkGeometry, NULL, pkBuffData, m_pkCurrProp);
//
//		// Set the shader programs
//		// This is to give the shader final 'override' authority
//		uiRet = pkShader->SetupShaderPrograms(pkGeometry, pkSkinInstance, NULL,	pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);
//
//		m_pkRenderState->CommitShaderConstants();
//
//		if (pkBuffData->GetIB())
//		{
//			unsigned int uiStartIndex = 0;
//			for (unsigned int i = 0; i < pkBuffData->GetNumArrays(); i++)
//			{
//				unsigned int uiPrimitiveCount;
//				const unsigned short* pusArrayLengths = 
//					pkBuffData->GetArrayLengths();
//				if (pusArrayLengths)
//				{
//					// Assumes multiple arrays mean strips
//					uiPrimitiveCount = pusArrayLengths[i] - 2;
//				}
//				else
//				{
//					NIASSERT(pkBuffData->GetNumArrays() == 1);
//					uiPrimitiveCount = pkBuffData->GetTriCount();
//				}
//
//				m_pkD3DDevice9->DrawIndexedPrimitive(pkBuffData->GetType(), 
//					pkBuffData->GetBaseVertexIndex(), 0, 
//					pkBuffData->GetVertCount(), uiStartIndex, 
//					uiPrimitiveCount);
//
//				uiStartIndex += uiPrimitiveCount + 2;
//
//				uiMetricDPCalls++;
//				NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_TRIS, 
//					uiPrimitiveCount);
//				NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
//					pkBuffData->GetVertCount());
//			}
//		}
//		else
//		{
//			m_pkD3DDevice9->DrawPrimitive(pkBuffData->GetType(), 
//				pkBuffData->GetBaseVertexIndex(), pkBuffData->GetTriCount());
//
//			uiMetricDPCalls++;
//			NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_TRIS, 
//				pkBuffData->GetTriCount());
//			NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_VERTS, 
//				pkBuffData->GetTriCount() * 3);
//		}
//
//		// Perform any post-rendering steps
//		uiRet = pkShader->PostRender(pkGeometry, pkSkinInstance, NULL, 
//			pkBuffData, m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);
//
//		// Inform the shader to move to the next pass
//		uiRemainingPasses = pkShader->NextPass();
//	}
//
//	// PostProcess the pipeline
//	pkShader->PostProcessPipeline(pkGeometry, pkSkinInstance, pkBuffData, 
//		m_pkCurrProp, m_pkCurrEffects, kWorld, kWorldBound);
//
//	// Reset the normalize normals flag
//	m_pkRenderState->SetInternalNormalizeNormals(false);
//
//	NIMETRICS_DX9RENDERER_AGGREGATEVALUE(DRAW_PRIMITIVE, uiMetricDPCalls);
//}



