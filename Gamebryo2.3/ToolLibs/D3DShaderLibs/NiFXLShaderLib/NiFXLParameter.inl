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
inline void NiFXLParameter::LinkHandle(FXLEffect* pkEffect)
{
    m_pkParam = pkEffect->GetParameterHandle(m_kName);
}
//---------------------------------------------------------------------------
inline void NiFXLParameter::DestroyRendererData()
{
    if (m_pkTexture)
    {
        m_pkTexture->Release();
        m_pkTexture = NULL;
    }
}
//---------------------------------------------------------------------------
inline void NiFXLParameter::ReleaseBoneArray()
{
    NiDelete[] ms_pkMatrixArray;
    ms_pkMatrixArray = NULL;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiFXLParameter::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
inline void NiFXLParameter::SetParameterPtr(FXLParameterPtr pkParam)
{
    m_pkParam = pkParam;
}
//---------------------------------------------------------------------------
inline FXLParameterPtr NiFXLParameter::GetParameterPtr() const
{
    return m_pkParam;
}
//---------------------------------------------------------------------------
inline bool NiFXLParameter::RequiresTime() const
{
    return (m_ePredefinedMapping == 
            NiD3DShaderConstantMap::SCM_DEF_CONSTS_TIME ||
        m_ePredefinedMapping == 
            NiD3DShaderConstantMap::SCM_DEF_CONSTS_SINTIME ||
        m_ePredefinedMapping == 
            NiD3DShaderConstantMap::SCM_DEF_CONSTS_COSTIME ||
        m_ePredefinedMapping == 
            NiD3DShaderConstantMap::SCM_DEF_CONSTS_TANTIME ||
        m_ePredefinedMapping == 
            NiD3DShaderConstantMap::
                SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME);
}
//---------------------------------------------------------------------------
inline NiD3DShaderConstantMap::DefinedMappings 
    NiFXLParameter::GetPredefinedMapping() const
{
    return m_ePredefinedMapping;
}
//---------------------------------------------------------------------------
inline unsigned int NiFXLParameter::GetArrayCount() const
{
    return m_uiArrayCount;
}
//---------------------------------------------------------------------------
