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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10Shader.h"

#include "NiD3D10Error.h"
#include "NiD3D10GeometryData.h"
#include "NiD3D10GeometryShader.h"
#include "NiD3D10Pass.h"
#include "NiD3D10PixelShader.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10RenderStateManager.h"
#include "NiD3D10RenderStateGroup.h"
#include "NiD3D10ShaderConstantManager.h"
#include "NiD3D10ShaderConstantMap.h"
#include "NiD3D10VertexShader.h"

#include <NiGeometry.h>
#include <NiSCMExtraData.h>
#include <NiShaderConstantMapEntry.h>

NiImplementRTTI(NiD3D10Shader, NiD3D10ShaderInterface);

NiFixedString NiD3D10Shader::ms_kEmergentShaderMapName = NULL;
NiD3D10Shader::DynamicEffectPacker NiD3D10Shader::ms_apfnDynEffectPackers[
    NiTextureEffect::NUM_COORD_GEN];

//---------------------------------------------------------------------------
void NiD3D10Shader::_SDMInit()
{
    ms_kEmergentShaderMapName = "__NDL_SCMData";

    ms_apfnDynEffectPackers[NiTextureEffect::WORLD_PARALLEL] = 
        &PackWorldParallelEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::WORLD_PERSPECTIVE] = 
        &PackWorldPerspectiveEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::SPHERE_MAP] = 
        &PackWorldSphereEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::SPECULAR_CUBE_MAP] = 
        &PackSpecularCubeEffect;
    ms_apfnDynEffectPackers[NiTextureEffect::DIFFUSE_CUBE_MAP] = 
        &PackDiffuseCubeEffect;
}
//---------------------------------------------------------------------------
void NiD3D10Shader::_SDMShutdown()
{
    ms_kEmergentShaderMapName = NULL;
}
//---------------------------------------------------------------------------
NiD3D10Shader::NiD3D10Shader() :
    m_uiCurrentPass(0),
    m_pkCurrentPass(NULL),
    m_uiBoneMatrixRegisters(4),
    m_bTransposeBones(false),
    m_bWorldSpaceBones(false),
    m_bUsesNiRenderState(false)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10Shader::~NiD3D10Shader()
{
    /* */
}
//---------------------------------------------------------------------------
bool NiD3D10Shader::IsInitialized()
{
    return m_bInitialized;
}
//---------------------------------------------------------------------------
bool NiD3D10Shader::Initialize()
{
    return NiD3D10ShaderInterface::Initialize();
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::PreProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    if (!m_bInitialized)
    {
        // Safety catch - fail if the shader hasn't been initialized
        return UINT_MAX;
    }

    NIASSERT(NiD3D10Renderer::GetRenderer());

    NiD3D10RenderStateManager* pkRenderState = 
        NiD3D10Renderer::GetRenderer()->GetRenderStateManager();

    NIASSERT(pkRenderState);

    if (m_bUsesNiRenderState)
        pkRenderState->SetProperties(pkState);

    if (m_spRenderStateGroup)
        pkRenderState->SetRenderStateGroup(m_spRenderStateGroup);

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::UpdatePipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::SetupRenderingPass(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    NIASSERT(m_pkCurrentPass);
    return m_pkCurrentPass->SetupRenderingPass(pkGeometry, pkSkin, 
        (NiD3D10GeometryData*)pkRendererData, pkState, pkEffects, kWorld, 
        kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::SetupTransformations(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    NIASSERT(NiD3D10Renderer::GetRenderer());
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();

    if (m_uiCurrentPass == 0/* || m_pkD3DRenderer->GetBatchRendering()*/)
    {
        if (pkPartition)
        {
            pkRenderer->CalculateBoneMatrices(pkSkin, kWorld, 
                m_bTransposeBones, m_uiBoneMatrixRegisters, 
                m_bWorldSpaceBones);
        }

        pkRenderer->SetModelTransform(kWorld);
    }

    return 0;
}
//---------------------------------------------------------------------------
NiGeometryData::RendererData* NiD3D10Shader::PrepareGeometryForRendering(
    NiGeometry* pkGeometry, const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState)
{
    NIASSERT(NiD3D10Renderer::GetRenderer());
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();

    NiD3D10GeometryData* pkD3D10GeomData = 
        (NiD3D10GeometryData*)pkRendererData;
    NIASSERT(pkD3D10GeomData);

    NiD3D10VertexDescription* pkDesc = 
        (NiD3D10VertexDescription*)pkGeometry->GetShaderDecl();
    if (!pkDesc)
    {
        NIVERIFY(NiShaderDeclaration::CreateDeclForGeometry(pkGeometry));
        pkDesc = (NiD3D10VertexDescription*)pkGeometry->GetShaderDecl();
    }

    // On the first pass, the geometry should be packed. This call is a 
    // safety net to catch geometry that was not precached.
    // If the geometry has already been packed, this part of the function 
    // will quick-out.
    // 
    if (pkGeometry != NULL && (m_uiCurrentPass == 0 || 
        (pkGeometry->GetConsistency() == NiGeometryData::VOLATILE &&
        pkPartition != NULL)))
    {
        pkRenderer->PackGeometry(pkD3D10GeomData, 
            pkGeometry->GetModelData(), pkGeometry->GetSkinInstance(), pkDesc);
    }

    // Note that D3D10 requires a vertex shader.
    NIASSERT(m_pkCurrentPass->GetVertexShader());

    // Update input layout with current vertex shader!
    ID3D10Blob* pkByteCode = 
        m_pkCurrentPass->GetVertexShader()->GetShaderByteCode();
    NIASSERT(pkByteCode);
    pkD3D10GeomData->UpdateInputLayout(pkByteCode->GetBufferPointer(),
        pkByteCode->GetBufferSize());

    return pkRendererData;
}
//---------------------------------------------------------------------------
NiGeometryData::RendererData* 
    NiD3D10Shader::PrepareGeometryForRendering_Points(
    NiGeometry* pkGeometry, const NiSkinPartition::Partition* pkPartition,
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState)
{
    NIASSERT(NiD3D10Renderer::GetRenderer());
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();

    NiD3D10GeometryData* pkD3D10GeomData = 
        (NiD3D10GeometryData*)pkRendererData;
    NIASSERT(pkD3D10GeomData);

    NiD3D10VertexDescription* pkDesc = 
        (NiD3D10VertexDescription*)pkGeometry->GetShaderDecl();
    if (!pkDesc)
    {
        // Temporary trick system into thinking normals, binormals, tangents,
        // and one set of texture coordinates are present.
        // If needed, they will be automatically generated.
        unsigned int uiCurrentFlags = pkD3D10GeomData->GetFlags();
        bool bHasColors = 
            NiD3D10GeometryData::GetHasColorsFromFlags(uiCurrentFlags);
        unsigned int uiTempFlags = NiD3D10GeometryData::CreateVertexFlags(
            bHasColors, true, true, 1);
        pkD3D10GeomData->SetFlags(uiTempFlags);

        NIVERIFY(NiShaderDeclaration::CreateDeclForGeometry(pkGeometry));
        pkDesc = (NiD3D10VertexDescription*)pkGeometry->GetShaderDecl();

        pkD3D10GeomData->SetFlags(uiCurrentFlags);
    }

    // On the first pass, the geometry should be packed. This call is a 
    // safety net to catch geometry that was not precached.
    // If the geometry has already been packed, this part of the function 
    // will quick-out.
    // 

    NIASSERT(pkPartition == NULL);
    if (pkGeometry != NULL && (m_uiCurrentPass == 0 || 
        (pkGeometry->GetConsistency() == NiGeometryData::VOLATILE &&
        pkPartition != NULL)))
    {
        // Need to calculate the model-space camera vectors
        const D3DXMATRIXA16& kInvView = pkRenderer->GetInverseViewMatrix();
        const NiMatrix3& kRotate =  pkGeometry->GetWorldRotate();
        NiPoint3 kModelCameraRight = kRotate * 
            NiPoint3(kInvView._11, kInvView._12, kInvView._13);
        NiPoint3 kModelCameraUp = kRotate * 
            NiPoint3(kInvView._21, kInvView._22, kInvView._23);

        pkRenderer->PackParticlesSprites(pkD3D10GeomData, 
            pkGeometry->GetModelData(), pkDesc, kModelCameraRight, 
            kModelCameraUp);
    }

    // Note that D3D10 requires a vertex shader.
    NIASSERT(m_pkCurrentPass->GetVertexShader());

    // Update input layout with current vertex shader!
    ID3D10Blob* pkByteCode = 
        m_pkCurrentPass->GetVertexShader()->GetShaderByteCode();
    NIASSERT(pkByteCode)
        pkD3D10GeomData->UpdateInputLayout(pkByteCode->GetBufferPointer(),
        pkByteCode->GetBufferSize());

    return pkRendererData;
}
//---------------------------------------------------------------------------
NiGeometryData::RendererData* NiD3D10Shader::PrepareGeometryForRendering_Lines(
    NiGeometry* pkGeometry, const NiSkinPartition::Partition* pkPartition,
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState)
{
    NIASSERT(NiD3D10Renderer::GetRenderer());
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();

    NiD3D10GeometryData* pkD3D10GeomData = 
        (NiD3D10GeometryData*)pkRendererData;
    NIASSERT(pkD3D10GeomData);

    NiD3D10VertexDescription* pkDesc = 
        (NiD3D10VertexDescription*)pkGeometry->GetShaderDecl();
    if (!pkDesc)
    {
        NIVERIFY(NiShaderDeclaration::CreateDeclForGeometry(pkGeometry));
        pkDesc = (NiD3D10VertexDescription*)pkGeometry->GetShaderDecl();
    }

    // On the first pass, the geometry should be packed. This call is a 
    // safety net to catch geometry that was not precached.
    // If the geometry has already been packed, this part of the function 
    // will quick-out.
    // 
    NIASSERT(pkPartition == NULL);
    if (pkGeometry != NULL && (m_uiCurrentPass == 0 || 
        (pkGeometry->GetConsistency() == NiGeometryData::VOLATILE &&
        pkPartition != NULL)))
    {
        pkRenderer->PackLines(pkD3D10GeomData, 
            pkGeometry->GetModelData(), pkDesc);
    }

    // Note that D3D10 requires a vertex shader.
    NIASSERT(m_pkCurrentPass->GetVertexShader());

    // Update input layout with current vertex shader!
    ID3D10Blob* pkByteCode = 
        m_pkCurrentPass->GetVertexShader()->GetShaderByteCode();
    NIASSERT(pkByteCode)
        pkD3D10GeomData->UpdateInputLayout(pkByteCode->GetBufferPointer(),
        pkByteCode->GetBufferSize());

    return pkRendererData;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::SetupShaderPrograms(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // If there is an NiSCMExtraData object on this geometry, we want to 
    // reset the iterator to 0 so we can hit that cache when we set 
    // attribute constants.
    ResetSCMExtraData(pkGeometry);

    // Setup the shader programs and constants for the current pass.
    // If this is the first pass, then also set the 'global' constants
    // and render states.
    // Set the shader constants
    NIASSERT(m_pkCurrentPass);
    unsigned int uiRet = m_pkCurrentPass->ApplyShaderPrograms(pkGeometry, 
        pkSkin, pkPartition, (NiD3D10GeometryData*)pkRendererData, pkState, 
        pkEffects, kWorld, kWorldBound, m_uiCurrentPass);

    // Now that the shaders have been set, if there are any 'global' 
    // mappings, set them now. This has to occur now since the pixel
    // shader has to be set prior to setting pixel shader constants.
    // This must be done every pass, since using a different shader on
    // a different pass may require constant remapping.
    NiShaderError eErr = NISHADERERR_OK;

    NIASSERT(NiD3D10Renderer::GetRenderer());

    NiD3D10ShaderConstantManager* pkShaderConstantManager = 
        NiD3D10Renderer::GetRenderer()->GetShaderConstantManager();
    NIASSERT(pkShaderConstantManager);

    // Vertex shader mappings
    NiD3D10VertexShader* pkVertexShader = m_pkCurrentPass->GetVertexShader();
    if (pkVertexShader)
    {
        const unsigned int uiMapCount = m_kVertexShaderConstantMaps.GetSize();
        NIASSERT(uiMapCount == 0);
        for (unsigned int i = 0; i < uiMapCount; i++)
        {
            NiD3D10ShaderConstantMap* pkMap = 
                m_kVertexShaderConstantMaps.GetAt(i);
            if (pkMap)
            {
                eErr = pkMap->UpdateShaderConstants(pkGeometry, pkSkin, 
                    pkPartition, (NiD3D10GeometryData*)pkRendererData, pkState,
                    pkEffects, kWorld, kWorldBound, m_uiCurrentPass, true);
                pkShaderConstantManager->SetShaderConstantMap(
                    pkVertexShader, pkMap);
            }
        }
    }

    // Geometry shader mappings
    NiD3D10GeometryShader* pkGeometryShader = 
        m_pkCurrentPass->GetGeometryShader();
    if (pkGeometryShader)
    {
        const unsigned int uiMapCount = 
            m_kGeometryShaderConstantMaps.GetSize();
        NIASSERT(uiMapCount == 0);
        for (unsigned int i = 0; i < uiMapCount; i++)
        {
            NiD3D10ShaderConstantMap* pkMap = 
                m_kGeometryShaderConstantMaps.GetAt(i);
            if (pkMap)
            {
                eErr = pkMap->UpdateShaderConstants(pkGeometry, pkSkin, 
                    pkPartition, (NiD3D10GeometryData*)pkRendererData, pkState,
                    pkEffects, kWorld, kWorldBound, m_uiCurrentPass, true);
                pkShaderConstantManager->SetShaderConstantMap(
                    pkGeometryShader, pkMap);
            }
        }
    }

    // Pixel shader mappings
    NiD3D10PixelShader* pkPixelShader = m_pkCurrentPass->GetPixelShader();
    if (pkPixelShader)
    {
        const unsigned int uiMapCount = m_kPixelShaderConstantMaps.GetSize();
        NIASSERT(uiMapCount == 0);
        for (unsigned int i = 0; i < uiMapCount; i++)
        {
            NiD3D10ShaderConstantMap* pkMap = 
                m_kPixelShaderConstantMaps.GetAt(i);
            if (pkMap)
            {
                eErr = pkMap->UpdateShaderConstants(pkGeometry, pkSkin, 
                    pkPartition, (NiD3D10GeometryData*)pkRendererData, pkState,
                    pkEffects, kWorld, kWorldBound, m_uiCurrentPass, true);
                pkShaderConstantManager->SetShaderConstantMap(
                    pkPixelShader, pkMap);
            }
        }
    }

    uiRet = m_pkCurrentPass->ApplyShaderConstants(pkGeometry, 
        pkSkin, pkPartition, (NiD3D10GeometryData*)pkRendererData, pkState, 
        pkEffects, kWorld, kWorldBound, m_uiCurrentPass);

    return uiRet;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::PostProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return 0;
}
//---------------------------------------------------------------------------
bool NiD3D10Shader::GetVertexInputSemantics(
    NiShaderDeclaration::ShaderRegisterEntry* pkSemantics)
{
    // Note that due to limitations on the GetShaderDecl function at the time
    // of authoring, this function could not be promoted into the base class.
    // If you modify this code, please insure that all platform renderers
    // are updated appropriately.
    NiD3D10VertexDescription* pkDesc = GetVertexDescription();
    if (!pkDesc)
    {
        pkSemantics[0].m_uiUsageIndex = NiMaterial::VS_INPUTS_USE_GEOMETRY;
        return false;
    }

    unsigned int uiStreamCount = pkDesc->GetStreamCount();
    unsigned int uiEntryCount = 0;
    unsigned int uiSemanticCount = 0;
    const NiShaderDeclaration::ShaderRegisterEntry* pkEntry = NULL;

    for (unsigned int ui = 0; ui < uiStreamCount; ++ui)
    {
        uiEntryCount = pkDesc->GetEntryCount(ui);
        for (unsigned int uj = 0; uj < uiEntryCount; ++uj)
        {
            // uj counts entries, and GetEntry takes the entry arg first.
            pkEntry = pkDesc->GetEntry(uj, ui);
            if (!pkEntry)
                continue;

            pkSemantics[uiSemanticCount].m_uiPreferredStream = ui;
            pkSemantics[uiSemanticCount].m_eInput = pkEntry->m_eInput;
            pkSemantics[uiSemanticCount].m_eType = pkEntry->m_eType;
            pkSemantics[uiSemanticCount].m_kUsage = pkEntry->m_kUsage;
            pkSemantics[uiSemanticCount].m_uiUsageIndex = 
                pkEntry->m_uiUsageIndex; 
            pkSemantics[uiSemanticCount].m_uiExtraData = 
                pkEntry->m_uiExtraData;
            ++uiSemanticCount;

            if (uiSemanticCount == NiMaterial::VS_INPUTS_MAX_NUM - 1)
                break;
        }
        if (uiSemanticCount == NiMaterial::VS_INPUTS_MAX_NUM - 1)
            break;
    }

    // Terminate the array.
    pkSemantics[uiSemanticCount].m_uiUsageIndex = 
        NiMaterial::VS_INPUTS_TERMINATE_ARRAY;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::FirstPass()
{
    m_uiCurrentPass = 0;
    unsigned int uiPassCount = m_kPasses.GetSize();
    if (m_uiCurrentPass < uiPassCount)
        m_pkCurrentPass = m_kPasses.GetAt(m_uiCurrentPass);
    else
        m_pkCurrentPass = NULL;

    return uiPassCount;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Shader::NextPass()
{
    // Close out the current pass
    if (m_pkCurrentPass != NULL)
        m_pkCurrentPass->PostProcessRenderingPass(m_uiCurrentPass);

    m_uiCurrentPass++;
    unsigned int uiPassCount = m_kPasses.GetSize();
    unsigned int uiRemainingPasses = 0;
    if (m_uiCurrentPass < uiPassCount)
    {
        m_pkCurrentPass = m_kPasses.GetAt(m_uiCurrentPass);
        uiRemainingPasses = uiPassCount - m_uiCurrentPass; 
    }
    else
    {
        m_pkCurrentPass = NULL;
    }

    return uiRemainingPasses;
}
//---------------------------------------------------------------------------
bool NiD3D10Shader::SetupGeometry(NiGeometry* pkGeometry)
{
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10Shader::SetupSCMExtraData(NiD3D10Shader* pkShader, 
    NiGeometry* pkGeometry)
{
    // Remove any previous instances of the extra data cache.
    pkGeometry->RemoveExtraData(ms_kEmergentShaderMapName);

    // Determine the number of entries and allocate a new instance so that
    // we get a tightly packed array of entries.
    unsigned int uiNumVertexEntries = 0;
    unsigned int uiNumGeometryEntries = 0;
    unsigned int uiNumPixelEntries = 0;
    unsigned int uiCount = 0;

    // Establish the number of entries in the global constant map times the
    // number of passes.

    // Vertex Shader Constants
    const unsigned int uiVSMapCount = 
        pkShader->m_kVertexShaderConstantMaps.GetSize();
    unsigned int i = 0;
    for (; i < uiVSMapCount; i++)
    {
        NiD3D10ShaderConstantMap* pkMap = 
            pkShader->m_kVertexShaderConstantMaps.GetAt(i);
        if (pkMap)
        {
            unsigned int uiEntryCount = pkMap->GetEntryCount();
            for (unsigned int j = 0; j < uiEntryCount; j++)
            {
                NiShaderConstantMapEntry* pkEntry = pkMap->GetEntryAtIndex(j);
                if (pkEntry && pkEntry->IsAttribute())
                    ++uiNumVertexEntries;
            }
        }
    }
    // If the passes array is not tightly packed, this could cause some
    // waste in the arrays, but that is not likely.
    uiNumVertexEntries *= pkShader->m_kPasses.GetSize();

    // Geometry Shader Constants
    const unsigned int uiGSMapCount = 
        pkShader->m_kGeometryShaderConstantMaps.GetSize();
    for (i = 0; i < uiGSMapCount; i++)
    {
        NiD3D10ShaderConstantMap* pkMap = 
            pkShader->m_kGeometryShaderConstantMaps.GetAt(i);
        if (pkMap)
        {
            unsigned int uiEntryCount = pkMap->GetEntryCount();
            for (unsigned int j = 0; j < uiEntryCount; j++)
            {
                NiShaderConstantMapEntry* pkEntry = pkMap->GetEntryAtIndex(j);
                if (pkEntry && pkEntry->IsAttribute())
                    ++uiNumGeometryEntries;
            }
        }
    }
    // If the passes array is not tightly packed, this could cause some
    // waste in the arrays, but that is not likely.
    uiNumGeometryEntries *= pkShader->m_kPasses.GetSize();

    // Pixel Shader Constants
    const unsigned int uiPSMapCount = 
        pkShader->m_kPixelShaderConstantMaps.GetSize();
    for (i = 0; i < uiPSMapCount; i++)
    {
        NiD3D10ShaderConstantMap* pkMap = 
            pkShader->m_kPixelShaderConstantMaps.GetAt(i);
        if (pkMap)
        {
            unsigned int uiEntryCount = pkMap->GetEntryCount();
            for (unsigned int j = 0; j < uiEntryCount; j++)
            {
                NiShaderConstantMapEntry* pkEntry = pkMap->GetEntryAtIndex(j);
                if (pkEntry && pkEntry->IsAttribute())
                    ++uiNumPixelEntries;
            }
        }
    }
    // If the passes array is not tightly packed, this could cause some
    // waste in the arrays, but that is not likely.
    uiNumPixelEntries *= pkShader->m_kPasses.GetSize();

    unsigned int uiPasses;
    unsigned int uiNumPasses = pkShader->m_kPasses.GetSize();
    for (uiPasses = 0; uiPasses < uiNumPasses; uiPasses++)
    {
        NiD3D10Pass* pkPass = pkShader->m_kPasses.GetAt(uiPasses);
        if (pkPass)
        {
            // Check each pass for constant maps and then increment the entry
            // count if that entry is an attribute type entry.
            const unsigned int uiVSPerPassMapCount = 
                pkPass->GetVertexConstantMapCount();
            for (i = 0; i < uiVSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetVertexConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                            ++uiNumVertexEntries;
                    }
                }
            }

            const unsigned int uiGSPerPassMapCount = 
                pkPass->GetGeometryConstantMapCount();
            for (i = 0; i < uiGSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetGeometryConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                            ++uiNumGeometryEntries;
                    }
                }
            }

            const unsigned int uiPSPerPassMapCount = 
                pkPass->GetPixelConstantMapCount();
            for (i = 0; i < uiPSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetPixelConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                            ++uiNumPixelEntries;
                    }
                }
            }
        }
    }

    // Create the extra data cache table if necessary. We don't want to waste
    // time if there are no attribute type constant map entries.
    NiSCMExtraData* pkShaderExtraData = 0;
    if (uiNumVertexEntries > 0 || uiNumGeometryEntries > 0 ||
        uiNumPixelEntries > 0)
    {
        pkShaderExtraData = NiNew NiSCMExtraData(ms_kEmergentShaderMapName, 
            uiNumVertexEntries, uiNumGeometryEntries, uiNumPixelEntries);
    }
    else
    {
        return;
    }

    // Populate the extra data with pointers. Again, we only insert entries
    // into our cache when the entry in the constant map is of the attribute
    // type.
    for (uiPasses = 0; uiPasses < uiNumPasses; uiPasses++)
    {
        NiD3D10Pass* pkPass = pkShader->m_kPasses.GetAt(uiPasses);
        if (pkPass)
        {
            // Add global constant maps per pass so that we hit them
            // multiple times when iterating through the lists.
            for (i = 0; i < uiVSMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkShader->m_kVertexShaderConstantMaps.GetAt(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkGeometry->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(), 0, 
                                    NiGPUProgram::PROGRAM_VERTEX, pkExtra, 
                                    true);
                            }
                        }
                    }
                }
            }

            for (i = 0; i < uiGSMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkShader->m_kGeometryShaderConstantMaps.GetAt(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkGeometry->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(), 0, 
                                    NiGPUProgram::PROGRAM_GEOMETRY, pkExtra, 
                                    true);
                            }
                        }
                    }
                }
            }

            for (i = 0; i < uiPSMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkShader->m_kPixelShaderConstantMaps.GetAt(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkGeometry->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(), 0, 
                                    NiGPUProgram::PROGRAM_PIXEL, pkExtra, 
                                    true);
                            }
                        }
                    }
                }
            }

            // Add per pass shader constants.
            const unsigned int uiVSPerPassMapCount = 
                pkPass->GetVertexConstantMapCount();
            for (i = 0; i < uiVSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetVertexConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkGeometry->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(),
                                    0, NiGPUProgram::PROGRAM_VERTEX,
                                    pkExtra, false);
                            }
                        }
                    }
                }
            }

            const unsigned int uiGSPerPassMapCount = 
                pkPass->GetGeometryConstantMapCount();
            for (i = 0; i < uiGSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetGeometryConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkGeometry->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(),
                                    0, NiGPUProgram::PROGRAM_GEOMETRY,
                                    pkExtra, false);
                            }
                        }
                    }
                }
            }

            const unsigned int uiPSPerPassMapCount = 
                pkPass->GetPixelConstantMapCount();
            for (i = 0; i < uiPSPerPassMapCount; i++)
            {
                NiD3D10ShaderConstantMap* pkMap = 
                    pkPass->GetPixelConstantMap(i);
                if (pkMap)
                {
                    unsigned int uiEntryCount = pkMap->GetEntryCount();
                    for (unsigned int j = 0; j < uiEntryCount; j++)
                    {
                        NiShaderConstantMapEntry* pkEntry = 
                            pkMap->GetEntryAtIndex(j);
                        if (pkEntry && pkEntry->IsAttribute())
                        {
                            NiExtraData* pkExtra = 
                                pkGeometry->GetExtraData(pkEntry->GetKey());
                            if (pkExtra)
                            {
                                pkShaderExtraData->AddEntry(
                                    pkEntry->GetShaderRegister(),
                                    0, NiGPUProgram::PROGRAM_PIXEL,
                                    pkExtra, false);
                            }
                        }
                    }
                }
            }
        }
    }

    // Attach the NiSCMExtraData object which holds our cached values.
    pkGeometry->AddExtraData(pkShaderExtraData);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::ResetSCMExtraData(NiGeometry* pkGeometry)
{
    if (pkGeometry)
    {
        NiSCMExtraData* pkShaderData = 
            (NiSCMExtraData*)
            pkGeometry->GetExtraData(NiD3D10Shader::ms_kEmergentShaderMapName);
        if (pkShaderData)
            pkShaderData->Reset();
    }
}
//---------------------------------------------------------------------------
void NiD3D10Shader::DestroyRendererData()
{
    const unsigned int uiPassCount = m_kPasses.GetSize();
    for (unsigned int i = 0; i < uiPassCount; i++)
    {
        NiD3D10Pass* pkPass = m_kPasses.GetAt(i);
        if (pkPass)
        {
            NiD3D10VertexShader* pkVS = pkPass->GetVertexShader();
            if (pkVS)
                pkVS->DestroyRendererData();

            NiD3D10GeometryShader* pkGS = pkPass->GetGeometryShader();
            if (pkGS)
                pkGS->DestroyRendererData();

            NiD3D10PixelShader* pkPS = pkPass->GetPixelShader();
            if (pkPS)
                pkPS->DestroyRendererData();
        }
    }
    m_bInitialized = false;
}
//---------------------------------------------------------------------------
void NiD3D10Shader::RecreateRendererData()
{
    if (NiD3D10Renderer::GetRenderer() == NULL)
    {
        NIASSERT(!"NiD3D10Shader::RecreateRendererData> Invalid renderer!");
        return;
    }

    const unsigned int uiPassCount = m_kPasses.GetSize();
    for (unsigned int i = 0; i < uiPassCount; i++)
    {
        NiD3D10Pass* pkPass = m_kPasses.GetAt(i);
        if (pkPass)
        {
            NiD3D10VertexShader* pkVS = pkPass->GetVertexShader();
            if (pkVS)
                pkVS->RecreateRendererData();

            NiD3D10GeometryShader* pkGS = pkPass->GetGeometryShader();
            if (pkGS)
                pkGS->RecreateRendererData();

            NiD3D10PixelShader* pkPS = pkPass->GetPixelShader();
            if (pkPS)
                pkPS->RecreateRendererData();
        }
    }
    m_bInitialized = true;
}//---------------------------------------------------------------------------
bool NiD3D10Shader::GetUsesNiRenderState() const
{
    return m_bUsesNiRenderState;
}
//---------------------------------------------------------------------------
bool NiD3D10Shader::SetUsesNiRenderState(bool bUses)
{
    m_bUsesNiRenderState = bUses;
    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Shader::GetBoneParameters(bool& bTransposeBones, 
    unsigned int& uiBoneMatrixRegisters, bool& bWorldSpaceBones)
{
    bTransposeBones = m_bTransposeBones;
    uiBoneMatrixRegisters = m_uiBoneMatrixRegisters;
    bWorldSpaceBones = m_bWorldSpaceBones;
}
//---------------------------------------------------------------------------
void NiD3D10Shader::SetBoneParameters(bool bTransposeBones, 
    unsigned int uiBoneMatrixRegisters, bool bWorldSpaceBones)
{
    m_bTransposeBones = bTransposeBones;
    m_uiBoneMatrixRegisters = uiBoneMatrixRegisters;
    m_bWorldSpaceBones = bWorldSpaceBones;
}
//---------------------------------------------------------------------------
const NiFixedString& NiD3D10Shader::GetEmergentShaderMapName()
{
    return ms_kEmergentShaderMapName;
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackDynamicEffect(
    const NiTextureEffect* pkTextureEffect, NiD3D10Pass* pkPass, 
    unsigned int uiSampler)
{
    ms_apfnDynEffectPackers[(unsigned int)
        pkTextureEffect->GetTextureCoordGen()](
        pkTextureEffect->GetWorldProjectionMatrix(),
        pkTextureEffect->GetWorldProjectionTranslation(), pkPass, uiSampler);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::UseWorldSpaceSphereMaps(bool bWorldSpace)
{
    ms_apfnDynEffectPackers[NiTextureEffect::SPHERE_MAP] =
        (bWorldSpace ? &PackWorldSphereEffect : &PackCameraSphereEffect);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackWorldParallelEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3D10Pass* pkPass, unsigned int uiSampler)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackWorldPerspectiveEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3D10Pass* pkPass, unsigned int uiSampler)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackWorldSphereEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3D10Pass* pkPass, unsigned int uiSampler)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackCameraSphereEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3D10Pass* pkPass, unsigned int uiSampler)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackSpecularCubeEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3D10Pass* pkPass, unsigned int uiSampler)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiD3D10Shader::PackDiffuseCubeEffect(const NiMatrix3& kWorldMat,
    const NiPoint3& kWorldTrans, NiD3D10Pass* pkPass, unsigned int uiSampler)
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
