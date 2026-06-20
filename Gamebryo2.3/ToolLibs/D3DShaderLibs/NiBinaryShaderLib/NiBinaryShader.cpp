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
#include "NiBinaryShaderLibPCH.h"

#include "NiBinaryShader.h"
#include <NiD3DDefines.h>
#include <NiD3DShaderProgramFactory.h>
#include <NiD3DRendererHeaders.h>
#include <NiD3DUtility.h>
#include <NiTimeSyncController.h>


NiImplementRTTI(NiBinaryShader, NiD3DShader);

//---------------------------------------------------------------------------
// Helper functions
static inline unsigned int GetSamplerValue(
    NiD3DRenderer* pkD3DRenderer,
    unsigned int uiState,
    unsigned int uiValue, 
    NiTexturingProperty::ClampMode eClampMode,
    NiTexturingProperty::FilterMode eFilterMode)
{    
    switch (uiState)
    {
        case NiD3DRenderState::NISAMP_ADDRESSU:
            return pkD3DRenderer->GetD3DClampMode(eClampMode).m_eU;
        case NiD3DRenderState::NISAMP_ADDRESSV:
            return pkD3DRenderer->GetD3DClampMode(eClampMode).m_eV;
        case NiD3DRenderState::NISAMP_MAGFILTER:
            return pkD3DRenderer->GetD3DFilterMode(eFilterMode).m_eMag;
        case NiD3DRenderState::NISAMP_MINFILTER:
            return pkD3DRenderer->GetD3DFilterMode(eFilterMode).m_eMin;
        case NiD3DRenderState::NISAMP_MIPFILTER:
            return pkD3DRenderer->GetD3DFilterMode(eFilterMode).m_eMip;
        default:
            // Invalid sampler state.
            NIASSERT(false);
            return uiValue;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NiBinaryShader::NiBinaryShader() :
    NiD3DShader(),
    m_uiBonesPerPartition(0), 
    m_uiBoneMatrixRegisters(0), 
    m_eBinormalTangentMethod(NiGeometryData::NBT_METHOD_NONE),
    m_uiBinormalTangentUVSource(NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT),
    m_eBoneCalcMethod(BONECALC_SKIN)
{
    for (unsigned int ui = 0; ui < m_kPasses.GetAllocatedSize(); ui++)
        m_kPasses.SetAt(ui, 0);
}
//---------------------------------------------------------------------------
NiBinaryShader::~NiBinaryShader()
{
}
//---------------------------------------------------------------------------
bool NiBinaryShader::Initialize()
{
    if (!m_bInitialized)
    {
        if (!NiD3DShader::Initialize())
            return false;

        // We are going to cycle through the passes and load the shader
        // programs. That should be all we need to do at this point!
        NiD3DPass* pkPass;
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
        m_bInitialized = true;
    }
    return m_bInitialized;
}
//---------------------------------------------------------------------------
unsigned int NiBinaryShader::PreProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return NiD3DShader::PreProcessPipeline(pkGeometry, pkSkin, pkRendererData, 
        pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiBinaryShader::UpdatePipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    for (unsigned int uiPass = 0; uiPass < m_kPasses.GetSize(); uiPass++)
    {
        NiD3DPass* pkCurPass = m_kPasses.GetAt(uiPass);
        if (!pkCurPass)
        {
            continue;
        }
        for (unsigned int uiStage = 0; uiStage < pkCurPass->GetStageCount();
            uiStage++)
        {
            NiD3DTextureStage* pkCurStage = pkCurPass->GetStage(uiStage);
            if (!pkCurStage)
            {
                continue;
            }
            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            PrepareTextureStage(pkGeometry, pkCurStage, pkTexProp, pkEffects,
                uiPass);
        }
    }

    return NiD3DShader::UpdatePipeline(pkGeometry, pkSkin, pkRendererData, 
        pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiBinaryShader::SetupRenderingPass(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return NiD3DShader::SetupRenderingPass(pkGeometry, pkSkin, pkRendererData, 
        pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiBinaryShader::SetupTransformations(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    if ((m_uiBonesPerPartition > 4) && pkPartition)
    {
        // We need to set the bone matrices ourselves.
        if (m_uiCurrentPass == 0)
        {
            BoneMatrixCalcMethod eMethod = GetBoneCalcMethod();
            m_pkD3DRenderer->CalculateBoneMatrices(pkSkin, kWorld, true, 
                m_uiBoneMatrixRegisters, (eMethod == BONECALC_NO_SKIN));
        }

        m_pkD3DRenderState->SetBoneCount(
            pkPartition->m_usBonesPerVertex);

        // Force the calculation of the 'world' D3D matrix
        // but indicate it shouldn't be pushed to the device
        m_pkD3DRenderer->SetModelTransform(kWorld, false);

        return 0;
    }
    else
    {
        // Set if there is a vertex shader
        NiD3DPass* pkPass = m_kPasses.GetAt(m_uiCurrentPass);
        if (pkPass && pkPass->GetVertexShader())
        {
            if (m_uiCurrentPass == 0)
            {
                // This is to catch the case where someone is doing
                // 4 bones/partition and a shader program.
                // This is an odd case, as the developer should likely
                // being doing higher bones/partition if they are
                // writing a shader to do the skinning...
                if (pkPartition)
                {
                    BoneMatrixCalcMethod eMethod = GetBoneCalcMethod();
                    m_pkD3DRenderer->CalculateBoneMatrices(pkSkin, kWorld, 
                        true, m_uiBoneMatrixRegisters,
                        (eMethod == BONECALC_NO_SKIN));
                }
            }

            // This has to be done each pass so that when multipass
            // objects are being batch rendered, the martices are
            // updated properly! We may want to have some sort of 
            // flag indicate that a batch is in progress, so that
            // we don't do this every pass for non-batched objects.
            if (pkPartition)
            {
                m_pkD3DRenderState->SetBoneCount(
                    pkPartition->m_usBonesPerVertex);
                m_pkD3DRenderer->SetModelTransform(kWorld, false);
            }
            else
            {
                m_pkD3DRenderState->SetBoneCount(0);
                m_pkD3DRenderer->SetModelTransform(kWorld, false);
            }

            return 0;
        }
    }

    return NiD3DShader::SetupTransformations(pkGeometry, pkSkin, 
        pkPartition, pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiBinaryShader::SetupShaderPrograms(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return NiD3DShader::SetupShaderPrograms(pkGeometry, pkSkin, pkPartition, 
        pkRendererData, pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
NiGeometryData::RendererData* NiBinaryShader::PrepareGeometryForRendering(
    NiGeometry* pkGeometry, const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState)
{
    return NiD3DShader::PrepareGeometryForRendering(pkGeometry, pkPartition,
        pkRendererData, pkState);
}
//---------------------------------------------------------------------------
unsigned int NiBinaryShader::PostProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return NiD3DShader::PostProcessPipeline(pkGeometry, pkSkin, pkRendererData,
        pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiBinaryShader::NextPass()
{
    return NiD3DShader::NextPass();
}
//---------------------------------------------------------------------------
bool NiBinaryShader::SetupGeometry(NiGeometry* pkGeometry)
{
    if (!pkGeometry)
        return false;

    //-----------------------------------------------------------------------
    // Ensure that there is BinormalTangent data if required
    if (m_eBinormalTangentMethod != NiGeometryData::NBT_METHOD_NONE)
    {
        // Check the geoemtry data's method versus ours
        if (pkGeometry->GetNormalBinormalTangentMethod() !=
            m_eBinormalTangentMethod)
        {
            NIASSERT(!"SetupGeometry> BinormalTangent method conflict!");
        }
/***
        // Get the geometry data
        if (!pkGeometry->GetNormalBinormalTangent())
        {
            if (!NiD3DUtility::SetupBinormalTangentData(pkGeometry))
                return false;
        }
***/
    }

    //-----------------------------------------------------------------------
    // Add a 'dummy' alpha property if one is needed
    NiAlphaProperty* pkAlphaProperty = 0;
    unsigned int uiValue;
    bool bSave;

    // Check the renderstate group for alphablend/alphatest
    if (m_pkRenderStateGroup)
    {
        if (m_pkRenderStateGroup->GetRenderState(
            D3DRS_ALPHABLENDENABLE, uiValue, bSave))
        {
            // If it's enabled, set it in the property
            if (uiValue == 1)
            {
                if (!pkAlphaProperty)
                {
                    pkAlphaProperty = NiNew NiAlphaProperty();
                    NIASSERT(pkAlphaProperty);
                }
                pkAlphaProperty->SetAlphaBlending(true);
            }
        }

        if (m_pkRenderStateGroup->GetRenderState(
            D3DRS_ALPHATESTENABLE, uiValue, bSave))
        {
            if (uiValue == 1)
            {
                if (!pkAlphaProperty)
                {
                    pkAlphaProperty = NiNew NiAlphaProperty();
                    NIASSERT(pkAlphaProperty);
                }
                pkAlphaProperty->SetAlphaTesting(true);
            }
        }
    }

    // Check the first pass afterwards, as it can override the global
    NiD3DPass* pkPass = m_kPasses.GetAt(0);
    if (pkPass)
    {
        if (pkPass->GetRenderState(D3DRS_ALPHABLENDENABLE, uiValue, bSave))
        {
            // If it's enabled, set it in the property
            if (uiValue == 1)
            {
                if (!pkAlphaProperty)
                {
                    pkAlphaProperty = NiNew NiAlphaProperty();
                    NIASSERT(pkAlphaProperty);
                }
                pkAlphaProperty->SetAlphaBlending(true);
            }
        }

        if (pkPass->GetRenderState(D3DRS_ALPHATESTENABLE, uiValue, bSave))
        {
            if (uiValue == 1)
            {
                if (!pkAlphaProperty)
                {
                    pkAlphaProperty = NiNew NiAlphaProperty();
                    NIASSERT(pkAlphaProperty);
                }
                pkAlphaProperty->SetAlphaTesting(true);
            }
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
    // See if there is a 'time' attribute required
    bool bTimeRequired = false;

    // Cycle over each pass and check the contant maps for occurances of one
    // of the time-based pre-defines.
    // First, check the 'global' attributes
    if (m_spPixelConstantMap || m_spVertexConstantMap)
    {
        // Pixel shader map
        if (m_spPixelConstantMap &&
            (m_spPixelConstantMap->GetEntry("time") ||
             m_spPixelConstantMap->GetEntry("sin_time") ||
             m_spPixelConstantMap->GetEntry("cos_time") ||
             m_spPixelConstantMap->GetEntry("tan_time") ||
             m_spPixelConstantMap->GetEntry("time_sin_cos_tan")))
        {
            bTimeRequired = true;
        }
        // Vertex shader map
        if (m_spVertexConstantMap &&
            (m_spVertexConstantMap->GetEntry("time") ||
             m_spVertexConstantMap->GetEntry("sin_time") ||
             m_spVertexConstantMap->GetEntry("cos_time") ||
             m_spVertexConstantMap->GetEntry("tan_time") ||
             m_spVertexConstantMap->GetEntry("time_sin_cos_tan")))
        {
            bTimeRequired = true;
        }
    }

    if (!bTimeRequired)
    {
        // Check each pass
        for (unsigned int ui = 0; ui < m_kPasses.GetSize(); ui++)
        {
            pkPass = m_kPasses.GetAt(ui);
            if (pkPass)
            {
                NiD3DShaderConstantMap* pkPixelConstantMap = 
                    pkPass->GetPixelConstantMap();
                NiD3DShaderConstantMap* pkVertexConstantMap = 
                    pkPass->GetVertexConstantMap();
                // Pixel shader map
                if (pkPixelConstantMap && 
                    (pkPixelConstantMap->GetEntry("time") ||
                     pkPixelConstantMap->GetEntry("sin_time") ||
                     pkPixelConstantMap->GetEntry("cos_time") ||
                     pkPixelConstantMap->GetEntry("tan_time") ||
                     pkPixelConstantMap->GetEntry("time_sin_cos_tan")))
                {
                    bTimeRequired = true;
                }
                // Vertex shader map
                if (pkVertexConstantMap &&
                    (pkVertexConstantMap->GetEntry("time") ||
                     pkVertexConstantMap->GetEntry("sin_time") ||
                     pkVertexConstantMap->GetEntry("cos_time") ||
                     pkVertexConstantMap->GetEntry("tan_time") ||
                     pkVertexConstantMap->GetEntry("time_sin_cos_tan")))
                {
                    bTimeRequired = true;
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

    //-----------------------------------------------------------------------
    // Add any missing attributes...

    // Check passes for software vertex processing
    bool bSoftwareVP = false;
    for (unsigned int ui = 0; ui < m_kPasses.GetSize(); ui++)
    {
        pkPass = m_kPasses.GetAt(ui);
        if (pkPass)
        {
            if (pkPass->GetSoftwareVertexProcessing())
            {
                bSoftwareVP = true;
                break;
            }
        }
    }

    if (m_spShaderDecl)
        m_spShaderDecl->SetSoftwareVertexProcessing(bSoftwareVP);

    return NiD3DShader::SetupGeometry(pkGeometry);
}
//---------------------------------------------------------------------------
bool NiBinaryShader::LoadVertexShaderProgram(NiD3DPass& kPass)
{
    const char* pcProgramFileName = kPass.GetVertexShaderProgramFileName();

    // If there is no program to load, it's not a failure.
    // Just return true!
    if (!pcProgramFileName || (strcmp(pcProgramFileName, "") == 0))
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

    // We need to utilize the usage flag here...
    NiD3DVertexShader* pkVS = 
        NiD3DShaderProgramFactory::CreateVertexShaderFromFile(acTrueFileName, 
        acShaderName, pcShaderEntryPoint, 
        kPass.GetVertexShaderProgramShaderTarget(), 
        m_spShaderDecl->GetD3DDeclaration(), 0, 
        kPass.GetSoftwareVertexProcessing());

    if (!pkVS)
        return false;

    kPass.SetVertexShader(pkVS);

    return true;
}
//---------------------------------------------------------------------------
bool NiBinaryShader::LoadGeometryShaderProgram(NiD3DPass& kPass)
{
    NIASSERT(NiIsKindOf(NiD3DRenderer, NiRenderer::GetRenderer()));
    return false;
}
//---------------------------------------------------------------------------
bool NiBinaryShader::LoadPixelShaderProgram(NiD3DPass& kPass)
{
    const char* pcProgramFileName = kPass.GetPixelShaderProgramFileName();

    // If there is no program to load, it's not a failure.
    // Just return true!
    if (!pcProgramFileName || (strcmp(pcProgramFileName, "") == 0))
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
        NIASSERT(strlen(pcProgramFileName) + strlen(pcShaderEntryPoint) + 2 
            < _MAX_PATH);
        NiSprintf(acShaderName, _MAX_PATH, "%s##%s", pcProgramFileName, 
            pcShaderEntryPoint);
    }
    else
    {
        NiStrcpy(acShaderName, _MAX_PATH, pcProgramFileName);
    }

    NiD3DPixelShader* pkPS = 
        NiD3DShaderProgramFactory::CreatePixelShaderFromFile(acTrueFileName, 
        acShaderName, pcShaderEntryPoint, 
        kPass.GetPixelShaderProgramShaderTarget());
    if (!pkPS)
        return false;

    kPass.SetPixelShader(pkPS);

    return true;
}
//---------------------------------------------------------------------------
bool NiBinaryShader::ResolveVertexShaderFileName(const char* pcOriginalName,
    char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NiD3DUtility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName, 
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
bool NiBinaryShader::ResolveGeometryShaderFileName(const char* pcOriginalName,
    char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NiD3DUtility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName, 
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
bool NiBinaryShader::ResolvePixelShaderFileName(const char* pcOriginalName,
    char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NiD3DUtility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName, 
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
void NiBinaryShader::PrepareTextureStage(NiGeometry* pkGeometry,
    NiD3DTextureStage* pkStage,
    const NiTexturingProperty* pkTexProp,
    const NiDynamicEffectState* pkEffects,
    unsigned int uiPass)
{
    NiTexture* pkTexture = NULL;
    NiTexturingProperty::ClampMode eClampMode =
        NiTexturingProperty::CLAMP_S_CLAMP_T;
    NiTexturingProperty::FilterMode eFilterMode =
        NiTexturingProperty::FILTER_NEAREST;

    unsigned int uiTextureFlags = pkStage->GetTextureFlags();
    unsigned short usObjTextureFlags = pkStage->GetObjTextureFlags();

    if (usObjTextureFlags == NiD3DTextureStage::TSOTF_IGNORE)
    {
        if (uiTextureFlags != NiD3DTextureStage::TSTF_IGNORE)
        {
            const NiTexturingProperty::Map* pkMap = NULL;
            NIASSERT(pkTexProp);
            if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_MASK) == 0)
            {
                switch (uiTextureFlags & NiD3DTextureStage::TSTF_NDL_TYPEMASK)
                {
                case NiD3DTextureStage::TSTF_NONE:
                    break;
                case NiD3DTextureStage::TSTF_NDL_BASE:
                    pkMap = pkTexProp->GetBaseMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_DARK:
                    pkMap = pkTexProp->GetDarkMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_DETAIL:
                    pkMap = pkTexProp->GetDetailMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_GLOSS:
                    pkMap = pkTexProp->GetGlossMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_GLOW:
                    pkMap = pkTexProp->GetGlowMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_BUMP:
                    pkMap = pkTexProp->GetBumpMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_NORMAL:
                    pkMap = pkTexProp->GetNormalMap();
                    break;
                case NiD3DTextureStage::TSTF_NDL_PARALLAX:
                    pkMap = pkTexProp->GetParallaxMap();
                    break;
                }
            }
            else if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_MASK) ==
                NiD3DTextureStage::TSTF_MAP_DECAL)
            {
                unsigned int uiIndex = uiTextureFlags &
                    NiD3DTextureStage::TSTF_INDEX_MASK;
                if (uiIndex < pkTexProp->GetDecalArrayCount())
                {
                    pkMap = pkTexProp->GetDecalMap(uiIndex);
                }
            }
            else if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_MASK) ==
                NiD3DTextureStage::TSTF_MAP_SHADER)
            {
                unsigned int uiIndex = uiTextureFlags &
                    NiD3DTextureStage::TSTF_INDEX_MASK;
                if (uiIndex < pkTexProp->GetShaderArrayCount())
                {
                    pkMap = pkTexProp->GetShaderMap(uiIndex);
                }
            }

            if (pkMap)
            {
                pkTexture = pkMap->GetTexture();
                eClampMode = pkMap->GetClampMode();
                eFilterMode = pkMap->GetFilterMode();
                // What else is needed?

                // If we are to use the index from the map, set that now
                // as well
                if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_USE_MASK) == 
                    NiD3DTextureStage::TSTF_MAP_USE_INDEX)
                {
                    unsigned int uiTexCoordIndex = pkMap->GetTextureIndex();

                    // We need to take the transformation info into account
                    uiTexCoordIndex |= pkStage->GetTexCoordIndex() &
                        ~NiD3DTextureStage::TSTF_INDEX_MASK;
                    
                    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX,
                        uiTexCoordIndex, pkStage->GetSaveTexCoordIndex());
                }
            }
        }
    }
    else
    {
        NiTextureEffect* pkTextureEffect = NULL;
        NiShadowGenerator* pkGenerator = NULL;
        unsigned short usObjectIndex = usObjTextureFlags &
            NiD3DTextureStage::TSOTF_INDEX_MASK;
        switch ((usObjTextureFlags & NiD3DTextureStage::TSOTF_TYPE_MASK) >>
            NiD3DTextureStage::TSOTF_TYPE_SHIFT)
        {
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
                    if (usIndex++ == usObjectIndex)
                    {
                        pkTextureEffect = pkProjShadow;
                        break;
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
                    if (usIndex++ == usObjectIndex)
                    {
                        pkTextureEffect = pkProjLight;
                        break;
                    }
                }
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_FOGMAP:
                pkTextureEffect = pkEffects->GetFogMap();
                break;
            case NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT:
                {
                    NiDynamicEffect* pkDynEffect = 
                        NiShaderConstantMap::GetDynamicEffectForObject(
                        pkEffects, 
                        NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT, 
                        usObjectIndex);

                    if (pkDynEffect)
                        pkGenerator = pkDynEffect->GetShadowGenerator();
                    break;
                }
            case NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT:
                {
                    NiDynamicEffect* pkDynEffect = 
                        NiShaderConstantMap::GetDynamicEffectForObject(
                        pkEffects, 
                        NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT, 
                        usObjectIndex);

                    if (pkDynEffect)
                        pkGenerator = pkDynEffect->GetShadowGenerator();
                    break;
                }
            case NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT:
                {
                    NiDynamicEffect* pkDynEffect = 
                        NiShaderConstantMap::GetDynamicEffectForObject(
                        pkEffects, 
                        NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT, 
                        usObjectIndex);

                    if (pkDynEffect)
                        pkGenerator = pkDynEffect->GetShadowGenerator();
                    break;
                }
            case NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT:
                {
                    NiDynamicEffect* pkDynEffect = 
                        NiShaderConstantMap::GetDynamicEffectForObject(
                        pkEffects, 
                        NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT, 
                        usObjectIndex);

                    if (pkDynEffect)
                        pkGenerator = pkDynEffect->GetShadowGenerator();
                    break;
                }
            case NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT:
                {
                    NiDynamicEffect* pkDynEffect = 
                        NiShaderConstantMap::GetDynamicEffectForObject(
                        pkEffects, 
                        NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT, 
                        usObjectIndex);

                    if (pkDynEffect)
                        pkGenerator = pkDynEffect->GetShadowGenerator();
                    break;
                }
            case NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT:
                {
                    NiDynamicEffect* pkDynEffect = 
                        NiShaderConstantMap::GetDynamicEffectForObject(
                        pkEffects, NiShaderAttributeDesc::
                        OT_EFFECT_SHADOWDIRECTIONALLIGHT, usObjectIndex);

                    if (pkDynEffect)
                        pkGenerator = pkDynEffect->GetShadowGenerator();
                    break;
                }
            default:
                // This assertion is hit when the object type is not one
                // that has a texture.
                NIASSERT(false);
                break;
        }

        if (pkTextureEffect)
        {
            pkTexture = pkTextureEffect->GetEffectTexture();
            eClampMode = pkTextureEffect->GetTextureClamp();
            eFilterMode = pkTextureEffect->GetTextureFilter();

            // If UseMapIndex is specified, grab and set the tex coord gen
            // mode and 
            if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_USE_MASK) == 
                NiD3DTextureStage::TSTF_MAP_USE_INDEX)
            {
                PackDynamicEffect(pkTextureEffect, pkStage,
                    pkStage->GetSaveTexCoordIndex(), m_pkD3DRenderer);
            }
        }
        else if (pkGenerator)
        {
            NiShadowMap* pkShadowMap = pkGenerator->RetrieveShadowMap(
                uiPass, pkGeometry);
            NIASSERT(pkShadowMap);

            pkTexture = pkShadowMap->GetTexture();
            eClampMode = pkShadowMap->GetClampMode();
            eFilterMode = pkShadowMap->GetFilterMode();
        }

    }

    // Even if the texture is NULL, we need to set that here to prevent a 
    // stale texture from getting used by accident.
    pkStage->SetTexture(pkTexture);

    // Set "UseMapValue" sampler states.
    NiD3DTextureStageGroup* pkGroup = pkStage->GetTextureStageGroup();
    if (pkGroup && pkGroup->GetUseMapValueCount() > 0)
    {
        unsigned int uiState, uiValue;
        bool bUseMapValue;

        if (pkGroup->GetSavedSamplerStateCount() > 0)
        {
            NIVERIFY(pkGroup->GetFirstSavedSamplerState(uiState,
                uiValue, bUseMapValue));

            if (bUseMapValue)
            {
                pkGroup->SetSamplerState(uiState, GetSamplerValue(
                    m_pkD3DRenderer, uiState,
                    uiValue, eClampMode, eFilterMode), true);
            }
            while (pkGroup->GetNextSavedSamplerState(uiState, uiValue,
                bUseMapValue))
            {
                if (bUseMapValue)
                {
                    pkGroup->SetSamplerState(uiState, GetSamplerValue(
                        m_pkD3DRenderer, 
                        uiState, uiValue, eClampMode, eFilterMode), true,
                        bUseMapValue);
                }
            }
        }
        if (pkGroup->GetNoSaveSamplerStateCount() > 0)
        {
            NIVERIFY(pkGroup->GetFirstNoSaveSamplerState(uiState,
                uiValue, bUseMapValue));

            if (bUseMapValue)
            {
                pkGroup->SetSamplerState(uiState, GetSamplerValue(
                    m_pkD3DRenderer, uiState,
                    uiValue, eClampMode, eFilterMode), false,
                    bUseMapValue);
            }
            while (pkGroup->GetNextNoSaveSamplerState(uiState, uiValue,
                bUseMapValue))
            {
                if (bUseMapValue)
                {
                    pkGroup->SetSamplerState(uiState, GetSamplerValue(
                        m_pkD3DRenderer, 
                        uiState, uiValue, eClampMode, eFilterMode), false,
                        bUseMapValue);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
