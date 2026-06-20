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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NiD3D10BinaryShader.h"
#include "NSBD3D10Utility.h"

#include <NiD3D10Renderer.h>
#include <NiD3D10RenderStateManager.h>
#include <NiD3D10ShaderProgramFactory.h>
#include <NiTimeSyncController.h>


NiImplementRTTI(NiD3D10BinaryShader, NiD3D10Shader);

//---------------------------------------------------------------------------
NiD3D10BinaryShader::NiD3D10BinaryShader() :
    NiD3D10Shader(),
    m_uiBonesPerPartition(0), 
    m_uiBoneMatrixRegisters(0), 
    m_eBinormalTangentMethod(NiGeometryData::NBT_METHOD_NONE),
    m_uiBinormalTangentUVSource(NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT),
    m_eBoneCalcMethod(BONECALC_SKIN),
    m_uiPassCount(0)
{
    for (unsigned int ui = 0; ui < m_kPasses.GetAllocatedSize(); ui++)
        m_kPasses.SetAt(ui, 0);
}
//---------------------------------------------------------------------------
NiD3D10BinaryShader::~NiD3D10BinaryShader()
{
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::Initialize()
{
    if (m_bInitialized)
        return true;

    // Cycle through the passes and load the shader programs.  That step
    // should be all that's needed at this point.
    NiD3D10Pass* pkPass;
    unsigned int uiCount = m_kPasses.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkPass = m_kPasses.GetAt(ui);
        if (pkPass)
        {
            if (!LoadVertexShaderProgram(*pkPass))
            {
                // Implement a failure response.
            }
            if (!LoadGeometryShaderProgram(*pkPass))
            {
                // Implement a failure response.
            }
            if (!LoadPixelShaderProgram(*pkPass))
            {
                // Implement a failure response.
            }
        }
    }

    // All bone matrices will be transposed.
    SetBoneParameters(true, GetBoneMatrixRegisters(),
        GetBoneCalcMethod() == BONECALC_NO_SKIN);
    
    if (!NiD3D10Shader::Initialize())
        return false;

    m_bInitialized = true;
    return m_bInitialized;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10BinaryShader::UpdatePipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin,
    NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    for (unsigned int uiPass = 0; uiPass < m_kPasses.GetSize(); uiPass++)
    {
        NiD3D10Pass* pkPass = m_kPasses.GetAt(uiPass);
        if (!pkPass)
        {
            continue;
        }

        const NiTexturingProperty* pkTexProp = pkState->GetTexturing();

        const unsigned int uiSamplerCount = pkPass->GetActiveSamplerCount();
        NIASSERT(uiSamplerCount == pkPass->GetActiveTextureCount());

        for (unsigned int i = 0; i < uiSamplerCount; i++)
        {
            PrepareTexture(pkGeometry, pkPass, i, pkTexProp, pkEffects, 
                uiPass);
        }
    }

    return NiD3D10Shader::UpdatePipeline(pkGeometry, pkSkin, pkRendererData, 
        pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
void NiD3D10BinaryShader::PrepareTexture(NiGeometry* pkGeometry, 
    NiD3D10Pass* pkPass, unsigned int uiTexture,
    const NiTexturingProperty* pkTexProp, 
    const NiDynamicEffectState* pkEffects, unsigned int uiPass)
{
    NIASSERT(pkPass != 0 && uiTexture < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);

    // For the time being, assume that the texture ID and the sampler ID
    // correspond directly.

    NiTexture* pkTexture = NULL;
    NiTexturingProperty::ClampMode eClampMode =
        NiTexturingProperty::CLAMP_S_CLAMP_T;
    NiTexturingProperty::FilterMode eFilterMode =
        NiTexturingProperty::FILTER_NEAREST;

    unsigned int uiGBMapFlags = pkPass->GetGamebryoMapFlags(uiTexture);
    unsigned short usObjectTextureFlags = 
        pkPass->GetObjectTextureFlags(uiTexture);

    if (usObjectTextureFlags == NiD3D10Pass::OTF_IGNORE)
    {
        if (uiGBMapFlags != NiD3D10Pass::GB_MAP_IGNORE)
        {
            const NiTexturingProperty::Map* pkMap = NULL;
            NIASSERT(pkTexProp);
            if ((uiGBMapFlags & NiD3D10Pass::GB_MAP_TYPEMASK) != 0)
            {
                switch (uiGBMapFlags & NiD3D10Pass::GB_MAP_TYPEMASK)
                {
                case NiD3D10Pass::GB_MAP_NONE:
                    break;
                case NiD3D10Pass::GB_MAP_BASE:
                    pkMap = pkTexProp->GetBaseMap();
                    break;
                case NiD3D10Pass::GB_MAP_DARK:
                    pkMap = pkTexProp->GetDarkMap();
                    break;
                case NiD3D10Pass::GB_MAP_DETAIL:
                    pkMap = pkTexProp->GetDetailMap();
                    break;
                case NiD3D10Pass::GB_MAP_GLOSS:
                    pkMap = pkTexProp->GetGlossMap();
                    break;
                case NiD3D10Pass::GB_MAP_GLOW:
                    pkMap = pkTexProp->GetGlowMap();
                    break;
                case NiD3D10Pass::GB_MAP_BUMP:
                    pkMap = pkTexProp->GetBumpMap();
                    break;
                case NiD3D10Pass::GB_MAP_NORMAL:
                    pkMap = pkTexProp->GetNormalMap();
                    break;
                case NiD3D10Pass::GB_MAP_PARALLAX:
                    pkMap = pkTexProp->GetParallaxMap();
                    break;
                case NiD3D10Pass::GB_MAP_DECAL:
                {
                    unsigned int uiIndex = uiGBMapFlags &
                        NiD3D10Pass::GB_MAP_INDEX_MASK;
                    if (uiIndex < pkTexProp->GetDecalArrayCount())
                        pkMap = pkTexProp->GetDecalMap(uiIndex);
                    break;
                }
                case NiD3D10Pass::GB_MAP_SHADER:
                {
                    unsigned int uiIndex = uiGBMapFlags &
                        NiD3D10Pass::GB_MAP_INDEX_MASK;
                    if (uiIndex < pkTexProp->GetShaderArrayCount())
                        pkMap = pkTexProp->GetShaderMap(uiIndex);
                    break;
                }
                }
            }

            if (pkMap)
            {
                pkTexture = pkMap->GetTexture();
                eClampMode = pkMap->GetClampMode();
                eFilterMode = pkMap->GetFilterMode();

                // Note that the texture coordinate index is ignored, since
                // that value must be specified in the pixel shader.
            }
        }
    }
    else if (pkEffects)
    {
        NiTextureEffect* pkTextureEffect = NULL;
        unsigned short usObjectIndex = usObjectTextureFlags &
            NiD3D10Pass::OTF_INDEX_MASK;
        switch ((usObjectTextureFlags & NiD3D10Pass::OTF_TYPE_MASK) >>
            NiD3D10Pass::OTF_TYPE_SHIFT)
        {
        case NiShaderAttributeDesc::OT_EFFECT_DIRSHADOWMAP:
        {
            NiLight* pkLight = (NiLight*)
                NiShaderConstantMap::GetDynamicEffectForObject(pkEffects,
                NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT, 
                usObjectIndex);

            NIASSERT(pkLight);

            NiShadowGenerator* pkGenerator = pkLight->GetShadowGenerator();
            NIASSERT(pkGenerator);

            NiShadowMap* pkShadowMap = 
                pkGenerator->RetrieveShadowMap(
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, pkGeometry);
            NIASSERT(pkShadowMap);

            pkTexture = pkShadowMap->GetTexture();
            eClampMode = pkShadowMap->GetClampMode();
            eFilterMode = pkShadowMap->GetFilterMode();
            break;
        }
        case NiShaderAttributeDesc::OT_EFFECT_POINTSHADOWMAP:
        {
            NiLight* pkLight = (NiLight*)
                NiShaderConstantMap::GetDynamicEffectForObject(
                pkEffects, NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT, 
                usObjectIndex);
            NIASSERT(pkLight);

            NiShadowGenerator* pkGenerator = pkLight->GetShadowGenerator();
            NIASSERT(pkGenerator);

            NiShadowMap* pkShadowMap = 
                pkGenerator->RetrieveShadowMap(
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, pkGeometry);
            NIASSERT(pkShadowMap);

            pkTexture = pkShadowMap->GetTexture();
            eClampMode = pkShadowMap->GetClampMode();
            eFilterMode = pkShadowMap->GetFilterMode();
            break;
        }
        case NiShaderAttributeDesc::OT_EFFECT_SPOTSHADOWMAP:
        {
            NiLight* pkLight = (NiLight*)
                NiShaderConstantMap::GetDynamicEffectForObject(
                pkEffects, NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT, 
                usObjectIndex);
            NIASSERT(pkLight);

            NiShadowGenerator* pkGenerator = pkLight->GetShadowGenerator();
            NIASSERT(pkGenerator);

            NiShadowMap* pkShadowMap = 
                pkGenerator->RetrieveShadowMap(
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, pkGeometry);
            NIASSERT(pkShadowMap);

            pkTexture = pkShadowMap->GetTexture();
            eClampMode = pkShadowMap->GetClampMode();
            eFilterMode = pkShadowMap->GetFilterMode();
            break;
        }
        case NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP:
            pkTextureEffect = pkEffects->GetEnvironmentMap();
            break;
        case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP:
        {
            unsigned short usIndex = 0;
            NiDynEffectStateIter kIter =
                pkEffects->GetProjShadowHeadPos();
            while (kIter)
            {
                NiTextureEffect* pkProjShadow =
                    pkEffects->GetNextProjShadow(kIter);
                if (pkProjShadow && pkProjShadow->GetSwitch() == true)
                {
                    if (usIndex++ == usObjectIndex)
                    {
                        pkTextureEffect = pkProjShadow;
                        break;
                    }
                }
            }
            break;
        }
        case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP:
        {
            unsigned short usIndex = 0;
            NiDynEffectStateIter kIter =
                pkEffects->GetProjLightHeadPos();
            while (kIter)
            {
                NiTextureEffect* pkProjLight =
                    pkEffects->GetNextProjLight(kIter);
                if (pkProjLight && pkProjLight->GetSwitch() == true)
                {
                    if (usIndex++ == usObjectIndex)
                    {
                        pkTextureEffect = pkProjLight;
                        break;
                    }
                }
            }
            break;
        }
        case NiShaderAttributeDesc::OT_EFFECT_FOGMAP:
            pkTextureEffect = pkEffects->GetFogMap();
            break;
        default:
            // This assertion is hit when the object type is not one
            // that has a texture.
            NIASSERT(false);
            break;
        }

        if (pkTextureEffect && pkTextureEffect->GetSwitch())
        {
            pkTexture = pkTextureEffect->GetEffectTexture();
            eClampMode = pkTextureEffect->GetTextureClamp();
            eFilterMode = pkTextureEffect->GetTextureFilter();
        }
    }

    // Even if the texture is NULL, we need to set that here to prevent a 
    // stale texture from getting used by accident.
    pkPass->SetPixelTexture(uiTexture, pkTexture);

    NiD3D10RenderStateGroup* pkRSGroup = pkPass->GetRenderStateGroup();
    if (pkRSGroup == NULL)
    {
        pkRSGroup = NiNew NiD3D10RenderStateGroup;
        pkPass->SetRenderStateGroup(pkRSGroup);
    }

    // Check to see if RenderStateGroup already defines these parameters
    // before filling in the version from the texturing property.
    D3D10_FILTER eTempFilter;
    if (pkRSGroup->GetSamplerFilter(NiGPUProgram::PROGRAM_PIXEL, uiTexture, 
        eTempFilter) == false)
    {
        pkRSGroup->SetSamplerFilter(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
            NiD3D10RenderStateManager::ConvertGbFilterModeToD3D10Filter(
            eFilterMode));

        bool bMipmapEnable = 
            NiD3D10RenderStateManager::ConvertGbFilterModeToMipmapEnable(
            eFilterMode);

        float fMaxLOD = (bMipmapEnable ? D3D10_FLOAT32_MAX : 0.0f);
        pkRSGroup->SetSamplerMaxLOD(NiGPUProgram::PROGRAM_PIXEL, uiTexture, 
            fMaxLOD);
    }

    D3D10_TEXTURE_ADDRESS_MODE eTempAddress;
    if (pkRSGroup->GetSamplerAddressU(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
        eTempAddress) == false)
    {
        pkRSGroup->SetSamplerAddressU(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
            NiD3D10RenderStateManager::ConvertGbClampModeToD3D10AddressU(
            eClampMode));
    }

    if (pkRSGroup->GetSamplerAddressV(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
        eTempAddress) == false)
    {
        pkRSGroup->SetSamplerAddressV(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
            NiD3D10RenderStateManager::ConvertGbClampModeToD3D10AddressV(
            eClampMode));
    }
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::SetupGeometry(NiGeometry* pkGeometry)
{
    if (!pkGeometry)
        return false;

    // Ensure that there is BinormalTangent data if required
    if (m_eBinormalTangentMethod != NiGeometryData::NBT_METHOD_NONE)
    {
        // Check the geoemtry data's method versus ours
        if (pkGeometry->GetNormalBinormalTangentMethod() !=
            m_eBinormalTangentMethod)
        {
            NIASSERT(!"SetupGeometry>  BinormalTangent method conflict!");
        }
    }

    //-----------------------------------------------------------------------

    // Add a 'dummy' alpha property if one is needed.

    NiAlphaProperty* pkAlphaProperty = NULL;

    // Check the render state group for any alpha blend among the render
    // targets.
    if (m_spRenderStateGroup)
    {
        bool bAlphaBlendEnabled = false;
        for (unsigned int i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;
            i++)
        {
            bool bBlendEnable = false;
            if (m_spRenderStateGroup->GetBSBlendEnable(i, bBlendEnable))
            {
                if (bBlendEnable)
                {
                    bAlphaBlendEnabled = true;
                    break;
                }
            }
        }

        // If it's enabled, set it in the property.
        if (bAlphaBlendEnabled)
        {
            if (!pkAlphaProperty)
            {
                pkAlphaProperty = NiNew NiAlphaProperty();
                NIASSERT(pkAlphaProperty);
            }
            pkAlphaProperty->SetAlphaBlending(true);
        }
    }

    // Check the first pass afterwards, as it can override the global
    NiD3D10Pass* pkPass = m_kPasses.GetAt(0);
    if (pkPass)
    {
        bool bAlphaBlendEnabled = false;
        for (unsigned int i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;
            i++)
        {
            bool bBlendEnable = false;
            NiD3D10RenderStateGroup* pkRSGroup
                = pkPass->GetRenderStateGroup();
            if (pkRSGroup && pkRSGroup->GetBSBlendEnable(i, bBlendEnable))
            {
                if (bBlendEnable)
                {
                    bAlphaBlendEnabled = true;
                    break;
                }
            }
        }

        // If it's enabled, set it in the property.
        if (bAlphaBlendEnabled)
        {
            if (!pkAlphaProperty)
            {
                pkAlphaProperty = NiNew NiAlphaProperty();
                NIASSERT(pkAlphaProperty);
            }
            pkAlphaProperty->SetAlphaBlending(true);
        }
    }

    if (pkAlphaProperty)
    {
        NiAlphaProperty* pkAttached = 
            (NiAlphaProperty*)pkGeometry->GetProperty(NiProperty::ALPHA);
        if (pkAttached)
        {
            if (!pkAttached->GetAlphaBlending() && 
                !pkAttached->GetAlphaTesting())
            {
                pkGeometry->RemoveProperty(NiProperty::ALPHA);
                pkGeometry->AttachProperty(pkAlphaProperty);
            }
            else
            {
                NiDelete pkAlphaProperty;
            }
        }
        else
        {
            pkGeometry->AttachProperty(pkAlphaProperty);
        }
    }

    //-----------------------------------------------------------------------

    // See if there is a 'time' attribute required.  Cycle over each pass and
    // check the contant maps for occurrences of one of the time-based
    // pre-defines.  If any of the vertex, geometry, or pixel shader constant
    // maps uses time, then time is required.

    bool bTimeRequired = false;

    // First, check the 'global' attributes (constant maps per shader).
    for (unsigned int i = 0; i < GetVertexConstantMapCount(); i++)
    {
        if (IsTimeBased(GetVertexConstantMap(i)))
        {
            bTimeRequired = true;
            break;
        }
    }
    if (!bTimeRequired)
    {
        for (unsigned int i = 0; i < GetGeometryConstantMapCount(); i++)
        {
            if (IsTimeBased(GetGeometryConstantMap(i)))
            {
                bTimeRequired = true;
                break;
            }
        }
    }
    if (!bTimeRequired)
    {
        for (unsigned int i = 0; i < GetPixelConstantMapCount(); i++)
        {
            if (IsTimeBased(GetPixelConstantMap(i)))
            {
                bTimeRequired = true;
                break;
            }
        }
    }

    if (!bTimeRequired) // Next, check each pass.
    {
        for (unsigned int uiPass = 0; uiPass < m_kPasses.GetSize(); uiPass++)
        {
            pkPass = m_kPasses.GetAt(uiPass);
            if (pkPass)
            {
                // Check each pass for vertex, geometry, and pixel constant
                // maps.
                for (unsigned int i = 0;
                    i < pkPass->GetVertexConstantMapCount(); i++)
                {
                    if (IsTimeBased(pkPass->GetVertexConstantMap(i)))
                    {
                        bTimeRequired = true;
                        break;
                    }
                }
                for (unsigned int i = 0;
                    i < pkPass->GetGeometryConstantMapCount(); i++)
                {
                    if (IsTimeBased(pkPass->GetGeometryConstantMap(i)))
                    {
                        bTimeRequired = true;
                        break;
                    }
                }
                for (unsigned int i = 0;
                    i < pkPass->GetPixelConstantMapCount(); i++)
                {
                    if (IsTimeBased(pkPass->GetPixelConstantMap(i)))
                    {
                        bTimeRequired = true;
                        break;
                    }
                }
            }
            if (bTimeRequired)
                break;
        }
    }

    if (bTimeRequired)
    {
        NiTimeSyncController* pkTimeCtrlr = NiNew NiTimeSyncController();
        NIASSERT(pkTimeCtrlr);
        pkTimeCtrlr->SetTarget(pkGeometry);

        // Ensure object will be updated by UpdateSelected.
        NiAVObject* pkObject = pkGeometry;
        while (pkObject != NULL)
        {
            pkObject->SetSelectiveUpdate(true);
            pkObject = (NiAVObject*)(pkObject->GetParent());
        }
    }

    return NiD3D10Shader::SetupGeometry(pkGeometry);
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::IsTimeBased(NiD3D10ShaderConstantMap* pkMap)
{
    if (pkMap)
    {
        if (pkMap->GetEntry("time") || pkMap->GetEntry("sin_time") ||
            pkMap->GetEntry("cos_time") || pkMap->GetEntry("tan_time") ||
            pkMap->GetEntry("time_sin_cos_tan"))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::LoadVertexShaderProgram(NiD3D10Pass& kPass)
{
    const char* pcProgramFileName = kPass.GetVertexShaderProgramFileName();

    // If there is no program to load, it's not a failure.
    // Just return true!
    if (pcProgramFileName == NULL || *pcProgramFileName == '\0')
        return true;

    char acTrueFileName[_MAX_PATH];
    if (!ResolveVertexShaderFileName(pcProgramFileName, acTrueFileName,
        _MAX_PATH))
    {
        return false;
    }

    // Construct a shader name from the shader file and entry point
    const char* pcShaderEntryPoint = 
        kPass.GetVertexShaderProgramEntryPoint();
    char acShaderName[_MAX_PATH];
    if (pcShaderEntryPoint != NULL && *pcShaderEntryPoint != '\0')
    {
        NIASSERT(strlen(pcProgramFileName) + strlen(pcShaderEntryPoint) + 2 
            < _MAX_PATH);
        NiSprintf(acShaderName, _MAX_PATH, "%s##%s", pcProgramFileName, 
            pcShaderEntryPoint);
    }
    else
    {
        NiStrcpy(acShaderName, _MAX_PATH, pcProgramFileName);
    }

    unsigned int uiFlags = 0;
    NiFixedString kShaderTarget = kPass.GetVertexShaderProgramShaderTarget();
    if (kShaderTarget == "vs_1_1" ||
        kShaderTarget == "vs_2_0" ||
        kShaderTarget == "vs_2_a" ||
        kShaderTarget == "vs_3_0")
    {
        uiFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
        kShaderTarget = "vs_4_0";
    }

    NiD3D10VertexShaderPtr spVertexShader;
    bool bSuccess = NiD3D10ShaderProgramFactory::CreateVertexShaderFromFile(
        acTrueFileName, NULL, NULL, pcShaderEntryPoint, kShaderTarget,
        uiFlags, acShaderName, spVertexShader);

    if (!bSuccess)
        return false;

    kPass.SetVertexShader(spVertexShader);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::LoadGeometryShaderProgram(NiD3D10Pass& kPass)
{
    const char* pcProgramFileName = kPass.GetGeometryShaderProgramFileName();

    // If there is no program to load, it's not a failure.
    // Just return true!
    if (pcProgramFileName == NULL || *pcProgramFileName == '\0')
        return true;

    char acTrueFileName[_MAX_PATH];
    if (!ResolveGeometryShaderFileName(pcProgramFileName, acTrueFileName,
        _MAX_PATH))
    {
        return false;
    }

    // Construct a shader name from the shader file and entry point
    const char* pcShaderEntryPoint = 
        kPass.GetGeometryShaderProgramEntryPoint();
    char acShaderName[_MAX_PATH];
    if (pcShaderEntryPoint != NULL && *pcShaderEntryPoint != '\0')
    {
        NIASSERT(strlen(pcProgramFileName) + strlen(pcShaderEntryPoint) + 2 
            < _MAX_PATH);
        NiSprintf(acShaderName, _MAX_PATH, "%s##%s", pcProgramFileName, 
            pcShaderEntryPoint);
    }
    else
    {
        NiStrcpy(acShaderName, _MAX_PATH, pcProgramFileName);
    }

    unsigned int uiFlags = 0;
    NiFixedString kShaderTarget = kPass.GetGeometryShaderProgramShaderTarget();
    // No need to check for backwards compatibility for geometry shaders.

    NiD3D10GeometryShaderPtr spGeometryShader;
    bool bSuccess = NiD3D10ShaderProgramFactory::CreateGeometryShaderFromFile(
        acTrueFileName, NULL, NULL, pcShaderEntryPoint, kShaderTarget,
        uiFlags, acShaderName, spGeometryShader);

    if (!bSuccess)
        return false;

    kPass.SetGeometryShader(spGeometryShader);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::LoadPixelShaderProgram(NiD3D10Pass& kPass)
{
    const char* pcProgramFileName = kPass.GetPixelShaderProgramFileName();

    // If there is no program to load, it's not a failure.
    // Just return true!
    if (pcProgramFileName == NULL || *pcProgramFileName == '\0')
        return true;

    char acTrueFileName[_MAX_PATH];
    if (!ResolvePixelShaderFileName(pcProgramFileName, acTrueFileName,
        _MAX_PATH))
    {
        return false;
    }

    // Construct a shader name from the shader file and entry point
    const char* pcShaderEntryPoint = 
        kPass.GetPixelShaderProgramEntryPoint();
    char acShaderName[_MAX_PATH];
    if (pcShaderEntryPoint != NULL && *pcShaderEntryPoint != '\0')
    {
        NIASSERT (strlen(pcProgramFileName) + strlen(pcShaderEntryPoint) + 2 
            < _MAX_PATH);
        NiSprintf(acShaderName, _MAX_PATH, "%s##%s", pcProgramFileName, 
            pcShaderEntryPoint);
    }
    else
    {
        NiStrcpy(acShaderName, _MAX_PATH, pcProgramFileName);
    }

    unsigned int uiFlags = 0;
    NiFixedString kShaderTarget = kPass.GetPixelShaderProgramShaderTarget();
    if (kShaderTarget == "ps_1_1" ||
        kShaderTarget == "ps_1_2" ||
        kShaderTarget == "ps_1_3" ||
        kShaderTarget == "ps_1_4" ||
        kShaderTarget == "ps_2_0" ||
        kShaderTarget == "ps_2_a" ||
        kShaderTarget == "ps_2_b" ||
        kShaderTarget == "ps_3_0")
    {
        uiFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
        kShaderTarget = "ps_4_0";
    }

    NiD3D10PixelShaderPtr spPixelShader;
    bool bSuccess = NiD3D10ShaderProgramFactory::CreatePixelShaderFromFile(
        acTrueFileName, NULL, NULL, pcShaderEntryPoint, kShaderTarget, 
        uiFlags, acShaderName, spPixelShader);

    if (!bSuccess)
        return false;

    kPass.SetPixelShader(spPixelShader);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::ResolveVertexShaderFileName(
    const char* pcOriginalName, char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NSBD3D10Utility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName, 
        acFileExt);

    if (acFileExt[0] == '\0')
    {
        NiSprintf(pcTrueName, uiNameLength, "%s.vsh", acFileName);
    }
    else
    {
        NiSprintf(pcTrueName, uiNameLength, "%s%s", acFileName, acFileExt);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::ResolveGeometryShaderFileName(
    const char* pcOriginalName, char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NSBD3D10Utility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName,
        acFileExt);

    if (acFileExt[0] == '\0')
    {
        NiSprintf(pcTrueName, uiNameLength, "%s.gsh", acFileName);
    }
    else
    {
        NiSprintf(pcTrueName, uiNameLength, "%s%s", acFileName, acFileExt);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10BinaryShader::ResolvePixelShaderFileName(
    const char* pcOriginalName, char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NSBD3D10Utility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName,
        acFileExt);

    if (acFileExt[0] == '\0')
    {
        NiSprintf(pcTrueName, uiNameLength, "%s.psh", acFileName);
    }
    else
    {
        NiSprintf(pcTrueName, uiNameLength, "%s%s", acFileName, acFileExt);
    }

    return true;
}
//---------------------------------------------------------------------------
