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

#include "NiMaterial.h"
#include "NiShader.h"

NiImplementRootRTTI(NiShader);
NiImplementRootRTTI(NiShader::NiShaderInstanceDescriptor);

//---------------------------------------------------------------------------
NiShader::NiShader() :
    m_uiImplementation(0),
    m_bInitialized(false)
{
}
//---------------------------------------------------------------------------
NiShader::~NiShader()
{
}
//---------------------------------------------------------------------------
const NiFixedString& NiShader::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
void NiShader::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
unsigned int NiShader::GetImplementation() const
{
    return m_uiImplementation;
}
//---------------------------------------------------------------------------
void NiShader::SetImplementation(unsigned int uiImplementation)
{
    m_uiImplementation = uiImplementation;
}
//---------------------------------------------------------------------------
inline bool NiShader::IsInitialized()
{
    return m_bInitialized;
}
//---------------------------------------------------------------------------
bool NiShader::Initialize()
{
    m_bInitialized = true;

    return true;
}
//---------------------------------------------------------------------------
bool NiShader::SetupGeometry(NiGeometry* pkGeometry)
{
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiShader::PreProcessPipeline(NiGeometry* pkGeometry,
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::UpdatePipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::SetupRenderingPass(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::SetupTransformations(NiGeometry* pkGeometry,
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::SetupShaderPrograms(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::PostRender(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::PostProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return 0;
}
//---------------------------------------------------------------------------
NiGeometryData::RendererData* NiShader::PrepareGeometryForRendering(
    NiGeometry* pkGeometry, const NiSkinPartition::Partition* pkPartition,
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState)
{
    return 0;
}
//---------------------------------------------------------------------------
bool NiShader::GetVertexInputSemantics(
        NiShaderDeclaration::ShaderRegisterEntry* pakSemantics)
{
    // Default implementation requests position.
    pakSemantics[0].m_uiPreferredStream = 0;
    pakSemantics[0].m_eInput = NiShaderDeclaration::SHADERPARAM_NI_POSITION;
    pakSemantics[0].m_eType = NiShaderDeclaration::SPTYPE_FLOAT3;
    pakSemantics[0].m_kUsage = NiShaderDeclaration::UsageToString(
        NiShaderDeclaration::SPUSAGE_POSITION);
    pakSemantics[0].m_uiUsageIndex = 0; 
    pakSemantics[0].m_uiExtraData = 0;

    pakSemantics[1].m_uiUsageIndex = NiMaterial::VS_INPUTS_TERMINATE_ARRAY;
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiShader::FirstPass()
{
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiShader::NextPass()
{
    return 0;
}
//---------------------------------------------------------------------------
bool NiShader::IsGenericallyConfigurable()
{
    return false;
}
//---------------------------------------------------------------------------
NiShaderDeclaration* NiShader::CreateShaderDeclaration(
    unsigned int uiMaxStreamEntryCount, unsigned int uiStreamCount)
{
    return NULL;
}
//---------------------------------------------------------------------------
bool NiShader::AppendRenderPass(unsigned int& uiPassId)
{
    return false;
}
//---------------------------------------------------------------------------
bool NiShader::SetAlphaOverride(unsigned int uiPassId, bool bAlphaBlend,
    bool bUsePreviousSrcBlendMode, 
    NiAlphaProperty::AlphaFunction eSrcBlendMode, 
    bool bUsePreviousDestBlendMode, 
    NiAlphaProperty::AlphaFunction eDestBlendMode)
{
    return false;
}

//---------------------------------------------------------------------------
bool NiShader::SetUsesNiRenderState(bool bRenderState)
{
    return false;
}
//---------------------------------------------------------------------------
bool NiShader::SetGPUProgram(unsigned int uiPassId, 
    NiGPUProgram* pkProgram, NiGPUProgram::ProgramType& eProgramType)
{
    return false;
}
//---------------------------------------------------------------------------
NiShaderConstantMap* NiShader::CreateShaderConstantMap(
    unsigned int uiPassId, NiGPUProgram::ProgramType eProgramType,
    unsigned int uiMapIndex)
{
    return NULL;
}
//---------------------------------------------------------------------------
bool NiShader::AppendTextureSampler(unsigned int uiPassId, 
    unsigned int& uiSamplerId, const NiFixedString& kSemantic,
    const NiFixedString& kVariableName, unsigned int uiInstance)
{
    return false;
}
//---------------------------------------------------------------------------
const NiShader::NiShaderInstanceDescriptor* 
NiShader::GetShaderInstanceDesc() const
{
    return NULL;
}
//---------------------------------------------------------------------------
