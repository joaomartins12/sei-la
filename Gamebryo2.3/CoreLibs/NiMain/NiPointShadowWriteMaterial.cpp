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

#include "NiPointShadowWriteMaterial.h"
#include "NiShadowGenerator.h"
#include "NiStandardMaterialDescriptor.h"
#include "NiStandardPixelProgramDescriptor.h"
#include "NiStandardVertexProgramDescriptor.h"
#include "NiShadowTechnique.h"
#include "NiRenderer.h"


#define NIPOINTSHADOWWRITEMATERIAL_VERTEX_VERSION   2
#define NIPOINTSHADOWWRITEMATERIAL_PIXEL_VERSION    3

NiImplementRTTI(NiPointShadowWriteMaterial, NiFragmentMaterial);

//---------------------------------------------------------------------------
NiPointShadowWriteMaterial::NiPointShadowWriteMaterial(
    NiMaterialNodeLibrary* pkLibrary, bool bAutoCreateCaches) : 
    NiStandardMaterial("NiPointShadowWriteMat", pkLibrary, 
    NIPOINTSHADOWWRITEMATERIAL_VERTEX_VERSION, 0, 
    NIPOINTSHADOWWRITEMATERIAL_PIXEL_VERSION, bAutoCreateCaches),
    m_kDescriptorName("NiStandardMaterialDescriptor")
{
    /* */
}
//---------------------------------------------------------------------------
bool NiPointShadowWriteMaterial::GenerateDescriptor(const NiGeometry* pkGeom, 
    const NiSkinInstance* pkSkin, const NiPropertyState* pkPropState, 
    const NiDynamicEffectState* pkEffectState,
    NiMaterialDescriptor& kMaterialDesc)
{    
    if (!pkPropState)
    {
        NIASSERT(!"Could not find property state! Try calling"
            " UpdateProperties.\n");
        return false;
    }

    NiStandardMaterialDescriptor* pkDesc = (NiStandardMaterialDescriptor*) 
        &kMaterialDesc;
    pkDesc->m_kIdentifier = m_kDescriptorName;

    // Handle transform
    NiSkinPartition* pkPartition = NULL;
    if (pkSkin != NULL)
        pkPartition = pkSkin->GetSkinPartition();

    if (pkSkin != NULL && pkPartition != NULL)
    {
        NiSkinPartition::Partition* pkIndexedPartition = 
            pkPartition->GetPartitions();

        if (pkPartition->GetPartitionCount() > 0 && 
            pkIndexedPartition != NULL && 
            pkIndexedPartition[0].m_pucBonePalette != NULL)
        {
            pkDesc->SetTRANSFORM(TRANSFORM_SKINNED);
        }
        else
        {
            pkDesc->SetTRANSFORM(TRANSFORM_SKINNED_NOPALETTE);
        }

    }
    else
    {
        pkDesc->SetTRANSFORM(TRANSFORM_DEFAULT);
    }


    // Handle TexGen data and UV sets
    if (pkPropState)
    {
        NiTexturingProperty* pkTexProp = pkPropState->GetTexturing();
        NiAlphaProperty* pkAlphaProp = pkPropState->GetAlpha();

        // Alpha property
        if (pkAlphaProp)
        {
            if (pkAlphaProp->GetAlphaTesting() &&
                pkAlphaProp->GetTestMode() == NiAlphaProperty::TEST_GREATER)
            {
                pkDesc->SetALPHATEST(true);
            }
            else
            {
                pkDesc->SetALPHATEST(false);
            }
        }

        // Only reference texturing property if their is alpha information.
        if (pkTexProp && pkAlphaProp && pkDesc->GetALPHATEST())
        {
            // Handle adding textures to the shader description.
            // Note that the order in which these textures are handled MUST
            // match the order in which they were declared in the descriptor.
            // This will guarantee that the UVSets for Map## will correspond 
            // to the correct semantic texture map later on.

            NiTexturingProperty::Map* apkActiveMaps[
                STANDARD_PIPE_MAX_TEXTURE_MAPS];
                memset(apkActiveMaps, 0, sizeof(NiTexturingProperty::Map*) * 
                    STANDARD_PIPE_MAX_TEXTURE_MAPS);

                // Count number of texture sets
                unsigned int uiNumTextureSets = 0;
                for (unsigned int i = 0; i < 8; i++)
                {
                    if (pkGeom->ContainsVertexData(
                        (NiShaderDeclaration::ShaderParameter)
                        (NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0 + i)))
                    {
                        uiNumTextureSets++;
                    }
                }

                switch (pkTexProp->GetApplyMode())
                {
                case NiTexturingProperty::APPLY_REPLACE:
                    pkDesc->SetAPPLYMODE(APPLY_REPLACE);
                    break;
                default:
                case NiTexturingProperty::APPLY_MODULATE:
                    pkDesc->SetAPPLYMODE(APPLY_MODULATE);
                    break;
                }

                NiTexturingProperty::Map* pkMap = NULL;

                unsigned int uiMapIdx = 0;

                // It is customary in Gamebryo to skip all textures if there
                // are no texture coordinates in the geometry.
                if (uiNumTextureSets != 0)
                {
                    if (pkTexProp->ValidMapExists(
                        NiTexturingProperty::BASE_INDEX))
                    {
                        pkMap = pkTexProp->GetBaseMap();
                        pkDesc->SetBASEMAPCOUNT(1);
                        apkActiveMaps[MAP_BASE] = pkMap;
                        uiMapIdx++;
                    }
                }

                // First find out how many UV sets are used as-is
                unsigned int auiSrcUVSetUsedDirectly[
                    STANDARD_PIPE_MAX_TEXTURE_MAPS];
                    memset(auiSrcUVSetUsedDirectly, 0, 
                        STANDARD_PIPE_MAX_TEXTURE_MAPS*sizeof(unsigned int));  
                    unsigned int uiTexTransforms = 0;

                    unsigned int uiWhichMap = 0;

                    for (unsigned int ui = 0; 
                        ui < STANDARD_PIPE_MAX_TEXTURE_MAPS; ui++)
                    {
                        NiTexturingProperty::Map* pkMap = apkActiveMaps[ui];
                        if (!pkMap || uiNumTextureSets == 0)
                            continue;

                        TexGenOutput eTexOutput = 
                            TEX_OUT_PASSTHRU;
                        unsigned int uiSrcIndex = pkMap->GetTextureIndex();
                        if (uiSrcIndex > uiNumTextureSets)
                            uiSrcIndex = uiNumTextureSets;

                        if (pkMap->GetTextureTransform() != NULL)
                        {
                            eTexOutput = TEX_OUT_TRANSFORM;
                            uiTexTransforms++;
                        }
                        else
                        {
                            auiSrcUVSetUsedDirectly[uiSrcIndex]++;
                        }

                        NIASSERT(uiSrcIndex < STANDARD_PIPE_MAX_TEXTURE_MAPS);

                        pkDesc->SetTextureUsage(
                            uiWhichMap, uiSrcIndex, eTexOutput);
                        uiWhichMap++;
                    }
                    NIASSERT(uiWhichMap == uiMapIdx);

                    // Pack all texture coordinates, even if they're not 
                    // currently in use.
                    pkDesc->SetINPUTUVCOUNT(uiNumTextureSets);
        }

    }


    if (pkEffectState)
    {
        unsigned int uiShadowTechniqueSlot = 0;

        // Search lights in the order Point, Directional, Spot. This is 
        // required because all of NiStandardMaterial assumes lights have 
        // been added in this order.

        // Search Point Lights
        NiDynEffectStateIter kLightIter = pkEffectState->GetLightHeadPos();
        while (kLightIter != NULL)
        {
            NiLight* pkLight = pkEffectState->GetNextLight(kLightIter);
            if (pkLight && 
                (pkLight->GetEffectType() == NiDynamicEffect::POINT_LIGHT ||
                pkLight->GetEffectType() == 
                NiDynamicEffect::SHADOWPOINT_LIGHT))
            {
                NiShadowGenerator* pkGenerator = 
                    pkLight->GetShadowGenerator();

                if (pkGenerator && pkGenerator->GetActive())
                {
                    unsigned short usActiveSlot = pkGenerator->
                        GetShadowTechnique()->GetActiveTechniqueSlot();

                    // If this assert is hit it means the light is assigned
                    // a NiShadowTechnique that is not one of the active 
                    // NiShadowTechniques.
                    NIASSERT (usActiveSlot < 
                        NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES );

                    // Only use the assigned shadow technique from the 
                    // light if the NiShadowTechnique has a higher priority
                    // than the one already applied. 
                    // Note: NiShadowTechnique priority is defined by the 
                    // slot the NiShadowTechnique is assigned to. 
                    uiShadowTechniqueSlot = 
                        NiMax(usActiveSlot, uiShadowTechniqueSlot);
                }
            }
        }


        // Search Directional Lights
        kLightIter = pkEffectState->GetLightHeadPos();
        while (kLightIter != NULL)
        {
            NiLight* pkLight = pkEffectState->GetNextLight(kLightIter);
            if (pkLight && 
                (pkLight->GetEffectType() == NiDynamicEffect::DIR_LIGHT ||
                pkLight->GetEffectType() == NiDynamicEffect::SHADOWDIR_LIGHT))
            {
                NiShadowGenerator* pkGenerator = 
                    pkLight->GetShadowGenerator();

                if (pkGenerator && pkGenerator->GetActive())
                {
                    unsigned short usActiveSlot = pkGenerator->
                        GetShadowTechnique()->GetActiveTechniqueSlot();

                    // If this assert is hit it means the light is assigned
                    // a NiShadowTechnique that is not one of the active 
                    // NiShadowTechniques.
                    NIASSERT (usActiveSlot < 
                        NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES );

                    // Only use the assigned shadow technique from the 
                    // light if the NiShadowTechnique has a higher priority
                    // than the one already applied. 
                    // Note: NiShadowTechnique priority is defined by the 
                    // slot the NiShadowTechnique is assigned to. 
                    uiShadowTechniqueSlot = 
                        NiMax(usActiveSlot, uiShadowTechniqueSlot);
                }
            }
        }

        // Search Spot Lights
        kLightIter = pkEffectState->GetLightHeadPos();
        while (kLightIter != NULL)
        {
            NiLight* pkLight = pkEffectState->GetNextLight(kLightIter);
            if (pkLight && 
                (pkLight->GetEffectType() == NiDynamicEffect::SPOT_LIGHT ||
                pkLight->GetEffectType() == NiDynamicEffect::SHADOWSPOT_LIGHT))
            {
                NiShadowGenerator* pkGenerator = 
                    pkLight->GetShadowGenerator();

                if (pkGenerator && pkGenerator->GetActive())
                {
                    unsigned short usActiveSlot = pkGenerator->
                        GetShadowTechnique()->GetActiveTechniqueSlot();

                    // If this assert is hit it means the light is assigned
                    // a NiShadowTechnique that is not one of the active 
                    // NiShadowTechniques.
                    NIASSERT (usActiveSlot < 
                        NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES );

                    // Only use the assigned shadow technique from the 
                    // light if the NiShadowTechnique has a higher priority
                    // than the one already applied. 
                    // Note: NiShadowTechnique priority is defined by the 
                    // slot the NiShadowTechnique is assigned to. 
                    uiShadowTechniqueSlot = 
                        NiMax(usActiveSlot, uiShadowTechniqueSlot);
                }
            }
        }


        pkDesc->SetSHADOWTECHNIQUE(uiShadowTechniqueSlot);
    }


    return true;
}
//---------------------------------------------------------------------------
NiGPUProgram* NiPointShadowWriteMaterial::GeneratePixelShaderProgram(
    NiGPUProgramDescriptor* pkDesc, 
    NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms)
{
    // Overwrite the pixel shader because there is nothing to do here
    // but check for alpha textures and write the depth to the color.

    char acFilename[NI_MAX_PATH];

    NIVERIFY(pkDesc->GenerateKey(acFilename, 48));

    bool bFailedCompilePreviously = false;
    NiGPUProgram* pkCachedShader =
        m_aspProgramCaches[NiGPUProgram::PROGRAM_PIXEL]->FindCachedProgram(
        acFilename, kUniforms, bFailedCompilePreviously);

    if (pkCachedShader)
        return pkCachedShader;

    if (bFailedCompilePreviously)
        return NULL;

#if defined(_DEBUG)
    NiOutputDebugString("Generating pixel shader for object \"");
    NiOutputDebugString((const char*)m_kDebugIdentifier);
    NiOutputDebugString("\":\n");
#endif

    Context kContext;
    kContext.m_spConfigurator = NiNew NiMaterialConfigurator(
        m_aspProgramCaches[NiGPUProgram::PROGRAM_PIXEL]);

    NIASSERT(pkDesc->m_kIdentifier == "NiStandardPixelProgramDescriptor");
    NiStandardPixelProgramDescriptor* pkPixelDesc = 
        (NiStandardPixelProgramDescriptor*)pkDesc;

    // Add constant map elements
    kContext.m_spUniforms = NiNew NiMaterialResourceProducerNode("Uniforms",
        "Uniform");
    kContext.m_spConfigurator->AddNode(kContext.m_spUniforms);

    // Create statics
    kContext.m_spStatics = NiNew NiMaterialResourceProducerNode("Statics", 
        "Static");
    kContext.m_spConfigurator->AddNode(kContext.m_spStatics);

    // Create pixel in
    kContext.m_spInputs = NiNew 
        NiMaterialResourceProducerNode("VertexOut", "Vertex");
    // This is necessary for D3D10 support.
    kContext.m_spInputs->AddOutputResource("float4", "Position",
        "World",  "PosProjected");
    
    NiMaterialResource* pkWorldViewVect = 
        kContext.m_spInputs->AddOutputResource("float3", "TexCoord",
        "World",  "WorldViewVector");
    kContext.m_spConfigurator->AddNode(kContext.m_spInputs);


    NiMaterialResource* apkUVSets[STANDARD_PIPE_MAX_UVS_FOR_TEXTURES];

    unsigned int uiTexIndex = 0;
    for (unsigned int ui = 0; ui < pkPixelDesc->GetInputUVCount(); 
        ui++)
    {
        apkUVSets[uiTexIndex] = kContext.m_spInputs->AddOutputResource(
            "float2", "TexCoord", "", GenerateUVSetName(uiTexIndex));
        uiTexIndex++;
    }

    // Wire up shader to sample the base map and perfrom an alpha test based on
    // the base map's alpha component.
    if (pkPixelDesc->GetALPHATEST() != 0 && 
        pkPixelDesc->GetBASEMAPCOUNT() == 1)
    {
        NiMaterialResource* pkDiffuseTexAccum = NULL;
        NiMaterialResource* pkOpacityTexAccum = NULL;

        if (!HandleBaseMap(kContext, 
            FindUVSetIndexForTextureEnum( MAP_BASE, 
            pkPixelDesc, apkUVSets), pkDiffuseTexAccum, pkOpacityTexAccum, 
            true))
        {
            return false;
        }

        NiMaterialNode* pkAlphaTestNode = 
            GetAttachableNodeFromLibrary("ApplyAlphaTest");
        kContext.m_spConfigurator->AddNode(pkAlphaTestNode);

        NiMaterialResource* pkTestFunction = AddOutputPredefined(
            kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_DEF_ALPHA_TEST_FUNC);

        kContext.m_spConfigurator->AddBinding(pkTestFunction, 
            pkAlphaTestNode->GetInputResourceByVariableName(
            "AlphaTestFunction"));

        NiMaterialResource* pkTestRef = AddOutputPredefined(
            kContext.m_spUniforms, 
            NiShaderConstantMap::SCM_DEF_ALPHA_TEST_REF);
        kContext.m_spConfigurator->AddBinding(pkTestRef, 
            pkAlphaTestNode->GetInputResourceByVariableName(
            "AlphaTestRef"));

        kContext.m_spConfigurator->AddBinding(pkOpacityTexAccum,
            pkAlphaTestNode->GetInputResourceByVariableName("AlphaTestValue"));
    }

    // Create pixel out
    kContext.m_spOutputs = NiNew NiMaterialResourceConsumerNode("PixelOut", 
        "Pixel");
    NiMaterialResource* pkPixelOutColor = 
        kContext.m_spOutputs->AddInputResource("float4", "Color", "", "Color");
    kContext.m_spConfigurator->AddNode(kContext.m_spOutputs);

    // Connect pixel shader node to the assigned ShadowTechnique.
    unsigned int uiShadowTechniqueID = pkPixelDesc->GetSHADOWTECHNIQUE();
    NiShadowTechnique* pkShadowTechnique =
        NiShadowManager::GetKnownShadowTechnique(uiShadowTechniqueID);

    // If this assert is hit it means the NiShadowTechnique referenced by
    // is not one of the active NiShadowTechnique objects. A NiShadowTechnique
    // can only be used by the render if it is one of the active techniques.
    NIASSERT(pkShadowTechnique->GetActiveTechniqueSlot() < 
        NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES);

    NiMaterialNode* pkDepthNode = GetAttachableNodeFromLibrary(
        pkShadowTechnique->GetWriteFragmentName(
        NiStandardMaterial::LIGHT_POINT));
    kContext.m_spConfigurator->AddNode(pkDepthNode);

    // ViewVector Input
    kContext.m_spConfigurator->AddBinding(pkWorldViewVect, 
        pkDepthNode->GetInputResourceByVariableName("WorldViewVector"));

    // Final color output
    kContext.m_spConfigurator->AddBinding("OutputColor", pkDepthNode, 
        pkPixelOutColor);

    return kContext.m_spConfigurator->Evaluate(acFilename, 
        NiGPUProgram::PROGRAM_PIXEL, kUniforms);
}
//---------------------------------------------------------------------------
NiGPUProgram* NiPointShadowWriteMaterial::GenerateVertexShaderProgram(
    NiGPUProgramDescriptor* pkDesc, 
    NiTObjectPtrSet<NiMaterialResourcePtr>& kUniforms)
{
    char acFilename[NI_MAX_PATH];
    NIVERIFY(pkDesc->GenerateKey(acFilename, 48));
    if (!m_aspProgramCaches[NiGPUProgram::PROGRAM_VERTEX])
        return NULL;

    bool bFailedToCompilePreviously = false;
    NiGPUProgram* pkCachedShader =
        m_aspProgramCaches[NiGPUProgram::PROGRAM_VERTEX]->FindCachedProgram(
        acFilename, kUniforms, bFailedToCompilePreviously);

    if (pkCachedShader)
        return pkCachedShader;

    if (bFailedToCompilePreviously)
        return NULL;

#if defined(_DEBUG)
    NiOutputDebugString("Generating vertex shader for object \"");
    NiOutputDebugString((const char*)m_kDebugIdentifier);
    NiOutputDebugString("\":\n");
#endif

    Context kContext;
    kContext.m_spConfigurator = NiNew NiMaterialConfigurator(
        m_aspProgramCaches[NiGPUProgram::PROGRAM_VERTEX]);

    NIASSERT(pkDesc->m_kIdentifier == "NiStandardVertexProgramDescriptor");
    NiStandardVertexProgramDescriptor* pkVertexDesc = 
        (NiStandardVertexProgramDescriptor*)pkDesc;

    // Create vertex in
    kContext.m_spInputs = NiNew NiMaterialResourceProducerNode("VertexIn",
        "Vertex");  
    kContext.m_spConfigurator->AddNode(kContext.m_spInputs);

    // Add constant map elements
    kContext.m_spUniforms = NiNew NiMaterialResourceProducerNode("Uniforms",
        "Uniform");
    kContext.m_spConfigurator->AddNode(kContext.m_spUniforms);

    kContext.m_spStatics = NiNew NiMaterialResourceProducerNode("Statics",
        "Static");
    kContext.m_spConfigurator->AddNode(kContext.m_spStatics);

    // Create vertex out
    kContext.m_spOutputs = NiNew NiMaterialResourceConsumerNode("VertexOut", 
        "Vertex");
    NiMaterialResource* pkVertOutProjPos = 
        kContext.m_spOutputs->AddInputResource("float4", "Position", "World", 
        "PosProjected");
    kContext.m_spConfigurator->AddNode(kContext.m_spOutputs);

    // Handle transform pipeline
    NiMaterialResource* pkWorldPos = NULL;
    NiMaterialResource* pkWorldNormal = NULL;
    NiMaterialResource* pkWorldView = NULL;
    NiMaterialResource* pkViewPos = NULL;

    if (!SetupTransformPipeline(kContext, pkVertOutProjPos, pkVertexDesc,
        true, false, pkWorldPos, pkViewPos, pkWorldNormal, 
        pkWorldView))
    {
        SetFailedGPUProgram(NiGPUProgram::PROGRAM_VERTEX, pkDesc);
        return NULL;
    }

    unsigned int uiNextUVSet = 0;
    NiMaterialResource* apkOutputUVs[STANDARD_PIPE_MAX_UVS_FOR_TEXTURES];
    memset(apkOutputUVs, 0, STANDARD_PIPE_MAX_UVS_FOR_TEXTURES * 
        sizeof(NiMaterialResource*));

    if (!HandleTextureUVSets(kContext, pkVertexDesc, apkOutputUVs,
        STANDARD_PIPE_MAX_UVS_FOR_TEXTURES, uiNextUVSet))
    {
        SetFailedGPUProgram(NiGPUProgram::PROGRAM_VERTEX, pkDesc);
        return NULL;
    }

    // Bind the output texture array to output resources.
    for (unsigned int ui = 0; ui < uiNextUVSet; ui++)
    {
        NIASSERT(apkOutputUVs[ui] != NULL);
        NiMaterialResource* pkVertOutTexCoord = 
            kContext.m_spOutputs->AddInputResource(
            apkOutputUVs[ui]->GetType(), "TexCoord",
            "", GenerateUVSetName(ui));

        kContext.m_spConfigurator->AddBinding(apkOutputUVs[ui],
            pkVertOutTexCoord);
    }

    NiGPUProgram* pkProgram = kContext.m_spConfigurator->Evaluate(acFilename, 
        NiGPUProgram::PROGRAM_VERTEX, kUniforms);

    if (pkProgram == NULL)
    {
        SetFailedGPUProgram(NiGPUProgram::PROGRAM_VERTEX, pkDesc);
    }

    return pkProgram;
}
//---------------------------------------------------------------------------
unsigned int NiPointShadowWriteMaterial::VerifyShaderPrograms(
    NiGPUProgram* pkVertexShader, NiGPUProgram* pkGeometryShader, 
    NiGPUProgram* pkPixelShader)
{
    unsigned int uiReturnCode = RC_SUCCESS;
    if (pkVertexShader == NULL)
        uiReturnCode |= RC_COMPILE_FAILURE_VERTEX;
    if (pkPixelShader == NULL)
        uiReturnCode |= RC_COMPILE_FAILURE_PIXEL;
    // No need to check geometry shader

    return uiReturnCode;
}
//---------------------------------------------------------------------------
bool NiPointShadowWriteMaterial::SetupTransformPipeline(Context& kContext, 
    NiMaterialResource* pkOutProjPos,
    NiStandardVertexProgramDescriptor* pkVertexDesc, bool bForceWorldView,
    bool bForceViewPos, NiMaterialResource*& pkWorldPos, 
    NiMaterialResource*& pkViewPos, NiMaterialResource*& pkWorldNormal,
    NiMaterialResource*& pkWorldView)
{
    pkWorldPos = NULL;
    pkWorldNormal = NULL;
    pkWorldView = NULL;

    NiMaterialResource* pkWorldMatrix = NULL;

    if (!HandlePositionFragment(kContext, 
        (TransformType)pkVertexDesc->GetTRANSFORM(),
        pkWorldPos, pkWorldMatrix))
    {
        return false;
    }

    NiMaterialResource* pkProjPos = NULL;
    if (!HandleViewProjectionFragment(kContext, bForceViewPos, 
        pkWorldPos, pkProjPos, pkViewPos))
    {
        return false;
    }

    if (!pkProjPos || !pkOutProjPos)
        return false;

    if (bForceWorldView)
    {
        NiMaterialResource* pkTangentView = NULL;

        if (!HandleViewVectorFragment(kContext,
            pkWorldPos, pkWorldNormal, false, NULL,
            false, pkWorldView, pkTangentView))
        {
            return false;
        }

        //if (pkWorldView)
        //{
        //    // Insert view vector
        //    NiMaterialResource* pkWorldTexCoord = 
        //        kContext.m_spOutputs->AddInputResource("float3",
        //        "TexCoord", "", "ViewVector");
        //    kContext.m_spConfigurator->AddBinding(pkWorldView, 
        //        pkWorldTexCoord);
        //}
    }

    kContext.m_spConfigurator->AddBinding(pkProjPos, 
        pkOutProjPos); 


    if (pkVertexDesc->GetOUTPUTWORLDPOS() == 1)
    {
        // Insert world position
        NiMaterialResource* pkWorldTexCoord = 
            kContext.m_spOutputs->AddInputResource("float4",
            "TexCoord", "", "WorldPos");
        kContext.m_spOutputs->AddOutputResource("float4",
            "TexCoord", "", "WorldPos");
        kContext.m_spConfigurator->AddBinding(pkWorldPos, 
            pkWorldTexCoord);
    }

    return true;
}
//---------------------------------------------------------------------------
NiFragmentMaterial::ReturnCode 
    NiPointShadowWriteMaterial::GenerateShaderDescArray(
    NiMaterialDescriptor* pkMaterialDescriptor,
    RenderPassDescriptor* pkRenderPasses, unsigned int uiMaxCount,
    unsigned int& uiCountAdded)
{
    NIASSERT(uiMaxCount != 0);
    uiCountAdded = 0;

    if (pkMaterialDescriptor->m_kIdentifier != 
        "NiStandardMaterialDescriptor")
        return RC_INVALID_MATERIAL;

    // Make sure that we're using the Gamebryo render state on the first pass.
    // Also ensure that no alpha blending is used as this is not needed for 
    // rendering to shadow maps.
    pkRenderPasses[0].m_bUsesNiRenderState = true;
    pkRenderPasses[0].m_bAlphaOverride = true;
    pkRenderPasses[0].m_bAlphaBlend = false;

    // Reset all object offsets for the first pass.
    pkRenderPasses[0].m_bResetObjectOffsets = true;

    NiStandardMaterialDescriptor* pkMatlDesc = 
        (NiStandardMaterialDescriptor*) pkMaterialDescriptor;

    // Uncomment these lines to get a human-readable version of the 
    // material description
    // char acBuffer[2048];
    // pkMatlDesc->ToString(acBuffer, 2048);

    NiStandardVertexProgramDescriptor* pkVertexDesc = 
        (NiStandardVertexProgramDescriptor*) &pkRenderPasses[0].m_kVertexDesc;
    pkVertexDesc->m_kIdentifier = "NiStandardVertexProgramDescriptor";

    NiStandardPixelProgramDescriptor* pkPixelDesc = 
        (NiStandardPixelProgramDescriptor*) &pkRenderPasses[0].m_kPixelDesc;
    pkPixelDesc->m_kIdentifier = "NiStandardPixelProgramDescriptor";

    // Pixel Desc
    unsigned int uiApplyMode = pkMatlDesc->GetAPPLYMODE();
    pkPixelDesc->SetAPPLYMODE(uiApplyMode);

    unsigned int uiBaseCount = pkMatlDesc->GetBASEMAPCOUNT();
    pkPixelDesc->SetBASEMAPCOUNT(uiBaseCount);

    unsigned int uiAlphaTest = pkMatlDesc->GetALPHATEST();
    pkPixelDesc->SetALPHATEST(uiAlphaTest);

    unsigned int uiShadowTechniqueSlot = pkMatlDesc->GetSHADOWTECHNIQUE();
    NiShadowTechnique* pkShadowTechnique = 
        NiShadowManager::GetActiveShadowTechnique(uiShadowTechniqueSlot);
        
    pkPixelDesc->SetSHADOWTECHNIQUE(pkShadowTechnique->GetTechniqueID());

    // Vertex Desc
    unsigned int uiTransform = pkMatlDesc->GetTRANSFORM();
    pkVertexDesc->SetTRANSFORM(uiTransform);

    unsigned int auiUVSets[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(auiUVSets, UINT_MAX, sizeof(auiUVSets));

    TexGenOutput aeTexGenOutputs[STANDARD_PIPE_MAX_TEXTURE_MAPS];
    memset(aeTexGenOutputs, 0, sizeof(aeTexGenOutputs));

    unsigned int uiTextureCount = pkMatlDesc->GetStandardTextureCount();
    NIASSERT(uiTextureCount <= STANDARD_PIPE_MAX_TEXTURE_MAPS);

    for (unsigned int ui = 0; ui < uiTextureCount; ui++)
    {
        pkMatlDesc->GetTextureUsage(ui, auiUVSets[ui], aeTexGenOutputs[ui]);
    }

    AssignTextureCoordinates(auiUVSets, aeTexGenOutputs, uiTextureCount, 
        pkVertexDesc, pkPixelDesc);


    uiCountAdded++;
    return RC_SUCCESS;
}
//---------------------------------------------------------------------------
NiShader* NiPointShadowWriteMaterial::CreateShader(
    NiMaterialDescriptor* pkMaterialDescriptor)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (pkRenderer == NULL)
        return false;

    return pkRenderer->GetShadowWriteShader(pkMaterialDescriptor);
}
//---------------------------------------------------------------------------
bool NiPointShadowWriteMaterial::SetupPackingRequirements(NiShader* pkShader,
    NiMaterialDescriptor* pkMaterialDescriptor,
    RenderPassDescriptor* pkRenderPassDesc, unsigned int uiCount)
{
    NiShaderDeclarationPtr spShaderDecl = 
        pkShader->CreateShaderDeclaration(15);

    if (!spShaderDecl)
    {
        NIASSERT(!"Invalid shader declaration");
        return false;
    }

    NiStandardMaterialDescriptor* pkRealMaterialDesc = 
        (NiStandardMaterialDescriptor*) pkMaterialDescriptor;

    unsigned int uiEntryCount = 0;
    spShaderDecl->SetEntry(uiEntryCount++,
        NiShaderDeclaration::SHADERPARAM_NI_POSITION,
        NiShaderDeclaration::SPTYPE_FLOAT3);

    if (pkRealMaterialDesc->GetTRANSFORM() == TRANSFORM_SKINNED)
    {
        spShaderDecl->SetEntry(uiEntryCount++,
            NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT,
            NiShaderDeclaration::SPTYPE_FLOAT4);
        spShaderDecl->SetEntry(uiEntryCount++,
            NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES,
            NiShaderDeclaration::SPTYPE_UBYTE4);
    }
    else if (pkRealMaterialDesc->GetTRANSFORM() == 
        TRANSFORM_SKINNED_NOPALETTE)
    {
        spShaderDecl->SetEntry(uiEntryCount++,
            NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT,
            NiShaderDeclaration::SPTYPE_FLOAT4);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPointShadowWriteMaterial::HandlePositionFragment(Context& kContext, 
    TransformType eTransType, NiMaterialResource*& pkVertWorldPos,
    NiMaterialResource*& pkWorldMatrix)
{
    pkVertWorldPos = NULL;
    pkWorldMatrix = NULL;

    if (eTransType == TRANSFORM_DEFAULT)
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "TransformPosition");
        kContext.m_spConfigurator->AddNode(pkProjFrag);

        // Add Per-Vertex Elements
        kContext.m_spInputs->AddOutputResource("float3", "Position", "Local", 
            "Position", 1);

        // Add constant map elements
        pkWorldMatrix = AddOutputPredefined(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_DEF_WORLD, 4);

        // Bind projection
        kContext.m_spConfigurator->AddBinding("Position", kContext.m_spInputs, 
            "Position", pkProjFrag);

        kContext.m_spConfigurator->AddBinding(pkWorldMatrix,
            "World", pkProjFrag);

        pkVertWorldPos = pkProjFrag->GetOutputResourceByVariableName(
            "WorldPos");
    }
    else if (eTransType == TRANSFORM_SKINNED)
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "TransformSkinnedPosition");
        kContext.m_spConfigurator->AddNode(pkProjFrag);

        // Add Per-Vertex Elements
        kContext.m_spInputs->AddOutputResource("float3", "Position", 
            "Local", "Position");
        kContext.m_spInputs->AddOutputResource("float4", "BlendWeight", 
            "Local", "BlendWeights");
        kContext.m_spInputs->AddOutputResource("int4", "BlendIndices", 
            "Local", "BlendIndices");

        // Add constant map elements
        NiMaterialResource* pkBoneMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_BONE_MATRIX_3,
            3, 30);

        pkWorldMatrix = AddOutputPredefined(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_DEF_SKINWORLD, 4);

        // Bind projection
        kContext.m_spConfigurator->AddBinding("Position", kContext.m_spInputs, 
            "Position", pkProjFrag);
        kContext.m_spConfigurator->AddBinding("BlendWeights",
            kContext.m_spInputs, "BlendWeights", pkProjFrag);
        kContext.m_spConfigurator->AddBinding("BlendIndices", 
            kContext.m_spInputs, "BlendIndices", pkProjFrag);       

        kContext.m_spConfigurator->AddBinding(pkBoneMatrix,
            "Bones", pkProjFrag);
        kContext.m_spConfigurator->AddBinding(pkWorldMatrix,
            "SkinToWorldTransform", pkProjFrag);

        pkVertWorldPos = pkProjFrag->GetOutputResourceByVariableName(
            "WorldPos");


        pkWorldMatrix = 
            pkProjFrag->GetOutputResourceByVariableName("SkinBoneTransform");
    }
    else if (eTransType == TRANSFORM_SKINNED_NOPALETTE)
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "TransformSkinnedPositionNoIndices");
        kContext.m_spConfigurator->AddNode(pkProjFrag);

        // Add Per-Vertex Elements
        kContext.m_spInputs->AddOutputResource("float3", "Position", 
            "Local", "Position");
        kContext.m_spInputs->AddOutputResource("float4", "BlendWeight", 
            "Local", "BlendWeights");
        // Add constant map elements
        NiMaterialResource* pkBoneMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_BONE_MATRIX_3,
            3, 4);

        pkWorldMatrix = AddOutputPredefined(kContext.m_spUniforms,
            NiShaderConstantMap::SCM_DEF_SKINWORLD, 4);

        // Bind projection
        kContext.m_spConfigurator->AddBinding("Position", kContext.m_spInputs, 
            "Position", pkProjFrag);
        kContext.m_spConfigurator->AddBinding("BlendWeights",
            kContext.m_spInputs, "BlendWeights", pkProjFrag);    

        kContext.m_spConfigurator->AddBinding(pkBoneMatrix,
            "Bones", pkProjFrag);
        kContext.m_spConfigurator->AddBinding(pkWorldMatrix,
            "SkinToWorldTransform", pkProjFrag);

        pkVertWorldPos = pkProjFrag->GetOutputResourceByVariableName(
            "WorldPos");


        pkWorldMatrix = 
            pkProjFrag->GetOutputResourceByVariableName("SkinBoneTransform");
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiPointShadowWriteMaterial::HandleViewProjectionFragment(
    Context& kContext, 
    bool bForceViewPos, NiMaterialResource* pkVertWorldPos, 
    NiMaterialResource*& pkVertOutProjectedPos,
    NiMaterialResource*& pkVertOutViewPos)
{
    if (bForceViewPos)
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "ProjectPositionWorldToViewToProj");
        if (!pkProjFrag)
        {
            NIASSERT(!"Error in fragment");
            return false;
        }

        kContext.m_spConfigurator->AddNode(pkProjFrag);

        NiMaterialResource* pkViewMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_VIEW, 4);
        kContext.m_spConfigurator->AddBinding(pkViewMatrix, 
            pkProjFrag->GetInputResourceByVariableName("ViewTransform"));

        NiMaterialResource* pkProjMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_PROJ, 4);
        kContext.m_spConfigurator->AddBinding(pkProjMatrix, 
            pkProjFrag->GetInputResourceByVariableName("ProjTransform"));

        kContext.m_spConfigurator->AddBinding(pkVertWorldPos, 
            pkProjFrag->GetInputResourceByVariableName("WorldPosition"));

        pkVertOutViewPos = pkProjFrag->GetOutputResourceByVariableName(
            "ViewPos");

        pkVertOutProjectedPos = pkProjFrag->GetOutputResourceByVariableName(
            "ProjPos");
    }
    else
    {
        NiMaterialNode* pkProjFrag = GetAttachableNodeFromLibrary(
            "ProjectPositionWorldToProj");
        if (!pkProjFrag)
        {
            NIASSERT(!"Error in fragment");
            return false;
        }

        kContext.m_spConfigurator->AddNode(pkProjFrag);

        NiMaterialResource* pkViewProjMatrix = AddOutputPredefined(
            kContext.m_spUniforms, NiShaderConstantMap::SCM_DEF_VIEWPROJ, 4);

        kContext.m_spConfigurator->AddBinding(pkViewProjMatrix, 
            pkProjFrag->GetInputResourceByVariableName("ViewProjection"));
        kContext.m_spConfigurator->AddBinding(pkVertWorldPos, 
            pkProjFrag->GetInputResourceByVariableName("WorldPosition"));

        pkVertOutProjectedPos = pkProjFrag->GetOutputResourceByVariableName(
            "ProjPos");

    }

    // add projected position as a texture coordinate 
    NiMaterialResource* pkVertOutProjTexCoord = 
        kContext.m_spOutputs->AddInputResource("float4", "TexCoord", "World", 
        "PosProjectedPassThrough");

    // Split vertex out projected position into the new texture coordinate
    // as well as output it from this fragment.  This is necessary because
    // (for now) you can't bind two named outputs to a single input.
    NiMaterialNode* pkSplitterNode = GetAttachableNodeFromLibrary(
        "TeeFloat4");
    kContext.m_spConfigurator->AddNode(pkSplitterNode);

    kContext.m_spConfigurator->AddBinding(pkVertOutProjectedPos,
        pkSplitterNode->GetInputResourceByVariableName("Input"));

    kContext.m_spConfigurator->AddBinding(
        pkSplitterNode->GetOutputResourceByVariableName("Output1"),
        pkVertOutProjTexCoord);

    pkVertOutProjectedPos = pkSplitterNode->GetOutputResourceByVariableName(
        "Output2");

    return true;
}
//---------------------------------------------------------------------------
bool NiPointShadowWriteMaterial::HandleViewVectorFragment(Context& kContext,
    NiMaterialResource* pkWorldPos, NiMaterialResource* pkWorldNorm,
    NiMaterialResource* pkWorldBinorm, NiMaterialResource* pkWorldTangent,
    bool bComputeTangent, NiMaterialResource*& pkWorldViewVector, 
    NiMaterialResource*& pkTangentViewVector)
{
    pkWorldViewVector = NULL;
    pkTangentViewVector = NULL;

    NiMaterialNode* pkNode = GetAttachableNodeFromLibrary(
        "CalculateViewVector");
    kContext.m_spConfigurator->AddNode(pkNode);

    kContext.m_spConfigurator->AddBinding(pkWorldPos, 
        pkNode->GetInputResourceByVariableName("WorldPos"));

    NiMaterialResource* pkCameraPosition = AddOutputPredefined(
        kContext.m_spUniforms,
        NiShaderConstantMap::SCM_DEF_EYE_POS);

    kContext.m_spConfigurator->AddBinding(pkCameraPosition, 
        pkNode->GetInputResourceByVariableName("CameraPos")); 

    // This vector will not be normalized!
    pkWorldViewVector = 
        pkNode->GetOutputResourceByVariableName("WorldViewVector");

    NiMaterialResource* pkVertOutProjTexCoord = 
        kContext.m_spOutputs->AddInputResource("float3", "TexCoord", "World", 
        "WorldViewVector");

    kContext.m_spConfigurator->AddBinding(pkWorldViewVector,
        pkVertOutProjTexCoord);

    return true;
}
//---------------------------------------------------------------------------
bool NiPointShadowWriteMaterial::GetVertexInputSemantics(
    NiGeometry* pkGeometry, unsigned int uiMaterialExtraData,
    NiShaderDeclaration::ShaderRegisterEntry* pakSemantics)
{
    // We'll always need position. We'll always request stream 0.
    pakSemantics[0].m_uiPreferredStream = 0;
    pakSemantics[0].m_eInput = NiShaderDeclaration::SHADERPARAM_NI_POSITION;
    pakSemantics[0].m_eType = NiShaderDeclaration::SPTYPE_FLOAT3;
    pakSemantics[0].m_kUsage = NiShaderDeclaration::UsageToString(
        NiShaderDeclaration::SPUSAGE_POSITION);
    pakSemantics[0].m_uiUsageIndex = 0; 
    pakSemantics[0].m_uiExtraData = 0;

    unsigned int uiEntryCount = 1;

    // Handle skinned transforms.
    NiSkinPartition* pkPartition = NULL;
    NiSkinInstance* pkSkin = pkGeometry->GetSkinInstance();
    if (pkSkin != NULL)
        pkPartition = pkSkin->GetSkinPartition();

    if (pkSkin != NULL && pkPartition != NULL)
    {
        NiSkinPartition::Partition* pkIndexedPartition = 
            pkPartition->GetPartitions();

        // Add bone weights.
        pakSemantics[uiEntryCount].m_uiPreferredStream = 0;
        pakSemantics[uiEntryCount].m_eInput = 
            NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT;
        pakSemantics[uiEntryCount].m_eType = 
            NiShaderDeclaration::SPTYPE_FLOAT3;
        pakSemantics[uiEntryCount].m_kUsage = 
            NiShaderDeclaration::UsageToString(
            NiShaderDeclaration::SPUSAGE_BLENDWEIGHT);
        pakSemantics[uiEntryCount].m_uiUsageIndex = 0; 
        pakSemantics[uiEntryCount].m_uiExtraData = 0;
        ++uiEntryCount;

        if (pkPartition->GetPartitionCount() > 0 && 
            pkIndexedPartition != NULL && 
            pkIndexedPartition[0].m_pucBonePalette != NULL)
        {
            pakSemantics[uiEntryCount].m_uiPreferredStream = 0;
            pakSemantics[uiEntryCount].m_eInput = 
                NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES;
            pakSemantics[uiEntryCount].m_eType = 
                NiShaderDeclaration::SPTYPE_UBYTE4;
            pakSemantics[uiEntryCount].m_kUsage = 
                NiShaderDeclaration::UsageToString(
                NiShaderDeclaration::SPUSAGE_BLENDINDICES);
            pakSemantics[uiEntryCount].m_uiUsageIndex = 0; 
            pakSemantics[uiEntryCount].m_uiExtraData = 0;
            ++uiEntryCount;
        }
    }

    // Signal the end of our requirements by packing a usage index of 
    // NiMaterial::VS_INPUTS_TERMINATE_ARRAY.
    NIASSERT(uiEntryCount < NiMaterial::VS_INPUTS_MAX_NUM);
    pakSemantics[uiEntryCount].m_uiUsageIndex = 
        NiMaterial::VS_INPUTS_TERMINATE_ARRAY;

    return true;
}
//---------------------------------------------------------------------------
