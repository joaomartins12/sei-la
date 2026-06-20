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

#include "NiD3D10ShaderProgram.h"

NiImplementRTTI(NiD3D10ShaderProgram, NiGPUProgram);

//---------------------------------------------------------------------------
NiD3D10ShaderProgram::NiD3D10ShaderProgram(NiGPUProgram::ProgramType eType,
    ID3D10Blob* pkShaderByteCode) :
    NiGPUProgram(eType),
    m_pkReflection(NULL),
    m_pkShaderByteCode(pkShaderByteCode),
    m_pkConstantBufferNames(NULL),
    m_uiConstantBufferCount(0)
{
    if (m_pkShaderByteCode)
        m_pkShaderByteCode->AddRef();
}
//---------------------------------------------------------------------------
NiD3D10ShaderProgram::~NiD3D10ShaderProgram()
{
    ClearShaderReflectionData();
    ClearShaderReflection();

    if (m_pkShaderByteCode)
        m_pkShaderByteCode->Release();
}
//---------------------------------------------------------------------------
const NiFixedString& NiD3D10ShaderProgram::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgram::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
ID3D10Blob* NiD3D10ShaderProgram::GetShaderByteCode() const
{
    return m_pkShaderByteCode;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgram::SetShaderByteCode(ID3D10Blob* pkShaderByteCode)
{
    if (pkShaderByteCode == m_pkShaderByteCode)
        return;

    if (pkShaderByteCode)
        pkShaderByteCode->AddRef();
    if (m_pkShaderByteCode)
        m_pkShaderByteCode->Release();
    m_pkShaderByteCode = pkShaderByteCode;
}
//---------------------------------------------------------------------------
NiD3D10ShaderProgramCreator* NiD3D10ShaderProgram::GetCreator() const
{
    return m_pkCreator;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgram::SetCreator(NiD3D10ShaderProgramCreator* pkCreator)
{
    m_pkCreator = pkCreator;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgram::SetShaderReflection(
    ID3D10ShaderReflection* pkReflection, bool bUpdateShaderConstantInfo)
{
    if (m_pkReflection != pkReflection)
    {
        if (pkReflection)
            pkReflection->AddRef();
        if (m_pkReflection)
            m_pkReflection->Release();
        m_pkReflection = pkReflection;
    }

    if (bUpdateShaderConstantInfo)
        UpdateShaderReflectionData();
}
//---------------------------------------------------------------------------
ID3D10ShaderReflection* NiD3D10ShaderProgram::GetShaderReflection() const
{
    return m_pkReflection;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgram::ClearShaderReflection()
{
    if (m_pkReflection)
        m_pkReflection->Release();
    m_pkReflection = NULL;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10ShaderProgram::GetConstantBufferCount() const
{
    return m_uiConstantBufferCount;
}
//---------------------------------------------------------------------------
const NiFixedString NiD3D10ShaderProgram::GetConstantBufferName(
    unsigned int uiIndex) const
{
    if (uiIndex < m_uiConstantBufferCount)
    {
        return m_pkConstantBufferNames[uiIndex];
    }
    else
    {
        NiFixedString kNull;
        return kNull;
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgram::ClearShaderReflectionData()
{
    if (m_pkConstantBufferNames)
    {
        NIASSERT(m_uiConstantBufferCount != 0);
        NiDelete[] m_pkConstantBufferNames;
        m_pkConstantBufferNames = NULL;
        m_uiConstantBufferCount = 0;
    }
    else
    {
        NIASSERT(m_uiConstantBufferCount == 0);
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderProgram::UpdateShaderReflectionData()
{
    ClearShaderReflectionData();

    if (m_pkReflection == NULL)
        return;

    union 
    {
        D3D10_SHADER_DESC kDesc;
        char acHack[1024];
    } kTempHack;
    HRESULT hr = m_pkReflection->GetDesc(&(kTempHack.kDesc));
    if (FAILED(hr))
    {
        return;
    }
    D3D10_SHADER_DESC kDesc = kTempHack.kDesc;

    m_uiConstantBufferCount = kDesc.ConstantBuffers;
    if (m_uiConstantBufferCount)
    {
        m_pkConstantBufferNames = NiNew NiFixedString[m_uiConstantBufferCount];
        for (unsigned int i = 0; i < m_uiConstantBufferCount; i++)
        {
            ID3D10ShaderReflectionConstantBuffer* pkSCBReflection =
                m_pkReflection->GetConstantBufferByIndex(i);
            if (pkSCBReflection == NULL)
            {
                continue;
            }
            D3D10_SHADER_BUFFER_DESC kBufferDesc;
            hr = pkSCBReflection->GetDesc(&kBufferDesc);
            if (FAILED(hr))
            {
                continue;
            }
            m_pkConstantBufferNames[i] = kBufferDesc.Name;
        }
    }
}
//---------------------------------------------------------------------------
