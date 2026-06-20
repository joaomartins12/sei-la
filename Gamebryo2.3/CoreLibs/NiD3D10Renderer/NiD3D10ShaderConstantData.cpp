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

#include "NiD3D10ShaderConstantData.h"

//---------------------------------------------------------------------------
NiD3D10ShaderConstantData::NiD3D10ShaderConstantData(size_t uiBufferSize, 
    unsigned int uiAccessHints) :
    m_uiBufferIndex(BUFFER_INDEX_UNASSIGNED)
{
    bool bResult = NiD3D10DataStream::Create(uiBufferSize, uiAccessHints, 
        NiD3D10DataStream::USAGE_SHADER_CONSTANT, m_spDataStream);

    if (bResult == false)
        m_spDataStream = NULL;
}
//---------------------------------------------------------------------------
NiD3D10ShaderConstantData::~NiD3D10ShaderConstantData()
{
    /* */
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderConstantData::Create(size_t uiBufferSize, 
    unsigned int uiAccessHints, 
    NiD3D10ShaderConstantDataPtr& spShaderConstantData)
{
    spShaderConstantData = NiNew NiD3D10ShaderConstantData(uiBufferSize, 
        uiAccessHints);
    if (spShaderConstantData == NULL || 
        spShaderConstantData->m_spDataStream == NULL)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiD3D10DataStream* NiD3D10ShaderConstantData::GetDataStream() const
{
    return m_spDataStream;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantData::SetDataStream(NiD3D10DataStream* pkDataStream)
{
    m_spDataStream = pkDataStream;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10ShaderConstantData::GetBufferIndex() const
{
    return m_uiBufferIndex;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantData::SetBufferIndex(unsigned int uiBufferIndex)
{
    m_uiBufferIndex = uiBufferIndex;
}
//---------------------------------------------------------------------------
