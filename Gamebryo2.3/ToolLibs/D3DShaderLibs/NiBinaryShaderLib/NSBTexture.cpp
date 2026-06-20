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
#include "NiBinaryShaderLibPCH.h"

#include "NSBTexture.h"
#include "NSBUtility.h"

#include <NiD3DTextureStage.h>

//---------------------------------------------------------------------------
NSBTexture::NSBTexture() :
    m_pcName(NULL),
    m_uiStage(0),
    m_uiTextureFlags(0),
    m_usObjTextureFlags(0)
{
    /* */
}
//---------------------------------------------------------------------------
NSBTexture::~NSBTexture()
{
    NiFree(m_pcName);
}
//---------------------------------------------------------------------------
void NSBTexture::SetName(const char* pcName)
{
    NiD3DUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
bool NSBTexture::SetupD3DTextureStage(NiD3DTextureStage& kD3DStage)
{

#if defined(WIN32)
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    return SetupD3DTextureStage_DX9(kD3DStage);
#elif defined(_XENON)
    return SetupD3DTextureStage_Xenon(kD3DStage);
#endif 

    return false;
}
//---------------------------------------------------------------------------
bool NSBTexture::SaveBinary(NiBinaryStream& kStream)
{
    if (!NiD3DUtility::WriteString(kStream, m_pcName))
        return false;
    NiStreamSaveBinary(kStream, m_uiStage);
    NiStreamSaveBinary(kStream, m_uiTextureFlags);
    NiStreamSaveBinary(kStream, m_usObjTextureFlags);

    return true;
}
//---------------------------------------------------------------------------
bool NSBTexture::LoadBinary(NiBinaryStream& kStream)
{
    unsigned int uiStringSize = 0;
    if (!NiD3DUtility::AllocateAndReadString(kStream, m_pcName, uiStringSize))
        return false;
    NiStreamLoadBinary(kStream, m_uiStage);
    NiStreamLoadBinary(kStream, m_uiTextureFlags);
    NiStreamLoadBinary(kStream, m_usObjTextureFlags);

    return true;
}
//---------------------------------------------------------------------------
#if defined(_DEBUG)
//---------------------------------------------------------------------------
void NSBTexture::Dump(FILE* pf)
{
    NSBUtility::Dump(pf, true, "Texture %s\n", m_pcName);
    NSBUtility::IndentInsert();

    NSBUtility::Dump(pf, true, "Stage         = %d\n", m_uiStage);
    NSBUtility::Dump(pf, true, "Texture Flags = 0x%08x\n", m_uiTextureFlags);
    NSBUtility::Dump(pf, true, "Object Flags = 0x%04x\n", m_usObjTextureFlags);

    NSBUtility::IndentRemove();
}
//---------------------------------------------------------------------------
#endif  //#if defined(_DEBUG)
//---------------------------------------------------------------------------
