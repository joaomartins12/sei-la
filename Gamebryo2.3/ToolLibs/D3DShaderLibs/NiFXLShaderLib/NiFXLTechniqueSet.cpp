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

#include "NiFXLTechniqueSet.h"
#include "NiFXLAttributeTable.h"
#include "NiFXLTechnique.h"

#include <NiShaderDesc.h>
#include <NiD3DUtility.h>

//---------------------------------------------------------------------------
NiFXLTechniqueSet::NiFXLTechniqueSet() :
    m_pcName(NULL),
    m_pkBestImplementation(NULL)
{ /* */ }
//---------------------------------------------------------------------------
NiFXLTechniqueSet::~NiFXLTechniqueSet()
{
    NiD3DUtility::Log("Killing NiFXLTechniqueSet %s\n", m_pcName);

    m_kImplementations.RemoveAll();

    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
NiShaderDesc* NiFXLTechniqueSet::GetShaderDesc(
    NiFXLAttributeTable* pkAttribTable)
{
    if (m_spShaderDesc == NULL)
    {
        m_spShaderDesc = NiNew NiShaderDesc();
        NIASSERT(m_spShaderDesc);

        m_spShaderDesc->SetName(m_pcName);
        NiFXLTechnique* pkTech = m_kImplementations.GetAt(0);
        if (pkTech)
            m_spShaderDesc->SetDescription(pkTech->GetDescription());

        m_spShaderDesc->SetNumberOfImplementations(
            m_kImplementations.GetEffectiveSize());

        // Add attributes
        NiShaderAttributeDesc* pkDesc = pkAttribTable->GetFirstAttribute();
        while (pkDesc)
        {
            NiShaderAttributeDesc* pkNewDesc = 
                NiNew NiShaderAttributeDesc;
            NIASSERT(pkNewDesc);
            
            *pkNewDesc = *pkDesc;

            m_spShaderDesc->AddAttribute(pkNewDesc);

            pkDesc = pkAttribTable->GetNextAttribute();
        }

        for (unsigned int i = 0; i < m_kImplementations.GetSize(); i++)
        {
            pkTech = m_kImplementations.GetAt(i);
            if (pkTech)
            {
                // Add requirements
                NiShaderRequirementDesc* pkReq = pkTech->GetRequirements();
                NIASSERT(pkReq);

                m_spShaderDesc->AddPixelShaderVersion(
                    pkReq->GetMinPixelShaderVersion());
                m_spShaderDesc->AddPixelShaderVersion(
                    pkReq->GetMaxPixelShaderVersion());
                m_spShaderDesc->AddVertexShaderVersion(
                    pkReq->GetMinVertexShaderVersion());
                m_spShaderDesc->AddVertexShaderVersion(
                    pkReq->GetMaxVertexShaderVersion());
                m_spShaderDesc->AddUserDefinedVersion(
                    pkReq->GetMinUserDefinedVersion());
                m_spShaderDesc->AddUserDefinedVersion(
                    pkReq->GetMaxUserDefinedVersion());
                m_spShaderDesc->AddPlatformFlags(pkReq->GetPlatformFlags());

                m_spShaderDesc->SetBonesPerPartition(
                    pkReq->GetBonesPerPartition());
                m_spShaderDesc->SetBinormalTangentMethod(
                    pkReq->GetBinormalTangentMethod());

                m_spShaderDesc->SetSoftwareVPAcceptable(
                    pkReq->GetSoftwareVPAcceptable());
                m_spShaderDesc->SetSoftwareVPRequired(
                    pkReq->GetSoftwareVPRequired());

                m_spShaderDesc->SetImplementationDescription(i, pkReq);
            }
        }
    }
    return m_spShaderDesc;
}
//---------------------------------------------------------------------------
NiFXLShader* NiFXLTechniqueSet::GetD3DShader(
    NiD3DRenderer* pkRenderer, unsigned int uiImplementation)
{
    NIASSERT(pkRenderer);
    NiFXLTechnique* pkTechnique = NULL;
    if (uiImplementation < m_kImplementations.GetSize())
        pkTechnique = m_kImplementations.GetAt(uiImplementation);

    NiFXLShader* pkShader = NULL;
    if (pkTechnique)
        pkShader = pkTechnique->GetD3DShader(pkRenderer);
    else
        pkShader = GetBestD3DShader(pkRenderer);

    return pkShader;
}
//---------------------------------------------------------------------------
NiFXLShader* NiFXLTechniqueSet::GetBestD3DShader(
    NiD3DRenderer* pkRenderer)
{
    if (m_pkBestImplementation)
        return m_pkBestImplementation->GetD3DShader(pkRenderer);

    for (unsigned int i = 0; i < m_kImplementations.GetSize(); i++)
    {
        NiFXLTechnique* pkTechnique = m_kImplementations.GetAt(i);
        if (pkTechnique)
        {
            NiFXLShader* pkShader = 
                pkTechnique->GetD3DShader(pkRenderer);
            if (pkShader)
            {
                m_pkBestImplementation = pkTechnique;
                pkShader->SetImplementation(NiShader::DEFAULT_IMPLEMENTATION);
                return pkShader;
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiFXLTechniqueSet::DestroyRendererData()
{
    for (unsigned int i = 0; i < m_kImplementations.GetSize(); i++)
    {
        NiFXLTechnique* pkTech = m_kImplementations.GetAt(i);
        if (pkTech)
            pkTech->DestroyRendererData();
    }
}
//---------------------------------------------------------------------------
void NiFXLTechniqueSet::AddImplementation(
    NiFXLTechnique* pkTechnique)
{
    NIASSERT(pkTechnique);
    m_kImplementations.SetAtGrow(pkTechnique->GetImplementation(), 
        pkTechnique);
}
//---------------------------------------------------------------------------
void NiFXLTechniqueSet::SetName(const char* pcName)
{
    NiD3DUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
const char* NiFXLTechniqueSet::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
unsigned int NiFXLTechniqueSet::GetImplementationCount() const
{
    return m_kImplementations.GetEffectiveSize();
}

//---------------------------------------------------------------------------
NiFXLTechnique* NiFXLTechniqueSet::GetTechnique(
    unsigned int uiImplementation)
{
    if (uiImplementation < m_kImplementations.GetSize())
        return m_kImplementations.GetAt(uiImplementation);
    return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiFXLTechniqueSet::GetTechniqueArrayCount() const
{
    return m_kImplementations.GetSize();
}
//---------------------------------------------------------------------------
