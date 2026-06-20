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

#include "NiShaderLibrary.h"
#include "NiShader.h"

//---------------------------------------------------------------------------
NiShaderLibrary::NiShaderLibrary(char* pcName) :
    m_pcName(0)
{
    SetName(pcName);
}
//---------------------------------------------------------------------------
NiShaderLibrary::~NiShaderLibrary()
{
    NiFree(m_pcName);
    m_pcName = 0;
}
//---------------------------------------------------------------------------
const char* NiShaderLibrary::GetName()
{
    return m_pcName;
}
//---------------------------------------------------------------------------
void NiShaderLibrary::SetName(char* pcName)
{
    NiFree(m_pcName);
    m_pcName = 0;

    if (pcName && pcName[0] != '\0')
    {
        unsigned int uiLen = strlen(pcName) + 1;
        m_pcName = NiAlloc(char,uiLen);
        NIASSERT(m_pcName);
        NiStrcpy(m_pcName, uiLen, pcName);
    }
}
//---------------------------------------------------------------------------
NiShader* NiShaderLibrary::GetShader(NiRenderer* pkRenderer, 
    const char* pcName, unsigned int uiImplementation)
{
    // Default implementation returns 0
    return 0;
}
//---------------------------------------------------------------------------
bool NiShaderLibrary::ReleaseShader(const char* pcName, 
    unsigned int uiImplementation)
{
    // Default implementation does nothing
    return false;
}
//---------------------------------------------------------------------------
bool NiShaderLibrary::ReleaseShader(NiShader* pkShader)
{
    // Default implementation does nothing
    return false;
}
//---------------------------------------------------------------------------
