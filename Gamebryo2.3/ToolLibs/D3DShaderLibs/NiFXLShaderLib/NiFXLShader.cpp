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
#include "NiFXLShaderLibPCH.h"

#include "NiFXLShader.h"
#include "NiFXLFactory.h"
#include "NiFXLFile.h"
#include "NiFXLParameter.h"
#include "NiFXLShaderSDM.h"
#include "NiFXLTechnique.h"
#include <NiD3DDefines.h>
#include <NiD3DRendererHeaders.h>
#include <NiD3DUtility.h>
#include <NiTimeSyncController.h>

#include <NiNode.h>

static NiFXLShaderSDM NiFXLShaderSDMObject;

NiImplementRTTI(NiFXLShader, NiD3DShader);

//---------------------------------------------------------------------------
NiFXLShader::NiFXLShader() :
    NiD3DShader(),
    m_pkFXLTechnique(NULL),
    m_pkFXLEffect(NULL),
    m_pkTechnique(NULL),
    m_uiActualImplementation(0),
    m_uiBoneMatrixRegisters(0), 
    m_eBoneCalcMethod(BONECALC_SKIN)
{
    for (unsigned int ui = 0; ui < m_kPasses.GetAllocatedSize(); ui++)
        m_kPasses.SetAt(ui, 0);
}
//---------------------------------------------------------------------------
NiFXLShader::~NiFXLShader()
{
    // No passes should have been set
}
//---------------------------------------------------------------------------
bool NiFXLShader::Initialize()
{
    if (!m_bInitialized)
    {
        if (!NiD3DShader::Initialize())
            return false;

        if (!m_kName.Exists() || m_kName.GetLength() == 0)
            return false;

        RecreateRendererData();
        if (m_pkTechnique == NULL)
            return false;
    }

    return m_bInitialized;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::PreProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    unsigned int uiReturn = NiD3DShader::PreProcessPipeline(pkGeometry, 
        pkSkin, pkRendererData, pkState, pkEffects, kWorld, kWorldBound);

    // Make sure renderer data is up to date
    if (m_pkTechnique == NULL)
    {
        RecreateRendererData();
        if (m_pkFXLEffect == NULL)
        {
            NiD3DRenderer::Error("NiFXLShader::PreProcessPipeline"
                " - %s - Effect failed to load\n", (const char*)m_kName);
        }
    }

    return uiReturn;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::UpdatePipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Do nothing; this is handled by the FXLEffect object
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::SetupRenderingPass(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    // Do nothing; this is handled by the FXLEffect object
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::SetupTransformations(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    unsigned int uiReturn = 0;

    NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)pkRendererData;

    if (pkPartition)
    {
        if (m_uiCurrentPass == 0)
        {
            BoneMatrixCalcMethod eMethod = GetBoneCalcMethod();
            m_pkD3DRenderer->CalculateBoneMatrices(pkSkin, kWorld, 
                true, m_uiBoneMatrixRegisters,
                (eMethod == BONECALC_NO_SKIN));
        }

        m_pkD3DRenderState->SetBoneCount(
            pkPartition->m_usBonesPerVertex);
    }
    else
    {
        m_pkD3DRenderState->SetBoneCount(0);
    }

    // Force the calculation of the 'world' D3D matrix
    // but indicate it shouldn't be pushed to the device
    m_pkD3DRenderer->SetModelTransform(kWorld, false);


    // Set parameters and textures
    NiFXLParameter* pkParam = 
        m_pkFXLTechnique->GetFirstParameter();
    while (pkParam)
    {
        NiD3DError eResult = pkParam->SetParameter(m_pkFXLEffect, 
            pkGeometry, pkSkin, pkPartition, pkBuffData, pkState, 
            pkEffects, kWorld, kWorldBound, m_uiCurrentPass);

        if (eResult != NISHADERERR_OK)
        {
            switch (eResult)
            {
                case NISHADERERR_DYNEFFECTNOTFOUND:
                    if (NiShaderErrorSettings::GetAllowDynEffectNotFound())
                    {
                        NiD3DRenderer::Warning(
                            "NiFXLShader::SetupTransformations - %s - "
                            "Parameter %s on geometry with name \"%s\" "
                            "references a nonexistent NiDynamicEffect "
                            "object. Default values used.\n", 
                            (const char*)m_kName, 
                            (const char*)pkParam->GetName(), 
                            (const char*)pkGeometry->GetName());
                    }
                    break;
                default:
                    NiD3DRenderer::Error(
                        "NiFXLShader::SetupTransformations - %s - "
                        "Parameter %s failed to be set on geometry with "
                        "name \"%s\"\n", (const char*)m_kName,
                        (const char*)pkParam->GetName(), 
                        (const char*)pkGeometry->GetName());
                    break;
            }
        }

        pkParam = m_pkFXLTechnique->GetNextParameter();
    }

    return uiReturn;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::SetupShaderPrograms(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)pkRendererData;

    m_pkFXLEffect->BeginPassFromIndex(m_uiCurrentPass);

    m_pkFXLEffect->Commit();

    if (m_uiCurrentPass == 0)
    {
        // Set the object's FVF in case it isn't handled by the D3DXEffect
        if (pkBuffData == NULL)
        {
            if (pkPartition)
            {
                pkBuffData = (NiGeometryBufferData*)pkPartition->m_pkBuffData;
            }
            else
            {
                pkBuffData = (NiGeometryBufferData*)
                    pkGeometry->GetModelData()->GetRendererData();
            }
            NIASSERT(pkBuffData);
        }

        NIASSERT(pkBuffData->GetVertexDeclaration());
        m_pkD3DRenderState->SetDeclaration(
            pkBuffData->GetVertexDeclaration());

    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::PostRender(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, 
    const NiSkinPartition::Partition* pkPartition, 
    NiGeometryData::RendererData* pkRendererData, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects, 
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    m_pkFXLEffect->EndPass();
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::PostProcessPipeline(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, NiGeometryData::RendererData* pkRendererData,
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    const NiTransform& kWorld, const NiBound& kWorldBound)
{
    return NiD3DShader::PostProcessPipeline(pkGeometry, pkSkin, pkRendererData,
        pkState, pkEffects, kWorld, kWorldBound);
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::FirstPass()
{
    m_pkFXLEffect->BeginTechnique(m_pkTechnique, 0);

    m_uiCurrentPass = 0;
    if (m_uiPassCount == 0)
        m_pkFXLEffect->EndTechnique();

    return m_uiPassCount;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::NextPass()
{
    m_uiCurrentPass++;
    unsigned int uiRemainingPasses = m_uiPassCount - m_uiCurrentPass;

    if (uiRemainingPasses == 0)
        m_pkFXLEffect->EndTechnique();

    return uiRemainingPasses;
}
//---------------------------------------------------------------------------
bool NiFXLShader::SetupGeometry(NiGeometry* pkGeometry)
{
    if (!pkGeometry)
        return false;

    // Attach time parameter
    bool bTimeRequired = m_pkFXLTechnique->GetTimeParameterPresent();

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
            pkObject = pkObject->GetParent();
        }
    }

    CreateShaderDeclaration();

    return NiD3DShader::SetupGeometry(pkGeometry);
}
//---------------------------------------------------------------------------
void NiFXLShader::DestroyRendererData()
{
    m_pkFXLTechnique->GetFile()->DestroyRendererData();
    
    if (m_pkFXLEffect)
    {
        m_pkFXLEffect->Release();
        m_pkFXLEffect = NULL;
    }

    m_pkTechnique = NULL;

    m_spShaderDecl = 0;

    m_bInitialized = false;
}
//---------------------------------------------------------------------------
void NiFXLShader::CreateShaderDeclaration()
{
    if (m_spShaderDecl == NULL)
    {
        // Get pixel/vertex shader annotations
        FXLTECHNIQUE_DESC kTechniqueDesc;
        m_pkFXLEffect->GetTechniqueDesc(m_pkTechnique, &kTechniqueDesc);

        FXLPASS_DESC* pkPassDescs = NiExternalNew 
            FXLPASS_DESC[kTechniqueDesc.Passes];

        unsigned int i = 0;
        unsigned int uiTotalSemanticCount = 0;
        for (; i < kTechniqueDesc.Passes; i++)
        {
            FXLHANDLE hPass = m_pkFXLEffect->GetPassHandleFromIndex(
                m_pkTechnique, i);
            m_pkFXLEffect->GetPassDesc(hPass, pkPassDescs + i);

            if (pkPassDescs[i].pVertexShaderFunction != NULL)
            {
                unsigned int uiSemanticCount = 0;
                D3DXGetShaderInputSemantics(
                    pkPassDescs[i].pVertexShaderFunction, NULL, 
                    &uiSemanticCount);

                uiTotalSemanticCount += uiSemanticCount;
            }
        }

        if (uiTotalSemanticCount == 0)
        {
            NiExternalDelete[] pkPassDescs;
            return;
        }

        unsigned int uiFilledSemanticCount = 0;
        D3DXSEMANTIC* pkSemantics = NiExternalNew 
            D3DXSEMANTIC[uiTotalSemanticCount];
        for (i = 0; i < kTechniqueDesc.Passes; i++)
        {
            if (pkPassDescs[i].pVertexShaderFunction != NULL)
            {
                unsigned int uiSemanticCount = 0;
                D3DXSEMANTIC* pkBase = pkSemantics + uiFilledSemanticCount;

                D3DXGetShaderInputSemantics(
                    pkPassDescs[i].pVertexShaderFunction, pkBase, 
                    &uiSemanticCount);

                if (i > 0)
                {
                    // Eliminate redundant semantics
                    D3DXSEMANTIC* pkNew = pkBase;
                    for (unsigned int j = 0; j < uiSemanticCount; j++)
                    {
                        D3DXSEMANTIC* pkExisting = pkSemantics;
                        while (pkExisting < pkBase)
                        {
                            if (pkExisting->Usage == pkNew->Usage &&
                                pkExisting->UsageIndex == pkNew->UsageIndex)
                            {
                                pkNew->Usage = 0xFFFFFFFF;
                                pkNew->UsageIndex = 0xFFFFFFFF;
                                uiTotalSemanticCount--;
                                break;
                            }

                            pkExisting++;
                        }
                        pkNew++;
                    }
                }

                uiFilledSemanticCount += uiSemanticCount;
            }
        }
        NIASSERT(uiFilledSemanticCount >= uiTotalSemanticCount);

        NiExternalDelete[] pkPassDescs;

        if (uiFilledSemanticCount == 0)
            return;

        // Assume only one stream
        NiD3DShaderDeclaration* pkShaderDecl = 
            NiXenonShaderDeclaration::Create(
                m_pkD3DRenderer, uiTotalSemanticCount, 1);

        unsigned int uiEntry = 0;
        for (i = 0; i < uiFilledSemanticCount; i++)
        {
            if (pkSemantics[i].Usage != 0xFFFFFFFF &&
                pkSemantics[i].UsageIndex != 0xFFFFFFFF)
            {
                NiD3DShaderDeclaration::ShaderParameter eInput;
                NiD3DShaderDeclaration::ShaderParameterType eType;
                NiD3DShaderDeclaration::ShaderParameterUsage eUsage;
                NiD3DShaderDeclaration::ShaderParameterTesselator eTess;

                bool bSuccess = 
                    NiXenonShaderDeclaration::GetShaderParameterValues(
                        (D3DDECLUSAGE)pkSemantics[i].Usage, 
                        pkSemantics[i].UsageIndex, eInput, eType, 
                        eUsage, eTess);
                if (bSuccess)
                {
                    pkShaderDecl->SetEntry(0, uiEntry, eInput, eType, 
                        NiShaderDeclaration::UsageToString(eUsage), 
                        pkSemantics[i].UsageIndex, eTess);
                    uiEntry++;
                }
            }
        }
        NiExternalDelete[] pkSemantics;

        SetShaderDecl(pkShaderDecl);
    }
}
//---------------------------------------------------------------------------
void NiFXLShader::RecreateRendererData()
{
    NiFXLFile* pkFile = m_pkFXLTechnique->GetFile();
    FXLEffect* pkEffect = pkFile->GetEffect();

/*    if (pkEffect == NULL)
    {
        pkFile->LoadEffect(m_pkD3DRenderer);
        pkEffect = pkFile->GetEffect();
        if (pkEffect == NULL)
            return;
    }*/
    m_pkFXLEffect = pkEffect;

    NiFXLTechnique* pkTechnique = pkFile->GetTechnique(
        (const char*)m_kName, m_uiActualImplementation);

    NIASSERT(pkTechnique);

    m_uiPassCount = pkTechnique->GetNumPasses();

    m_pkTechnique = pkTechnique->GetTechniquePtr();

    CreateShaderDeclaration();
}
//---------------------------------------------------------------------------
NiD3DShaderDeclaration* NiFXLShader::GetShaderDecl() const
{
    return m_spShaderDecl;
}
//---------------------------------------------------------------------------
void NiFXLShader::SetShaderDecl(NiD3DShaderDeclaration* pkShaderDecl)
{
    m_spShaderDecl = pkShaderDecl;
}
//---------------------------------------------------------------------------
void NiFXLShader::SetEffectTechnique(NiFXLTechnique* pkFXLTechnique)
{
    m_pkFXLTechnique = pkFXLTechnique;
}
//---------------------------------------------------------------------------
void NiFXLShader::SetTechniquePtr(FXLTechniquePtr pkTechnique)
{
    m_pkTechnique = pkTechnique;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::GetBoneMatrixRegisters() const
{
    return m_uiBoneMatrixRegisters;
}
//---------------------------------------------------------------------------
void NiFXLShader::SetBoneMatrixRegisters(unsigned int uiRegisters)
{
    m_uiBoneMatrixRegisters = uiRegisters;
}
//---------------------------------------------------------------------------
NiFXLShader::BoneMatrixCalcMethod NiFXLShader::GetBoneCalcMethod() const
{
    return m_eBoneCalcMethod;
}
//---------------------------------------------------------------------------
void NiFXLShader::SetBoneCalcMethod(NiFXLShader::BoneMatrixCalcMethod eMethod)
{
    m_eBoneCalcMethod = eMethod;
}
//---------------------------------------------------------------------------
void NiFXLShader::SetActualImplementation(unsigned int uiActualImplementation)
{
    m_uiActualImplementation = uiActualImplementation;
}
//---------------------------------------------------------------------------
unsigned int NiFXLShader::GetActualImplementation() const
{
    return m_uiActualImplementation;
}
//---------------------------------------------------------------------------
