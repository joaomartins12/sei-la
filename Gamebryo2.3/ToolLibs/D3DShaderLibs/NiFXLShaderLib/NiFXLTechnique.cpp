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

#include "NiFXLTechnique.h"
#include "NiFXLFile.h"
#include "NiFXLTechniqueDesc.h"
#include "NiFXLTechniqueSet.h"

//---------------------------------------------------------------------------
NiFXLTechnique::NiFXLTechnique() :
    m_pcTechniqueName(NULL), 
    m_uiImplementation(0), 
    m_pkTechnique(0),
    m_pkFile(NULL),
    m_kParamIter(NULL),
    m_uiNumPasses(0),
    m_bTimeParameter(false),
    m_bUsesNiRenderState(false),
    m_bUsesNiLightState(false)
{ /* */ }
//---------------------------------------------------------------------------
NiFXLTechnique::~NiFXLTechnique()
{
    NiD3DUtility::Log("Killing NiFXLTechnique %d\n", 
        m_uiImplementation);

    NiFree(m_pcTechniqueName);

    // Don't delete parameters - NiFXLFile owns them
    m_kParameters.RemoveAll();
}
//---------------------------------------------------------------------------
NiFXLShader* NiFXLTechnique::GetD3DShader(
    NiD3DRenderer* pkRenderer)
{
    NIASSERT(pkRenderer);

    FXLEffect* pkFXLEffect = m_pkFile->GetRenderableEffect();

    if (m_spShader == NULL)
    {
        if (m_pkTechnique == NULL)
            return NULL;

        NIASSERT(m_pkReqDesc);
        unsigned int uiBonesPerPartition = 
            m_pkReqDesc->GetBonesPerPartition();
        NiFXLShader::BoneMatrixCalcMethod eMethod = 
            NiFXLShader::BONECALC_SKIN;
        unsigned int uiBoneMatrixRegisters = 4;

        if (uiBonesPerPartition != 0)
        {
            // Skinning shader; search for associated bone array
            NiTListIterator kIter = m_kParameters.GetHeadPos();
            NiFXLParameter* pkParam = NULL;
            NiD3DShaderConstantMap::DefinedMappings eMapping;
            while (kIter)
            {
                pkParam = m_kParameters.GetNext(kIter);
                if (pkParam == NULL)
                    continue;

                eMapping = pkParam->GetPredefinedMapping();

                if (eMapping < NiD3DShaderConstantMap::SCM_DEF_BONE_MATRIX_3 ||
                    eMapping > 
                        NiD3DShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_4)
                {
                    continue;
                }

                // Possible hit; check array size and presence in shader
                if (uiBonesPerPartition > pkParam->GetArrayCount())
                    continue;

                // pkParam is the correct parameter
                break;
            }

            if (pkParam)
            {
                if (eMapping == NiD3DShaderConstantMap::SCM_DEF_BONE_MATRIX_3)
                {
                    eMethod = NiFXLShader::BONECALC_NO_SKIN;
                    uiBoneMatrixRegisters = 3;
                }
                else if (eMapping == 
                    NiD3DShaderConstantMap::SCM_DEF_BONE_MATRIX_4)
                {
                    eMethod = NiFXLShader::BONECALC_NO_SKIN;
                    uiBoneMatrixRegisters = 4;
                }
                else if (eMapping == 
                    NiD3DShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_3)
                {
                    eMethod = NiFXLShader::BONECALC_SKIN;
                    uiBoneMatrixRegisters = 3;
                }
                else if (eMapping == 
                    NiD3DShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_4)
                {
                    eMethod = NiFXLShader::BONECALC_SKIN;
                    uiBoneMatrixRegisters = 4;
                }
            }
        }

        NiD3DRenderState* pkRenderState = pkRenderer->GetRenderState();

        m_spShader = NiNew NiFXLShader;
        m_spShader->SetEffectTechnique(this);
        m_spShader->SetName(GetTechniqueName());
        m_spShader->SetImplementation(m_uiImplementation);
        m_spShader->SetActualImplementation(m_uiImplementation);

        m_spShader->SetUsesNiRenderState(m_bUsesNiRenderState);
        m_spShader->SetUsesNiLightState(m_bUsesNiLightState);

        if (uiBonesPerPartition != 0)
        {
            m_spShader->SetBoneMatrixRegisters(uiBoneMatrixRegisters);
            m_spShader->SetBoneCalcMethod(eMethod);
        }
    }

    return m_spShader;
}
//---------------------------------------------------------------------------
void NiFXLTechnique::Initialize(NiFXLTechniqueDesc* pkTechDesc)
{
    NIASSERT(pkTechDesc);

    NiD3DUtility::SetString(m_pcTechniqueName, 0,
        pkTechDesc->m_pcName);

    m_pkReqDesc = NiNew NiShaderRequirementDesc;
    m_pkReqDesc->SetName(pkTechDesc->m_pcName);
    m_pkReqDesc->SetDescription(pkTechDesc->m_pcDesc);

    m_pkReqDesc->AddPixelShaderVersion(pkTechDesc->m_uiPSVersion);
    m_pkReqDesc->AddVertexShaderVersion(pkTechDesc->m_uiVSVersion);
    m_pkReqDesc->AddUserDefinedVersion(pkTechDesc->m_uiUserVersion);

    m_pkReqDesc->SetPlatformXenon(true);

    m_pkReqDesc->SetBonesPerPartition(pkTechDesc->m_uiBonesPerPartition);
    m_pkReqDesc->SetBinormalTangentMethod(pkTechDesc->m_eNBTMethod);

    m_uiImplementation = pkTechDesc->m_uiImplementation;

    m_bUsesNiRenderState = pkTechDesc->m_bUsesNiRenderState;
    m_bUsesNiLightState  = pkTechDesc->m_bUsesNiLightState;
}
//---------------------------------------------------------------------------
void NiFXLTechnique::LinkHandle(FXLEffect* pkEffect)
{
    FXLTechniquePtr pkTechnique = 
        pkEffect->GetTechniqueHandle(m_pkReqDesc->GetName());
    if (pkTechnique == m_pkTechnique)
        return;

    m_pkTechnique = pkTechnique;
    NIASSERT(m_pkTechnique);

    // Fill in the array of vertex shader flags
    FXLTECHNIQUE_DESC kTechDesc;
    pkEffect->GetTechniqueDesc(m_pkTechnique, &kTechDesc);
    
    if (m_uiNumPasses < kTechDesc.Passes)
    {
        m_uiNumPasses = kTechDesc.Passes;
    }
}
//---------------------------------------------------------------------------
