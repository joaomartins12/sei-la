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

#include "NiFXLParser.h"

//---------------------------------------------------------------------------
NiFXLFile::NiFXLFile() : m_pcName(NULL),
    m_uiDXVersion(0),
    m_kTechniqueIter(NULL),
    m_pkCurrTechniqueSet(NULL),
    m_uiIndex(0),
    m_pkFXLEffect(NULL),
    m_bRenderableEffect(false),
    m_bParsed(false)
{
}
//---------------------------------------------------------------------------
NiFXLFile::~NiFXLFile()
{
    NiD3DUtility::Log("Killing NiFXLFile %s\n", m_pcName);

    // NiD3DXEffectFactory won't exist if it has already been shut down.
    // In this case, there's no need to remove the effect from the map.
    NiFXLFactory* pkEffectFactory = NiFXLFactory::GetInstance(false);
    if (pkEffectFactory)
        pkEffectFactory->RemoveFXLFileFromMap(m_pcName);

    DestroyRendererData();

    NiFree(m_pcName);

    NiTStringMap<NiFXLTechniqueSetPtr> m_kTechniques;

    m_kTechniqueIter = m_kTechniques.GetFirstPos();
    while (m_kTechniqueIter)
    {
        const char* pcName;
        NiFXLTechniqueSetPtr spTechniqueSet;
        m_kTechniques.GetNext(m_kTechniqueIter, pcName, spTechniqueSet);
        m_kTechniques.SetAt(pcName, NULL);
    }
    m_kTechniques.RemoveAll();

    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiFXLParameter* pkParam = m_kParameters.GetNext(kParamIter);
        NiDelete pkParam;
    }
    m_kParameters.RemoveAll();

    if (m_pkFXLEffect)
        m_pkFXLEffect->Release();
}
//---------------------------------------------------------------------------
bool NiFXLFile::Initialize(FXLEffect* pkFXL)
{
    m_pkFXLEffect = pkFXL;

    if (!ParseEffect())
        return false;

    m_bRenderableEffect = true;
    
    return LinkHandles();
}
//---------------------------------------------------------------------------
bool NiFXLFile::ParseEffect()
{
    if (m_pkFXLEffect == NULL || m_bParsed)
        return true;

    unsigned int uiNumParameters = 0;
    unsigned int uiNumTechniques = 0;
    bool bSuccess = NiFXLParser::GetParametersAndTechniques(
        m_pkFXLEffect, uiNumParameters, uiNumTechniques);

    if (!bSuccess)
        return false;

    NiFXLParser::ResetIndices();
    unsigned int i = 0;
    for (; i < uiNumParameters; i++)
    {
        NiFXLParameterDesc* pkParamDesc = 
            NiFXLParser::GetParameter(m_pkFXLEffect, i);

        if (pkParamDesc)
        {
            NiFXLParameter* pkParam = NiNew NiFXLParameter;
            if (pkParam->Initialize(pkParamDesc, &m_kAttribTable))
                m_kParameters.AddTail(pkParam);
        }
    }
    
    for (i = 0; i < uiNumTechniques; i++)
    {
        NiFXLTechniqueDesc* pkTechDesc = 
            NiFXLParser::GetTechnique(m_pkFXLEffect, i);

        if (pkTechDesc)
        {
            NiFXLTechnique* pkTechnique = NiNew NiFXLTechnique;
            pkTechnique->Initialize(pkTechDesc);
            pkTechnique->SetFile(this);

            AddTechnique(pkTechnique);
        }
    }

    m_bParsed = true;
    return true;
}
//---------------------------------------------------------------------------
void NiFXLFile::DestroyRendererData()
{
    if (m_pkFXLEffect)
    {
        m_pkFXLEffect->Release();
        m_pkFXLEffect = NULL;
    }
}
//---------------------------------------------------------------------------
NiFXLShader* NiFXLFile::GetD3DShader(NiD3DRenderer* pkRenderer, 
    const char* pcName, unsigned int uiImplementation)
{
    // Before a shader can be returned, it needs to have the effect created
    NIASSERT(m_pkFXLEffect);

    NiFXLTechniqueSetPtr spTechniqueSet;
    m_kTechniques.GetAt(pcName, spTechniqueSet);

    if (spTechniqueSet)
        return spTechniqueSet->GetD3DShader(pkRenderer, uiImplementation);

    return NULL;
}
//---------------------------------------------------------------------------
bool NiFXLFile::ReleaseShader(const char* pcName, 
    unsigned int uiImplementation)
{
    NiFXLTechniqueSetPtr spTechniqueSet;
    m_kTechniques.GetAt(pcName, spTechniqueSet);

    if (spTechniqueSet == NULL)
        return false;

    NiFXLTechnique* pkTechnique = 
        spTechniqueSet->GetTechnique(uiImplementation);
    if (pkTechnique == NULL)
        return false;

    return pkTechnique->ReleaseShader();
}
//---------------------------------------------------------------------------
const char* NiFXLFile::GetName() const
{
    return m_pcName;
}
//---------------------------------------------------------------------------
void NiFXLFile::SetName(const char* pcName)
{
    NiD3DUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
void NiFXLFile::SetDXVersion(unsigned int uiDXVersion)
{
    m_uiDXVersion = uiDXVersion;
}
//---------------------------------------------------------------------------
NiFXLTechnique* NiFXLFile::GetFirstTechnique()
{
    m_kTechniqueIter = m_kTechniques.GetFirstPos();
    if (m_kTechniqueIter == NULL)
        return NULL;
    m_pkCurrTechniqueSet = NULL;
    m_uiIndex = 0;
    return GetNextTechnique();
}
//---------------------------------------------------------------------------
NiFXLTechnique* NiFXLFile::GetNextTechnique()
{
    while (m_kTechniqueIter)
    {
        if (m_pkCurrTechniqueSet == NULL)
        {
            const char* pcName = NULL;
            NiFXLTechniqueSetPtr spTechniqueSet;
            m_kTechniques.GetNext(m_kTechniqueIter, pcName, spTechniqueSet);
            m_pkCurrTechniqueSet = spTechniqueSet;
        }
        NIASSERT(m_pkCurrTechniqueSet);
        while (m_uiIndex < m_pkCurrTechniqueSet->GetTechniqueArrayCount())
        {
            NiFXLTechnique* pkTech = 
                m_pkCurrTechniqueSet->GetTechnique(m_uiIndex++);
            if (pkTech)
                return pkTech;
        }
        m_pkCurrTechniqueSet = NULL;
        m_uiIndex = 0;
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiFXLTechnique* NiFXLFile::GetTechnique(const char* pcName, 
    unsigned int uiImplementation) const
{
    NiFXLTechniqueSetPtr spTechniqueSet;
    m_kTechniques.GetAt(pcName, spTechniqueSet);
    if (spTechniqueSet != NULL)
    {
        NiFXLTechnique* pkTechnique = spTechniqueSet->GetTechnique(
            uiImplementation);
        return pkTechnique;
    }
    return NULL;
}
//---------------------------------------------------------------------------
void NiFXLFile::AddTechnique(NiFXLTechnique* pkTechnique)
{
    // Add appropriate parameters
    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiFXLParameter* pkParam = m_kParameters.GetNext(kParamIter);
        if (ParamIsUsed(pkParam, pkTechnique))
            pkTechnique->AddParameter(pkParam);
    }

    const char* pcName = pkTechnique->GetTechniqueName();
    NiFXLTechniqueSetPtr spTechniqueSet;
    if (!m_kTechniques.GetAt(pcName, spTechniqueSet) || 
        spTechniqueSet == NULL)
    {
        spTechniqueSet = NiNew NiFXLTechniqueSet;
        spTechniqueSet->SetName(pcName);
        m_kTechniques.SetAt(pcName, spTechniqueSet);
    }

    spTechniqueSet->AddImplementation(pkTechnique);
}
//---------------------------------------------------------------------------
FXLEffect* NiFXLFile::GetRenderableEffect() const
{
    if (m_bRenderableEffect)
        return m_pkFXLEffect;
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiShaderDesc* NiFXLFile::GetFirstShaderDesc()
{
    m_kTechniqueIter = m_kTechniques.GetFirstPos();
    return GetNextShaderDesc();
}
//---------------------------------------------------------------------------
NiShaderDesc* NiFXLFile::GetNextShaderDesc()
{
    if (m_kTechniqueIter)
    {
        const char* pcName = NULL;
        NiFXLTechniqueSetPtr spTechniqueSet;
        m_kTechniques.GetNext(m_kTechniqueIter, pcName, spTechniqueSet);
        if (spTechniqueSet != NULL)
            return spTechniqueSet->GetShaderDesc(&m_kAttribTable);
    }
    return NULL;
}
//---------------------------------------------------------------------------
bool NiFXLFile::LinkHandles()
{
    if (m_pkFXLEffect == NULL || m_bRenderableEffect == false)
        return false;

    // Link parameters
    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiFXLParameter* pkParam = m_kParameters.GetNext(kParamIter);
        pkParam->LinkHandle(m_pkFXLEffect);
    }

    // Link each technique to a NiD3DXEffectTechnique
    NiFXLTechniqueSetPtr spSet;
    NiTMapIterator kTechIter = m_kTechniques.GetFirstPos();
    while (kTechIter)
    {
        const char* pcName = NULL;
        m_kTechniques.GetNext(kTechIter, pcName, spSet);
        if (spSet == NULL)
            continue;

        unsigned int uiNumTechniques = spSet->GetTechniqueArrayCount();
        for (unsigned int i = 0; i < uiNumTechniques; i++)
        {
            // Ensure technique description is updated now that 
            // the device exists
            NiFXLTechniqueDesc* pkTechDesc = 
                NiFXLParser::GetTechnique(m_pkFXLEffect, i);

            NiFXLTechnique* pkTech = spSet->GetTechnique(i);
            if (pkTech == NULL)
                continue;

            pkTech->LinkHandle(m_pkFXLEffect);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiFXLFile::ParamIsUsed(NiFXLParameter* pkParam, 
    NiFXLTechnique* pkTechnique)
{
    // Go through the FXL object to get handles because we haven't linked the
    // handles in our system yet.
    FXLHANDLE hParam = m_pkFXLEffect->GetParameterHandle(pkParam->GetName());
    FXLHANDLE hTech = m_pkFXLEffect->GetTechniqueHandle(
        pkTechnique->GetName());
    
    FXLTECHNIQUE_DESC kTechDesc;
    m_pkFXLEffect->GetTechniqueDesc(hTech, &kTechDesc);
    
    unsigned int uiNumPasses = kTechDesc.Passes;

    // If it is an array, get the first element of the array. You can not
    // get the parameter context for an array or struct.
    FXLPARAMETER_DESC kDesc;
    m_pkFXLEffect->GetParameterDesc(hParam, &kDesc);
    if (kDesc.Content != FXLPACONTENT_DATA)
    {
        // We don't handle structs at this time although it's likely that
        // everything here would work fine if this were a struct.
        NIASSERT(kDesc.Content == FXLPACONTENT_ARRAY);
        hParam = m_pkFXLEffect->GetElementHandle(hParam, 0);
    }

    for (unsigned int ui = 0; ui < uiNumPasses; ++ui)
    {
        FXLPARAMETER_CONTEXT eContext = m_pkFXLEffect->GetParameterContext(
            m_pkFXLEffect->GetPassHandleFromIndex(hTech, ui), hParam);
        if (eContext != 0)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
FXLEffect* NiFXLFile::GetEffect() const
{
    return m_pkFXLEffect;
}
//---------------------------------------------------------------------------