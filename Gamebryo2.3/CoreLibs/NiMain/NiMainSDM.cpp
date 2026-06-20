// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

// Precompiled Header
#include "NiMainPCH.h"

#include "NiMain.h"
#include "NiMainSDM.h"
#include "NiOldScreenSpace.h"
#include "NiGlobalStringTable.h"

// Implement constructor.
bool NiMainSDM::ms_bInitialized = false;
NiMainSDM::NiMainSDM()
{
    static int siCounter = 0;
    if (siCounter++ == 0)
    {
        NiStaticDataManager::SetRootLibrary(NiMainSDM::Init,
            NiMainSDM::Shutdown);
    }
}


//---------------------------------------------------------------------------
#ifdef NIMAIN_EXPORT
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ulReason, LPVOID lpReserved) \
{
    switch (ulReason)
    {
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_PROCESS_DETACH:
            NiStaticDataManager::SetRootLibrary(NULL, NULL);
            break;
    }
    return TRUE;
}
#endif
//---------------------------------------------------------------------------
void NiMainSDM::Init()
{
    NiImplementSDMInitCheck();
    NiGlobalStringTable::_SDMInit();

    // init default properties
    NiShaderConstantMap::_SDMInit();
    NiTextureEffect::_SDMInit();
    NiAlphaProperty::_SDMInit();
    NiDitherProperty::_SDMInit();
    NiFogProperty::_SDMInit();
    NiMaterialProperty::_SDMInit();
    NiMetricsLayer::_SDMInit();
    NiRendererSpecificProperty::_SDMInit();
    NiShadeProperty::_SDMInit();
    NiShaderConstantMapEntry::_SDMInit();
    NiShaderDeclaration::_SDMInit();
    NiSpecularProperty::_SDMInit();
    NiStencilProperty::_SDMInit();
    NiTexturingProperty::_SDMInit();
    NiVertexColorProperty::_SDMInit();
    NiWireframeProperty::_SDMInit();
    NiZBufferProperty::_SDMInit();

    NiImageConverter::_SDMInit();

    NiStream::_SDMInit();

    NiRegisterStream(NiAdditionalGeometryData);
    NiRegisterStream(NiAlphaAccumulator);
    NiRegisterStream(NiAlphaProperty);
    NiRegisterStream(NiAmbientLight);

    // NiAutoNormalParticles has been removed from the engine
    // Because it never added any data to its base class, we can
    // simply instantiate the base class in streaming.
//    NiRegisterStream(NiAutoNormalParticles);
    NiStream::RegisterLoader("NiAutoNormalParticles", 
        NiParticles::CreateObject);

    // NiAutoNormalParticlesData has been removed from the engine
    // Because it never added any data to its base class, we can
    // simply instantiate the base class in streaming.
//    NiRegisterStream(NiAutoNormalParticlesData);
    NiStream::RegisterLoader("NiAutoNormalParticlesData", 
        NiParticlesData::CreateObject);

    NiRegisterStream(NiBillboardNode);
    NiRegisterStream(NiBinaryExtraData);
    NiRegisterStream(NiBooleanExtraData);
    NiRegisterStream(NiBSPNode);
    NiRegisterStream(NiCamera);

    // NiClusterAccumulator has been removed from the engine
//    NiRegisterStream(NiClusterAccumulator);
    NiStream::RegisterLoader("NiClusterAccumulator", 
        NiAlphaAccumulator::CreateObject);
    
    // NiCollisionSwitch has been removed from the engine
    // Its behavior can be achieved with collision propagation flags
    // in NiAVObject.
//    NiRegisterStream(NiCollisionSwitch);
    NiStream::RegisterLoader("NiCollisionSwitch", NiNode::CreateObject);

    NiRegisterStream(NiColorExtraData);
    NiRegisterStream(NiDefaultAVObjectPalette);
    NiRegisterStream(NiDirectionalLight);
    NiRegisterStream(NiDitherProperty);
    NiRegisterStream(NiExtraData);
    NiRegisterStream(NiFloatExtraData);
    NiRegisterStream(NiFloatsExtraData);
    NiRegisterStream(NiFogProperty);
    NiRegisterStream(NiIntegerExtraData);
    NiRegisterStream(NiIntegersExtraData);
    NiRegisterStream(NiLODNode);
    NiRegisterStream(NiLines);
    NiRegisterStream(NiLinesData);
    NiRegisterStream(NiMaterialProperty);
    NiRegisterStream(NiNode);
    NiRegisterStream(NiPalette);
    NiRegisterStream(NiParticles);
    NiRegisterStream(NiParticlesData);
    NiRegisterStream(NiPersistentSrcTextureRendererData);
    NiRegisterStream(NiPixelData);
    NiRegisterStream(NiPointLight);
    NiRegisterStream(NiRangeLODData);
    NiRegisterStream(NiRendererSpecificProperty);
    NiRegisterStream(NiScreenLODData);
    NiRegisterStream(NiScreenElements);
    NiRegisterStream(NiScreenElementsData);
    NiRegisterStream(NiScreenTexture);
    NiRegisterStream(NiShadeProperty);
    NiRegisterStream(NiSkinData);
    NiRegisterStream(NiSkinInstance);
    NiRegisterStream(NiSkinPartition);
    NiRegisterStream(NiSortAdjustNode);
    NiRegisterStream(NiSourceTexture);
    NiRegisterStream(NiSourceCubeMap);
    NiRegisterStream(NiSpecularProperty);
    NiRegisterStream(NiSpotLight);
    NiRegisterStream(NiStencilProperty);
    NiRegisterStream(NiStringExtraData);
    NiRegisterStream(NiStringsExtraData);
    NiRegisterStream(NiSwitchNode);
    NiRegisterStream(NiSwitchStringExtraData);
    NiRegisterStream(NiTextureEffect);
    NiRegisterStream(NiTexturingProperty);
    NiRegisterStream(NiTriShape);
    NiRegisterStream(NiTriShapeData);
    NiRegisterStream(NiTriShapeDynamicData);
    NiRegisterStream(NiTriStrips);
    NiRegisterStream(NiTriStripsData);
    NiRegisterStream(NiVectorExtraData);
    NiRegisterStream(NiVertexColorProperty);
    NiRegisterStream(NiVertWeightsExtraData);
    NiRegisterStream(NiWireframeProperty);
    NiRegisterStream(NiZBufferProperty);

    // Deprecated classes that can still be loaded
    NiRegisterStream(NiScreenSpaceCamera);
    NiRegisterStream(NiScreenGeometry);
    NiRegisterStream(NiScreenGeometryData);
    NiRegisterStream(NiScreenPolygon);
    NiScreenGeometryData::_SDMInit();

    NiTexture::RendererData::_SDMInit();

    NiPoint3::_SDMInit();

    NiMaterialLibrary::_SDMInit();
    NiSingleShaderMaterialLibrary::_SDMInit();
    NiMaterial::_SDMInit();

    NiRenderClick::_SDMInit();
    NiRenderFrame::_SDMInit();
    NiRenderStep::_SDMInit();
    NiRenderView::_SDMInit();
    NiViewRenderClick::_SDMInit();

    NiShadowMap::_SDMInit();
    NiShadowCubeMap::_SDMInit();

#ifdef WIN32
// identify processor to ensure that optimizations for it are installed
    NiProcessorSpecificCode::OptimizeMainEngineForProcessor(); 
#ifdef _DEBUG
    char acStr[256];
    NiSprintf(acStr, 256, "Gamebryo Processor Detection:  Processor is: %s\n",
        NiProcessorSpecificCode::GetIdentityString());
    NiOutputDebugString(acStr);
#endif // #ifdef _DEBUG
#endif // #ifdef WIN32
}
//---------------------------------------------------------------------------
void NiMainSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiAdditionalGeometryData);
    NiUnregisterStream(NiAlphaAccumulator);
    NiUnregisterStream(NiAlphaProperty);
    NiUnregisterStream(NiAmbientLight);
    NiUnregisterStream(NiAutoNormalParticles);
    NiUnregisterStream(NiAutoNormalParticlesData);
    NiUnregisterStream(NiBillboardNode);
    NiUnregisterStream(NiBinaryExtraData);
    NiUnregisterStream(NiBooleanExtraData);
    NiUnregisterStream(NiBSPNode);
    NiUnregisterStream(NiCamera);
    NiUnregisterStream(NiClusterAccumulator);
    NiUnregisterStream(NiCollisionSwitch);
    NiUnregisterStream(NiColorExtraData);
    NiUnregisterStream(NiDefaultAVObjectPalette);
    NiUnregisterStream(NiDirectionalLight);
    NiUnregisterStream(NiDitherProperty);
    NiUnregisterStream(NiExtraData);
    NiUnregisterStream(NiFloatExtraData);
    NiUnregisterStream(NiFloatsExtraData);
    NiUnregisterStream(NiFogProperty);
    NiUnregisterStream(NiIntegerExtraData);
    NiUnregisterStream(NiIntegersExtraData);
    NiUnregisterStream(NiLODNode);
    NiUnregisterStream(NiLines);
    NiUnregisterStream(NiLinesData);
    NiUnregisterStream(NiMaterialProperty);
    NiUnregisterStream(NiNode);
    NiUnregisterStream(NiPalette);
    NiUnregisterStream(NiParticles);
    NiUnregisterStream(NiParticlesData);
    NiUnregisterStream(NiPersistentSrcTextureRendererData);
    NiUnregisterStream(NiPixelData);
    NiUnregisterStream(NiPointLight);
    NiUnregisterStream(NiRangeLODData);
    NiUnregisterStream(NiRendererSpecificProperty);
    NiUnregisterStream(NiScreenLODData);
    NiUnregisterStream(NiScreenTexture);
    NiUnregisterStream(NiShadeProperty);
    NiUnregisterStream(NiSkinData);
    NiUnregisterStream(NiSkinInstance);
    NiUnregisterStream(NiSkinPartition);
    NiUnregisterStream(NiSortAdjustNode);
    NiUnregisterStream(NiScreenElements);
    NiUnregisterStream(NiScreenElementsData);
    NiUnregisterStream(NiSourceTexture);
    NiUnregisterStream(NiSpecularProperty);
    NiUnregisterStream(NiSpotLight);
    NiUnregisterStream(NiStencilProperty);
    NiUnregisterStream(NiStringExtraData);
    NiUnregisterStream(NiStringsExtraData);
    NiUnregisterStream(NiSwitchNode);
    NiUnregisterStream(NiSwitchStringExtraData);
    NiUnregisterStream(NiTextureEffect);
    NiUnregisterStream(NiTexturingProperty);
    NiUnregisterStream(NiTriShape);
    NiUnregisterStream(NiTriShapeData);
    NiUnregisterStream(NiTriShapeDynamicData);
    NiUnregisterStream(NiTriStrips);
    NiUnregisterStream(NiTriStripsData);
    NiUnregisterStream(NiVectorExtraData);
    NiUnregisterStream(NiVertexColorProperty);
    NiUnregisterStream(NiVertWeightsExtraData);
    NiUnregisterStream(NiWireframeProperty);
    NiUnregisterStream(NiZBufferProperty);

    // Deprecated classes that can still be loaded
    NiUnregisterStream(NiScreenGeometry);
    NiUnregisterStream(NiScreenGeometryData);
    NiUnregisterStream(NiScreenPolygon);
    NiUnregisterStream(NiScreenSpaceCamera);

    NiShaderConstantMap::_SDMShutdown();
    NiTextureEffect::_SDMShutdown();
    NiAlphaProperty::_SDMShutdown();
    NiDitherProperty::_SDMShutdown();
    NiFogProperty::_SDMShutdown();
    NiMaterialProperty::_SDMShutdown();
    NiMetricsLayer::_SDMShutdown();
    NiRendererSpecificProperty::_SDMShutdown();
    NiShadeProperty::_SDMShutdown();
    NiShaderDeclaration::_SDMShutdown();
    NiSpecularProperty::_SDMShutdown();
    NiStencilProperty::_SDMShutdown();
    NiTexturingProperty::_SDMShutdown();
    NiVertexColorProperty::_SDMShutdown();
    NiWireframeProperty::_SDMShutdown();
    NiZBufferProperty::_SDMShutdown();

    NiImageConverter::_SDMShutdown();
    NiStream::_SDMShutdown();
    NiRenderer::_SDMShutdown();

    NiScreenGeometryData::_SDMShutdown();
    NiTexture::RendererData::_SDMShutdown();

    NiPoint3::_SDMShutdown();

    NiMaterial::_SDMShutdown();   
    NiSingleShaderMaterialLibrary::_SDMShutdown();
    NiMaterialLibrary::_SDMShutdown();

    NiRenderClick::_SDMShutdown();
    NiRenderFrame::_SDMShutdown();
    NiRenderStep::_SDMShutdown();
    NiRenderView::_SDMShutdown();
    NiViewRenderClick::_SDMShutdown();

    NiShadowMap::_SDMShutdown();
    NiShadowCubeMap::_SDMShutdown();

    // Shutdown shared memory pool
    NiAllocatorShutdown(size_t);

    NiGlobalStringTable::_SDMShutdown();
}
//---------------------------------------------------------------------------
