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
inline bool NiFXLTechnique::ReleaseShader()
{
    bool bRet = (m_spShader != NULL);
    m_spShader = NULL;
    return bRet;
}
//---------------------------------------------------------------------------
inline void NiFXLTechnique::SetTechniquePtr(FXLTechniquePtr pkTechnique)
{
    m_pkTechnique = pkTechnique;
    if (m_spShader)
        m_spShader->SetTechniquePtr(pkTechnique);
}
//---------------------------------------------------------------------------
inline void NiFXLTechnique::DestroyRendererData()
{
    m_pkTechnique = NULL;
    if (m_spShader)
        m_spShader->DestroyRendererData();
}
//---------------------------------------------------------------------------
inline void NiFXLTechnique::AddParameter(NiFXLParameter* pkParam)
{
    m_kParameters.AddTail(pkParam);
    if (pkParam->RequiresTime())
        m_bTimeParameter = true;
}
//---------------------------------------------------------------------------
inline NiFXLParameter* NiFXLTechnique::GetFirstParameter()
{
    m_kParamIter = m_kParameters.GetHeadPos();
    return GetNextParameter();
}
//---------------------------------------------------------------------------
inline NiFXLParameter* NiFXLTechnique::GetNextParameter()
{
    if (m_kParamIter == NULL)
        return NULL;
    return m_kParameters.GetNext(m_kParamIter);
}
//---------------------------------------------------------------------------
inline NiFXLParameter* NiFXLTechnique::GetParameter(
    const char* pcParamName)
{
    NiTListIterator kIter = m_kParameters.GetHeadPos();
    while (kIter)
    {
        NiFXLParameter* pkParam = m_kParameters.GetNext(kIter);
        if (pkParam && strcmp(pkParam->GetName(), pcParamName) == 0)
            return pkParam;
    }
    return NULL;
}
//---------------------------------------------------------------------------
inline NiShaderRequirementDesc* NiFXLTechnique::GetRequirements()
{
    return m_pkReqDesc;
}
//---------------------------------------------------------------------------
inline const char* NiFXLTechnique::GetName() const
{
    return m_pkReqDesc->GetName();
}
//---------------------------------------------------------------------------
inline const char* NiFXLTechnique::GetDescription() const
{
    return m_pkReqDesc->GetDescription();
}
//---------------------------------------------------------------------------
inline const char* NiFXLTechnique::GetTechniqueName() const
{
    if (m_pcTechniqueName)
        return m_pcTechniqueName;
    else
        return m_pkReqDesc->GetName();
}
//---------------------------------------------------------------------------
inline unsigned int NiFXLTechnique::GetImplementation() const
{
    return m_uiImplementation;
}
//---------------------------------------------------------------------------
inline FXLTechniquePtr NiFXLTechnique::GetTechniquePtr() const
{
    return m_pkTechnique;
}
//---------------------------------------------------------------------------
inline NiFXLFile* NiFXLTechnique::GetFile() const
{
    return m_pkFile;
}
//---------------------------------------------------------------------------
inline void NiFXLTechnique::SetFile(NiFXLFile* pkFile)
{
    m_pkFile = pkFile;
}
//---------------------------------------------------------------------------
inline bool NiFXLTechnique::GetTimeParameterPresent() const
{
    return m_bTimeParameter;
}
//---------------------------------------------------------------------------
inline unsigned int NiFXLTechnique::GetNumPasses() const
{
    return m_uiNumPasses;
}
//---------------------------------------------------------------------------
